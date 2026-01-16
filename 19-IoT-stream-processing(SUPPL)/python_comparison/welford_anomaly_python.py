#!/usr/bin/env python3
"""
=============================================================================
WEEK 19: IoT and Stream Processing - Python Comparison
Welford's Algorithm and Anomaly Detection
=============================================================================

This file demonstrates Python equivalents of the C implementations.
NOT for execution - for comparison and understanding only.

Author: ATP Course - ASE-CSIE Bucharest
=============================================================================
"""

from dataclasses import dataclass
from enum import Enum
from typing import Optional, List, Tuple
import math

# =============================================================================
# PART 1: WELFORD'S ONLINE ALGORITHM
# =============================================================================

class WelfordState:
    """
    Welford's online algorithm for computing running mean and variance.
    
    This algorithm is numerically stable and computes statistics in a
    single pass, making it ideal for streaming data where we cannot
    revisit previous values.
    
    The naive formula: var = E[X²] - E[X]² can suffer from catastrophic
    cancellation when E[X²] and E[X]² are large and similar.
    
    Welford's algorithm avoids this by computing:
        M2 = sum((x_i - mean)²)
    
    incrementally using the identity:
        M2_new = M2_old + (x - mean_old) * (x - mean_new)
    
    Time Complexity: O(1) per update
    Space Complexity: O(1)
    
    Reference: Welford, B.P. (1962). "Note on a method for calculating
               corrected sums of squares and products"
    """
    
    def __init__(self):
        """Initialise Welford state."""
        self.count: int = 0
        self.mean: float = 0.0
        self.M2: float = 0.0  # Sum of squared differences from mean
    
    def update(self, value: float) -> None:
        """
        Update running statistics with new value.
        
        Algorithm:
            1. Increment count
            2. Compute delta = value - mean (using OLD mean)
            3. Update mean incrementally
            4. Compute delta2 = value - mean (using NEW mean)
            5. Update M2 += delta * delta2
        
        Args:
            value: New observation
        """
        self.count += 1
        
        # CRITICAL: delta uses OLD mean, delta2 uses NEW mean
        delta = value - self.mean
        self.mean += delta / self.count
        delta2 = value - self.mean  # Note: uses updated mean!
        self.M2 += delta * delta2
    
    @property
    def variance(self) -> float:
        """
        Sample variance (using Bessel's correction).
        
        Returns:
            Sample variance, or 0.0 if count < 2
        """
        if self.count < 2:
            return 0.0
        return self.M2 / (self.count - 1)
    
    @property
    def population_variance(self) -> float:
        """
        Population variance (no Bessel's correction).
        
        Returns:
            Population variance, or 0.0 if count < 1
        """
        if self.count < 1:
            return 0.0
        return self.M2 / self.count
    
    @property
    def stddev(self) -> float:
        """Sample standard deviation."""
        return math.sqrt(self.variance)
    
    def reset(self) -> None:
        """Reset all statistics."""
        self.count = 0
        self.mean = 0.0
        self.M2 = 0.0
    
    def __repr__(self) -> str:
        return (f"WelfordState(count={self.count}, mean={self.mean:.4f}, "
                f"var={self.variance:.4f}, std={self.stddev:.4f})")


def welford_combine(states: List[WelfordState]) -> WelfordState:
    """
    Combine multiple Welford states (parallel/distributed computation).
    
    This is useful when computing statistics across multiple streams
    or in a map-reduce pattern.
    
    Args:
        states: List of WelfordState objects to combine
        
    Returns:
        Combined WelfordState
    """
    if not states:
        return WelfordState()
    
    combined = WelfordState()
    
    for state in states:
        if state.count == 0:
            continue
            
        if combined.count == 0:
            combined.count = state.count
            combined.mean = state.mean
            combined.M2 = state.M2
        else:
            # Chan's parallel algorithm
            delta = state.mean - combined.mean
            combined_count = combined.count + state.count
            
            combined.mean = (combined.count * combined.mean + 
                           state.count * state.mean) / combined_count
            
            combined.M2 += state.M2 + delta * delta * (
                combined.count * state.count / combined_count)
            
            combined.count = combined_count
    
    return combined


# =============================================================================
# PART 2: ANOMALY DETECTION
# =============================================================================

class AnomalySeverity(Enum):
    """Severity levels for detected anomalies."""
    NORMAL = 0
    WARNING = 1
    CRITICAL = 2


@dataclass
class AnomalyResult:
    """Result of anomaly detection."""
    is_anomaly: bool = False
    z_score: float = 0.0
    severity: AnomalySeverity = AnomalySeverity.NORMAL
    value: float = 0.0
    threshold_used: float = 0.0


def detect_anomaly(value: float, mean: float, stddev: float,
                   threshold: float = 2.5) -> AnomalyResult:
    """
    Detect if a value is anomalous using z-score.
    
    The z-score measures how many standard deviations a value is
    from the mean:
        z = (x - μ) / σ
    
    Common thresholds:
        |z| > 2.0: ~5% of normal distribution (unusual)
        |z| > 2.5: ~1% of normal distribution (suspicious)
        |z| > 3.0: ~0.3% of normal distribution (anomaly)
    
    Args:
        value: Value to check
        mean: Population mean
        stddev: Population standard deviation
        threshold: Z-score threshold for anomaly detection
        
    Returns:
        AnomalyResult with detection details
    """
    result = AnomalyResult(value=value, threshold_used=threshold)
    
    # Guard against division by zero
    if stddev < 1e-10:
        return result
    
    result.z_score = (value - mean) / stddev
    abs_z = abs(result.z_score)
    
    if abs_z > threshold:
        result.is_anomaly = True
        # Determine severity based on how far beyond threshold
        if abs_z > threshold + 1.0:
            result.severity = AnomalySeverity.CRITICAL
        else:
            result.severity = AnomalySeverity.WARNING
    
    return result


class AnomalyDetector:
    """
    Online anomaly detector with baseline establishment.
    
    The detector first collects a baseline of normal readings to
    establish mean and variance, then uses z-score detection for
    subsequent readings.
    """
    
    def __init__(self, baseline_count: int = 10, threshold: float = 2.5):
        """
        Initialise anomaly detector.
        
        Args:
            baseline_count: Number of readings to establish baseline
            threshold: Z-score threshold for anomaly detection
        """
        self.stats = WelfordState()
        self.baseline_count = baseline_count
        self.threshold = threshold
        self.baseline_ready = False
    
    def process(self, value: float) -> AnomalyResult:
        """
        Process a reading and detect anomalies.
        
        During baseline phase, readings are accumulated to establish
        normal statistics. After baseline is ready, anomaly detection
        is performed.
        
        Args:
            value: Sensor reading
            
        Returns:
            AnomalyResult (always normal during baseline phase)
        """
        result = AnomalyResult(value=value, threshold_used=self.threshold)
        
        if not self.baseline_ready:
            # Still building baseline
            self.stats.update(value)
            
            if self.stats.count >= self.baseline_count:
                self.baseline_ready = True
            
            return result  # Normal during baseline
        
        # Baseline ready - check for anomaly
        return detect_anomaly(value, self.stats.mean, 
                             self.stats.stddev, self.threshold)
    
    def process_and_update(self, value: float) -> AnomalyResult:
        """
        Process reading and update statistics (for adaptive baseline).
        
        This variant updates statistics even after baseline is established,
        allowing the detector to adapt to gradual drift.
        
        Args:
            value: Sensor reading
            
        Returns:
            AnomalyResult
        """
        result = self.process(value)
        
        # Optionally update stats with non-anomalous readings
        if not result.is_anomaly:
            self.stats.update(value)
        
        return result
    
    @property
    def mean(self) -> float:
        """Current mean."""
        return self.stats.mean
    
    @property
    def stddev(self) -> float:
        """Current standard deviation."""
        return self.stats.stddev
    
    def reset(self) -> None:
        """Reset detector state."""
        self.stats.reset()
        self.baseline_ready = False


# =============================================================================
# PART 3: SCIPY ALTERNATIVE
# =============================================================================

def scipy_zscore(data: List[float]) -> List[float]:
    """
    Calculate z-scores using scipy (vectorised).
    
    This is efficient for batch processing but requires all data
    to be available upfront.
    
    Args:
        data: List of values
        
    Returns:
        List of z-scores
    """
    from scipy import stats
    return stats.zscore(data).tolist()


def scipy_detect_outliers(data: List[float], threshold: float = 2.5
                         ) -> List[Tuple[int, float, float]]:
    """
    Detect outliers in batch using scipy.
    
    Args:
        data: List of values
        threshold: Z-score threshold
        
    Returns:
        List of (index, value, z_score) for outliers
    """
    from scipy import stats
    z_scores = stats.zscore(data)
    
    outliers = []
    for i, (val, z) in enumerate(zip(data, z_scores)):
        if abs(z) > threshold:
            outliers.append((i, val, z))
    
    return outliers


# =============================================================================
# DEMONSTRATION
# =============================================================================

def demo_welford():
    """Demonstrate Welford's algorithm vs naive approach."""
    print("=" * 60)
    print("WELFORD'S ALGORITHM DEMONSTRATION")
    print("=" * 60)
    
    # Test data
    data = [2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0]
    
    # Welford's algorithm
    welford = WelfordState()
    for val in data:
        welford.update(val)
    
    print(f"\nData: {data}")
    print(f"\nWelford's algorithm:")
    print(f"  Mean: {welford.mean:.4f}")
    print(f"  Sample Variance: {welford.variance:.4f}")
    print(f"  Standard Deviation: {welford.stddev:.4f}")
    
    # Compare with numpy
    import numpy as np
    print(f"\nNumPy comparison:")
    print(f"  Mean: {np.mean(data):.4f}")
    print(f"  Sample Variance: {np.var(data, ddof=1):.4f}")
    print(f"  Standard Deviation: {np.std(data, ddof=1):.4f}")
    
    # Demonstrate numerical stability with large numbers
    print("\n" + "-" * 40)
    print("Numerical Stability Test:")
    
    # Values close together but large
    large_data = [1000000.0 + i * 0.1 for i in range(100)]
    
    welford2 = WelfordState()
    for val in large_data:
        welford2.update(val)
    
    print(f"  Welford variance: {welford2.variance:.10f}")
    print(f"  NumPy variance:   {np.var(large_data, ddof=1):.10f}")


def demo_anomaly_detection():
    """Demonstrate anomaly detection."""
    print("\n" + "=" * 60)
    print("ANOMALY DETECTION DEMONSTRATION")
    print("=" * 60)
    
    # Simulated temperature readings with anomalies
    readings = [
        22.1, 22.3, 21.8, 22.0, 22.2,  # Baseline (first 5)
        21.9, 22.1, 22.0, 21.7, 22.4,  # Baseline (next 5)
        22.0, 21.8, 45.0, 22.1, 22.3,  # Anomaly at index 12 (45.0)
        22.0, 21.9, -5.0, 22.2, 22.0   # Anomaly at index 17 (-5.0)
    ]
    
    detector = AnomalyDetector(baseline_count=10, threshold=2.5)
    
    print(f"\n{'Index':>5} {'Value':>8} {'Status':>12} {'Z-Score':>10} {'Severity':>10}")
    print("-" * 50)
    
    for i, val in enumerate(readings):
        result = detector.process(val)
        
        if not detector.baseline_ready or i < detector.baseline_count:
            status = "BASELINE"
            z_str = "-"
            sev_str = "-"
        else:
            status = "ANOMALY" if result.is_anomaly else "Normal"
            z_str = f"{result.z_score:.2f}"
            sev_str = result.severity.name
        
        print(f"{i:>5} {val:>8.1f} {status:>12} {z_str:>10} {sev_str:>10}")
    
    print(f"\nBaseline mean: {detector.mean:.2f}")
    print(f"Baseline stddev: {detector.stddev:.2f}")


def demo_scipy_comparison():
    """Show scipy batch processing alternative."""
    print("\n" + "=" * 60)
    print("SCIPY BATCH PROCESSING COMPARISON")
    print("=" * 60)
    
    try:
        data = [22.1, 22.3, 21.8, 22.0, 22.2, 21.9, 22.1, 45.0, 22.0, -5.0]
        
        outliers = scipy_detect_outliers(data, threshold=2.5)
        
        print(f"\nData: {data}")
        print(f"\nOutliers detected (threshold=2.5):")
        for idx, val, z in outliers:
            print(f"  Index {idx}: value={val:.1f}, z-score={z:.2f}")
        
    except ImportError:
        print("scipy not installed - skipping batch demo")


if __name__ == "__main__":
    demo_welford()
    demo_anomaly_detection()
    demo_scipy_comparison()

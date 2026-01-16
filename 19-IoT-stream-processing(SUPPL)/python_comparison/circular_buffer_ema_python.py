#!/usr/bin/env python3
"""
=============================================================================
WEEK 19: IoT and Stream Processing - Python Comparison
Circular Buffer and Moving Averages
=============================================================================

This file demonstrates Python equivalents of the C implementations.
NOT for execution - for comparison and understanding only.

Author: ATP Course - ASE-CSIE Bucharest
=============================================================================
"""

from collections import deque
from dataclasses import dataclass
from typing import List, Optional
import numpy as np

# =============================================================================
# PART 1: CIRCULAR BUFFER
# =============================================================================

class CircularBuffer:
    """
    Circular buffer (ring buffer) implementation using collections.deque.
    
    Python's deque with maxlen handles wraparound automatically,
    unlike C where we must manually compute (head + 1) % capacity.
    
    Time Complexity:
        - push: O(1)
        - mean: O(1) with cached sum, O(n) without
        - variance: O(n)
    
    Space Complexity: O(n) where n = capacity
    """
    
    def __init__(self, capacity: int):
        """
        Initialise circular buffer with given capacity.
        
        Args:
            capacity: Maximum number of elements
        """
        self.data = deque(maxlen=capacity)
        self.cached_sum = 0.0  # Maintain running sum for O(1) mean
    
    def push(self, value: float) -> None:
        """
        Add value to buffer. If full, oldest value is removed automatically.
        
        Args:
            value: Value to add
        """
        if len(self.data) == self.data.maxlen:
            # Buffer is full - subtract the value that will be removed
            self.cached_sum -= self.data[0]
        
        self.data.append(value)
        self.cached_sum += value
    
    def mean(self) -> float:
        """
        Calculate mean of values in buffer.
        
        Returns:
            Mean value, or 0.0 if buffer is empty
        
        Time Complexity: O(1) due to cached sum
        """
        if not self.data:
            return 0.0
        return self.cached_sum / len(self.data)
    
    def variance(self) -> float:
        """
        Calculate sample variance of values in buffer.
        
        Returns:
            Sample variance (using Bessel's correction), or 0.0 if < 2 values
        
        Time Complexity: O(n)
        """
        if len(self.data) < 2:
            return 0.0
        
        mean = self.mean()
        sum_sq_diff = sum((x - mean) ** 2 for x in self.data)
        return sum_sq_diff / (len(self.data) - 1)
    
    def stddev(self) -> float:
        """Calculate standard deviation."""
        return np.sqrt(self.variance())
    
    @property
    def count(self) -> int:
        """Number of elements currently in buffer."""
        return len(self.data)
    
    @property
    def capacity(self) -> int:
        """Maximum capacity of buffer."""
        return self.data.maxlen
    
    def oldest(self) -> Optional[float]:
        """Return oldest value without removing it."""
        return self.data[0] if self.data else None
    
    def newest(self) -> Optional[float]:
        """Return newest value without removing it."""
        return self.data[-1] if self.data else None
    
    def __repr__(self) -> str:
        return f"CircularBuffer({list(self.data)}, capacity={self.capacity})"


# =============================================================================
# PART 2: SIMPLE MOVING AVERAGE
# =============================================================================

class SimpleMovingAverage:
    """
    Simple Moving Average using circular buffer.
    
    SMA_t = (1/n) * sum(x_{t-n+1}, ..., x_t)
    
    All values in the window have equal weight.
    """
    
    def __init__(self, window_size: int):
        """
        Initialise SMA with window size.
        
        Args:
            window_size: Number of values to include in average
        """
        self.buffer = CircularBuffer(window_size)
    
    def update(self, value: float) -> float:
        """
        Add new value and return current SMA.
        
        Args:
            value: New value to add
            
        Returns:
            Current simple moving average
        """
        self.buffer.push(value)
        return self.buffer.mean()
    
    @property
    def value(self) -> float:
        """Current SMA value."""
        return self.buffer.mean()


# =============================================================================
# PART 3: EXPONENTIAL MOVING AVERAGE
# =============================================================================

class ExponentialMovingAverage:
    """
    Exponential Moving Average implementation.
    
    EMA_t = alpha * x_t + (1 - alpha) * EMA_{t-1}
    
    More recent values have higher weight. The smoothing factor alpha
    controls the rate of decay:
    - alpha close to 1: fast response, less smoothing
    - alpha close to 0: slow response, more smoothing
    
    Time Complexity: O(1)
    Space Complexity: O(1)
    """
    
    def __init__(self, alpha: float):
        """
        Initialise EMA with smoothing factor.
        
        Args:
            alpha: Smoothing factor (0 < alpha <= 1)
        """
        if not 0 < alpha <= 1:
            raise ValueError("alpha must be between 0 (exclusive) and 1 (inclusive)")
        
        self.alpha = alpha
        self.value: Optional[float] = None
        self.initialised = False
    
    def update(self, new_value: float) -> float:
        """
        Update EMA with new value.
        
        Args:
            new_value: New observation
            
        Returns:
            Updated EMA value
        """
        if not self.initialised:
            self.value = new_value
            self.initialised = True
        else:
            # EMA formula: alpha * new + (1 - alpha) * old
            self.value = self.alpha * new_value + (1 - self.alpha) * self.value
        
        return self.value
    
    def reset(self) -> None:
        """Reset EMA state."""
        self.value = None
        self.initialised = False
    
    def __repr__(self) -> str:
        return f"EMA(alpha={self.alpha}, value={self.value})"


# =============================================================================
# PART 4: PANDAS ALTERNATIVES (VECTORISED)
# =============================================================================

def pandas_sma(data: List[float], window: int) -> List[float]:
    """
    Calculate SMA using pandas (vectorised).
    
    This is more efficient for batch processing but requires
    all data to be available upfront.
    
    Args:
        data: List of values
        window: Window size
        
    Returns:
        List of SMA values (NaN for first window-1 values)
    """
    import pandas as pd
    series = pd.Series(data)
    return series.rolling(window=window).mean().tolist()


def pandas_ema(data: List[float], alpha: float) -> List[float]:
    """
    Calculate EMA using pandas (vectorised).
    
    Args:
        data: List of values
        alpha: Smoothing factor
        
    Returns:
        List of EMA values
    """
    import pandas as pd
    series = pd.Series(data)
    return series.ewm(alpha=alpha, adjust=False).mean().tolist()


# =============================================================================
# DEMONSTRATION
# =============================================================================

def demo_circular_buffer():
    """Demonstrate circular buffer operations."""
    print("=" * 60)
    print("CIRCULAR BUFFER DEMONSTRATION")
    print("=" * 60)
    
    cb = CircularBuffer(5)
    
    print(f"\nCapacity: {cb.capacity}")
    print(f"Initial state: {cb}")
    
    # Push some values
    for i, val in enumerate([10, 20, 30, 40, 50, 60, 70], 1):
        cb.push(val)
        print(f"After push({val}): count={cb.count}, mean={cb.mean():.2f}, "
              f"oldest={cb.oldest()}, newest={cb.newest()}")
    
    print(f"\nFinal variance: {cb.variance():.2f}")
    print(f"Final stddev: {cb.stddev():.2f}")


def demo_moving_averages():
    """Compare SMA and EMA on sample data."""
    print("\n" + "=" * 60)
    print("MOVING AVERAGES COMPARISON")
    print("=" * 60)
    
    # Sample data with a spike
    data = [20.0, 21.0, 20.5, 21.5, 45.0, 22.0, 21.0, 20.0, 20.5, 21.0]
    
    sma = SimpleMovingAverage(window_size=3)
    ema_slow = ExponentialMovingAverage(alpha=0.1)
    ema_fast = ExponentialMovingAverage(alpha=0.5)
    
    print(f"\n{'Value':>8} {'SMA(3)':>10} {'EMA(0.1)':>10} {'EMA(0.5)':>10}")
    print("-" * 42)
    
    for val in data:
        sma_val = sma.update(val)
        ema_slow_val = ema_slow.update(val)
        ema_fast_val = ema_fast.update(val)
        
        print(f"{val:>8.1f} {sma_val:>10.2f} {ema_slow_val:>10.2f} {ema_fast_val:>10.2f}")
    
    print("\nNote: Spike at value 45.0")
    print("- SMA responds but recovers after 3 values (window size)")
    print("- EMA(0.1) responds slowly, takes many values to recover")
    print("- EMA(0.5) responds quickly but also recovers quickly")


def demo_pandas_comparison():
    """Show pandas vectorised alternatives."""
    print("\n" + "=" * 60)
    print("PANDAS VECTORISED ALTERNATIVES")
    print("=" * 60)
    
    try:
        import pandas as pd
        
        data = [10, 12, 11, 15, 13, 14, 16, 12, 11, 13]
        
        sma_values = pandas_sma(data, window=3)
        ema_values = pandas_ema(data, alpha=0.3)
        
        print(f"\n{'Index':>6} {'Value':>8} {'SMA(3)':>10} {'EMA(0.3)':>10}")
        print("-" * 38)
        
        for i, (val, sma, ema) in enumerate(zip(data, sma_values, ema_values)):
            sma_str = f"{sma:.2f}" if not np.isnan(sma) else "NaN"
            print(f"{i:>6} {val:>8} {sma_str:>10} {ema:.2f}")
        
    except ImportError:
        print("pandas not installed - skipping vectorised demo")


if __name__ == "__main__":
    demo_circular_buffer()
    demo_moving_averages()
    demo_pandas_comparison()

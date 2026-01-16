#!/usr/bin/env python3
"""
=============================================================================
WEEK 19: ALGORITHMS FOR IoT AND STREAM PROCESSING
Python Comparison: MQTT Patterns and Sensor Simulation
=============================================================================

This file demonstrates Python equivalents of the C implementations for
MQTT publish-subscribe patterns and complete sensor simulation pipelines.

NOTE: This file is for COMPARISON and EDUCATIONAL purposes only.
      It is NOT meant to replace the C implementation, but to show
      equivalent patterns in Python for cross-language understanding.

Key differences from C:
  - Python's paho-mqtt library for real MQTT (vs simulation)
  - asyncio for concurrent sensor handling
  - dataclasses for clean struct equivalents
  - generators for infinite streams
  - numpy for efficient numeric operations

=============================================================================
"""

from dataclasses import dataclass, field
from typing import Callable, Optional, List, Dict, Any, Generator
from collections import deque
from enum import Enum, auto
import time
import random
import math
import re
import heapq


# =============================================================================
# PART 1: MQTT-STYLE PUBLISH-SUBSCRIBE SIMULATION
# =============================================================================

class TopicMatcher:
    """
    MQTT topic matching with wildcard support.
    
    Wildcards:
      + : Single-level wildcard (matches one level)
      # : Multi-level wildcard (matches remainder)
    
    Examples:
      "sensors/+/temperature" matches "sensors/room1/temperature"
      "sensors/#" matches "sensors/room1/temp/celsius"
    
    Time Complexity: O(n) where n = topic length
    """
    
    @staticmethod
    def matches(filter_pattern: str, topic: str) -> bool:
        """Check if topic matches the filter pattern with wildcards."""
        filter_parts = filter_pattern.split('/')
        topic_parts = topic.split('/')
        
        fi = 0  # Filter index
        ti = 0  # Topic index
        
        while fi < len(filter_parts) and ti < len(topic_parts):
            if filter_parts[fi] == '#':
                # Multi-level wildcard: matches everything remaining
                return True
            elif filter_parts[fi] == '+':
                # Single-level wildcard: matches this level
                fi += 1
                ti += 1
            elif filter_parts[fi] == topic_parts[ti]:
                # Exact match
                fi += 1
                ti += 1
            else:
                return False
        
        # Handle trailing # in filter
        if fi < len(filter_parts) and filter_parts[fi] == '#':
            return True
        
        # Both must be exhausted for match
        return fi == len(filter_parts) and ti == len(topic_parts)


@dataclass
class Message:
    """MQTT-style message."""
    topic: str
    payload: Any
    timestamp: float = field(default_factory=time.time)
    qos: int = 0
    retain: bool = False


class MessageBroker:
    """
    Simple in-memory MQTT-style message broker.
    
    Supports:
      - Topic subscriptions with wildcards
      - Message retention
      - QoS simulation (basic)
    
    In production, use paho-mqtt client with a real broker like Mosquitto.
    
    Time Complexity:
      - Publish: O(s × t) where s = subscribers, t = topic length
      - Subscribe: O(1)
    """
    
    def __init__(self, max_retained: int = 1000):
        self._subscriptions: Dict[str, List[Callable[[Message], None]]] = {}
        self._retained: Dict[str, Message] = {}
        self._max_retained = max_retained
        self._message_count = 0
    
    def subscribe(self, topic_filter: str, 
                  callback: Callable[[Message], None]) -> None:
        """
        Subscribe to a topic pattern.
        
        Args:
            topic_filter: Topic pattern (supports + and # wildcards)
            callback: Function to call with matching messages
        """
        if topic_filter not in self._subscriptions:
            self._subscriptions[topic_filter] = []
        self._subscriptions[topic_filter].append(callback)
        
        # Deliver retained messages matching this filter
        for topic, msg in self._retained.items():
            if TopicMatcher.matches(topic_filter, topic):
                callback(msg)
    
    def unsubscribe(self, topic_filter: str, 
                    callback: Optional[Callable] = None) -> None:
        """Remove subscription."""
        if topic_filter in self._subscriptions:
            if callback:
                self._subscriptions[topic_filter].remove(callback)
            else:
                del self._subscriptions[topic_filter]
    
    def publish(self, topic: str, payload: Any, 
                qos: int = 0, retain: bool = False) -> None:
        """
        Publish a message to a topic.
        
        Args:
            topic: Exact topic (no wildcards)
            payload: Message content
            qos: Quality of Service (0, 1, or 2)
            retain: Whether to retain message for future subscribers
        """
        msg = Message(topic=topic, payload=payload, qos=qos, retain=retain)
        self._message_count += 1
        
        # Store retained message
        if retain:
            self._retained[topic] = msg
            # Limit retained messages
            if len(self._retained) > self._max_retained:
                oldest_topic = min(self._retained, 
                                   key=lambda t: self._retained[t].timestamp)
                del self._retained[oldest_topic]
        
        # Deliver to matching subscribers
        for filter_pattern, callbacks in self._subscriptions.items():
            if TopicMatcher.matches(filter_pattern, topic):
                for callback in callbacks:
                    callback(msg)
    
    @property
    def stats(self) -> Dict[str, int]:
        """Get broker statistics."""
        return {
            'subscriptions': sum(len(cbs) for cbs in self._subscriptions.values()),
            'retained_messages': len(self._retained),
            'total_published': self._message_count
        }


# =============================================================================
# PART 2: SENSOR SIMULATION
# =============================================================================

class SensorType(Enum):
    """Common sensor types with typical parameters."""
    TEMPERATURE = auto()    # °C, typical range: -40 to 85
    HUMIDITY = auto()       # %, range: 0 to 100
    PRESSURE = auto()       # hPa, range: 300 to 1100
    LIGHT = auto()          # lux, range: 0 to 100000
    ACCELEROMETER = auto()  # g, range: -16 to 16


@dataclass
class SensorConfig:
    """Configuration for sensor simulation."""
    sensor_type: SensorType
    base_value: float
    noise_stddev: float
    drift_rate: float = 0.0         # Units per second
    anomaly_probability: float = 0.01
    anomaly_magnitude: float = 5.0  # In standard deviations
    sample_interval: float = 1.0    # Seconds


class SensorSimulator:
    """
    Realistic sensor data simulator.
    
    Generates readings with:
      - Base value (can vary with time for patterns)
      - Gaussian noise
      - Linear drift
      - Random anomalies (spikes)
    
    The formula mirrors the C implementation:
      value = base + drift × time + N(0, σ) + anomaly
    
    Python advantage: Generators provide infinite streams naturally.
    """
    
    def __init__(self, config: SensorConfig, seed: Optional[int] = None):
        self.config = config
        self._rng = random.Random(seed)
        self._start_time = time.time()
        self._reading_count = 0
    
    def _box_muller(self) -> float:
        """
        Generate standard normal using Box-Muller transform.
        
        Equivalent to C implementation (no numpy).
        Could use random.gauss() but this shows the algorithm.
        """
        u1 = self._rng.random()
        u2 = self._rng.random()
        
        # Avoid log(0)
        while u1 < 1e-10:
            u1 = self._rng.random()
        
        z = math.sqrt(-2.0 * math.log(u1)) * math.cos(2.0 * math.pi * u2)
        return z
    
    def read(self) -> float:
        """
        Generate a single sensor reading.
        
        Time Complexity: O(1)
        """
        elapsed = time.time() - self._start_time
        self._reading_count += 1
        
        # Base value with drift
        value = self.config.base_value + self.config.drift_rate * elapsed
        
        # Add Gaussian noise
        value += self._box_muller() * self.config.noise_stddev
        
        # Random anomaly
        if self._rng.random() < self.config.anomaly_probability:
            sign = 1 if self._rng.random() > 0.5 else -1
            value += sign * self.config.anomaly_magnitude * self.config.noise_stddev
        
        return value
    
    def stream(self, count: Optional[int] = None) -> Generator[float, None, None]:
        """
        Generate a stream of sensor readings.
        
        Args:
            count: Number of readings (None for infinite)
        
        Yields:
            Sensor readings at configured interval
        
        Python advantage: Generator provides lazy evaluation and
                          natural infinite stream semantics.
        """
        generated = 0
        while count is None or generated < count:
            yield self.read()
            generated += 1
            time.sleep(self.config.sample_interval)
    
    def batch(self, count: int) -> List[float]:
        """Generate a batch of readings without delay (for testing)."""
        return [self.read() for _ in range(count)]


# =============================================================================
# PART 3: COMPLETE PIPELINE
# =============================================================================

@dataclass
class PipelineStats:
    """Statistics from pipeline processing."""
    readings_processed: int = 0
    anomalies_detected: int = 0
    messages_published: int = 0
    current_mean: float = 0.0
    current_stddev: float = 0.0


class StreamPipeline:
    """
    Complete IoT stream processing pipeline.
    
    Integrates:
      1. Sensor simulation (or real data input)
      2. Circular buffer for windowed statistics
      3. Welford's algorithm for online mean/variance
      4. Anomaly detection via z-score
      5. MQTT-style publishing
    
    This class demonstrates the complete flow from the C example
    using Pythonic patterns.
    """
    
    def __init__(self, 
                 window_size: int = 20,
                 ema_alpha: float = 0.1,
                 anomaly_threshold: float = 3.0,
                 broker: Optional[MessageBroker] = None,
                 topic_prefix: str = "sensors"):
        
        self.window_size = window_size
        self.ema_alpha = ema_alpha
        self.anomaly_threshold = anomaly_threshold
        self.broker = broker or MessageBroker()
        self.topic_prefix = topic_prefix
        
        # Circular buffer using deque (automatic size management)
        self._buffer: deque = deque(maxlen=window_size)
        
        # Welford state
        self._count = 0
        self._mean = 0.0
        self._M2 = 0.0
        
        # EMA state
        self._ema: Optional[float] = None
        
        # Statistics
        self.stats = PipelineStats()
    
    def _welford_update(self, value: float) -> None:
        """
        Update Welford running statistics.
        
        Critical: delta uses OLD mean, delta2 uses NEW mean.
        """
        self._count += 1
        delta = value - self._mean
        self._mean += delta / self._count
        delta2 = value - self._mean  # Uses NEW mean
        self._M2 += delta * delta2
    
    @property
    def variance(self) -> float:
        """Sample variance from Welford state."""
        if self._count < 2:
            return 0.0
        return self._M2 / (self._count - 1)
    
    @property
    def stddev(self) -> float:
        """Standard deviation."""
        return math.sqrt(self.variance)
    
    def process(self, value: float, sensor_id: str = "sensor1") -> Dict[str, Any]:
        """
        Process a single sensor reading through the pipeline.
        
        Returns dictionary with processing results:
          - value: Original reading
          - ema: Exponential moving average
          - sma: Simple moving average (window)
          - is_anomaly: Whether anomaly detected
          - z_score: Z-score if computed
          - published_topics: Topics where data was published
        """
        self.stats.readings_processed += 1
        published = []
        
        # 1. Add to circular buffer
        self._buffer.append(value)
        
        # 2. Update Welford statistics
        self._welford_update(value)
        
        # 3. Calculate SMA from buffer
        sma = sum(self._buffer) / len(self._buffer)
        
        # 4. Update EMA
        if self._ema is None:
            self._ema = value
        else:
            self._ema = self.ema_alpha * value + (1 - self.ema_alpha) * self._ema
        
        # 5. Anomaly detection (need baseline first)
        is_anomaly = False
        z_score = None
        
        if self._count >= self.window_size:
            self.stats.current_mean = self._mean
            self.stats.current_stddev = self.stddev
            
            if self.stddev > 1e-10:
                z_score = abs(value - self._mean) / self.stddev
                is_anomaly = z_score > self.anomaly_threshold
                
                if is_anomaly:
                    self.stats.anomalies_detected += 1
        
        # 6. Publish to broker
        base_topic = f"{self.topic_prefix}/{sensor_id}"
        
        # Always publish value
        self.broker.publish(f"{base_topic}/value", value)
        published.append(f"{base_topic}/value")
        
        # Publish smoothed value
        self.broker.publish(f"{base_topic}/ema", self._ema)
        published.append(f"{base_topic}/ema")
        
        # Publish anomaly if detected
        if is_anomaly:
            self.broker.publish(f"{base_topic}/anomaly", {
                'value': value,
                'z_score': z_score,
                'mean': self._mean,
                'stddev': self.stddev
            }, retain=True)
            published.append(f"{base_topic}/anomaly")
        
        self.stats.messages_published += len(published)
        
        return {
            'value': value,
            'ema': self._ema,
            'sma': sma,
            'is_anomaly': is_anomaly,
            'z_score': z_score,
            'published_topics': published
        }
    
    def process_batch(self, values: List[float], 
                      sensor_id: str = "sensor1") -> List[Dict[str, Any]]:
        """Process multiple readings."""
        return [self.process(v, sensor_id) for v in values]


# =============================================================================
# PART 4: PYTHONIC ALTERNATIVES (USING PANDAS/SCIPY)
# =============================================================================

def demo_pandas_pipeline():
    """
    Demonstrate pandas-based stream processing.
    
    For batch processing, pandas provides vectorised operations
    that are much faster than iterative approaches.
    """
    try:
        import pandas as pd
        import numpy as np
    except ImportError:
        print("  pandas/numpy not available for this demo")
        return
    
    print("\n  Pandas-based Stream Processing:")
    print("  " + "─" * 50)
    
    # Generate sample data
    np.random.seed(42)
    n = 1000
    base = 25.0
    noise = np.random.normal(0, 0.5, n)
    
    # Add anomalies
    anomaly_idx = np.random.choice(n, size=10, replace=False)
    noise[anomaly_idx] += np.random.choice([-1, 1], size=10) * 5
    
    data = base + noise
    
    # Create DataFrame
    df = pd.DataFrame({
        'value': data,
        'timestamp': pd.date_range('2024-01-01', periods=n, freq='s')
    })
    
    # Calculate rolling statistics (vectorised, very fast)
    window = 20
    df['sma'] = df['value'].rolling(window=window).mean()
    df['ema'] = df['value'].ewm(alpha=0.1, adjust=False).mean()
    df['rolling_std'] = df['value'].rolling(window=window).std()
    
    # Z-score anomaly detection
    df['z_score'] = (df['value'] - df['sma']) / df['rolling_std']
    df['is_anomaly'] = df['z_score'].abs() > 3.0
    
    # Results
    anomaly_count = df['is_anomaly'].sum()
    print(f"    Processed {n} readings")
    print(f"    Detected {anomaly_count} anomalies")
    print(f"    Mean: {df['value'].mean():.2f}, Std: {df['value'].std():.2f}")
    
    # Show some anomalies
    print("\n    Sample anomalies:")
    anomalies = df[df['is_anomaly']].head(5)
    for idx, row in anomalies.iterrows():
        print(f"      t={idx}: value={row['value']:.2f}, z={row['z_score']:.2f}")


def demo_scipy_zscore():
    """
    Demonstrate scipy.stats for batch anomaly detection.
    """
    try:
        from scipy import stats
        import numpy as np
    except ImportError:
        print("  scipy not available for this demo")
        return
    
    print("\n  Scipy Z-Score Detection:")
    print("  " + "─" * 50)
    
    # Sample data with anomalies
    np.random.seed(42)
    data = np.random.normal(25, 0.5, 100)
    data[10] = 30  # Anomaly
    data[50] = 20  # Anomaly
    
    # Compute z-scores in one call
    z_scores = stats.zscore(data)
    
    # Find anomalies
    threshold = 3.0
    anomalies = np.abs(z_scores) > threshold
    
    print(f"    Data points: {len(data)}")
    print(f"    Anomalies found: {anomalies.sum()}")
    print(f"    Anomaly indices: {np.where(anomalies)[0].tolist()}")


# =============================================================================
# DEMONSTRATIONS
# =============================================================================

def demo_topic_matching():
    """Demonstrate MQTT topic matching."""
    print("\n")
    print("╔═══════════════════════════════════════════════════════════════╗")
    print("║      PART 1: MQTT TOPIC MATCHING                              ║")
    print("╚═══════════════════════════════════════════════════════════════╝\n")
    
    test_cases = [
        ("sensors/+/temperature", "sensors/room1/temperature", True),
        ("sensors/+/temperature", "sensors/room1/humidity", False),
        ("sensors/#", "sensors/room1/temp/celsius", True),
        ("sensors/room1/#", "sensors/room2/temp", False),
        ("sensors/+/+", "sensors/room1/temp", True),
        ("sensors/+/+", "sensors/room1/temp/unit", False),
    ]
    
    print("  Testing wildcard patterns:\n")
    for pattern, topic, expected in test_cases:
        result = TopicMatcher.matches(pattern, topic)
        status = "✓" if result == expected else "✗"
        print(f"    {status} '{pattern}' vs '{topic}': {result}")


def demo_message_broker():
    """Demonstrate MQTT-style broker."""
    print("\n")
    print("╔═══════════════════════════════════════════════════════════════╗")
    print("║      PART 2: MESSAGE BROKER SIMULATION                        ║")
    print("╚═══════════════════════════════════════════════════════════════╝\n")
    
    broker = MessageBroker()
    
    # Track received messages
    received = []
    
    def on_temperature(msg: Message):
        received.append(('temp', msg))
        print(f"    [Temp Handler] {msg.topic}: {msg.payload}")
    
    def on_all_sensors(msg: Message):
        received.append(('all', msg))
        print(f"    [All Handler] {msg.topic}: {msg.payload}")
    
    # Subscribe
    broker.subscribe("sensors/+/temperature", on_temperature)
    broker.subscribe("sensors/#", on_all_sensors)
    
    print("  Subscriptions active:")
    print("    - sensors/+/temperature → temperature handler")
    print("    - sensors/# → all sensors handler\n")
    
    # Publish
    print("  Publishing messages:\n")
    broker.publish("sensors/room1/temperature", 23.5)
    broker.publish("sensors/room1/humidity", 45.0)
    broker.publish("sensors/room2/temperature", 24.1)
    
    print(f"\n  Broker stats: {broker.stats}")
    print(f"  Messages received by handlers: {len(received)}")


def demo_sensor_simulation():
    """Demonstrate sensor simulation."""
    print("\n")
    print("╔═══════════════════════════════════════════════════════════════╗")
    print("║      PART 3: SENSOR SIMULATION                                ║")
    print("╚═══════════════════════════════════════════════════════════════╝\n")
    
    config = SensorConfig(
        sensor_type=SensorType.TEMPERATURE,
        base_value=25.0,
        noise_stddev=0.5,
        drift_rate=0.0,
        anomaly_probability=0.05,
        anomaly_magnitude=5.0,
        sample_interval=0.0
    )
    
    sensor = SensorSimulator(config, seed=42)
    
    # Generate batch
    readings = sensor.batch(20)
    
    print("  Temperature sensor simulation (base=25°C, σ=0.5°C):\n")
    print("    Reading  Value   Status")
    print("    " + "─" * 30)
    
    mean = sum(readings) / len(readings)
    stddev = (sum((x - mean) ** 2 for x in readings) / (len(readings) - 1)) ** 0.5
    
    for i, val in enumerate(readings):
        z = abs(val - mean) / stddev if stddev > 0 else 0
        status = "ANOMALY!" if z > 3.0 else ""
        print(f"    {i+1:3d}      {val:6.2f}  {status}")
    
    print(f"\n  Statistics: mean={mean:.2f}°C, stddev={stddev:.2f}°C")


def demo_complete_pipeline():
    """Demonstrate complete stream processing pipeline."""
    print("\n")
    print("╔═══════════════════════════════════════════════════════════════╗")
    print("║      PART 4: COMPLETE STREAM PIPELINE                         ║")
    print("╚═══════════════════════════════════════════════════════════════╝\n")
    
    # Create pipeline with broker
    broker = MessageBroker()
    pipeline = StreamPipeline(
        window_size=10,
        ema_alpha=0.2,
        anomaly_threshold=2.5,
        broker=broker,
        topic_prefix="factory"
    )
    
    # Subscribe to anomalies
    def on_anomaly(msg: Message):
        data = msg.payload
        print(f"    🚨 ANOMALY on {msg.topic}: value={data['value']:.2f}, z={data['z_score']:.2f}")
    
    broker.subscribe("factory/+/anomaly", on_anomaly)
    
    # Generate test data
    sensor = SensorSimulator(
        SensorConfig(
            sensor_type=SensorType.TEMPERATURE,
            base_value=25.0,
            noise_stddev=0.3,
            anomaly_probability=0.1,
            anomaly_magnitude=4.0,
            sample_interval=0.0
        ),
        seed=42
    )
    
    print("  Processing 30 sensor readings:\n")
    print("    #   Value   EMA     SMA     Z-Score  Status")
    print("    " + "─" * 50)
    
    readings = sensor.batch(30)
    for i, value in enumerate(readings):
        result = pipeline.process(value, "temp1")
        
        z_str = f"{result['z_score']:.2f}" if result['z_score'] else "N/A"
        status = "ANOMALY" if result['is_anomaly'] else ""
        
        print(f"    {i+1:2d}  {value:6.2f}  {result['ema']:6.2f}  "
              f"{result['sma']:6.2f}  {z_str:>7}  {status}")
    
    print(f"\n  Pipeline Statistics:")
    print(f"    Readings processed: {pipeline.stats.readings_processed}")
    print(f"    Anomalies detected: {pipeline.stats.anomalies_detected}")
    print(f"    Messages published: {pipeline.stats.messages_published}")
    print(f"    Final mean: {pipeline.stats.current_mean:.2f}")
    print(f"    Final stddev: {pipeline.stats.current_stddev:.2f}")


def demo_python_alternatives():
    """Demonstrate pandas/scipy alternatives."""
    print("\n")
    print("╔═══════════════════════════════════════════════════════════════╗")
    print("║      PART 5: PYTHONIC ALTERNATIVES (pandas/scipy)             ║")
    print("╚═══════════════════════════════════════════════════════════════╝")
    
    demo_pandas_pipeline()
    demo_scipy_zscore()


# =============================================================================
# COMPARISON WITH C IMPLEMENTATION
# =============================================================================

def print_comparison():
    """Print C vs Python comparison notes."""
    print("\n")
    print("╔═══════════════════════════════════════════════════════════════╗")
    print("║      C vs PYTHON COMPARISON                                    ║")
    print("╚═══════════════════════════════════════════════════════════════╝\n")
    
    comparisons = [
        ("Circular Buffer", 
         "Manual wraparound with modulo",
         "collections.deque(maxlen=N)"),
        
        ("MQTT Broker",
         "Custom topic tree with linked lists",
         "Dictionary of callbacks + regex"),
        
        ("Sensor Simulation",
         "time.h + manual Box-Muller",
         "random.gauss() or numpy.random"),
        
        ("Stream Processing",
         "Explicit loops, manual memory",
         "Generators, automatic GC"),
        
        ("Batch Analytics",
         "Loop-based (or external library)",
         "pandas.rolling(), numpy vectorised"),
        
        ("Real MQTT",
         "libmosquitto or paho.mqtt.c",
         "paho-mqtt-python"),
    ]
    
    print("  ┌──────────────────────┬──────────────────────┬──────────────────────┐")
    print("  │      Concept         │         C            │       Python         │")
    print("  ├──────────────────────┼──────────────────────┼──────────────────────┤")
    
    for concept, c_impl, py_impl in comparisons:
        print(f"  │ {concept:<20} │ {c_impl:<20} │ {py_impl:<20} │")
    
    print("  └──────────────────────┴──────────────────────┴──────────────────────┘")
    
    print("\n  When to use C:")
    print("    • Embedded devices with limited resources")
    print("    • Real-time constraints (deterministic timing)")
    print("    • Direct hardware access")
    print("    • Maximum performance for edge processing")
    
    print("\n  When to use Python:")
    print("    • Cloud/server-side stream processing")
    print("    • Rapid prototyping and experimentation")
    print("    • Integration with ML frameworks (TensorFlow, PyTorch)")
    print("    • Data analysis and visualisation")


# =============================================================================
# MAIN
# =============================================================================

if __name__ == "__main__":
    print("\n")
    print("╔═══════════════════════════════════════════════════════════════╗")
    print("║                                                               ║")
    print("║     WEEK 19: ALGORITHMS FOR IoT AND STREAM PROCESSING         ║")
    print("║         Python Comparison: MQTT and Sensor Simulation         ║")
    print("║                                                               ║")
    print("╚═══════════════════════════════════════════════════════════════╝")
    
    demo_topic_matching()
    demo_message_broker()
    demo_sensor_simulation()
    demo_complete_pipeline()
    demo_python_alternatives()
    print_comparison()
    
    print("\n")
    print("╔═══════════════════════════════════════════════════════════════╗")
    print("║                    DEMONSTRATION COMPLETE                     ║")
    print("╚═══════════════════════════════════════════════════════════════╝\n")

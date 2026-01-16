# Week 19: Algorithms for IoT and Stream Processing

## 🎯 Learning Objectives

Upon completion of this laboratory session, students will be able to:

1. **Remember** the fundamental characteristics of streaming data models and their constraints (single-pass processing, bounded memory, real-time requirements)

2. **Understand** the mathematical foundations of online algorithms including Welford's method for running statistics and exponential moving averages

3. **Apply** circular buffer implementations to maintain sliding windows over continuous data streams with O(1) operations

4. **Analyse** the trade-offs between accuracy, memory consumption and latency in stream processing systems

5. **Evaluate** anomaly detection strategies using statistical methods (z-score) and determine appropriate thresholds for different application domains

6. **Create** complete IoT data processing pipelines integrating sensor simulation, filtering, statistical analysis and publish-subscribe messaging patterns

---

## 📜 Historical Context

The emergence of stream processing algorithms represents a paradigm shift from traditional batch computation. In the 1990s, as telecommunications networks began generating massive call detail records (CDRs), researchers recognised that storing all data before processing was becoming economically infeasible. This observation catalysed the development of algorithms that could process data "on the fly" — examining each element exactly once before discarding it.

The term "Internet of Things" was coined by Kevin Ashton in 1999 whilst presenting at Procter & Gamble about linking RFID technology to the internet for supply chain management. However, the theoretical foundations for processing IoT data streams emerged primarily from the database and algorithms communities. S. Muthukrishnan's influential 2005 monograph "Data Streams: Algorithms and Applications" established the formal model that continues to guide stream algorithm design.

The confluence of cheap sensors, ubiquitous connectivity and cloud computing transformed IoT from concept to reality circa 2010. By 2025, analysts estimate over 75 billion connected devices generating zettabytes of streaming data annually. This explosion necessitates algorithms that can extract actionable insights whilst respecting severe resource constraints — the very algorithms this laboratory explores.

### Key Figure: S. Muthukrishnan (1965–)

S. Muthukrishnan, known colloquially as "Muthu," pioneered the theoretical foundations of streaming algorithms whilst at Bell Labs and Rutgers University. His work formalised the streaming model: algorithms receive a sequence of elements a₁, a₂, ..., aₙ and must answer queries using space sublinear in n, typically O(polylog n). He developed fundamental techniques including the Count-Min Sketch (with Graham Cormode) and contributed extensively to frequency moment estimation.

> *"The streaming model captures the reality of modern data processing: we cannot afford to store everything, so we must extract what matters as data flows past."*
> — S. Muthukrishnan, Data Streams: Algorithms and Applications (2005)

---

## 📚 Theoretical Foundations

### 1. The Streaming Data Model

Traditional algorithms assume random access to their entire input. Stream processing fundamentally differs by imposing three constraints:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                     STREAMING DATA MODEL CONSTRAINTS                        │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   1. SEQUENTIAL ACCESS                                                      │
│      • Data arrives as an ordered sequence: a₁, a₂, a₃, ...               │
│      • Cannot revisit previous elements                                     │
│      • Processing must be "online" (decisions before seeing future)        │
│                                                                             │
│   2. BOUNDED MEMORY                                                         │
│      • Storage: O(polylog n) or O(ε⁻² log n) for approximation             │
│      • Cannot store the entire stream                                       │
│      • Must summarise with "sketches" or "synopses"                        │
│                                                                             │
│   3. REAL-TIME REQUIREMENTS                                                 │
│      • Per-element processing: O(1) or O(log n) time                       │
│      • Latency constraints for many IoT applications                       │
│      • May need answers at any moment (anytime algorithms)                 │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

                Data Stream
    ─────────────────────────────────────────────────▶
    │ a₁ │ a₂ │ a₃ │ a₄ │ a₅ │ ... │ aₙ │
    └────┴────┴────┴────┴────┴─────┴────┘
              │
              ▼
    ┌───────────────────┐
    │    Algorithm      │     Memory: O(log n) or O(1/ε²)
    │    ┌─────────┐    │
    │    │ Sketch  │    │     Per-item time: O(1)
    │    └─────────┘    │
    └─────────┬─────────┘
              │
              ▼
         Query Answer
```

### 2. Circular Buffers (Ring Buffers)

Circular buffers provide the foundation for sliding window computations. By reusing memory locations cyclically, they maintain the most recent k elements using exactly O(k) space:

```
    Initial state (capacity=5, empty):
    ┌─────┬─────┬─────┬─────┬─────┐
    │  -  │  -  │  -  │  -  │  -  │
    └─────┴─────┴─────┴─────┴─────┘
      ▲
     head (next insert position)

    After inserting 10, 20, 30:
    ┌─────┬─────┬─────┬─────┬─────┐
    │ 10  │ 20  │ 30  │  -  │  -  │
    └─────┴─────┴─────┴─────┴─────┘
                        ▲
                       head

    After inserting 40, 50, 60 (wraparound occurs):
    ┌─────┬─────┬─────┬─────┬─────┐
    │ 60  │ 20  │ 30  │ 40  │ 50  │
    └─────┴─────┴─────┴─────┴─────┘
            ▲
           head (oldest: 20, newest: 60)
```

Key operations achieve O(1) complexity:

```c
typedef struct {
    double *data;         /* Array of values */
    size_t capacity;      /* Maximum elements */
    size_t count;         /* Current elements */
    size_t head;          /* Next insertion index */
    double sum;           /* Cached sum for O(1) mean */
} CircularBuffer;

/* Insert with wraparound */
void cb_push(CircularBuffer *cb, double value) {
    if (cb->count == cb->capacity) {
        cb->sum -= cb->data[cb->head];  /* Remove oldest */
    } else {
        cb->count++;
    }
    cb->data[cb->head] = value;
    cb->sum += value;
    cb->head = (cb->head + 1) % cb->capacity;  /* Wrap around */
}
```

### 3. Moving Averages

Moving averages smooth noisy signals by averaging recent values. Two variants are fundamental:

**Simple Moving Average (SMA)**: Average of last k values
```
SMA_k(t) = (x_{t} + x_{t-1} + ... + x_{t-k+1}) / k
```

**Exponential Moving Average (EMA)**: Weighted infinite history, recent values matter more
```
EMA(t) = α · x_t + (1 - α) · EMA(t-1)
       = α · x_t + α(1-α) · x_{t-1} + α(1-α)² · x_{t-2} + ...
```

The smoothing factor α ∈ (0, 1) controls responsiveness:
- α close to 1: Rapid response, follows signal closely
- α close to 0: Heavy smoothing, slow response to changes

```
    Signal vs EMA (α = 0.3)
    
    Value
      │
   40 ┤     ╱╲
      │    ╱  ╲         ╱╲
   30 ┤   ╱    ╲       ╱  ╲
      │  ╱      ╲─────╱    ╲      ← Raw signal
   20 ┤─╱                    ╲
      │   ────────────────────    ← EMA (smoother)
   10 ┤
      │
      └──────────────────────────▶ Time
```

### 4. Welford's Online Algorithm

Computing variance traditionally requires two passes: first to compute mean, second for squared deviations. Welford's algorithm (1962) computes both in a single pass with numerical stability:

```
Algorithm (for each new value x):
    count ← count + 1
    delta ← x - mean
    mean ← mean + delta / count
    delta2 ← x - mean           (note: uses new mean)
    M2 ← M2 + delta × delta2
    
    variance ← M2 / (count - 1)  (Bessel's correction)
    stddev ← √variance
```

This algorithm avoids the "catastrophic cancellation" problem that plagues the naïve formula `Var = E[X²] - E[X]²` when variance is small relative to mean.

```c
typedef struct {
    double mean;
    double M2;      /* Sum of squared differences from running mean */
    size_t count;
} WelfordState;

void welford_update(WelfordState *state, double value) {
    state->count++;
    double delta = value - state->mean;
    state->mean += delta / state->count;
    double delta2 = value - state->mean;  /* New mean! */
    state->M2 += delta * delta2;
}

double welford_variance(WelfordState *state) {
    if (state->count < 2) return 0.0;
    return state->M2 / (state->count - 1);
}
```

### 5. Z-Score Anomaly Detection

The z-score measures how many standard deviations a value lies from the mean:

```
z = (x - μ) / σ

where:
    x = observed value
    μ = mean (estimated from running statistics)
    σ = standard deviation
```

For normally distributed data, probability thresholds are:
- |z| > 2.0: ~5% of values (moderately unusual)
- |z| > 2.5: ~1% of values (suspicious)
- |z| > 3.0: ~0.3% of values (highly anomalous)

```
    Normal Distribution with Z-Score Thresholds
    
                      ┌───┐
                     ╱│   │╲
                    ╱ │   │ ╲
                   ╱  │99%│  ╲
                  ╱   │   │   ╲
                 ╱    └───┘    ╲
                ╱               ╲
               ╱                 ╲
    ──────────╱───────────────────╲──────────▶
           -3σ  -2σ  -σ   μ   σ   2σ  3σ
           
    ◀──── Anomaly ───▶  Normal  ◀──── Anomaly ───▶
```

### 6. MQTT Publish-Subscribe Pattern

MQTT (Message Queuing Telemetry Transport) is the de facto standard for IoT messaging. Its publish-subscribe architecture decouples data producers (sensors) from consumers (processing nodes):

```
    ┌─────────────────────────────────────────────────────────────────┐
    │                    MQTT ARCHITECTURE                            │
    └─────────────────────────────────────────────────────────────────┘
    
    Publishers                    Broker                   Subscribers
    ──────────                    ──────                   ───────────
    
    ┌─────────┐                ┌─────────┐               ┌─────────┐
    │ Temp    │───publish────▶│         │◀──subscribe──│ Logger  │
    │ Sensor  │  "room/temp"  │         │  "room/#"    │         │
    └─────────┘               │         │               └─────────┘
                              │         │
    ┌─────────┐               │  MQTT   │               ┌─────────┐
    │ Humidity│───publish────▶│ Broker  │◀──subscribe──│ Monitor │
    │ Sensor  │  "room/hum"   │         │  "room/temp" │         │
    └─────────┘               │         │               └─────────┘
                              │         │
    ┌─────────┐               │         │               ┌─────────┐
    │ Motion  │───publish────▶│         │◀──subscribe──│ Alert   │
    │ Sensor  │ "door/motion" │         │   "+/motion" │ System  │
    └─────────┘               └─────────┘               └─────────┘
    
    Topic Patterns:
    ─────────────
    • "room/temp"    - Exact match
    • "room/#"       - Wildcard: all under room/
    • "+/motion"     - Single-level wildcard
```

Key properties:
- Lightweight protocol (minimal overhead)
- Quality of Service levels (0: at-most-once, 1: at-least-once, 2: exactly-once)
- Retained messages (new subscribers receive last value)
- Last Will and Testament (notification on disconnect)

---

## 🏭 Industrial Applications

### 1. Predictive Maintenance

Manufacturing equipment generates continuous sensor streams (vibration, temperature, current). Stream processing detects anomalies before failures:

```c
/* Vibration monitoring for motor bearing failure prediction */
typedef struct {
    WelfordState baseline;     /* Normal operating statistics */
    CircularBuffer recent;     /* Last 100 readings */
    double alarm_threshold;    /* z-score threshold */
    int consecutive_alarms;    /* Debouncing counter */
} VibrationMonitor;

AlertLevel check_vibration(VibrationMonitor *mon, double reading) {
    double z = (reading - mon->baseline.mean) / 
               sqrt(welford_variance(&mon->baseline));
    
    if (fabs(z) > mon->alarm_threshold) {
        mon->consecutive_alarms++;
        if (mon->consecutive_alarms >= 3) {
            return ALERT_CRITICAL;  /* Sustained anomaly */
        }
        return ALERT_WARNING;
    }
    mon->consecutive_alarms = 0;
    return ALERT_NORMAL;
}
```

### 2. Smart Grid Monitoring

Electrical grids require real-time frequency and voltage monitoring across thousands of nodes:

```c
/* Grid frequency deviation detection */
#define NOMINAL_FREQUENCY 50.0  /* Hz (Europe) */
#define CRITICAL_DEVIATION 0.5  /* Hz */

typedef struct {
    double ema_frequency;
    double alpha;
    time_t last_update;
} GridNode;

bool check_grid_stability(GridNode *node, double measured_freq) {
    node->ema_frequency = node->alpha * measured_freq + 
                          (1 - node->alpha) * node->ema_frequency;
    
    double deviation = fabs(node->ema_frequency - NOMINAL_FREQUENCY);
    return deviation < CRITICAL_DEVIATION;
}
```

### 3. Environmental Sensor Networks

Air quality monitoring stations stream pollution data requiring multi-sensor fusion:

```c
/* AQI calculation from multiple pollutant sensors */
typedef struct {
    CircularBuffer pm25_window;   /* Particulate matter 2.5μm */
    CircularBuffer pm10_window;   /* Particulate matter 10μm */
    CircularBuffer o3_window;     /* Ozone */
    CircularBuffer no2_window;    /* Nitrogen dioxide */
} AirQualityStation;

int calculate_aqi(AirQualityStation *station) {
    double pm25_avg = cb_mean(&station->pm25_window);
    double pm10_avg = cb_mean(&station->pm10_window);
    /* ... calculate individual AQIs, return max ... */
    return max_component_aqi;
}
```

### 4. Vehicle Telemetry

Connected vehicles generate high-frequency streams for fleet management and safety:

```c
/* Driver behaviour scoring from accelerometer stream */
typedef struct {
    WelfordState accel_stats;
    int harsh_braking_count;
    int harsh_acceleration_count;
    double trip_distance_km;
} DriverProfile;

void process_acceleration(DriverProfile *dp, double accel_g) {
    welford_update(&dp->accel_stats, accel_g);
    
    if (accel_g < -0.4) {  /* Harsh braking threshold */
        dp->harsh_braking_count++;
    } else if (accel_g > 0.3) {  /* Harsh acceleration */
        dp->harsh_acceleration_count++;
    }
}

double compute_safety_score(DriverProfile *dp) {
    double events_per_km = (dp->harsh_braking_count + 
                           dp->harsh_acceleration_count) /
                          dp->trip_distance_km;
    return 100.0 - (events_per_km * 10.0);  /* Simplified scoring */
}
```

---

## 💻 Laboratory Exercises

### Exercise 1: Stream Processing Pipeline

Implement a complete sensor data processing pipeline with the following components:

**Requirements:**
- Circular buffer for sliding window statistics
- Exponential moving average (EMA) filter
- Welford's algorithm for running mean and variance
- Z-score based anomaly detection
- Command-line configuration for window size, EMA alpha and threshold

**File:** `src/exercise1.c`

**Testing:** `./exercise1 -w 10 -a 0.2 -t 2.5 < data/sensor_stream.txt`

### Exercise 2: MQTT Broker Simulation

Implement a simplified MQTT-style message broker with publish-subscribe functionality:

**Requirements:**
- Topic-based message routing
- Subscription management with exact matching
- Per-topic statistics (message count, value range, average)
- Sensor simulation with configurable noise and drift
- Callback mechanism for message delivery

**File:** `src/exercise2.c`

**Testing:** `./exercise2 < data/mqtt_messages.txt`

---

## 🔧 Compilation and Execution

```bash
# Build all targets
make

# Run complete demonstration
make run

# Run specific programmes
make run-example    # Run example1 only
make run-ex1        # Run exercise1 with test data
make run-ex2        # Run exercise2 with MQTT messages

# Run automated tests
make test

# Check for memory leaks
make valgrind

# Run in Docker container
make docker

# Build and run Docker interactively
docker build -t week-19-iot .
docker run --rm -it week-19-iot bash

# Clean build artifacts
make clean

# Display help
make help

# Show project information
make info
```

---

## 📁 Directory Structure

```
19-iot-stream-processing/
├── README.md                           # This documentation
├── Makefile                            # Build automation (305 lines)
├── Dockerfile                          # Container definition
│
├── slides/
│   ├── presentation-week19.html        # Main presentation (~40 slides)
│   └── presentation-comparativ.html    # Pseudocode/C/Python comparison
│
├── src/
│   ├── example1.c                      # Complete demonstration (950 lines)
│   ├── exercise1.c                     # Stream pipeline exercise
│   └── exercise2.c                     # MQTT broker exercise
│
├── data/
│   ├── sensor_stream.txt               # Temperature readings with anomalies
│   ├── sensor_anomalies.txt            # Data with multiple anomaly types
│   └── mqtt_messages.txt               # Sample MQTT-style messages
│
├── tests/
│   ├── test1_input.txt                 # Exercise 1 test input
│   ├── test1_expected.txt              # Exercise 1 expected output
│   ├── test2_input.txt                 # Exercise 2 test input
│   └── test2_expected.txt              # Exercise 2 expected output
│
├── python_comparison/
│   ├── circular_buffer.py              # Python circular buffer
│   ├── stream_stats.py                 # Welford's algorithm in Python
│   └── mqtt_simulation.py              # MQTT pattern demonstration
│
├── teme/
│   ├── homework-requirements.md        # Homework specifications
│   └── homework-extended.md            # Bonus challenges
│
└── solution/
    ├── exercise1_sol.c                 # Exercise 1 solution
    ├── exercise2_sol.c                 # Exercise 2 solution
    ├── homework1_sol.c                 # Homework 1 solution
    └── homework2_sol.c                 # Homework 2 solution
```

---

## 📖 Recommended Reading

### Essential

- Cormen, Leiserson, Rivest, Stein: *Introduction to Algorithms*, 4th ed., Chapter 4 (Divide-and-Conquer, recurrences) — foundational complexity analysis

- Muthukrishnan, S.: *Data Streams: Algorithms and Applications*, NOW Publishers, 2005 — seminal monograph on streaming models

### Advanced

- Welford, B.P.: "Note on a Method for Calculating Corrected Sums of Squares and Products," *Technometrics*, Vol. 4, No. 3, 1962 — original one-pass variance algorithm

- OASIS MQTT Version 5.0 Specification, 2019 — authoritative protocol reference

### Online Resources

- Stanford CS 246: Mining Massive Data Sets (streaming algorithms lectures)
  https://web.stanford.edu/class/cs246/

- MQTT.org — Protocol documentation and tutorials
  https://mqtt.org/

- InfluxDB Documentation — Time-series database concepts
  https://docs.influxdata.com/

---

## ✅ Self-Assessment Checklist

After completing this laboratory, verify that you can:

- [ ] Explain the three fundamental constraints of the streaming data model
- [ ] Implement a circular buffer with O(1) push, mean and oldest-element operations
- [ ] Derive the EMA recurrence relation and select appropriate α for different smoothing needs
- [ ] Apply Welford's algorithm and explain why it is numerically superior to the naïve variance formula
- [ ] Calculate z-scores and determine appropriate anomaly thresholds for given false-positive tolerances
- [ ] Design a topic-based publish-subscribe system with callback mechanisms
- [ ] Integrate multiple stream processing components into a coherent pipeline
- [ ] Analyse the memory and time complexity of sliding window algorithms
- [ ] Distinguish between scenarios suited for simple vs exponential moving averages
- [ ] Implement debouncing logic to avoid spurious anomaly alerts

---

## 💼 Interview Preparation

Common interview questions on stream processing and IoT:

1. **Algorithm Design**: "Design a system to detect credit card fraud in real-time from a stream of transactions."
   - Key insight: Combine user-specific baseline statistics with z-score anomaly detection; consider both amount and frequency patterns

2. **Memory Constraints**: "How would you find the median of a continuous stream of integers?"
   - Key insight: Two heaps approach (max-heap for lower half, min-heap for upper half) achieves O(log n) per element

3. **System Design**: "Design a real-time dashboard showing the top 10 most viewed pages in the last hour."
   - Key insight: Sliding window with Count-Min Sketch for frequency estimation, periodic heap extraction for top-k

4. **Trade-offs**: "When would you prefer EMA over SMA for smoothing sensor data?"
   - Key insight: EMA requires O(1) memory vs O(k) for SMA, responds faster to changes, but weights history differently

5. **IoT Architecture**: "How would you handle a sensor that intermittently sends duplicate or out-of-order readings?"
   - Key insight: Sequence numbers for deduplication, bounded reorder buffer with timeout, idempotent processing

---

## 🔗 Next Week Preview

**Week 20: Parallel and Concurrent Programming**

Having mastered stream processing algorithms, we advance to their parallel execution. Week 20 introduces POSIX threads (pthreads), mutex synchronisation, condition variables and lock-free data structures using C11 atomics. We shall implement parallel versions of stream processing pipelines, exploring the producer-consumer pattern and the Map-Reduce paradigm. The laboratory culminates in a multi-threaded anomaly detection system capable of processing multiple sensor streams concurrently.

---

## 🔌 Real Hardware Extension (Optional)

> **Note:** This section is for students who wish to experiment with physical hardware.
> Arduino kits (ESP32, Arduino Due) are available for borrowing from the faculty library.

The algorithms in this laboratory translate directly to embedded systems. An ESP32 microcontroller with a DHT22 temperature/humidity sensor provides an excellent platform for experimentation:

**Advantages over simulation:**
- Real sensor noise characteristics (not synthetic)
- Actual timing constraints and interrupt handling
- Experience with limited memory (520 KB SRAM)
- Wireless (WiFi/BLE) data transmission

**Getting started:**
1. Install Arduino IDE with ESP32 board support
2. Connect DHT22 to GPIO pin (3.3V logic)
3. Adapt circular buffer code for Arduino (use `static` allocation)
4. Implement MQTT client using PubSubClient library
5. Stream to a local Mosquitto broker or cloud service

The `solution/` directory includes `esp32_example.ino` demonstrating temperature streaming with on-device anomaly detection.

---

*Laboratory materials prepared for ATP Course*  
*Academy of Economic Studies - CSIE Bucharest*  
*Alternative learning kit for non-formal education*

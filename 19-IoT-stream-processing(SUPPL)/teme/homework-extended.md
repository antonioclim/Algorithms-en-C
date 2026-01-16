# Extended Challenges - Week 19

## 🚀 Advanced Challenges (Optional)

Each correctly solved challenge earns **+10 bonus points** (maximum +50 points).

These challenges are designed for students who wish to push beyond the core requirements and develop advanced skills in IoT systems, stream processing and real-time analytics.

---

## ⭐ Challenge 1: Adaptive Sampling with Change Detection (Difficulty: Medium)

### Description

In resource-constrained IoT devices, transmitting every reading wastes bandwidth and energy. Implement an adaptive sampling system that increases sampling rate when the signal changes rapidly and decreases it during stable periods.

### Requirements

- Implement change detection using CUSUM (Cumulative Sum) algorithm
- Dynamically adjust sampling interval based on signal dynamics
- Maintain minimum/maximum bounds on sampling rate
- Track bandwidth savings compared to fixed-rate sampling

### Algorithm: CUSUM

```
g_high = max(0, g_high + (x - μ - k))
g_low  = max(0, g_low + (μ - x - k))

If g_high > h or g_low > h:
    Change detected! Reset g_high, g_low
```

Where:
- `μ` = target mean
- `k` = allowable drift (slack)
- `h` = decision threshold

### Implementation Hints

```c
typedef struct {
    double g_high;          /* Upper CUSUM statistic */
    double g_low;           /* Lower CUSUM statistic */
    double target_mean;     /* Expected value */
    double k;               /* Allowable drift */
    double h;               /* Threshold */
    
    /* Adaptive sampling */
    int current_interval_ms;
    int min_interval_ms;
    int max_interval_ms;
    
    /* Statistics */
    size_t samples_taken;
    size_t changes_detected;
} AdaptiveSampler;

bool adaptive_sampler_update(AdaptiveSampler *as, double value);
int adaptive_sampler_get_interval(AdaptiveSampler *as);
double adaptive_sampler_bandwidth_savings(AdaptiveSampler *as);
```

### Testing Scenarios

1. Stable signal (should achieve >80% bandwidth savings)
2. Step change (should detect within 3 samples)
3. Gradual drift (should adapt sampling smoothly)
4. Oscillating signal (should maintain higher rate)

### Bonus Points: +10

---

## ⭐ Challenge 2: Multi-Sensor Kalman Filter (Difficulty: Hard)

### Description

Implement a Kalman filter for fusing noisy sensor readings from multiple sources. The Kalman filter optimally combines predictions with measurements, weighted by their uncertainties.

### Requirements

- Implement 1D Kalman filter for single-variable tracking
- Support multiple sensors with different noise characteristics
- Predict between measurements (dead reckoning)
- Visualise estimation vs ground truth

### Kalman Filter Equations

```
Prediction step:
  x_pred = F × x_prev + B × u
  P_pred = F × P_prev × F^T + Q

Update step:
  K = P_pred × H^T × (H × P_pred × H^T + R)^{-1}
  x = x_pred + K × (z - H × x_pred)
  P = (I - K × H) × P_pred
```

For 1D case (simplified):
```c
/* Prediction */
state = state + process_noise;
P = P + Q;

/* Update */
K = P / (P + R);           /* Kalman gain */
state = state + K * (measurement - state);
P = (1 - K) * P;
```

### Implementation Hints

```c
typedef struct {
    double state;           /* Current estimate */
    double P;               /* Estimate uncertainty */
    double Q;               /* Process noise */
    double R;               /* Measurement noise */
    size_t update_count;
} KalmanFilter1D;

void kalman_predict(KalmanFilter1D *kf, double dt);
void kalman_update(KalmanFilter1D *kf, double measurement, double sensor_R);
double kalman_get_estimate(KalmanFilter1D *kf);
double kalman_get_uncertainty(KalmanFilter1D *kf);
```

### Testing Scenarios

1. Single noisy sensor (compare with raw moving average)
2. Two sensors with different noise levels
3. Intermittent sensor (predict during gaps)
4. Sensor with drift (detect and handle)

### Bonus Points: +10

---

## ⭐ Challenge 3: Time-Series Compression (Difficulty: Medium-Hard)

### Description

IoT devices often need to store or transmit historical data efficiently. Implement a time-series compression algorithm that exploits temporal patterns while maintaining bounded error.

### Requirements

- Implement Swing Filter algorithm (or Dead-band compression)
- Guarantee maximum deviation from original values
- Support decompression to reconstruct signal
- Calculate compression ratio

### Algorithm: Swing Filter

The algorithm maintains upper and lower "swing" lines. A point is stored only when it cannot be reached by extending both lines from the last stored point.

```
┌────────────────────────────────┐
│    •                           │  Upper swing
│   /  \                         │
│  /    •                        │  Stored point
│ •──────╲─────────•─────        │
│          ╲      /              │
│           •────•               │  Lower swing
└────────────────────────────────┘
```

### Implementation Hints

```c
typedef struct {
    double *timestamps;
    double *values;
    size_t count;
    size_t capacity;
} CompressedSeries;

typedef struct {
    double max_error;           /* Maximum allowed deviation */
    
    /* Swing state */
    double last_stored_time;
    double last_stored_value;
    double upper_slope_max;
    double lower_slope_min;
    
    CompressedSeries output;
    size_t input_count;         /* For compression ratio */
} SwingFilter;

void swing_filter_add(SwingFilter *sf, double timestamp, double value);
double swing_filter_interpolate(SwingFilter *sf, double timestamp);
double swing_filter_compression_ratio(SwingFilter *sf);
double swing_filter_max_actual_error(SwingFilter *sf, double *original_times,
                                       double *original_values, size_t count);
```

### Testing Scenarios

1. Linear signal (should achieve very high compression)
2. Noisy signal (compression vs error trade-off)
3. Step changes (should capture transitions accurately)
4. Real sensor data (if available)

### Bonus Points: +10

---

## ⭐ Challenge 4: Distributed Sensor Consensus (Difficulty: Hard)

### Description

In distributed IoT networks, sensors must agree on a value without a central coordinator. Implement a consensus algorithm where each node only communicates with its neighbours.

### Requirements

- Implement average consensus protocol
- Support arbitrary network topology (as adjacency matrix)
- Detect and handle Byzantine (malicious) nodes
- Converge to true average within tolerance

### Algorithm: Iterative Average Consensus

```
At each iteration, node i updates:
  x_i(t+1) = x_i(t) + ε × Σ (x_j(t) - x_i(t))  for all neighbours j

Where ε = step size (typically 1 / max_degree)
```

### Implementation Hints

```c
typedef struct {
    double *values;             /* Current node values */
    double *next_values;        /* Buffer for synchronous update */
    int **adjacency;            /* Adjacency matrix */
    size_t node_count;
    double epsilon;             /* Step size */
    size_t iteration;
} ConsensusNetwork;

void consensus_init(ConsensusNetwork *cn, double *initial_values, size_t n);
void consensus_set_topology(ConsensusNetwork *cn, int **adj);
void consensus_iterate(ConsensusNetwork *cn);
double consensus_get_max_deviation(ConsensusNetwork *cn);
bool consensus_has_converged(ConsensusNetwork *cn, double tolerance);

/* Byzantine fault tolerance */
void consensus_mark_byzantine(ConsensusNetwork *cn, size_t node_idx);
double consensus_robust_mean(ConsensusNetwork *cn);  /* Excludes outliers */
```

### Network Topologies to Test

```
Ring:         Star:         Mesh:
  1─2           1              1─2
 /   \         /|\            /|X|\
4     3       2 3 4          3─4─5
 \   /         \|/
  ───           5
```

### Testing Scenarios

1. Ring topology (slow convergence)
2. Fully connected (fast convergence)
3. One Byzantine node (should still converge to correct value)
4. Network partition (should detect non-convergence)

### Bonus Points: +10

---

## ⭐ Challenge 5: Edge ML Inference Engine (Difficulty: Very Hard)

### Description

Implement a minimal neural network inference engine for edge devices. Focus on efficient fixed-point arithmetic and minimal memory footprint.

### Requirements

- Support dense (fully connected) layers
- Implement ReLU and sigmoid activations
- Use 8-bit quantised weights
- Demonstrate on a simple classification task

### Network Architecture

```
Input (N features)
    │
    ▼
Dense Layer 1 (8 neurons, ReLU)
    │
    ▼
Dense Layer 2 (4 neurons, ReLU)
    │
    ▼
Output Layer (C classes, Softmax)
```

### Quantisation Approach

```
Quantised value: q = round(x / scale)
Dequantised: x ≈ q × scale

For 8-bit: q ∈ [-128, 127]
scale = (max_val - min_val) / 255
```

### Implementation Hints

```c
typedef struct {
    int8_t *weights;            /* Quantised weights */
    int32_t *bias;              /* 32-bit bias (accumulated) */
    size_t input_size;
    size_t output_size;
    float weight_scale;
    float bias_scale;
} QuantisedDenseLayer;

typedef struct {
    QuantisedDenseLayer *layers;
    size_t layer_count;
    int32_t *activations;       /* Working buffer */
    size_t max_layer_size;
} TinyNeuralNet;

void tnn_forward(TinyNeuralNet *nn, const int8_t *input, int8_t *output);
int tnn_classify(TinyNeuralNet *nn, const float *input);
size_t tnn_memory_usage(TinyNeuralNet *nn);

/* Quantisation utilities */
int8_t quantise_value(float x, float scale, float zero_point);
float dequantise_value(int8_t q, float scale, float zero_point);
```

### Test Task: XOR Classification

Train (externally) a network to classify XOR:
```
Input: [0,0] → Output: 0
Input: [0,1] → Output: 1
Input: [1,0] → Output: 1
Input: [1,1] → Output: 0
```

### Testing Scenarios

1. XOR classification (100% accuracy expected)
2. Simple sensor anomaly detection (normal vs anomaly)
3. Memory usage analysis (compare with float implementation)
4. Inference speed benchmark

### Bonus Points: +10

---

## 📊 Bonus Point System

| Challenges Completed | Total Bonus | Special Recognition |
|---------------------|-------------|---------------------|
| 1 challenge | +10 points | — |
| 2 challenges | +20 points | — |
| 3 challenges | +30 points | "Stream Processing Expert" badge |
| 4 challenges | +40 points | "Stream Processing Expert" badge |
| All 5 challenges | +50 points | "IoT Systems Master" badge 🏆 |

### Badge Benefits

Students earning badges receive:
- Recognition in the course hall of fame
- Priority consideration for research assistant positions
- Letter of recommendation upon request

---

## 📋 Submission Guidelines for Challenges

1. **Separate submission** from regular homework
2. **Include README** explaining your approach and design decisions
3. **Provide test cases** demonstrating functionality
4. **Comment complex algorithms** thoroughly
5. **Include complexity analysis** for key functions

### File Naming

```
challenge1_adaptive_sampling.c
challenge2_kalman_filter.c
challenge3_time_compression.c
challenge4_consensus.c
challenge5_tiny_nn.c
```

### Documentation Requirements

For each challenge, include:
- Algorithm explanation with mathematical background
- Complexity analysis (time and space)
- Test results with graphs if applicable
- Discussion of trade-offs and limitations

---

## 💡 General Tips

1. **Start with the theory** — Understand the mathematical foundations before coding. Challenge 2 (Kalman) and Challenge 4 (Consensus) require solid linear algebra understanding.

2. **Implement incrementally** — For Challenge 5, start with float arithmetic and verify correctness before quantising.

3. **Test edge cases** — What happens with zero variance? Empty input? Network partition?

4. **Profile your code** — These algorithms often run on resource-constrained devices. Memory and CPU efficiency matter.

5. **Use visualisation** — Plotting your results (even ASCII plots) helps debug and understand algorithm behaviour.

6. **Read the papers** — Original algorithm papers often contain implementation insights:
   - CUSUM: Page (1954)
   - Kalman Filter: Kalman (1960)
   - Swing Filter: Srivastava et al. (2005)
   - Consensus: Olfati-Saber & Murray (2004)

---

## 🔬 Additional Resources

### For Challenge 1 (Adaptive Sampling)
- E.S. Page, "Continuous Inspection Schemes", Biometrika 1954
- Douglas-Peucker algorithm (alternative approach)

### For Challenge 2 (Kalman Filter)
- Welch & Bishop, "An Introduction to the Kalman Filter", UNC-Chapel Hill
- Interactive Kalman Filter demonstration: https://www.kalmanfilter.net/

### For Challenge 3 (Compression)
- Elmeleegy et al., "Online Piece-wise Linear Approximation of Numerical Streams with Precision Guarantees", VLDB 2009

### For Challenge 4 (Consensus)
- Olfati-Saber, "Consensus and Cooperation in Networked Multi-Agent Systems", Proceedings of the IEEE 2007

### For Challenge 5 (Edge ML)
- TensorFlow Lite Micro documentation
- CMSIS-NN library for ARM Cortex-M

---

Good luck, and remember: these challenges represent cutting-edge IoT techniques used in industry! 🚀

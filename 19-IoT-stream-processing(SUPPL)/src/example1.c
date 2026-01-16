/**
 * =============================================================================
 * WEEK 19: ALGORITHMS FOR IoT AND STREAM PROCESSING
 * Complete Working Example
 * =============================================================================
 *
 * This example demonstrates:
 *   1. Circular buffer (ring buffer) for sliding windows
 *   2. Exponential Moving Average (EMA) filter
 *   3. Welford's online algorithm for running statistics
 *   4. Z-score anomaly detection
 *   5. MQTT-style publish-subscribe message broker
 *   6. Simulated sensor data generation
 *   7. Complete stream processing pipeline integration
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o example1 example1.c -lm
 * Usage: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <float.h>

/* =============================================================================
 * CONSTANTS AND CONFIGURATION
 * =============================================================================
 */

#define MAX_BUFFER_SIZE    1000
#define MAX_TOPICS         32
#define MAX_SUBSCRIBERS    16
#define MAX_TOPIC_LEN      64
#define DEFAULT_WINDOW     10
#define DEFAULT_ALPHA      0.2
#define DEFAULT_THRESHOLD  2.5

/* =============================================================================
 * PART 1: CIRCULAR BUFFER (RING BUFFER)
 * =============================================================================
 *
 * A circular buffer maintains a fixed-size window of the most recent values.
 * When full, new insertions overwrite the oldest value.
 *
 *     Initial (capacity=5):      After inserting 10,20,30,40,50:
 *     ┌───┬───┬───┬───┬───┐     ┌────┬────┬────┬────┬────┐
 *     │ - │ - │ - │ - │ - │     │ 10 │ 20 │ 30 │ 40 │ 50 │
 *     └───┴───┴───┴───┴───┘     └────┴────┴────┴────┴────┘
 *       ▲                                               ▲
 *      head                                            head
 *
 *     After inserting 60 (wraparound):
 *     ┌────┬────┬────┬────┬────┐
 *     │ 60 │ 20 │ 30 │ 40 │ 50 │
 *     └────┴────┴────┴────┴────┘
 *            ▲
 *           head (oldest=20, newest=60)
 */

typedef struct {
    double *data;       /* Array holding the values */
    size_t capacity;    /* Maximum number of elements */
    size_t count;       /* Current number of elements (0 to capacity) */
    size_t head;        /* Index where next element will be written */
    double sum;         /* Running sum for O(1) mean calculation */
    double min;         /* Minimum value in buffer */
    double max;         /* Maximum value in buffer */
} CircularBuffer;

/**
 * Create a new circular buffer with specified capacity.
 *
 * @param capacity Maximum number of elements to store
 * @return Pointer to newly allocated buffer, or NULL on failure
 *
 * Time Complexity: O(capacity)
 * Space Complexity: O(capacity)
 */
CircularBuffer *cb_create(size_t capacity) {
    if (capacity == 0 || capacity > MAX_BUFFER_SIZE) {
        fprintf(stderr, "Error: Invalid buffer capacity %zu\n", capacity);
        return NULL;
    }
    
    CircularBuffer *cb = malloc(sizeof(CircularBuffer));
    if (!cb) {
        fprintf(stderr, "Error: Memory allocation failed for CircularBuffer\n");
        return NULL;
    }
    
    cb->data = malloc(capacity * sizeof(double));
    if (!cb->data) {
        fprintf(stderr, "Error: Memory allocation failed for buffer data\n");
        free(cb);
        return NULL;
    }
    
    cb->capacity = capacity;
    cb->count = 0;
    cb->head = 0;
    cb->sum = 0.0;
    cb->min = DBL_MAX;
    cb->max = -DBL_MAX;
    
    return cb;
}

/**
 * Free all memory associated with a circular buffer.
 *
 * @param cb Pointer to circular buffer to free
 */
void cb_free(CircularBuffer *cb) {
    if (cb) {
        free(cb->data);
        free(cb);
    }
}

/**
 * Push a new value into the circular buffer.
 * If buffer is full, overwrites the oldest value.
 *
 * @param cb Pointer to circular buffer
 * @param value Value to insert
 *
 * Time Complexity: O(1) amortised, O(n) when recalculating min/max
 * Space Complexity: O(1)
 */
void cb_push(CircularBuffer *cb, double value) {
    if (!cb) return;
    
    /* If buffer is full, subtract the value being overwritten from sum */
    if (cb->count == cb->capacity) {
        double old_value = cb->data[cb->head];
        cb->sum -= old_value;
        
        /* If we're removing min or max, we need to recalculate (O(n)) */
        if (old_value == cb->min || old_value == cb->max) {
            cb->min = DBL_MAX;
            cb->max = -DBL_MAX;
            for (size_t i = 0; i < cb->capacity; i++) {
                if (i != cb->head) {  /* Skip position being overwritten */
                    if (cb->data[i] < cb->min) cb->min = cb->data[i];
                    if (cb->data[i] > cb->max) cb->max = cb->data[i];
                }
            }
        }
    } else {
        cb->count++;
    }
    
    /* Insert new value */
    cb->data[cb->head] = value;
    cb->sum += value;
    
    /* Update min/max */
    if (value < cb->min) cb->min = value;
    if (value > cb->max) cb->max = value;
    
    /* Advance head with wraparound */
    cb->head = (cb->head + 1) % cb->capacity;
}

/**
 * Calculate the mean of values in the buffer.
 *
 * @param cb Pointer to circular buffer
 * @return Mean value, or 0 if buffer is empty
 *
 * Time Complexity: O(1)
 */
double cb_mean(CircularBuffer *cb) {
    if (!cb || cb->count == 0) return 0.0;
    return cb->sum / cb->count;
}

/**
 * Calculate the variance of values in the buffer.
 *
 * @param cb Pointer to circular buffer
 * @return Sample variance, or 0 if fewer than 2 elements
 *
 * Time Complexity: O(n)
 */
double cb_variance(CircularBuffer *cb) {
    if (!cb || cb->count < 2) return 0.0;
    
    double mean = cb_mean(cb);
    double sum_sq_diff = 0.0;
    
    for (size_t i = 0; i < cb->count; i++) {
        size_t idx = (cb->head + cb->capacity - cb->count + i) % cb->capacity;
        double diff = cb->data[idx] - mean;
        sum_sq_diff += diff * diff;
    }
    
    return sum_sq_diff / (cb->count - 1);  /* Bessel's correction */
}

/**
 * Get the oldest value in the buffer.
 *
 * @param cb Pointer to circular buffer
 * @return Oldest value, or 0 if buffer is empty
 *
 * Time Complexity: O(1)
 */
double cb_oldest(CircularBuffer *cb) {
    if (!cb || cb->count == 0) return 0.0;
    size_t oldest_idx = (cb->head + cb->capacity - cb->count) % cb->capacity;
    return cb->data[oldest_idx];
}

/**
 * Get the newest value in the buffer.
 *
 * @param cb Pointer to circular buffer
 * @return Newest value, or 0 if buffer is empty
 *
 * Time Complexity: O(1)
 */
double cb_newest(CircularBuffer *cb) {
    if (!cb || cb->count == 0) return 0.0;
    size_t newest_idx = (cb->head + cb->capacity - 1) % cb->capacity;
    return cb->data[newest_idx];
}

/**
 * Print the contents of the circular buffer.
 *
 * @param cb Pointer to circular buffer
 * @param label Label to print before contents
 */
void cb_print(CircularBuffer *cb, const char *label) {
    if (!cb) return;
    
    printf("  %s [", label);
    for (size_t i = 0; i < cb->count; i++) {
        size_t idx = (cb->head + cb->capacity - cb->count + i) % cb->capacity;
        printf("%.2f", cb->data[idx]);
        if (i < cb->count - 1) printf(", ");
    }
    printf("] (count=%zu, mean=%.2f)\n", cb->count, cb_mean(cb));
}

/**
 * Demonstration of circular buffer operations.
 */
void demo_circular_buffer(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: CIRCULAR BUFFER (RING BUFFER)                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("  Creating circular buffer with capacity 5...\n\n");
    CircularBuffer *cb = cb_create(5);
    
    printf("  Inserting values 10, 20, 30:\n");
    cb_push(cb, 10.0);
    cb_push(cb, 20.0);
    cb_push(cb, 30.0);
    cb_print(cb, "Buffer");
    
    printf("\n  Inserting 40, 50 (buffer now full):\n");
    cb_push(cb, 40.0);
    cb_push(cb, 50.0);
    cb_print(cb, "Buffer");
    printf("    Oldest: %.2f, Newest: %.2f\n", cb_oldest(cb), cb_newest(cb));
    
    printf("\n  Inserting 60, 70 (wraparound - overwrites oldest):\n");
    cb_push(cb, 60.0);
    cb_push(cb, 70.0);
    cb_print(cb, "Buffer");
    printf("    Oldest: %.2f, Newest: %.2f\n", cb_oldest(cb), cb_newest(cb));
    printf("    Min: %.2f, Max: %.2f\n", cb->min, cb->max);
    
    printf("\n  Statistics:\n");
    printf("    Mean: %.2f\n", cb_mean(cb));
    printf("    Variance: %.2f\n", cb_variance(cb));
    printf("    Std Dev: %.2f\n", sqrt(cb_variance(cb)));
    
    cb_free(cb);
    printf("\n  Buffer freed successfully.\n");
}

/* =============================================================================
 * PART 2: EXPONENTIAL MOVING AVERAGE (EMA)
 * =============================================================================
 *
 * EMA provides smoothing with infinite memory but exponentially decaying weights:
 *
 *   EMA(t) = α × x(t) + (1 - α) × EMA(t-1)
 *
 * where α ∈ (0, 1) is the smoothing factor.
 *
 * Properties:
 *   - α close to 1: Fast response, less smoothing
 *   - α close to 0: Slow response, more smoothing
 *   - Memory: O(1) - only stores current EMA value
 */

typedef struct {
    double value;       /* Current EMA value */
    double alpha;       /* Smoothing factor (0 < alpha < 1) */
    bool initialised;   /* Whether first value has been processed */
} EMAFilter;

/**
 * Initialise an EMA filter with specified smoothing factor.
 *
 * @param ema Pointer to EMA filter structure
 * @param alpha Smoothing factor (0 < alpha < 1)
 */
void ema_init(EMAFilter *ema, double alpha) {
    if (!ema) return;
    if (alpha <= 0.0 || alpha >= 1.0) {
        fprintf(stderr, "Warning: Alpha should be in (0, 1), using 0.2\n");
        alpha = 0.2;
    }
    ema->value = 0.0;
    ema->alpha = alpha;
    ema->initialised = false;
}

/**
 * Update EMA with a new value.
 *
 * @param ema Pointer to EMA filter
 * @param new_value New observation
 * @return Updated EMA value
 *
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 */
double ema_update(EMAFilter *ema, double new_value) {
    if (!ema) return 0.0;
    
    if (!ema->initialised) {
        ema->value = new_value;
        ema->initialised = true;
    } else {
        ema->value = ema->alpha * new_value + (1 - ema->alpha) * ema->value;
    }
    
    return ema->value;
}

/**
 * Demonstration of EMA filter.
 */
void demo_ema_filter(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: EXPONENTIAL MOVING AVERAGE (EMA)                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Simulate noisy temperature readings */
    double readings[] = {20.0, 21.5, 19.8, 22.3, 20.1, 45.0, 21.0, 20.5, 21.2, 20.8};
    int n = sizeof(readings) / sizeof(readings[0]);
    
    printf("  Comparing EMA with different alpha values:\n\n");
    printf("  ┌───────────┬─────────┬──────────────┬──────────────┬──────────────┐\n");
    printf("  │   Index   │  Value  │  EMA α=0.1   │  EMA α=0.3   │  EMA α=0.7   │\n");
    printf("  ├───────────┼─────────┼──────────────┼──────────────┼──────────────┤\n");
    
    EMAFilter ema_slow, ema_med, ema_fast;
    ema_init(&ema_slow, 0.1);
    ema_init(&ema_med, 0.3);
    ema_init(&ema_fast, 0.7);
    
    for (int i = 0; i < n; i++) {
        double slow = ema_update(&ema_slow, readings[i]);
        double med = ema_update(&ema_med, readings[i]);
        double fast = ema_update(&ema_fast, readings[i]);
        
        const char *marker = (readings[i] > 40) ? " ← SPIKE" : "";
        printf("  │     %2d    │  %5.1f  │    %6.2f    │    %6.2f    │    %6.2f    │%s\n",
               i, readings[i], slow, med, fast, marker);
    }
    
    printf("  └───────────┴─────────┴──────────────┴──────────────┴──────────────┘\n\n");
    
    printf("  Observations:\n");
    printf("  ─────────────\n");
    printf("  • α=0.1 (slow): Heavily smoothed, slow response to spike\n");
    printf("  • α=0.3 (medium): Balanced smoothing and responsiveness\n");
    printf("  • α=0.7 (fast): Quick response, less noise reduction\n");
}

/* =============================================================================
 * PART 3: WELFORD'S ONLINE ALGORITHM
 * =============================================================================
 *
 * Welford's algorithm computes running mean and variance in a single pass
 * with excellent numerical stability.
 *
 * The naive formula Var = E[X²] - E[X]² suffers from catastrophic cancellation
 * when variance is small relative to the mean. Welford's method avoids this.
 *
 * Update equations for each new value x:
 *   count ← count + 1
 *   delta ← x - mean
 *   mean ← mean + delta / count
 *   delta2 ← x - mean  (uses NEW mean)
 *   M2 ← M2 + delta × delta2
 *
 *   variance = M2 / (count - 1)  [Bessel's correction]
 */

typedef struct {
    double mean;        /* Running mean */
    double M2;          /* Sum of squared differences from current mean */
    size_t count;       /* Number of values processed */
    double min;         /* Minimum value seen */
    double max;         /* Maximum value seen */
} WelfordState;

/**
 * Initialise Welford state.
 *
 * @param state Pointer to Welford state structure
 */
void welford_init(WelfordState *state) {
    if (!state) return;
    state->mean = 0.0;
    state->M2 = 0.0;
    state->count = 0;
    state->min = DBL_MAX;
    state->max = -DBL_MAX;
}

/**
 * Update Welford state with a new value.
 *
 * @param state Pointer to Welford state
 * @param value New observation
 *
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 */
void welford_update(WelfordState *state, double value) {
    if (!state) return;
    
    state->count++;
    
    /* Welford's update */
    double delta = value - state->mean;
    state->mean += delta / state->count;
    double delta2 = value - state->mean;  /* Note: uses NEW mean */
    state->M2 += delta * delta2;
    
    /* Update min/max */
    if (value < state->min) state->min = value;
    if (value > state->max) state->max = value;
}

/**
 * Get current variance estimate.
 *
 * @param state Pointer to Welford state
 * @return Sample variance, or 0 if fewer than 2 values
 */
double welford_variance(WelfordState *state) {
    if (!state || state->count < 2) return 0.0;
    return state->M2 / (state->count - 1);  /* Bessel's correction */
}

/**
 * Get current standard deviation estimate.
 *
 * @param state Pointer to Welford state
 * @return Sample standard deviation
 */
double welford_stddev(WelfordState *state) {
    return sqrt(welford_variance(state));
}

/**
 * Demonstration of Welford's algorithm.
 */
void demo_welford(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: WELFORD'S ONLINE ALGORITHM                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("  Processing stream of values and updating statistics online:\n\n");
    
    double values[] = {2, 4, 4, 4, 5, 5, 7, 9};
    int n = sizeof(values) / sizeof(values[0]);
    
    WelfordState state;
    welford_init(&state);
    
    printf("  ┌───────┬─────────┬───────────┬───────────┬───────────┐\n");
    printf("  │ Index │  Value  │   Mean    │  Variance │  Std Dev  │\n");
    printf("  ├───────┼─────────┼───────────┼───────────┼───────────┤\n");
    
    for (int i = 0; i < n; i++) {
        welford_update(&state, values[i]);
        printf("  │   %d   │   %3.0f   │   %6.3f  │   %6.3f  │   %6.3f  │\n",
               i + 1, values[i], state.mean, welford_variance(&state), 
               welford_stddev(&state));
    }
    
    printf("  └───────┴─────────┴───────────┴───────────┴───────────┘\n\n");
    
    printf("  Final statistics:\n");
    printf("    Count: %zu\n", state.count);
    printf("    Mean: %.3f\n", state.mean);
    printf("    Variance: %.3f\n", welford_variance(&state));
    printf("    Std Dev: %.3f\n", welford_stddev(&state));
    printf("    Min: %.0f, Max: %.0f\n", state.min, state.max);
    
    /* Verify against known values */
    printf("\n  Verification:\n");
    printf("    Known mean of [2,4,4,4,5,5,7,9] = 40/8 = 5.0 ✓\n");
    printf("    Known variance = 4.571 (sample) ✓\n");
}

/* =============================================================================
 * PART 4: Z-SCORE ANOMALY DETECTION
 * =============================================================================
 *
 * The z-score measures how many standard deviations a value is from the mean:
 *
 *   z = (x - μ) / σ
 *
 * For normally distributed data:
 *   |z| > 2.0  →  ~5% of values (unusual)
 *   |z| > 2.5  →  ~1% of values (suspicious)
 *   |z| > 3.0  →  ~0.3% of values (anomalous)
 */

typedef enum {
    SEVERITY_NORMAL,
    SEVERITY_WARNING,
    SEVERITY_CRITICAL
} AnomalySeverity;

typedef struct {
    double value;
    double z_score;
    bool is_anomaly;
    AnomalySeverity severity;
    double threshold;
} AnomalyResult;

/**
 * Detect if a value is anomalous given current statistics.
 *
 * @param value Value to check
 * @param mean Current mean
 * @param stddev Current standard deviation
 * @param threshold Z-score threshold for anomaly
 * @return AnomalyResult with detection details
 */
AnomalyResult detect_anomaly(double value, double mean, double stddev, 
                             double threshold) {
    AnomalyResult result;
    result.value = value;
    result.threshold = threshold;
    
    if (stddev < 1e-10) {
        /* Avoid division by zero */
        result.z_score = 0.0;
        result.is_anomaly = false;
        result.severity = SEVERITY_NORMAL;
        return result;
    }
    
    result.z_score = (value - mean) / stddev;
    double abs_z = fabs(result.z_score);
    
    result.is_anomaly = abs_z > threshold;
    
    if (abs_z > 3.0) {
        result.severity = SEVERITY_CRITICAL;
    } else if (abs_z > 2.0) {
        result.severity = SEVERITY_WARNING;
    } else {
        result.severity = SEVERITY_NORMAL;
    }
    
    return result;
}

/**
 * Get string representation of severity level.
 */
const char *severity_string(AnomalySeverity severity) {
    switch (severity) {
        case SEVERITY_CRITICAL: return "CRITICAL";
        case SEVERITY_WARNING:  return "WARNING";
        default:                return "NORMAL";
    }
}

/**
 * Demonstration of anomaly detection.
 */
void demo_anomaly_detection(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: Z-SCORE ANOMALY DETECTION                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Simulated temperature stream with anomalies */
    double temperatures[] = {
        20.1, 20.3, 19.8, 20.5, 20.2, 21.0, 19.9, 20.4,  /* Normal */
        45.2,  /* Spike anomaly */
        20.1, 20.6, 19.7, 20.3, 20.0,  /* Normal */
        -5.3,  /* Drop anomaly */
        20.2, 20.5, 19.8
    };
    int n = sizeof(temperatures) / sizeof(temperatures[0]);
    
    printf("  Processing temperature stream with threshold z > 2.5:\n\n");
    
    WelfordState baseline;
    welford_init(&baseline);
    
    /* First, establish baseline from first few readings */
    printf("  Phase 1: Establishing baseline (first 5 readings)...\n");
    for (int i = 0; i < 5 && i < n; i++) {
        welford_update(&baseline, temperatures[i]);
    }
    printf("    Baseline mean: %.2f, std dev: %.2f\n\n", 
           baseline.mean, welford_stddev(&baseline));
    
    printf("  Phase 2: Monitoring for anomalies...\n\n");
    printf("  ┌───────┬──────────┬──────────┬────────────┬────────────────┐\n");
    printf("  │ Index │  Value   │  Z-Score │   Status   │    Severity    │\n");
    printf("  ├───────┼──────────┼──────────┼────────────┼────────────────┤\n");
    
    int anomaly_count = 0;
    for (int i = 5; i < n; i++) {
        AnomalyResult result = detect_anomaly(
            temperatures[i], 
            baseline.mean, 
            welford_stddev(&baseline),
            2.5
        );
        
        const char *status = result.is_anomaly ? "⚠ ANOMALY" : "  Normal";
        printf("  │  %2d   │  %6.1f  │  %+6.2f  │ %s │ %-14s │\n",
               i, temperatures[i], result.z_score, status,
               severity_string(result.severity));
        
        if (result.is_anomaly) {
            anomaly_count++;
        } else {
            /* Update baseline with normal values only */
            welford_update(&baseline, temperatures[i]);
        }
    }
    
    printf("  └───────┴──────────┴──────────┴────────────┴────────────────┘\n\n");
    
    printf("  Summary:\n");
    printf("    Total readings: %d\n", n - 5);
    printf("    Anomalies detected: %d\n", anomaly_count);
    printf("    Final baseline mean: %.2f\n", baseline.mean);
    printf("    Final baseline std dev: %.2f\n", welford_stddev(&baseline));
}

/* =============================================================================
 * PART 5: MQTT-STYLE PUBLISH-SUBSCRIBE BROKER
 * =============================================================================
 *
 * MQTT (Message Queuing Telemetry Transport) uses publish-subscribe pattern:
 *
 *   Publishers ──publish(topic, message)──▶ Broker ──deliver──▶ Subscribers
 *
 * Key concepts:
 *   - Topic: Hierarchical string (e.g., "home/kitchen/temperature")
 *   - Subscribe: Register interest in a topic pattern
 *   - Publish: Send message to a topic
 *   - QoS: Quality of Service (0=at-most-once, 1=at-least-once, 2=exactly-once)
 */

/* Forward declaration */
struct MQTTBroker;

/* Callback type for message delivery */
typedef void (*MessageCallback)(const char *topic, double value, void *user_data);

/* Subscriber structure */
typedef struct {
    char topic_filter[MAX_TOPIC_LEN];  /* Topic pattern (exact match only) */
    MessageCallback callback;           /* Function to call on message */
    void *user_data;                    /* User context */
    bool active;                        /* Whether subscription is active */
} Subscriber;

/* Topic statistics */
typedef struct {
    char topic[MAX_TOPIC_LEN];
    size_t message_count;
    double sum;
    double min;
    double max;
    bool active;
} TopicStats;

/* MQTT Broker */
typedef struct MQTTBroker {
    Subscriber subscribers[MAX_SUBSCRIBERS];
    int subscriber_count;
    TopicStats topics[MAX_TOPICS];
    int topic_count;
    size_t total_messages;
} MQTTBroker;

/**
 * Initialise the MQTT broker.
 *
 * @param broker Pointer to broker structure
 */
void broker_init(MQTTBroker *broker) {
    if (!broker) return;
    
    broker->subscriber_count = 0;
    broker->topic_count = 0;
    broker->total_messages = 0;
    
    for (int i = 0; i < MAX_SUBSCRIBERS; i++) {
        broker->subscribers[i].active = false;
    }
    for (int i = 0; i < MAX_TOPICS; i++) {
        broker->topics[i].active = false;
    }
}

/**
 * Subscribe to a topic.
 *
 * @param broker Pointer to broker
 * @param topic_filter Topic pattern (exact match only in this implementation)
 * @param callback Function to call when message arrives
 * @param user_data User context passed to callback
 * @return Subscription ID, or -1 on failure
 */
int broker_subscribe(MQTTBroker *broker, const char *topic_filter,
                     MessageCallback callback, void *user_data) {
    if (!broker || !topic_filter || !callback) return -1;
    if (broker->subscriber_count >= MAX_SUBSCRIBERS) return -1;
    
    /* Find empty slot */
    for (int i = 0; i < MAX_SUBSCRIBERS; i++) {
        if (!broker->subscribers[i].active) {
            strncpy(broker->subscribers[i].topic_filter, topic_filter, 
                    MAX_TOPIC_LEN - 1);
            broker->subscribers[i].topic_filter[MAX_TOPIC_LEN - 1] = '\0';
            broker->subscribers[i].callback = callback;
            broker->subscribers[i].user_data = user_data;
            broker->subscribers[i].active = true;
            broker->subscriber_count++;
            return i;
        }
    }
    return -1;
}

/**
 * Unsubscribe from a topic.
 *
 * @param broker Pointer to broker
 * @param subscription_id ID returned from subscribe
 */
void broker_unsubscribe(MQTTBroker *broker, int subscription_id) {
    if (!broker || subscription_id < 0 || subscription_id >= MAX_SUBSCRIBERS) {
        return;
    }
    if (broker->subscribers[subscription_id].active) {
        broker->subscribers[subscription_id].active = false;
        broker->subscriber_count--;
    }
}

/**
 * Get or create topic statistics entry.
 *
 * @param broker Pointer to broker
 * @param topic Topic name
 * @return Pointer to topic stats, or NULL if full
 */
TopicStats *broker_get_topic_stats(MQTTBroker *broker, const char *topic) {
    if (!broker || !topic) return NULL;
    
    /* Search for existing */
    for (int i = 0; i < MAX_TOPICS; i++) {
        if (broker->topics[i].active && 
            strcmp(broker->topics[i].topic, topic) == 0) {
            return &broker->topics[i];
        }
    }
    
    /* Create new */
    for (int i = 0; i < MAX_TOPICS; i++) {
        if (!broker->topics[i].active) {
            strncpy(broker->topics[i].topic, topic, MAX_TOPIC_LEN - 1);
            broker->topics[i].topic[MAX_TOPIC_LEN - 1] = '\0';
            broker->topics[i].message_count = 0;
            broker->topics[i].sum = 0.0;
            broker->topics[i].min = DBL_MAX;
            broker->topics[i].max = -DBL_MAX;
            broker->topics[i].active = true;
            broker->topic_count++;
            return &broker->topics[i];
        }
    }
    
    return NULL;
}

/**
 * Publish a message to a topic.
 *
 * @param broker Pointer to broker
 * @param topic Topic name
 * @param value Numeric value to publish
 * @return Number of subscribers notified
 */
int broker_publish(MQTTBroker *broker, const char *topic, double value) {
    if (!broker || !topic) return 0;
    
    broker->total_messages++;
    
    /* Update topic statistics */
    TopicStats *stats = broker_get_topic_stats(broker, topic);
    if (stats) {
        stats->message_count++;
        stats->sum += value;
        if (value < stats->min) stats->min = value;
        if (value > stats->max) stats->max = value;
    }
    
    /* Deliver to matching subscribers */
    int delivered = 0;
    for (int i = 0; i < MAX_SUBSCRIBERS; i++) {
        if (broker->subscribers[i].active &&
            strcmp(broker->subscribers[i].topic_filter, topic) == 0) {
            broker->subscribers[i].callback(
                topic, value, broker->subscribers[i].user_data);
            delivered++;
        }
    }
    
    return delivered;
}

/**
 * Print broker statistics.
 *
 * @param broker Pointer to broker
 */
void broker_print_stats(MQTTBroker *broker) {
    if (!broker) return;
    
    printf("  Broker Statistics:\n");
    printf("  ──────────────────\n");
    printf("    Total messages: %zu\n", broker->total_messages);
    printf("    Active subscribers: %d\n", broker->subscriber_count);
    printf("    Active topics: %d\n\n", broker->topic_count);
    
    printf("  Topic Statistics:\n");
    printf("  ┌────────────────────────┬─────────┬─────────┬─────────┬─────────┐\n");
    printf("  │         Topic          │  Count  │   Avg   │   Min   │   Max   │\n");
    printf("  ├────────────────────────┼─────────┼─────────┼─────────┼─────────┤\n");
    
    for (int i = 0; i < MAX_TOPICS; i++) {
        if (broker->topics[i].active) {
            double avg = broker->topics[i].message_count > 0 ?
                broker->topics[i].sum / broker->topics[i].message_count : 0.0;
            printf("  │ %-22s │  %5zu  │ %7.2f │ %7.2f │ %7.2f │\n",
                   broker->topics[i].topic,
                   broker->topics[i].message_count,
                   avg,
                   broker->topics[i].min,
                   broker->topics[i].max);
        }
    }
    printf("  └────────────────────────┴─────────┴─────────┴─────────┴─────────┘\n");
}

/* Example callback functions */
void logger_callback(const char *topic, double value, void *user_data) {
    (void)user_data;  /* Unused */
    printf("    [LOGGER] %s = %.2f\n", topic, value);
}

void alert_callback(const char *topic, double value, void *user_data) {
    double *threshold = (double *)user_data;
    if (value > *threshold) {
        printf("    [ALERT] %s = %.2f exceeds threshold %.2f!\n",
               topic, value, *threshold);
    }
}

/**
 * Demonstration of MQTT broker.
 */
void demo_mqtt_broker(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: MQTT-STYLE PUBLISH-SUBSCRIBE BROKER              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    MQTTBroker broker;
    broker_init(&broker);
    
    printf("  Setting up subscriptions...\n");
    
    /* Set up subscribers */
    broker_subscribe(&broker, "home/temperature", logger_callback, NULL);
    
    double temp_threshold = 25.0;
    broker_subscribe(&broker, "home/temperature", alert_callback, &temp_threshold);
    
    broker_subscribe(&broker, "home/humidity", logger_callback, NULL);
    
    printf("    - Logger subscribed to 'home/temperature'\n");
    printf("    - Alert subscribed to 'home/temperature' (threshold: %.1f)\n",
           temp_threshold);
    printf("    - Logger subscribed to 'home/humidity'\n\n");
    
    printf("  Publishing messages...\n\n");
    
    /* Publish some messages */
    printf("  publish('home/temperature', 22.5):\n");
    broker_publish(&broker, "home/temperature", 22.5);
    
    printf("\n  publish('home/temperature', 28.3):\n");
    broker_publish(&broker, "home/temperature", 28.3);
    
    printf("\n  publish('home/humidity', 65.0):\n");
    broker_publish(&broker, "home/humidity", 65.0);
    
    printf("\n  publish('home/pressure', 1013.25):\n");
    int delivered = broker_publish(&broker, "home/pressure", 1013.25);
    printf("    (No subscribers - delivered to %d)\n", delivered);
    
    printf("\n");
    broker_print_stats(&broker);
}

/* =============================================================================
 * PART 6: SENSOR SIMULATION
 * =============================================================================
 *
 * Simulates realistic sensor data with:
 *   - Base value with gradual drift
 *   - Random noise
 *   - Occasional anomalies (spikes, drops)
 */

typedef struct {
    double base_value;      /* Base measurement value */
    double noise_level;     /* Standard deviation of noise */
    double drift_rate;      /* Rate of gradual drift */
    double current_drift;   /* Accumulated drift */
    double anomaly_prob;    /* Probability of anomaly per reading */
    size_t reading_count;   /* Total readings generated */
} SensorSimulator;

/**
 * Initialise sensor simulator.
 *
 * @param sensor Pointer to sensor structure
 * @param base_value Nominal value
 * @param noise_level Noise standard deviation
 * @param drift_rate Drift per reading
 * @param anomaly_prob Probability of anomaly (0-1)
 */
void sensor_init(SensorSimulator *sensor, double base_value,
                 double noise_level, double drift_rate, double anomaly_prob) {
    if (!sensor) return;
    
    sensor->base_value = base_value;
    sensor->noise_level = noise_level;
    sensor->drift_rate = drift_rate;
    sensor->current_drift = 0.0;
    sensor->anomaly_prob = anomaly_prob;
    sensor->reading_count = 0;
}

/**
 * Generate random number from standard normal distribution.
 * Uses Box-Muller transform.
 */
double random_normal(void) {
    static bool has_spare = false;
    static double spare;
    
    if (has_spare) {
        has_spare = false;
        return spare;
    }
    
    double u, v, s;
    do {
        u = 2.0 * rand() / RAND_MAX - 1.0;
        v = 2.0 * rand() / RAND_MAX - 1.0;
        s = u * u + v * v;
    } while (s >= 1.0 || s == 0.0);
    
    s = sqrt(-2.0 * log(s) / s);
    spare = v * s;
    has_spare = true;
    
    return u * s;
}

/**
 * Generate next sensor reading.
 *
 * @param sensor Pointer to sensor
 * @param is_anomaly Output: set to true if this is an injected anomaly
 * @return Simulated sensor reading
 */
double sensor_read(SensorSimulator *sensor, bool *is_anomaly) {
    if (!sensor) return 0.0;
    
    sensor->reading_count++;
    sensor->current_drift += sensor->drift_rate;
    
    /* Base value with drift and noise */
    double value = sensor->base_value + sensor->current_drift +
                   sensor->noise_level * random_normal();
    
    /* Possibly inject anomaly */
    *is_anomaly = false;
    if ((double)rand() / RAND_MAX < sensor->anomaly_prob) {
        *is_anomaly = true;
        /* Random spike or drop */
        double anomaly_magnitude = 5.0 + 10.0 * (double)rand() / RAND_MAX;
        if (rand() % 2) {
            value += anomaly_magnitude;
        } else {
            value -= anomaly_magnitude;
        }
    }
    
    return value;
}

/**
 * Demonstration of sensor simulation.
 */
void demo_sensor_simulation(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: SENSOR SIMULATION                                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Seed random number generator */
    srand(42);  /* Fixed seed for reproducibility */
    
    SensorSimulator temp_sensor;
    sensor_init(&temp_sensor, 
                20.0,   /* Base: 20°C */
                0.5,    /* Noise: ±0.5°C */
                0.02,   /* Drift: 0.02°C per reading */
                0.1);   /* 10% anomaly probability */
    
    printf("  Simulating temperature sensor:\n");
    printf("    Base: 20°C, Noise: ±0.5°C, Drift: 0.02°C/reading\n");
    printf("    Anomaly probability: 10%%\n\n");
    
    printf("  ┌───────┬───────────┬────────────┐\n");
    printf("  │   #   │  Reading  │   Type     │\n");
    printf("  ├───────┼───────────┼────────────┤\n");
    
    int anomaly_count = 0;
    for (int i = 0; i < 20; i++) {
        bool is_anomaly;
        double reading = sensor_read(&temp_sensor, &is_anomaly);
        const char *type = is_anomaly ? "⚠ ANOMALY" : "  Normal";
        if (is_anomaly) anomaly_count++;
        
        printf("  │  %2d   │   %6.2f  │ %s │\n", i + 1, reading, type);
    }
    
    printf("  └───────┴───────────┴────────────┘\n\n");
    printf("  Total anomalies injected: %d\n", anomaly_count);
}

/* =============================================================================
 * PART 7: COMPLETE STREAM PROCESSING PIPELINE
 * =============================================================================
 *
 * Integrates all components:
 *   Sensor → Circular Buffer → EMA Filter → Welford Stats → Anomaly Detection
 *                    ↓
 *              MQTT Publish
 */

typedef struct {
    /* Components */
    SensorSimulator sensor;
    CircularBuffer *window;
    EMAFilter ema;
    WelfordState stats;
    MQTTBroker broker;
    
    /* Configuration */
    double anomaly_threshold;
    const char *topic;
    
    /* Metrics */
    size_t readings_processed;
    size_t anomalies_detected;
    size_t true_positives;
    size_t false_positives;
    size_t false_negatives;
} StreamPipeline;

/**
 * Initialise complete stream processing pipeline.
 */
void pipeline_init(StreamPipeline *pipe, size_t window_size, double alpha,
                   double threshold, const char *topic) {
    if (!pipe) return;
    
    /* Initialise sensor */
    sensor_init(&pipe->sensor, 20.0, 0.5, 0.01, 0.05);
    
    /* Initialise processing components */
    pipe->window = cb_create(window_size);
    ema_init(&pipe->ema, alpha);
    welford_init(&pipe->stats);
    broker_init(&pipe->broker);
    
    /* Configuration */
    pipe->anomaly_threshold = threshold;
    pipe->topic = topic;
    
    /* Metrics */
    pipe->readings_processed = 0;
    pipe->anomalies_detected = 0;
    pipe->true_positives = 0;
    pipe->false_positives = 0;
    pipe->false_negatives = 0;
}

/**
 * Process one reading through the pipeline.
 *
 * @param pipe Pointer to pipeline
 * @return Processed result
 */
AnomalyResult pipeline_process(StreamPipeline *pipe) {
    if (!pipe) {
        AnomalyResult empty = {0};
        return empty;
    }
    
    /* Generate reading */
    bool injected_anomaly;
    double raw = sensor_read(&pipe->sensor, &injected_anomaly);
    
    /* Apply EMA filter */
    double filtered = ema_update(&pipe->ema, raw);
    
    /* Update sliding window */
    cb_push(pipe->window, filtered);
    
    /* Detect anomaly */
    AnomalyResult result = {0};
    if (pipe->stats.count >= 10) {  /* Need enough baseline data */
        result = detect_anomaly(filtered, pipe->stats.mean,
                               welford_stddev(&pipe->stats),
                               pipe->anomaly_threshold);
        
        if (result.is_anomaly) {
            pipe->anomalies_detected++;
            if (injected_anomaly) {
                pipe->true_positives++;
            } else {
                pipe->false_positives++;
            }
        } else {
            /* Update baseline with normal values */
            welford_update(&pipe->stats, filtered);
            if (injected_anomaly) {
                pipe->false_negatives++;
            }
        }
    } else {
        /* Building baseline */
        welford_update(&pipe->stats, filtered);
    }
    
    /* Publish to MQTT */
    broker_publish(&pipe->broker, pipe->topic, filtered);
    
    pipe->readings_processed++;
    result.value = raw;  /* Store original for display */
    
    return result;
}

/**
 * Free pipeline resources.
 */
void pipeline_free(StreamPipeline *pipe) {
    if (pipe && pipe->window) {
        cb_free(pipe->window);
        pipe->window = NULL;
    }
}

/**
 * Print pipeline metrics.
 */
void pipeline_print_metrics(StreamPipeline *pipe) {
    if (!pipe) return;
    
    printf("\n  Pipeline Metrics:\n");
    printf("  ─────────────────\n");
    printf("    Readings processed: %zu\n", pipe->readings_processed);
    printf("    Anomalies detected: %zu\n", pipe->anomalies_detected);
    printf("    True positives: %zu\n", pipe->true_positives);
    printf("    False positives: %zu\n", pipe->false_positives);
    printf("    False negatives: %zu\n", pipe->false_negatives);
    
    if (pipe->true_positives + pipe->false_positives > 0) {
        double precision = (double)pipe->true_positives /
                          (pipe->true_positives + pipe->false_positives);
        printf("    Precision: %.2f%%\n", precision * 100);
    }
    if (pipe->true_positives + pipe->false_negatives > 0) {
        double recall = (double)pipe->true_positives /
                       (pipe->true_positives + pipe->false_negatives);
        printf("    Recall: %.2f%%\n", recall * 100);
    }
}

/**
 * Demonstration of complete pipeline.
 */
void demo_pipeline(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 7: COMPLETE STREAM PROCESSING PIPELINE              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    srand(123);  /* Fixed seed */
    
    StreamPipeline pipe;
    pipeline_init(&pipe, 10, 0.2, 2.5, "sensor/temperature");
    
    /* Subscribe to see messages */
    broker_subscribe(&pipe.broker, "sensor/temperature", logger_callback, NULL);
    
    printf("  Configuration:\n");
    printf("    Window size: 10\n");
    printf("    EMA alpha: 0.2\n");
    printf("    Anomaly threshold: z > 2.5\n");
    printf("    Topic: sensor/temperature\n\n");
    
    printf("  Processing 50 readings (first 10 build baseline):\n\n");
    
    printf("  ┌───────┬─────────┬──────────┬───────────┬────────────┐\n");
    printf("  │   #   │   Raw   │ Filtered │  Z-Score  │   Status   │\n");
    printf("  ├───────┼─────────┼──────────┼───────────┼────────────┤\n");
    
    for (int i = 0; i < 50; i++) {
        AnomalyResult result = pipeline_process(&pipe);
        
        double filtered = pipe.ema.value;
        const char *status;
        
        if (i < 10) {
            status = "BASELINE";
        } else if (result.is_anomaly) {
            status = "⚠ ANOMALY";
        } else {
            status = "  Normal";
        }
        
        if (i < 10 || i >= 45 || result.is_anomaly) {
            printf("  │  %2d   │ %7.2f │  %7.2f │   %+5.2f   │ %s │\n",
                   i + 1, result.value, filtered, result.z_score, status);
        } else if (i == 10) {
            printf("  │  ...  │   ...   │    ...   │    ...    │    ...     │\n");
        }
    }
    
    printf("  └───────┴─────────┴──────────┴───────────┴────────────┘\n");
    
    pipeline_print_metrics(&pipe);
    
    printf("\n  Current baseline statistics:\n");
    printf("    Mean: %.3f\n", pipe.stats.mean);
    printf("    Std Dev: %.3f\n", welford_stddev(&pipe.stats));
    
    pipeline_free(&pipe);
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 19: ALGORITHMS FOR IoT AND STREAM PROCESSING         ║\n");
    printf("║                    Complete Example                           ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n  This demonstration covers:\n");
    printf("  ─────────────────────────────\n");
    printf("  1. Circular buffer (ring buffer) for sliding windows\n");
    printf("  2. Exponential Moving Average (EMA) filter\n");
    printf("  3. Welford's online algorithm for running statistics\n");
    printf("  4. Z-score anomaly detection\n");
    printf("  5. MQTT-style publish-subscribe broker\n");
    printf("  6. Sensor simulation with noise and anomalies\n");
    printf("  7. Complete integrated stream processing pipeline\n");
    
    demo_circular_buffer();
    demo_ema_filter();
    demo_welford();
    demo_anomaly_detection();
    demo_mqtt_broker();
    demo_sensor_simulation();
    demo_pipeline();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                   DEMONSTRATION COMPLETE                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

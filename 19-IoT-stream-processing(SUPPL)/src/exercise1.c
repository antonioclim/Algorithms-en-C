/**
 * =============================================================================
 * WEEK 19: ALGORITHMS FOR IoT AND STREAM PROCESSING
 * Exercise 1: Stream Processing Pipeline
 * =============================================================================
 *
 * OBJECTIVE:
 * Implement a complete sensor data processing pipeline that includes:
 *   - Circular buffer for sliding window statistics
 *   - Exponential Moving Average (EMA) filter
 *   - Welford's online algorithm for running mean/variance
 *   - Z-score anomaly detection
 *
 * INSTRUCTIONS:
 * Complete the functions marked with TODO. Each TODO includes:
 *   - Description of what to implement
 *   - Expected time/space complexity
 *   - Hints for implementation
 *
 * TESTING:
 *   make run-ex1
 *   ./exercise1 -w 10 -a 0.2 -t 2.5 < data/sensor_stream.txt
 *   ./exercise1 -stdin < data/sensor_anomalies.txt
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <getopt.h>

/* =============================================================================
 * CONFIGURATION
 * =============================================================================
 */

#define MAX_BUFFER_SIZE 1000
#define DEFAULT_WINDOW  10
#define DEFAULT_ALPHA   0.2
#define DEFAULT_THRESHOLD 2.5
#define MAX_LINE_LENGTH 256

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

/**
 * Circular buffer for sliding window operations.
 *
 * Maintains a fixed-size window of the most recent values.
 * When full, new values overwrite the oldest.
 */
typedef struct {
    double *data;       /* Array of values */
    size_t capacity;    /* Maximum elements */
    size_t count;       /* Current elements (0 to capacity) */
    size_t head;        /* Index for next insertion */
    double sum;         /* Running sum for O(1) mean */
} CircularBuffer;

/**
 * Exponential Moving Average filter.
 *
 * EMA(t) = α × x(t) + (1 - α) × EMA(t-1)
 */
typedef struct {
    double value;       /* Current EMA value */
    double alpha;       /* Smoothing factor (0 < α < 1) */
    bool initialised;   /* Has first value been processed? */
} EMAFilter;

/**
 * Welford's online statistics accumulator.
 *
 * Computes running mean and variance in a single pass
 * with numerical stability.
 */
typedef struct {
    double mean;        /* Running mean */
    double M2;          /* Sum of squared differences */
    size_t count;       /* Number of values seen */
} WelfordState;

/**
 * Anomaly detection result.
 */
typedef struct {
    double value;       /* Original value */
    double filtered;    /* After EMA filtering */
    double z_score;     /* Z-score */
    bool is_anomaly;    /* Whether flagged as anomaly */
} ProcessingResult;

/**
 * Complete processing pipeline.
 */
typedef struct {
    CircularBuffer *buffer;
    EMAFilter ema;
    WelfordState stats;
    double threshold;
    size_t readings;
    size_t anomalies;
} Pipeline;

/* =============================================================================
 * TODO 1: CIRCULAR BUFFER - CREATE
 * =============================================================================
 *
 * Create and initialise a circular buffer.
 *
 * Requirements:
 *   - Allocate CircularBuffer structure
 *   - Allocate data array of specified capacity
 *   - Initialise all fields (count=0, head=0, sum=0)
 *   - Handle allocation failures gracefully
 *
 * Time Complexity: O(1)
 * Space Complexity: O(capacity)
 *
 * Hint: Use malloc for both structure and array
 */
CircularBuffer *cb_create(size_t capacity) {
    /* TODO: Implement circular buffer creation */
    
    /* Validate capacity */
    if (capacity == 0 || capacity > MAX_BUFFER_SIZE) {
        fprintf(stderr, "Error: Invalid capacity %zu\n", capacity);
        return NULL;
    }
    
    /* Allocate structure */
    CircularBuffer *cb = malloc(sizeof(CircularBuffer));
    if (!cb) {
        fprintf(stderr, "Error: Failed to allocate CircularBuffer\n");
        return NULL;
    }
    
    /* TODO: Allocate data array */
    /* YOUR CODE HERE */
    
    /* TODO: Initialise all fields */
    /* YOUR CODE HERE */
    
    return cb;
}

/* =============================================================================
 * TODO 2: CIRCULAR BUFFER - FREE
 * =============================================================================
 *
 * Free all memory associated with circular buffer.
 *
 * Requirements:
 *   - Free the data array
 *   - Free the structure itself
 *   - Handle NULL pointer safely
 *
 * Time Complexity: O(1)
 */
void cb_free(CircularBuffer *cb) {
    /* TODO: Implement memory cleanup */
    
    /* YOUR CODE HERE */
}

/* =============================================================================
 * TODO 3: CIRCULAR BUFFER - PUSH
 * =============================================================================
 *
 * Insert a new value into the circular buffer.
 *
 * Requirements:
 *   - If buffer not full, just add value
 *   - If buffer full, subtract old value from sum first
 *   - Update sum with new value
 *   - Advance head with wraparound: head = (head + 1) % capacity
 *
 * Time Complexity: O(1)
 *
 * Visualisation:
 *   Before (full, capacity=3): [10, 20, 30], head=0
 *   push(40):
 *     - Subtract data[0]=10 from sum
 *     - Add 40 at position 0
 *     - head becomes 1
 *   After: [40, 20, 30], head=1 (oldest is 20)
 */
void cb_push(CircularBuffer *cb, double value) {
    if (!cb) return;
    
    /* TODO: Handle case when buffer is full */
    /* Hint: Subtract the value being overwritten from sum */
    
    /* YOUR CODE HERE */
    
    /* TODO: Insert new value */
    /* Hint: cb->data[cb->head] = value; */
    
    /* YOUR CODE HERE */
    
    /* TODO: Update sum and advance head */
    /* Hint: Use modulo for wraparound */
    
    /* YOUR CODE HERE */
}

/**
 * Calculate mean of values in buffer.
 * (Provided - uses your cb_push implementation)
 */
double cb_mean(CircularBuffer *cb) {
    if (!cb || cb->count == 0) return 0.0;
    return cb->sum / cb->count;
}

/**
 * Calculate variance of values in buffer.
 * (Provided for reference)
 */
double cb_variance(CircularBuffer *cb) {
    if (!cb || cb->count < 2) return 0.0;
    
    double mean = cb_mean(cb);
    double sum_sq = 0.0;
    
    for (size_t i = 0; i < cb->count; i++) {
        size_t idx = (cb->head + cb->capacity - cb->count + i) % cb->capacity;
        double diff = cb->data[idx] - mean;
        sum_sq += diff * diff;
    }
    
    return sum_sq / (cb->count - 1);
}

/* =============================================================================
 * TODO 4: EMA UPDATE
 * =============================================================================
 *
 * Update Exponential Moving Average with new value.
 *
 * Formula: EMA(t) = α × new_value + (1 - α) × EMA(t-1)
 *
 * Requirements:
 *   - First value: EMA = value (initialise)
 *   - Subsequent: Apply EMA formula
 *   - Mark as initialised after first value
 *
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 *
 * Example (α = 0.3):
 *   Value 10.0 → EMA = 10.0 (first)
 *   Value 20.0 → EMA = 0.3×20 + 0.7×10 = 13.0
 *   Value 15.0 → EMA = 0.3×15 + 0.7×13 = 13.6
 */
double ema_update(EMAFilter *ema, double value) {
    if (!ema) return 0.0;
    
    /* TODO: Handle first value (initialisation) */
    /* Hint: Check ema->initialised flag */
    
    /* YOUR CODE HERE */
    
    /* TODO: Apply EMA formula for subsequent values */
    /* Hint: ema->value = α × value + (1 - α) × ema->value */
    
    /* YOUR CODE HERE */
    
    return ema->value;
}

/**
 * Initialise EMA filter.
 * (Provided)
 */
void ema_init(EMAFilter *ema, double alpha) {
    if (!ema) return;
    ema->value = 0.0;
    ema->alpha = (alpha > 0 && alpha < 1) ? alpha : DEFAULT_ALPHA;
    ema->initialised = false;
}

/* =============================================================================
 * TODO 5: WELFORD UPDATE
 * =============================================================================
 *
 * Update running statistics using Welford's algorithm.
 *
 * Algorithm:
 *   count = count + 1
 *   delta = value - mean
 *   mean = mean + delta / count
 *   delta2 = value - mean     (note: uses NEW mean!)
 *   M2 = M2 + delta × delta2
 *
 * The key insight is using both the old delta (before mean update)
 * and new delta (after mean update) in the M2 calculation.
 *
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 */
void welford_update(WelfordState *state, double value) {
    if (!state) return;
    
    /* TODO: Increment count */
    /* YOUR CODE HERE */
    
    /* TODO: Calculate delta (before mean update) */
    /* YOUR CODE HERE */
    
    /* TODO: Update mean */
    /* Hint: mean += delta / count */
    /* YOUR CODE HERE */
    
    /* TODO: Calculate delta2 (after mean update) */
    /* YOUR CODE HERE */
    
    /* TODO: Update M2 */
    /* Hint: M2 += delta × delta2 */
    /* YOUR CODE HERE */
}

/**
 * Get variance from Welford state.
 * (Provided)
 */
double welford_variance(WelfordState *state) {
    if (!state || state->count < 2) return 0.0;
    return state->M2 / (state->count - 1);  /* Bessel's correction */
}

/**
 * Get standard deviation from Welford state.
 * (Provided)
 */
double welford_stddev(WelfordState *state) {
    return sqrt(welford_variance(state));
}

/**
 * Initialise Welford state.
 * (Provided)
 */
void welford_init(WelfordState *state) {
    if (!state) return;
    state->mean = 0.0;
    state->M2 = 0.0;
    state->count = 0;
}

/* =============================================================================
 * TODO 6: ANOMALY DETECTION
 * =============================================================================
 *
 * Detect if a value is anomalous using z-score.
 *
 * Z-score formula: z = (value - mean) / stddev
 *
 * Requirements:
 *   - Calculate z-score
 *   - Mark as anomaly if |z| > threshold
 *   - Handle edge case: stddev near zero
 *
 * Time Complexity: O(1)
 *
 * Example:
 *   mean=20, stddev=2, threshold=2.5
 *   value=25: z=(25-20)/2 = 2.5 → borderline
 *   value=26: z=(26-20)/2 = 3.0 → anomaly!
 */
bool detect_anomaly(double value, double mean, double stddev, 
                    double threshold, double *z_score) {
    /* TODO: Handle edge case (stddev too small) */
    /* Hint: If stddev < 1e-10, set z_score to 0 and return false */
    
    /* YOUR CODE HERE */
    
    /* TODO: Calculate z-score */
    /* YOUR CODE HERE */
    
    /* TODO: Determine if anomaly */
    /* Hint: return fabs(*z_score) > threshold */
    
    /* YOUR CODE HERE */
    
    return false;  /* Replace with your implementation */
}

/* =============================================================================
 * TODO 7: PROCESS READING
 * =============================================================================
 *
 * Process a single sensor reading through the complete pipeline.
 *
 * Pipeline stages:
 *   1. Apply EMA filter to smooth the input
 *   2. Add filtered value to circular buffer
 *   3. If enough data (count >= 10), detect anomaly
 *   4. If normal, update Welford statistics (for future baseline)
 *   5. Update counters
 *
 * Time Complexity: O(1)
 */
ProcessingResult process_reading(Pipeline *pipe, double raw_value) {
    ProcessingResult result = {0};
    result.value = raw_value;
    
    if (!pipe) return result;
    
    /* TODO: Stage 1 - Apply EMA filter */
    /* YOUR CODE HERE */
    
    /* TODO: Stage 2 - Add to circular buffer */
    /* YOUR CODE HERE */
    
    /* TODO: Stage 3 - Detect anomaly if enough baseline data */
    /* Hint: Need at least 10 values before detecting anomalies */
    /* YOUR CODE HERE */
    
    /* TODO: Stage 4 - Update Welford stats if not anomaly */
    /* YOUR CODE HERE */
    
    /* TODO: Stage 5 - Update counters */
    pipe->readings++;
    if (result.is_anomaly) {
        pipe->anomalies++;
    }
    
    return result;
}

/* =============================================================================
 * PIPELINE MANAGEMENT (PROVIDED)
 * =============================================================================
 */

/**
 * Initialise processing pipeline.
 */
void pipeline_init(Pipeline *pipe, size_t window, double alpha, double threshold) {
    if (!pipe) return;
    
    pipe->buffer = cb_create(window);
    ema_init(&pipe->ema, alpha);
    welford_init(&pipe->stats);
    pipe->threshold = threshold;
    pipe->readings = 0;
    pipe->anomalies = 0;
}

/**
 * Free pipeline resources.
 */
void pipeline_free(Pipeline *pipe) {
    if (pipe) {
        cb_free(pipe->buffer);
    }
}

/**
 * Print pipeline statistics.
 */
void pipeline_print_stats(Pipeline *pipe) {
    if (!pipe) return;
    
    printf("\nPipeline Statistics:\n");
    printf("────────────────────\n");
    printf("  Readings processed: %zu\n", pipe->readings);
    printf("  Anomalies detected: %zu\n", pipe->anomalies);
    printf("  Baseline mean: %.3f\n", pipe->stats.mean);
    printf("  Baseline stddev: %.3f\n", welford_stddev(&pipe->stats));
    if (pipe->buffer) {
        printf("  Window mean: %.3f\n", cb_mean(pipe->buffer));
    }
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

void print_usage(const char *prog) {
    printf("Usage: %s [options]\n", prog);
    printf("Options:\n");
    printf("  -w SIZE    Window size for circular buffer (default: %d)\n", 
           DEFAULT_WINDOW);
    printf("  -a ALPHA   EMA smoothing factor, 0<α<1 (default: %.1f)\n", 
           DEFAULT_ALPHA);
    printf("  -t THRESH  Anomaly z-score threshold (default: %.1f)\n", 
           DEFAULT_THRESHOLD);
    printf("  -n COUNT   Number of readings to generate (default: 30)\n");
    printf("  -stdin     Read values from stdin instead of generating\n");
    printf("  -h         Show this help message\n");
    printf("\nExample:\n");
    printf("  %s -w 10 -a 0.2 -t 2.5 -stdin < data/sensor_stream.txt\n", prog);
}

int main(int argc, char *argv[]) {
    /* Default configuration */
    size_t window = DEFAULT_WINDOW;
    double alpha = DEFAULT_ALPHA;
    double threshold = DEFAULT_THRESHOLD;
    int count = 30;
    bool use_stdin = false;
    
    /* Parse command line arguments */
    int opt;
    while ((opt = getopt(argc, argv, "w:a:t:n:sh")) != -1) {
        switch (opt) {
            case 'w':
                window = (size_t)atoi(optarg);
                break;
            case 'a':
                alpha = atof(optarg);
                break;
            case 't':
                threshold = atof(optarg);
                break;
            case 'n':
                count = atoi(optarg);
                break;
            case 's':
                use_stdin = true;
                break;
            case 'h':
            default:
                print_usage(argv[0]);
                return (opt == 'h') ? 0 : 1;
        }
    }
    
    /* Also check for -stdin without dash handling */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-stdin") == 0) {
            use_stdin = true;
        }
    }
    
    printf("Stream Processing Pipeline\n");
    printf("══════════════════════════\n");
    printf("Configuration:\n");
    printf("  Window size: %zu\n", window);
    printf("  EMA alpha: %.2f\n", alpha);
    printf("  Anomaly threshold: z > %.2f\n", threshold);
    printf("  Input: %s\n\n", use_stdin ? "stdin" : "generated");
    
    /* Initialise pipeline */
    Pipeline pipe;
    pipeline_init(&pipe, window, alpha, threshold);
    
    if (!pipe.buffer) {
        fprintf(stderr, "Failed to initialise pipeline\n");
        return 1;
    }
    
    /* Print header */
    printf("┌───────┬──────────┬──────────┬───────────┬────────────┐\n");
    printf("│   #   │   Raw    │ Filtered │  Z-Score  │   Status   │\n");
    printf("├───────┼──────────┼──────────┼───────────┼────────────┤\n");
    
    /* Process readings */
    if (use_stdin) {
        char line[MAX_LINE_LENGTH];
        int i = 0;
        while (fgets(line, sizeof(line), stdin)) {
            double value = atof(line);
            ProcessingResult result = process_reading(&pipe, value);
            
            const char *status;
            if (pipe.stats.count < 10) {
                status = "BASELINE";
            } else if (result.is_anomaly) {
                status = "⚠ ANOMALY";
            } else {
                status = "  Normal";
            }
            
            printf("│  %3d  │ %8.2f │ %8.2f │   %+6.2f  │ %s │\n",
                   ++i, result.value, result.filtered, result.z_score, status);
        }
    } else {
        /* Generate synthetic data with anomalies */
        srand(42);
        for (int i = 0; i < count; i++) {
            double value = 20.0 + 0.5 * ((double)rand() / RAND_MAX - 0.5);
            
            /* Inject anomalies at specific positions */
            if (i == 12) value = 45.0;  /* Spike */
            if (i == 22) value = -5.0;  /* Drop */
            
            ProcessingResult result = process_reading(&pipe, value);
            
            const char *status;
            if (pipe.stats.count < 10) {
                status = "BASELINE";
            } else if (result.is_anomaly) {
                status = "⚠ ANOMALY";
            } else {
                status = "  Normal";
            }
            
            printf("│  %3d  │ %8.2f │ %8.2f │   %+6.2f  │ %s │\n",
                   i + 1, result.value, result.filtered, result.z_score, status);
        }
    }
    
    printf("└───────┴──────────┴──────────┴───────────┴────────────┘\n");
    
    /* Print statistics */
    pipeline_print_stats(&pipe);
    
    /* Cleanup */
    pipeline_free(&pipe);
    
    return 0;
}

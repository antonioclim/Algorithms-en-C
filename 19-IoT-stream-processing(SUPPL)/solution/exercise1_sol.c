/**
 * =============================================================================
 * WEEK 19: ALGORITHMS FOR IoT AND STREAM PROCESSING
 * Exercise 1: Stream Processing Pipeline - SOLUTION
 * =============================================================================
 *
 * This is the complete reference solution with all TODOs implemented.
 *
 * Key learning points demonstrated:
 *   1. Circular buffer with O(1) mean via cached sum
 *   2. EMA filter with proper initialisation
 *   3. Welford's algorithm for numerical stability
 *   4. Z-score anomaly detection
 *   5. Complete pipeline integration
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c -lm
 * Usage: ./exercise1_sol -w 10 -a 0.2 -t 2.5 -stdin < ../data/sensor_stream.txt
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
 * SOLUTION 1: CIRCULAR BUFFER - CREATE
 * =============================================================================
 *
 * Implementation notes:
 *   - Allocate structure first, then data array
 *   - Initialise all fields to clean state
 *   - Handle allocation failures gracefully
 *
 * Time Complexity: O(1)
 * Space Complexity: O(capacity)
 */
CircularBuffer *cb_create(size_t capacity) {
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
    
    /* SOLUTION: Allocate data array */
    cb->data = malloc(capacity * sizeof(double));
    if (!cb->data) {
        fprintf(stderr, "Error: Failed to allocate buffer data\n");
        free(cb);
        return NULL;
    }
    
    /* SOLUTION: Initialise all fields */
    cb->capacity = capacity;
    cb->count = 0;
    cb->head = 0;
    cb->sum = 0.0;
    
    /* Optional: Zero-initialise data array for cleanliness */
    for (size_t i = 0; i < capacity; i++) {
        cb->data[i] = 0.0;
    }
    
    return cb;
}

/* =============================================================================
 * SOLUTION 2: CIRCULAR BUFFER - FREE
 * =============================================================================
 *
 * Implementation notes:
 *   - Check for NULL before freeing
 *   - Free data array first, then structure
 *
 * Time Complexity: O(1)
 */
void cb_free(CircularBuffer *cb) {
    /* SOLUTION: Handle NULL safely and free in correct order */
    if (!cb) return;
    
    free(cb->data);  /* Free data array first */
    free(cb);        /* Then free structure */
}

/* =============================================================================
 * SOLUTION 3: CIRCULAR BUFFER - PUSH
 * =============================================================================
 *
 * Implementation notes:
 *   - Key insight: subtract old value BEFORE overwriting
 *   - Use modulo for wraparound
 *   - Track count separately from head position
 *
 * Visualisation:
 *   Before (full, capacity=3): [10, 20, 30], head=0, sum=60
 *   push(40):
 *     - sum -= data[0] (60-10=50)
 *     - data[0] = 40
 *     - sum += 40 (50+40=90)
 *     - head = (0+1) % 3 = 1
 *   After: [40, 20, 30], head=1, sum=90 (oldest is 20 at index 1)
 *
 * Time Complexity: O(1)
 */
void cb_push(CircularBuffer *cb, double value) {
    if (!cb) return;
    
    /* SOLUTION: Handle case when buffer is full */
    if (cb->count == cb->capacity) {
        /* Buffer is full - subtract the value we're about to overwrite */
        cb->sum -= cb->data[cb->head];
    } else {
        /* Buffer not full yet - just increment count */
        cb->count++;
    }
    
    /* SOLUTION: Insert new value at head position */
    cb->data[cb->head] = value;
    
    /* SOLUTION: Update sum */
    cb->sum += value;
    
    /* SOLUTION: Advance head with wraparound */
    cb->head = (cb->head + 1) % cb->capacity;
}

/**
 * Calculate mean of values in buffer.
 * Uses cached sum for O(1) complexity.
 */
double cb_mean(CircularBuffer *cb) {
    if (!cb || cb->count == 0) return 0.0;
    return cb->sum / cb->count;
}

/**
 * Calculate variance of values in buffer.
 * O(n) - iterates through all values.
 */
double cb_variance(CircularBuffer *cb) {
    if (!cb || cb->count < 2) return 0.0;
    
    double mean = cb_mean(cb);
    double sum_sq = 0.0;
    
    /* Iterate through valid values in circular order */
    for (size_t i = 0; i < cb->count; i++) {
        /* Calculate index: start from oldest value */
        size_t idx = (cb->head + cb->capacity - cb->count + i) % cb->capacity;
        double diff = cb->data[idx] - mean;
        sum_sq += diff * diff;
    }
    
    return sum_sq / (cb->count - 1);  /* Bessel's correction */
}

/* =============================================================================
 * SOLUTION 4: EMA UPDATE
 * =============================================================================
 *
 * Implementation notes:
 *   - First value becomes EMA directly (initialisation)
 *   - Subsequent values use weighted average
 *   - Formula: EMA_new = α × value + (1 - α) × EMA_old
 *
 * Example (α = 0.3):
 *   Value 10.0 → EMA = 10.0 (first value)
 *   Value 20.0 → EMA = 0.3×20 + 0.7×10 = 6 + 7 = 13.0
 *   Value 15.0 → EMA = 0.3×15 + 0.7×13 = 4.5 + 9.1 = 13.6
 *
 * Time Complexity: O(1)
 */
double ema_update(EMAFilter *ema, double value) {
    if (!ema) return 0.0;
    
    /* SOLUTION: Handle first value (initialisation) */
    if (!ema->initialised) {
        ema->value = value;
        ema->initialised = true;
        return ema->value;
    }
    
    /* SOLUTION: Apply EMA formula for subsequent values */
    /* EMA_new = α × new_value + (1 - α) × EMA_old */
    ema->value = ema->alpha * value + (1.0 - ema->alpha) * ema->value;
    
    return ema->value;
}

/**
 * Initialise EMA filter.
 */
void ema_init(EMAFilter *ema, double alpha) {
    if (!ema) return;
    ema->value = 0.0;
    ema->alpha = (alpha > 0 && alpha < 1) ? alpha : DEFAULT_ALPHA;
    ema->initialised = false;
}

/* =============================================================================
 * SOLUTION 5: WELFORD UPDATE
 * =============================================================================
 *
 * Implementation notes:
 *   - Key insight: use OLD delta and NEW delta
 *   - delta = value - mean (before update)
 *   - delta2 = value - mean (after update)
 *   - M2 += delta × delta2
 *
 * This formulation avoids catastrophic cancellation in the naive
 * formula: Var = E[X²] - E[X]² which can give negative results
 * due to floating-point errors.
 *
 * Reference: B.P. Welford (1962). "Note on a Method for Calculating
 * Corrected Sums of Squares and Products". Technometrics 4(3):419–420.
 *
 * Time Complexity: O(1)
 */
void welford_update(WelfordState *state, double value) {
    if (!state) return;
    
    /* SOLUTION: Increment count */
    state->count++;
    
    /* SOLUTION: Calculate delta (before mean update) */
    double delta = value - state->mean;
    
    /* SOLUTION: Update mean */
    state->mean += delta / state->count;
    
    /* SOLUTION: Calculate delta2 (after mean update) - uses NEW mean! */
    double delta2 = value - state->mean;
    
    /* SOLUTION: Update M2 */
    state->M2 += delta * delta2;
}

/**
 * Get variance from Welford state.
 * Uses Bessel's correction for sample variance.
 */
double welford_variance(WelfordState *state) {
    if (!state || state->count < 2) return 0.0;
    return state->M2 / (state->count - 1);  /* Bessel's correction */
}

/**
 * Get standard deviation from Welford state.
 */
double welford_stddev(WelfordState *state) {
    return sqrt(welford_variance(state));
}

/**
 * Initialise Welford state.
 */
void welford_init(WelfordState *state) {
    if (!state) return;
    state->mean = 0.0;
    state->M2 = 0.0;
    state->count = 0;
}

/* =============================================================================
 * SOLUTION 6: ANOMALY DETECTION
 * =============================================================================
 *
 * Implementation notes:
 *   - Z-score measures how many standard deviations from mean
 *   - Handle edge case: stddev near zero (constant signal)
 *   - Use fabs() for absolute value comparison
 *
 * Z-score interpretation:
 *   |z| < 1.0  : Within 1 stddev (68% of normal data)
 *   |z| < 2.0  : Within 2 stddev (95% of normal data)
 *   |z| < 2.5  : Within 2.5 stddev (99% of normal data)
 *   |z| < 3.0  : Within 3 stddev (99.7% of normal data)
 *   |z| > 3.0  : Very unusual (0.3% chance if normal)
 *
 * Time Complexity: O(1)
 */
bool detect_anomaly(double value, double mean, double stddev, 
                    double threshold, double *z_score) {
    /* SOLUTION: Handle edge case (stddev too small) */
    if (stddev < 1e-10) {
        /* Constant or near-constant signal - can't compute z-score */
        if (z_score) *z_score = 0.0;
        return false;
    }
    
    /* SOLUTION: Calculate z-score */
    double z = (value - mean) / stddev;
    if (z_score) *z_score = z;
    
    /* SOLUTION: Determine if anomaly using absolute value */
    return fabs(z) > threshold;
}

/* =============================================================================
 * SOLUTION 7: PROCESS READING
 * =============================================================================
 *
 * Implementation notes:
 *   - Pipeline order matters:
 *     1. Smooth with EMA first (reduces noise)
 *     2. Store in circular buffer (for windowed stats)
 *     3. Detect anomaly using long-term baseline
 *     4. Update baseline only with normal readings
 *
 * Design decision: We only update Welford stats for normal readings.
 * This prevents anomalies from corrupting the baseline, making
 * future detection more robust.
 *
 * Time Complexity: O(1)
 */
ProcessingResult process_reading(Pipeline *pipe, double raw_value) {
    ProcessingResult result = {0};
    result.value = raw_value;
    
    if (!pipe) return result;
    
    /* SOLUTION Stage 1: Apply EMA filter */
    result.filtered = ema_update(&pipe->ema, raw_value);
    
    /* SOLUTION Stage 2: Add to circular buffer */
    cb_push(pipe->buffer, result.filtered);
    
    /* SOLUTION Stage 3: Detect anomaly if enough baseline data */
    /* Need at least 10 values to establish meaningful baseline */
    if (pipe->stats.count >= 10) {
        result.is_anomaly = detect_anomaly(
            result.filtered,
            pipe->stats.mean,
            welford_stddev(&pipe->stats),
            pipe->threshold,
            &result.z_score
        );
    } else {
        /* During baseline period, mark as not anomaly */
        result.is_anomaly = false;
        result.z_score = 0.0;
    }
    
    /* SOLUTION Stage 4: Update Welford stats if not anomaly */
    /* This keeps the baseline clean from outliers */
    if (!result.is_anomaly) {
        welford_update(&pipe->stats, result.filtered);
    }
    
    /* SOLUTION Stage 5: Update counters */
    pipe->readings++;
    if (result.is_anomaly) {
        pipe->anomalies++;
    }
    
    return result;
}

/* =============================================================================
 * PIPELINE MANAGEMENT
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
    if (pipe->readings > 0) {
        printf("  Anomaly rate: %.1f%%\n", 
               100.0 * pipe->anomalies / pipe->readings);
    }
    printf("  Baseline mean: %.3f\n", pipe->stats.mean);
    printf("  Baseline stddev: %.3f\n", welford_stddev(&pipe->stats));
    printf("  Baseline samples: %zu\n", pipe->stats.count);
    if (pipe->buffer) {
        printf("  Window mean: %.3f\n", cb_mean(pipe->buffer));
        printf("  Window size: %zu/%zu\n", 
               pipe->buffer->count, pipe->buffer->capacity);
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
    
    printf("Stream Processing Pipeline - SOLUTION\n");
    printf("═════════════════════════════════════\n");
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

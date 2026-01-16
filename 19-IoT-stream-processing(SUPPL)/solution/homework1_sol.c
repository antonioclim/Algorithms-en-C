/**
 * =============================================================================
 * WEEK 19: ALGORITHMS FOR IoT AND STREAM PROCESSING
 * Homework 1: Multi-Sensor Fusion System - SOLUTION
 * =============================================================================
 *
 * This solution demonstrates:
 *   1. Sensor array management with individual statistics
 *   2. Inverse-variance weighted sensor fusion
 *   3. Failure detection (stuck, drift, spike)
 *   4. Robust estimation with failed sensor exclusion
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c -lm
 * Usage: ./homework1_sol
 *
 * Industrial Application:
 *   Redundant sensors are common in safety-critical systems (aviation,
 *   nuclear plants, medical devices). This code demonstrates the core
 *   algorithms used in sensor fusion and failure detection.
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <time.h>

/* =============================================================================
 * CONFIGURATION
 * =============================================================================
 */

#define MAX_SENSORS         16
#define HISTORY_SIZE        20
#define MIN_BASELINE        5       /* Minimum readings before fusion */
#define STUCK_THRESHOLD     5       /* Consecutive identical readings */
#define SPIKE_Z_THRESHOLD   4.0     /* Z-score for spike detection */
#define DRIFT_THRESHOLD     3.0     /* Group deviation threshold */
#define EPSILON             1e-10   /* For floating point comparison */

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

/**
 * Circular buffer for sensor history.
 */
typedef struct {
    double data[HISTORY_SIZE];
    size_t head;
    size_t count;
    double sum;
} CircularBuffer;

/**
 * Welford's online statistics.
 */
typedef struct {
    double mean;
    double M2;
    size_t count;
} WelfordState;

/**
 * Sensor status enumeration.
 */
typedef enum {
    SENSOR_NORMAL,      /* Operating correctly */
    SENSOR_SUSPECTED,   /* Possible fault detected */
    SENSOR_FAILED       /* Confirmed failure, excluded from fusion */
} SensorStatus;

/**
 * Individual sensor node.
 */
typedef struct {
    char id[32];
    CircularBuffer history;
    WelfordState stats;
    double weight;                  /* Fusion weight (0-1) */
    SensorStatus status;
    size_t consecutive_errors;      /* For failure confirmation */
    double last_value;              /* For stuck detection */
    size_t stuck_count;             /* Consecutive unchanged readings */
} SensorNode;

/**
 * Sensor array for fusion.
 */
typedef struct {
    SensorNode sensors[MAX_SENSORS];
    size_t count;
    size_t capacity;
    double group_mean;              /* Current group consensus */
    double group_variance;
    size_t total_readings;
} SensorArray;

/* =============================================================================
 * CIRCULAR BUFFER OPERATIONS
 * =============================================================================
 */

void cb_init(CircularBuffer *cb) {
    if (!cb) return;
    memset(cb->data, 0, sizeof(cb->data));
    cb->head = 0;
    cb->count = 0;
    cb->sum = 0.0;
}

void cb_push(CircularBuffer *cb, double value) {
    if (!cb) return;
    
    if (cb->count == HISTORY_SIZE) {
        cb->sum -= cb->data[cb->head];
    } else {
        cb->count++;
    }
    
    cb->data[cb->head] = value;
    cb->sum += value;
    cb->head = (cb->head + 1) % HISTORY_SIZE;
}

double cb_mean(CircularBuffer *cb) {
    if (!cb || cb->count == 0) return 0.0;
    return cb->sum / cb->count;
}

double cb_variance(CircularBuffer *cb) {
    if (!cb || cb->count < 2) return 0.0;
    
    double mean = cb_mean(cb);
    double sum_sq = 0.0;
    
    for (size_t i = 0; i < cb->count; i++) {
        size_t idx = (cb->head + HISTORY_SIZE - cb->count + i) % HISTORY_SIZE;
        double diff = cb->data[idx] - mean;
        sum_sq += diff * diff;
    }
    
    return sum_sq / (cb->count - 1);
}

double cb_oldest(CircularBuffer *cb) {
    if (!cb || cb->count == 0) return 0.0;
    size_t oldest_idx = (cb->head + HISTORY_SIZE - cb->count) % HISTORY_SIZE;
    return cb->data[oldest_idx];
}

double cb_newest(CircularBuffer *cb) {
    if (!cb || cb->count == 0) return 0.0;
    size_t newest_idx = (cb->head + HISTORY_SIZE - 1) % HISTORY_SIZE;
    return cb->data[newest_idx];
}

/* =============================================================================
 * WELFORD STATISTICS
 * =============================================================================
 */

void welford_init(WelfordState *state) {
    if (!state) return;
    state->mean = 0.0;
    state->M2 = 0.0;
    state->count = 0;
}

void welford_update(WelfordState *state, double value) {
    if (!state) return;
    
    state->count++;
    double delta = value - state->mean;
    state->mean += delta / state->count;
    double delta2 = value - state->mean;
    state->M2 += delta * delta2;
}

double welford_variance(WelfordState *state) {
    if (!state || state->count < 2) return 0.0;
    return state->M2 / (state->count - 1);
}

double welford_stddev(WelfordState *state) {
    return sqrt(welford_variance(state));
}

/* =============================================================================
 * PART A: SENSOR ARRAY MANAGEMENT
 * =============================================================================
 */

/**
 * Initialise sensor array.
 */
void sensor_array_init(SensorArray *arr) {
    if (!arr) return;
    
    arr->count = 0;
    arr->capacity = MAX_SENSORS;
    arr->group_mean = 0.0;
    arr->group_variance = 0.0;
    arr->total_readings = 0;
    
    for (size_t i = 0; i < MAX_SENSORS; i++) {
        arr->sensors[i].id[0] = '\0';
        cb_init(&arr->sensors[i].history);
        welford_init(&arr->sensors[i].stats);
        arr->sensors[i].weight = 1.0;
        arr->sensors[i].status = SENSOR_NORMAL;
        arr->sensors[i].consecutive_errors = 0;
        arr->sensors[i].last_value = 0.0;
        arr->sensors[i].stuck_count = 0;
    }
}

/**
 * Add a new sensor to the array.
 *
 * @param arr Sensor array
 * @param id  Sensor identifier string
 * @return Index of added sensor, or -1 on failure
 */
int sensor_array_add(SensorArray *arr, const char *id) {
    if (!arr || !id || arr->count >= arr->capacity) return -1;
    
    SensorNode *node = &arr->sensors[arr->count];
    
    strncpy(node->id, id, sizeof(node->id) - 1);
    node->id[sizeof(node->id) - 1] = '\0';
    
    cb_init(&node->history);
    welford_init(&node->stats);
    node->weight = 1.0;
    node->status = SENSOR_NORMAL;
    node->consecutive_errors = 0;
    node->last_value = 0.0;
    node->stuck_count = 0;
    
    return arr->count++;
}

/**
 * Get status string for display.
 */
const char *status_string(SensorStatus status) {
    switch (status) {
        case SENSOR_NORMAL:    return "NORMAL";
        case SENSOR_SUSPECTED: return "SUSPECTED";
        case SENSOR_FAILED:    return "FAILED";
        default:               return "UNKNOWN";
    }
}

/* =============================================================================
 * PART C: FAILURE DETECTION
 * =============================================================================
 */

/**
 * Detect stuck sensor (value unchanged for N readings).
 *
 * @param node      Sensor node
 * @param new_value Newly received value
 * @param threshold Number of consecutive identical readings
 * @return true if sensor appears stuck
 */
bool detect_stuck(SensorNode *node, double new_value, int threshold) {
    if (!node) return false;
    
    /* Check if value is same as last (within epsilon) */
    if (fabs(new_value - node->last_value) < EPSILON) {
        node->stuck_count++;
    } else {
        node->stuck_count = 0;
    }
    
    node->last_value = new_value;
    
    return (int)node->stuck_count >= threshold;
}

/**
 * Detect drift fault (gradual deviation from group consensus).
 *
 * @param node       Sensor node
 * @param group_mean Current group consensus mean
 * @param group_stddev Current group standard deviation
 * @param threshold  Number of standard deviations for drift detection
 * @return true if sensor shows drift fault
 */
bool detect_drift(SensorNode *node, double group_mean, 
                  double group_stddev, double threshold) {
    if (!node) return false;
    
    /* Need sufficient history */
    if (node->history.count < MIN_BASELINE) return false;
    
    /* If group stddev is too small, can't reliably detect drift */
    if (group_stddev < EPSILON) return false;
    
    /* Calculate sensor's current mean */
    double sensor_mean = cb_mean(&node->history);
    
    /* Calculate z-score of sensor's mean relative to group */
    double z = fabs(sensor_mean - group_mean) / group_stddev;
    
    return z > threshold;
}

/**
 * Detect spike fault (sudden large deviation).
 *
 * @param node      Sensor node
 * @param new_value Newly received value
 * @param threshold Z-score threshold for spike
 * @return true if value is a spike
 */
bool detect_spike(SensorNode *node, double new_value, double threshold) {
    if (!node) return false;
    
    /* Need sufficient baseline */
    if (node->stats.count < MIN_BASELINE) return false;
    
    double stddev = welford_stddev(&node->stats);
    
    /* If stddev is too small, can't detect spikes */
    if (stddev < EPSILON) return false;
    
    /* Calculate z-score */
    double z = fabs(new_value - node->stats.mean) / stddev;
    
    return z > threshold;
}

/**
 * Run all failure detection checks on a sensor.
 */
void check_sensor_health(SensorArray *arr, int idx, double new_value) {
    if (!arr || idx < 0 || idx >= (int)arr->count) return;
    
    SensorNode *node = &arr->sensors[idx];
    bool fault_detected = false;
    
    /* Check for stuck sensor */
    if (detect_stuck(node, new_value, STUCK_THRESHOLD)) {
        printf("    ⚠ Sensor %s: STUCK detected\n", node->id);
        fault_detected = true;
    }
    
    /* Check for spike */
    if (detect_spike(node, new_value, SPIKE_Z_THRESHOLD)) {
        printf("    ⚠ Sensor %s: SPIKE detected (value=%.2f)\n", 
               node->id, new_value);
        fault_detected = true;
    }
    
    /* Check for drift (uses group consensus) */
    if (arr->count > 1 && arr->total_readings > MIN_BASELINE * arr->count) {
        double group_stddev = sqrt(arr->group_variance);
        if (detect_drift(node, arr->group_mean, group_stddev, DRIFT_THRESHOLD)) {
            printf("    ⚠ Sensor %s: DRIFT detected\n", node->id);
            fault_detected = true;
        }
    }
    
    /* Update status based on consecutive errors */
    if (fault_detected) {
        node->consecutive_errors++;
        
        if (node->consecutive_errors >= 3 && node->status == SENSOR_NORMAL) {
            node->status = SENSOR_SUSPECTED;
            printf("    → Sensor %s: Status changed to SUSPECTED\n", node->id);
        } else if (node->consecutive_errors >= 5 && node->status == SENSOR_SUSPECTED) {
            node->status = SENSOR_FAILED;
            printf("    → Sensor %s: Status changed to FAILED\n", node->id);
        }
    } else {
        /* Gradually recover if no faults */
        if (node->consecutive_errors > 0) {
            node->consecutive_errors--;
        }
        
        /* Can recover from suspected (but not failed) */
        if (node->status == SENSOR_SUSPECTED && node->consecutive_errors == 0) {
            node->status = SENSOR_NORMAL;
            printf("    ✓ Sensor %s: Recovered to NORMAL\n", node->id);
        }
    }
}

/* =============================================================================
 * PART B: WEIGHTED AVERAGING FUSION
 * =============================================================================
 */

/**
 * Update sensor weights based on inverse variance.
 *
 * Sensors with lower variance (more consistent) get higher weight.
 */
void update_weights(SensorArray *arr) {
    if (!arr || arr->count == 0) return;
    
    double total_inverse_var = 0.0;
    
    /* Calculate inverse variances for normal sensors */
    for (size_t i = 0; i < arr->count; i++) {
        SensorNode *node = &arr->sensors[i];
        
        if (node->status != SENSOR_NORMAL) {
            node->weight = 0.0;
            continue;
        }
        
        double var = welford_variance(&node->stats);
        
        /* Avoid division by zero; use minimum variance */
        if (var < EPSILON) var = EPSILON;
        
        node->weight = 1.0 / var;
        total_inverse_var += node->weight;
    }
    
    /* Normalise weights to sum to 1 */
    if (total_inverse_var > EPSILON) {
        for (size_t i = 0; i < arr->count; i++) {
            arr->sensors[i].weight /= total_inverse_var;
        }
    }
}

/**
 * Calculate fused sensor estimate.
 *
 * Uses inverse-variance weighting (optimal for Gaussian sensors):
 *   fused = Σ(weight_i × value_i) / Σ(weight_i)
 *
 * where weight_i = 1/variance_i
 *
 * @param arr Sensor array
 * @return Fused estimate, or NAN if all sensors failed
 */
double sensor_fusion_estimate(SensorArray *arr) {
    if (!arr || arr->count == 0) return NAN;
    
    double weighted_sum = 0.0;
    double total_weight = 0.0;
    int normal_count = 0;
    
    for (size_t i = 0; i < arr->count; i++) {
        SensorNode *node = &arr->sensors[i];
        
        /* Only include normal sensors */
        if (node->status != SENSOR_NORMAL) continue;
        
        /* Need minimum baseline */
        if (node->history.count < MIN_BASELINE) continue;
        
        double latest = cb_newest(&node->history);
        weighted_sum += node->weight * latest;
        total_weight += node->weight;
        normal_count++;
    }
    
    /* Handle case where all sensors failed */
    if (normal_count == 0 || total_weight < EPSILON) {
        return NAN;
    }
    
    return weighted_sum / total_weight;
}

/**
 * Calculate fusion confidence (0-1 scale).
 *
 * Based on:
 *   - Number of functioning sensors
 *   - Agreement between sensors
 *   - Individual sensor variances
 */
double sensor_fusion_confidence(SensorArray *arr) {
    if (!arr || arr->count == 0) return 0.0;
    
    int normal_count = 0;
    double min_variance = DBL_MAX;
    double max_variance = 0.0;
    
    for (size_t i = 0; i < arr->count; i++) {
        SensorNode *node = &arr->sensors[i];
        
        if (node->status != SENSOR_NORMAL) continue;
        if (node->history.count < MIN_BASELINE) continue;
        
        normal_count++;
        double var = welford_variance(&node->stats);
        
        if (var < min_variance) min_variance = var;
        if (var > max_variance) max_variance = var;
    }
    
    if (normal_count == 0) return 0.0;
    
    /* Confidence factors */
    double sensor_factor = (double)normal_count / arr->count;
    
    /* Variance consistency (1.0 if all same, lower if different) */
    double var_ratio = 1.0;
    if (max_variance > EPSILON) {
        var_ratio = min_variance / max_variance;
    }
    
    /* Combined confidence */
    double confidence = 0.5 * sensor_factor + 0.5 * var_ratio;
    
    return confidence;
}

/**
 * Update group statistics (consensus).
 */
void update_group_stats(SensorArray *arr) {
    if (!arr || arr->count == 0) return;
    
    double sum = 0.0;
    double sum_sq = 0.0;
    int count = 0;
    
    for (size_t i = 0; i < arr->count; i++) {
        SensorNode *node = &arr->sensors[i];
        
        if (node->status != SENSOR_NORMAL) continue;
        if (node->history.count < MIN_BASELINE) continue;
        
        double value = cb_newest(&node->history);
        sum += value;
        sum_sq += value * value;
        count++;
    }
    
    if (count > 0) {
        arr->group_mean = sum / count;
        
        if (count > 1) {
            arr->group_variance = (sum_sq - sum * sum / count) / (count - 1);
        } else {
            arr->group_variance = 0.0;
        }
    }
}

/**
 * Process a new reading for a sensor.
 */
void sensor_array_update(SensorArray *arr, int idx, double value) {
    if (!arr || idx < 0 || idx >= (int)arr->count) return;
    
    SensorNode *node = &arr->sensors[idx];
    
    /* Check health before processing */
    check_sensor_health(arr, idx, value);
    
    /* Update history buffer */
    cb_push(&node->history, value);
    
    /* Update Welford statistics (only if not a spike) */
    if (!detect_spike(node, value, SPIKE_Z_THRESHOLD)) {
        welford_update(&node->stats, value);
    }
    
    arr->total_readings++;
    
    /* Update group consensus and weights periodically */
    if (arr->total_readings % arr->count == 0) {
        update_group_stats(arr);
        update_weights(arr);
    }
}

/* =============================================================================
 * DEMONSTRATION
 * =============================================================================
 */

/**
 * Generate simulated sensor reading with optional faults.
 */
double simulate_sensor(double base, double noise, double *drift,
                      double drift_rate, bool inject_stuck, bool inject_spike,
                      double *last_value) {
    if (inject_stuck && last_value) {
        return *last_value;  /* Return unchanged value */
    }
    
    double value = base + *drift + noise * (2.0 * rand() / RAND_MAX - 1.0);
    *drift += drift_rate;
    
    if (inject_spike) {
        value += 15.0;  /* Large spike */
    }
    
    if (last_value) *last_value = value;
    
    return value;
}

void print_array_status(SensorArray *arr) {
    printf("\n  Sensor Status:\n");
    printf("  ┌────────────┬──────────┬─────────┬─────────┬──────────┐\n");
    printf("  │   Sensor   │  Status  │ Weight  │  Mean   │  StdDev  │\n");
    printf("  ├────────────┼──────────┼─────────┼─────────┼──────────┤\n");
    
    for (size_t i = 0; i < arr->count; i++) {
        SensorNode *node = &arr->sensors[i];
        printf("  │ %-10s │ %-8s │  %5.3f  │ %7.2f │  %6.3f  │\n",
               node->id,
               status_string(node->status),
               node->weight,
               node->stats.count > 0 ? node->stats.mean : 0.0,
               welford_stddev(&node->stats));
    }
    
    printf("  └────────────┴──────────┴─────────┴─────────┴──────────┘\n");
}

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║       WEEK 19: MULTI-SENSOR FUSION SYSTEM - SOLUTION          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    srand((unsigned int)time(NULL));
    
    /* Initialise sensor array */
    SensorArray arr;
    sensor_array_init(&arr);
    
    /* Add sensors */
    sensor_array_add(&arr, "TEMP_A");
    sensor_array_add(&arr, "TEMP_B");
    sensor_array_add(&arr, "TEMP_C");
    sensor_array_add(&arr, "TEMP_D");
    
    printf("Configuration:\n");
    printf("  Sensors: %zu\n", arr.count);
    printf("  History size: %d\n", HISTORY_SIZE);
    printf("  Baseline readings: %d\n\n", MIN_BASELINE);
    
    /* Simulation parameters */
    double base_temp = 25.0;
    double drifts[4] = {0.0, 0.0, 0.0, 0.0};
    double last_values[4] = {25.0, 25.0, 25.0, 25.0};
    
    /* Run simulation */
    printf("Running simulation (50 iterations)...\n");
    printf("═════════════════════════════════════\n\n");
    
    for (int iter = 0; iter < 50; iter++) {
        printf("Iteration %d:\n", iter + 1);
        
        /* Generate readings for each sensor */
        for (int s = 0; s < 4; s++) {
            bool inject_stuck = (s == 1 && iter >= 20 && iter <= 30);
            bool inject_spike = (s == 2 && (iter == 15 || iter == 35));
            double drift_rate = (s == 3 && iter >= 25) ? 0.3 : 0.0;
            
            double value = simulate_sensor(
                base_temp, 0.3, &drifts[s], drift_rate,
                inject_stuck, inject_spike, &last_values[s]
            );
            
            printf("  %s reading: %.2f", arr.sensors[s].id, value);
            sensor_array_update(&arr, s, value);
            printf("\n");
        }
        
        /* Calculate and display fusion result */
        double fused = sensor_fusion_estimate(&arr);
        double confidence = sensor_fusion_confidence(&arr);
        
        if (!isnan(fused)) {
            printf("\n  Fused estimate: %.2f (confidence: %.1f%%)\n",
                   fused, confidence * 100);
        } else {
            printf("\n  ⚠ All sensors failed - no fusion available\n");
        }
        
        /* Periodic status display */
        if ((iter + 1) % 10 == 0) {
            print_array_status(&arr);
        }
        
        printf("\n");
    }
    
    /* Final summary */
    printf("═══════════════════════════════════\n");
    printf("           FINAL SUMMARY\n");
    printf("═══════════════════════════════════\n");
    
    print_array_status(&arr);
    
    double final_fused = sensor_fusion_estimate(&arr);
    double final_conf = sensor_fusion_confidence(&arr);
    
    printf("\n  Final fused estimate: %.2f\n", final_fused);
    printf("  Final confidence: %.1f%%\n", final_conf * 100);
    printf("  Total readings processed: %zu\n", arr.total_readings);
    
    int normal_count = 0, failed_count = 0;
    for (size_t i = 0; i < arr.count; i++) {
        if (arr.sensors[i].status == SENSOR_NORMAL) normal_count++;
        if (arr.sensors[i].status == SENSOR_FAILED) failed_count++;
    }
    printf("  Sensors: %d normal, %d failed\n", normal_count, failed_count);
    
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    DEMONSTRATION COMPLETE                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

/**
 * =============================================================================
 * WEEK 18: MACHINE LEARNING FUNDAMENTALS IN C
 * Homework 1 Solution: Complete ML Pipeline
 * =============================================================================
 *
 * COMPLETE SOLUTION - For Instructor Use Only
 *
 * This solution demonstrates:
 *   1. Comprehensive data loading and preprocessing
 *   2. Multiple model implementations (Linear Reg, K-NN, K-Means)
 *   3. Cross-validation framework
 *   4. Model comparison and selection
 *   5. Final evaluation report
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c -lm
 * Usage: ./homework1_sol
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <float.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

#define MAX_SAMPLES 500
#define MAX_FEATURES 10
#define NUM_FOLDS 5
#define LEARNING_RATE 0.01
#define MAX_ITERATIONS 500

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

typedef struct {
    double features[MAX_FEATURES];
    double target;
    int label;  /* For classification */
} DataSample;

typedef struct {
    double mean[MAX_FEATURES];
    double std_dev[MAX_FEATURES];
    double target_mean;
    double target_std;
    int n_features;
    bool fitted;
} Scaler;

typedef struct {
    double weights[MAX_FEATURES];
    double bias;
    int n_features;
} LinearRegressor;

typedef struct {
    DataSample *data;
    int n_samples;
    int k;
} KNNRegressor;

typedef struct {
    double centroids[10][MAX_FEATURES];
    int k;
    int n_features;
} KMeansRegressor;

typedef struct {
    double mse;
    double rmse;
    double mae;
    double r_squared;
} RegressionMetrics;

typedef struct {
    char name[32];
    double train_mse;
    double test_mse;
    double cv_mse;
    double cv_std;
} ModelResult;

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

static double random_normal(void) {
    double u1 = (double)rand() / RAND_MAX + 1e-10;
    double u2 = (double)rand() / RAND_MAX;
    return sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
}

static void shuffle_indices(int *indices, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }
}

/* =============================================================================
 * DATA GENERATION
 * =============================================================================
 */

/**
 * Generates a multivariate regression dataset
 * y = 3*x0 + 2*x1 - x2 + 0.5*x3 + noise
 */
static void generate_regression_data(DataSample *samples, int n, int n_features) {
    double true_weights[] = {3.0, 2.0, -1.0, 0.5, 0.3, -0.2, 0.1, 0.0, 0.0, 0.0};
    
    for (int i = 0; i < n; i++) {
        samples[i].target = 5.0;  /* Bias */
        
        for (int f = 0; f < n_features; f++) {
            samples[i].features[f] = random_normal() * 2.0 + 5.0;
            samples[i].target += true_weights[f] * samples[i].features[f];
        }
        
        /* Add noise */
        samples[i].target += random_normal() * 2.0;
    }
}

/* =============================================================================
 * SCALER
 * =============================================================================
 */

static void scaler_fit(Scaler *s, DataSample *samples, int n, int n_features) {
    s->n_features = n_features;
    
    /* Compute means */
    for (int f = 0; f < n_features; f++) {
        s->mean[f] = 0.0;
        for (int i = 0; i < n; i++) {
            s->mean[f] += samples[i].features[f];
        }
        s->mean[f] /= n;
    }
    
    s->target_mean = 0.0;
    for (int i = 0; i < n; i++) {
        s->target_mean += samples[i].target;
    }
    s->target_mean /= n;
    
    /* Compute std devs */
    for (int f = 0; f < n_features; f++) {
        s->std_dev[f] = 0.0;
        for (int i = 0; i < n; i++) {
            double diff = samples[i].features[f] - s->mean[f];
            s->std_dev[f] += diff * diff;
        }
        s->std_dev[f] = sqrt(s->std_dev[f] / n);
        if (s->std_dev[f] < 1e-10) s->std_dev[f] = 1.0;
    }
    
    s->target_std = 0.0;
    for (int i = 0; i < n; i++) {
        double diff = samples[i].target - s->target_mean;
        s->target_std += diff * diff;
    }
    s->target_std = sqrt(s->target_std / n);
    if (s->target_std < 1e-10) s->target_std = 1.0;
    
    s->fitted = true;
}

static void scaler_transform(Scaler *s, DataSample *samples, int n) {
    for (int i = 0; i < n; i++) {
        for (int f = 0; f < s->n_features; f++) {
            samples[i].features[f] = 
                (samples[i].features[f] - s->mean[f]) / s->std_dev[f];
        }
        samples[i].target = 
            (samples[i].target - s->target_mean) / s->target_std;
    }
}

static double scaler_inverse_target(Scaler *s, double normalised) {
    return normalised * s->target_std + s->target_mean;
}

/* =============================================================================
 * LINEAR REGRESSION
 * =============================================================================
 */

static void linear_init(LinearRegressor *lr, int n_features) {
    lr->n_features = n_features;
    for (int f = 0; f < n_features; f++) {
        lr->weights[f] = ((double)rand() / RAND_MAX - 0.5) * 0.1;
    }
    lr->bias = 0.0;
}

static double linear_predict(LinearRegressor *lr, double *features) {
    double pred = lr->bias;
    for (int f = 0; f < lr->n_features; f++) {
        pred += lr->weights[f] * features[f];
    }
    return pred;
}

static void linear_train(LinearRegressor *lr, DataSample *samples, int n) {
    for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
        double grad_w[MAX_FEATURES] = {0};
        double grad_b = 0.0;
        
        for (int i = 0; i < n; i++) {
            double pred = linear_predict(lr, samples[i].features);
            double error = pred - samples[i].target;
            
            for (int f = 0; f < lr->n_features; f++) {
                grad_w[f] += error * samples[i].features[f];
            }
            grad_b += error;
        }
        
        for (int f = 0; f < lr->n_features; f++) {
            lr->weights[f] -= LEARNING_RATE * grad_w[f] / n;
        }
        lr->bias -= LEARNING_RATE * grad_b / n;
    }
}

/* =============================================================================
 * K-NN REGRESSION
 * =============================================================================
 */

static void knn_init(KNNRegressor *knn, DataSample *data, int n, int k) {
    knn->data = data;
    knn->n_samples = n;
    knn->k = k;
}

static double knn_predict(KNNRegressor *knn, double *features, int n_features) {
    /* Compute all distances */
    typedef struct { double dist; double target; } DistTarget;
    DistTarget *dt = malloc(knn->n_samples * sizeof(DistTarget));
    
    for (int i = 0; i < knn->n_samples; i++) {
        double dist = 0.0;
        for (int f = 0; f < n_features; f++) {
            double diff = features[f] - knn->data[i].features[f];
            dist += diff * diff;
        }
        dt[i].dist = sqrt(dist);
        dt[i].target = knn->data[i].target;
    }
    
    /* Partial sort to find k nearest */
    for (int i = 0; i < knn->k; i++) {
        int min_idx = i;
        for (int j = i + 1; j < knn->n_samples; j++) {
            if (dt[j].dist < dt[min_idx].dist) {
                min_idx = j;
            }
        }
        DistTarget temp = dt[i];
        dt[i] = dt[min_idx];
        dt[min_idx] = temp;
    }
    
    /* Average of k nearest */
    double sum = 0.0;
    for (int i = 0; i < knn->k; i++) {
        sum += dt[i].target;
    }
    
    free(dt);
    return sum / knn->k;
}

/* =============================================================================
 * K-MEANS REGRESSION (cluster-based prediction)
 * =============================================================================
 */

static void kmeans_fit(KMeansRegressor *km, DataSample *samples, int n, int k, int n_features) {
    km->k = k;
    km->n_features = n_features;
    
    /* Random initialisation */
    for (int c = 0; c < k; c++) {
        int idx = rand() % n;
        for (int f = 0; f < n_features; f++) {
            km->centroids[c][f] = samples[idx].features[f];
        }
    }
    
    int *assignments = malloc(n * sizeof(int));
    
    /* Lloyd's algorithm */
    for (int iter = 0; iter < 50; iter++) {
        /* Assignment */
        for (int i = 0; i < n; i++) {
            double min_dist = DBL_MAX;
            for (int c = 0; c < k; c++) {
                double dist = 0.0;
                for (int f = 0; f < n_features; f++) {
                    double diff = samples[i].features[f] - km->centroids[c][f];
                    dist += diff * diff;
                }
                if (dist < min_dist) {
                    min_dist = dist;
                    assignments[i] = c;
                }
            }
        }
        
        /* Update */
        double sums[10][MAX_FEATURES] = {{0}};
        int counts[10] = {0};
        
        for (int i = 0; i < n; i++) {
            int c = assignments[i];
            counts[c]++;
            for (int f = 0; f < n_features; f++) {
                sums[c][f] += samples[i].features[f];
            }
        }
        
        for (int c = 0; c < k; c++) {
            if (counts[c] > 0) {
                for (int f = 0; f < n_features; f++) {
                    km->centroids[c][f] = sums[c][f] / counts[c];
                }
            }
        }
    }
    
    free(assignments);
}

static double kmeans_predict(KMeansRegressor *km, DataSample *train, int n_train,
                             double *features) {
    /* Find nearest centroid */
    int best_c = 0;
    double min_dist = DBL_MAX;
    
    for (int c = 0; c < km->k; c++) {
        double dist = 0.0;
        for (int f = 0; f < km->n_features; f++) {
            double diff = features[f] - km->centroids[c][f];
            dist += diff * diff;
        }
        if (dist < min_dist) {
            min_dist = dist;
            best_c = c;
        }
    }
    
    /* Average target of samples in that cluster */
    double sum = 0.0;
    int count = 0;
    
    for (int i = 0; i < n_train; i++) {
        double dist = 0.0;
        for (int f = 0; f < km->n_features; f++) {
            double diff = train[i].features[f] - km->centroids[best_c][f];
            dist += diff * diff;
        }
        
        /* Check if this sample belongs to same cluster */
        bool in_cluster = true;
        for (int c = 0; c < km->k; c++) {
            if (c == best_c) continue;
            double d = 0.0;
            for (int f = 0; f < km->n_features; f++) {
                double diff = train[i].features[f] - km->centroids[c][f];
                d += diff * diff;
            }
            if (d < dist) {
                in_cluster = false;
                break;
            }
        }
        
        if (in_cluster) {
            sum += train[i].target;
            count++;
        }
    }
    
    return (count > 0) ? sum / count : 0.0;
}

/* =============================================================================
 * EVALUATION
 * =============================================================================
 */

static RegressionMetrics compute_metrics(double *predictions, double *actuals, int n,
                                         Scaler *scaler) {
    RegressionMetrics m = {0};
    double mean_actual = 0.0;
    
    for (int i = 0; i < n; i++) {
        mean_actual += actuals[i];
    }
    mean_actual /= n;
    
    double ss_res = 0.0, ss_tot = 0.0;
    
    for (int i = 0; i < n; i++) {
        double pred_orig = scaler_inverse_target(scaler, predictions[i]);
        double actual_orig = scaler_inverse_target(scaler, actuals[i]);
        double error = pred_orig - actual_orig;
        
        m.mse += error * error;
        m.mae += fabs(error);
        
        double norm_error = predictions[i] - actuals[i];
        ss_res += norm_error * norm_error;
        ss_tot += (actuals[i] - mean_actual) * (actuals[i] - mean_actual);
    }
    
    m.mse /= n;
    m.rmse = sqrt(m.mse);
    m.mae /= n;
    m.r_squared = 1.0 - (ss_res / ss_tot);
    
    return m;
}

/* =============================================================================
 * CROSS-VALIDATION
 * =============================================================================
 */

static void cross_validate(DataSample *data, int n, int n_features,
                           const char *model_name, double *mean_mse, double *std_mse) {
    int fold_size = n / NUM_FOLDS;
    double fold_mses[NUM_FOLDS];
    
    int *indices = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) indices[i] = i;
    shuffle_indices(indices, n);
    
    for (int fold = 0; fold < NUM_FOLDS; fold++) {
        /* Split data */
        DataSample *train = malloc((n - fold_size) * sizeof(DataSample));
        DataSample *val = malloc(fold_size * sizeof(DataSample));
        
        int train_idx = 0, val_idx = 0;
        for (int i = 0; i < n; i++) {
            int idx = indices[i];
            if (i >= fold * fold_size && i < (fold + 1) * fold_size) {
                val[val_idx++] = data[idx];
            } else {
                train[train_idx++] = data[idx];
            }
        }
        
        int n_train = train_idx;
        int n_val = val_idx;
        
        /* Scale */
        Scaler scaler = {0};
        scaler_fit(&scaler, train, n_train, n_features);
        scaler_transform(&scaler, train, n_train);
        scaler_transform(&scaler, val, n_val);
        
        /* Train and evaluate */
        double mse = 0.0;
        
        if (strcmp(model_name, "linear") == 0) {
            LinearRegressor lr;
            linear_init(&lr, n_features);
            linear_train(&lr, train, n_train);
            
            for (int i = 0; i < n_val; i++) {
                double pred = linear_predict(&lr, val[i].features);
                double error = pred - val[i].target;
                mse += error * error;
            }
        } else if (strcmp(model_name, "knn") == 0) {
            KNNRegressor knn;
            knn_init(&knn, train, n_train, 5);
            
            for (int i = 0; i < n_val; i++) {
                double pred = knn_predict(&knn, val[i].features, n_features);
                double error = pred - val[i].target;
                mse += error * error;
            }
        } else if (strcmp(model_name, "kmeans") == 0) {
            KMeansRegressor km;
            kmeans_fit(&km, train, n_train, 5, n_features);
            
            for (int i = 0; i < n_val; i++) {
                double pred = kmeans_predict(&km, train, n_train, val[i].features);
                double error = pred - val[i].target;
                mse += error * error;
            }
        }
        
        fold_mses[fold] = mse / n_val;
        
        free(train);
        free(val);
    }
    
    /* Compute mean and std */
    *mean_mse = 0.0;
    for (int f = 0; f < NUM_FOLDS; f++) {
        *mean_mse += fold_mses[f];
    }
    *mean_mse /= NUM_FOLDS;
    
    *std_mse = 0.0;
    for (int f = 0; f < NUM_FOLDS; f++) {
        double diff = fold_mses[f] - *mean_mse;
        *std_mse += diff * diff;
    }
    *std_mse = sqrt(*std_mse / NUM_FOLDS);
    
    free(indices);
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 18: MACHINE LEARNING FUNDAMENTALS                    ║\n");
    printf("║     Homework 1 Solution: Complete ML Pipeline                 ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    srand(42);
    
    const int n_samples = 300;
    const int n_features = 4;
    
    DataSample *data = malloc(n_samples * sizeof(DataSample));
    DataSample *train = malloc(n_samples * sizeof(DataSample));
    DataSample *test = malloc(n_samples * sizeof(DataSample));
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      STEP 1: DATA GENERATION                                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    generate_regression_data(data, n_samples, n_features);
    printf("Generated %d samples with %d features\n", n_samples, n_features);
    printf("True model: y = 3*x0 + 2*x1 - x2 + 0.5*x3 + 5 + noise\n\n");
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      STEP 2: TRAIN/TEST SPLIT                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int n_train = (int)(n_samples * 0.8);
    int n_test = n_samples - n_train;
    
    int *indices = malloc(n_samples * sizeof(int));
    for (int i = 0; i < n_samples; i++) indices[i] = i;
    shuffle_indices(indices, n_samples);
    
    for (int i = 0; i < n_train; i++) {
        train[i] = data[indices[i]];
    }
    for (int i = 0; i < n_test; i++) {
        test[i] = data[indices[n_train + i]];
    }
    
    printf("Train: %d samples, Test: %d samples\n\n", n_train, n_test);
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      STEP 3: PREPROCESSING                                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Scaler scaler = {0};
    scaler_fit(&scaler, train, n_train, n_features);
    scaler_transform(&scaler, train, n_train);
    scaler_transform(&scaler, test, n_test);
    
    printf("Z-score normalisation applied\n\n");
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      STEP 4: CROSS-VALIDATION                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    ModelResult results[3];
    
    /* Linear Regression CV */
    strcpy(results[0].name, "Linear Regression");
    cross_validate(data, n_samples, n_features, "linear",
                   &results[0].cv_mse, &results[0].cv_std);
    
    /* K-NN CV */
    strcpy(results[1].name, "K-NN (k=5)");
    cross_validate(data, n_samples, n_features, "knn",
                   &results[1].cv_mse, &results[1].cv_std);
    
    /* K-Means CV */
    strcpy(results[2].name, "K-Means (k=5)");
    cross_validate(data, n_samples, n_features, "kmeans",
                   &results[2].cv_mse, &results[2].cv_std);
    
    printf("%d-Fold Cross-Validation Results:\n", NUM_FOLDS);
    printf("┌────────────────────┬────────────────┬────────────────┐\n");
    printf("│ Model              │ Mean MSE       │ Std MSE        │\n");
    printf("├────────────────────┼────────────────┼────────────────┤\n");
    for (int i = 0; i < 3; i++) {
        printf("│ %-18s │ %14.4f │ %14.4f │\n",
               results[i].name, results[i].cv_mse, results[i].cv_std);
    }
    printf("└────────────────────┴────────────────┴────────────────┘\n\n");
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      STEP 5: FINAL MODEL TRAINING AND EVALUATION              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Train all models on full training set */
    LinearRegressor lr;
    linear_init(&lr, n_features);
    linear_train(&lr, train, n_train);
    
    KNNRegressor knn;
    knn_init(&knn, train, n_train, 5);
    
    KMeansRegressor km;
    kmeans_fit(&km, train, n_train, 5, n_features);
    
    /* Evaluate on test set */
    double *preds = malloc(n_test * sizeof(double));
    double *actuals = malloc(n_test * sizeof(double));
    
    for (int i = 0; i < n_test; i++) {
        actuals[i] = test[i].target;
    }
    
    printf("Test Set Results:\n");
    printf("┌────────────────────┬────────────────┬────────────────┬────────────────┐\n");
    printf("│ Model              │ MSE            │ RMSE           │ R²             │\n");
    printf("├────────────────────┼────────────────┼────────────────┼────────────────┤\n");
    
    /* Linear Regression */
    for (int i = 0; i < n_test; i++) {
        preds[i] = linear_predict(&lr, test[i].features);
    }
    RegressionMetrics lr_metrics = compute_metrics(preds, actuals, n_test, &scaler);
    printf("│ %-18s │ %14.4f │ %14.4f │ %14.4f │\n",
           "Linear Regression", lr_metrics.mse, lr_metrics.rmse, lr_metrics.r_squared);
    
    /* K-NN */
    for (int i = 0; i < n_test; i++) {
        preds[i] = knn_predict(&knn, test[i].features, n_features);
    }
    RegressionMetrics knn_metrics = compute_metrics(preds, actuals, n_test, &scaler);
    printf("│ %-18s │ %14.4f │ %14.4f │ %14.4f │\n",
           "K-NN (k=5)", knn_metrics.mse, knn_metrics.rmse, knn_metrics.r_squared);
    
    /* K-Means */
    for (int i = 0; i < n_test; i++) {
        preds[i] = kmeans_predict(&km, train, n_train, test[i].features);
    }
    RegressionMetrics km_metrics = compute_metrics(preds, actuals, n_test, &scaler);
    printf("│ %-18s │ %14.4f │ %14.4f │ %14.4f │\n",
           "K-Means (k=5)", km_metrics.mse, km_metrics.rmse, km_metrics.r_squared);
    
    printf("└────────────────────┴────────────────┴────────────────┴────────────────┘\n\n");
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      STEP 6: MODEL SELECTION                                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Find best model by CV MSE */
    int best_idx = 0;
    for (int i = 1; i < 3; i++) {
        if (results[i].cv_mse < results[best_idx].cv_mse) {
            best_idx = i;
        }
    }
    
    printf("Best model based on %d-fold CV: %s\n", NUM_FOLDS, results[best_idx].name);
    printf("CV MSE: %.4f (±%.4f)\n\n", results[best_idx].cv_mse, results[best_idx].cv_std);
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      LEARNED COEFFICIENTS (Linear Regression)                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("True coefficients: w = [3.0, 2.0, -1.0, 0.5], b = 5.0\n");
    printf("Learned (normalised): w = [");
    for (int f = 0; f < n_features; f++) {
        printf("%.3f%s", lr.weights[f], (f < n_features - 1) ? ", " : "");
    }
    printf("], b = %.3f\n\n", lr.bias);
    
    /* Clean up */
    free(data);
    free(train);
    free(test);
    free(indices);
    free(preds);
    free(actuals);
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    HOMEWORK 1 COMPLETE                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

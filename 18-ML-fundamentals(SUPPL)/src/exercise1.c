/**
 * =============================================================================
 * WEEK 18: MACHINE LEARNING FUNDAMENTALS IN C
 * Exercise 1: House Price Prediction with Linear Regression
 * =============================================================================
 *
 * OBJECTIVE:
 * Implement a complete linear regression pipeline for predicting house prices
 * based on features such as square footage, number of bedrooms and age.
 *
 * This exercise reinforces:
 *   1. Data loading and preprocessing
 *   2. Feature normalisation (z-score)
 *   3. Gradient descent implementation
 *   4. Model evaluation metrics (MSE, R²)
 *   5. Train/test split methodology
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c -lm
 * Usage: ./exercise1 [data/housing.csv]
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

#define MAX_SAMPLES 200
#define MAX_FEATURES 5
#define MAX_LINE_LENGTH 256
#define LEARNING_RATE 0.01
#define MAX_ITERATIONS 1000
#define CONVERGENCE_THRESHOLD 1e-6

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

/**
 * Dataset structure holding features and targets.
 */
typedef struct {
    double features[MAX_SAMPLES][MAX_FEATURES];
    double targets[MAX_SAMPLES];
    size_t n_samples;
    size_t n_features;
    char feature_names[MAX_FEATURES][32];
} Dataset;

/**
 * Linear Regression Model.
 */
typedef struct {
    double *weights;
    double bias;
    size_t n_features;
} LinearModel;

/**
 * Normalisation parameters for each feature.
 */
typedef struct {
    double *means;
    double *stds;
    size_t n_features;
} Normaliser;

/**
 * Training history for visualisation.
 */
typedef struct {
    double *losses;
    size_t n_iterations;
} TrainingHistory;

/* =============================================================================
 * PART 1: DATA LOADING AND PREPROCESSING
 * =============================================================================
 */

/**
 * Generates synthetic housing dataset.
 * 
 * Features:
 *   - Square footage (800-4000 sq ft)
 *   - Number of bedrooms (1-5)
 *   - Age of house (0-50 years)
 * 
 * Target: Price = 50*sqft + 10000*bedrooms - 1000*age + 100000 + noise
 *
 * @param dataset Pointer to dataset structure to fill
 * @param n_samples Number of samples to generate
 */
void generate_housing_data(Dataset *dataset, size_t n_samples) {
    printf("Generating synthetic housing dataset...\n");
    
    dataset->n_samples = n_samples;
    dataset->n_features = 3;
    
    strcpy(dataset->feature_names[0], "sqft");
    strcpy(dataset->feature_names[1], "bedrooms");
    strcpy(dataset->feature_names[2], "age");
    
    srand(42);  /* Fixed seed for reproducibility */
    
    for (size_t i = 0; i < n_samples; i++) {
        /* Generate features */
        double sqft = 800 + (rand() % 3200);
        double bedrooms = 1 + (rand() % 5);
        double age = rand() % 51;
        
        dataset->features[i][0] = sqft;
        dataset->features[i][1] = bedrooms;
        dataset->features[i][2] = age;
        
        /* Generate target with known relationship + noise */
        double noise = ((double)rand() / RAND_MAX - 0.5) * 50000;
        dataset->targets[i] = 50 * sqft + 10000 * bedrooms - 1000 * age + 100000 + noise;
    }
    
    printf("  Generated %zu samples with %zu features\n", 
           dataset->n_samples, dataset->n_features);
}

/**
 * Loads dataset from CSV file.
 * Expected format: feature1,feature2,...,target
 * First line should be header with column names.
 *
 * @param filename Path to CSV file
 * @param dataset Pointer to dataset structure to fill
 * @return true on success, false on failure
 */
bool load_csv(const char *filename, Dataset *dataset) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Warning: Could not open %s, using synthetic data\n", filename);
        return false;
    }
    
    char line[MAX_LINE_LENGTH];
    
    /* Read header */
    if (fgets(line, MAX_LINE_LENGTH, file) == NULL) {
        fclose(file);
        return false;
    }
    
    /* TODO: Parse header to get feature names */
    /* Hint: Use strtok to split by comma */
    
    /* TODO: Read data rows */
    /* Hint: Parse each line, storing features and target */
    
    fclose(file);
    printf("  Loaded %zu samples from %s\n", dataset->n_samples, filename);
    return true;
}

/**
 * Splits dataset into training and test sets.
 *
 * @param full Full dataset
 * @param train Training set (output)
 * @param test Test set (output)
 * @param train_ratio Proportion of data for training (e.g., 0.8)
 */
void train_test_split(const Dataset *full, Dataset *train, Dataset *test, 
                      double train_ratio) {
    size_t train_size = (size_t)(full->n_samples * train_ratio);
    size_t test_size = full->n_samples - train_size;
    
    train->n_samples = train_size;
    train->n_features = full->n_features;
    test->n_samples = test_size;
    test->n_features = full->n_features;
    
    /* Copy feature names */
    for (size_t j = 0; j < full->n_features; j++) {
        strcpy(train->feature_names[j], full->feature_names[j]);
        strcpy(test->feature_names[j], full->feature_names[j]);
    }
    
    /* TODO: Implement the split */
    /* Copy first train_size samples to train */
    /* Copy remaining samples to test */
    
    /* ===== YOUR CODE HERE ===== */
    for (size_t i = 0; i < train_size; i++) {
        for (size_t j = 0; j < full->n_features; j++) {
            train->features[i][j] = full->features[i][j];
        }
        train->targets[i] = full->targets[i];
    }
    
    for (size_t i = 0; i < test_size; i++) {
        for (size_t j = 0; j < full->n_features; j++) {
            test->features[i][j] = full->features[train_size + i][j];
        }
        test->targets[i] = full->targets[train_size + i];
    }
    /* ===== END YOUR CODE ===== */
    
    printf("  Train/Test split: %zu / %zu samples\n", train_size, test_size);
}

/* =============================================================================
 * PART 2: FEATURE NORMALISATION
 * =============================================================================
 */

/**
 * Creates a normaliser and fits it to training data.
 * Uses z-score normalisation: x' = (x - mean) / std
 *
 * @param n_features Number of features
 * @return Pointer to normaliser, NULL on failure
 */
Normaliser *normaliser_create(size_t n_features) {
    Normaliser *norm = (Normaliser *)malloc(sizeof(Normaliser));
    if (norm == NULL) return NULL;
    
    norm->n_features = n_features;
    norm->means = (double *)calloc(n_features, sizeof(double));
    norm->stds = (double *)calloc(n_features, sizeof(double));
    
    if (norm->means == NULL || norm->stds == NULL) {
        free(norm->means);
        free(norm->stds);
        free(norm);
        return NULL;
    }
    
    return norm;
}

/**
 * Frees normaliser memory.
 */
void normaliser_free(Normaliser *norm) {
    if (norm != NULL) {
        free(norm->means);
        free(norm->stds);
        free(norm);
    }
}

/**
 * Fits normaliser to data (computes means and stds).
 *
 * @param norm Normaliser to fit
 * @param dataset Dataset to compute statistics from
 */
void normaliser_fit(Normaliser *norm, const Dataset *dataset) {
    /* TODO: Compute mean and std for each feature */
    
    /* ===== YOUR CODE HERE ===== */
    /* Step 1: Compute means */
    for (size_t j = 0; j < dataset->n_features; j++) {
        double sum = 0.0;
        for (size_t i = 0; i < dataset->n_samples; i++) {
            sum += dataset->features[i][j];
        }
        norm->means[j] = sum / dataset->n_samples;
    }
    
    /* Step 2: Compute standard deviations */
    for (size_t j = 0; j < dataset->n_features; j++) {
        double variance = 0.0;
        for (size_t i = 0; i < dataset->n_samples; i++) {
            double diff = dataset->features[i][j] - norm->means[j];
            variance += diff * diff;
        }
        norm->stds[j] = sqrt(variance / dataset->n_samples);
        
        /* Prevent division by zero */
        if (norm->stds[j] < 1e-10) {
            norm->stds[j] = 1.0;
        }
    }
    /* ===== END YOUR CODE ===== */
    
    printf("  Normaliser fitted:\n");
    for (size_t j = 0; j < dataset->n_features; j++) {
        printf("    Feature %zu: mean=%.2f, std=%.2f\n", 
               j, norm->means[j], norm->stds[j]);
    }
}

/**
 * Transforms dataset using fitted normaliser.
 *
 * @param norm Fitted normaliser
 * @param dataset Dataset to transform (modified in place)
 */
void normaliser_transform(const Normaliser *norm, Dataset *dataset) {
    /* TODO: Apply z-score normalisation to each feature */
    
    /* ===== YOUR CODE HERE ===== */
    for (size_t i = 0; i < dataset->n_samples; i++) {
        for (size_t j = 0; j < dataset->n_features; j++) {
            dataset->features[i][j] = 
                (dataset->features[i][j] - norm->means[j]) / norm->stds[j];
        }
    }
    /* ===== END YOUR CODE ===== */
}

/* =============================================================================
 * PART 3: LINEAR REGRESSION MODEL
 * =============================================================================
 */

/**
 * Creates a linear regression model.
 *
 * @param n_features Number of input features
 * @return Pointer to model, NULL on failure
 */
LinearModel *model_create(size_t n_features) {
    LinearModel *model = (LinearModel *)malloc(sizeof(LinearModel));
    if (model == NULL) return NULL;
    
    model->n_features = n_features;
    model->weights = (double *)calloc(n_features, sizeof(double));
    model->bias = 0.0;
    
    if (model->weights == NULL) {
        free(model);
        return NULL;
    }
    
    return model;
}

/**
 * Frees model memory.
 */
void model_free(LinearModel *model) {
    if (model != NULL) {
        free(model->weights);
        free(model);
    }
}

/**
 * Predicts target for a single sample.
 *
 * @param model Linear regression model
 * @param features Feature values for one sample
 * @return Predicted value
 */
double model_predict_single(const LinearModel *model, const double *features) {
    double prediction = model->bias;
    
    /* TODO: Compute weighted sum */
    /* prediction = bias + sum(weight[j] * feature[j]) */
    
    /* ===== YOUR CODE HERE ===== */
    for (size_t j = 0; j < model->n_features; j++) {
        prediction += model->weights[j] * features[j];
    }
    /* ===== END YOUR CODE ===== */
    
    return prediction;
}

/**
 * Computes Mean Squared Error.
 * MSE = (1/n) * sum((y_pred - y_true)^2)
 *
 * @param model Model to evaluate
 * @param dataset Dataset to evaluate on
 * @return MSE value
 */
double compute_mse(const LinearModel *model, const Dataset *dataset) {
    double mse = 0.0;
    
    /* TODO: Compute MSE */
    
    /* ===== YOUR CODE HERE ===== */
    for (size_t i = 0; i < dataset->n_samples; i++) {
        double pred = model_predict_single(model, dataset->features[i]);
        double diff = pred - dataset->targets[i];
        mse += diff * diff;
    }
    mse /= dataset->n_samples;
    /* ===== END YOUR CODE ===== */
    
    return mse;
}

/**
 * Computes R² (coefficient of determination).
 * R² = 1 - (SS_res / SS_tot)
 *
 * @param model Model to evaluate
 * @param dataset Dataset to evaluate on
 * @return R² value (1.0 is perfect)
 */
double compute_r2(const LinearModel *model, const Dataset *dataset) {
    /* TODO: Compute R² score */
    
    /* ===== YOUR CODE HERE ===== */
    /* Step 1: Compute mean of targets */
    double mean_y = 0.0;
    for (size_t i = 0; i < dataset->n_samples; i++) {
        mean_y += dataset->targets[i];
    }
    mean_y /= dataset->n_samples;
    
    /* Step 2: Compute SS_res and SS_tot */
    double ss_res = 0.0, ss_tot = 0.0;
    for (size_t i = 0; i < dataset->n_samples; i++) {
        double pred = model_predict_single(model, dataset->features[i]);
        ss_res += (dataset->targets[i] - pred) * (dataset->targets[i] - pred);
        ss_tot += (dataset->targets[i] - mean_y) * (dataset->targets[i] - mean_y);
    }
    
    /* Step 3: Compute R² */
    if (ss_tot < 1e-10) return 0.0;
    return 1.0 - (ss_res / ss_tot);
    /* ===== END YOUR CODE ===== */
}

/* =============================================================================
 * PART 4: GRADIENT DESCENT TRAINING
 * =============================================================================
 */

/**
 * Trains linear regression model using gradient descent.
 *
 * Gradient update rules:
 *   w[j] = w[j] - α * (2/n) * Σ x[i][j] * (y_pred[i] - y_true[i])
 *   b = b - α * (2/n) * Σ (y_pred[i] - y_true[i])
 *
 * @param model Model to train
 * @param dataset Training dataset
 * @param learning_rate Learning rate α
 * @param max_iterations Maximum number of iterations
 * @param verbose Print progress if true
 * @return Training history
 */
TrainingHistory *model_fit(LinearModel *model, const Dataset *dataset,
                           double learning_rate, size_t max_iterations,
                           bool verbose) {
    TrainingHistory *history = (TrainingHistory *)malloc(sizeof(TrainingHistory));
    history->losses = (double *)malloc(max_iterations * sizeof(double));
    history->n_iterations = 0;
    
    double *predictions = (double *)malloc(dataset->n_samples * sizeof(double));
    double *grad_w = (double *)calloc(model->n_features, sizeof(double));
    
    if (verbose) {
        printf("\nTraining Linear Regression...\n");
        printf("  Learning rate: %.4f\n", learning_rate);
        printf("  Max iterations: %zu\n\n", max_iterations);
        printf("  Iter      MSE           R²\n");
        printf("  ─────────────────────────────\n");
    }
    
    double prev_loss = INFINITY;
    
    for (size_t iter = 0; iter < max_iterations; iter++) {
        /* Forward pass: compute predictions */
        for (size_t i = 0; i < dataset->n_samples; i++) {
            predictions[i] = model_predict_single(model, dataset->features[i]);
        }
        
        /* Compute loss */
        double mse = compute_mse(model, dataset);
        double r2 = compute_r2(model, dataset);
        history->losses[iter] = mse;
        history->n_iterations = iter + 1;
        
        if (verbose && (iter < 10 || iter % 100 == 0)) {
            printf("  %4zu    %10.2f    %7.4f\n", iter, mse, r2);
        }
        
        /* Check convergence */
        if (fabs(prev_loss - mse) < CONVERGENCE_THRESHOLD) {
            if (verbose) {
                printf("\n  Converged at iteration %zu\n", iter);
            }
            break;
        }
        prev_loss = mse;
        
        /* TODO: Compute gradients and update parameters */
        
        /* ===== YOUR CODE HERE ===== */
        /* Reset gradients */
        memset(grad_w, 0, model->n_features * sizeof(double));
        double grad_b = 0.0;
        
        /* Accumulate gradients */
        for (size_t i = 0; i < dataset->n_samples; i++) {
            double error = predictions[i] - dataset->targets[i];
            for (size_t j = 0; j < model->n_features; j++) {
                grad_w[j] += dataset->features[i][j] * error;
            }
            grad_b += error;
        }
        
        /* Update parameters */
        for (size_t j = 0; j < model->n_features; j++) {
            model->weights[j] -= learning_rate * (2.0 / dataset->n_samples) * grad_w[j];
        }
        model->bias -= learning_rate * (2.0 / dataset->n_samples) * grad_b;
        /* ===== END YOUR CODE ===== */
    }
    
    free(predictions);
    free(grad_w);
    
    return history;
}

/**
 * Frees training history.
 */
void history_free(TrainingHistory *history) {
    if (history != NULL) {
        free(history->losses);
        free(history);
    }
}

/* =============================================================================
 * PART 5: VISUALISATION
 * =============================================================================
 */

/**
 * Prints ASCII learning curve.
 *
 * @param history Training history
 */
void plot_learning_curve(const TrainingHistory *history) {
    printf("\nLearning Curve (Loss vs Iterations):\n");
    printf("────────────────────────────────────────────────────────────\n");
    
    /* Find max loss for scaling */
    double max_loss = 0.0;
    for (size_t i = 0; i < history->n_iterations; i++) {
        if (history->losses[i] > max_loss) {
            max_loss = history->losses[i];
        }
    }
    
    int height = 10;
    int width = 50;
    
    for (int row = height; row >= 0; row--) {
        double threshold = max_loss * row / height;
        printf("%10.0f │", threshold);
        
        for (int col = 0; col < width; col++) {
            size_t idx = (col * history->n_iterations) / width;
            if (idx < history->n_iterations && history->losses[idx] >= threshold) {
                printf("█");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
    
    printf("           └");
    for (int col = 0; col < width; col++) printf("─");
    printf("\n");
    printf("            0%s%zu\n", 
           "                                   ",
           history->n_iterations);
    printf("                              Iterations\n");
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 1: House Price Prediction                        ║\n");
    printf("║     Linear Regression with Gradient Descent                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Step 1: Load or generate data */
    Dataset full_data;
    
    if (argc > 1) {
        if (!load_csv(argv[1], &full_data)) {
            generate_housing_data(&full_data, 100);
        }
    } else {
        generate_housing_data(&full_data, 100);
    }
    
    /* Step 2: Split into train/test */
    Dataset train_data, test_data;
    train_test_split(&full_data, &train_data, &test_data, 0.8);
    
    /* Step 3: Create and fit normaliser */
    Normaliser *norm = normaliser_create(train_data.n_features);
    normaliser_fit(norm, &train_data);
    normaliser_transform(norm, &train_data);
    normaliser_transform(norm, &test_data);
    
    /* Step 4: Create model */
    LinearModel *model = model_create(train_data.n_features);
    
    /* Step 5: Train model */
    TrainingHistory *history = model_fit(model, &train_data, 
                                         LEARNING_RATE, MAX_ITERATIONS, true);
    
    /* Step 6: Evaluate on test set */
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                      EVALUATION RESULTS                        \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    double train_mse = compute_mse(model, &train_data);
    double train_r2 = compute_r2(model, &train_data);
    double test_mse = compute_mse(model, &test_data);
    double test_r2 = compute_r2(model, &test_data);
    
    printf("Training Set:\n");
    printf("  MSE: %.2f\n", train_mse);
    printf("  R²:  %.4f\n\n", train_r2);
    
    printf("Test Set:\n");
    printf("  MSE: %.2f\n", test_mse);
    printf("  R²:  %.4f\n\n", test_r2);
    
    /* Step 7: Show learned parameters */
    printf("Learned Parameters:\n");
    for (size_t j = 0; j < model->n_features; j++) {
        printf("  w[%s] = %.4f\n", train_data.feature_names[j], model->weights[j]);
    }
    printf("  bias = %.4f\n", model->bias);
    
    /* Step 8: Plot learning curve */
    plot_learning_curve(history);
    
    /* Cleanup */
    model_free(model);
    normaliser_free(norm);
    history_free(history);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    EXERCISE 1 COMPLETE                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

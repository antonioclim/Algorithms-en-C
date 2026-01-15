/**
 * =============================================================================
 * WEEK 18: MACHINE LEARNING FUNDAMENTALS IN C
 * Exercise 1 Solution: House Price Prediction with Linear Regression
 * =============================================================================
 *
 * COMPLETE SOLUTION - For Instructor Use Only
 *
 * This solution demonstrates:
 *   1. Data loading and preprocessing
 *   2. Train/test split (80/20)
 *   3. Z-score normalisation
 *   4. Gradient descent training
 *   5. Model evaluation (MSE, R²)
 *   6. Learning curve visualisation
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c -lm
 * Usage: ./exercise1_sol
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
 * CONSTANTS AND CONFIGURATION
 * =============================================================================
 */

#define MAX_SAMPLES 1000
#define NUM_FEATURES 3          /* sqft, bedrooms, age */
#define LEARNING_RATE 0.01
#define MAX_ITERATIONS 1000
#define CONVERGENCE_THRESHOLD 1e-6
#define TRAIN_RATIO 0.8

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

typedef struct {
    double features[NUM_FEATURES];
    double price;
} HouseSample;

typedef struct {
    double mean[NUM_FEATURES];
    double std_dev[NUM_FEATURES];
    double price_mean;
    double price_std;
    bool fitted;
} Normaliser;

typedef struct {
    double weights[NUM_FEATURES];
    double bias;
    double learning_rate;
    int iterations_trained;
    double final_loss;
} LinearModel;

typedef struct {
    double mse;
    double rmse;
    double mae;
    double r_squared;
} EvaluationMetrics;

/* =============================================================================
 * PART 1: DATA GENERATION (SYNTHETIC HOUSING DATA)
 * =============================================================================
 *
 * Generates synthetic housing data with known relationships:
 *   price = 50 * sqft + 10000 * bedrooms - 1000 * age + 100000 + noise
 *
 * This allows us to verify our model learns the correct coefficients.
 */

/**
 * Generates a random double in the specified range
 */
static double random_range(double min, double max) {
    return min + (max - min) * ((double)rand() / RAND_MAX);
}

/**
 * Generates a normally distributed random value using Box-Muller transform
 */
static double random_normal(double mean, double std_dev) {
    double u1 = (double)rand() / RAND_MAX;
    double u2 = (double)rand() / RAND_MAX;
    
    /* Avoid log(0) */
    if (u1 < 1e-10) u1 = 1e-10;
    
    double z = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
    return mean + std_dev * z;
}

/**
 * Generates synthetic housing dataset
 *
 * True relationship:
 *   price = 50 * sqft + 10000 * bedrooms - 1000 * age + 100000
 *
 * @param samples Output array for generated samples
 * @param n Number of samples to generate
 */
static void generate_housing_data(HouseSample *samples, int n) {
    printf("Generating %d synthetic housing samples...\n", n);
    printf("True relationship: price = 50*sqft + 10000*bedrooms - 1000*age + 100000\n\n");
    
    for (int i = 0; i < n; i++) {
        /* Generate features with realistic ranges */
        double sqft = random_range(800, 3500);       /* Square footage */
        double bedrooms = floor(random_range(1, 6)); /* 1-5 bedrooms */
        double age = random_range(0, 50);            /* House age in years */
        
        /* Calculate price using true model with Gaussian noise */
        double noise = random_normal(0, 15000);
        double price = 50.0 * sqft + 10000.0 * bedrooms - 1000.0 * age + 100000.0 + noise;
        
        /* Store sample */
        samples[i].features[0] = sqft;
        samples[i].features[1] = bedrooms;
        samples[i].features[2] = age;
        samples[i].price = price;
    }
}

/**
 * Loads housing data from CSV file
 *
 * Expected format: sqft,bedrooms,age,price
 *
 * @param filename Path to CSV file
 * @param samples Output array for loaded samples
 * @param max_samples Maximum number of samples to load
 * @return Number of samples loaded, or -1 on error
 */
static int load_housing_csv(const char *filename, HouseSample *samples, int max_samples) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Warning: Could not open %s, using synthetic data\n", filename);
        return -1;
    }
    
    char line[256];
    int count = 0;
    
    /* Skip header line */
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return -1;
    }
    
    /* Read data lines */
    while (fgets(line, sizeof(line), fp) && count < max_samples) {
        double sqft, bedrooms, age, price;
        
        if (sscanf(line, "%lf,%lf,%lf,%lf", &sqft, &bedrooms, &age, &price) == 4) {
            samples[count].features[0] = sqft;
            samples[count].features[1] = bedrooms;
            samples[count].features[2] = age;
            samples[count].price = price;
            count++;
        }
    }
    
    fclose(fp);
    printf("Loaded %d samples from %s\n", count, filename);
    return count;
}

/* =============================================================================
 * PART 2: TRAIN/TEST SPLIT
 * =============================================================================
 *
 * Shuffles data and splits into training and test sets.
 * This is crucial for evaluating model generalisation.
 */

/**
 * Fisher-Yates shuffle for randomising sample order
 */
static void shuffle_samples(HouseSample *samples, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        HouseSample temp = samples[i];
        samples[i] = samples[j];
        samples[j] = temp;
    }
}

/**
 * Splits dataset into training and test sets
 *
 * @param all_samples Complete dataset
 * @param n Total number of samples
 * @param train_samples Output array for training samples
 * @param test_samples Output array for test samples
 * @param train_ratio Proportion for training (e.g., 0.8)
 * @param n_train Output: number of training samples
 * @param n_test Output: number of test samples
 */
static void train_test_split(HouseSample *all_samples, int n,
                             HouseSample *train_samples, HouseSample *test_samples,
                             double train_ratio, int *n_train, int *n_test) {
    /* Shuffle data first */
    shuffle_samples(all_samples, n);
    
    *n_train = (int)(n * train_ratio);
    *n_test = n - *n_train;
    
    /* Copy to respective arrays */
    for (int i = 0; i < *n_train; i++) {
        train_samples[i] = all_samples[i];
    }
    
    for (int i = 0; i < *n_test; i++) {
        test_samples[i] = all_samples[*n_train + i];
    }
    
    printf("Train/Test split: %d training, %d test samples (%.0f%%/%.0f%%)\n",
           *n_train, *n_test, train_ratio * 100, (1 - train_ratio) * 100);
}

/* =============================================================================
 * PART 3: NORMALISATION
 * =============================================================================
 *
 * Z-score normalisation: x_norm = (x - mean) / std_dev
 *
 * IMPORTANT: Fit normaliser on training data only, then transform both
 * training and test data using the same parameters.
 */

/**
 * Fits normaliser parameters on training data
 *
 * Computes mean and standard deviation for each feature
 */
static void normaliser_fit(Normaliser *norm, HouseSample *samples, int n) {
    /* Initialise accumulators */
    for (int j = 0; j < NUM_FEATURES; j++) {
        norm->mean[j] = 0.0;
        norm->std_dev[j] = 0.0;
    }
    norm->price_mean = 0.0;
    norm->price_std = 0.0;
    
    /* Compute means */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < NUM_FEATURES; j++) {
            norm->mean[j] += samples[i].features[j];
        }
        norm->price_mean += samples[i].price;
    }
    
    for (int j = 0; j < NUM_FEATURES; j++) {
        norm->mean[j] /= n;
    }
    norm->price_mean /= n;
    
    /* Compute standard deviations */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < NUM_FEATURES; j++) {
            double diff = samples[i].features[j] - norm->mean[j];
            norm->std_dev[j] += diff * diff;
        }
        double diff = samples[i].price - norm->price_mean;
        norm->price_std += diff * diff;
    }
    
    for (int j = 0; j < NUM_FEATURES; j++) {
        norm->std_dev[j] = sqrt(norm->std_dev[j] / n);
        /* Prevent division by zero */
        if (norm->std_dev[j] < 1e-10) norm->std_dev[j] = 1.0;
    }
    norm->price_std = sqrt(norm->price_std / n);
    if (norm->price_std < 1e-10) norm->price_std = 1.0;
    
    norm->fitted = true;
    
    printf("\nNormalisation parameters (fitted on training data):\n");
    printf("┌──────────────┬────────────────┬────────────────┐\n");
    printf("│ Feature      │ Mean           │ Std Dev        │\n");
    printf("├──────────────┼────────────────┼────────────────┤\n");
    printf("│ sqft         │ %14.2f │ %14.2f │\n", norm->mean[0], norm->std_dev[0]);
    printf("│ bedrooms     │ %14.2f │ %14.2f │\n", norm->mean[1], norm->std_dev[1]);
    printf("│ age          │ %14.2f │ %14.2f │\n", norm->mean[2], norm->std_dev[2]);
    printf("│ price        │ %14.2f │ %14.2f │\n", norm->price_mean, norm->price_std);
    printf("└──────────────┴────────────────┴────────────────┘\n");
}

/**
 * Transforms samples using fitted normaliser
 */
static void normaliser_transform(Normaliser *norm, HouseSample *samples, int n) {
    if (!norm->fitted) {
        fprintf(stderr, "Error: Normaliser must be fitted before transform\n");
        return;
    }
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < NUM_FEATURES; j++) {
            samples[i].features[j] = 
                (samples[i].features[j] - norm->mean[j]) / norm->std_dev[j];
        }
        samples[i].price = 
            (samples[i].price - norm->price_mean) / norm->price_std;
    }
}

/**
 * Inverse transform to get original scale predictions
 */
static double inverse_transform_price(Normaliser *norm, double normalised_price) {
    return normalised_price * norm->price_std + norm->price_mean;
}

/* =============================================================================
 * PART 4: LINEAR REGRESSION MODEL
 * =============================================================================
 *
 * Model: y = w₁x₁ + w₂x₂ + w₃x₃ + b
 *
 * Training uses gradient descent:
 *   w_j := w_j - α * (1/n) * Σ(ŷᵢ - yᵢ) * x_ij
 *   b := b - α * (1/n) * Σ(ŷᵢ - yᵢ)
 */

/**
 * Initialises model with small random weights
 */
static void model_init(LinearModel *model, double learning_rate) {
    for (int j = 0; j < NUM_FEATURES; j++) {
        model->weights[j] = random_range(-0.01, 0.01);
    }
    model->bias = 0.0;
    model->learning_rate = learning_rate;
    model->iterations_trained = 0;
    model->final_loss = 0.0;
}

/**
 * Computes prediction for a single sample
 */
static double model_predict(LinearModel *model, double *features) {
    double prediction = model->bias;
    for (int j = 0; j < NUM_FEATURES; j++) {
        prediction += model->weights[j] * features[j];
    }
    return prediction;
}

/**
 * Computes Mean Squared Error loss
 */
static double compute_mse(LinearModel *model, HouseSample *samples, int n) {
    double mse = 0.0;
    for (int i = 0; i < n; i++) {
        double pred = model_predict(model, samples[i].features);
        double error = pred - samples[i].price;
        mse += error * error;
    }
    return mse / n;
}

/**
 * Trains model using batch gradient descent
 *
 * @param model Model to train
 * @param samples Training samples
 * @param n Number of training samples
 * @param loss_history Array to store loss at each iteration
 * @return Number of iterations until convergence
 */
static int model_train(LinearModel *model, HouseSample *samples, int n,
                       double *loss_history) {
    printf("\nTraining linear regression model...\n");
    printf("Learning rate: %.4f, Max iterations: %d\n\n", 
           model->learning_rate, MAX_ITERATIONS);
    
    double prev_loss = compute_mse(model, samples, n);
    
    for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
        /* Compute gradients */
        double grad_weights[NUM_FEATURES] = {0.0};
        double grad_bias = 0.0;
        
        for (int i = 0; i < n; i++) {
            double pred = model_predict(model, samples[i].features);
            double error = pred - samples[i].price;
            
            for (int j = 0; j < NUM_FEATURES; j++) {
                grad_weights[j] += error * samples[i].features[j];
            }
            grad_bias += error;
        }
        
        /* Update parameters */
        for (int j = 0; j < NUM_FEATURES; j++) {
            model->weights[j] -= model->learning_rate * grad_weights[j] / n;
        }
        model->bias -= model->learning_rate * grad_bias / n;
        
        /* Compute new loss */
        double current_loss = compute_mse(model, samples, n);
        loss_history[iter] = current_loss;
        
        /* Print progress periodically */
        if (iter % 100 == 0 || iter == MAX_ITERATIONS - 1) {
            printf("  Iteration %4d: MSE = %.6f\n", iter, current_loss);
        }
        
        /* Check convergence */
        if (fabs(prev_loss - current_loss) < CONVERGENCE_THRESHOLD) {
            printf("\n✓ Converged at iteration %d (loss change < %.1e)\n", 
                   iter, CONVERGENCE_THRESHOLD);
            model->iterations_trained = iter + 1;
            model->final_loss = current_loss;
            return iter + 1;
        }
        
        prev_loss = current_loss;
    }
    
    model->iterations_trained = MAX_ITERATIONS;
    model->final_loss = prev_loss;
    printf("\n⚠ Reached maximum iterations without full convergence\n");
    return MAX_ITERATIONS;
}

/* =============================================================================
 * PART 5: MODEL EVALUATION
 * =============================================================================
 *
 * Metrics:
 *   - MSE: Mean Squared Error
 *   - RMSE: Root Mean Squared Error (same units as target)
 *   - MAE: Mean Absolute Error
 *   - R²: Coefficient of determination (1 = perfect fit)
 */

/**
 * Computes comprehensive evaluation metrics
 */
static EvaluationMetrics evaluate_model(LinearModel *model, HouseSample *samples, 
                                         int n, Normaliser *norm) {
    EvaluationMetrics metrics = {0};
    
    double ss_res = 0.0;  /* Residual sum of squares */
    double ss_tot = 0.0;  /* Total sum of squares */
    double mean_actual = 0.0;
    
    /* Compute mean of actual values (in normalised space) */
    for (int i = 0; i < n; i++) {
        mean_actual += samples[i].price;
    }
    mean_actual /= n;
    
    /* Compute error metrics */
    for (int i = 0; i < n; i++) {
        double pred_norm = model_predict(model, samples[i].features);
        double actual_norm = samples[i].price;
        
        /* Error in normalised space */
        double error_norm = pred_norm - actual_norm;
        
        /* Convert to original scale for interpretable metrics */
        double pred_orig = inverse_transform_price(norm, pred_norm);
        double actual_orig = inverse_transform_price(norm, actual_norm);
        double error_orig = pred_orig - actual_orig;
        
        metrics.mse += error_orig * error_orig;
        metrics.mae += fabs(error_orig);
        
        ss_res += error_norm * error_norm;
        ss_tot += (actual_norm - mean_actual) * (actual_norm - mean_actual);
    }
    
    metrics.mse /= n;
    metrics.rmse = sqrt(metrics.mse);
    metrics.mae /= n;
    metrics.r_squared = 1.0 - (ss_res / ss_tot);
    
    return metrics;
}

/**
 * Prints evaluation metrics in formatted table
 */
static void print_metrics(const char *set_name, EvaluationMetrics *metrics) {
    printf("\n%s Set Evaluation:\n", set_name);
    printf("┌────────────────┬────────────────┐\n");
    printf("│ Metric         │ Value          │\n");
    printf("├────────────────┼────────────────┤\n");
    printf("│ MSE            │ %14.2f │\n", metrics->mse);
    printf("│ RMSE           │ %14.2f │\n", metrics->rmse);
    printf("│ MAE            │ %14.2f │\n", metrics->mae);
    printf("│ R²             │ %14.4f │\n", metrics->r_squared);
    printf("└────────────────┴────────────────┘\n");
}

/* =============================================================================
 * PART 6: LEARNING CURVE VISUALISATION
 * =============================================================================
 *
 * ASCII-based visualisation of training loss over iterations
 */

#define PLOT_WIDTH 60
#define PLOT_HEIGHT 15

/**
 * Draws ASCII learning curve
 */
static void plot_learning_curve(double *loss_history, int n_iterations) {
    printf("\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                    LEARNING CURVE                              ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
    
    /* Find min and max loss for scaling */
    double max_loss = loss_history[0];
    double min_loss = loss_history[n_iterations - 1];
    
    for (int i = 0; i < n_iterations; i++) {
        if (loss_history[i] > max_loss) max_loss = loss_history[i];
        if (loss_history[i] < min_loss) min_loss = loss_history[i];
    }
    
    double range = max_loss - min_loss;
    if (range < 1e-10) range = 1.0;
    
    /* Draw plot */
    char plot[PLOT_HEIGHT][PLOT_WIDTH + 1];
    
    /* Initialise with spaces */
    for (int y = 0; y < PLOT_HEIGHT; y++) {
        for (int x = 0; x < PLOT_WIDTH; x++) {
            plot[y][x] = ' ';
        }
        plot[y][PLOT_WIDTH] = '\0';
    }
    
    /* Plot data points */
    for (int x = 0; x < PLOT_WIDTH && x < n_iterations; x++) {
        int idx = (x * n_iterations) / PLOT_WIDTH;
        if (idx >= n_iterations) idx = n_iterations - 1;
        
        double normalised = (loss_history[idx] - min_loss) / range;
        int y = (int)((1.0 - normalised) * (PLOT_HEIGHT - 1));
        
        if (y < 0) y = 0;
        if (y >= PLOT_HEIGHT) y = PLOT_HEIGHT - 1;
        
        plot[y][x] = '*';
    }
    
    /* Print plot with axes */
    printf("Loss\n");
    printf("%.4f ┤", max_loss);
    for (int x = 0; x < PLOT_WIDTH; x++) printf("%c", plot[0][x]);
    printf("\n");
    
    for (int y = 1; y < PLOT_HEIGHT - 1; y++) {
        printf("        │");
        for (int x = 0; x < PLOT_WIDTH; x++) printf("%c", plot[y][x]);
        printf("\n");
    }
    
    printf("%.4f ┤", min_loss);
    for (int x = 0; x < PLOT_WIDTH; x++) printf("%c", plot[PLOT_HEIGHT - 1][x]);
    printf("\n");
    
    printf("        └");
    for (int x = 0; x < PLOT_WIDTH; x++) printf("─");
    printf("\n");
    printf("         0                    Iterations                    %d\n", n_iterations);
}

/* =============================================================================
 * PART 7: LEARNED COEFFICIENTS ANALYSIS
 * =============================================================================
 */

/**
 * Prints learned model coefficients with interpretation
 */
static void print_learned_coefficients(LinearModel *model, Normaliser *norm) {
    printf("\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                    LEARNED COEFFICIENTS                        ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
    
    /* Denormalise weights for interpretation */
    double true_weights[NUM_FEATURES];
    const double true_coefs[] = {50.0, 10000.0, -1000.0};  /* Ground truth */
    
    for (int j = 0; j < NUM_FEATURES; j++) {
        true_weights[j] = model->weights[j] * norm->price_std / norm->std_dev[j];
    }
    double true_bias = model->bias * norm->price_std + norm->price_mean;
    for (int j = 0; j < NUM_FEATURES; j++) {
        true_bias -= true_weights[j] * norm->mean[j];
    }
    
    printf("Model (normalised): price = %.4f*sqft + %.4f*bedrooms + %.4f*age + %.4f\n\n",
           model->weights[0], model->weights[1], model->weights[2], model->bias);
    
    printf("Model (original scale):\n");
    printf("  price = %.2f*sqft + %.2f*bedrooms + %.2f*age + %.2f\n\n",
           true_weights[0], true_weights[1], true_weights[2], true_bias);
    
    printf("True model (for comparison):\n");
    printf("  price = 50.00*sqft + 10000.00*bedrooms - 1000.00*age + 100000.00\n\n");
    
    printf("Coefficient comparison:\n");
    printf("┌──────────────┬────────────────┬────────────────┬────────────────┐\n");
    printf("│ Feature      │ Learned        │ True           │ Error %%        │\n");
    printf("├──────────────┼────────────────┼────────────────┼────────────────┤\n");
    
    const char *feature_names[] = {"sqft", "bedrooms", "age"};
    for (int j = 0; j < NUM_FEATURES; j++) {
        double error_pct = 100.0 * fabs(true_weights[j] - true_coefs[j]) / fabs(true_coefs[j]);
        printf("│ %-12s │ %14.2f │ %14.2f │ %13.2f%% │\n",
               feature_names[j], true_weights[j], true_coefs[j], error_pct);
    }
    printf("│ bias         │ %14.2f │ %14.2f │ %13.2f%% │\n",
           true_bias, 100000.0, 100.0 * fabs(true_bias - 100000.0) / 100000.0);
    printf("└──────────────┴────────────────┴────────────────┴────────────────┘\n");
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
    printf("║     Exercise 1 Solution: House Price Prediction               ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Seed random number generator */
    srand(42);  /* Fixed seed for reproducibility */
    
    /* Allocate memory for samples */
    HouseSample *all_samples = malloc(MAX_SAMPLES * sizeof(HouseSample));
    HouseSample *train_samples = malloc(MAX_SAMPLES * sizeof(HouseSample));
    HouseSample *test_samples = malloc(MAX_SAMPLES * sizeof(HouseSample));
    double *loss_history = malloc(MAX_ITERATIONS * sizeof(double));
    
    if (!all_samples || !train_samples || !test_samples || !loss_history) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: DATA LOADING                                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int n_samples;
    
    /* Try to load from CSV, fall back to synthetic data */
    n_samples = load_housing_csv("../data/housing.csv", all_samples, MAX_SAMPLES);
    if (n_samples < 0) {
        n_samples = 200;  /* Generate 200 synthetic samples */
        generate_housing_data(all_samples, n_samples);
    }
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: TRAIN/TEST SPLIT                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int n_train, n_test;
    train_test_split(all_samples, n_samples, train_samples, test_samples,
                     TRAIN_RATIO, &n_train, &n_test);
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: NORMALISATION                                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Normaliser norm = {0};
    
    /* IMPORTANT: Fit only on training data */
    normaliser_fit(&norm, train_samples, n_train);
    
    /* Transform both sets using training statistics */
    normaliser_transform(&norm, train_samples, n_train);
    normaliser_transform(&norm, test_samples, n_test);
    
    printf("\n✓ Training and test data normalised\n");
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: MODEL TRAINING                                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    LinearModel model;
    model_init(&model, LEARNING_RATE);
    
    int iterations = model_train(&model, train_samples, n_train, loss_history);
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: MODEL EVALUATION                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    EvaluationMetrics train_metrics = evaluate_model(&model, train_samples, n_train, &norm);
    EvaluationMetrics test_metrics = evaluate_model(&model, test_samples, n_test, &norm);
    
    print_metrics("Training", &train_metrics);
    print_metrics("Test", &test_metrics);
    
    /* Check for overfitting */
    printf("\nOverfitting analysis:\n");
    double r2_diff = train_metrics.r_squared - test_metrics.r_squared;
    if (r2_diff > 0.1) {
        printf("⚠ Warning: Significant gap between train and test R² (%.4f)\n", r2_diff);
        printf("  This may indicate overfitting.\n");
    } else {
        printf("✓ Model generalises well (train-test R² gap: %.4f)\n", r2_diff);
    }
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: LEARNING CURVE                                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    plot_learning_curve(loss_history, iterations);
    
    /* ─────────────────────────────────────────────────────────────────────── */
    print_learned_coefficients(&model, &norm);
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      SAMPLE PREDICTIONS                                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("First 5 test samples:\n");
    printf("┌───────────┬───────────┬───────────┬───────────────┬───────────────┬───────────┐\n");
    printf("│ Sqft      │ Bedrooms  │ Age       │ Actual Price  │ Predicted     │ Error     │\n");
    printf("├───────────┼───────────┼───────────┼───────────────┼───────────────┼───────────┤\n");
    
    for (int i = 0; i < 5 && i < n_test; i++) {
        double pred_norm = model_predict(&model, test_samples[i].features);
        double pred = inverse_transform_price(&norm, pred_norm);
        double actual = inverse_transform_price(&norm, test_samples[i].price);
        double error = pred - actual;
        
        /* Denormalise features for display */
        double sqft = test_samples[i].features[0] * norm.std_dev[0] + norm.mean[0];
        double beds = test_samples[i].features[1] * norm.std_dev[1] + norm.mean[1];
        double age = test_samples[i].features[2] * norm.std_dev[2] + norm.mean[2];
        
        printf("│ %9.0f │ %9.0f │ %9.1f │ %13.0f │ %13.0f │ %+9.0f │\n",
               sqft, beds, age, actual, pred, error);
    }
    printf("└───────────┴───────────┴───────────┴───────────────┴───────────────┴───────────┘\n");
    
    /* Clean up */
    free(all_samples);
    free(train_samples);
    free(test_samples);
    free(loss_history);
    
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    EXERCISE 1 COMPLETE                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

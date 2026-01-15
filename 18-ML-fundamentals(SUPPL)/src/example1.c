/**
 * =============================================================================
 * WEEK 18: MACHINE LEARNING FUNDAMENTALS IN C
 * Complete Working Example
 * =============================================================================
 *
 * This example demonstrates:
 *   1. Matrix operations for linear algebra foundations
 *   2. Gradient descent optimisation
 *   3. Linear regression (simple and multiple)
 *   4. Logistic regression for binary classification
 *   5. K-Nearest Neighbours (K-NN) algorithm
 *   6. K-Means clustering
 *   7. Decision tree basics
 *   8. Perceptron and simple neural network
 *   9. Model evaluation metrics
 *  10. Data normalisation techniques
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

#define MAX_FEATURES 10
#define MAX_SAMPLES 1000
#define MAX_ITERATIONS 10000
#define LEARNING_RATE 0.01
#define CONVERGENCE_THRESHOLD 1e-6
#define K_NEIGHBOURS 5
#define K_CLUSTERS 3

/* =============================================================================
 * PART 1: MATRIX OPERATIONS FOR LINEAR ALGEBRA
 * =============================================================================
 */

/**
 * Matrix structure for linear algebra operations.
 * Uses row-major order storage.
 *
 * Memory Layout:
 *   For a 3x3 matrix:
 *   data[0] data[1] data[2]   <- Row 0
 *   data[3] data[4] data[5]   <- Row 1
 *   data[6] data[7] data[8]   <- Row 2
 */
typedef struct {
    double *data;
    size_t rows;
    size_t cols;
} Matrix;

/**
 * Creates a new matrix with given dimensions.
 * All elements are initialised to zero.
 *
 * @param rows Number of rows
 * @param cols Number of columns
 * @return Pointer to newly allocated matrix, NULL on failure
 *
 * Time Complexity: O(rows × cols)
 * Space Complexity: O(rows × cols)
 */
Matrix *matrix_create(size_t rows, size_t cols) {
    Matrix *m = (Matrix *)malloc(sizeof(Matrix));
    if (m == NULL) {
        return NULL;
    }
    
    m->rows = rows;
    m->cols = cols;
    m->data = (double *)calloc(rows * cols, sizeof(double));
    
    if (m->data == NULL) {
        free(m);
        return NULL;
    }
    
    return m;
}

/**
 * Frees all memory associated with a matrix.
 *
 * @param m Matrix to free
 */
void matrix_free(Matrix *m) {
    if (m != NULL) {
        free(m->data);
        free(m);
    }
}

/**
 * Gets element at position (row, col).
 * Uses zero-based indexing.
 *
 * @param m Matrix
 * @param row Row index
 * @param col Column index
 * @return Value at specified position
 */
double matrix_get(const Matrix *m, size_t row, size_t col) {
    return m->data[row * m->cols + col];
}

/**
 * Sets element at position (row, col).
 *
 * @param m Matrix
 * @param row Row index
 * @param col Column index
 * @param value Value to set
 */
void matrix_set(Matrix *m, size_t row, size_t col, double value) {
    m->data[row * m->cols + col] = value;
}

/**
 * Multiplies two matrices: C = A × B
 *
 * @param A First matrix (m × n)
 * @param B Second matrix (n × p)
 * @return Result matrix (m × p), NULL on dimension mismatch
 *
 * Time Complexity: O(m × n × p)
 * Space Complexity: O(m × p)
 */
Matrix *matrix_multiply(const Matrix *A, const Matrix *B) {
    if (A->cols != B->rows) {
        fprintf(stderr, "Matrix multiplication error: incompatible dimensions\n");
        return NULL;
    }
    
    Matrix *C = matrix_create(A->rows, B->cols);
    if (C == NULL) {
        return NULL;
    }
    
    for (size_t i = 0; i < A->rows; i++) {
        for (size_t j = 0; j < B->cols; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < A->cols; k++) {
                sum += matrix_get(A, i, k) * matrix_get(B, k, j);
            }
            matrix_set(C, i, j, sum);
        }
    }
    
    return C;
}

/**
 * Transposes a matrix: B = A^T
 *
 * @param A Input matrix (m × n)
 * @return Transposed matrix (n × m)
 *
 * Time Complexity: O(m × n)
 * Space Complexity: O(m × n)
 */
Matrix *matrix_transpose(const Matrix *A) {
    Matrix *B = matrix_create(A->cols, A->rows);
    if (B == NULL) {
        return NULL;
    }
    
    for (size_t i = 0; i < A->rows; i++) {
        for (size_t j = 0; j < A->cols; j++) {
            matrix_set(B, j, i, matrix_get(A, i, j));
        }
    }
    
    return B;
}

/**
 * Adds two matrices element-wise: C = A + B
 *
 * @param A First matrix
 * @param B Second matrix (must have same dimensions as A)
 * @return Result matrix, NULL on dimension mismatch
 */
Matrix *matrix_add(const Matrix *A, const Matrix *B) {
    if (A->rows != B->rows || A->cols != B->cols) {
        fprintf(stderr, "Matrix addition error: incompatible dimensions\n");
        return NULL;
    }
    
    Matrix *C = matrix_create(A->rows, A->cols);
    if (C == NULL) {
        return NULL;
    }
    
    for (size_t i = 0; i < A->rows * A->cols; i++) {
        C->data[i] = A->data[i] + B->data[i];
    }
    
    return C;
}

/**
 * Scales a matrix by a scalar: B = α × A
 *
 * @param A Input matrix
 * @param scalar Scaling factor
 * @return Scaled matrix
 */
Matrix *matrix_scale(const Matrix *A, double scalar) {
    Matrix *B = matrix_create(A->rows, A->cols);
    if (B == NULL) {
        return NULL;
    }
    
    for (size_t i = 0; i < A->rows * A->cols; i++) {
        B->data[i] = scalar * A->data[i];
    }
    
    return B;
}

/**
 * Prints a matrix to stdout with formatting.
 *
 * @param m Matrix to print
 * @param name Name to display
 */
void matrix_print(const Matrix *m, const char *name) {
    printf("%s (%zux%zu):\n", name, m->rows, m->cols);
    for (size_t i = 0; i < m->rows; i++) {
        printf("  [");
        for (size_t j = 0; j < m->cols; j++) {
            printf(" %8.4f", matrix_get(m, i, j));
        }
        printf(" ]\n");
    }
}

/**
 * Demonstrates Part 1: Matrix Operations
 */
void demo_part1(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: Matrix Operations for Linear Algebra             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Creating matrices for demonstration...\n\n");
    
    /* Create a 2×3 matrix */
    Matrix *A = matrix_create(2, 3);
    double vals_A[] = {1, 2, 3, 4, 5, 6};
    for (int i = 0; i < 6; i++) {
        A->data[i] = vals_A[i];
    }
    matrix_print(A, "Matrix A");
    
    /* Create a 3×2 matrix */
    Matrix *B = matrix_create(3, 2);
    double vals_B[] = {7, 8, 9, 10, 11, 12};
    for (int i = 0; i < 6; i++) {
        B->data[i] = vals_B[i];
    }
    printf("\n");
    matrix_print(B, "Matrix B");
    
    /* Matrix multiplication */
    printf("\nMatrix Multiplication C = A × B:\n");
    Matrix *C = matrix_multiply(A, B);
    if (C != NULL) {
        matrix_print(C, "Matrix C");
        matrix_free(C);
    }
    
    /* Transpose */
    printf("\nMatrix Transpose A^T:\n");
    Matrix *A_T = matrix_transpose(A);
    if (A_T != NULL) {
        matrix_print(A_T, "Matrix A^T");
        matrix_free(A_T);
    }
    
    matrix_free(A);
    matrix_free(B);
    
    printf("\n✓ Matrix operations demonstrated successfully\n");
}

/* =============================================================================
 * PART 2: GRADIENT DESCENT OPTIMISATION
 * =============================================================================
 */

/**
 * Structure to hold gradient descent training history.
 */
typedef struct {
    double *losses;
    size_t num_iterations;
} TrainingHistory;

/**
 * Computes the mean squared error (MSE) loss.
 *
 * MSE = (1/n) × Σ(y_pred - y_true)²
 *
 * @param y_true Array of true values
 * @param y_pred Array of predicted values
 * @param n Number of samples
 * @return MSE loss value
 */
double compute_mse(const double *y_true, const double *y_pred, size_t n) {
    double mse = 0.0;
    for (size_t i = 0; i < n; i++) {
        double diff = y_pred[i] - y_true[i];
        mse += diff * diff;
    }
    return mse / (double)n;
}

/**
 * Computes the R² (coefficient of determination) score.
 *
 * R² = 1 - (SS_res / SS_tot)
 * where SS_res = Σ(y_true - y_pred)² and SS_tot = Σ(y_true - y_mean)²
 *
 * @param y_true Array of true values
 * @param y_pred Array of predicted values
 * @param n Number of samples
 * @return R² score (1.0 is perfect)
 */
double compute_r2(const double *y_true, const double *y_pred, size_t n) {
    /* Compute mean of y_true */
    double mean = 0.0;
    for (size_t i = 0; i < n; i++) {
        mean += y_true[i];
    }
    mean /= (double)n;
    
    /* Compute SS_res and SS_tot */
    double ss_res = 0.0, ss_tot = 0.0;
    for (size_t i = 0; i < n; i++) {
        ss_res += (y_true[i] - y_pred[i]) * (y_true[i] - y_pred[i]);
        ss_tot += (y_true[i] - mean) * (y_true[i] - mean);
    }
    
    return 1.0 - (ss_res / ss_tot);
}

/**
 * Demonstrates Part 2: Gradient Descent Concept
 */
void demo_part2(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: Gradient Descent Optimisation                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Gradient Descent Algorithm:\n");
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│  while not converged:                                           │\n");
    printf("│      gradient = ∂Loss/∂parameters                               │\n");
    printf("│      parameters = parameters - learning_rate × gradient         │\n");
    printf("└─────────────────────────────────────────────────────────────────┘\n\n");
    
    printf("Finding minimum of f(x) = x² - 4x + 4 = (x-2)²\n");
    printf("The minimum is at x = 2 where f(x) = 0\n\n");
    
    double x = 10.0;  /* Starting point */
    double learning_rate = 0.1;
    
    printf("Starting at x = %.2f\n", x);
    printf("Learning rate = %.2f\n\n", learning_rate);
    
    printf("Iteration    x        f(x)      gradient\n");
    printf("─────────────────────────────────────────\n");
    
    for (int iter = 0; iter < 20; iter++) {
        double fx = (x - 2) * (x - 2);
        double gradient = 2 * (x - 2);  /* df/dx = 2x - 4 */
        
        if (iter < 10 || iter % 5 == 0) {
            printf("  %3d      %7.4f   %7.4f   %7.4f\n", iter, x, fx, gradient);
        }
        
        x = x - learning_rate * gradient;
        
        if (fabs(gradient) < CONVERGENCE_THRESHOLD) {
            printf("\n✓ Converged at iteration %d\n", iter);
            break;
        }
    }
    
    printf("\nFinal x = %.6f (expected: 2.0)\n", x);
    printf("\n✓ Gradient descent concept demonstrated\n");
}

/* =============================================================================
 * PART 3: LINEAR REGRESSION
 * =============================================================================
 */

/**
 * Linear Regression Model
 * y = w × x + b (simple linear regression)
 * y = Σ(w_i × x_i) + b (multiple linear regression)
 */
typedef struct {
    double *weights;    /* Weight for each feature */
    double bias;        /* Bias term (intercept) */
    size_t num_features;
} LinearRegression;

/**
 * Creates a new linear regression model.
 *
 * @param num_features Number of input features
 * @return Pointer to model, NULL on failure
 */
LinearRegression *linear_regression_create(size_t num_features) {
    LinearRegression *model = (LinearRegression *)malloc(sizeof(LinearRegression));
    if (model == NULL) {
        return NULL;
    }
    
    model->num_features = num_features;
    model->weights = (double *)calloc(num_features, sizeof(double));
    model->bias = 0.0;
    
    if (model->weights == NULL) {
        free(model);
        return NULL;
    }
    
    return model;
}

/**
 * Frees a linear regression model.
 *
 * @param model Model to free
 */
void linear_regression_free(LinearRegression *model) {
    if (model != NULL) {
        free(model->weights);
        free(model);
    }
}

/**
 * Predicts output for a single sample.
 *
 * @param model Linear regression model
 * @param x Feature values for one sample
 * @return Predicted value
 */
double linear_regression_predict_single(const LinearRegression *model, const double *x) {
    double prediction = model->bias;
    for (size_t i = 0; i < model->num_features; i++) {
        prediction += model->weights[i] * x[i];
    }
    return prediction;
}

/**
 * Trains the linear regression model using gradient descent.
 *
 * Gradient update rules:
 *   w = w - α × (2/n) × Σ x_i × (y_pred_i - y_true_i)
 *   b = b - α × (2/n) × Σ (y_pred_i - y_true_i)
 *
 * @param model Model to train
 * @param X Feature matrix (n_samples × n_features)
 * @param y Target values
 * @param n_samples Number of training samples
 * @param learning_rate Learning rate α
 * @param max_iterations Maximum iterations
 * @param verbose Print progress if true
 */
void linear_regression_fit(LinearRegression *model,
                           const double **X,
                           const double *y,
                           size_t n_samples,
                           double learning_rate,
                           size_t max_iterations,
                           bool verbose) {
    double *predictions = (double *)malloc(n_samples * sizeof(double));
    double *grad_w = (double *)calloc(model->num_features, sizeof(double));
    
    if (predictions == NULL || grad_w == NULL) {
        free(predictions);
        free(grad_w);
        return;
    }
    
    if (verbose) {
        printf("Training linear regression...\n");
        printf("Iter       MSE         R²\n");
        printf("────────────────────────────\n");
    }
    
    for (size_t iter = 0; iter < max_iterations; iter++) {
        /* Forward pass: compute predictions */
        for (size_t i = 0; i < n_samples; i++) {
            predictions[i] = linear_regression_predict_single(model, X[i]);
        }
        
        /* Compute loss (for monitoring) */
        double mse = compute_mse(y, predictions, n_samples);
        double r2 = compute_r2(y, predictions, n_samples);
        
        if (verbose && (iter < 5 || iter % 100 == 0)) {
            printf("%4zu    %9.6f   %7.4f\n", iter, mse, r2);
        }
        
        /* Compute gradients */
        memset(grad_w, 0, model->num_features * sizeof(double));
        double grad_b = 0.0;
        
        for (size_t i = 0; i < n_samples; i++) {
            double error = predictions[i] - y[i];
            for (size_t j = 0; j < model->num_features; j++) {
                grad_w[j] += X[i][j] * error;
            }
            grad_b += error;
        }
        
        /* Update parameters */
        for (size_t j = 0; j < model->num_features; j++) {
            model->weights[j] -= learning_rate * (2.0 / n_samples) * grad_w[j];
        }
        model->bias -= learning_rate * (2.0 / n_samples) * grad_b;
        
        /* Check convergence */
        if (mse < CONVERGENCE_THRESHOLD) {
            if (verbose) {
                printf("Converged at iteration %zu\n", iter);
            }
            break;
        }
    }
    
    free(predictions);
    free(grad_w);
}

/**
 * Demonstrates Part 3: Linear Regression
 */
void demo_part3(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: Linear Regression                                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Linear Regression Model: y = w₀x₀ + w₁x₁ + ... + b\n\n");
    
    /* Generate synthetic data: y = 3x + 2 + noise */
    size_t n_samples = 20;
    double X_data[20];
    double y_data[20];
    const double *X[20];
    
    srand(42);
    printf("Generating training data: y = 3x + 2 + noise\n");
    printf("  x        y_true\n");
    printf("─────────────────\n");
    
    for (size_t i = 0; i < n_samples; i++) {
        X_data[i] = (double)i / 2.0;
        double noise = ((double)rand() / RAND_MAX - 0.5) * 2.0;
        y_data[i] = 3.0 * X_data[i] + 2.0 + noise;
        X[i] = &X_data[i];
        
        if (i < 5) {
            printf("  %5.2f    %6.3f\n", X_data[i], y_data[i]);
        }
    }
    printf("  ... (%zu more samples)\n\n", n_samples - 5);
    
    /* Train model */
    LinearRegression *model = linear_regression_create(1);
    if (model == NULL) {
        printf("Error: Failed to create model\n");
        return;
    }
    
    linear_regression_fit(model, X, y_data, n_samples, 0.01, 500, true);
    
    printf("\nLearned parameters:\n");
    printf("  Weight (w) = %.4f (expected: 3.0)\n", model->weights[0]);
    printf("  Bias (b)   = %.4f (expected: 2.0)\n", model->bias);
    
    /* Test predictions */
    printf("\nTest predictions:\n");
    printf("  x = 5.0 -> y_pred = %.4f (expected: ~17.0)\n",
           linear_regression_predict_single(model, &(double){5.0}));
    printf("  x = 10.0 -> y_pred = %.4f (expected: ~32.0)\n",
           linear_regression_predict_single(model, &(double){10.0}));
    
    linear_regression_free(model);
    printf("\n✓ Linear regression demonstrated successfully\n");
}

/* =============================================================================
 * PART 4: LOGISTIC REGRESSION
 * =============================================================================
 */

/**
 * Sigmoid activation function.
 * σ(z) = 1 / (1 + e^(-z))
 *
 * @param z Input value
 * @return Output in range (0, 1)
 */
double sigmoid(double z) {
    /* Clip input to prevent overflow */
    if (z > 500.0) z = 500.0;
    if (z < -500.0) z = -500.0;
    return 1.0 / (1.0 + exp(-z));
}

/**
 * Logistic Regression Model for binary classification.
 * P(y=1|x) = σ(w·x + b)
 */
typedef struct {
    double *weights;
    double bias;
    size_t num_features;
} LogisticRegression;

/**
 * Creates a logistic regression model.
 */
LogisticRegression *logistic_regression_create(size_t num_features) {
    LogisticRegression *model = (LogisticRegression *)malloc(sizeof(LogisticRegression));
    if (model == NULL) {
        return NULL;
    }
    
    model->num_features = num_features;
    model->weights = (double *)calloc(num_features, sizeof(double));
    model->bias = 0.0;
    
    if (model->weights == NULL) {
        free(model);
        return NULL;
    }
    
    return model;
}

/**
 * Frees logistic regression model.
 */
void logistic_regression_free(LogisticRegression *model) {
    if (model != NULL) {
        free(model->weights);
        free(model);
    }
}

/**
 * Predicts probability for a single sample.
 */
double logistic_regression_predict_proba(const LogisticRegression *model, const double *x) {
    double z = model->bias;
    for (size_t i = 0; i < model->num_features; i++) {
        z += model->weights[i] * x[i];
    }
    return sigmoid(z);
}

/**
 * Predicts class label (0 or 1) for a single sample.
 */
int logistic_regression_predict(const LogisticRegression *model, const double *x) {
    return logistic_regression_predict_proba(model, x) >= 0.5 ? 1 : 0;
}

/**
 * Computes binary cross-entropy loss.
 * L = -(1/n) × Σ[y×log(p) + (1-y)×log(1-p)]
 */
double compute_cross_entropy(const int *y_true, const double *y_pred, size_t n) {
    double loss = 0.0;
    for (size_t i = 0; i < n; i++) {
        double p = y_pred[i];
        /* Clip to prevent log(0) */
        if (p < 1e-15) p = 1e-15;
        if (p > 1.0 - 1e-15) p = 1.0 - 1e-15;
        
        loss -= y_true[i] * log(p) + (1 - y_true[i]) * log(1 - p);
    }
    return loss / n;
}

/**
 * Trains logistic regression using gradient descent.
 */
void logistic_regression_fit(LogisticRegression *model,
                              const double **X,
                              const int *y,
                              size_t n_samples,
                              double learning_rate,
                              size_t max_iterations,
                              bool verbose) {
    double *predictions = (double *)malloc(n_samples * sizeof(double));
    double *grad_w = (double *)calloc(model->num_features, sizeof(double));
    
    if (verbose) {
        printf("Training logistic regression...\n");
        printf("Iter    Cross-Entropy    Accuracy\n");
        printf("─────────────────────────────────\n");
    }
    
    for (size_t iter = 0; iter < max_iterations; iter++) {
        /* Forward pass */
        for (size_t i = 0; i < n_samples; i++) {
            predictions[i] = logistic_regression_predict_proba(model, X[i]);
        }
        
        /* Compute loss and accuracy */
        double loss = compute_cross_entropy(y, predictions, n_samples);
        int correct = 0;
        for (size_t i = 0; i < n_samples; i++) {
            if ((predictions[i] >= 0.5) == y[i]) {
                correct++;
            }
        }
        double accuracy = (double)correct / n_samples;
        
        if (verbose && (iter < 5 || iter % 100 == 0)) {
            printf("%4zu       %9.6f       %5.1f%%\n", iter, loss, accuracy * 100);
        }
        
        /* Compute gradients */
        memset(grad_w, 0, model->num_features * sizeof(double));
        double grad_b = 0.0;
        
        for (size_t i = 0; i < n_samples; i++) {
            double error = predictions[i] - y[i];
            for (size_t j = 0; j < model->num_features; j++) {
                grad_w[j] += X[i][j] * error;
            }
            grad_b += error;
        }
        
        /* Update parameters */
        for (size_t j = 0; j < model->num_features; j++) {
            model->weights[j] -= learning_rate * grad_w[j] / n_samples;
        }
        model->bias -= learning_rate * grad_b / n_samples;
    }
    
    free(predictions);
    free(grad_w);
}

/**
 * Demonstrates Part 4: Logistic Regression
 */
void demo_part4(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: Logistic Regression for Binary Classification    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Sigmoid Function: σ(z) = 1 / (1 + e^(-z))\n");
    printf("\n");
    printf("     z      σ(z)\n");
    printf("────────────────\n");
    for (double z = -4; z <= 4; z += 1.0) {
        printf("  %5.1f    %5.3f\n", z, sigmoid(z));
    }
    
    /* Generate 2D classification data */
    printf("\nGenerating 2D classification data...\n");
    size_t n_samples = 40;
    double X_data[40][2];
    int y_data[40];
    const double *X[40];
    
    srand(42);
    
    /* Class 0: centred around (1, 1) */
    /* Class 1: centred around (4, 4) */
    for (size_t i = 0; i < n_samples / 2; i++) {
        X_data[i][0] = 1.0 + ((double)rand() / RAND_MAX - 0.5) * 2.0;
        X_data[i][1] = 1.0 + ((double)rand() / RAND_MAX - 0.5) * 2.0;
        y_data[i] = 0;
        X[i] = X_data[i];
    }
    for (size_t i = n_samples / 2; i < n_samples; i++) {
        X_data[i][0] = 4.0 + ((double)rand() / RAND_MAX - 0.5) * 2.0;
        X_data[i][1] = 4.0 + ((double)rand() / RAND_MAX - 0.5) * 2.0;
        y_data[i] = 1;
        X[i] = X_data[i];
    }
    
    printf("Class 0 samples: centred around (1, 1)\n");
    printf("Class 1 samples: centred around (4, 4)\n\n");
    
    /* Train model */
    LogisticRegression *model = logistic_regression_create(2);
    logistic_regression_fit(model, X, y_data, n_samples, 0.1, 500, true);
    
    printf("\nLearned parameters:\n");
    printf("  w0 = %.4f, w1 = %.4f, b = %.4f\n",
           model->weights[0], model->weights[1], model->bias);
    
    /* Test predictions */
    printf("\nTest predictions:\n");
    double test1[] = {1.0, 1.0};
    double test2[] = {4.0, 4.0};
    double test3[] = {2.5, 2.5};
    printf("  (1, 1) -> P(class=1) = %.3f, predicted class: %d\n",
           logistic_regression_predict_proba(model, test1),
           logistic_regression_predict(model, test1));
    printf("  (4, 4) -> P(class=1) = %.3f, predicted class: %d\n",
           logistic_regression_predict_proba(model, test2),
           logistic_regression_predict(model, test2));
    printf("  (2.5, 2.5) -> P(class=1) = %.3f, predicted class: %d\n",
           logistic_regression_predict_proba(model, test3),
           logistic_regression_predict(model, test3));
    
    logistic_regression_free(model);
    printf("\n✓ Logistic regression demonstrated successfully\n");
}

/* =============================================================================
 * PART 5: K-NEAREST NEIGHBOURS (K-NN)
 * =============================================================================
 */

/**
 * Computes Euclidean distance between two points.
 * d = √(Σ(x_i - y_i)²)
 *
 * @param a First point
 * @param b Second point
 * @param dim Dimensionality
 * @return Euclidean distance
 */
double euclidean_distance(const double *a, const double *b, size_t dim) {
    double sum = 0.0;
    for (size_t i = 0; i < dim; i++) {
        double diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

/**
 * Computes Manhattan (L1) distance between two points.
 * d = Σ|x_i - y_i|
 */
double manhattan_distance(const double *a, const double *b, size_t dim) {
    double sum = 0.0;
    for (size_t i = 0; i < dim; i++) {
        sum += fabs(a[i] - b[i]);
    }
    return sum;
}

/**
 * K-NN classifier structure.
 */
typedef struct {
    double **X_train;
    int *y_train;
    size_t n_samples;
    size_t n_features;
    int k;
} KNN;

/**
 * Creates a K-NN classifier.
 */
KNN *knn_create(int k) {
    KNN *knn = (KNN *)malloc(sizeof(KNN));
    if (knn == NULL) {
        return NULL;
    }
    
    knn->k = k;
    knn->X_train = NULL;
    knn->y_train = NULL;
    knn->n_samples = 0;
    knn->n_features = 0;
    
    return knn;
}

/**
 * Fits K-NN by storing training data.
 * Note: K-NN is a lazy learner - no actual training happens.
 */
void knn_fit(KNN *knn, double **X, int *y, size_t n_samples, size_t n_features) {
    knn->n_samples = n_samples;
    knn->n_features = n_features;
    
    /* Allocate and copy training data */
    knn->X_train = (double **)malloc(n_samples * sizeof(double *));
    knn->y_train = (int *)malloc(n_samples * sizeof(int));
    
    for (size_t i = 0; i < n_samples; i++) {
        knn->X_train[i] = (double *)malloc(n_features * sizeof(double));
        memcpy(knn->X_train[i], X[i], n_features * sizeof(double));
        knn->y_train[i] = y[i];
    }
}

/**
 * Predicts class label for a single sample using K-NN.
 */
int knn_predict(const KNN *knn, const double *x) {
    /* Compute distances to all training samples */
    double *distances = (double *)malloc(knn->n_samples * sizeof(double));
    int *indices = (int *)malloc(knn->n_samples * sizeof(int));
    
    for (size_t i = 0; i < knn->n_samples; i++) {
        distances[i] = euclidean_distance(x, knn->X_train[i], knn->n_features);
        indices[i] = (int)i;
    }
    
    /* Find k nearest neighbours (simple selection) */
    for (int i = 0; i < knn->k; i++) {
        for (size_t j = i + 1; j < knn->n_samples; j++) {
            if (distances[j] < distances[i]) {
                double temp_d = distances[i];
                distances[i] = distances[j];
                distances[j] = temp_d;
                
                int temp_i = indices[i];
                indices[i] = indices[j];
                indices[j] = temp_i;
            }
        }
    }
    
    /* Vote among k nearest */
    int votes[10] = {0};  /* Assume max 10 classes */
    int max_class = 0;
    
    for (int i = 0; i < knn->k; i++) {
        int label = knn->y_train[indices[i]];
        votes[label]++;
        if (label > max_class) {
            max_class = label;
        }
    }
    
    /* Find majority class */
    int predicted = 0;
    int max_votes = 0;
    for (int i = 0; i <= max_class; i++) {
        if (votes[i] > max_votes) {
            max_votes = votes[i];
            predicted = i;
        }
    }
    
    free(distances);
    free(indices);
    
    return predicted;
}

/**
 * Frees K-NN classifier.
 */
void knn_free(KNN *knn) {
    if (knn != NULL) {
        if (knn->X_train != NULL) {
            for (size_t i = 0; i < knn->n_samples; i++) {
                free(knn->X_train[i]);
            }
            free(knn->X_train);
        }
        free(knn->y_train);
        free(knn);
    }
}

/**
 * Demonstrates Part 5: K-Nearest Neighbours
 */
void demo_part5(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: K-Nearest Neighbours (K-NN)                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("K-NN Algorithm:\n");
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│  1. Compute distance from query point to all training points    │\n");
    printf("│  2. Select K nearest neighbours                                 │\n");
    printf("│  3. Return majority class among neighbours                      │\n");
    printf("└─────────────────────────────────────────────────────────────────┘\n\n");
    
    /* Create Iris-like dataset (simplified) */
    printf("Creating simplified Iris-like dataset...\n");
    size_t n_samples = 30;
    double X_data[30][2];
    int y_data[30];
    double *X[30];
    
    srand(42);
    
    /* Class 0: Setosa-like */
    for (int i = 0; i < 10; i++) {
        X_data[i][0] = 1.0 + ((double)rand() / RAND_MAX) * 0.5;
        X_data[i][1] = 0.2 + ((double)rand() / RAND_MAX) * 0.3;
        y_data[i] = 0;
        X[i] = X_data[i];
    }
    /* Class 1: Versicolor-like */
    for (int i = 10; i < 20; i++) {
        X_data[i][0] = 2.5 + ((double)rand() / RAND_MAX) * 1.0;
        X_data[i][1] = 1.0 + ((double)rand() / RAND_MAX) * 0.5;
        y_data[i] = 1;
        X[i] = X_data[i];
    }
    /* Class 2: Virginica-like */
    for (int i = 20; i < 30; i++) {
        X_data[i][0] = 4.5 + ((double)rand() / RAND_MAX) * 1.0;
        X_data[i][1] = 1.8 + ((double)rand() / RAND_MAX) * 0.7;
        y_data[i] = 2;
        X[i] = X_data[i];
    }
    
    printf("Classes: 0=Setosa, 1=Versicolor, 2=Virginica\n");
    printf("Features: Petal length and width (simulated)\n\n");
    
    /* Create and fit K-NN */
    KNN *knn = knn_create(5);
    knn_fit(knn, X, y_data, n_samples, 2);
    
    printf("K-NN fitted with K=%d\n\n", knn->k);
    
    /* Test predictions */
    printf("Test predictions:\n");
    double test_samples[5][2] = {
        {1.2, 0.3},   /* Should be class 0 */
        {2.8, 1.2},   /* Should be class 1 */
        {5.0, 2.0},   /* Should be class 2 */
        {1.8, 0.6},   /* Border case */
        {3.5, 1.5}    /* Border case */
    };
    
    const char *class_names[] = {"Setosa", "Versicolor", "Virginica"};
    
    for (int i = 0; i < 5; i++) {
        int pred = knn_predict(knn, test_samples[i]);
        printf("  (%.1f, %.1f) -> Predicted: %d (%s)\n",
               test_samples[i][0], test_samples[i][1],
               pred, class_names[pred]);
    }
    
    knn_free(knn);
    printf("\n✓ K-NN demonstrated successfully\n");
}

/* =============================================================================
 * PART 6: K-MEANS CLUSTERING
 * =============================================================================
 */

/**
 * K-Means clustering structure.
 */
typedef struct {
    double **centroids;
    int *assignments;
    size_t k;
    size_t n_features;
    size_t n_samples;
} KMeans;

/**
 * Creates a K-Means instance.
 */
KMeans *kmeans_create(size_t k, size_t n_features) {
    KMeans *km = (KMeans *)malloc(sizeof(KMeans));
    if (km == NULL) {
        return NULL;
    }
    
    km->k = k;
    km->n_features = n_features;
    km->n_samples = 0;
    km->assignments = NULL;
    
    km->centroids = (double **)malloc(k * sizeof(double *));
    for (size_t i = 0; i < k; i++) {
        km->centroids[i] = (double *)calloc(n_features, sizeof(double));
    }
    
    return km;
}

/**
 * Initialises centroids using K-Means++ algorithm.
 */
void kmeans_init_plusplus(KMeans *km, double **X, size_t n_samples) {
    /* Choose first centroid randomly */
    size_t first = rand() % n_samples;
    memcpy(km->centroids[0], X[first], km->n_features * sizeof(double));
    
    double *distances = (double *)malloc(n_samples * sizeof(double));
    
    /* Choose remaining centroids */
    for (size_t c = 1; c < km->k; c++) {
        double total_dist = 0.0;
        
        /* Compute distance to nearest centroid for each point */
        for (size_t i = 0; i < n_samples; i++) {
            double min_dist = DBL_MAX;
            for (size_t j = 0; j < c; j++) {
                double d = euclidean_distance(X[i], km->centroids[j], km->n_features);
                if (d < min_dist) {
                    min_dist = d;
                }
            }
            distances[i] = min_dist * min_dist;  /* Square for probability */
            total_dist += distances[i];
        }
        
        /* Choose next centroid with probability proportional to distance² */
        double r = ((double)rand() / RAND_MAX) * total_dist;
        double cumsum = 0.0;
        for (size_t i = 0; i < n_samples; i++) {
            cumsum += distances[i];
            if (cumsum >= r) {
                memcpy(km->centroids[c], X[i], km->n_features * sizeof(double));
                break;
            }
        }
    }
    
    free(distances);
}

/**
 * Fits K-Means clustering using Lloyd's algorithm.
 *
 * @param km K-Means instance
 * @param X Data points (n_samples × n_features)
 * @param n_samples Number of samples
 * @param max_iterations Maximum iterations
 * @param verbose Print progress if true
 */
void kmeans_fit(KMeans *km, double **X, size_t n_samples, 
                size_t max_iterations, bool verbose) {
    km->n_samples = n_samples;
    km->assignments = (int *)malloc(n_samples * sizeof(int));
    
    /* Initialise centroids using K-Means++ */
    kmeans_init_plusplus(km, X, n_samples);
    
    if (verbose) {
        printf("Training K-Means (K=%zu)...\n", km->k);
        printf("Iter    Inertia        Changes\n");
        printf("──────────────────────────────\n");
    }
    
    for (size_t iter = 0; iter < max_iterations; iter++) {
        int changes = 0;
        double inertia = 0.0;
        
        /* Assignment step: assign each point to nearest centroid */
        for (size_t i = 0; i < n_samples; i++) {
            double min_dist = DBL_MAX;
            int nearest = 0;
            
            for (size_t c = 0; c < km->k; c++) {
                double d = euclidean_distance(X[i], km->centroids[c], km->n_features);
                if (d < min_dist) {
                    min_dist = d;
                    nearest = (int)c;
                }
            }
            
            if (km->assignments[i] != nearest) {
                changes++;
                km->assignments[i] = nearest;
            }
            
            inertia += min_dist * min_dist;
        }
        
        if (verbose && (iter < 5 || iter % 5 == 0)) {
            printf("%4zu    %10.4f      %4d\n", iter, inertia, changes);
        }
        
        /* Update step: recompute centroids */
        size_t *counts = (size_t *)calloc(km->k, sizeof(size_t));
        
        /* Reset centroids */
        for (size_t c = 0; c < km->k; c++) {
            memset(km->centroids[c], 0, km->n_features * sizeof(double));
        }
        
        /* Sum all points in each cluster */
        for (size_t i = 0; i < n_samples; i++) {
            int c = km->assignments[i];
            for (size_t j = 0; j < km->n_features; j++) {
                km->centroids[c][j] += X[i][j];
            }
            counts[c]++;
        }
        
        /* Divide by count to get mean */
        for (size_t c = 0; c < km->k; c++) {
            if (counts[c] > 0) {
                for (size_t j = 0; j < km->n_features; j++) {
                    km->centroids[c][j] /= (double)counts[c];
                }
            }
        }
        
        free(counts);
        
        /* Check convergence */
        if (changes == 0) {
            if (verbose) {
                printf("Converged at iteration %zu\n", iter);
            }
            break;
        }
    }
}

/**
 * Predicts cluster assignment for a new point.
 */
int kmeans_predict(const KMeans *km, const double *x) {
    double min_dist = DBL_MAX;
    int nearest = 0;
    
    for (size_t c = 0; c < km->k; c++) {
        double d = euclidean_distance(x, km->centroids[c], km->n_features);
        if (d < min_dist) {
            min_dist = d;
            nearest = (int)c;
        }
    }
    
    return nearest;
}

/**
 * Frees K-Means instance.
 */
void kmeans_free(KMeans *km) {
    if (km != NULL) {
        for (size_t i = 0; i < km->k; i++) {
            free(km->centroids[i]);
        }
        free(km->centroids);
        free(km->assignments);
        free(km);
    }
}

/**
 * Demonstrates Part 6: K-Means Clustering
 */
void demo_part6(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: K-Means Clustering                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("K-Means Algorithm (Lloyd's):\n");
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│  1. Initialise K centroids (K-Means++)                          │\n");
    printf("│  2. Repeat until convergence:                                   │\n");
    printf("│     a. Assign each point to nearest centroid                    │\n");
    printf("│     b. Update centroids as mean of assigned points              │\n");
    printf("└─────────────────────────────────────────────────────────────────┘\n\n");
    
    /* Generate clustered data */
    printf("Generating 3-cluster dataset...\n");
    size_t n_samples = 45;
    double X_data[45][2];
    double *X[45];
    
    srand(42);
    
    /* Cluster 0: centred at (2, 2) */
    for (int i = 0; i < 15; i++) {
        X_data[i][0] = 2.0 + ((double)rand() / RAND_MAX - 0.5) * 2.0;
        X_data[i][1] = 2.0 + ((double)rand() / RAND_MAX - 0.5) * 2.0;
        X[i] = X_data[i];
    }
    /* Cluster 1: centred at (8, 2) */
    for (int i = 15; i < 30; i++) {
        X_data[i][0] = 8.0 + ((double)rand() / RAND_MAX - 0.5) * 2.0;
        X_data[i][1] = 2.0 + ((double)rand() / RAND_MAX - 0.5) * 2.0;
        X[i] = X_data[i];
    }
    /* Cluster 2: centred at (5, 8) */
    for (int i = 30; i < 45; i++) {
        X_data[i][0] = 5.0 + ((double)rand() / RAND_MAX - 0.5) * 2.0;
        X_data[i][1] = 8.0 + ((double)rand() / RAND_MAX - 0.5) * 2.0;
        X[i] = X_data[i];
    }
    
    printf("True cluster centres: (2,2), (8,2), (5,8)\n\n");
    
    /* Fit K-Means */
    KMeans *km = kmeans_create(3, 2);
    kmeans_fit(km, X, n_samples, 50, true);
    
    printf("\nLearned centroids:\n");
    for (size_t c = 0; c < km->k; c++) {
        printf("  Cluster %zu: (%.3f, %.3f)\n", c,
               km->centroids[c][0], km->centroids[c][1]);
    }
    
    /* Count assignments */
    int counts[3] = {0};
    for (size_t i = 0; i < n_samples; i++) {
        counts[km->assignments[i]]++;
    }
    printf("\nCluster sizes: %d, %d, %d\n", counts[0], counts[1], counts[2]);
    
    /* Test prediction */
    printf("\nTest predictions:\n");
    double test_points[3][2] = {{2.0, 2.0}, {8.0, 2.0}, {5.0, 8.0}};
    for (int i = 0; i < 3; i++) {
        int pred = kmeans_predict(km, test_points[i]);
        printf("  (%.1f, %.1f) -> Cluster %d\n",
               test_points[i][0], test_points[i][1], pred);
    }
    
    kmeans_free(km);
    printf("\n✓ K-Means clustering demonstrated successfully\n");
}

/* =============================================================================
 * PART 7: DECISION TREE (SIMPLIFIED)
 * =============================================================================
 */

/**
 * Decision Tree Node structure.
 */
typedef struct TreeNode {
    int feature_index;      /* Which feature to split on (-1 for leaf) */
    double threshold;       /* Split threshold */
    int class_label;        /* Class label (for leaf nodes) */
    struct TreeNode *left;  /* Left child (feature < threshold) */
    struct TreeNode *right; /* Right child (feature >= threshold) */
} TreeNode;

/**
 * Computes Gini impurity for a set of labels.
 * Gini = 1 - Σ(p_i)²
 * where p_i is the proportion of class i
 */
double gini_impurity(const int *labels, size_t n, int num_classes) {
    if (n == 0) return 0.0;
    
    int *counts = (int *)calloc(num_classes, sizeof(int));
    for (size_t i = 0; i < n; i++) {
        counts[labels[i]]++;
    }
    
    double gini = 1.0;
    for (int c = 0; c < num_classes; c++) {
        double p = (double)counts[c] / (double)n;
        gini -= p * p;
    }
    
    free(counts);
    return gini;
}

/**
 * Creates a leaf node.
 */
TreeNode *create_leaf(const int *labels, size_t n) {
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    node->feature_index = -1;
    node->threshold = 0.0;
    node->left = NULL;
    node->right = NULL;
    
    /* Find majority class */
    int counts[10] = {0};
    int max_class = 0;
    for (size_t i = 0; i < n; i++) {
        counts[labels[i]]++;
        if (labels[i] > max_class) max_class = labels[i];
    }
    
    int majority = 0;
    int max_count = 0;
    for (int c = 0; c <= max_class; c++) {
        if (counts[c] > max_count) {
            max_count = counts[c];
            majority = c;
        }
    }
    
    node->class_label = majority;
    return node;
}

/**
 * Builds a simple decision tree recursively.
 * Simplified version: only considers median split for each feature.
 */
TreeNode *build_tree(double **X, int *y, size_t n, size_t n_features,
                     int num_classes, int max_depth, int depth) {
    /* Base cases */
    if (depth >= max_depth || n <= 1) {
        return create_leaf(y, n);
    }
    
    /* Check if all same class */
    bool all_same = true;
    for (size_t i = 1; i < n; i++) {
        if (y[i] != y[0]) {
            all_same = false;
            break;
        }
    }
    if (all_same) {
        return create_leaf(y, n);
    }
    
    /* Find best split */
    double best_gini = 1.0;
    int best_feature = 0;
    double best_threshold = 0.0;
    
    for (size_t f = 0; f < n_features; f++) {
        /* Find min and max for this feature */
        double fmin = X[0][f], fmax = X[0][f];
        for (size_t i = 1; i < n; i++) {
            if (X[i][f] < fmin) fmin = X[i][f];
            if (X[i][f] > fmax) fmax = X[i][f];
        }
        
        /* Try a few thresholds */
        for (int t = 1; t <= 4; t++) {
            double threshold = fmin + (fmax - fmin) * t / 5.0;
            
            /* Split data */
            int *y_left = (int *)malloc(n * sizeof(int));
            int *y_right = (int *)malloc(n * sizeof(int));
            size_t n_left = 0, n_right = 0;
            
            for (size_t i = 0; i < n; i++) {
                if (X[i][f] < threshold) {
                    y_left[n_left++] = y[i];
                } else {
                    y_right[n_right++] = y[i];
                }
            }
            
            /* Compute weighted Gini */
            if (n_left > 0 && n_right > 0) {
                double gini_left = gini_impurity(y_left, n_left, num_classes);
                double gini_right = gini_impurity(y_right, n_right, num_classes);
                double weighted_gini = (n_left * gini_left + n_right * gini_right) / n;
                
                if (weighted_gini < best_gini) {
                    best_gini = weighted_gini;
                    best_feature = (int)f;
                    best_threshold = threshold;
                }
            }
            
            free(y_left);
            free(y_right);
        }
    }
    
    /* Create node */
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    node->feature_index = best_feature;
    node->threshold = best_threshold;
    node->class_label = -1;
    
    /* Split data and recurse */
    double **X_left = (double **)malloc(n * sizeof(double *));
    double **X_right = (double **)malloc(n * sizeof(double *));
    int *y_left = (int *)malloc(n * sizeof(int));
    int *y_right = (int *)malloc(n * sizeof(int));
    size_t n_left = 0, n_right = 0;
    
    for (size_t i = 0; i < n; i++) {
        if (X[i][best_feature] < best_threshold) {
            X_left[n_left] = X[i];
            y_left[n_left] = y[i];
            n_left++;
        } else {
            X_right[n_right] = X[i];
            y_right[n_right] = y[i];
            n_right++;
        }
    }
    
    node->left = build_tree(X_left, y_left, n_left, n_features,
                            num_classes, max_depth, depth + 1);
    node->right = build_tree(X_right, y_right, n_right, n_features,
                             num_classes, max_depth, depth + 1);
    
    free(X_left);
    free(X_right);
    free(y_left);
    free(y_right);
    
    return node;
}

/**
 * Predicts class using decision tree.
 */
int tree_predict(const TreeNode *node, const double *x) {
    if (node->feature_index == -1) {
        return node->class_label;
    }
    
    if (x[node->feature_index] < node->threshold) {
        return tree_predict(node->left, x);
    } else {
        return tree_predict(node->right, x);
    }
}

/**
 * Prints decision tree structure.
 */
void tree_print(const TreeNode *node, int depth) {
    for (int i = 0; i < depth; i++) printf("  ");
    
    if (node->feature_index == -1) {
        printf("Leaf: class %d\n", node->class_label);
    } else {
        printf("Split: X[%d] < %.3f\n", node->feature_index, node->threshold);
        tree_print(node->left, depth + 1);
        tree_print(node->right, depth + 1);
    }
}

/**
 * Frees decision tree.
 */
void tree_free(TreeNode *node) {
    if (node != NULL) {
        tree_free(node->left);
        tree_free(node->right);
        free(node);
    }
}

/**
 * Demonstrates Part 7: Decision Tree
 */
void demo_part7(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 7: Decision Tree Classification                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Decision Tree splits data recursively using feature thresholds.\n");
    printf("Gini impurity: G = 1 - Σ(p_i)²\n\n");
    
    /* Create simple dataset */
    size_t n = 20;
    double X_data[20][2];
    int y_data[20];
    double *X[20];
    
    srand(42);
    
    /* Class 0: lower-left quadrant */
    for (int i = 0; i < 10; i++) {
        X_data[i][0] = 1.0 + ((double)rand() / RAND_MAX) * 2.0;
        X_data[i][1] = 1.0 + ((double)rand() / RAND_MAX) * 2.0;
        y_data[i] = 0;
        X[i] = X_data[i];
    }
    /* Class 1: upper-right quadrant */
    for (int i = 10; i < 20; i++) {
        X_data[i][0] = 4.0 + ((double)rand() / RAND_MAX) * 2.0;
        X_data[i][1] = 4.0 + ((double)rand() / RAND_MAX) * 2.0;
        y_data[i] = 1;
        X[i] = X_data[i];
    }
    
    printf("Building decision tree (max_depth=3)...\n\n");
    
    TreeNode *tree = build_tree(X, y_data, n, 2, 2, 3, 0);
    
    printf("Tree structure:\n");
    tree_print(tree, 0);
    
    /* Test predictions */
    printf("\nTest predictions:\n");
    double tests[4][2] = {{1.5, 1.5}, {5.0, 5.0}, {3.0, 3.0}, {1.0, 5.0}};
    for (int i = 0; i < 4; i++) {
        int pred = tree_predict(tree, tests[i]);
        printf("  (%.1f, %.1f) -> Class %d\n", tests[i][0], tests[i][1], pred);
    }
    
    tree_free(tree);
    printf("\n✓ Decision tree demonstrated successfully\n");
}

/* =============================================================================
 * PART 8: PERCEPTRON AND SIMPLE NEURAL NETWORK
 * =============================================================================
 */

/**
 * Perceptron: single neuron model.
 * Output: sign(w · x + b)
 */
typedef struct {
    double *weights;
    double bias;
    size_t n_features;
} Perceptron;

/**
 * Creates a perceptron.
 */
Perceptron *perceptron_create(size_t n_features) {
    Perceptron *p = (Perceptron *)malloc(sizeof(Perceptron));
    p->n_features = n_features;
    p->weights = (double *)calloc(n_features, sizeof(double));
    p->bias = 0.0;
    return p;
}

/**
 * Frees a perceptron.
 */
void perceptron_free(Perceptron *p) {
    if (p != NULL) {
        free(p->weights);
        free(p);
    }
}

/**
 * Perceptron prediction: returns 0 or 1.
 */
int perceptron_predict(const Perceptron *p, const double *x) {
    double z = p->bias;
    for (size_t i = 0; i < p->n_features; i++) {
        z += p->weights[i] * x[i];
    }
    return z >= 0.0 ? 1 : 0;
}

/**
 * Trains perceptron using the perceptron learning rule.
 */
void perceptron_fit(Perceptron *p, double **X, int *y, size_t n,
                    size_t max_iterations, bool verbose) {
    if (verbose) {
        printf("Training perceptron...\n");
    }
    
    for (size_t iter = 0; iter < max_iterations; iter++) {
        int errors = 0;
        
        for (size_t i = 0; i < n; i++) {
            int pred = perceptron_predict(p, X[i]);
            
            if (pred != y[i]) {
                errors++;
                double update = (y[i] == 1) ? 1.0 : -1.0;
                
                for (size_t j = 0; j < p->n_features; j++) {
                    p->weights[j] += update * X[i][j];
                }
                p->bias += update;
            }
        }
        
        if (verbose && (iter < 5 || iter % 10 == 0)) {
            printf("  Iteration %zu: %d errors\n", iter, errors);
        }
        
        if (errors == 0) {
            if (verbose) {
                printf("  Converged at iteration %zu\n", iter);
            }
            break;
        }
    }
}

/**
 * ReLU activation function.
 */
double relu(double x) {
    return x > 0.0 ? x : 0.0;
}

/**
 * Simple two-layer neural network for XOR.
 * Architecture: 2 inputs -> 2 hidden neurons -> 1 output
 */
typedef struct {
    double w_hidden[2][2];  /* Weights from input to hidden */
    double b_hidden[2];     /* Hidden biases */
    double w_output[2];     /* Weights from hidden to output */
    double b_output;        /* Output bias */
} SimpleNN;

/**
 * Forward pass through simple neural network.
 */
double nn_forward(const SimpleNN *nn, double x1, double x2) {
    /* Hidden layer */
    double h1 = sigmoid(nn->w_hidden[0][0] * x1 + nn->w_hidden[0][1] * x2 + nn->b_hidden[0]);
    double h2 = sigmoid(nn->w_hidden[1][0] * x1 + nn->w_hidden[1][1] * x2 + nn->b_hidden[1]);
    
    /* Output layer */
    double output = sigmoid(nn->w_output[0] * h1 + nn->w_output[1] * h2 + nn->b_output);
    
    return output;
}

/**
 * Demonstrates Part 8: Perceptron and Neural Network
 */
void demo_part8(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 8: Perceptron and Neural Network                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Part A: Perceptron on linearly separable data */
    printf("--- Part A: Perceptron on Linearly Separable Data ---\n\n");
    
    double X_data[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    int y_and[4] = {0, 0, 0, 1};  /* AND gate */
    int y_or[4] = {0, 1, 1, 1};   /* OR gate */
    int y_xor[4] = {0, 1, 1, 0};  /* XOR gate */
    double *X[4] = {X_data[0], X_data[1], X_data[2], X_data[3]};
    
    /* Train on AND gate (linearly separable) */
    printf("Training perceptron on AND gate:\n");
    Perceptron *p_and = perceptron_create(2);
    perceptron_fit(p_and, X, y_and, 4, 100, true);
    
    printf("AND gate predictions:\n");
    for (int i = 0; i < 4; i++) {
        printf("  (%.0f, %.0f) -> %d (expected: %d)\n",
               X_data[i][0], X_data[i][1],
               perceptron_predict(p_and, X_data[i]), y_and[i]);
    }
    
    perceptron_free(p_and);
    
    /* Train on OR gate (linearly separable) */
    printf("\nTraining perceptron on OR gate:\n");
    Perceptron *p_or = perceptron_create(2);
    perceptron_fit(p_or, X, y_or, 4, 100, true);
    
    printf("OR gate predictions:\n");
    for (int i = 0; i < 4; i++) {
        printf("  (%.0f, %.0f) -> %d (expected: %d)\n",
               X_data[i][0], X_data[i][1],
               perceptron_predict(p_or, X_data[i]), y_or[i]);
    }
    
    perceptron_free(p_or);
    
    /* XOR: not linearly separable - perceptron fails */
    printf("\nTraining perceptron on XOR gate (will fail!):\n");
    Perceptron *p_xor = perceptron_create(2);
    perceptron_fit(p_xor, X, y_xor, 4, 100, true);
    
    printf("XOR gate predictions (perceptron fails):\n");
    for (int i = 0; i < 4; i++) {
        printf("  (%.0f, %.0f) -> %d (expected: %d) %s\n",
               X_data[i][0], X_data[i][1],
               perceptron_predict(p_xor, X_data[i]), y_xor[i],
               perceptron_predict(p_xor, X_data[i]) != y_xor[i] ? "✗" : "✓");
    }
    perceptron_free(p_xor);
    
    /* Part B: Two-layer neural network solves XOR */
    printf("\n--- Part B: Two-Layer Neural Network Solves XOR ---\n\n");
    printf("Architecture: 2 inputs -> 2 hidden (sigmoid) -> 1 output (sigmoid)\n\n");
    
    /* Pre-computed weights that solve XOR */
    SimpleNN nn = {
        .w_hidden = {{20.0, 20.0}, {-20.0, -20.0}},
        .b_hidden = {-10.0, 30.0},
        .w_output = {20.0, 20.0},
        .b_output = -30.0
    };
    
    printf("Using pre-computed weights:\n");
    printf("  Hidden weights:\n");
    printf("    w[0] = [%.1f, %.1f], b[0] = %.1f\n",
           nn.w_hidden[0][0], nn.w_hidden[0][1], nn.b_hidden[0]);
    printf("    w[1] = [%.1f, %.1f], b[1] = %.1f\n",
           nn.w_hidden[1][0], nn.w_hidden[1][1], nn.b_hidden[1]);
    printf("  Output weights: [%.1f, %.1f], b = %.1f\n\n",
           nn.w_output[0], nn.w_output[1], nn.b_output);
    
    printf("XOR gate predictions (neural network succeeds):\n");
    for (int i = 0; i < 4; i++) {
        double output = nn_forward(&nn, X_data[i][0], X_data[i][1]);
        int pred = output >= 0.5 ? 1 : 0;
        printf("  (%.0f, %.0f) -> %.4f -> %d (expected: %d) %s\n",
               X_data[i][0], X_data[i][1], output, pred, y_xor[i],
               pred == y_xor[i] ? "✓" : "✗");
    }
    
    printf("\n✓ Neural network concepts demonstrated successfully\n");
}

/* =============================================================================
 * PART 9: DATA NORMALISATION
 * =============================================================================
 */

/**
 * Min-Max normalisation: scales features to [0, 1].
 * x_norm = (x - min) / (max - min)
 */
void minmax_normalise(double *data, size_t n, double *min_out, double *max_out) {
    double min_val = data[0], max_val = data[0];
    
    for (size_t i = 1; i < n; i++) {
        if (data[i] < min_val) min_val = data[i];
        if (data[i] > max_val) max_val = data[i];
    }
    
    *min_out = min_val;
    *max_out = max_val;
    
    double range = max_val - min_val;
    if (range > 0) {
        for (size_t i = 0; i < n; i++) {
            data[i] = (data[i] - min_val) / range;
        }
    }
}

/**
 * Z-score normalisation (standardisation).
 * x_norm = (x - mean) / std
 */
void zscore_normalise(double *data, size_t n, double *mean_out, double *std_out) {
    /* Compute mean */
    double mean = 0.0;
    for (size_t i = 0; i < n; i++) {
        mean += data[i];
    }
    mean /= n;
    
    /* Compute standard deviation */
    double variance = 0.0;
    for (size_t i = 0; i < n; i++) {
        variance += (data[i] - mean) * (data[i] - mean);
    }
    variance /= n;
    double std = sqrt(variance);
    
    *mean_out = mean;
    *std_out = std;
    
    if (std > 0) {
        for (size_t i = 0; i < n; i++) {
            data[i] = (data[i] - mean) / std;
        }
    }
}

/**
 * Demonstrates Part 9: Data Normalisation
 */
void demo_part9(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 9: Data Normalisation Techniques                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    double data[] = {10.0, 20.0, 30.0, 40.0, 50.0, 100.0, 200.0};
    size_t n = 7;
    
    printf("Original data: ");
    for (size_t i = 0; i < n; i++) {
        printf("%.1f ", data[i]);
    }
    printf("\n\n");
    
    /* Min-Max normalisation */
    double data_minmax[7];
    memcpy(data_minmax, data, n * sizeof(double));
    double min_val, max_val;
    minmax_normalise(data_minmax, n, &min_val, &max_val);
    
    printf("Min-Max Normalisation: x' = (x - min) / (max - min)\n");
    printf("  min = %.1f, max = %.1f\n", min_val, max_val);
    printf("  Normalised: ");
    for (size_t i = 0; i < n; i++) {
        printf("%.3f ", data_minmax[i]);
    }
    printf("\n\n");
    
    /* Z-score normalisation */
    double data_zscore[7];
    memcpy(data_zscore, data, n * sizeof(double));
    double mean, std;
    zscore_normalise(data_zscore, n, &mean, &std);
    
    printf("Z-Score Normalisation: x' = (x - mean) / std\n");
    printf("  mean = %.2f, std = %.2f\n", mean, std);
    printf("  Normalised: ");
    for (size_t i = 0; i < n; i++) {
        printf("%+.3f ", data_zscore[i]);
    }
    printf("\n\n");
    
    printf("Why normalise?\n");
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│  1. Gradient descent converges faster                           │\n");
    printf("│  2. Features contribute equally regardless of scale             │\n");
    printf("│  3. Distance-based algorithms (K-NN, K-Means) work better       │\n");
    printf("│  4. Numerical stability in matrix operations                    │\n");
    printf("└─────────────────────────────────────────────────────────────────┘\n");
    
    printf("\n✓ Data normalisation demonstrated successfully\n");
}

/* =============================================================================
 * PART 10: COMPREHENSIVE BENCHMARK
 * =============================================================================
 */

/**
 * Measures accuracy of a classifier.
 */
double compute_accuracy(const int *y_true, const int *y_pred, size_t n) {
    int correct = 0;
    for (size_t i = 0; i < n; i++) {
        if (y_true[i] == y_pred[i]) {
            correct++;
        }
    }
    return (double)correct / n;
}

/**
 * Demonstrates Part 10: Comprehensive Benchmark
 */
void demo_part10(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 10: ML Algorithms Comparison                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Algorithm Characteristics Summary:\n\n");
    
    printf("┌───────────────────┬───────────┬───────────┬───────────┬───────────┐\n");
    printf("│     Algorithm     │  Type     │  Training │ Inference │   Space   │\n");
    printf("├───────────────────┼───────────┼───────────┼───────────┼───────────┤\n");
    printf("│ Linear Regression │ Supervised│  O(n·m·i) │  O(m)     │  O(m)     │\n");
    printf("│ Logistic Regress. │ Supervised│  O(n·m·i) │  O(m)     │  O(m)     │\n");
    printf("│ K-NN              │ Supervised│  O(1)     │  O(n·m)   │  O(n·m)   │\n");
    printf("│ K-Means           │ Unsupervis│  O(n·k·i) │  O(k·m)   │  O(k·m)   │\n");
    printf("│ Decision Tree     │ Supervised│  O(n·m·d) │  O(d)     │  O(nodes) │\n");
    printf("│ Perceptron        │ Supervised│  O(n·m·i) │  O(m)     │  O(m)     │\n");
    printf("│ Neural Network    │ Supervised│  O(n·w·i) │  O(w)     │  O(w)     │\n");
    printf("└───────────────────┴───────────┴───────────┴───────────┴───────────┘\n\n");
    
    printf("Legend: n=samples, m=features, i=iterations, k=clusters, d=depth, w=weights\n\n");
    
    printf("When to Use Each Algorithm:\n\n");
    
    printf("• Linear Regression:    Continuous target, linear relationship\n");
    printf("• Logistic Regression:  Binary classification, probability needed\n");
    printf("• K-NN:                 Non-linear boundaries, small datasets\n");
    printf("• K-Means:              Clustering, unsupervised grouping\n");
    printf("• Decision Tree:        Interpretable rules, mixed feature types\n");
    printf("• Perceptron:           Simple linear classification\n");
    printf("• Neural Network:       Complex patterns, large datasets\n\n");
    
    printf("Key Insights from This Laboratory:\n");
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│  1. Gradient descent is fundamental to training ML models       │\n");
    printf("│  2. Linear models have elegant closed-form solutions            │\n");
    printf("│  3. K-NN is a lazy learner - stores all training data          │\n");
    printf("│  4. K-Means iteratively refines cluster centroids              │\n");
    printf("│  5. Decision trees recursively partition feature space         │\n");
    printf("│  6. Neural networks can learn non-linear decision boundaries   │\n");
    printf("│  7. Data normalisation is crucial for many algorithms          │\n");
    printf("└─────────────────────────────────────────────────────────────────┘\n");
    
    printf("\n✓ ML algorithms comparison complete\n");
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 18: MACHINE LEARNING FUNDAMENTALS IN C               ║\n");
    printf("║                  Complete Working Example                     ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Set random seed for reproducibility */
    srand(42);
    
    demo_part1();   /* Matrix Operations */
    demo_part2();   /* Gradient Descent */
    demo_part3();   /* Linear Regression */
    demo_part4();   /* Logistic Regression */
    demo_part5();   /* K-NN */
    demo_part6();   /* K-Means */
    demo_part7();   /* Decision Tree */
    demo_part8();   /* Perceptron and Neural Network */
    demo_part9();   /* Data Normalisation */
    demo_part10();  /* Comprehensive Benchmark */
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                  DEMONSTRATION COMPLETE                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

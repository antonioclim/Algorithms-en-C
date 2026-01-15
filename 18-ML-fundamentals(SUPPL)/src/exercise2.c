/**
 * =============================================================================
 * WEEK 18: MACHINE LEARNING FUNDAMENTALS IN C
 * Exercise 2: K-Nearest Neighbours and K-Means Clustering
 * =============================================================================
 *
 * This exercise implements two fundamental machine learning algorithms:
 *   1. K-Nearest Neighbours (K-NN) for classification
 *   2. K-Means clustering for unsupervised learning
 *
 * Both algorithms are applied to the famous Iris dataset, enabling direct
 * comparison between supervised and unsupervised approaches.
 *
 * Learning Objectives:
 *   - Implement distance metrics (Euclidean, Manhattan)
 *   - Apply K-NN classification with variable K
 *   - Implement K-Means with K-Means++ initialisation
 *   - Evaluate clustering quality with silhouette score
 *   - Compare classification vs clustering results
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c -lm
 * Usage: ./exercise2 [path_to_iris_data]
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
 * CONSTANTS AND CONFIGURATION
 * =============================================================================
 */

#define IRIS_SAMPLES      150
#define IRIS_FEATURES     4
#define IRIS_CLASSES      3
#define MAX_LINE_LENGTH   256
#define DEFAULT_K_NN      5
#define DEFAULT_K_MEANS   3
#define MAX_ITERATIONS    100
#define CONVERGENCE_THRESHOLD 1e-6

/* Feature names for Iris dataset */
static const char *FEATURE_NAMES[] = {
    "Sepal Length",
    "Sepal Width",
    "Petal Length",
    "Petal Width"
};

/* Class names for Iris dataset */
static const char *CLASS_NAMES[] = {
    "Iris-setosa",
    "Iris-versicolor",
    "Iris-virginica"
};

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

/**
 * Structure representing a single Iris sample
 */
typedef struct {
    double features[IRIS_FEATURES];     /* Feature values */
    int true_label;                      /* Ground truth class (0, 1, 2) */
    int predicted_label;                 /* Predicted class for evaluation */
} IrisSample;

/**
 * Structure for the complete Iris dataset
 */
typedef struct {
    IrisSample *samples;
    size_t count;
    double feature_min[IRIS_FEATURES];   /* For normalisation */
    double feature_max[IRIS_FEATURES];
    double feature_mean[IRIS_FEATURES];
    double feature_std[IRIS_FEATURES];
} IrisDataset;

/**
 * Structure for K-Means clustering result
 */
typedef struct {
    double **centroids;                  /* K centroids, each with IRIS_FEATURES */
    int *assignments;                    /* Cluster assignment for each sample */
    int k;                               /* Number of clusters */
    int iterations;                      /* Iterations until convergence */
    double inertia;                      /* Sum of squared distances to centroids */
} KMeansResult;

/**
 * Structure for distance and index pair (used in K-NN)
 */
typedef struct {
    double distance;
    int index;
} DistanceIndexPair;

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Generate Iris dataset synthetically (for when file not available)
 *
 * This generates data that approximates the real Iris dataset characteristics:
 *   - Setosa: small petals, distinct from others
 *   - Versicolor: medium-sized, overlaps with virginica
 *   - Virginica: larger overall, overlaps with versicolor
 */
void generate_synthetic_iris(IrisDataset *dataset) {
    dataset->count = IRIS_SAMPLES;
    dataset->samples = malloc(IRIS_SAMPLES * sizeof(IrisSample));

    if (!dataset->samples) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    /* Seed for reproducibility */
    srand(42);

    /* Mean and std for each class and feature (approximating real Iris) */
    /* Format: [class][feature] = {mean, std} */
    double params[3][4][2] = {
        /* Setosa */
        {{5.0, 0.35}, {3.4, 0.38}, {1.5, 0.17}, {0.2, 0.11}},
        /* Versicolor */
        {{5.9, 0.52}, {2.8, 0.31}, {4.3, 0.47}, {1.3, 0.20}},
        /* Virginica */
        {{6.6, 0.64}, {3.0, 0.32}, {5.5, 0.55}, {2.0, 0.27}}
    };

    for (int i = 0; i < IRIS_SAMPLES; i++) {
        int class_idx = i / 50;  /* 50 samples per class */
        dataset->samples[i].true_label = class_idx;
        dataset->samples[i].predicted_label = -1;

        for (int f = 0; f < IRIS_FEATURES; f++) {
            /* Box-Muller transform for normal distribution */
            double u1 = (rand() + 1.0) / (RAND_MAX + 1.0);
            double u2 = (rand() + 1.0) / (RAND_MAX + 1.0);
            double z = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);

            double mean = params[class_idx][f][0];
            double std = params[class_idx][f][1];
            dataset->samples[i].features[f] = mean + z * std;

            /* Clamp to reasonable values */
            if (dataset->samples[i].features[f] < 0.1) {
                dataset->samples[i].features[f] = 0.1;
            }
        }
    }

    printf("Generated synthetic Iris dataset with %d samples\n", IRIS_SAMPLES);
}

/**
 * Load Iris dataset from CSV file
 *
 * Expected format: sepal_length,sepal_width,petal_length,petal_width,class_name
 *
 * @param dataset Pointer to dataset structure to populate
 * @param filename Path to the CSV file
 * @return true if successful, false otherwise
 */
bool load_iris_from_file(IrisDataset *dataset, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Cannot open file '%s', generating synthetic data...\n", filename);
        return false;
    }

    dataset->samples = malloc(IRIS_SAMPLES * sizeof(IrisSample));
    if (!dataset->samples) {
        fclose(file);
        return false;
    }

    char line[MAX_LINE_LENGTH];
    dataset->count = 0;

    /* Skip header if present */
    if (fgets(line, sizeof(line), file)) {
        if (strstr(line, "sepal") || strstr(line, "Sepal")) {
            /* Header line, skip it */
        } else {
            /* No header, rewind */
            rewind(file);
        }
    }

    while (fgets(line, sizeof(line), file) && dataset->count < IRIS_SAMPLES) {
        IrisSample *sample = &dataset->samples[dataset->count];
        char class_name[64];

        int parsed = sscanf(line, "%lf,%lf,%lf,%lf,%63s",
                           &sample->features[0], &sample->features[1],
                           &sample->features[2], &sample->features[3],
                           class_name);

        if (parsed == 5) {
            /* Determine class label from name */
            if (strstr(class_name, "setosa")) {
                sample->true_label = 0;
            } else if (strstr(class_name, "versicolor")) {
                sample->true_label = 1;
            } else if (strstr(class_name, "virginica")) {
                sample->true_label = 2;
            } else {
                sample->true_label = -1;
            }
            sample->predicted_label = -1;
            dataset->count++;
        }
    }

    fclose(file);
    printf("Loaded %zu samples from '%s'\n", dataset->count, filename);
    return dataset->count > 0;
}

/**
 * Compute dataset statistics (min, max, mean, std)
 */
void compute_dataset_statistics(IrisDataset *dataset) {
    /* Initialise */
    for (int f = 0; f < IRIS_FEATURES; f++) {
        dataset->feature_min[f] = DBL_MAX;
        dataset->feature_max[f] = -DBL_MAX;
        dataset->feature_mean[f] = 0.0;
        dataset->feature_std[f] = 0.0;
    }

    /* First pass: min, max, sum for mean */
    for (size_t i = 0; i < dataset->count; i++) {
        for (int f = 0; f < IRIS_FEATURES; f++) {
            double val = dataset->samples[i].features[f];
            if (val < dataset->feature_min[f]) dataset->feature_min[f] = val;
            if (val > dataset->feature_max[f]) dataset->feature_max[f] = val;
            dataset->feature_mean[f] += val;
        }
    }

    /* Compute means */
    for (int f = 0; f < IRIS_FEATURES; f++) {
        dataset->feature_mean[f] /= dataset->count;
    }

    /* Second pass: variance for std */
    for (size_t i = 0; i < dataset->count; i++) {
        for (int f = 0; f < IRIS_FEATURES; f++) {
            double diff = dataset->samples[i].features[f] - dataset->feature_mean[f];
            dataset->feature_std[f] += diff * diff;
        }
    }

    for (int f = 0; f < IRIS_FEATURES; f++) {
        dataset->feature_std[f] = sqrt(dataset->feature_std[f] / dataset->count);
    }
}

/**
 * Print dataset statistics
 */
void print_dataset_statistics(const IrisDataset *dataset) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    DATASET STATISTICS                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    printf("%-15s %10s %10s %10s %10s\n",
           "Feature", "Min", "Max", "Mean", "Std");
    printf("─────────────────────────────────────────────────────────────────\n");

    for (int f = 0; f < IRIS_FEATURES; f++) {
        printf("%-15s %10.3f %10.3f %10.3f %10.3f\n",
               FEATURE_NAMES[f],
               dataset->feature_min[f], dataset->feature_max[f],
               dataset->feature_mean[f], dataset->feature_std[f]);
    }

    printf("\nClass distribution:\n");
    int class_counts[IRIS_CLASSES] = {0};
    for (size_t i = 0; i < dataset->count; i++) {
        if (dataset->samples[i].true_label >= 0 &&
            dataset->samples[i].true_label < IRIS_CLASSES) {
            class_counts[dataset->samples[i].true_label]++;
        }
    }

    for (int c = 0; c < IRIS_CLASSES; c++) {
        printf("  %-20s: %d samples (%.1f%%)\n",
               CLASS_NAMES[c], class_counts[c],
               100.0 * class_counts[c] / dataset->count);
    }
}

/**
 * Free dataset memory
 */
void free_dataset(IrisDataset *dataset) {
    if (dataset->samples) {
        free(dataset->samples);
        dataset->samples = NULL;
    }
    dataset->count = 0;
}

/* =============================================================================
 * DISTANCE METRICS
 * =============================================================================
 */

/**
 * Compute Euclidean distance between two feature vectors
 *
 * Formula: d(a, b) = sqrt(Σ(a_i - b_i)²)
 *
 * Time Complexity: O(d) where d = number of dimensions
 * Space Complexity: O(1)
 *
 * TODO: Implement the Euclidean distance formula
 */
double euclidean_distance(const double *a, const double *b, int dim) {
    double sum = 0.0;

    /* ═══════════════════════════════════════════════════════════════════════
     * TODO: Calculate the sum of squared differences
     *
     * For each dimension i from 0 to dim-1:
     *   1. Compute the difference: diff = a[i] - b[i]
     *   2. Add diff * diff to sum
     *
     * Finally, return sqrt(sum)
     * ═══════════════════════════════════════════════════════════════════════
     */

    /* YOUR CODE HERE */
    (void)a;  /* Remove this line when implementing */
    (void)b;  /* Remove this line when implementing */
    (void)dim; /* Remove this line when implementing */

    return sqrt(sum);
}

/**
 * Compute Manhattan distance between two feature vectors
 *
 * Formula: d(a, b) = Σ|a_i - b_i|
 *
 * Also known as L1 distance or city block distance
 *
 * Time Complexity: O(d)
 * Space Complexity: O(1)
 *
 * TODO: Implement the Manhattan distance formula
 */
double manhattan_distance(const double *a, const double *b, int dim) {
    double sum = 0.0;

    /* ═══════════════════════════════════════════════════════════════════════
     * TODO: Calculate the sum of absolute differences
     *
     * For each dimension i:
     *   sum += fabs(a[i] - b[i])
     * ═══════════════════════════════════════════════════════════════════════
     */

    /* YOUR CODE HERE */
    (void)a;
    (void)b;
    (void)dim;

    return sum;
}

/* =============================================================================
 * PART 1: K-NEAREST NEIGHBOURS CLASSIFICATION
 * =============================================================================
 */

/**
 * Compare function for sorting distance-index pairs
 */
int compare_distances(const void *a, const void *b) {
    const DistanceIndexPair *pa = (const DistanceIndexPair *)a;
    const DistanceIndexPair *pb = (const DistanceIndexPair *)b;

    if (pa->distance < pb->distance) return -1;
    if (pa->distance > pb->distance) return 1;
    return 0;
}

/**
 * Predict class for a single sample using K-NN
 *
 * Algorithm:
 *   1. Compute distances to all training samples
 *   2. Sort by distance
 *   3. Select K nearest neighbours
 *   4. Vote among neighbours (majority wins)
 *
 * @param query Feature vector to classify
 * @param train Training dataset
 * @param train_indices Indices of training samples to use
 * @param n_train Number of training samples
 * @param k Number of neighbours
 * @param use_manhattan If true, use Manhattan distance; else Euclidean
 * @return Predicted class label
 *
 * TODO: Complete the K-NN prediction algorithm
 */
int knn_predict_single(const double *query, const IrisDataset *train,
                       const int *train_indices, int n_train,
                       int k, bool use_manhattan) {
    /* Allocate array for distances */
    DistanceIndexPair *distances = malloc(n_train * sizeof(DistanceIndexPair));
    if (!distances) return -1;

    /* ═══════════════════════════════════════════════════════════════════════
     * TODO: Step 1 - Compute distances from query to all training samples
     *
     * For each training sample i (using train_indices[i]):
     *   1. Get pointer to training sample features
     *   2. Compute distance using either Manhattan or Euclidean
     *   3. Store in distances[i].distance
     *   4. Store train_indices[i] in distances[i].index
     * ═══════════════════════════════════════════════════════════════════════
     */

    /* YOUR CODE HERE */
    (void)query;
    (void)train;
    (void)train_indices;
    (void)use_manhattan;

    /* Step 2: Sort distances (provided) */
    qsort(distances, n_train, sizeof(DistanceIndexPair), compare_distances);

    /* ═══════════════════════════════════════════════════════════════════════
     * TODO: Step 3 - Count votes among K nearest neighbours
     *
     * Create an array votes[IRIS_CLASSES] initialised to 0
     * For each of the first K elements in sorted distances:
     *   1. Get the sample index: idx = distances[j].index
     *   2. Get the class label: label = train->samples[idx].true_label
     *   3. Increment votes[label]
     * ═══════════════════════════════════════════════════════════════════════
     */

    int votes[IRIS_CLASSES] = {0};

    /* YOUR CODE HERE */
    (void)k;

    /* Step 4: Find majority vote (provided) */
    int predicted = 0;
    int max_votes = votes[0];

    for (int c = 1; c < IRIS_CLASSES; c++) {
        if (votes[c] > max_votes) {
            max_votes = votes[c];
            predicted = c;
        }
    }

    free(distances);
    return predicted;
}

/**
 * Evaluate K-NN classifier using train/test split
 *
 * @param dataset Full dataset
 * @param test_ratio Fraction of data to use for testing (e.g. 0.2)
 * @param k Number of neighbours
 * @param use_manhattan Distance metric selection
 * @return Classification accuracy (0.0 to 1.0)
 */
double knn_evaluate(IrisDataset *dataset, double test_ratio, int k,
                    bool use_manhattan) {
    int n_test = (int)(dataset->count * test_ratio);
    int n_train = dataset->count - n_test;

    /* Create index arrays for train/test split */
    int *all_indices = malloc(dataset->count * sizeof(int));
    for (size_t i = 0; i < dataset->count; i++) {
        all_indices[i] = i;
    }

    /* Shuffle indices */
    srand(42);  /* Fixed seed for reproducibility */
    for (size_t i = dataset->count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = all_indices[i];
        all_indices[i] = all_indices[j];
        all_indices[j] = temp;
    }

    /* Split into train and test */
    int *train_indices = all_indices;
    int *test_indices = all_indices + n_train;

    /* Classify test samples */
    int correct = 0;
    for (int i = 0; i < n_test; i++) {
        int test_idx = test_indices[i];
        double *query = dataset->samples[test_idx].features;

        int predicted = knn_predict_single(query, dataset, train_indices,
                                           n_train, k, use_manhattan);

        dataset->samples[test_idx].predicted_label = predicted;

        if (predicted == dataset->samples[test_idx].true_label) {
            correct++;
        }
    }

    double accuracy = (double)correct / n_test;
    free(all_indices);

    return accuracy;
}

/**
 * Demonstrate K-NN classification
 */
void demo_knn(IrisDataset *dataset) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: K-NEAREST NEIGHBOURS CLASSIFICATION              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    printf("K-NN is a non-parametric, instance-based learning algorithm.\n");
    printf("It classifies new points based on majority vote of K nearest\n");
    printf("neighbours in the training set.\n\n");

    printf("Testing different values of K with 80/20 train/test split:\n\n");

    printf("%-5s %-15s %-15s\n", "K", "Euclidean Acc", "Manhattan Acc");
    printf("─────────────────────────────────────────\n");

    int k_values[] = {1, 3, 5, 7, 9, 11};
    int n_k = sizeof(k_values) / sizeof(k_values[0]);

    for (int i = 0; i < n_k; i++) {
        int k = k_values[i];
        double acc_euclidean = knn_evaluate(dataset, 0.2, k, false);
        double acc_manhattan = knn_evaluate(dataset, 0.2, k, true);

        printf("%-5d %13.1f%% %13.1f%%\n",
               k, acc_euclidean * 100, acc_manhattan * 100);
    }

    printf("\nNote: Your results may differ until you complete the TODOs.\n");
    printf("Expected accuracy range with complete implementation: 90-97%%\n");
}

/* =============================================================================
 * PART 2: K-MEANS CLUSTERING
 * =============================================================================
 */

/**
 * Initialise centroids using K-Means++ algorithm
 *
 * K-Means++ chooses initial centroids that are spread apart:
 *   1. First centroid: random sample
 *   2. Subsequent centroids: sample with probability proportional to D²
 *      where D is distance to nearest existing centroid
 *
 * This leads to better clustering results than random initialisation.
 *
 * @param dataset Input dataset
 * @param k Number of clusters
 * @return Array of k centroid arrays (each with IRIS_FEATURES elements)
 *
 * TODO: Implement K-Means++ initialisation
 */
double **kmeans_plus_plus_init(const IrisDataset *dataset, int k) {
    /* Allocate centroids */
    double **centroids = malloc(k * sizeof(double *));
    for (int i = 0; i < k; i++) {
        centroids[i] = malloc(IRIS_FEATURES * sizeof(double));
    }

    /* Track which samples are chosen as centroids */
    bool *chosen = calloc(dataset->count, sizeof(bool));

    /* ═══════════════════════════════════════════════════════════════════════
     * TODO: Step 1 - Choose first centroid randomly
     *
     * 1. Pick a random index: first_idx = rand() % dataset->count
     * 2. Copy features from dataset->samples[first_idx] to centroids[0]
     * 3. Mark chosen[first_idx] = true
     * ═══════════════════════════════════════════════════════════════════════
     */

    /* YOUR CODE HERE */

    /* ═══════════════════════════════════════════════════════════════════════
     * TODO: Step 2 - Choose remaining k-1 centroids
     *
     * For centroid c from 1 to k-1:
     *   1. For each sample i, compute D[i] = minimum distance to any
     *      existing centroid (0 to c-1)
     *   2. Compute total_weight = Σ D[i]²
     *   3. Pick random threshold = (rand() / RAND_MAX) * total_weight
     *   4. Find sample where cumulative D² exceeds threshold
     *   5. Copy that sample's features to centroids[c]
     *   6. Mark it as chosen
     * ═══════════════════════════════════════════════════════════════════════
     */

    /* YOUR CODE HERE - placeholder copies first sample */
    for (int c = 0; c < k; c++) {
        int idx = c % dataset->count;
        for (int f = 0; f < IRIS_FEATURES; f++) {
            centroids[c][f] = dataset->samples[idx].features[f];
        }
    }

    free(chosen);
    return centroids;
}

/**
 * Assign each sample to the nearest centroid
 *
 * @param dataset Input dataset
 * @param centroids Current centroid positions
 * @param k Number of centroids
 * @param assignments Output array for cluster assignments
 * @return Inertia (sum of squared distances to assigned centroids)
 *
 * TODO: Implement cluster assignment
 */
double kmeans_assign(const IrisDataset *dataset, double **centroids,
                     int k, int *assignments) {
    double inertia = 0.0;

    /* ═══════════════════════════════════════════════════════════════════════
     * TODO: For each sample, find the nearest centroid
     *
     * For each sample i:
     *   1. Initialise best_cluster = 0, best_dist = DBL_MAX
     *   2. For each centroid c from 0 to k-1:
     *      - Compute dist = euclidean_distance(sample features, centroids[c])
     *      - If dist < best_dist: update best_dist and best_cluster
     *   3. Set assignments[i] = best_cluster
     *   4. Add best_dist * best_dist to inertia
     * ═══════════════════════════════════════════════════════════════════════
     */

    /* YOUR CODE HERE */
    (void)dataset;
    (void)centroids;
    (void)k;
    (void)assignments;

    return inertia;
}

/**
 * Update centroid positions based on current assignments
 *
 * New centroid = mean of all samples assigned to that cluster
 *
 * @param dataset Input dataset
 * @param centroids Centroid positions to update
 * @param k Number of centroids
 * @param assignments Current cluster assignments
 * @return Maximum centroid movement (for convergence check)
 *
 * TODO: Implement centroid update step
 */
double kmeans_update(const IrisDataset *dataset, double **centroids,
                     int k, const int *assignments) {
    double max_movement = 0.0;

    /* ═══════════════════════════════════════════════════════════════════════
     * TODO: Compute new centroid positions
     *
     * For each cluster c:
     *   1. Sum features of all samples assigned to cluster c
     *   2. Count samples in cluster c
     *   3. New centroid = sum / count (handle empty clusters!)
     *   4. Compute movement = distance from old centroid to new
     *   5. Update max_movement if this movement is larger
     * ═══════════════════════════════════════════════════════════════════════
     */

    /* YOUR CODE HERE */
    (void)dataset;
    (void)centroids;
    (void)k;
    (void)assignments;

    return max_movement;
}

/**
 * Run K-Means clustering algorithm
 *
 * @param dataset Input dataset
 * @param k Number of clusters
 * @param max_iter Maximum iterations
 * @return KMeansResult structure with clustering results
 */
KMeansResult kmeans_cluster(IrisDataset *dataset, int k, int max_iter) {
    KMeansResult result;
    result.k = k;
    result.iterations = 0;
    result.inertia = DBL_MAX;

    /* Initialise centroids using K-Means++ */
    result.centroids = kmeans_plus_plus_init(dataset, k);

    /* Allocate assignments array */
    result.assignments = malloc(dataset->count * sizeof(int));

    /* Lloyd's algorithm: iterate until convergence */
    for (int iter = 0; iter < max_iter; iter++) {
        result.iterations = iter + 1;

        /* Assignment step */
        result.inertia = kmeans_assign(dataset, result.centroids,
                                       k, result.assignments);

        /* Update step */
        double movement = kmeans_update(dataset, result.centroids,
                                        k, result.assignments);

        /* Check convergence */
        if (movement < CONVERGENCE_THRESHOLD) {
            printf("Converged after %d iterations\n", result.iterations);
            break;
        }
    }

    return result;
}

/**
 * Free K-Means result memory
 */
void free_kmeans_result(KMeansResult *result) {
    if (result->centroids) {
        for (int i = 0; i < result->k; i++) {
            free(result->centroids[i]);
        }
        free(result->centroids);
    }
    if (result->assignments) {
        free(result->assignments);
    }
}

/**
 * Compute silhouette score for clustering quality
 *
 * Silhouette = (b - a) / max(a, b)
 * where:
 *   a = mean distance to other samples in same cluster
 *   b = mean distance to samples in nearest different cluster
 *
 * Range: -1 to 1 (higher is better)
 */
double compute_silhouette_score(const IrisDataset *dataset,
                                const KMeansResult *result) {
    double total_silhouette = 0.0;
    int valid_samples = 0;

    for (size_t i = 0; i < dataset->count; i++) {
        int cluster_i = result->assignments[i];

        /* Compute a(i) - mean distance within cluster */
        double sum_same = 0.0;
        int count_same = 0;

        for (size_t j = 0; j < dataset->count; j++) {
            if (i != j && result->assignments[j] == cluster_i) {
                sum_same += euclidean_distance(dataset->samples[i].features,
                                               dataset->samples[j].features,
                                               IRIS_FEATURES);
                count_same++;
            }
        }

        double a_i = (count_same > 0) ? sum_same / count_same : 0.0;

        /* Compute b(i) - mean distance to nearest other cluster */
        double b_i = DBL_MAX;

        for (int c = 0; c < result->k; c++) {
            if (c != cluster_i) {
                double sum_other = 0.0;
                int count_other = 0;

                for (size_t j = 0; j < dataset->count; j++) {
                    if (result->assignments[j] == c) {
                        sum_other += euclidean_distance(dataset->samples[i].features,
                                                        dataset->samples[j].features,
                                                        IRIS_FEATURES);
                        count_other++;
                    }
                }

                if (count_other > 0) {
                    double mean_other = sum_other / count_other;
                    if (mean_other < b_i) {
                        b_i = mean_other;
                    }
                }
            }
        }

        /* Compute silhouette for this sample */
        if (a_i < b_i) {
            total_silhouette += (b_i - a_i) / b_i;
        } else if (a_i > b_i) {
            total_silhouette += (b_i - a_i) / a_i;
        }
        /* If a_i == b_i, silhouette contribution is 0 */

        valid_samples++;
    }

    return (valid_samples > 0) ? total_silhouette / valid_samples : 0.0;
}

/**
 * Compute cluster purity (how well clusters match true labels)
 */
double compute_purity(const IrisDataset *dataset, const KMeansResult *result) {
    int correct = 0;

    for (int c = 0; c < result->k; c++) {
        /* Count class distribution in this cluster */
        int class_counts[IRIS_CLASSES] = {0};

        for (size_t i = 0; i < dataset->count; i++) {
            if (result->assignments[i] == c) {
                int true_class = dataset->samples[i].true_label;
                if (true_class >= 0 && true_class < IRIS_CLASSES) {
                    class_counts[true_class]++;
                }
            }
        }

        /* Find majority class */
        int max_count = 0;
        for (int j = 0; j < IRIS_CLASSES; j++) {
            if (class_counts[j] > max_count) {
                max_count = class_counts[j];
            }
        }

        correct += max_count;
    }

    return (double)correct / dataset->count;
}

/**
 * Demonstrate K-Means clustering
 */
void demo_kmeans(IrisDataset *dataset) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: K-MEANS CLUSTERING                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    printf("K-Means is an unsupervised learning algorithm that partitions\n");
    printf("data into K clusters by minimising within-cluster variance.\n\n");

    printf("Running K-Means with K=3 (matching Iris classes)...\n\n");

    srand(42);
    KMeansResult result = kmeans_cluster(dataset, DEFAULT_K_MEANS, MAX_ITERATIONS);

    printf("\nClustering Results:\n");
    printf("─────────────────────────────────────────\n");
    printf("Iterations:      %d\n", result.iterations);
    printf("Inertia:         %.2f\n", result.inertia);

    double silhouette = compute_silhouette_score(dataset, &result);
    double purity = compute_purity(dataset, &result);

    printf("Silhouette Score: %.3f\n", silhouette);
    printf("Cluster Purity:   %.1f%%\n", purity * 100);

    /* Print centroid values */
    printf("\nFinal Centroids:\n");
    printf("%-10s", "Cluster");
    for (int f = 0; f < IRIS_FEATURES; f++) {
        printf(" %12s", FEATURE_NAMES[f]);
    }
    printf("\n");
    printf("─────────────────────────────────────────────────────────────────\n");

    for (int c = 0; c < result.k; c++) {
        printf("%-10d", c);
        for (int f = 0; f < IRIS_FEATURES; f++) {
            printf(" %12.2f", result.centroids[c][f]);
        }
        printf("\n");
    }

    /* Compare clusters to true labels */
    printf("\nCluster-Class Confusion Matrix:\n");
    printf("             ");
    for (int j = 0; j < IRIS_CLASSES; j++) {
        printf(" %10s", CLASS_NAMES[j]);
    }
    printf("\n");

    for (int c = 0; c < result.k; c++) {
        printf("Cluster %-4d", c);
        for (int j = 0; j < IRIS_CLASSES; j++) {
            int count = 0;
            for (size_t i = 0; i < dataset->count; i++) {
                if (result.assignments[i] == c &&
                    dataset->samples[i].true_label == j) {
                    count++;
                }
            }
            printf(" %10d", count);
        }
        printf("\n");
    }

    /* Elbow method demonstration */
    printf("\nElbow Method - Inertia vs K:\n");
    printf("─────────────────────────────\n");
    printf("%-5s %15s\n", "K", "Inertia");

    for (int k = 1; k <= 6; k++) {
        KMeansResult temp = kmeans_cluster(dataset, k, MAX_ITERATIONS);
        printf("%-5d %15.2f\n", k, temp.inertia);
        free_kmeans_result(&temp);
    }

    printf("\nNote: Look for the 'elbow' where inertia reduction slows.\n");
    printf("For Iris data, K=3 typically shows clear elbow.\n");

    free_kmeans_result(&result);
}

/* =============================================================================
 * PART 3: COMPARISON AND VISUALISATION
 * =============================================================================
 */

/**
 * ASCII scatter plot of clusters (first 2 features)
 */
void plot_clusters_ascii(const IrisDataset *dataset, const KMeansResult *result) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      CLUSTER VISUALISATION (Petal Length vs Petal Width)      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    const int WIDTH = 60;
    const int HEIGHT = 20;
    char plot[HEIGHT][WIDTH + 1];

    /* Initialise plot */
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            plot[y][x] = '.';
        }
        plot[y][WIDTH] = '\0';
    }

    /* Use petal length (feature 2) and petal width (feature 3) */
    double x_min = dataset->feature_min[2];
    double x_max = dataset->feature_max[2];
    double y_min = dataset->feature_min[3];
    double y_max = dataset->feature_max[3];

    /* Plot samples */
    char symbols[] = {'0', '1', '2', '3', '4'};

    for (size_t i = 0; i < dataset->count; i++) {
        double x_val = dataset->samples[i].features[2];
        double y_val = dataset->samples[i].features[3];

        int px = (int)((x_val - x_min) / (x_max - x_min) * (WIDTH - 1));
        int py = HEIGHT - 1 - (int)((y_val - y_min) / (y_max - y_min) * (HEIGHT - 1));

        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
            int cluster = result->assignments[i];
            if (cluster >= 0 && cluster < 5) {
                plot[py][px] = symbols[cluster];
            }
        }
    }

    /* Plot centroids */
    for (int c = 0; c < result->k && c < 5; c++) {
        double x_val = result->centroids[c][2];
        double y_val = result->centroids[c][3];

        int px = (int)((x_val - x_min) / (x_max - x_min) * (WIDTH - 1));
        int py = HEIGHT - 1 - (int)((y_val - y_min) / (y_max - y_min) * (HEIGHT - 1));

        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
            plot[py][px] = '*';  /* Centroid marker */
        }
    }

    /* Print plot */
    printf("Petal Width\n");
    printf("    ^\n");
    for (int y = 0; y < HEIGHT; y++) {
        if (y == 0) {
            printf("%4.1f|", y_max);
        } else if (y == HEIGHT - 1) {
            printf("%4.1f|", y_min);
        } else {
            printf("    |");
        }
        printf("%s\n", plot[y]);
    }
    printf("    +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("> Petal Length\n");
    printf("     %.1f", x_min);
    for (int x = 0; x < WIDTH - 12; x++) printf(" ");
    printf("%.1f\n\n", x_max);

    printf("Legend: 0,1,2 = cluster assignments, * = centroid\n");
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 18: MACHINE LEARNING FUNDAMENTALS IN C               ║\n");
    printf("║     Exercise 2: K-NN and K-Means on Iris Dataset              ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    /* Load or generate dataset */
    IrisDataset dataset = {0};

    const char *data_file = (argc > 1) ? argv[1] : "data/iris.csv";

    if (!load_iris_from_file(&dataset, data_file)) {
        generate_synthetic_iris(&dataset);
    }

    /* Compute statistics */
    compute_dataset_statistics(&dataset);
    print_dataset_statistics(&dataset);

    /* Part 1: K-NN Classification */
    demo_knn(&dataset);

    /* Part 2: K-Means Clustering */
    demo_kmeans(&dataset);

    /* Part 3: Visualisation */
    srand(42);
    KMeansResult viz_result = kmeans_cluster(&dataset, 3, MAX_ITERATIONS);
    plot_clusters_ascii(&dataset, &viz_result);
    free_kmeans_result(&viz_result);

    /* Summary */
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    COMPARISON SUMMARY                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    printf("K-NN (Supervised):\n");
    printf("  + Uses known labels for training\n");
    printf("  + Direct classification of new samples\n");
    printf("  + No training phase (lazy learning)\n");
    printf("  - Requires labelled data\n");
    printf("  - Slow prediction for large datasets\n\n");

    printf("K-Means (Unsupervised):\n");
    printf("  + Discovers structure without labels\n");
    printf("  + Fast clustering after training\n");
    printf("  + Finds natural groupings\n");
    printf("  - Requires choosing K\n");
    printf("  - Sensitive to initialisation\n\n");

    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    EXERCISE COMPLETE                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    /* Cleanup */
    free_dataset(&dataset);

    return 0;
}

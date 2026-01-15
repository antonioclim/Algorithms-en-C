/**
 * =============================================================================
 * WEEK 18: MACHINE LEARNING FUNDAMENTALS IN C
 * Exercise 2 Solution: K-NN and K-Means on Iris Dataset
 * =============================================================================
 *
 * COMPLETE SOLUTION - For Instructor Use Only
 *
 * This solution demonstrates:
 *   1. Iris dataset loading and preprocessing
 *   2. K-Nearest Neighbours classification
 *   3. K-Means clustering
 *   4. Comparison of supervised vs unsupervised approaches
 *   5. Model evaluation with confusion matrix
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c -lm
 * Usage: ./exercise2_sol
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
 * CONSTANTS AND CONFIGURATION
 * =============================================================================
 */

#define MAX_SAMPLES 200
#define NUM_FEATURES 4          /* sepal_length, sepal_width, petal_length, petal_width */
#define NUM_CLASSES 3           /* setosa, versicolor, virginica */
#define K_NEIGHBOURS 5          /* For K-NN */
#define K_CLUSTERS 3            /* For K-Means */
#define MAX_KMEANS_ITER 100
#define TRAIN_RATIO 0.8

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

typedef struct {
    double features[NUM_FEATURES];
    int label;                  /* 0=setosa, 1=versicolor, 2=virginica */
} IrisSample;

typedef struct {
    double mean[NUM_FEATURES];
    double std_dev[NUM_FEATURES];
    bool fitted;
} Normaliser;

typedef struct {
    int k;
    IrisSample *train_data;
    int n_train;
} KNNClassifier;

typedef struct {
    double centroids[K_CLUSTERS][NUM_FEATURES];
    int assignments[MAX_SAMPLES];
    int k;
    int n_samples;
    int iterations;
} KMeansModel;

typedef struct {
    int matrix[NUM_CLASSES][NUM_CLASSES];  /* Rows: actual, Cols: predicted */
    double accuracy;
    double precision[NUM_CLASSES];
    double recall[NUM_CLASSES];
} ConfusionMatrix;

/* Class names for output */
static const char *CLASS_NAMES[] = {"setosa", "versicolor", "virginica"};

/* =============================================================================
 * PART 1: DATA LOADING
 * =============================================================================
 */

/**
 * Generates synthetic Iris-like dataset based on known distributions
 */
static void generate_iris_data(IrisSample *samples, int *n_samples) {
    printf("Generating synthetic Iris dataset...\n\n");
    
    /* Approximate means and std devs for each class and feature */
    /* Format: [class][feature] */
    const double means[3][4] = {
        {5.0, 3.4, 1.5, 0.2},  /* setosa */
        {5.9, 2.8, 4.3, 1.3},  /* versicolor */
        {6.6, 3.0, 5.6, 2.0}   /* virginica */
    };
    
    const double stds[3][4] = {
        {0.35, 0.38, 0.17, 0.10},
        {0.52, 0.31, 0.47, 0.20},
        {0.64, 0.32, 0.55, 0.27}
    };
    
    int idx = 0;
    for (int cls = 0; cls < NUM_CLASSES; cls++) {
        for (int i = 0; i < 50; i++) {
            for (int f = 0; f < NUM_FEATURES; f++) {
                /* Box-Muller for normal distribution */
                double u1 = (double)rand() / RAND_MAX + 1e-10;
                double u2 = (double)rand() / RAND_MAX;
                double z = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
                samples[idx].features[f] = means[cls][f] + stds[cls][f] * z;
            }
            samples[idx].label = cls;
            idx++;
        }
    }
    
    *n_samples = idx;
    printf("Generated %d samples (50 per class)\n", *n_samples);
}

/**
 * Loads Iris data from CSV file
 *
 * Expected format: sepal_length,sepal_width,petal_length,petal_width,species
 */
static int load_iris_csv(const char *filename, IrisSample *samples, int max_samples) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Warning: Could not open %s, using synthetic data\n", filename);
        return -1;
    }
    
    char line[256];
    int count = 0;
    
    /* Skip header */
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return -1;
    }
    
    while (fgets(line, sizeof(line), fp) && count < max_samples) {
        double sl, sw, pl, pw;
        char species[32];
        
        if (sscanf(line, "%lf,%lf,%lf,%lf,%31s", &sl, &sw, &pl, &pw, species) == 5) {
            samples[count].features[0] = sl;
            samples[count].features[1] = sw;
            samples[count].features[2] = pl;
            samples[count].features[3] = pw;
            
            /* Parse species name */
            if (strstr(species, "setosa")) {
                samples[count].label = 0;
            } else if (strstr(species, "versicolor")) {
                samples[count].label = 1;
            } else {
                samples[count].label = 2;  /* virginica */
            }
            count++;
        }
    }
    
    fclose(fp);
    printf("Loaded %d samples from %s\n", count, filename);
    return count;
}

/* =============================================================================
 * PART 2: DATA PREPROCESSING
 * =============================================================================
 */

/**
 * Fisher-Yates shuffle
 */
static void shuffle_samples(IrisSample *samples, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        IrisSample temp = samples[i];
        samples[i] = samples[j];
        samples[j] = temp;
    }
}

/**
 * Train/test split
 */
static void train_test_split(IrisSample *all, int n,
                             IrisSample *train, IrisSample *test,
                             int *n_train, int *n_test) {
    shuffle_samples(all, n);
    
    *n_train = (int)(n * TRAIN_RATIO);
    *n_test = n - *n_train;
    
    for (int i = 0; i < *n_train; i++) {
        train[i] = all[i];
    }
    for (int i = 0; i < *n_test; i++) {
        test[i] = all[*n_train + i];
    }
    
    printf("Split: %d train, %d test\n", *n_train, *n_test);
}

/**
 * Z-score normalisation
 */
static void normaliser_fit(Normaliser *norm, IrisSample *samples, int n) {
    for (int f = 0; f < NUM_FEATURES; f++) {
        norm->mean[f] = 0.0;
        for (int i = 0; i < n; i++) {
            norm->mean[f] += samples[i].features[f];
        }
        norm->mean[f] /= n;
        
        norm->std_dev[f] = 0.0;
        for (int i = 0; i < n; i++) {
            double diff = samples[i].features[f] - norm->mean[f];
            norm->std_dev[f] += diff * diff;
        }
        norm->std_dev[f] = sqrt(norm->std_dev[f] / n);
        if (norm->std_dev[f] < 1e-10) norm->std_dev[f] = 1.0;
    }
    norm->fitted = true;
}

static void normaliser_transform(Normaliser *norm, IrisSample *samples, int n) {
    for (int i = 0; i < n; i++) {
        for (int f = 0; f < NUM_FEATURES; f++) {
            samples[i].features[f] = 
                (samples[i].features[f] - norm->mean[f]) / norm->std_dev[f];
        }
    }
}

/* =============================================================================
 * PART 3: K-NEAREST NEIGHBOURS
 * =============================================================================
 */

/**
 * Euclidean distance between two feature vectors
 */
static double euclidean_distance(const double *a, const double *b) {
    double sum = 0.0;
    for (int f = 0; f < NUM_FEATURES; f++) {
        double diff = a[f] - b[f];
        sum += diff * diff;
    }
    return sqrt(sum);
}

/**
 * Manhattan distance between two feature vectors
 */
static double manhattan_distance(const double *a, const double *b) {
    double sum = 0.0;
    for (int f = 0; f < NUM_FEATURES; f++) {
        sum += fabs(a[f] - b[f]);
    }
    return sum;
}

/**
 * Structure for distance-label pairs (for sorting)
 */
typedef struct {
    double distance;
    int label;
} DistanceLabel;

/**
 * Comparison function for qsort
 */
static int compare_distance(const void *a, const void *b) {
    double diff = ((DistanceLabel *)a)->distance - ((DistanceLabel *)b)->distance;
    return (diff > 0) - (diff < 0);
}

/**
 * Initialises K-NN classifier
 */
static void knn_init(KNNClassifier *knn, IrisSample *train_data, int n_train, int k) {
    knn->k = k;
    knn->train_data = train_data;
    knn->n_train = n_train;
}

/**
 * Predicts class for a single sample using K-NN
 */
static int knn_predict(KNNClassifier *knn, const double *features) {
    DistanceLabel *distances = malloc(knn->n_train * sizeof(DistanceLabel));
    
    /* Compute distances to all training samples */
    for (int i = 0; i < knn->n_train; i++) {
        distances[i].distance = euclidean_distance(features, knn->train_data[i].features);
        distances[i].label = knn->train_data[i].label;
    }
    
    /* Sort by distance */
    qsort(distances, knn->n_train, sizeof(DistanceLabel), compare_distance);
    
    /* Count votes from K nearest neighbours */
    int votes[NUM_CLASSES] = {0};
    for (int i = 0; i < knn->k && i < knn->n_train; i++) {
        votes[distances[i].label]++;
    }
    
    /* Find majority class */
    int max_votes = 0;
    int prediction = 0;
    for (int c = 0; c < NUM_CLASSES; c++) {
        if (votes[c] > max_votes) {
            max_votes = votes[c];
            prediction = c;
        }
    }
    
    free(distances);
    return prediction;
}

/**
 * Evaluates K-NN on test set
 */
static void knn_evaluate(KNNClassifier *knn, IrisSample *test, int n_test,
                         ConfusionMatrix *cm) {
    memset(cm->matrix, 0, sizeof(cm->matrix));
    
    for (int i = 0; i < n_test; i++) {
        int pred = knn_predict(knn, test[i].features);
        int actual = test[i].label;
        cm->matrix[actual][pred]++;
    }
    
    /* Compute accuracy */
    int correct = 0;
    for (int c = 0; c < NUM_CLASSES; c++) {
        correct += cm->matrix[c][c];
    }
    cm->accuracy = (double)correct / n_test;
    
    /* Compute precision and recall */
    for (int c = 0; c < NUM_CLASSES; c++) {
        int tp = cm->matrix[c][c];
        int pred_total = 0, actual_total = 0;
        
        for (int i = 0; i < NUM_CLASSES; i++) {
            pred_total += cm->matrix[i][c];   /* Column sum */
            actual_total += cm->matrix[c][i]; /* Row sum */
        }
        
        cm->precision[c] = (pred_total > 0) ? (double)tp / pred_total : 0.0;
        cm->recall[c] = (actual_total > 0) ? (double)tp / actual_total : 0.0;
    }
}

/* =============================================================================
 * PART 4: K-MEANS CLUSTERING
 * =============================================================================
 */

/**
 * K-Means++ initialisation
 */
static void kmeans_plus_plus_init(KMeansModel *km, IrisSample *samples, int n) {
    double *min_dist = malloc(n * sizeof(double));
    
    /* Choose first centroid randomly */
    int first = rand() % n;
    for (int f = 0; f < NUM_FEATURES; f++) {
        km->centroids[0][f] = samples[first].features[f];
    }
    
    /* Initialise distances */
    for (int i = 0; i < n; i++) {
        min_dist[i] = euclidean_distance(samples[i].features, km->centroids[0]);
    }
    
    /* Choose remaining centroids */
    for (int c = 1; c < km->k; c++) {
        /* Compute cumulative distribution */
        double total = 0.0;
        for (int i = 0; i < n; i++) {
            total += min_dist[i] * min_dist[i];
        }
        
        /* Sample proportional to D²(x) */
        double r = ((double)rand() / RAND_MAX) * total;
        double cumsum = 0.0;
        int chosen = 0;
        
        for (int i = 0; i < n; i++) {
            cumsum += min_dist[i] * min_dist[i];
            if (cumsum >= r) {
                chosen = i;
                break;
            }
        }
        
        /* Set new centroid */
        for (int f = 0; f < NUM_FEATURES; f++) {
            km->centroids[c][f] = samples[chosen].features[f];
        }
        
        /* Update minimum distances */
        for (int i = 0; i < n; i++) {
            double d = euclidean_distance(samples[i].features, km->centroids[c]);
            if (d < min_dist[i]) {
                min_dist[i] = d;
            }
        }
    }
    
    free(min_dist);
}

/**
 * Initialises K-Means model
 */
static void kmeans_init(KMeansModel *km, int k, IrisSample *samples, int n) {
    km->k = k;
    km->n_samples = n;
    km->iterations = 0;
    
    /* Use K-Means++ initialisation */
    kmeans_plus_plus_init(km, samples, n);
    
    /* Initial assignment */
    for (int i = 0; i < n; i++) {
        km->assignments[i] = -1;
    }
}

/**
 * Assignment step: assign each sample to nearest centroid
 *
 * @return Number of samples that changed assignment
 */
static int kmeans_assign(KMeansModel *km, IrisSample *samples) {
    int changes = 0;
    
    for (int i = 0; i < km->n_samples; i++) {
        double min_dist = DBL_MAX;
        int best_cluster = 0;
        
        for (int c = 0; c < km->k; c++) {
            double d = euclidean_distance(samples[i].features, km->centroids[c]);
            if (d < min_dist) {
                min_dist = d;
                best_cluster = c;
            }
        }
        
        if (km->assignments[i] != best_cluster) {
            km->assignments[i] = best_cluster;
            changes++;
        }
    }
    
    return changes;
}

/**
 * Update step: recompute centroids as mean of assigned samples
 */
static void kmeans_update(KMeansModel *km, IrisSample *samples) {
    /* Reset centroids */
    double sums[K_CLUSTERS][NUM_FEATURES] = {{0}};
    int counts[K_CLUSTERS] = {0};
    
    /* Sum features per cluster */
    for (int i = 0; i < km->n_samples; i++) {
        int c = km->assignments[i];
        counts[c]++;
        for (int f = 0; f < NUM_FEATURES; f++) {
            sums[c][f] += samples[i].features[f];
        }
    }
    
    /* Compute means */
    for (int c = 0; c < km->k; c++) {
        if (counts[c] > 0) {
            for (int f = 0; f < NUM_FEATURES; f++) {
                km->centroids[c][f] = sums[c][f] / counts[c];
            }
        }
    }
}

/**
 * Computes Within-Cluster Sum of Squares (inertia)
 */
static double kmeans_inertia(KMeansModel *km, IrisSample *samples) {
    double inertia = 0.0;
    
    for (int i = 0; i < km->n_samples; i++) {
        int c = km->assignments[i];
        double d = euclidean_distance(samples[i].features, km->centroids[c]);
        inertia += d * d;
    }
    
    return inertia;
}

/**
 * Runs Lloyd's algorithm
 */
static void kmeans_fit(KMeansModel *km, IrisSample *samples) {
    printf("Running K-Means clustering (k=%d)...\n\n", km->k);
    
    for (int iter = 0; iter < MAX_KMEANS_ITER; iter++) {
        /* Assignment step */
        int changes = kmeans_assign(km, samples);
        
        /* Update step */
        kmeans_update(km, samples);
        
        km->iterations = iter + 1;
        double inertia = kmeans_inertia(km, samples);
        
        if (iter % 10 == 0 || changes == 0) {
            printf("  Iteration %3d: Inertia = %.4f, Changes = %d\n",
                   iter, inertia, changes);
        }
        
        /* Check convergence */
        if (changes == 0) {
            printf("\n✓ Converged at iteration %d\n", iter);
            break;
        }
    }
}

/**
 * Maps cluster labels to class labels using majority voting
 */
static void kmeans_map_labels(KMeansModel *km, IrisSample *samples, int *cluster_to_class) {
    /* Count actual labels per cluster */
    int counts[K_CLUSTERS][NUM_CLASSES] = {{0}};
    
    for (int i = 0; i < km->n_samples; i++) {
        int c = km->assignments[i];
        int label = samples[i].label;
        counts[c][label]++;
    }
    
    /* Assign each cluster to majority class */
    for (int c = 0; c < km->k; c++) {
        int max_count = 0;
        cluster_to_class[c] = 0;
        
        for (int l = 0; l < NUM_CLASSES; l++) {
            if (counts[c][l] > max_count) {
                max_count = counts[c][l];
                cluster_to_class[c] = l;
            }
        }
    }
}

/**
 * Evaluates K-Means clustering quality
 */
static void kmeans_evaluate(KMeansModel *km, IrisSample *samples, ConfusionMatrix *cm) {
    int cluster_to_class[K_CLUSTERS];
    kmeans_map_labels(km, samples, cluster_to_class);
    
    memset(cm->matrix, 0, sizeof(cm->matrix));
    
    for (int i = 0; i < km->n_samples; i++) {
        int pred = cluster_to_class[km->assignments[i]];
        int actual = samples[i].label;
        cm->matrix[actual][pred]++;
    }
    
    /* Compute accuracy */
    int correct = 0;
    for (int c = 0; c < NUM_CLASSES; c++) {
        correct += cm->matrix[c][c];
    }
    cm->accuracy = (double)correct / km->n_samples;
    
    /* Compute precision and recall */
    for (int c = 0; c < NUM_CLASSES; c++) {
        int tp = cm->matrix[c][c];
        int pred_total = 0, actual_total = 0;
        
        for (int i = 0; i < NUM_CLASSES; i++) {
            pred_total += cm->matrix[i][c];
            actual_total += cm->matrix[c][i];
        }
        
        cm->precision[c] = (pred_total > 0) ? (double)tp / pred_total : 0.0;
        cm->recall[c] = (actual_total > 0) ? (double)tp / actual_total : 0.0;
    }
}

/* =============================================================================
 * PART 5: VISUALISATION AND REPORTING
 * =============================================================================
 */

/**
 * Prints confusion matrix
 */
static void print_confusion_matrix(const char *title, ConfusionMatrix *cm) {
    printf("\n%s:\n", title);
    printf("┌────────────┬──────────┬──────────┬──────────┐\n");
    printf("│ Actual\\Pred│ setosa   │ versicol │ virginic │\n");
    printf("├────────────┼──────────┼──────────┼──────────┤\n");
    
    for (int i = 0; i < NUM_CLASSES; i++) {
        printf("│ %-10s │", CLASS_NAMES[i]);
        for (int j = 0; j < NUM_CLASSES; j++) {
            printf(" %8d │", cm->matrix[i][j]);
        }
        printf("\n");
    }
    printf("└────────────┴──────────┴──────────┴──────────┘\n");
    
    printf("\nAccuracy: %.2f%%\n", cm->accuracy * 100);
    
    printf("\nPer-class metrics:\n");
    printf("┌────────────┬────────────┬────────────┐\n");
    printf("│ Class      │ Precision  │ Recall     │\n");
    printf("├────────────┼────────────┼────────────┤\n");
    for (int c = 0; c < NUM_CLASSES; c++) {
        printf("│ %-10s │ %10.2f │ %10.2f │\n",
               CLASS_NAMES[c], cm->precision[c], cm->recall[c]);
    }
    printf("└────────────┴────────────┴────────────┘\n");
}

/**
 * Prints K-Means cluster centroids
 */
static void print_centroids(KMeansModel *km, Normaliser *norm) {
    printf("\nCluster centroids (original scale):\n");
    printf("┌─────────┬────────────┬────────────┬────────────┬────────────┐\n");
    printf("│ Cluster │ sepal_len  │ sepal_wid  │ petal_len  │ petal_wid  │\n");
    printf("├─────────┼────────────┼────────────┼────────────┼────────────┤\n");
    
    for (int c = 0; c < km->k; c++) {
        printf("│    %d    │", c);
        for (int f = 0; f < NUM_FEATURES; f++) {
            double val = km->centroids[c][f] * norm->std_dev[f] + norm->mean[f];
            printf(" %10.2f │", val);
        }
        printf("\n");
    }
    printf("└─────────┴────────────┴────────────┴────────────┴────────────┘\n");
}

/**
 * ASCII scatter plot of first two features
 */
static void plot_scatter(IrisSample *samples, int n, KMeansModel *km) {
    printf("\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║    SCATTER PLOT (Sepal Length vs Petal Length)                 ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
    
    #define PLOT_W 50
    #define PLOT_H 20
    
    char plot[PLOT_H][PLOT_W + 1];
    
    /* Find ranges */
    double x_min = DBL_MAX, x_max = -DBL_MAX;
    double y_min = DBL_MAX, y_max = -DBL_MAX;
    
    for (int i = 0; i < n; i++) {
        double x = samples[i].features[0];  /* sepal length */
        double y = samples[i].features[2];  /* petal length */
        if (x < x_min) x_min = x;
        if (x > x_max) x_max = x;
        if (y < y_min) y_min = y;
        if (y > y_max) y_max = y;
    }
    
    /* Initialise plot */
    for (int row = 0; row < PLOT_H; row++) {
        for (int col = 0; col < PLOT_W; col++) {
            plot[row][col] = '.';
        }
        plot[row][PLOT_W] = '\0';
    }
    
    /* Plot points */
    char symbols[] = {'0', '1', '2'};  /* Cluster symbols */
    
    for (int i = 0; i < n; i++) {
        double x = samples[i].features[0];
        double y = samples[i].features[2];
        
        int col = (int)((x - x_min) / (x_max - x_min) * (PLOT_W - 1));
        int row = (int)((y_max - y) / (y_max - y_min) * (PLOT_H - 1));
        
        if (col >= 0 && col < PLOT_W && row >= 0 && row < PLOT_H) {
            if (km != NULL) {
                plot[row][col] = symbols[km->assignments[i]];
            } else {
                plot[row][col] = symbols[samples[i].label];
            }
        }
    }
    
    /* Print plot */
    printf("Petal Length\n");
    for (int row = 0; row < PLOT_H; row++) {
        printf("  │%s\n", plot[row]);
    }
    printf("  └");
    for (int col = 0; col < PLOT_W; col++) printf("─");
    printf("\n                    Sepal Length\n\n");
    printf("  Legend: 0=Cluster 0/Setosa, 1=Cluster 1/Versicolor, 2=Cluster 2/Virginica\n");
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
    printf("║     Exercise 2 Solution: K-NN and K-Means on Iris             ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    srand(42);
    
    /* Allocate memory */
    IrisSample *all_samples = malloc(MAX_SAMPLES * sizeof(IrisSample));
    IrisSample *train_samples = malloc(MAX_SAMPLES * sizeof(IrisSample));
    IrisSample *test_samples = malloc(MAX_SAMPLES * sizeof(IrisSample));
    
    if (!all_samples || !train_samples || !test_samples) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: DATA LOADING                                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int n_samples;
    n_samples = load_iris_csv("../data/iris.csv", all_samples, MAX_SAMPLES);
    if (n_samples < 0) {
        generate_iris_data(all_samples, &n_samples);
    }
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: PREPROCESSING                                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int n_train, n_test;
    train_test_split(all_samples, n_samples, train_samples, test_samples,
                     &n_train, &n_test);
    
    Normaliser norm = {0};
    normaliser_fit(&norm, train_samples, n_train);
    normaliser_transform(&norm, train_samples, n_train);
    normaliser_transform(&norm, test_samples, n_test);
    
    printf("✓ Data normalised\n");
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: K-NEAREST NEIGHBOURS                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Training K-NN classifier (k=%d)...\n", K_NEIGHBOURS);
    printf("Note: K-NN is a lazy learner - no explicit training required.\n\n");
    
    KNNClassifier knn;
    knn_init(&knn, train_samples, n_train, K_NEIGHBOURS);
    
    ConfusionMatrix knn_cm;
    knn_evaluate(&knn, test_samples, n_test, &knn_cm);
    
    print_confusion_matrix("K-NN Confusion Matrix", &knn_cm);
    
    /* Test different K values */
    printf("\n\nK-NN accuracy for different K values:\n");
    printf("┌───────┬────────────┐\n");
    printf("│ K     │ Accuracy   │\n");
    printf("├───────┼────────────┤\n");
    
    for (int k = 1; k <= 11; k += 2) {
        KNNClassifier temp_knn;
        knn_init(&temp_knn, train_samples, n_train, k);
        ConfusionMatrix temp_cm;
        knn_evaluate(&temp_knn, test_samples, n_test, &temp_cm);
        printf("│ %5d │ %9.2f%% │\n", k, temp_cm.accuracy * 100);
    }
    printf("└───────┴────────────┘\n");
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: K-MEANS CLUSTERING                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Need a copy for K-Means (uses all data) */
    IrisSample *kmeans_data = malloc(n_samples * sizeof(IrisSample));
    for (int i = 0; i < n_train; i++) {
        kmeans_data[i] = train_samples[i];
    }
    for (int i = 0; i < n_test; i++) {
        kmeans_data[n_train + i] = test_samples[i];
    }
    
    KMeansModel km;
    kmeans_init(&km, K_CLUSTERS, kmeans_data, n_samples);
    kmeans_fit(&km, kmeans_data);
    
    print_centroids(&km, &norm);
    
    ConfusionMatrix km_cm;
    kmeans_evaluate(&km, kmeans_data, &km_cm);
    print_confusion_matrix("K-Means Confusion Matrix (after label mapping)", &km_cm);
    
    /* Elbow analysis */
    printf("\n\nElbow analysis (Inertia vs K):\n");
    printf("┌───────┬────────────────┐\n");
    printf("│ K     │ Inertia        │\n");
    printf("├───────┼────────────────┤\n");
    
    for (int k = 1; k <= 6; k++) {
        KMeansModel temp_km;
        kmeans_init(&temp_km, k, kmeans_data, n_samples);
        kmeans_fit(&temp_km, kmeans_data);
        double inertia = kmeans_inertia(&temp_km, kmeans_data);
        printf("│ %5d │ %14.4f │\n", k, inertia);
    }
    printf("└───────┴────────────────┘\n");
    printf("\nNote: Look for the 'elbow' where inertia decrease slows down.\n");
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: COMPARISON                                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("┌────────────────────┬────────────────────┬────────────────────┐\n");
    printf("│ Aspect             │ K-NN               │ K-Means            │\n");
    printf("├────────────────────┼────────────────────┼────────────────────┤\n");
    printf("│ Type               │ Supervised         │ Unsupervised       │\n");
    printf("│ Uses labels?       │ Yes (training)     │ No                 │\n");
    printf("│ Accuracy           │ %17.2f%% │ %17.2f%% │\n",
           knn_cm.accuracy * 100, km_cm.accuracy * 100);
    printf("│ Training time      │ O(1)               │ O(n·k·iter)        │\n");
    printf("│ Prediction time    │ O(n·d)             │ O(k·d)             │\n");
    printf("└────────────────────┴────────────────────┴────────────────────┘\n");
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      VISUALISATION                                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    plot_scatter(kmeans_data, n_samples, &km);
    
    /* Clean up */
    free(all_samples);
    free(train_samples);
    free(test_samples);
    free(kmeans_data);
    
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    EXERCISE 2 COMPLETE                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

#!/usr/bin/env python3
"""
=============================================================================
Week 18: Machine Learning Fundamentals
Python Comparison: K-Means Clustering

This file provides a Python/scikit-learn reference implementation for
comparison with the C implementation. NOT for execution during the course.
=============================================================================

Purpose:
    - Validate C implementation results
    - Demonstrate equivalent algorithms
    - Highlight syntax differences between C and Python
    - Show scikit-learn's API for professional comparison

Usage (for validation only):
    python3 kmeans_python.py

Requirements:
    pip install numpy scikit-learn matplotlib

=============================================================================
"""

import numpy as np
from typing import List, Tuple, Optional
import time


# =============================================================================
# PART 1: MANUAL K-MEANS IMPLEMENTATION (matches C logic)
# =============================================================================

class KMeansManual:
    """
    K-Means clustering from scratch.
    
    This implementation mirrors the C version for direct comparison,
    using NumPy for efficiency whilst maintaining algorithmic clarity.
    
    Attributes:
        k: Number of clusters
        max_iterations: Maximum number of Lloyd iterations
        tolerance: Convergence threshold for centroid movement
        centroids: Final cluster centroids (k × d array)
        labels: Cluster assignment for each sample
        inertia: Within-cluster sum of squares (WCSS)
    """
    
    def __init__(self, k: int = 3, max_iterations: int = 100, 
                 tolerance: float = 1e-4, random_state: Optional[int] = None):
        """
        Initialise K-Means parameters.
        
        Args:
            k: Number of clusters
            max_iterations: Maximum iterations for convergence
            tolerance: Minimum centroid movement to continue
            random_state: Seed for reproducibility
        """
        self.k = k
        self.max_iterations = max_iterations
        self.tolerance = tolerance
        self.random_state = random_state
        self.centroids = None
        self.labels = None
        self.inertia = None
        self.n_iterations = 0
    
    def _euclidean_distance(self, a: np.ndarray, b: np.ndarray) -> float:
        """
        Compute Euclidean distance between two points.
        
        Equivalent C function:
            double euclidean_distance(double *a, double *b, int dim) {
                double sum = 0.0;
                for (int i = 0; i < dim; i++) {
                    double diff = a[i] - b[i];
                    sum += diff * diff;
                }
                return sqrt(sum);
            }
        """
        return np.sqrt(np.sum((a - b) ** 2))
    
    def _init_random(self, X: np.ndarray) -> np.ndarray:
        """
        Random initialisation: select k random samples as centroids.
        
        Equivalent C:
            for (int i = 0; i < k; i++) {
                int idx = rand() % n;
                memcpy(centroids[i], data[idx], dim * sizeof(double));
            }
        """
        np.random.seed(self.random_state)
        indices = np.random.choice(X.shape[0], self.k, replace=False)
        return X[indices].copy()
    
    def _init_kmeans_plusplus(self, X: np.ndarray) -> np.ndarray:
        """
        K-Means++ initialisation for better convergence.
        
        Algorithm:
            1. Choose first centroid uniformly at random
            2. For each subsequent centroid:
               a. Compute D(x)² = distance to nearest existing centroid
               b. Choose new centroid with probability ∝ D(x)²
            3. Repeat until k centroids selected
        
        This dramatically improves convergence speed and quality.
        
        Equivalent C (simplified):
            // First centroid: random
            int idx = rand() % n;
            memcpy(centroids[0], data[idx], dim * sizeof(double));
            
            // Subsequent centroids: proportional to D²
            for (int c = 1; c < k; c++) {
                double *distances = malloc(n * sizeof(double));
                double total = 0.0;
                
                for (int i = 0; i < n; i++) {
                    double min_dist = INFINITY;
                    for (int j = 0; j < c; j++) {
                        double d = euclidean_distance(data[i], centroids[j], dim);
                        if (d < min_dist) min_dist = d;
                    }
                    distances[i] = min_dist * min_dist;
                    total += distances[i];
                }
                
                // Roulette wheel selection
                double r = ((double)rand() / RAND_MAX) * total;
                double cumulative = 0.0;
                for (int i = 0; i < n; i++) {
                    cumulative += distances[i];
                    if (cumulative >= r) {
                        memcpy(centroids[c], data[i], dim * sizeof(double));
                        break;
                    }
                }
                free(distances);
            }
        """
        np.random.seed(self.random_state)
        n_samples, n_features = X.shape
        centroids = np.zeros((self.k, n_features))
        
        # First centroid: random sample
        idx = np.random.randint(n_samples)
        centroids[0] = X[idx].copy()
        
        # Subsequent centroids: proportional to D²
        for c in range(1, self.k):
            # Compute squared distances to nearest centroid
            distances_sq = np.zeros(n_samples)
            for i in range(n_samples):
                min_dist = float('inf')
                for j in range(c):
                    dist = self._euclidean_distance(X[i], centroids[j])
                    if dist < min_dist:
                        min_dist = dist
                distances_sq[i] = min_dist ** 2
            
            # Normalise to probabilities
            probabilities = distances_sq / distances_sq.sum()
            
            # Sample new centroid
            idx = np.random.choice(n_samples, p=probabilities)
            centroids[c] = X[idx].copy()
        
        return centroids
    
    def _assign_clusters(self, X: np.ndarray) -> np.ndarray:
        """
        Assign each sample to nearest centroid.
        
        Equivalent C:
            for (int i = 0; i < n; i++) {
                double min_dist = INFINITY;
                int best_cluster = 0;
                
                for (int c = 0; c < k; c++) {
                    double d = euclidean_distance(data[i], centroids[c], dim);
                    if (d < min_dist) {
                        min_dist = d;
                        best_cluster = c;
                    }
                }
                assignments[i] = best_cluster;
            }
        """
        labels = np.zeros(X.shape[0], dtype=int)
        
        for i in range(X.shape[0]):
            min_dist = float('inf')
            best_cluster = 0
            
            for c in range(self.k):
                dist = self._euclidean_distance(X[i], self.centroids[c])
                if dist < min_dist:
                    min_dist = dist
                    best_cluster = c
            
            labels[i] = best_cluster
        
        return labels
    
    def _update_centroids(self, X: np.ndarray, labels: np.ndarray) -> np.ndarray:
        """
        Update centroids as mean of assigned samples.
        
        Equivalent C:
            for (int c = 0; c < k; c++) {
                // Reset centroid
                memset(new_centroids[c], 0, dim * sizeof(double));
                int count = 0;
                
                // Sum all samples assigned to this cluster
                for (int i = 0; i < n; i++) {
                    if (assignments[i] == c) {
                        for (int d = 0; d < dim; d++) {
                            new_centroids[c][d] += data[i][d];
                        }
                        count++;
                    }
                }
                
                // Divide by count (handle empty clusters)
                if (count > 0) {
                    for (int d = 0; d < dim; d++) {
                        new_centroids[c][d] /= count;
                    }
                } else {
                    // Reinitialise empty cluster
                    int idx = rand() % n;
                    memcpy(new_centroids[c], data[idx], dim * sizeof(double));
                }
            }
        """
        new_centroids = np.zeros_like(self.centroids)
        
        for c in range(self.k):
            cluster_mask = labels == c
            if np.sum(cluster_mask) > 0:
                new_centroids[c] = X[cluster_mask].mean(axis=0)
            else:
                # Reinitialise empty cluster with random sample
                idx = np.random.randint(X.shape[0])
                new_centroids[c] = X[idx].copy()
        
        return new_centroids
    
    def _compute_inertia(self, X: np.ndarray, labels: np.ndarray) -> float:
        """
        Compute within-cluster sum of squares (WCSS).
        
        Inertia = Σ_clusters Σ_samples ||x - centroid||²
        
        This is the objective function K-Means minimises.
        
        Equivalent C:
            double inertia = 0.0;
            for (int i = 0; i < n; i++) {
                int c = assignments[i];
                double d = euclidean_distance(data[i], centroids[c], dim);
                inertia += d * d;
            }
            return inertia;
        """
        inertia = 0.0
        for i in range(X.shape[0]):
            c = labels[i]
            dist = self._euclidean_distance(X[i], self.centroids[c])
            inertia += dist ** 2
        return inertia
    
    def fit(self, X: np.ndarray, init: str = 'kmeans++') -> 'KMeansManual':
        """
        Fit K-Means to data using Lloyd's algorithm.
        
        Lloyd's Algorithm:
            1. Initialise centroids (random or k-means++)
            2. Repeat until convergence:
               a. Assignment: assign each point to nearest centroid
               b. Update: recompute centroids as cluster means
            3. Convergence: centroids don't move significantly
        
        Args:
            X: Training data (n_samples × n_features)
            init: Initialisation method ('random' or 'kmeans++')
        
        Returns:
            self: Fitted model
        """
        # Initialisation
        if init == 'kmeans++':
            self.centroids = self._init_kmeans_plusplus(X)
        else:
            self.centroids = self._init_random(X)
        
        print(f"K-Means Training (k={self.k}, init={init})")
        print("=" * 50)
        
        # Lloyd's iterations
        for iteration in range(self.max_iterations):
            # Assignment step
            self.labels = self._assign_clusters(X)
            
            # Update step
            new_centroids = self._update_centroids(X, self.labels)
            
            # Check convergence
            centroid_shift = 0.0
            for c in range(self.k):
                shift = self._euclidean_distance(self.centroids[c], new_centroids[c])
                centroid_shift = max(centroid_shift, shift)
            
            self.centroids = new_centroids
            self.inertia = self._compute_inertia(X, self.labels)
            
            if (iteration + 1) % 10 == 0 or iteration == 0:
                print(f"  Iteration {iteration + 1:3d}: Inertia = {self.inertia:.4f}, "
                      f"Max shift = {centroid_shift:.6f}")
            
            if centroid_shift < self.tolerance:
                print(f"\n  Converged at iteration {iteration + 1}")
                break
        
        self.n_iterations = iteration + 1
        print(f"\nFinal Inertia: {self.inertia:.4f}")
        
        return self
    
    def predict(self, X: np.ndarray) -> np.ndarray:
        """
        Predict cluster labels for new samples.
        """
        return self._assign_clusters(X)
    
    def fit_predict(self, X: np.ndarray, init: str = 'kmeans++') -> np.ndarray:
        """
        Fit model and return cluster labels.
        """
        self.fit(X, init)
        return self.labels


# =============================================================================
# PART 2: ELBOW METHOD FOR K SELECTION
# =============================================================================

def elbow_analysis(X: np.ndarray, k_range: range = range(1, 11),
                   random_state: int = 42) -> Tuple[List[int], List[float]]:
    """
    Perform elbow analysis to select optimal k.
    
    The elbow method plots inertia vs k and looks for the "elbow"
    where adding more clusters yields diminishing returns.
    
    Equivalent C:
        for (int k = 1; k <= max_k; k++) {
            kmeans_fit(data, n, dim, k);
            inertias[k-1] = kmeans_inertia();
            printf("k=%d: Inertia=%.4f\n", k, inertias[k-1]);
        }
    
    Args:
        X: Training data
        k_range: Range of k values to test
        random_state: Random seed for reproducibility
    
    Returns:
        k_values: List of k values tested
        inertias: Corresponding inertia values
    """
    print("\n" + "=" * 60)
    print("ELBOW ANALYSIS")
    print("=" * 60)
    
    k_values = list(k_range)
    inertias = []
    
    for k in k_values:
        kmeans = KMeansManual(k=k, max_iterations=100, random_state=random_state)
        kmeans.fit(X, init='kmeans++')
        inertias.append(kmeans.inertia)
        print(f"k={k}: Inertia = {kmeans.inertia:.4f}")
    
    return k_values, inertias


def plot_elbow_ascii(k_values: List[int], inertias: List[float],
                     width: int = 60, height: int = 20):
    """
    Plot elbow curve using ASCII characters.
    
    This mirrors the ASCII plotting in the C implementation.
    """
    print("\n" + "=" * 60)
    print("ELBOW PLOT (ASCII)")
    print("=" * 60)
    
    min_inertia = min(inertias)
    max_inertia = max(inertias)
    inertia_range = max_inertia - min_inertia
    
    if inertia_range == 0:
        inertia_range = 1
    
    # Create plot grid
    grid = [[' ' for _ in range(width)] for _ in range(height)]
    
    # Plot points
    for i, (k, inertia) in enumerate(zip(k_values, inertias)):
        x = int((i / (len(k_values) - 1)) * (width - 1)) if len(k_values) > 1 else width // 2
        y = int((1 - (inertia - min_inertia) / inertia_range) * (height - 1))
        y = max(0, min(height - 1, y))
        x = max(0, min(width - 1, x))
        grid[y][x] = '*'
    
    # Draw axes and plot
    print(f"Inertia")
    print(f"  ^")
    print(f"{max_inertia:7.1f} |" + '-' * width)
    
    for row in grid:
        print("        |" + ''.join(row))
    
    print(f"{min_inertia:7.1f} |" + '-' * width + "> k")
    
    # X-axis labels
    k_labels = "        "
    for i, k in enumerate(k_values):
        pos = int((i / (len(k_values) - 1)) * width) if len(k_values) > 1 else width // 2
        k_labels = k_labels[:8+pos] + str(k) + k_labels[9+pos:]
    print(k_labels[:8+width])


# =============================================================================
# PART 3: CLUSTER QUALITY METRICS
# =============================================================================

def silhouette_coefficient(X: np.ndarray, labels: np.ndarray) -> float:
    """
    Compute silhouette coefficient for clustering quality.
    
    Silhouette coefficient measures how similar samples are to their own
    cluster compared to other clusters. Range: [-1, 1], higher is better.
    
    For each sample i:
        a(i) = mean distance to other samples in same cluster
        b(i) = mean distance to samples in nearest other cluster
        s(i) = (b(i) - a(i)) / max(a(i), b(i))
    
    Overall silhouette = mean of all s(i)
    
    Equivalent C (simplified):
        double total_silhouette = 0.0;
        for (int i = 0; i < n; i++) {
            double a = compute_mean_intra_cluster_distance(i);
            double b = compute_min_inter_cluster_distance(i);
            double s = (b - a) / fmax(a, b);
            total_silhouette += s;
        }
        return total_silhouette / n;
    """
    n = len(labels)
    unique_labels = np.unique(labels)
    
    if len(unique_labels) <= 1:
        return 0.0  # Need at least 2 clusters
    
    silhouettes = []
    
    for i in range(n):
        own_cluster = labels[i]
        
        # a(i): mean distance to samples in same cluster
        same_cluster_mask = labels == own_cluster
        same_cluster_mask[i] = False  # Exclude self
        
        if np.sum(same_cluster_mask) == 0:
            a_i = 0.0
        else:
            a_i = np.mean([np.sqrt(np.sum((X[i] - X[j])**2)) 
                          for j in range(n) if same_cluster_mask[j]])
        
        # b(i): minimum mean distance to other clusters
        b_i = float('inf')
        for cluster in unique_labels:
            if cluster == own_cluster:
                continue
            
            other_mask = labels == cluster
            if np.sum(other_mask) > 0:
                mean_dist = np.mean([np.sqrt(np.sum((X[i] - X[j])**2))
                                    for j in range(n) if other_mask[j]])
                b_i = min(b_i, mean_dist)
        
        # Silhouette for sample i
        if max(a_i, b_i) == 0:
            s_i = 0.0
        else:
            s_i = (b_i - a_i) / max(a_i, b_i)
        
        silhouettes.append(s_i)
    
    return np.mean(silhouettes)


# =============================================================================
# PART 4: COMPARISON WITH SCIKIT-LEARN
# =============================================================================

def compare_with_sklearn(X: np.ndarray, k: int = 3, random_state: int = 42):
    """
    Compare manual implementation with scikit-learn.
    
    This validates our implementation against the industry standard.
    """
    try:
        from sklearn.cluster import KMeans as SklearnKMeans
        from sklearn.metrics import silhouette_score as sklearn_silhouette
    except ImportError:
        print("\nscikit-learn not available for comparison")
        return
    
    print("\n" + "=" * 60)
    print("COMPARISON: Manual vs scikit-learn")
    print("=" * 60)
    
    # Manual implementation
    start = time.time()
    manual = KMeansManual(k=k, random_state=random_state)
    manual.fit(X, init='kmeans++')
    manual_time = time.time() - start
    manual_silhouette = silhouette_coefficient(X, manual.labels)
    
    # scikit-learn
    start = time.time()
    sklearn = SklearnKMeans(n_clusters=k, random_state=random_state, 
                            init='k-means++', n_init=1)
    sklearn_labels = sklearn.fit_predict(X)
    sklearn_time = time.time() - start
    sklearn_silhouette_score = sklearn_silhouette(X, sklearn_labels)
    
    print(f"\n{'Metric':<25} {'Manual':>15} {'scikit-learn':>15}")
    print("-" * 55)
    print(f"{'Inertia':<25} {manual.inertia:>15.4f} {sklearn.inertia_:>15.4f}")
    print(f"{'Silhouette Score':<25} {manual_silhouette:>15.4f} {sklearn_silhouette_score:>15.4f}")
    print(f"{'Training Time (s)':<25} {manual_time:>15.4f} {sklearn_time:>15.4f}")
    print(f"{'Iterations':<25} {manual.n_iterations:>15d} {sklearn.n_iter_:>15d}")
    
    # Centroid comparison
    print("\nCentroid Comparison (sorted by first coordinate):")
    manual_sorted = sorted(manual.centroids.tolist(), key=lambda x: x[0])
    sklearn_sorted = sorted(sklearn.cluster_centers_.tolist(), key=lambda x: x[0])
    
    for i, (m, s) in enumerate(zip(manual_sorted, sklearn_sorted)):
        diff = np.sqrt(sum((a-b)**2 for a, b in zip(m, s)))
        print(f"  Cluster {i}: Manual={m[:2]}..., sklearn={s[:2]}..., diff={diff:.4f}")


# =============================================================================
# PART 5: GENERATE SYNTHETIC DATA
# =============================================================================

def generate_clustered_data(n_samples: int = 300, n_features: int = 2,
                            n_clusters: int = 3, cluster_std: float = 1.0,
                            random_state: int = 42) -> Tuple[np.ndarray, np.ndarray]:
    """
    Generate synthetic clustered data for testing.
    
    Creates well-separated Gaussian blobs.
    
    Equivalent C:
        for (int c = 0; c < n_clusters; c++) {
            double center_x = (rand() % 20) - 10;
            double center_y = (rand() % 20) - 10;
            
            for (int i = 0; i < samples_per_cluster; i++) {
                data[idx][0] = center_x + gaussian_noise() * std;
                data[idx][1] = center_y + gaussian_noise() * std;
                labels[idx] = c;
                idx++;
            }
        }
    """
    np.random.seed(random_state)
    
    samples_per_cluster = n_samples // n_clusters
    X = np.zeros((n_samples, n_features))
    y = np.zeros(n_samples, dtype=int)
    
    # Generate cluster centres
    centres = np.random.uniform(-10, 10, (n_clusters, n_features))
    
    idx = 0
    for c in range(n_clusters):
        n = samples_per_cluster if c < n_clusters - 1 else n_samples - idx
        X[idx:idx+n] = centres[c] + np.random.normal(0, cluster_std, (n, n_features))
        y[idx:idx+n] = c
        idx += n
    
    # Shuffle
    perm = np.random.permutation(n_samples)
    return X[perm], y[perm]


def load_iris_data() -> Tuple[np.ndarray, np.ndarray]:
    """
    Load Iris dataset (or generate synthetic equivalent).
    """
    try:
        from sklearn.datasets import load_iris
        iris = load_iris()
        return iris.data, iris.target
    except ImportError:
        print("Using synthetic Iris-like data")
        return generate_clustered_data(150, 4, 3, 0.5, 42)


# =============================================================================
# PART 6: MAIN DEMONSTRATION
# =============================================================================

def main():
    """
    Main demonstration comparing K-Means implementations.
    """
    print("╔" + "═" * 62 + "╗")
    print("║" + " " * 62 + "║")
    print("║     WEEK 18: K-MEANS CLUSTERING - PYTHON COMPARISON         ║")
    print("║" + " " * 62 + "║")
    print("╚" + "═" * 62 + "╝")
    
    # Generate test data
    print("\n" + "=" * 60)
    print("GENERATING TEST DATA")
    print("=" * 60)
    
    X_synth, y_true = generate_clustered_data(300, 2, 3, 1.5, 42)
    print(f"Synthetic data: {X_synth.shape[0]} samples, {X_synth.shape[1]} features")
    print(f"True clusters: {len(np.unique(y_true))}")
    
    # Test manual K-Means
    print("\n" + "=" * 60)
    print("TEST 1: SYNTHETIC DATA (2D)")
    print("=" * 60)
    
    kmeans = KMeansManual(k=3, max_iterations=100, random_state=42)
    labels = kmeans.fit_predict(X_synth, init='kmeans++')
    
    silhouette = silhouette_coefficient(X_synth, labels)
    print(f"\nSilhouette Score: {silhouette:.4f}")
    
    # Cluster sizes
    print("\nCluster Sizes:")
    for c in range(kmeans.k):
        size = np.sum(labels == c)
        print(f"  Cluster {c}: {size} samples ({100*size/len(labels):.1f}%)")
    
    # Elbow analysis
    k_values, inertias = elbow_analysis(X_synth, range(1, 8), 42)
    plot_elbow_ascii(k_values, inertias)
    
    # Test on Iris
    print("\n" + "=" * 60)
    print("TEST 2: IRIS DATASET")
    print("=" * 60)
    
    X_iris, y_iris = load_iris_data()
    print(f"Iris data: {X_iris.shape[0]} samples, {X_iris.shape[1]} features")
    
    kmeans_iris = KMeansManual(k=3, max_iterations=100, random_state=42)
    iris_labels = kmeans_iris.fit_predict(X_iris, init='kmeans++')
    
    iris_silhouette = silhouette_coefficient(X_iris, iris_labels)
    print(f"\nSilhouette Score: {iris_silhouette:.4f}")
    
    # Cluster-to-class mapping
    print("\nCluster-Class Correspondence:")
    for c in range(3):
        cluster_mask = iris_labels == c
        if np.sum(cluster_mask) > 0:
            true_labels_in_cluster = y_iris[cluster_mask]
            majority_class = np.bincount(true_labels_in_cluster).argmax()
            purity = np.sum(true_labels_in_cluster == majority_class) / np.sum(cluster_mask)
            print(f"  Cluster {c}: Majority class = {majority_class}, "
                  f"Purity = {purity:.2%}")
    
    # Compare with scikit-learn
    compare_with_sklearn(X_iris, k=3, random_state=42)
    
    # Initialisation comparison
    print("\n" + "=" * 60)
    print("INITIALISATION COMPARISON")
    print("=" * 60)
    
    print("\nRandom Initialisation:")
    km_random = KMeansManual(k=3, max_iterations=100, random_state=42)
    km_random.fit(X_iris, init='random')
    
    print("\nK-Means++ Initialisation:")
    km_pp = KMeansManual(k=3, max_iterations=100, random_state=42)
    km_pp.fit(X_iris, init='kmeans++')
    
    print(f"\nComparison:")
    print(f"  Random:    Inertia = {km_random.inertia:.4f}, "
          f"Iterations = {km_random.n_iterations}")
    print(f"  K-Means++: Inertia = {km_pp.inertia:.4f}, "
          f"Iterations = {km_pp.n_iterations}")
    
    print("\n╔" + "═" * 62 + "╗")
    print("║                 DEMONSTRATION COMPLETE                       ║")
    print("╚" + "═" * 62 + "╝")


if __name__ == "__main__":
    main()

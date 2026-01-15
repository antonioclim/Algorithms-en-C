#!/usr/bin/env python3
"""
==============================================================================
WEEK 18: MACHINE LEARNING FUNDAMENTALS
Python Comparison: K-Nearest Neighbours
==============================================================================

This file provides Python implementations of K-NN classification using both
NumPy from scratch and scikit-learn for comparison with the C implementation.

Purpose: Educational reference only - demonstrates the K-NN algorithm in a
high-level language to help students understand the concepts before
implementing in C.

Usage:
    python3 knn_python.py [iris.csv]

Dependencies:
    - numpy
    - scikit-learn (optional, for comparison)

==============================================================================
"""

import numpy as np
import sys
from collections import Counter


def load_iris_data(filename='data/iris.csv'):
    """
    Load Iris dataset from CSV file.
    
    Expected format: sepal_length,sepal_width,petal_length,petal_width,class
    """
    features = []
    labels = []
    class_map = {
        'Iris-setosa': 0,
        'Iris-versicolor': 1,
        'Iris-virginica': 2
    }
    
    try:
        with open(filename, 'r') as f:
            # Skip header
            next(f)
            for line in f:
                parts = line.strip().split(',')
                if len(parts) == 5:
                    features.append([float(x) for x in parts[:4]])
                    class_name = parts[4].strip()
                    labels.append(class_map.get(class_name, -1))
        
        return np.array(features), np.array(labels)
    
    except FileNotFoundError:
        print(f"File not found: {filename}")
        print("Generating synthetic Iris-like data...")
        return generate_synthetic_iris()


def generate_synthetic_iris(n_per_class=50):
    """Generate synthetic Iris-like dataset."""
    np.random.seed(42)
    
    # Mean and std for each class (approximating real Iris)
    params = {
        0: {'mean': [5.0, 3.4, 1.5, 0.2], 'std': [0.35, 0.38, 0.17, 0.11]},  # setosa
        1: {'mean': [5.9, 2.8, 4.3, 1.3], 'std': [0.52, 0.31, 0.47, 0.20]},  # versicolor
        2: {'mean': [6.6, 3.0, 5.5, 2.0], 'std': [0.64, 0.32, 0.55, 0.27]}   # virginica
    }
    
    features = []
    labels = []
    
    for class_idx in range(3):
        mean = params[class_idx]['mean']
        std = params[class_idx]['std']
        
        for _ in range(n_per_class):
            sample = [np.random.normal(m, s) for m, s in zip(mean, std)]
            features.append(sample)
            labels.append(class_idx)
    
    return np.array(features), np.array(labels)


def euclidean_distance(a, b):
    """
    Compute Euclidean distance between two vectors.
    
    Formula: d(a, b) = sqrt(Σ(a_i - b_i)²)
    """
    return np.sqrt(np.sum((a - b) ** 2))


def manhattan_distance(a, b):
    """
    Compute Manhattan distance between two vectors.
    
    Formula: d(a, b) = Σ|a_i - b_i|
    """
    return np.sum(np.abs(a - b))


class KNNClassifier:
    """
    K-Nearest Neighbours classifier implemented from scratch.
    
    This is a lazy learning algorithm - no actual training occurs.
    All computation happens at prediction time.
    """
    
    def __init__(self, k=5, distance_metric='euclidean'):
        self.k = k
        self.distance_metric = distance_metric
        self.X_train = None
        self.y_train = None
    
    def _get_distance_func(self):
        """Return the appropriate distance function."""
        if self.distance_metric == 'manhattan':
            return manhattan_distance
        return euclidean_distance
    
    def fit(self, X, y):
        """
        'Train' the model (simply stores the data).
        
        K-NN is instance-based: it memorises the training data
        rather than learning parameters.
        """
        self.X_train = np.array(X)
        self.y_train = np.array(y)
        return self
    
    def predict_single(self, x):
        """
        Predict class for a single sample.
        
        Algorithm:
            1. Compute distances to all training samples
            2. Find K nearest neighbours
            3. Vote among neighbours (majority wins)
        """
        distance_func = self._get_distance_func()
        
        # Compute all distances
        distances = [(distance_func(x, x_train), y_train) 
                     for x_train, y_train in zip(self.X_train, self.y_train)]
        
        # Sort by distance
        distances.sort(key=lambda x: x[0])
        
        # Get K nearest neighbours' labels
        k_nearest_labels = [label for _, label in distances[:self.k]]
        
        # Vote
        vote_counts = Counter(k_nearest_labels)
        return vote_counts.most_common(1)[0][0]
    
    def predict(self, X):
        """Predict classes for multiple samples."""
        return np.array([self.predict_single(x) for x in X])
    
    def score(self, X, y):
        """Compute classification accuracy."""
        predictions = self.predict(X)
        return np.mean(predictions == y)


def train_test_split(X, y, test_ratio=0.2, random_state=42):
    """Split data into training and testing sets."""
    np.random.seed(random_state)
    n_samples = len(y)
    n_test = int(n_samples * test_ratio)
    
    indices = np.random.permutation(n_samples)
    test_idx = indices[:n_test]
    train_idx = indices[n_test:]
    
    return X[train_idx], X[test_idx], y[train_idx], y[test_idx]


def confusion_matrix(y_true, y_pred, n_classes=3):
    """Compute confusion matrix."""
    matrix = np.zeros((n_classes, n_classes), dtype=int)
    for true, pred in zip(y_true, y_pred):
        if 0 <= true < n_classes and 0 <= pred < n_classes:
            matrix[true][pred] += 1
    return matrix


def main():
    print("=" * 70)
    print("K-NEAREST NEIGHBOURS - PYTHON IMPLEMENTATION")
    print("=" * 70)
    print()
    
    # Load data
    filename = sys.argv[1] if len(sys.argv) > 1 else 'data/iris.csv'
    X, y = load_iris_data(filename)
    print(f"Loaded {len(y)} samples with {X.shape[1]} features")
    
    # Dataset statistics
    class_names = ['setosa', 'versicolor', 'virginica']
    print("\nClass distribution:")
    for i, name in enumerate(class_names):
        count = np.sum(y == i)
        print(f"  {name}: {count} samples")
    print()
    
    # Split data
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_ratio=0.2)
    print(f"Training samples: {len(y_train)}")
    print(f"Testing samples:  {len(y_test)}")
    print()
    
    # Test different K values
    print("Testing different K values:")
    print("-" * 50)
    print(f"{'K':<5} {'Euclidean':>15} {'Manhattan':>15}")
    
    for k in [1, 3, 5, 7, 9, 11]:
        knn_euclid = KNNClassifier(k=k, distance_metric='euclidean')
        knn_euclid.fit(X_train, y_train)
        acc_euclid = knn_euclid.score(X_test, y_test)
        
        knn_manh = KNNClassifier(k=k, distance_metric='manhattan')
        knn_manh.fit(X_train, y_train)
        acc_manh = knn_manh.score(X_test, y_test)
        
        print(f"{k:<5} {acc_euclid*100:>14.1f}% {acc_manh*100:>14.1f}%")
    
    print()
    
    # Best model confusion matrix
    print("Confusion Matrix (K=5, Euclidean):")
    print("-" * 50)
    
    best_model = KNNClassifier(k=5, distance_metric='euclidean')
    best_model.fit(X_train, y_train)
    y_pred = best_model.predict(X_test)
    
    cm = confusion_matrix(y_test, y_pred)
    
    print(f"{'':>15}", end='')
    for name in class_names:
        print(f"{name[:10]:>12}", end='')
    print()
    
    for i, name in enumerate(class_names):
        print(f"{name[:15]:>15}", end='')
        for j in range(3):
            print(f"{cm[i][j]:>12}", end='')
        print()
    
    print()
    
    # Compare with scikit-learn
    try:
        from sklearn.neighbors import KNeighborsClassifier
        
        print("Comparison with scikit-learn:")
        print("-" * 50)
        
        sklearn_knn = KNeighborsClassifier(n_neighbors=5)
        sklearn_knn.fit(X_train, y_train)
        sklearn_acc = sklearn_knn.score(X_test, y_test)
        
        print(f"Our implementation:    {best_model.score(X_test, y_test)*100:.1f}%")
        print(f"scikit-learn:          {sklearn_acc*100:.1f}%")
        print()
        
    except ImportError:
        print("scikit-learn not available for comparison")
        print()
    
    print("=" * 70)
    print("ANALYSIS COMPLETE")
    print("=" * 70)


if __name__ == "__main__":
    main()

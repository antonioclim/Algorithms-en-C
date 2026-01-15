#!/usr/bin/env python3
"""
==============================================================================
WEEK 18: MACHINE LEARNING FUNDAMENTALS
Python Comparison: Linear Regression
==============================================================================

This file provides Python implementations of linear regression using both
NumPy from scratch and scikit-learn for comparison with the C implementation.

Purpose: Educational reference only - demonstrates equivalent algorithms in
a high-level language to help students understand the mathematical concepts.

Usage:
    python3 linear_regression_python.py [housing.csv]

Dependencies:
    - numpy
    - pandas (optional, for CSV loading)
    - scikit-learn (optional, for comparison)

==============================================================================
"""

import numpy as np
import sys


def load_housing_data(filename='data/housing.csv'):
    """
    Load housing data from CSV file.
    
    Expected format: square_feet,bedrooms,age,price
    """
    try:
        data = np.genfromtxt(filename, delimiter=',', skip_header=1)
        X = data[:, :-1]  # Features
        y = data[:, -1]   # Target (price)
        return X, y
    except FileNotFoundError:
        print(f"File not found: {filename}")
        print("Generating synthetic data...")
        return generate_synthetic_data()


def generate_synthetic_data(n_samples=100):
    """Generate synthetic housing data."""
    np.random.seed(42)
    
    # Features: square_feet, bedrooms, age
    square_feet = np.random.uniform(800, 3000, n_samples)
    bedrooms = np.random.randint(1, 6, n_samples)
    age = np.random.randint(0, 50, n_samples)
    
    # Target: price = 50*sqft + 10000*bedrooms - 1000*age + 100000 + noise
    price = (50 * square_feet + 
             10000 * bedrooms - 
             1000 * age + 
             100000 + 
             np.random.normal(0, 15000, n_samples))
    
    X = np.column_stack([square_feet, bedrooms, age])
    return X, price


def z_score_normalise(X, mean=None, std=None):
    """
    Apply z-score normalisation.
    
    Formula: x_normalised = (x - mean) / std
    
    Parameters:
        X: Input features (n_samples x n_features)
        mean: Pre-computed mean (for test data)
        std: Pre-computed std (for test data)
    
    Returns:
        X_normalised, mean, std
    """
    if mean is None:
        mean = np.mean(X, axis=0)
    if std is None:
        std = np.std(X, axis=0)
        std[std == 0] = 1  # Avoid division by zero
    
    X_normalised = (X - mean) / std
    return X_normalised, mean, std


class LinearRegressionScratch:
    """
    Linear Regression implemented from scratch using NumPy.
    
    This implementation uses gradient descent optimisation.
    """
    
    def __init__(self, learning_rate=0.01, n_iterations=1000, verbose=False):
        self.learning_rate = learning_rate
        self.n_iterations = n_iterations
        self.verbose = verbose
        self.weights = None
        self.bias = None
        self.loss_history = []
    
    def fit(self, X, y):
        """
        Train the model using gradient descent.
        
        Algorithm:
            1. Initialise weights to zeros
            2. For each iteration:
               a. Compute predictions: y_pred = X · w + b
               b. Compute gradients:
                  ∂L/∂w = (2/n) × X^T · (y_pred - y)
                  ∂L/∂b = (2/n) × Σ(y_pred - y)
               c. Update: w = w - lr × ∂L/∂w
                         b = b - lr × ∂L/∂b
        """
        n_samples, n_features = X.shape
        
        # Initialise parameters
        self.weights = np.zeros(n_features)
        self.bias = 0.0
        self.loss_history = []
        
        for i in range(self.n_iterations):
            # Forward pass
            y_pred = np.dot(X, self.weights) + self.bias
            
            # Compute loss (MSE)
            loss = np.mean((y_pred - y) ** 2)
            self.loss_history.append(loss)
            
            # Compute gradients
            dw = (2 / n_samples) * np.dot(X.T, (y_pred - y))
            db = (2 / n_samples) * np.sum(y_pred - y)
            
            # Update parameters
            self.weights -= self.learning_rate * dw
            self.bias -= self.learning_rate * db
            
            if self.verbose and i % 100 == 0:
                print(f"Iteration {i}: Loss = {loss:.4f}")
        
        return self
    
    def predict(self, X):
        """Make predictions."""
        return np.dot(X, self.weights) + self.bias
    
    def score(self, X, y):
        """
        Compute R² score.
        
        R² = 1 - (SS_res / SS_tot)
        where:
            SS_res = Σ(y - y_pred)²
            SS_tot = Σ(y - y_mean)²
        """
        y_pred = self.predict(X)
        ss_res = np.sum((y - y_pred) ** 2)
        ss_tot = np.sum((y - np.mean(y)) ** 2)
        return 1 - (ss_res / ss_tot)


def mse(y_true, y_pred):
    """Compute Mean Squared Error."""
    return np.mean((y_true - y_pred) ** 2)


def rmse(y_true, y_pred):
    """Compute Root Mean Squared Error."""
    return np.sqrt(mse(y_true, y_pred))


def train_test_split(X, y, test_ratio=0.2, random_state=42):
    """Split data into training and testing sets."""
    np.random.seed(random_state)
    n_samples = len(y)
    n_test = int(n_samples * test_ratio)
    
    indices = np.random.permutation(n_samples)
    test_idx = indices[:n_test]
    train_idx = indices[n_test:]
    
    return X[train_idx], X[test_idx], y[train_idx], y[test_idx]


def main():
    print("=" * 70)
    print("LINEAR REGRESSION - PYTHON IMPLEMENTATION")
    print("=" * 70)
    print()
    
    # Load data
    filename = sys.argv[1] if len(sys.argv) > 1 else 'data/housing.csv'
    X, y = load_housing_data(filename)
    print(f"Loaded {len(y)} samples with {X.shape[1]} features")
    print()
    
    # Split data
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_ratio=0.2)
    print(f"Training samples: {len(y_train)}")
    print(f"Testing samples:  {len(y_test)}")
    print()
    
    # Normalise features
    X_train_norm, train_mean, train_std = z_score_normalise(X_train)
    X_test_norm, _, _ = z_score_normalise(X_test, train_mean, train_std)
    
    print("Feature Statistics:")
    print("-" * 50)
    feature_names = ['Square Feet', 'Bedrooms', 'Age']
    print(f"{'Feature':<15} {'Mean':>12} {'Std':>12}")
    for i, name in enumerate(feature_names):
        print(f"{name:<15} {train_mean[i]:>12.2f} {train_std[i]:>12.2f}")
    print()
    
    # Train model from scratch
    print("Training Linear Regression (from scratch)...")
    print("-" * 50)
    
    model = LinearRegressionScratch(
        learning_rate=0.01,
        n_iterations=1000,
        verbose=True
    )
    model.fit(X_train_norm, y_train)
    
    print()
    print("Model Parameters:")
    print(f"  Weights: {model.weights}")
    print(f"  Bias:    {model.bias:.2f}")
    print()
    
    # Evaluate
    train_pred = model.predict(X_train_norm)
    test_pred = model.predict(X_test_norm)
    
    print("Evaluation Metrics:")
    print("-" * 50)
    print(f"Training MSE:  {mse(y_train, train_pred):,.2f}")
    print(f"Training RMSE: {rmse(y_train, train_pred):,.2f}")
    print(f"Test MSE:      {mse(y_test, test_pred):,.2f}")
    print(f"Test RMSE:     {rmse(y_test, test_pred):,.2f}")
    print(f"R² Score:      {model.score(X_test_norm, y_test):.4f}")
    print()
    
    # Compare with scikit-learn (if available)
    try:
        from sklearn.linear_model import LinearRegression as SklearnLR
        
        print("Comparison with scikit-learn:")
        print("-" * 50)
        
        sklearn_model = SklearnLR()
        sklearn_model.fit(X_train_norm, y_train)
        sklearn_pred = sklearn_model.predict(X_test_norm)
        
        print(f"sklearn MSE:   {mse(y_test, sklearn_pred):,.2f}")
        print(f"sklearn R²:    {sklearn_model.score(X_test_norm, y_test):.4f}")
        print()
        
    except ImportError:
        print("scikit-learn not available for comparison")
        print()
    
    print("=" * 70)
    print("ANALYSIS COMPLETE")
    print("=" * 70)


if __name__ == "__main__":
    main()

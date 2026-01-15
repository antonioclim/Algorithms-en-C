# Week 18: Machine Learning Fundamentals in C
## Extended Challenges (Bonus)

---

## 🏆 Overview

These extended challenges are **optional** and designed for students who wish to deepen their understanding of machine learning algorithms. Each challenge explores advanced concepts beyond the core curriculum.

**Bonus Points**: Each challenge is worth up to 10 bonus points
**Maximum Bonus**: 30 points (best 3 of 5 challenges)

---

## Challenge 1: Mini-batch Stochastic Gradient Descent

### Objective

Implement efficient mini-batch gradient descent with configurable batch sizes and compare convergence behaviour across different batch configurations.

### Background

Full-batch gradient descent computes gradients over the entire dataset, which becomes computationally expensive for large datasets. Stochastic Gradient Descent (SGD) uses single samples, introducing noise but enabling faster iteration. Mini-batch SGD strikes a balance between these extremes.

### Requirements

1. **Implementation**
   - Extend linear regression to support mini-batch updates
   - Implement sample shuffling at each epoch
   - Support configurable batch sizes (1, 16, 32, 64, full)

2. **Convergence Analysis**
   - Track loss at each iteration (not just epoch)
   - Measure wall-clock time to reach target loss
   - Plot learning curves for different batch sizes (ASCII)

3. **Momentum Enhancement** (bonus within bonus)
   - Implement momentum-based SGD
   - Compare convergence with/without momentum
   - Formula: v_t = βv_{t-1} + (1-β)∇L; θ = θ - αv_t

### Expected Deliverables

```
challenge1/
├── sgd_variants.c      # Main implementation
├── batch_experiment.c  # Batch size comparison
├── Makefile
└── convergence.txt     # Results and analysis
```

### Evaluation Criteria

| Aspect | Points |
|--------|--------|
| Correct mini-batch implementation | 4 |
| Proper shuffling each epoch | 2 |
| Convergence comparison | 2 |
| Momentum implementation | 2 |

---

## Challenge 2: Regularisation Techniques (L1/L2)

### Objective

Implement L1 (Lasso) and L2 (Ridge) regularisation for linear regression and analyse their effects on model coefficients and generalisation.

### Background

Regularisation adds a penalty term to the loss function to prevent overfitting:
- **L2 (Ridge)**: Loss = MSE + λ∑w²ᵢ — shrinks weights uniformly
- **L1 (Lasso)**: Loss = MSE + λ∑|wᵢ| — produces sparse solutions

### Requirements

1. **Implementation**
   - Modify linear regression to support regularisation
   - Implement both L1 and L2 penalties
   - Support Elastic Net (combination of L1 and L2)

2. **Gradient Modifications**
   ```
   L2 gradient: ∂L/∂w = ∂MSE/∂w + 2λw
   L1 gradient: ∂L/∂w = ∂MSE/∂w + λ·sign(w)
   ```

3. **Analysis**
   - Create dataset with some irrelevant features
   - Compare coefficients with/without regularisation
   - Demonstrate L1 driving coefficients to exactly zero

4. **Cross-Validation for λ Selection**
   - Implement grid search over λ values
   - Plot validation error vs λ (ASCII)
   - Report optimal λ

### Expected Deliverables

```
challenge2/
├── regularisation.c     # Main implementation
├── lambda_search.c      # Hyperparameter tuning
├── Makefile
└── analysis_report.txt  # Coefficient analysis
```

### Evaluation Criteria

| Aspect | Points |
|--------|--------|
| L2 regularisation | 3 |
| L1 regularisation | 3 |
| Sparsity demonstration | 2 |
| Lambda selection via CV | 2 |

---

## Challenge 3: K-Fold Cross-Validation Framework

### Objective

Build a comprehensive cross-validation framework that supports stratified folds, multiple scoring metrics and automated model selection.

### Background

Cross-validation provides robust performance estimates by partitioning data into complementary subsets. K-fold CV rotates through K partitions, using each as a validation set exactly once.

### Requirements

1. **Basic K-Fold**
   - Implement data partitioning into K folds
   - Ensure each sample appears in exactly one fold
   - Support configurable K (default: 5)

2. **Stratified K-Fold**
   - Maintain class proportions in each fold
   - Critical for imbalanced datasets
   - Verify stratification correctness

3. **Nested Cross-Validation**
   - Outer loop: model evaluation
   - Inner loop: hyperparameter tuning
   - Prevents optimistic bias in performance estimates

4. **Multiple Metrics**
   - Support accuracy, precision, recall, F1
   - Report mean ± standard deviation
   - Statistical significance testing (optional)

### Example Output

```
╔═══════════════════════════════════════════════════════════════╗
║              5-FOLD CROSS-VALIDATION RESULTS                  ║
╚═══════════════════════════════════════════════════════════════╝

Model: K-NN (k=5)

Fold Results:
┌──────────┬───────────┬───────────┬───────────┬───────────┐
│ Fold     │ Accuracy  │ Precision │ Recall    │ F1 Score  │
├──────────┼───────────┼───────────┼───────────┼───────────┤
│ 1        │    0.9333 │    0.9412 │    0.9231 │    0.9320 │
│ 2        │    0.9000 │    0.9091 │    0.8889 │    0.8989 │
│ 3        │    0.9333 │    0.9375 │    0.9375 │    0.9375 │
│ 4        │    0.8667 │    0.8750 │    0.8750 │    0.8750 │
│ 5        │    0.9667 │    0.9688 │    0.9688 │    0.9688 │
├──────────┼───────────┼───────────┼───────────┼───────────┤
│ Mean     │    0.9200 │    0.9263 │    0.9187 │    0.9224 │
│ Std      │    0.0365 │    0.0344 │    0.0364 │    0.0351 │
└──────────┴───────────┴───────────┴───────────┴───────────┘
```

### Expected Deliverables

```
challenge3/
├── cross_validation.c   # CV framework
├── stratified_cv.c      # Stratified implementation
├── nested_cv.c          # Nested CV for hyperparameter tuning
├── Makefile
└── cv_results.txt
```

### Evaluation Criteria

| Aspect | Points |
|--------|--------|
| Basic K-fold implementation | 3 |
| Stratified sampling | 3 |
| Multiple metrics | 2 |
| Nested CV | 2 |

---

## Challenge 4: Random Forest (Ensemble Learning)

### Objective

Implement a simplified Random Forest classifier combining multiple decision trees with bagging and random feature selection.

### Background

Random Forests combat overfitting by:
1. **Bagging**: Training each tree on a bootstrap sample
2. **Feature randomness**: Each split considers random feature subset
3. **Majority voting**: Combining predictions from all trees

### Requirements

1. **Decision Tree Extension**
   - Implement max_depth limiting
   - Add min_samples_split parameter
   - Random feature subset at each split (√d features)

2. **Bootstrap Sampling**
   - Sample n points with replacement
   - Approximately 63.2% unique samples per tree
   - Out-of-bag (OOB) samples for validation

3. **Forest Implementation**
   - Support configurable number of trees (default: 100)
   - Implement parallel-ready structure (future pthreads)
   - Majority voting for classification

4. **Feature Importance**
   - Track how often each feature is used for splits
   - Calculate mean decrease in impurity
   - Report ranked feature importance

### Example Output

```
╔═══════════════════════════════════════════════════════════════╗
║              RANDOM FOREST RESULTS                            ║
╚═══════════════════════════════════════════════════════════════╝

Configuration:
  Number of trees: 50
  Max depth: 10
  Features per split: 2 (√4)

Performance:
  Training Accuracy: 100.00%
  Test Accuracy:      94.67%
  OOB Accuracy:       93.33%

Feature Importance:
┌──────────────────────┬───────────────┐
│ Feature              │ Importance    │
├──────────────────────┼───────────────┤
│ Petal Length         │     0.4523    │
│ Petal Width          │     0.3891    │
│ Sepal Length         │     0.0987    │
│ Sepal Width          │     0.0599    │
└──────────────────────┴───────────────┘
```

### Expected Deliverables

```
challenge4/
├── decision_tree.c     # Enhanced decision tree
├── random_forest.c     # Forest implementation
├── Makefile
└── forest_analysis.txt
```

### Evaluation Criteria

| Aspect | Points |
|--------|--------|
| Bootstrap sampling | 2 |
| Random feature selection | 3 |
| Forest aggregation | 3 |
| Feature importance | 2 |

---

## Challenge 5: Naive Bayes Classifier

### Objective

Implement Gaussian Naive Bayes for classification and compare with discriminative approaches (logistic regression, K-NN).

### Background

Naive Bayes is a generative classifier based on Bayes' theorem:

```
P(class|features) ∝ P(features|class) × P(class)
```

The "naive" assumption is that features are conditionally independent given the class, allowing:

```
P(x₁,x₂,...,xₙ|class) = ∏P(xᵢ|class)
```

For continuous features, Gaussian Naive Bayes assumes:
```
P(xᵢ|class) = N(μ_class, σ²_class)
```

### Requirements

1. **Training Phase**
   - Compute class priors P(class) from training data
   - Estimate μ and σ² for each feature per class
   - Handle numerical stability (log probabilities)

2. **Prediction Phase**
   - Compute log P(class) + Σ log P(xᵢ|class)
   - Return class with highest posterior probability
   - Provide probability estimates (normalised)

3. **Laplace Smoothing** (optional)
   - Add small constant to prevent zero probabilities
   - Critical for sparse categorical features

4. **Comparison Study**
   - Compare with K-NN and logistic regression
   - Measure training time (Naive Bayes is fast!)
   - Analyse where Naive Bayes excels/fails

### Mathematical Implementation

```c
/* Training: compute statistics */
typedef struct {
    double *class_priors;     /* P(class) for each class */
    double **means;           /* μ[class][feature] */
    double **variances;       /* σ²[class][feature] */
    int num_classes;
    int num_features;
} NaiveBayes;

/* Prediction: compute log probability */
double log_gaussian_pdf(double x, double mean, double var) {
    double log_coeff = -0.5 * log(2 * M_PI * var);
    double exponent = -0.5 * (x - mean) * (x - mean) / var;
    return log_coeff + exponent;
}

int predict(NaiveBayes *nb, double *sample) {
    int best_class = 0;
    double best_log_prob = -INFINITY;
    
    for (int c = 0; c < nb->num_classes; c++) {
        double log_prob = log(nb->class_priors[c]);
        for (int f = 0; f < nb->num_features; f++) {
            log_prob += log_gaussian_pdf(
                sample[f], 
                nb->means[c][f], 
                nb->variances[c][f]
            );
        }
        if (log_prob > best_log_prob) {
            best_log_prob = log_prob;
            best_class = c;
        }
    }
    return best_class;
}
```

### Expected Deliverables

```
challenge5/
├── naive_bayes.c       # Main implementation
├── comparison.c        # Comparison with other classifiers
├── Makefile
└── bayes_analysis.txt
```

### Evaluation Criteria

| Aspect | Points |
|--------|--------|
| Correct probability computation | 4 |
| Log-space implementation | 2 |
| Comparison study | 2 |
| Analysis of assumptions | 2 |

---

## 📋 Submission Guidelines

### Format

Submit bonus challenges as additional directories in your homework archive:

```
StudentName_Week18_Homework.zip
├── homework1/
├── homework2/
├── challenge1/        # If attempted
├── challenge3/        # If attempted
└── challenge5/        # If attempted
```

### Requirements

- All challenges must compile without warnings
- Include Makefile for each challenge
- No memory leaks (Valgrind verified)
- Clear documentation of approach
- Results file with analysis

### Partial Credit

- Partial implementations receive proportional credit
- Document what works and what remains incomplete
- Explain your approach even if not fully implemented

---

## 🎓 Learning Outcomes

Completing these challenges demonstrates:

1. **Mini-batch SGD**: Understanding of optimisation landscapes and trade-offs between noise and computation
2. **Regularisation**: Appreciation for bias-variance trade-off and model complexity control
3. **Cross-Validation**: Rigorous model evaluation methodology
4. **Random Forest**: Ensemble learning principles and variance reduction
5. **Naive Bayes**: Generative vs discriminative model comparison

---

## 💡 Tips for Success

1. **Start simple**: Get basic version working before optimisations
2. **Test incrementally**: Verify each component independently
3. **Use synthetic data**: Create simple cases with known answers
4. **Compare with Python**: Validate your C implementation against scikit-learn
5. **Profile your code**: Identify bottlenecks before optimising

---

## 📚 Reference Resources

### Mini-batch SGD
- Bottou, L. "Large-Scale Machine Learning with Stochastic Gradient Descent"
- Ruder, S. "An Overview of Gradient Descent Optimization Algorithms"

### Regularisation
- Hastie, Tibshirani & Friedman: Elements of Statistical Learning, Ch. 3
- Murphy: Machine Learning: A Probabilistic Perspective, Ch. 7

### Random Forest
- Breiman, L. "Random Forests" (2001)
- scikit-learn documentation on ensemble methods

### Naive Bayes
- Mitchell: Machine Learning, Ch. 6
- Murphy: Machine Learning: A Probabilistic Perspective, Ch. 3

---

*Extended challenges prepared for ATP Course*
*Academy of Economic Studies - CSIE Bucharest*
*Alternative learning kit for non-formal education*

# Week 18: Machine Learning Fundamentals in C
## Homework Requirements

---

## 📋 General Information

**Course**: Algorithms and Programming Techniques (ATP)
**Institution**: Academy of Economic Studies Bucharest - CSIE
**Week**: 18 - Machine Learning Fundamentals
**Total Points**: 100 (50 per assignment)
**Deadline**: Two weeks from laboratory session

---

## 🎯 Learning Objectives

These homework assignments reinforce:

1. Implementing complete ML pipelines from scratch in C
2. Understanding data preprocessing and its importance
3. Comparing multiple models systematically
4. Building neural networks without external libraries
5. Applying backpropagation for training

---

## 📝 Homework 1: Complete ML Pipeline (50 points)

### Overview

Implement a comprehensive machine learning pipeline that loads data, preprocesses it, trains multiple models and evaluates their performance systematically.

### Requirements

#### Part A: Data Handling (10 points)

1. **Data Loading**
   - Load CSV data with configurable delimiter
   - Handle missing values (skip or impute with mean)
   - Support both numeric and categorical features (encode categoricals)

2. **Train/Test Split**
   - Implement random shuffling with reproducible seed
   - Support configurable split ratio (default 80/20)
   - Ensure class balance in splits for classification

3. **Normalisation**
   - Implement Z-score normalisation
   - Implement Min-Max normalisation
   - Fit on training data only, transform both sets

#### Part B: Model Implementations (25 points)

1. **Linear Regression** (8 points)
   - Gradient descent optimisation
   - Configurable learning rate and iterations
   - MSE loss function

2. **K-Nearest Neighbours** (8 points)
   - Support both classification and regression
   - Implement Euclidean and Manhattan distance
   - Configurable K value

3. **K-Means Clustering** (9 points)
   - K-Means++ initialisation
   - Lloyd's algorithm
   - Compute within-cluster sum of squares

#### Part C: Evaluation Framework (15 points)

1. **Metrics**
   - MSE, RMSE, MAE for regression
   - R² (coefficient of determination)
   - Accuracy, Precision, Recall for classification

2. **Cross-Validation**
   - Implement K-fold cross-validation
   - Report mean and standard deviation of scores

3. **Model Comparison**
   - Generate comparison table
   - Recommend best model with justification

### Deliverables

```
homework1/
├── homework1.c          # Main implementation
├── data_utils.h         # Data loading and preprocessing
├── models.h             # Model implementations
├── evaluation.h         # Evaluation metrics
├── Makefile             # Build script
└── report.txt           # Results and analysis
```

### Expected Output

```
╔═══════════════════════════════════════════════════════════════╗
║              ML PIPELINE - EVALUATION RESULTS                 ║
╚═══════════════════════════════════════════════════════════════╝

Dataset: housing.csv
Samples: 300 (240 train, 60 test)
Features: 4

Cross-Validation Results (5-fold):
┌────────────────────┬────────────────┬────────────────┐
│ Model              │ Mean MSE       │ Std MSE        │
├────────────────────┼────────────────┼────────────────┤
│ Linear Regression  │      12.3456   │       2.1234   │
│ K-NN (k=5)        │      15.6789   │       3.4567   │
│ K-Means (k=5)     │      18.9012   │       4.5678   │
└────────────────────┴────────────────┴────────────────┘

Best Model: Linear Regression
Justification: Lowest CV MSE with acceptable variance
```

### Grading Criteria

| Component | Points | Criteria |
|-----------|--------|----------|
| Data Loading | 5 | Correct CSV parsing, error handling |
| Preprocessing | 5 | Proper normalisation, train/test split |
| Linear Regression | 8 | Working gradient descent, convergence |
| K-NN | 8 | Correct distance calculations, voting |
| K-Means | 9 | Proper clustering, K-Means++ init |
| Evaluation | 10 | Accurate metrics, cross-validation |
| Code Quality | 5 | Clean code, comments, no memory leaks |

---

## 📝 Homework 2: Neural Network from Scratch (50 points)

### Overview

Implement a fully-connected neural network with configurable architecture, supporting forward propagation, backpropagation and gradient descent training.

### Requirements

#### Part A: Network Architecture (15 points)

1. **Layer Structure**
   - Support arbitrary number of layers
   - Configurable neurons per layer
   - Multiple activation functions (Sigmoid, ReLU, Softmax)

2. **Weight Initialisation**
   - Random initialisation
   - Xavier/Glorot initialisation
   - He initialisation for ReLU

3. **Forward Propagation**
   - Matrix-vector operations
   - Proper activation function application
   - Store intermediate values for backprop

#### Part B: Training (25 points)

1. **Backpropagation** (15 points)
   - Correct gradient computation
   - Chain rule implementation
   - Output layer gradients for different losses

2. **Optimisation** (10 points)
   - Batch gradient descent
   - Mini-batch gradient descent
   - Configurable learning rate

#### Part C: Evaluation and Testing (10 points)

1. **XOR Problem**
   - Network solves XOR (not linearly separable)
   - Demonstrate perceptron limitation

2. **Digit Classification**
   - Train on simplified digit dataset
   - Achieve >90% accuracy on test set

### Network Specifications

```c
/* Minimum required structure */
typedef struct {
    int num_layers;
    int *layer_sizes;        /* Array of sizes */
    double **weights;        /* Weight matrices */
    double **biases;         /* Bias vectors */
    ActivationType *activations;
} NeuralNetwork;

/* Required functions */
void nn_init(NeuralNetwork *nn, int *sizes, int num_layers);
void nn_forward(NeuralNetwork *nn, double *input, double *output);
double nn_backward(NeuralNetwork *nn, double *target);
void nn_update_weights(NeuralNetwork *nn, double learning_rate);
void nn_train(NeuralNetwork *nn, Sample *data, int n, int epochs);
double nn_evaluate(NeuralNetwork *nn, Sample *data, int n);
void nn_free(NeuralNetwork *nn);
```

### Test Cases

**Test 1: XOR Problem**
```
Architecture: 2 → 4 → 2
Training: 1000 epochs
Expected: 100% accuracy on XOR truth table
```

**Test 2: Digit Classification**
```
Architecture: 16 → 32 → 16 → 10
Training: 500 epochs on 800 samples
Testing: 200 samples
Expected: >90% accuracy
```

### Deliverables

```
homework2/
├── homework2.c          # Main implementation
├── neural_network.h     # Network structure and functions
├── activations.h        # Activation functions
├── backprop.h           # Backpropagation implementation
├── Makefile             # Build script
└── training_log.txt     # Training progress and results
```

### Expected Output

```
╔═══════════════════════════════════════════════════════════════╗
║              NEURAL NETWORK - TRAINING RESULTS                ║
╚═══════════════════════════════════════════════════════════════╝

Network Architecture: 16 → 32 → 16 → 10
Total Parameters: 1098

Training Progress:
  Epoch  100: Loss = 2.3456, Accuracy = 45.67%
  Epoch  200: Loss = 1.5678, Accuracy = 67.89%
  Epoch  300: Loss = 0.8901, Accuracy = 82.34%
  Epoch  400: Loss = 0.4567, Accuracy = 91.23%
  Epoch  500: Loss = 0.2345, Accuracy = 95.67%

Final Results:
┌────────────────────┬────────────────┐
│ Metric             │ Value          │
├────────────────────┼────────────────┤
│ Training Accuracy  │        95.67%  │
│ Test Accuracy      │        92.34%  │
│ Training Time      │        12.34s  │
└────────────────────┴────────────────┘

✓ Target accuracy (>90%) achieved!
```

### Grading Criteria

| Component | Points | Criteria |
|-----------|--------|----------|
| Network Structure | 8 | Correct layer management, memory |
| Weight Init | 7 | Proper initialisation schemes |
| Forward Pass | 10 | Correct computations, activations |
| Backpropagation | 15 | Correct gradients, chain rule |
| XOR Solution | 5 | Solves XOR problem |
| Digit Classification | 5 | Achieves >90% accuracy |

---

## ⚠️ Important Notes

### Compilation Requirements

```bash
gcc -Wall -Wextra -std=c11 -pedantic -g -O0 homework1.c -o homework1 -lm
gcc -Wall -Wextra -std=c11 -pedantic -g -O0 homework2.c -o homework2 -lm
```

### Prohibited

- External machine learning libraries
- Python or other language implementations
- Copying code from the solutions directory

### Required

- All code must compile without warnings
- No memory leaks (verified with Valgrind)
- Clear comments explaining algorithms
- ASCII visualisations where appropriate

### Submission Format

Submit a ZIP archive containing:
```
StudentName_Week18_Homework.zip
├── homework1/
│   └── [all homework 1 files]
├── homework2/
│   └── [all homework 2 files]
└── README.md  # Brief description of implementations
```

---

## 📚 Reference Resources

1. **Gradient Descent**
   - CLRS Chapter 3: Growth of Functions
   - Bishop: Pattern Recognition and Machine Learning

2. **Neural Networks**
   - Nielsen: Neural Networks and Deep Learning (online)
   - Goodfellow et al.: Deep Learning

3. **K-Means**
   - Arthur & Vassilvitskii: k-means++

---

*Laboratory materials prepared for ATP Course*
*Academy of Economic Studies - CSIE Bucharest*
*Alternative learning kit for non-formal education*

# Week 18: Machine Learning Fundamentals in C

## 🎯 Learning Objectives

Upon completion of this laboratory session, students will be able to:

1. **Remember** the fundamental mathematical operations underlying machine learning algorithms, including matrix multiplication, vector operations and gradient computation
2. **Understand** the distinction between supervised and unsupervised learning paradigms, explaining when each approach is appropriate for different problem types
3. **Apply** gradient descent optimisation to train linear and logistic regression models from scratch in C
4. **Analyse** the computational complexity of different ML algorithms, comparing training time, inference time and memory requirements
5. **Evaluate** model performance using appropriate metrics such as mean squared error, R² score, accuracy and cross-entropy loss
6. **Create** complete implementations of fundamental ML algorithms including K-Nearest Neighbours, K-Means clustering and decision trees

---

## 📜 Historical Context

The field of machine learning represents one of the most significant intellectual achievements of the twentieth century, emerging from the convergence of statistics, computer science and cognitive psychology. The term "machine learning" was coined by Arthur Samuel in 1959 whilst working at IBM, where he developed a checkers-playing programme that could improve its performance through experience—a concept that seemed almost magical at the time.

The mathematical foundations trace back to the early work on regression by Carl Friedrich Gauss and Adrien-Marie Legendre in the early 1800s. The method of least squares, developed independently by both mathematicians, provides the theoretical basis for linear regression that remains central to machine learning today. However, the computational implementation of these ideas had to wait nearly two centuries for the advent of electronic computers.

The 1950s and 1960s witnessed the birth of modern neural network research. Frank Rosenblatt's perceptron, introduced in 1958, generated enormous excitement and equally enormous controversy. The subsequent demonstration by Minsky and Papert that single-layer perceptrons could not learn the XOR function led to the first "AI winter"—a period of reduced funding and interest. It was not until the 1980s, with the rediscovery and popularisation of the backpropagation algorithm by Rumelhart, Hinton and Williams, that neural networks regained prominence.

### Key Figure: Frank Rosenblatt (1928–1971)

Frank Rosenblatt was an American psychologist notable for his work in the field of artificial intelligence. Working at the Cornell Aeronautical Laboratory, Rosenblatt invented the perceptron in 1958, one of the first algorithmically described neural networks. His work was groundbreaking in demonstrating that machines could learn from data rather than being explicitly programmed.

Rosenblatt built the Mark I Perceptron, a machine designed for image recognition that used 400 photocells connected to a network of artificial neurons. The machine could learn to distinguish simple shapes, and Rosenblatt boldly predicted that perceptrons would eventually "be able to walk, talk, see, write, reproduce itself and be conscious of its existence."

Tragically, Rosenblatt died in a boating accident on his 43rd birthday, just as the AI winter was beginning. His pioneering contributions were only fully appreciated decades later when deep learning emerged as a dominant paradigm.

> *"The perceptron is not merely a brain model. It is, in effect, an electronic computer, which can be trained to learn specific tasks."*
> — Frank Rosenblatt, *Principles of Neurodynamics* (1962)

### Key Figure: Arthur Samuel (1901–1990)

Arthur Lee Samuel was an American pioneer in the field of computer gaming and artificial intelligence. He coined the term "machine learning" in 1959 whilst working at IBM, defining it as the "field of study that gives computers the ability to learn without being explicitly programmed."

Samuel's checkers-playing programme, developed on an IBM 701, was one of the first successful implementations of a self-improving machine. The programme used rote learning and generalisation, storing board positions and their outcomes to improve its play over time. By the early 1960s, the programme had defeated several skilled human players, providing dramatic evidence that machines could indeed learn.

> *"Programming computers to learn from experience should eventually eliminate the need for much of this detailed programming effort."*
> — Arthur Samuel, *Some Studies in Machine Learning Using the Game of Checkers* (1959)

---

## 📚 Theoretical Foundations

### 1. Supervised vs Unsupervised Learning

Machine learning algorithms fall into two primary categories based on the nature of the training data and the learning task.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          MACHINE LEARNING PARADIGMS                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   SUPERVISED LEARNING                    UNSUPERVISED LEARNING              │
│   ────────────────────                   ──────────────────────             │
│   • Training data includes labels        • No labels provided               │
│   • Goal: learn mapping X → Y            • Goal: discover structure          │
│   • Examples:                            • Examples:                         │
│     - Classification                      - Clustering                       │
│     - Regression                          - Dimensionality reduction         │
│                                           - Anomaly detection                │
│                                                                             │
│   Algorithms:                            Algorithms:                         │
│   • Linear Regression                    • K-Means                           │
│   • Logistic Regression                  • Hierarchical Clustering           │
│   • K-Nearest Neighbours                 • PCA                               │
│   • Decision Trees                       • DBSCAN                            │
│   • Neural Networks                      • Autoencoders                      │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**Supervised learning** involves training a model on labelled examples where both the input features X and the desired output Y are known. The model learns a function f: X → Y that can generalise to new, unseen examples.

**Unsupervised learning** operates on data without labels, seeking to discover hidden patterns or intrinsic structure. The most common unsupervised task is clustering, where similar data points are grouped together.

### 2. Gradient Descent Optimisation

Gradient descent is the workhorse algorithm for training machine learning models. It iteratively adjusts model parameters to minimise a loss function.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          GRADIENT DESCENT ALGORITHM                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   INITIALISE parameters θ randomly                                          │
│                                                                             │
│   REPEAT until convergence:                                                 │
│       gradient ← ∇_θ Loss(θ)           # Compute gradient                  │
│       θ ← θ - α × gradient             # Update parameters                  │
│                                                                             │
│   WHERE:                                                                    │
│       θ = model parameters (weights and biases)                            │
│       α = learning rate (step size)                                         │
│       Loss(θ) = objective function to minimise                              │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

The intuition behind gradient descent is elegantly simple: the gradient of a function points in the direction of steepest ascent. By moving in the opposite direction (negative gradient), we descend towards a minimum.

```
                    Loss
                      │
                  *   │   *
                 *    │    *
                *     │     *
               *      │      *
              *───────┼───────*
             *   ←────│────→   *
            *         │         *
           *──────────┼──────────*
          *           │           *
         *────────────┴────────────*  ← Minimum
                 Parameter θ
```

### 3. Linear Regression

Linear regression models the relationship between features and a continuous target variable as a linear function.

```
Model:      y = w₀x₀ + w₁x₁ + ... + wₙxₙ + b
            y = w·x + b   (vector notation)

Loss (MSE): L = (1/n) × Σᵢ(yᵢ - ŷᵢ)²

Gradients:  ∂L/∂w = (2/n) × Σᵢ xᵢ(ŷᵢ - yᵢ)
            ∂L/∂b = (2/n) × Σᵢ (ŷᵢ - yᵢ)
```

The normal equation provides a closed-form solution:

```
w* = (X^T X)^(-1) X^T y
```

However, gradient descent is preferred when dealing with large datasets or when the matrix inversion is computationally expensive.

### 4. Logistic Regression

Logistic regression extends linear regression to binary classification using the sigmoid function.

```
Sigmoid:    σ(z) = 1 / (1 + e^(-z))

Model:      P(y=1|x) = σ(w·x + b)

Properties of sigmoid:
            ┌─────────────────────────────────────────────────┐
            │     σ(z) ∈ (0, 1)           Always valid prob  │
            │     σ(0) = 0.5              Decision boundary  │
            │     σ(-z) = 1 - σ(z)        Symmetry property  │
            │     dσ/dz = σ(z)(1-σ(z))    Elegant derivative │
            └─────────────────────────────────────────────────┘

Cross-Entropy Loss:
            L = -(1/n) × Σᵢ [yᵢ log(pᵢ) + (1-yᵢ) log(1-pᵢ)]
```

### 5. K-Nearest Neighbours (K-NN)

K-NN is a non-parametric, instance-based learning algorithm that makes predictions based on the K training examples closest to the query point.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              K-NN ALGORITHM                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   PREDICT(query_point, K):                                                  │
│       1. Compute distance to all training points                           │
│       2. Select K nearest neighbours                                        │
│       3. For classification: return majority vote                          │
│          For regression: return mean of K targets                          │
│                                                                             │
│   Distance Metrics:                                                         │
│       Euclidean:  d(a,b) = √(Σᵢ(aᵢ - bᵢ)²)                                 │
│       Manhattan:  d(a,b) = Σᵢ|aᵢ - bᵢ|                                     │
│       Minkowski:  d(a,b) = (Σᵢ|aᵢ - bᵢ|ᵖ)^(1/p)                            │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

K-NN has no training phase—it simply stores all training examples. This makes it a "lazy learner" with O(1) training time but O(n·d) prediction time where n is the number of training samples and d is the dimensionality.

### 6. K-Means Clustering

K-Means partitions n observations into K clusters by iteratively assigning points to the nearest centroid and updating centroids as cluster means.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          K-MEANS (LLOYD'S ALGORITHM)                        │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   INITIALISE centroids μ₁, μ₂, ..., μₖ                                     │
│                                                                             │
│   REPEAT until convergence:                                                 │
│       # Assignment step                                                     │
│       FOR each point xᵢ:                                                    │
│           cᵢ = argminⱼ ||xᵢ - μⱼ||²    # Assign to nearest centroid       │
│                                                                             │
│       # Update step                                                         │
│       FOR each cluster j:                                                   │
│           μⱼ = (1/|Cⱼ|) × Σ_{i∈Cⱼ} xᵢ   # Recompute centroid              │
│                                                                             │
│   Objective (Inertia):                                                      │
│       J = Σⱼ Σ_{i∈Cⱼ} ||xᵢ - μⱼ||²     # Sum of squared distances         │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 7. Decision Trees

Decision trees recursively partition the feature space using axis-aligned splits, creating a tree structure where leaves represent class predictions.

```
Gini Impurity:    G = 1 - Σᵢ pᵢ²

Information Gain: IG(parent, split) = G(parent) - Σⱼ (|Cⱼ|/|parent|) × G(Cⱼ)

Example Tree:
                    [X₀ < 3.5?]
                    /          \
                 Yes            No
                  │              │
            [X₁ < 2.0?]     Class 1
            /        \
         Yes          No
          │           │
       Class 0    Class 1
```

### 8. The Perceptron and Neural Networks

The perceptron is the simplest neural network: a single neuron that computes a weighted sum of inputs and applies a step function.

```
Perceptron:     y = step(w·x + b)
                    where step(z) = 1 if z ≥ 0, else 0

Perceptron Learning Rule:
                IF prediction ≠ true_label:
                    w ← w + η × true_label × x
                    b ← b + η × true_label

Limitation:     Cannot learn XOR (not linearly separable)
```

Multi-layer networks overcome this limitation by stacking layers of neurons:

```
          Input      Hidden        Output
           Layer      Layer         Layer

           x₀ ─────────○─────────┐
                      ↑│↓        │
           x₁ ─────────○─────────┼──────→ ŷ
                      ↑│↓        │
           x₂ ─────────○─────────┘

       Forward pass:  h = σ(W₁·x + b₁)
                      ŷ = σ(W₂·h + b₂)
```

---

## 🏭 Industrial Applications

### Machine Learning in Database Systems

Modern database systems employ machine learning for query optimisation, cardinality estimation and indexing decisions. Systems like PostgreSQL and MySQL use learned cost models to choose optimal query execution plans.

```c
/* Simplified learned cardinality estimator */
double estimate_cardinality(QueryFeatures *features, LearnedModel *model) {
    double base_estimate = linear_predict(model, features);
    return exp(base_estimate);  /* Log-space prediction */
}
```

### Recommendation Systems

E-commerce platforms and streaming services use collaborative filtering and content-based methods to personalise recommendations.

### Anomaly Detection in Network Security

K-Means and K-NN are used to detect network intrusions by identifying traffic patterns that deviate from normal behaviour.

### Financial Prediction

Logistic regression and decision trees power credit scoring systems, where interpretability is crucial for regulatory compliance.

---

## 💻 Laboratory Exercises

### Exercise 1: House Price Prediction with Linear Regression

Implement a complete linear regression pipeline for predicting house prices based on features such as square footage, number of bedrooms and age.

**Requirements:**

- Load training data from CSV file
- Implement feature normalisation (z-score)
- Train linear regression using gradient descent
- Evaluate using MSE and R² metrics
- Visualise learning curve (loss vs iterations)
- Test on holdout data

**File:** `src/exercise1.c`

### Exercise 2: Iris Classification with K-NN and K-Means

Implement both K-NN classification and K-Means clustering on the Iris dataset, comparing the results.

**Requirements:**

- Load Iris dataset (provided in data/)
- Implement K-NN with configurable K
- Implement K-Means with K-Means++ initialisation
- Compare clustering assignments with true labels
- Calculate accuracy for K-NN
- Calculate cluster purity for K-Means

**File:** `src/exercise2.c`

---

## 🔧 Compilation and Execution

```bash
# Build all targets
make

# Run demonstration
make run

# Run specific example
make run-example

# Run automated tests
make test

# Check for memory leaks
make valgrind

# Run in Docker
docker build -t week-18-ml .
docker run --rm week-18-ml

# Clean build artifacts
make clean

# Display help
make help
```

---

## 📁 Directory Structure

```
18-ml-fundamentals/
├── README.md                       # This documentation
├── Makefile                        # Build automation
├── Dockerfile                      # Container build
│
├── slides/
│   ├── presentation-week18.html    # Main lecture slides
│   └── presentation-comparativ.html # Pseudocode/C/Python comparison
│
├── src/
│   ├── example1.c                  # Complete ML demonstration
│   ├── exercise1.c                 # Linear regression exercise
│   └── exercise2.c                 # K-NN and K-Means exercise
│
├── data/
│   ├── housing.csv                 # Housing price dataset
│   ├── iris.csv                    # Iris flower dataset
│   └── classification_2d.csv       # 2D classification data
│
├── tests/
│   ├── test1_input.txt
│   ├── test1_expected.txt
│   ├── test2_input.txt
│   └── test2_expected.txt
│
├── python_comparison/
│   ├── linear_regression_python.py
│   ├── knn_python.py
│   └── kmeans_python.py
│
├── teme/
│   ├── homework-requirements.md    # Homework specifications
│   └── homework-extended.md        # Bonus challenges
│
└── solution/
    ├── exercise1_sol.c
    ├── exercise2_sol.c
    ├── homework1_sol.c
    └── homework2_sol.c
```

---

## 📖 Recommended Reading

### Essential

- **Cormen, Leiserson, Rivest, Stein**: *Introduction to Algorithms*, Chapter 35 (Approximation Algorithms) — provides theoretical foundations for understanding algorithm complexity
- **Bishop, Christopher M.**: *Pattern Recognition and Machine Learning* — comprehensive coverage of probabilistic approaches to ML
- **Hastie, Tibshirani, Friedman**: *The Elements of Statistical Learning* — rigorous statistical treatment, available free online

### Historical

- **Rosenblatt, Frank** (1958): "The Perceptron: A Probabilistic Model for Information Storage and Organization in the Brain", *Psychological Review* 65(6): 386–408
- **Samuel, Arthur** (1959): "Some Studies in Machine Learning Using the Game of Checkers", *IBM Journal* 3(3): 210–229
- **Rumelhart, Hinton, Williams** (1986): "Learning representations by back-propagating errors", *Nature* 323: 533–536

### Online Resources

- [Visualizing K-Means](https://www.naftaliharris.com/blog/visualizing-k-means-clustering/) — Interactive K-Means visualisation
- [Neural Network Playground](https://playground.tensorflow.org/) — Interactive neural network visualisation
- [MIT OpenCourseWare 6.867](https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-867-machine-learning-fall-2006/) — Machine Learning course materials

---

## ✅ Self-Assessment Checklist

After completing this laboratory, verify that you can:

- [ ] Implement matrix multiplication and transpose operations in C
- [ ] Explain gradient descent and implement it for function minimisation
- [ ] Train a linear regression model and evaluate using MSE and R²
- [ ] Implement logistic regression with sigmoid activation
- [ ] Calculate Euclidean and Manhattan distances
- [ ] Implement K-NN classification with configurable K
- [ ] Implement K-Means clustering with proper initialisation
- [ ] Calculate Gini impurity for decision tree splits
- [ ] Explain why perceptrons cannot learn XOR
- [ ] Apply min-max and z-score normalisation to datasets
- [ ] Compare computational complexity of different ML algorithms
- [ ] Choose appropriate algorithms for different problem types

---

## 💼 Interview Preparation

Common interview questions on machine learning fundamentals:

1. **Explain the bias-variance trade-off.**
   - Key insight: High bias leads to underfitting, high variance to overfitting

2. **What is the difference between gradient descent and stochastic gradient descent?**
   - Key insight: SGD uses one sample per update, faster but noisier

3. **Why do we need feature normalisation?**
   - Key insight: Features on different scales dominate gradient updates

4. **How do you choose K in K-Means?**
   - Key insight: Elbow method, silhouette score

5. **Can K-NN be used for regression?**
   - Key insight: Yes, by averaging targets of K neighbours

6. **What is the time complexity of K-NN prediction?**
   - Key insight: O(n·d) without optimisation, O(log n) with KD-trees

7. **Explain overfitting in decision trees and how to prevent it.**
   - Key insight: Pruning, max depth, min samples per leaf

---

## 🔗 Next Week Preview

**Week 19: Algorithms for IoT and Stream Processing**

Having established the foundations of machine learning in pure C, we turn to the challenging domain of IoT and stream processing. Week 19 explores algorithms designed for constrained environments where memory is limited and data arrives continuously. Topics include circular buffers for sensor data, sliding window algorithms for real-time statistics, Kalman filtering for noisy measurements and anomaly detection in data streams. These techniques combine the ML foundations from this week with practical considerations for embedded systems.

---

## 🔌 Real Hardware Extension (Optional)

> **Note:** This section is for students who wish to experiment with physical hardware.
> Arduino kits (ESP32, Arduino Due) are available for borrowing from the faculty library.

The ML algorithms implemented in this laboratory can be deployed on microcontrollers for edge inference. The ESP32 provides sufficient computational power for simple models like linear regression and small decision trees.

**Advantages of Edge ML:**

- Reduced latency (no network round-trip)
- Privacy preservation (data stays local)
- Offline operation capability
- Lower bandwidth requirements

**Getting Started:**

1. Install PlatformIO or Arduino IDE
2. Export trained model weights to header files
3. Implement inference-only code (no training)
4. Optimise for fixed-point arithmetic if needed

Example: Deploy a trained linear regression model for real-time temperature prediction based on sensor readings.

---

*Laboratory materials prepared for ADSC Course*
*Academy of Economic Studies - CSIE Bucharest*
*Alternative learning kit for non-formal education*

---

## 📄 Licence Notice

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                                                                             │
│                        RESTRICTIVE LICENCE NOTICE                           │
│                                                                             │
│  © 2025 All Rights Reserved                                                 │
│                                                                             │
│  This educational material is provided for PERSONAL STUDY ONLY.             │
│                                                                             │
│  PROHIBITED without prior written consent:                                  │
│    • Publication in any form (print, digital, online)                      │
│    • Use in formal educational settings (courses, workshops)               │
│    • Commercial use or redistribution                                       │
│    • Derivative works for teaching purposes                                │
│                                                                             │
│  For licensing enquiries, contact the author.                               │
│                                                                             │
│  Author: Asistent (pe perioadă determinată) ing. dr. Antonio Clim          │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

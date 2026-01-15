# Week 15 Extended Challenges: Algorithmic Paradigms

## 🏆 Bonus Challenges

These optional challenges are for students seeking deeper understanding. Each challenge is worth bonus points that can offset penalties or improve your grade.

---

## Challenge 1: Randomised QuickSort Analysis (15 bonus points)

### Objective

Implement randomised QuickSort and empirically verify its probabilistic guarantees.

### Requirements

1. **Implementation**
   - Random pivot selection using high-quality PRNG
   - Track recursion depth for each run
   - Count comparisons precisely

2. **Analysis**
   - Run 10,000 trials on arrays of size n = 1000
   - Plot histogram of comparison counts
   - Calculate mean, variance, and 95% confidence interval
   - Compare with theoretical expectation: 1.39 n log n

3. **Report**
   - Explain why randomisation eliminates worst-case inputs
   - Discuss the birthday paradox in pivot collision analysis
   - Compare with deterministic median-of-three

### Expected Outcome

Demonstrate that randomised QuickSort achieves O(n log n) with high probability, and identify conditions (if any) that cause deviation.

---

## Challenge 2: Strassen's Matrix Multiplication (20 bonus points)

### Objective

Implement Strassen's algorithm for matrix multiplication and analyse when it outperforms the naive algorithm.

### Background

Standard matrix multiplication: O(n³)
Strassen's algorithm: O(n^2.807) using divide-and-conquer with only 7 recursive calls instead of 8.

### Requirements

1. **Implementation**
   - Naive O(n³) multiplication for baseline
   - Strassen's recursive algorithm
   - Hybrid approach: switch to naive below threshold

2. **Find crossover point**
   - Experimentally determine matrix size where Strassen beats naive
   - Typical crossover: 32 ≤ n ≤ 128

3. **Numerical stability**
   - Analyse floating-point error accumulation
   - Compare precision of both algorithms

### The Seven Products

```
P1 = A11 × (B12 - B22)
P2 = (A11 + A12) × B22
P3 = (A21 + A22) × B11
P4 = A22 × (B21 - B11)
P5 = (A11 + A22) × (B11 + B22)
P6 = (A12 - A22) × (B21 + B22)
P7 = (A11 - A21) × (B11 + B12)

C11 = P5 + P4 - P2 + P6
C12 = P1 + P2
C21 = P3 + P4
C22 = P1 + P5 - P3 - P7
```

---

## Challenge 3: Optimal Binary Search Tree (15 bonus points)

### Objective

Construct a binary search tree that minimises expected search cost given key access probabilities.

### Problem Definition

Given n keys k₁ < k₂ < ... < kₙ with access probabilities p₁, p₂, ..., pₙ, and gaps (unsuccessful searches) with probabilities q₀, q₁, ..., qₙ, construct BST minimising:

```
E[cost] = Σᵢ (depth(kᵢ) + 1) × pᵢ + Σⱼ (depth(gapⱼ) + 1) × qⱼ
```

### Requirements

1. **DP Solution**
   - State: dp[i][j] = minimum cost for keys i..j
   - Root choice: root[i][j] = optimal root for subtree
   - Complexity: O(n³) time, O(n²) space

2. **Tree Construction**
   - Backtrack using root[i][j] table
   - Build actual BST structure

3. **Visualisation**
   - ASCII tree output
   - Show expected search cost

### Example

```
Keys:   k1=10  k2=20  k3=30
Probs:  p1=0.3 p2=0.1 p3=0.2
Gaps:   q0=0.1 q1=0.1 q2=0.1 q3=0.1

Optimal tree:
      20
     /  \
   10    30

Expected cost: 1.7
```

---

## Challenge 4: Maximum Subarray (Kadane's Algorithm) (10 bonus points)

### Objective

Implement three solutions for finding the maximum sum contiguous subarray.

### Requirements

1. **Brute Force** - O(n³)
   - Check all possible subarrays

2. **Divide and Conquer** - O(n log n)
   - Maximum subarray is either:
     - Entirely in left half
     - Entirely in right half
     - Crosses the midpoint

3. **Kadane's Algorithm** - O(n)
   - Dynamic programming with optimal substructure
   - Track current sum and maximum seen

### Extensions

- Handle all-negative arrays
- Return subarray indices, not just sum
- 2D version: maximum sum rectangle in matrix (O(n³) algorithm)

### Example

```
Array: [-2, 1, -3, 4, -1, 2, 1, -5, 4]
Maximum subarray: [4, -1, 2, 1] with sum 6
```

---

## Challenge 5: Matrix Chain Multiplication (15 bonus points)

### Objective

Find the optimal parenthesisation for matrix chain multiplication to minimise scalar multiplications.

### Problem

Given matrices A₁ (p₀ × p₁), A₂ (p₁ × p₂), ..., Aₙ (pₙ₋₁ × pₙ), find parenthesisation that minimises total operations.

Example: A₁(10×30) × A₂(30×5) × A₃(5×60)

```
(A₁ × A₂) × A₃ = 10×30×5 + 10×5×60 = 1500 + 3000 = 4500
A₁ × (A₂ × A₃) = 30×5×60 + 10×30×60 = 9000 + 18000 = 27000
```

Optimal: 4500 operations with ((A₁ × A₂) × A₃)

### Requirements

1. **DP Solution**
   - State: dp[i][j] = minimum cost to multiply Aᵢ..Aⱼ
   - Recurrence: dp[i][j] = min over k of {dp[i][k] + dp[k+1][j] + pᵢ₋₁×pₖ×pⱼ}
   - Complexity: O(n³) time, O(n²) space

2. **Parenthesisation Recovery**
   - Track optimal split point s[i][j]
   - Recursively construct parenthesised expression

3. **Print optimal order**
   - Output like: ((A₁ × A₂) × (A₃ × A₄))

---

## 📤 Submission

Submit bonus challenges separately with filename `bonus_challenge_N.c` where N is the challenge number.

**Grading:** Bonus points are added to your homework score (max 100 + 75 = 175, but capped at 100 for grade calculation, excess serves as buffer against penalties).

---

## 📚 References

1. **CLRS** Chapter 4.2 (Strassen), 15.2 (Matrix Chain), 15.5 (Optimal BST)
2. **Knuth** Volume 3: Searching and Sorting
3. **Bentley** *Programming Pearls*, Column 7 (Maximum Subarray)
4. **Motwani & Raghavan** *Randomised Algorithms* (Quicksort analysis)

---

*Challenge yourself! The best way to learn algorithms is to implement them.*

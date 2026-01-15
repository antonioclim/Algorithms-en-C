# Week 15 Homework: Algorithmic Paradigms

## 📋 General Information

- **Deadline:** End of Week 16
- **Points:** 100 (10% of final grade)
- **Language:** C (C11 standard)
- **Compiler:** GCC with `-Wall -Wextra -std=c11`
- **Submission:** Upload source files to the course portal

---

## 📝 Homework 1: Comprehensive Sorting Library (50 points)

### Description

Create a sorting library that implements all major sorting algorithms covered in the ATP syllabus with performance benchmarking capabilities.

### Requirements

1. **QuickSort with Multiple Pivot Strategies** (10 points)
   - First element pivot
   - Median-of-three pivot
   - Random pivot
   - Allow runtime selection of strategy via function parameter

2. **Shell Sort with Multiple Gap Sequences** (10 points)
   - Shell's original sequence (N/2, N/4, ..., 1)
   - Hibbard sequence (2^k - 1)
   - Knuth sequence ((3^k - 1) / 2)
   - Compare performance across sequences

3. **Counting Sort** (8 points)
   - Handle arrays with elements in range [0, k]
   - Implement stable version
   - Return sorted array without modifying input

4. **Radix Sort** (12 points)
   - LSD (Least Significant Digit) variant for integers
   - MSD (Most Significant Digit) variant for strings
   - Support both decimal and arbitrary base

5. **Bucket Sort** (10 points)
   - For uniformly distributed floating-point numbers in [0, 1)
   - Configurable number of buckets
   - Use insertion sort within buckets

### Benchmarking Requirements

Create a benchmarking framework that:
- Generates test arrays: random, sorted, reverse-sorted, nearly-sorted
- Measures comparisons, swaps, and execution time
- Outputs formatted comparison table

### Example Usage

```bash
./homework1 benchmark 10000
```

### Output Format

```
═══════════════════════════════════════════════════════════════
                    SORTING ALGORITHM BENCHMARK
                         n = 10,000 elements
═══════════════════════════════════════════════════════════════

Random Array:
┌──────────────────────┬────────────────┬──────────┬──────────┐
│ Algorithm            │ Comparisons    │ Swaps    │ Time(ms) │
├──────────────────────┼────────────────┼──────────┼──────────┤
│ QuickSort (first)    │ 148,234        │ 54,321   │ 2.45     │
│ QuickSort (median3)  │ 142,567        │ 51,234   │ 2.31     │
│ QuickSort (random)   │ 145,890        │ 52,789   │ 2.38     │
│ ShellSort (Shell)    │ 234,567        │ 89,012   │ 3.67     │
│ ShellSort (Hibbard)  │ 198,234        │ 76,543   │ 3.12     │
│ ShellSort (Knuth)    │ 187,654        │ 71,234   │ 2.98     │
│ Counting Sort        │ N/A            │ N/A      │ 0.45     │
│ Radix Sort (LSD)     │ N/A            │ N/A      │ 0.78     │
│ Bucket Sort          │ 12,345         │ 5,678    │ 1.23     │
└──────────────────────┴────────────────┴──────────┴──────────┘

[Similar tables for Sorted, Reverse-Sorted, Nearly-Sorted arrays]

═══════════════════════════════════════════════════════════════
```

### File: `homework1_sorting_library.c`

---

## 📝 Homework 2: Algorithm Design Problems (50 points)

### Description

Solve four classic algorithmic problems that demonstrate the application of greedy and dynamic programming paradigms.

### Problem 1: Weighted Activity Selection (15 points)

Unlike the unweighted version where greedy works, weighted activities require DP.

**Input:**
- n activities with start time, finish time, and weight (profit)
- Activities cannot overlap
- Maximise total weight

**Requirements:**
- Sort activities by finish time
- Define dp[i] = max weight using activities 1..i
- Use binary search to find compatible activities
- Reconstruct the optimal selection

**Example:**
```
Activities: (1,4,5), (3,5,1), (0,6,8), (4,7,4), (3,8,6), (5,9,3), (6,10,2), (8,11,4)
Output: Maximum weight = 17, Selected: [(0,6,8), (6,10,2), ...]
```

### Problem 2: Huffman Encoding/Decoding (15 points)

Implement complete Huffman compression and decompression.

**Requirements:**
- Read input text and calculate character frequencies
- Build Huffman tree using priority queue (min-heap)
- Generate prefix codes for each character
- Encode text to binary representation
- Decode binary back to original text
- Calculate compression ratio

**Example:**
```
Input: "abracadabra"
Frequencies: a:5, b:2, r:2, c:1, d:1
Codes: a=0, b=10, r=110, c=1110, d=1111
Encoded: 0101100011100111001100 (22 bits vs 88 bits original)
Compression ratio: 75%
```

### Problem 3: 0/1 Knapsack with Item Reconstruction (10 points)

**Requirements:**
- Implement DP solution for 0/1 knapsack
- Return not just maximum value but the actual items selected
- Handle edge cases: empty knapsack, single item, exact capacity

**Input:**
```
n = 4, capacity = 7
Items: [(weight=1, value=1), (weight=3, value=4), (weight=4, value=5), (weight=5, value=7)]
```

**Output:**
```
Maximum value: 9
Selected items: [1, 2] (indices 1-based)
Total weight: 4 (using weight 3 + 1)
```

### Problem 4: Edit Distance with Alignment (10 points)

Compute the minimum number of operations (insert, delete, replace) to transform one string into another.

**Requirements:**
- Standard DP solution with O(mn) time and space
- Space-optimised version using O(min(m,n)) space
- Output the actual alignment showing operations

**Example:**
```
String 1: "kitten"
String 2: "sitting"
Edit distance: 3

Alignment:
k i t t e n -
| | | | | | |
s i t t i n g
R = = = R = I

Operations: Replace k→s, Replace e→i, Insert g
```

### File: `homework2_algorithm_design.c`

---

## 📊 Evaluation Criteria

| Criterion | Homework 1 | Homework 2 |
|-----------|------------|------------|
| Functional correctness | 25 | 25 |
| Algorithm efficiency | 10 | 10 |
| Code structure and readability | 10 | 10 |
| Memory management | 5 | 5 |
| **Total** | **50** | **50** |

### Penalties

| Issue | Penalty |
|-------|---------|
| Compiler warnings | -5 points per warning (max -10) |
| Memory leaks (Valgrind) | -10 points |
| Crashes on valid input | -15 points |
| Does not compile | -25 points |
| Incorrect complexity | -5 points per algorithm |
| Late submission (per day) | -10 points (max -30) |
| Plagiarism | -50 points + disciplinary action |

---

## 📤 Submission Guidelines

1. **Files to submit:**
   - `homework1_sorting_library.c`
   - `homework2_algorithm_design.c`
   - Any additional header files (if used)
   - Sample input/output demonstrating correctness

2. **Naming convention:**
   - Use exactly the filenames specified
   - Include your student ID in a comment at the top of each file

3. **Testing before submission:**
   ```bash
   # Compile with strict flags
   gcc -Wall -Wextra -std=c11 -o homework1 homework1_sorting_library.c -lm
   gcc -Wall -Wextra -std=c11 -o homework2 homework2_algorithm_design.c -lm
   
   # Check for memory leaks
   valgrind --leak-check=full ./homework1 benchmark 1000
   valgrind --leak-check=full ./homework2 < sample_input.txt
   ```

---

## 💡 Tips for Success

1. **Start with simpler algorithms** — Get Counting Sort and basic QuickSort working before tackling Radix Sort MSD or weighted activity selection.

2. **Test incrementally** — Create small test cases where you can verify the answer by hand.

3. **Handle edge cases** — Empty arrays, single elements, all equal elements, maximum values.

4. **Analyse complexity** — Make sure your implementations achieve the expected time complexity.

5. **Document your code** — Explain the recurrence relation for DP problems and the greedy choice for greedy problems.

6. **Use assertions** — Add `assert()` statements to catch bugs early.

---

## 🆘 Getting Help

- **Office hours:** Wednesday 14:00-16:00
- **Course forum:** Post questions (without sharing solution code)
- **Lab sessions:** Ask teaching assistants during Thursday lab

Good luck! 🍀

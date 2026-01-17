# Week 15 Homework: Algorithmic Paradigms

## Administrative information

- **Deadline:** end of Week 16
- **Weight:** 100 points (10% of the final grade)
- **Implementation language:** ISO C11
- **Expected toolchain:** GCC or Clang with warning flags enabled
- **Submission bundle:** a single archive containing source code, build scripts and a short technical report

You are expected to submit work that is *reproducible*. A submission is reproducible when a third party can compile and run it on a clean machine using only the instructions you provide and when the programme produces the same functional results for the same inputs.

## General constraints

1. Your code must compile without warnings under `-Wall -Wextra -std=c11`.
2. Your programme(s) must fail safely on invalid inputs (negative sizes, truncated files, non-integer tokens and overflow-prone values) by returning a non-zero exit status and printing a clear diagnostic to standard error.
3. Dynamic memory must be released. Memory leaks that appear under Valgrind are penalised.
4. Any randomised behaviour must be documented. If you benchmark randomised algorithms, you must state the seed policy and the number of trials.
5. Your report must be written in British English and must explain the algorithmic choices at the level of invariants and asymptotic reasoning.

## Deliverable A: Comprehensive sorting library (50 points)

### A.1 Objective

Design and implement a compact sorting library that supports multiple comparison-based algorithms and exposes a uniform interface. The intent is to make the *paradigm* explicit: the same interface should permit you to swap between divide and conquer methods and incremental refinement methods without rewriting your benchmarking harness.

### A.2 Required algorithms

You must implement the following algorithms for arrays of integers. Extensions to generic types are encouraged but not required.

1. **MergeSort** (divide and conquer)
   - Implement a top-down MergeSort with an auxiliary buffer.
   - State and respect the merge invariant: at each merge step, two adjacent sorted subarrays are combined into a single sorted subarray.

2. **QuickSort with multiple pivot strategies** (divide and conquer)
   - Strategy 1: last-element pivot (baseline)
   - Strategy 2: median-of-three pivot (first, middle and last)
   - Strategy 3: random pivot
   - The pivot strategy must be selectable at runtime through a function parameter.

3. **Shell sort with multiple gap sequences** (incremental refinement)
   - Shell’s original sequence: `n/2, n/4, …, 1`
   - Hibbard sequence: `2^k − 1`
   - Knuth sequence: `(3^k − 1)/2`
   - Provide a clean mechanism for selecting the gap sequence at runtime.

### A.3 Instrumentation requirements

You must report at least two cost metrics for each algorithm.

- **Key comparisons:** the number of pairwise key comparisons performed
- **Data movement metric:** either swaps (exchanges of two array elements) or moves (assignments) but you must define the metric precisely and use it consistently

Your report must include a short paragraph explaining how the chosen metric relates to the actual cost on modern hardware. For example, swaps are often a proxy for cache line traffic and branch misprediction is often a proxy for irregular control flow.

### A.4 Functional requirements

- Provide a command-line interface that supports:
  - reading an array from standard input
  - generating arrays of specified sizes and distributions
  - selecting an algorithm and a pivot or gap strategy
  - printing the sorted output for correctness runs
  - printing a compact summary for benchmarking runs

- Provide at least four input distributions:
  - uniform random
  - already sorted
  - reverse sorted
  - nearly sorted (a small fraction of elements perturbed)

### A.5 Minimum report content

Your report must contain the following sections.

1. **Algorithmic dossier** for each algorithm, including:
   - pseudocode
   - invariants
   - asymptotic time and space complexity
2. **Benchmark methodology**
   - machine description (CPU model, memory)
   - compiler and flags
   - number of trials and aggregation method (mean, median and dispersion)
3. **Results**
   - tables and at least one plot
   - a short interpretation grounded in the cost model
4. **Threats to validity**
   - a discussion of measurement noise, cache effects and the limits of small sample sizes

### A.6 Marking rubric for Deliverable A

- Correctness and edge-case handling: 20 points
- Interface design and clarity: 10 points
- Instrumentation validity: 10 points
- Benchmark methodology and interpretation: 10 points

## Deliverable B: Greedy algorithms versus dynamic programming (50 points)

### B.1 Objective

Implement paired greedy and dynamic programming solutions for selected problems and identify the boundary conditions under which greedy reasoning is valid.

### B.2 Required problems

1. **Coin change**
   - Implement a greedy algorithm that always takes the largest possible denomination.
   - Implement an unbounded dynamic programming algorithm that computes the minimum number of coins.
   - Provide at least one non-canonical coin system for which greedy is suboptimal.
   - Explain the failure as a violation of the greedy-choice property.

2. **Job sequencing with deadlines**
   - Implement the greedy scheduling algorithm that sorts jobs by profit and schedules each job at the latest available slot before its deadline.
   - Provide an argument for why this greedy strategy is optimal under the model assumptions.

3. **Longest increasing subsequence**
   - Implement the `O(n^2)` dynamic programming algorithm and reconstruct one optimal subsequence.
   - In the report, briefly mention the existence of the `O(n log n)` variant and explain why it is not a greedy algorithm in the usual sense.

### B.3 Minimum report content

Your report for Deliverable B must include:

- a clear statement of the greedy-choice property and optimal substructure
- a counterexample for greedy coin change with a worked trace
- dynamic programming recurrences for coin change and LIS
- complexity analysis (time and space)
- a short reflection on when a greedy approach is appropriate and when it is misleading

### B.4 Marking rubric for Deliverable B

- Correctness and reconstruction: 20 points
- Quality of counterexample and explanation: 10 points
- Dynamic programming formulation and reasoning: 10 points
- Writing quality and structure: 10 points

## Academic integrity

You may consult external references for theory but you must write your code and report yourself. If you reuse code fragments, you must cite the source and you must be able to explain the fragment line by line.

3. **ShellSort with multiple gap sequences** (incremental refinement)
   - Shell's original sequence: ⌊n/2⌋, ⌊n/4⌋, …, 1
   - Hibbard sequence: 2^k − 1
   - Knuth sequence: (3^k − 1) / 2
   - The gap sequence must be selectable at runtime.

### A.3 Required API

Provide a small header file (for example `sortlib.h`) and an implementation file (for example `sortlib.c`). The API must be usable from an external benchmarking programme without editing library internals. The minimum required interface is shown below. You may extend it, but you should not weaken it.

```c
typedef enum {
    PIVOT_LAST = 0,
    PIVOT_MEDIAN3 = 1,
    PIVOT_RANDOM = 2
} pivot_strategy_t;

typedef enum {
    GAP_SHELL = 0,
    GAP_HIBBARD = 1,
    GAP_KNUTH = 2
} gap_sequence_t;

typedef struct {
    long comparisons;
    long swaps;
    double milliseconds;
} sort_stats_t;

void mergesort_int(int *a, int n, sort_stats_t *s);
void quicksort_int(int *a, int n, pivot_strategy_t p, sort_stats_t *s);
void shellsort_int(int *a, int n, gap_sequence_t g, sort_stats_t *s);
```

Your library must define precisely what is counted as a comparison and what is counted as a swap or movement. If you count assignments rather than swaps you must rename the metric accordingly.

### A.4 Benchmarking protocol

Your benchmarking harness must report at least the following.

- problem size `n`
- input distribution (random, sorted, reverse-sorted, nearly sorted and all-equal)
- comparisons and swaps (or comparisons and moves)
- elapsed time measured consistently

You must report results from multiple trials when randomness is involved. You must state how you chose the seed and whether you fixed it for reproducibility.

### A.5 Correctness requirements

- The output array must be sorted in non-decreasing order.
- All algorithms must preserve the multiset of input values.
- MergeSort must be stable. QuickSort and ShellSort need not be stable, but if you claim stability you must justify it.

## Deliverable B: Greedy methods and dynamic programming (50 points)

### B.1 Objective

Implement and analyse two classes of problems that expose the conceptual boundary between greedy choice and global optimisation.

### B.2 Required components

1. **Coin change**
   - Implement a greedy algorithm that repeatedly chooses the largest denomination not exceeding the remaining amount.
   - Implement a dynamic programming algorithm that computes an optimal solution for arbitrary denominations.
   - Provide at least one non-canonical denomination system for which greedy is suboptimal and explain why.

2. **Job sequencing with deadlines**
   - Implement the greedy profit-first scheduler that assigns each job to the latest available time slot before its deadline.
   - Explain the scheduling invariant and the reason the profit-first heuristic is optimal under the stated constraints.

3. **Longest increasing subsequence**
   - Implement the O(n^2) dynamic programming algorithm with reconstruction of the subsequence.
   - State the recurrence and justify the reconstruction procedure.

### B.3 Report requirements

The report is assessed as a technical document rather than as prose. It must contain:

- problem statements and assumptions
- algorithm descriptions using pseudocode
- invariants or proof sketches for correctness
- asymptotic analysis in time and space
- at least one empirical experiment for each deliverable with interpretation

A typical report is 4–8 pages when typeset with sensible spacing. Excessive length without additional substance is not rewarded.

## Marking rubric and penalties

- **Correctness (50%)**: functional correctness on hidden tests, robust handling of edge cases and adherence to interface requirements.
- **Algorithmic fidelity (25%)**: correct implementation of the specified algorithms and appropriate complexity.
- **Engineering quality (15%)**: readability, modularity, meaningful error handling and absence of resource leaks.
- **Report quality (10%)**: clarity of explanation, correctness of asymptotic reasoning and alignment between claims and evidence.

Common penalties include:

- undefined behaviour (for example out-of-bounds access)
- memory leaks
- non-deterministic outputs in test mode without justification
- missing documentation for assumptions or measurement methodology

## Submission checklist

Before submission, ensure that:

1. `make` builds all artefacts on a clean clone.
2. `make test` passes on your machine.
3. Valgrind reports no leaks and no invalid memory accesses.
4. The report contains all required sections and is written in British English.

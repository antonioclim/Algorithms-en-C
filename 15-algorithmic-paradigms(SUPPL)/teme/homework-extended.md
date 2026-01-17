# Week 15 Extended Challenges: Algorithmic Paradigms

## Purpose and assessment model

The tasks in this document are optional and are intended for students who wish to explore the paradigms beyond the minimum assessed material. Each challenge yields bonus points that may compensate for penalties elsewhere, subject to the course rules. Bonus points are awarded only when the work demonstrates technical depth, methodological care and clear exposition.

Unless stated otherwise, all implementations should be written in ISO C11 and should compile without warnings under `-Wall -Wextra -std=c11`. Reports must be written in British English.

## General methodological requirements

For each challenge you attempt, provide:

- a clear problem statement and assumptions
- an algorithmic description using pseudocode
- a complexity discussion in time and space
- an empirical section that states the experimental design (input generation, number of trials, seed policy and statistical summaries)
- a short reflective section explaining what the experiment teaches about the underlying paradigm

## Challenge 1: Randomised QuickSort as a probabilistic algorithm (15 bonus points)

### Objective

Implement randomised QuickSort and empirically validate a probabilistic claim about its cost. The purpose is not merely to obtain small average runtimes, but to demonstrate that a randomised pivot policy turns adversarial input orders into a low-probability event.

### Deliverables

1. **Implementation**
   - A QuickSort implementation with random pivot selection.
   - Instrumentation that records comparisons, swaps and recursion depth.
   - A mode that runs repeated trials on fresh random permutations without printing per-trial transcripts.

2. **Empirical analysis**
   - Perform at least 10,000 trials for array sizes `n ∈ {10^3, 10^4}`.
   - Report the mean, standard deviation and a 95% confidence interval for the comparison count.
   - Plot a histogram of comparison counts and comment on tail behaviour.

3. **Interpretation**
   - Explain why randomisation prevents an adversary from reliably triggering the quadratic case.
   - Discuss how the seed policy affects reproducibility and how you ensured determinism when required.

### Notes

If you cite the well-known approximation that the expected number of comparisons is about `1.386 n log2 n` then you must justify the base of the logarithm and state whether the constant is empirical or derived.

## Challenge 2: Strassen’s matrix multiplication as a divide and conquer optimisation (20 bonus points)

### Objective

Implement Strassen’s algorithm for square matrices and compare it with the classical `O(n^3)` algorithm for moderate sizes. The primary learning outcome is to understand why asymptotic improvements may not dominate for realistic input sizes, particularly when constant factors and memory traffic are considered.

### Deliverables

- A classical multiplication baseline.
- A Strassen implementation with a tunable cutoff `n0` below which the baseline is used.
- A report that discusses both asymptotic complexity and observed performance.

### Experimental design guidance

Use sizes that cover both regimes, for example `n ∈ {64, 128, 256, 512}`. Report results for several cutoff choices and explain the trade-off.

## Challenge 3: Canonical coin systems and the boundary of greedy optimality (15 bonus points)

### Objective

Investigate when greedy coin change is optimal. Your task is to move from isolated counterexamples to a principled characterisation.

### Deliverables

- A programme that searches for amounts up to a bound `B` and detects greedy failures for a fixed coin system.
- At least two non-canonical coin systems where greedy fails and at least one canonical system where it does not fail for the tested range.
- A discussion of why a finite search does not constitute a proof and what additional arguments would be required.

## Challenge 4: Weighted interval scheduling as a greedy failure case (20 bonus points)

### Objective

Implement the weighted interval scheduling problem and show that the unweighted greedy strategy fails. Then implement the dynamic programming solution.

### Deliverables

- Input format and parser for intervals `(start, end, weight)`.
- A greedy baseline (for example earliest finish time) and at least one counterexample.
- A dynamic programming solution with reconstruction of the chosen set of intervals.

## Challenge 5: Edit distance as a dynamic programming archetype (15 bonus points)

### Objective

Implement Levenshtein edit distance and discuss how the DP table encodes optimal substructure.

### Deliverables

- A DP implementation that reports both distance and an alignment trace.
- A space-optimised variant that reduces memory from `O(mn)` to `O(min(m, n))`.
- A short discussion of when the space optimisation is valid and what information it discards.

## Submission guidance

Submit each challenge in a separate directory or with a clear naming convention. Provide a `README` for each challenge describing build steps and usage. If you include plots, embed them in the report or provide scripts that regenerate them.

2. **Analysis**
   - Run at least 10,000 trials for n = 1,000 and n = 10,000.
   - Plot the empirical distribution of comparison counts.
   - Report mean, standard deviation and an empirical 95% confidence interval.
   - Compare the observed mean with the asymptotic expectation E[C_n] ≈ 2n ln n.

3. **Report**
   - Explain why randomisation removes deterministic worst-case inputs.
   - Discuss how the recursion depth behaves under random pivots.
   - State clearly whether your empirical findings are consistent with theory and where discrepancies may originate (measurement overhead, finite-size effects and implementation details).

### Marking guidance

Full credit requires both correct implementation and a statistically literate experimental design. Purely descriptive plots without interpretation will not receive full credit.

## Challenge 2: Strassen matrix multiplication and thresholding (20 bonus points)

### Objective

Implement Strassen's algorithm for square matrices and study the practical cross-over point at which it becomes faster than the naive O(n^3) algorithm for your toolchain and machine.

### Requirements

- Implement both the naive algorithm and Strassen's algorithm.
- Use padding to handle matrix sizes that are not powers of two.
- Implement a threshold parameter t such that for subproblems of size n ≤ t the implementation falls back to the naive algorithm.

### Evaluation

- Provide a plot of runtime against n for several threshold values.
- Identify a sensible threshold regime and justify it.
- Discuss memory use, cache effects and constant factors.

## Challenge 3: Canonical coin systems and greedy optimality (15 bonus points)

### Objective

Investigate when the greedy coin change algorithm is guaranteed to be optimal. Your task is to treat the exercise as an empirical and theoretical investigation rather than as a single counterexample.

### Requirements

- Implement a generator of small coin systems (for example with maximum denomination ≤ 50 and at most 6 denominations).
- For each system, test greedy optimality for all target amounts up to a bound B (for example B = 200) by comparing greedy with dynamic programming.
- Classify the systems into canonical and non-canonical classes under your chosen bound.

### Discussion points

- Explain why testing up to a finite bound is evidence rather than proof.
- Relate your findings to known sufficient conditions (for example certain structured systems) and explain limitations.

## Challenge 4: From O(n^2) to O(n log n) for LIS (15 bonus points)

### Objective

Implement the patience sorting based O(n log n) algorithm for the longest increasing subsequence and compare it empirically with the O(n^2) dynamic programming algorithm.

### Requirements

- Implement both algorithms and ensure that both reconstruct an actual subsequence.
- Provide an experiment that varies n over at least three orders of magnitude.
- Discuss asymptotics and constant factors.

## Submission notes

Extended challenges must be submitted as an addendum to the main homework archive. Clearly separate files for each challenge and ensure that the report indicates which challenges are attempted.

# Week 15: Algorithmic Paradigms (Supplement)

## Scope and pedagogical intent

This laboratory week consolidates the course material by treating algorithm design as a discipline of *paradigms* rather than a catalogue of isolated techniques. The repository is organised around two complementary aims.

1. To make the paradigms operational through concrete implementations in ISO C11.
2. To make the paradigms analysable through explicit instrumentation, deterministic regression tests and an explicit mapping between mathematical recurrences and observable program behaviour.

The delivered code therefore emphasises three properties that are routinely neglected in introductory material but become decisive in professional and research settings.

- **Invariants that can be stated and checked.** Each algorithm is accompanied by a small set of invariants that explain *why* it works and that can be traced at runtime.
- **Cost models that are explicit.** Comparisons and swaps are counted in a defined manner and the implications of that definition are stated.
- **Reproducibility.** Randomised behaviour is made deterministic under non-interactive execution so that regression tests remain stable across runs.

## Repository structure

The repository follows a compact and conventional layout.

- `src/`
  - `example1.c` – complete worked exemplar spanning divide and conquer, greedy methods and dynamic programming
  - `exercise1.c` – instrumented sorting comparison suite
  - `exercise2.c` – boundary exploration between greedy algorithms and dynamic programming
- `tests/` – canonical inputs and expected transcripts used by `make test`
- `data/` – additional inputs for exploratory runs
- `solution/` – reference implementations and extended homework solutions
- `slides/` – teaching slides for the week
- `teme/` – homework specifications

## Building and testing

### Compilation

The project uses a single Makefile with three primary executables:

- `example1`
- `exercise1`
- `exercise2`

To compile everything:

```bash
make
```

### Regression testing

The repository includes deterministic, transcript-based regression tests.

```bash
make test
```

The test harness runs `exercise1` and `exercise2` against fixed inputs and compares stdout against the corresponding files in `tests/` using `diff`. A failure therefore indicates an externally observable behavioural mismatch rather than a stylistic difference.

### Memory safety checks

If Valgrind is installed, the Makefile provides a memory analysis target:

```bash
make valgrind
```

This is particularly relevant for week 15 because both exercises allocate temporary buffers whose lifetimes must be precisely controlled.

## Determinism and the role of randomness

`exercise1.c` includes a randomised pivot strategy for QuickSort. Randomisation is algorithmically meaningful because it suppresses adversarial inputs that force worst-case recursion depth. Randomisation is also a practical hazard for automated assessment because it can destabilise outputs.

To resolve this tension the programme uses a dual seeding policy.

- When standard input is a terminal (interactive use) the PRNG is seeded from the wall clock.
- When standard input is not a terminal (regression execution) the PRNG is seeded with a fixed constant.

This policy is deliberately conservative. It preserves a faithful classroom demonstration of randomised pivot selection while ensuring that `make test` is stable.

## Instrumentation model: comparisons and swaps

### Comparison counter

A **comparison** is counted whenever the programme performs a key comparison that decides ordering. In code this is represented by calls to `compare_counted(a, b)`.

- The counter measures *algorithmic decision cost*.
- It does not count loop-bound checks or integer arithmetic.

### Swap counter

A **swap** is counted when the programme exchanges the values stored in *two distinct memory locations* using `swap_counted(&x, &y)`.

- Swapping an element with itself is treated as a semantic no-op and is therefore not counted.
- This definition is chosen because it correlates with actual data movement and because it eliminates a common instrumentation artefact of Lomuto partitioning where self-swaps dominate the count.

This definition implies that algorithms written as shift-based insertion procedures should not interpret the swap counter as a proxy for assignments. If assignments are to be measured, a separate move counter should be introduced.

## Exercise 1: Sorting algorithm implementation and analysis

`src/exercise1.c` implements three families of sorting strategies and exposes their empirical behaviour through instrumentation.

- **MergeSort**: divide and conquer with stable merging
- **QuickSort**: divide and conquer with pivot-driven partitioning
- **ShellSort**: incremental refinement through gapped insertion

The programme reads an input array, prints it and then runs each algorithm on an independent copy.

### MergeSort

#### Algorithmic paradigm

MergeSort is the canonical example of divide and conquer.

- **Divide**: split the array into two halves.
- **Conquer**: recursively sort each half.
- **Combine**: merge two sorted halves in linear time.

#### Correctness invariants

1. After recursively sorting, the subarrays `A[left..mid]` and `A[mid+1..right]` are each sorted in non-decreasing order.
2. The merge procedure maintains the invariant that `A[left..k-1]` is the sorted merge of the consumed prefixes of the two subarrays.

#### Pseudocode

```text
MERGE_SORT(A, left, right):
    if left >= right:
        return
    mid <- left + floor((right - left) / 2)
    MERGE_SORT(A, left, mid)
    MERGE_SORT(A, mid + 1, right)
    MERGE(A, left, mid, right)

MERGE(A, left, mid, right):
    L <- copy of A[left..mid]
    R <- copy of A[mid+1..right]
    i <- 0, j <- 0
    for k from left to right:
        if j == |R| or (i < |L| and L[i] <= R[j]):
            A[k] <- L[i]; i <- i + 1
        else:
            A[k] <- R[j]; j <- j + 1
```

#### Complexity

Let `T(n) = 2T(n/2) + Θ(n)`. By the Master theorem this solves to `T(n) = Θ(n log n)`.

- Time: `Θ(n log n)` in best, average and worst cases
- Space: `Θ(n)` auxiliary due to the merge buffers
- Stability: yes
- In-place: no

### QuickSort

#### Algorithmic paradigm

QuickSort is also divide and conquer but the structure is defined by a *partition* step rather than a merge step.

- **Divide**: partition the array around a pivot so that elements less than or equal to the pivot are placed on its left and the rest on its right.
- **Conquer**: recursively sort the two partitions.
- **Combine**: trivial, because partitions are already in their final relative position.

#### Partition scheme used

The implementation uses the Lomuto partition scheme with three pivot strategies.

- Strategy 0: last element pivot
- Strategy 1: median-of-three pivot
- Strategy 2: random pivot

#### Partition invariant (Lomuto)

For a subarray `A[low..high]` with pivot `p = A[high]` the partition loop maintains:

- `A[low..i] <= p`
- `A[i+1..j-1] > p`
- `A[j..high-1]` are unclassified

At termination the final swap places the pivot at index `i+1` which is its final sorted position.

#### Pseudocode

```text
PARTITION_LAST(A, low, high):
    pivot <- A[high]
    i <- low - 1
    for j from low to high - 1:
        if A[j] <= pivot:
            i <- i + 1
            swap(A[i], A[j])
    swap(A[i+1], A[high])
    return i + 1

QUICKSORT(A, low, high, strategy):
    if low < high:
        p <- PARTITION(A, low, high, strategy)
        QUICKSORT(A, low, p - 1, strategy)
        QUICKSORT(A, p + 1, high, strategy)
```

#### Complexity and probabilistic guarantee

- Worst case: `Θ(n^2)` comparisons and `Θ(n)` recursion depth
- Average case (under randomised pivot selection): `Θ(n log n)` comparisons and `Θ(log n)` recursion depth in expectation

The randomised pivot strategy is a *distributional defence* against crafted inputs rather than a claim that the algorithm becomes worst-case optimal.

### ShellSort with Hibbard gaps

#### Algorithmic paradigm

ShellSort can be understood as a staged relaxation of the inversion structure of the array.

- Large gaps reduce long-range inversions cheaply.
- Smaller gaps progressively refine local order.
- The final gap 1 reduces to insertion sort but on an array that is typically close to sorted.

#### Hibbard gap sequence

The Hibbard sequence is defined as:

`g_k = 2^k - 1` for `k >= 1`.

For `n = 100` the sequence used is `63, 31, 15, 7, 3, 1`.

#### Pseudocode

```text
HIBBARD_GAPS(n):
    gaps <- []
    g <- 1
    while g < n:
        append(gaps, g)
        g <- 2*g + 1
    return reverse(gaps)

SHELLSORT_HIBBARD(A, n):
    for each gap in HIBBARD_GAPS(n):
        for i from gap to n - 1:
            j <- i
            while j >= gap and A[j-gap] > A[j]:
                swap(A[j], A[j-gap])
                j <- j - gap
```

#### Complexity

ShellSort does not admit a single closed-form complexity bound that is both tight and simple for all gap sequences. For Hibbard gaps the worst-case time is commonly stated as `O(n^{3/2})` while practical behaviour is often closer to `O(n^{4/3})` on random inputs.

For assessment purposes the key point is conceptual rather than asymptotic: the gap sequence is the design lever that mediates the trade-off between number of passes and local work per pass.

## Exercise 2: Greedy algorithms and dynamic programming

`src/exercise2.c` provides a structured comparison between greedy choice and dynamic programming.

- Coin change: greedy vs DP
- Job sequencing with deadlines: greedy
- Longest increasing subsequence: DP

The programme is menu-driven so that each part can be exercised independently.

### Coin change

#### Greedy algorithm

The greedy strategy selects the largest coin denomination that does not exceed the remaining amount.

Greedy is optimal for canonical coin systems such as `{1, 5, 10, 25}` but it is not universally optimal.

#### Dynamic programming algorithm

The DP algorithm solves the unbounded coin change problem by computing the minimum number of coins required to form every intermediate amount up to the target.

- State: `dp[x] =` minimum coins needed to form amount `x`
- Base: `dp[0] = 0`
- Recurrence:

`dp[x] = min_j (dp[x - coin[j]] + 1)` for all `coin[j] <= x`

Backtracking uses a `pick[]` array that records the last coin used in an optimal solution.

#### Demonstrating greedy failure

The repository includes a canonical counterexample.

- Coins: `{1, 3, 4}`
- Amount: `6`

Greedy produces `4 + 1 + 1` (3 coins) while DP produces `3 + 3` (2 coins).

This example is structurally important because it shows that the failure is not an implementation artefact but a property of the choice rule.

### Job sequencing with deadlines

#### Problem statement

Each job has an identifier, a deadline and a profit. Each job takes one unit of time and at most one job can be executed in a time slot. The objective is to maximise total profit.

#### Greedy strategy

1. Sort jobs in descending order of profit.
2. Consider jobs in that order and schedule each job in the latest available slot at or before its deadline.

This strategy is optimal for the unit-time version of the problem.

#### Pseudocode

```text
JOB_SEQUENCING(jobs):
    sort jobs by profit descending
    maxD <- maximum deadline
    schedule[0..maxD-1] <- empty
    profit <- 0
    for job in jobs:
        for slot from min(job.deadline, maxD) - 1 down to 0:
            if schedule[slot] empty:
                schedule[slot] <- job
                profit <- profit + job.profit
                break
    return schedule, profit
```

### Longest increasing subsequence

#### Dynamic programming formulation

The implementation uses the classical `O(n^2)` DP formulation because it is structurally transparent and because it supports direct reconstruction via predecessor pointers.

- State: `dp[i] =` length of the longest increasing subsequence ending at `i`
- Base: `dp[i] = 1`
- Recurrence:

`dp[i] = max(dp[j] + 1)` over all `j < i` such that `A[j] < A[i]`

A `parent[]` array records the predecessor index for reconstruction.

#### Pseudocode

```text
LIS(A):
    for i in 0..n-1:
        dp[i] <- 1
        parent[i] <- -1
    for i in 1..n-1:
        for j in 0..i-1:
            if A[j] < A[i] and dp[j] + 1 > dp[i]:
                dp[i] <- dp[j] + 1
                parent[i] <- j
    end <- argmax_i dp[i]
    reconstruct by following parent from end
```

## Worked exemplar: `src/example1.c`

`example1.c` is a complete, runnable narrative that interleaves code, output and analysis. It contains:

- Divide and conquer: MergeSort and QuickSort
- Recurrence reasoning and Master theorem commentary
- Greedy algorithms: activity selection, Huffman coding and fractional knapsack
- Dynamic programming: multiple Fibonacci variants, 0/1 knapsack and LCS

It is intended as a reference point for style and for the expected density of argument in written reports.

## Notes on the reference solutions

The `solution/` directory contains instructor-oriented implementations. In several places the reference code uses a different instrumentation convention to simplify explanation. The student exercises in `src/` are the normative artefacts for assessment and for regression tests.

## References

The following sources are suitable as primary citations for the algorithms and paradigms used in this repository.

| Topic | APA 7th reference | DOI |
|---|---|---|
| QuickSort | Hoare, C. A. R. (1961). Algorithm 64: Quicksort. *Communications of the ACM, 4*(7), 321. | https://doi.org/10.1145/366622.366644 |
| ShellSort | Shell, D. L. (1959). A high-speed sorting procedure. *Communications of the ACM, 2*(7), 30–32. | https://doi.org/10.1145/368370.368387 |
| Dynamic programming principle | Bellman, R. (1958). On a routing problem. *Quarterly of Applied Mathematics, 16*(1), 87–90. | https://doi.org/10.1090/qam/102435 |
| Longest common subsequence foundations | Hirschberg, D. S. (1975). A linear space algorithm for computing maximal common subsequences. *Communications of the ACM, 18*(6), 341–343. | https://doi.org/10.1145/360825.360861 |


## Instrumentation semantics and methodological cautions

### What is counted as a comparison

A *comparison* is counted when two keys are compared for ordering. In this repository this is implemented by the helper `compare_counted(a, b)` which returns a signed difference and increments a global counter. The counter is therefore a proxy for the number of key comparisons, not for the number of loop guard evaluations nor for the number of arithmetic operations.

Two consequences follow.

1. A loop whose termination depends on an index is not counted as a comparison because it does not compare keys.
2. A comparison that is short-circuited by a guard is also not counted because `compare_counted` is not invoked.

This convention aligns with the standard comparison model used in the analysis of comparison-based sorting.

### What is counted as a swap

A *swap* is counted when two *distinct* array locations exchange values. The helper `swap_counted(&x, &y)` therefore increments the swap counter only when the pointers refer to different addresses.

This is a deliberate choice. Many textbook implementations of Lomuto partitioning call `swap` even when the two indices coincide. Counting such self-swaps would inflate the swap total while reporting no movement of data between distinct locations. Under the present convention the swap counter more closely measures the disruptive component of the algorithm’s behaviour and is therefore more interpretable when relating statistics to cache behaviour and memory traffic.

When you report swap counts in coursework you should state the convention explicitly because different conventions can differ by large constant factors.

### Deterministic execution under test

`exercise1.c` includes a random-pivot QuickSort variant. Randomisation is conceptually valuable but it complicates regression testing because the pivot sequence depends on the pseudo-random generator state. To preserve both properties the programme uses the following policy.

- During interactive execution the generator is seeded from the wall clock.
- During non-interactive execution the generator is seeded with a fixed constant.

The non-interactive case is recognised by `isatty(STDIN_FILENO)`. Under `make test` standard input is redirected from a file and is therefore not a terminal. The fixed seed ensures that the expected transcript is stable and that a failing test is informative rather than stochastic.

A methodological corollary is that timing measurements printed in regression transcripts should not be interpreted as performance evidence. Meaningful benchmarking requires repeated trials, controlled CPU frequency scaling and careful treatment of measurement overhead.

## Exercise 1 dossier: `src/exercise1.c`

### Functional overview

`exercise1.c` reads an integer sequence, prints it and runs five instrumented sorting pipelines.

- MergeSort (divide and conquer)
- QuickSort with last-element pivot (divide and conquer)
- QuickSort with median-of-three pivot (divide and conquer)
- QuickSort with random pivot (randomised divide and conquer)
- ShellSort with Hibbard gaps (incremental improvement through diminishing increments)

Each pipeline is executed on a fresh copy of the original array. After sorting the programme verifies the monotonicity post-condition `arr[i-1] <= arr[i]` for all indices. If the check fails a warning is printed. The transcript is deterministic under non-interactive execution.

### MergeSort

#### Correctness argument as invariants

MergeSort rests on two invariants.

1. *Recursive correctness.* If the recursive calls correctly sort the left and right halves then the merge procedure receives two sorted arrays.
2. *Merge invariant.* At any step of the merge loop the output prefix `arr[left..k-1]` contains exactly the smallest `(k-left)` elements of the multiset union of the two inputs and that prefix is sorted.

The merge invariant is established at `k = left` trivially and maintained because the next element appended is the minimum of the two current heads. Termination yields a sorted output because once one side is exhausted the remainder of the other side is already sorted.

#### Pseudocode with explicit cost hooks

```text
MERGE_SORT(A, left, right):
    if left >= right:
        return
    mid <- left + floor((right-left)/2)
    MERGE_SORT(A, left, mid)
    MERGE_SORT(A, mid+1, right)
    MERGE(A, left, mid, right)

MERGE(A, left, mid, right):
    L <- copy A[left..mid]
    R <- copy A[mid+1..right]
    i <- 0, j <- 0, k <- left
    while i < |L| and j < |R|:
        if COMPARE(L[i], R[j]) <= 0:
            A[k] <- L[i]; i <- i + 1
        else:
            A[k] <- R[j]; j <- j + 1
        k <- k + 1
    append remaining elements of L or R
```

Here `COMPARE` corresponds to `compare_counted` and no swaps are performed.

#### Complexity

Let `T(n)` denote the running time on an array of length `n`.

- Divide: two subproblems of size `n/2`
- Combine: one linear merge pass

Hence `T(n) = 2T(n/2) + Θ(n)` and by the Master theorem `T(n) = Θ(n log n)`. The auxiliary space is `Θ(n)` due to the temporary buffers created in the merge.

### QuickSort

#### Partition invariant (Lomuto)

Let `pivot = A[high]`. During partitioning we maintain that:

- all elements in `A[low..i]` are `<= pivot`
- all elements in `A[i+1..j-1]` are `> pivot`

The invariant is maintained by advancing `j` and swapping the next element into the left region when it satisfies the `<=` predicate. At termination, swapping `A[i+1]` with the pivot places the pivot in its final position.

#### Pivot strategies

- *Last-element pivot:* simplest, analytically convenient but vulnerable to adversarial arrangements.
- *Median-of-three pivot:* reduces the probability of extreme imbalance on partially ordered data by sampling a constant number of candidates.
- *Random pivot:* uses randomisation to eliminate structured worst-case inputs in expectation.

The median-of-three strategy in this repository orders the triple `(low, mid, high)` using three comparisons and up to three swaps then moves the median to position `high` and calls the same Lomuto partition routine.

#### Complexity

QuickSort’s expected cost is `Θ(n log n)` but the worst-case remains `Θ(n^2)` when partitions become maximally unbalanced. The recursion depth is `Θ(log n)` in expectation and `Θ(n)` in the worst case. In practice the pivot strategy heavily influences constants.

### ShellSort with Hibbard gaps

ShellSort generalises insertion sort by allowing exchanges between positions separated by a *gap* `g`. For each gap the array is transformed into `g` interleaved subsequences and each is insertion-sorted.

The Hibbard sequence `g_k = 2^k - 1` has a provable upper bound of `O(n^{3/2})` comparisons for this variant which is asymptotically better than quadratic insertion sort while remaining simple to implement.

The implementation here uses gap-spaced swaps rather than element shifting so that the swap count retains its semantic meaning. This choice tends to increase constant factors compared with shift-based insertion but makes the instrumentation more interpretable.

### Worked trace for the canonical test vector

The regression test uses the input `64 34 25 12 22 11 90`. The following observations are useful when auditing your own instrumentation.

- MergeSort performs 13 key comparisons because each merge step compares heads until one side is exhausted and the recursion tree for `n=7` has merges of sizes `1+1`, `2+2` and `3+4`.
- Lomuto QuickSort performs 19 key comparisons because each partition compares every element except the pivot to the pivot and the recursion partitions the array into sizes `(6,0)`, `(0,5)`, `(3,1)` and `(1,0)`.
- The swap total differs dramatically depending on whether self-swaps are counted. This repository does not count them.

These counts are not universal constants. They are the consequences of a specific input, a specific partition scheme and a stated instrumentation convention.

## Exercise 2 dossier: `src/exercise2.c`

`exercise2.c` is designed to make the greedy-versus-dynamic-programming boundary concrete. The programme is menu-driven, yet every option is implemented as a pure function over arrays so that the logic is testable without interactive state.

### Coin change

#### Greedy algorithm

The greedy algorithm repeatedly takes the largest denomination that does not exceed the remaining amount.

```text
COIN_CHANGE_GREEDY(coins[0..n-1], amount):
    remaining <- amount
    for i from n-1 down to 0:
        take <- floor(remaining / coins[i])
        use[i] <- take
        remaining <- remaining mod coins[i]
    if remaining != 0:
        return IMPOSSIBLE
    return sum_i use[i]
```

The algorithm is correct on *canonical* coin systems but fails on many non-canonical ones. The programme includes a canonical counterexample `coins = {1, 3, 4}, amount = 6`.

#### Dynamic programming algorithm

The dynamic programming algorithm solves the unbounded variant by computing `dp[x] = min_j dp[x - coins[j]] + 1`.

```text
COIN_CHANGE_DP(coins[0..n-1], amount):
    dp[0] <- 0
    for x from 1 to amount:
        dp[x] <- +infty
        pick[x] <- -1
        for j from 0 to n-1:
            if coins[j] <= x and dp[x - coins[j]] != +infty:
                if dp[x - coins[j]] + 1 < dp[x]:
                    dp[x] <- dp[x - coins[j]] + 1
                    pick[x] <- j
    if dp[amount] == +infty:
        return IMPOSSIBLE
    backtrack using pick to reconstruct multiset
    return dp[amount]
```

Time complexity is `Θ(n·amount)` and space complexity is `Θ(amount)`. The reconstruction step is linear in the number of coins used.

### Job sequencing with deadlines

The job sequencing problem is a canonical greedy success story. Sorting by profit and scheduling each job as late as possible yields an optimal solution for the unweighted single-machine variant.

```text
JOB_SEQUENCING(jobs[1..m]):
    sort jobs by profit descending
    maxD <- max deadline
    schedule[0..maxD-1] <- EMPTY
    profit <- 0
    for each job in sorted order:
        for slot from min(job.deadline, maxD)-1 down to 0:
            if schedule[slot] is EMPTY:
                schedule[slot] <- job
                profit <- profit + job.profit
                break
    return profit, schedule
```

The time complexity is `O(m log m + m·maxD)`. For large `maxD` it is common to optimise the inner loop using a disjoint set union structure but the present implementation prioritises clarity.

### Longest increasing subsequence

The longest increasing subsequence is presented as a dynamic programming archetype. The implementation uses the classical quadratic recurrence.

```text
LIS_LENGTH_AND_WITNESS(A[0..n-1]):
    for i in 0..n-1:
        dp[i] <- 1
        parent[i] <- -1
    for i in 1..n-1:
        for j in 0..i-1:
            if A[j] < A[i] and dp[j] + 1 > dp[i]:
                dp[i] <- dp[j] + 1
                parent[i] <- j
    end <- argmax_i dp[i]
    reconstruct by following parent from end
```

The reconstruction is performed in reverse then written into the output array in forward order.

## Cross-language reference fragments

The assessed implementations are in C. The following fragments are intentionally short and are provided only to help students who think bilingually across ecosystems.

### MergeSort in Python (reference only)

```python
def mergesort(a):
    if len(a) <= 1:
        return a
    m = len(a) // 2
    left = mergesort(a[:m])
    right = mergesort(a[m:])
    out = []
    i = j = 0
    while i < len(left) and j < len(right):
        if left[i] <= right[j]:
            out.append(left[i]); i += 1
        else:
            out.append(right[j]); j += 1
    out.extend(left[i:])
    out.extend(right[j:])
    return out
```

### Coin change DP in C++ (reference only)

```cpp
std::vector<int> dp(amount + 1, INF);
std::vector<int> pick(amount + 1, -1);
dp[0] = 0;
for (int x = 1; x <= amount; ++x) {
    for (int j = 0; j < n; ++j) {
        int c = coins[j];
        if (c <= x && dp[x - c] != INF && dp[x - c] + 1 < dp[x]) {
            dp[x] = dp[x - c] + 1;
            pick[x] = j;
        }
    }
}
```

### LIS recurrence in Java (reference only)

```java
for (int i = 0; i < n; ++i) {
    dp[i] = 1;
    parent[i] = -1;
}
for (int i = 1; i < n; ++i) {
    for (int j = 0; j < i; ++j) {
        if (a[j] < a[i] && dp[j] + 1 > dp[i]) {
            dp[i] = dp[j] + 1;
            parent[i] = j;
        }
    }
}
```

These fragments are not optimised and should not be copied verbatim into assessed work without careful adaptation.

## Quality assurance checklist

When reviewing your own solutions, treat the following as a minimum standard.

1. Compile with warnings enabled and resolve all warnings.
2. Run `make test` and ensure the transcripts match exactly.
3. Run `make valgrind` to confirm that no heap memory is leaked.
4. Inspect failure modes by deliberately corrupting inputs (negative sizes, missing data and out-of-range values) and confirm that the programme fails safely.
5. For randomised algorithms, ensure that any report of performance is based on multiple trials and that the seed strategy is documented.


# Week 13 Homework: Weighted Graph Algorithms and Shortest Paths

## 1. Administrative information

### 1.1 Deliverables and deadline

- **Submission window:** end of Week 14 (consult the course platform for the authoritative timestamp)
- **Total marks:** 100 points
- **Implementation language:** ISO C11
- **Compilation environment:** GCC or Clang compatible with `-std=c11 -Wall -Wextra` and `-lm` where required

You must submit exactly two C source files:

- `homework1_multisource.c`
- `homework2_network.c`

Each file must compile as a standalone programme.

### 1.2 Permitted support code

You may reuse and adapt data structures developed in the laboratory, including the adjacency list representation and the binary heap priority queue pattern used for Dijkstra. If you reuse code you must:

- preserve correct memory ownership rules
- preserve asymptotic guarantees
- provide clear attribution in comments at the top of your submission

### 1.3 Quality constraints

Your submissions are assessed for algorithmic correctness and also for engineering quality.

Required properties:

- no memory leaks along exercised code paths
- no undefined behaviour
- input validation for invalid or inconsistent graph parameters
- deterministic output formatting that matches the stated specification

A programme that produces correct numbers but violates the output specification is not considered correct for the purposes of automated testing.

### 1.4 Academic integrity

You must implement your own solution. Similarities beyond standard algorithmic structure may be treated as collusion.

## 2. Homework 1: Multi-source shortest paths (50 points)

### 2.1 Problem statement

You are given a weighted directed graph with non-negative edge weights and a set of K source vertices. Your task is to compute, for every vertex v, the shortest distance from any of the sources to v.

In addition, you must report which source is the closest to v under the computed shortest distance. If multiple sources yield the same shortest distance you must break ties deterministically.

### 2.2 Input format

- Line 1: `V E K`
  - V: number of vertices, labelled 0 to V-1
  - E: number of directed edges
  - K: number of sources
- Line 2: K integers, the source vertices
- Next E lines: `u v w` describing a directed edge u -> v with weight w

Constraints:

- 1 <= V <= 1000
- 0 <= E <= 200000 (choose data structures accordingly)
- 1 <= K <= V
- w is an integer and w >= 0

### 2.3 Output format

For each vertex v in increasing order:

- If v is unreachable from all sources:

```
Vertex v: unreachable
```

- Otherwise:

```
Vertex v: distance = d, nearest source = s
```

Where d is the shortest distance and s is the selected nearest source.

Tie-breaking rule for s:

- If multiple sources yield the same shortest distance to v you must output the smallest source index.

### 2.4 Required algorithm

You must implement multi-source Dijkstra by initialising the priority queue with all sources.

Conceptual pseudocode:

```
MULTISOURCE_DIJKSTRA(G, Sources):
  for each vertex v:
    dist[v] <- INF
    nearest[v] <- -1
  for each s in Sources:
    dist[s] <- 0
    nearest[s] <- s

  PQ <- empty min-priority queue ordered by dist
  for each vertex v:
    PQ.insert(v, dist[v])

  while PQ not empty:
    u <- PQ.extract_min()
    if dist[u] == INF:
      break
    for each edge (u -> x) with weight w:
      cand <- dist[u] + w
      if cand < dist[x]:
        dist[x] <- cand
        nearest[x] <- nearest[u]
        PQ.decrease_key(x, cand)
      else if cand == dist[x] and nearest[u] < nearest[x]:
        nearest[x] <- nearest[u]
```

Correctness hinge:

- Dijkstra’s greedy selection remains valid because all weights are non-negative.
- The tie-breaking for `nearest[]` is a secondary selection criterion and must not violate the primary distance optimality.

### 2.5 Edge cases that must be handled

- K = 1 reduces to standard single-source Dijkstra
- E = 0, graph with isolated vertices
- duplicated sources in input (treat as a set)
- self-loops and parallel edges
- large weights that may overflow 32-bit addition if accumulated naively

Overflow guidance:

- Use a 64-bit accumulator when computing `cand = dist[u] + w` even if final output is within 32-bit range.

### 2.6 Marks breakdown (Homework 1)

- Input parsing and validation: 10
- Correct multi-source Dijkstra implementation: 20
- Correct nearest-source tracking with tie-breaking: 10
- Output specification compliance and determinism: 5
- Memory safety and code quality: 5

## 3. Homework 2: Network delay analysis (50 points)

### 3.1 Problem statement

You are given a directed network of computers with non-negative link delays. A signal originates at a source computer S and propagates through the network along directed links. Your task is to compute:

1. The earliest arrival time at every computer
2. Whether the signal can reach all computers
3. The total propagation time defined as the maximum arrival time among all reachable computers
4. A critical path for the slowest arrival
5. A timing report listing computers in order of arrival

### 3.2 Input format

- Line 1: `N M S`
  - N: number of computers, labelled 0 to N-1
  - M: number of directed links
  - S: source computer
- Next M lines: `a b t` describing a directed link a -> b with delay t milliseconds

Constraints:

- 1 <= N <= 1000
- 0 <= M <= 200000
- t is an integer and t >= 0

### 3.3 Output format

The output must contain the following blocks.

#### Header

```
Network Delay Analysis
======================
Source: Computer S
```

#### Reachability and total time

If at least one computer is unreachable, print:

```
Total propagation time: -1
All computers reachable: NO
```

Otherwise print:

```
Total propagation time: T ms
All computers reachable: YES
```

Where T is the maximum distance.

#### Arrival order report

You must print computers in non-decreasing order of arrival time. If two computers have the same arrival time you must print the smaller computer index first.

Format:

```
Arrival order:
  Computer v: d ms via path
```

For the source:

```
  Computer S: 0 ms (source)
```

For other reachable computers, `path` must be a sequence such as `0 -> 1 -> 3`.

#### Critical path

If all computers are reachable you must print the critical path and its time:

```
Critical path: 0 -> ... -> x (T ms)
```

If not all computers are reachable you must still identify the maximum among the reachable nodes and print:

```
Critical path among reachable nodes: 0 -> ... -> x (T ms)
```

### 3.4 Required algorithm

You must use Dijkstra’s algorithm with a heap-based priority queue.

Conceptual pseudocode:

```
DIJKSTRA(G, S):
  for v:
    dist[v] <- INF
    parent[v] <- -1
  dist[S] <- 0
  PQ <- min-priority queue over vertices by dist

  while PQ not empty:
    u <- extract_min(PQ)
    if dist[u] == INF:
      break
    for each edge (u -> v) with weight w:
      if dist[u] + w < dist[v]:
        dist[v] <- dist[u] + w
        parent[v] <- u
        decrease_key(PQ, v)
```

The timing report requires you to sort vertices by dist. You may either:

- build an array of pairs (vertex, dist) and use `qsort` with a comparator implementing the specified tie-break
- implement a stable counting method if delays are small, but this is optional

### 3.5 Marks breakdown (Homework 2)

- Input parsing and validation: 10
- Correct Dijkstra implementation: 15
- Correct total time and reachability: 10
- Correct path reconstruction and critical path reporting: 10
- Arrival order sorting and output compliance: 5

## 4. Submission checklist

Before submission, confirm all items below:

- both programmes compile with `-Wall -Wextra -std=c11` and link where needed
- output matches the specification exactly on the provided tests
- all allocated memory is freed in all normal exit paths
- integer overflow is handled conservatively
- files contain a short header comment with your name and student identifier


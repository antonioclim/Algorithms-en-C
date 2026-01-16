# Extended challenges for Week 13: shortest paths and graph optimisation

This document defines optional extensions for students who want deeper algorithmic engagement than the core homework requires. Each challenge is assessed as an enrichment task and can contribute bonus credit.

Unless explicitly stated otherwise, all deliverables must:

- be written in ISO C11
- compile with `-std=c11 -Wall -Wextra` and link with `-lm` if you use logarithms
- run deterministically on the provided test harness
- free all allocated memory in normal exit paths

If a challenge requires additional input files you must include them and document the invocation precisely.

## General assessment principles

An extension is credited only if it is both:

- algorithmically correct, meaning it implements the intended algorithm rather than producing numerically plausible output
- empirically justified, meaning you provide at least a minimal experiment that measures the behaviour you claim to optimise

You may reuse and adapt the data structures from the laboratory. If you do so, document the invariants you rely on and how your changes affect time and space complexity.

A strong submission treats correctness, complexity and reproducibility as equal priorities.

## Challenge A: Bidirectional Dijkstra for single-pair queries

### A.1 Motivation

If you need the shortest path between a single source s and destination t in a large sparse graph, standard Dijkstra explores a region whose size is primarily determined by the geometry of shortest path trees rather than by the geometric separation between s and t. Bidirectional Dijkstra reduces the explored state space by running two Dijkstra searches, one forward from s and one backward from t, and terminating when their search frontiers meet.

The algorithm is particularly effective when the graph is roughly undirected or when the reverse graph is readily available.

### A.2 Algorithmic specification

Maintain two sets of labels:

- forward search: dist_f[v], parent_f[v]
- backward search on the reverse graph: dist_b[v], parent_b[v]

Maintain two priority queues ordered by dist_f and dist_b.

Termination is based on a best-known upper bound mu on the s to t distance. Each time a vertex x is extracted in one direction and is already settled in the other direction, update mu = min(mu, dist_f[x] + dist_b[x]). Terminate when min_key(PQ_f) + min_key(PQ_b) >= mu.

Conceptual pseudocode:

```
BIDIRECTIONAL_DIJKSTRA(G, s, t):
  init dist_f, dist_b to INF
  dist_f[s] = 0
  dist_b[t] = 0
  PQ_f.insert(s, 0)
  PQ_b.insert(t, 0)
  mu = INF
  meet = -1

  while PQ_f not empty and PQ_b not empty:
    if PQ_f.min_key() + PQ_b.min_key() >= mu:
      break

    if PQ_f.min_key() <= PQ_b.min_key():
      u = PQ_f.extract_min()
      for each edge (u -> v, w) in G:
        if dist_f[u] + w < dist_f[v]:
          dist_f[v] = dist_f[u] + w
          parent_f[v] = u
          PQ_f.decrease_key(v, dist_f[v])
        if v is settled in backward:
          cand = dist_f[v] + dist_b[v]
          if cand < mu:
            mu = cand
            meet = v
    else:
      u = PQ_b.extract_min()
      for each edge (u -> v, w) in reverse(G):
        if dist_b[u] + w < dist_b[v]:
          dist_b[v] = dist_b[u] + w
          parent_b[v] = u
          PQ_b.decrease_key(v, dist_b[v])
        if v is settled in forward:
          cand = dist_f[v] + dist_b[v]
          if cand < mu:
            mu = cand
            meet = v

  if mu == INF:
    report no path
  else:
    reconstruct path s -> meet using parent_f
    reconstruct path meet -> t using parent_b (reverse direction)
    concatenate
```

### A.3 Deliverables

- a function `bidirectional_dijkstra(Graph *g, int s, int t, ...)` that returns the path and distance
- a driver programme that reads graphs and queries
- a report section in comments describing the termination condition you used and why it is correct

### A.4 Evaluation experiment

Create at least two synthetic graphs:

- a sparse grid-like graph where bidirectional search should help
- a dense graph where the overhead should dominate

Measure visited vertices and extracted-min operations for standard and bidirectional Dijkstra.

## Challenge B: A* search with admissible heuristics

### B.1 Motivation

A* is a best-first search that uses a heuristic h(v) that estimates the remaining distance from v to the goal. In many geometric graphs such as grids or road networks, a well-chosen heuristic reduces exploration dramatically compared with Dijkstra.

### B.2 Requirements

Implement A* for graphs whose vertices have 2D coordinates.

- Define h(v) as either Euclidean distance or Manhattan distance depending on edge model.
- Require that the heuristic is admissible. If you also enforce consistency then A* has particularly clean correctness properties.

Conceptual pseudocode:

```
A_STAR(G, s, t, h):
  for v:
    g_cost[v] = INF
    parent[v] = -1
  g_cost[s] = 0
  PQ ordered by f(v) = g_cost[v] + h(v)
  PQ.insert(s, f(s))

  while PQ not empty:
    u = PQ.extract_min()
    if u == t:
      break
    for each edge (u -> v, w):
      cand = g_cost[u] + w
      if cand < g_cost[v]:
        g_cost[v] = cand
        parent[v] = u
        PQ.decrease_key(v, g_cost[v] + h(v))
```

### B.3 Empirical requirement

Report the number of extracted vertices for A* and Dijkstra on the same instances. Your report must include at least one case where A* and Dijkstra expand identical sets, which typically happens when h is identically zero.

## Challenge C: Floyd–Warshall all-pairs shortest paths

### C.1 Motivation

Floyd–Warshall provides a compact dynamic programming formulation for all-pairs shortest paths. It is asymptotically expensive but conceptually clarifying and is useful for dense graphs with modest vertex counts.

### C.2 Specification

- Implement the standard triple loop dynamic programme for dist[i][j].
- Maintain a next-hop matrix `next[i][j]` so that you can reconstruct any shortest path.
- Detect negative cycles by checking whether dist[i][i] becomes negative.

Pseudocode:

```
FLOYD_WARSHALL(n, w):
  dist = w
  next = init_next_from_edges
  for k in 0..n-1:
    for i in 0..n-1:
      for j in 0..n-1:
        if dist[i][k] + dist[k][j] < dist[i][j]:
          dist[i][j] = dist[i][k] + dist[k][j]
          next[i][j] = next[i][k]
```

### C.3 Engineering requirement

Discuss memory layout. A naive `double **` representation fragments memory. A contiguous allocation improves cache behaviour. You must explain what representation you chose and why.

## Challenge D: Johnson’s algorithm for sparse all-pairs shortest paths

### D.1 Motivation

Johnson’s algorithm combines Bellman–Ford with Dijkstra to compute all-pairs shortest paths in sparse graphs that may contain negative edges but no negative cycles.

### D.2 Core idea

- Add a super-source q with zero-weight edges to all vertices.
- Run Bellman–Ford from q to compute potentials h(v).
- Reweight each edge (u, v) to w'(u, v) = w(u, v) + h(u) - h(v).
- Prove that w' is non-negative.
- Run Dijkstra from each vertex using w', then convert distances back.

### D.3 Deliverable

Implement Johnson’s algorithm as a module and compare its total runtime to Floyd–Warshall on graphs with the same vertex count but varying density.

## Challenge E: K shortest simple paths using Yen’s algorithm

### E.1 Motivation

Many applications require not just the single best path but several high-quality alternatives. Yen’s algorithm computes the K shortest simple paths between a given s and t.

### E.2 Requirements

- Implement the high-level structure of Yen’s algorithm.
- Use Dijkstra as the subroutine.
- Preserve determinism in tie-breaking.

Report:

- K shortest paths
- number of Dijkstra calls
- total runtime

## Challenge F: Arbitrage detection with transaction fees and bid–ask spreads

### F.1 Motivation

Real exchange markets include fees and spreads. A naive arbitrage detector that uses mid-market rates will frequently produce false positives.

### F.2 Specification

Extend Exercise 2 so that each directed edge has an effective rate:

- rate_eff = rate * (1 - fee)

or, for a bid–ask model:

- use bid rates for selling a currency
- use ask rates for buying a currency

You must formalise the model you adopt and show how it maps to the negative-log transformation.

## Challenge G: Property-based testing of shortest path invariants

Implement a generator for random graphs with controlled properties. For each generated instance:

- verify that all parent pointers correspond to real edges
- verify that following parent pointers decreases distance strictly in Dijkstra
- verify that Bellman–Ford detects a negative cycle when one is planted

You must include a deterministic random seed so that results are reproducible.

## Reporting requirements

For any challenge you submit, include a short report as comments at the top of your source file that contains:

- the algorithm implemented and any deviations from canonical presentations
- complexity analysis
- a description of your experiments and the results
- a statement of any limitations

## Marking guidance for extensions

Extensions are graded primarily on correctness and rigour. A typical allocation is:

- 50 percent correctness and completeness
- 30 percent quality of reasoning and invariants
- 20 percent empirical evaluation quality


## Appendix 1: determinism and tie-breaking conventions

When shortest path costs tie, more than one output is correct in a mathematical sense. Automated evaluation nevertheless requires a single canonical transcript. If you extend the laboratory code you should implement explicit tie-breaking policies rather than relying on unspecified iteration order.

Recommended conventions:

- When two vertices have the same key in a priority queue, extract the smaller vertex index first.
- When two candidate relaxations yield the same dist value, prefer the predecessor u with smaller index.
- When printing multiple paths in a report, print vertices in increasing order unless the problem specification requires an alternative.

These conventions do not change the set of correct shortest path distances but they make the parent pointers deterministic which stabilises path reconstruction.

## Appendix 2: data-structure engineering notes

### A2.1 Priority queue design

The laboratory uses a binary min-heap with an auxiliary position map. This design supports:

- extract-min in O(log V)
- decrease-key in O(log V)
- membership queries in O(1)

If you replace the heap you must state which operations your design supports and at what cost. For example:

- a pairing heap offers excellent practical performance with simpler code than a Fibonacci heap
- a radix heap can be advantageous when edge weights are integers in a bounded range

### A2.2 Edge storage

Three edge storage strategies occur frequently:

- adjacency list per vertex, optimal for sparse graphs and neighbour iteration
- adjacency matrix, optimal for dense graphs and O(1) adjacency tests but O(V^2) space
- edge list, convenient for Bellman–Ford and for algorithms that iterate edges globally

A strong extension submission explicitly motivates its choice.

## Appendix 3: experimental methodology

You are expected to provide minimal empirical evidence for performance claims. The following protocol is sufficient for most challenges:

1. Generate a family of graphs parameterised by V and E.
2. For each instance, run the algorithm 10 times and record wall-clock time.
3. Report median and interquartile range.
4. Ensure that input parsing and output printing are excluded from timed regions.

When measuring, prefer `clock_gettime(CLOCK_MONOTONIC, ...)` and compile with optimisation, for example `-O2`, while keeping warnings enabled.

## Appendix 4: suggested literature

The references below provide canonical expositions of the relevant algorithms. You are not required to cite them in your submission but they are useful for understanding formal proofs and for avoiding common pitfalls.

- Dijkstra, E. W. (1959). A note on two problems in connexion with graphs. Numerische Mathematik, 1(1), 269–271. DOI: https://doi.org/10.1007/BF01386390
- Bellman, R. (1958). On a routing problem. Quarterly of Applied Mathematics, 16(1), 87–90. DOI: https://doi.org/10.1090/qam/102435
- Floyd, R. W. (1962). Algorithm 97: Shortest path. Communications of the ACM, 5(6), 345. DOI: https://doi.org/10.1145/367766.368168
- Warshall, S. (1962). A theorem on boolean matrices. Journal of the ACM, 9(1), 11–12. DOI: https://doi.org/10.1145/321105.321107
- Hart, P. E., Nilsson, N. J., & Raphael, B. (1968). A formal basis for the heuristic determination of minimum cost paths. IEEE Transactions on Systems Science and Cybernetics, 4(2), 100–107. DOI: https://doi.org/10.1109/TSSC.1968.300136
- Johnson, D. B. (1977). Efficient algorithms for shortest paths in sparse networks. Journal of the ACM, 24(1), 1–13. DOI: https://doi.org/10.1145/321992.321993


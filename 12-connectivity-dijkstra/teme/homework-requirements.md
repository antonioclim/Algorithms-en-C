# Week 12 Homework: Graph Fundamentals

## Administrative details

- **Deadline:** End of Week 13
- **Weight:** 100 points (10% of final grade)
- **Implementation language:** C (C11)
- **Compiler:** GCC
- **Expected flags:** `-Wall -Wextra -std=c11`

The homework consists of two independent programmes that target the same conceptual material but with different modelling choices: an undirected graph for a social network and a directed graph for task dependencies.

## Submission artefacts

Submit **exactly two C source files**:

1. `homework1_social.c`
2. `homework2_scheduler.c`

Each file must include, at minimum:

- A header comment stating your name, your student ID and the compilation command you used.
- A short description of the approach and the asymptotic complexity of the main operations.
- Robust input validation and deterministic output formatting.

No additional libraries are permitted beyond the C standard library.

## Shared constraints and design expectations

### Graph size bounds

- Homework 1: up to 10,000 vertices and up to 200,000 edges.
- Homework 2: up to 10,000 vertices and up to 200,000 dependencies.

Your representation must therefore be adjacency list based. An adjacency matrix would require on the order of 10,000^2 integers which is not acceptable for memory.

### Memory management

The marking scheme assumes that your submission is valgrind-clean.

- Every allocation must have a corresponding free.
- Destroy routines must be reachable from all error paths.
- Programmes must not leak memory when they terminate early due to invalid input.

### Output determinism

If multiple correct answers exist, your output must still be deterministic. For example, when sorting recommendations you must define a tie-breaker.

## Homework 1: Social Network Analysis (50 points)

### Problem model

Users are vertices. Friendships are undirected edges. A query workload is executed against a fixed network.

### Input format

```
n m
u1 v1
u2 v2
...
q
RECOMMEND user_id
DISTANCE s t
COMMUNITIES
```

- `n` is the number of users (vertices) labelled `0..n-1`.
- `m` is the number of friendships.
- Each friendship line `u v` indicates an undirected connection.
- `q` is the number of queries.

### Required queries and semantics

#### 1. RECOMMEND user_id (15 points)

For a given user `u`, produce friend recommendations based on mutual friends.

- A candidate `c` is recommendable if:
  - `c != u`
  - `c` is not already a friend of `u`
  - `u` and `c` have at least **two** mutual friends
- Recommendations are sorted by:
  1. Mutual friend count descending
  2. Candidate id ascending (tie-breaker)

Suggested algorithmic pattern (two-hop counting)

Rather than computing set intersections for every candidate, count candidates reachable by two steps:

```text
RECOMMEND(u):
  mark all neighbours of u in isFriend[ ]
  mutualCount[ ] <- 0

  for each friend f in Adj[u]:
    for each candidate c in Adj[f]:
      if c == u: continue
      if isFriend[c]: continue
      mutualCount[c]++

  output all c with mutualCount[c] >= 2
  sort by (-mutualCount[c], c)
```

Complexity discussion

- Let deg(x) denote the degree of vertex x.
- The two-hop counting loop costs Theta(sum_{f in N(u)} deg(f)).
- This is typically far smaller than Theta(n * deg(u)) for sparse graphs.

Practical engineering note

- `isFriend` can be a temporary boolean array of length `n` that you reset by storing visited indices in a list.

#### 2. DISTANCE s t (15 points)

Compute degrees of separation using BFS.

- Output both the shortest distance and a concrete path.
- If `t` is unreachable from `s`, output a clear statement and omit the path.

Required output format (illustrative)

```text
Distance from 0 to 42: 3
Path: 0 -> 7 -> 23 -> 42
```

Suggested BFS with predecessor reconstruction

```text
BFS-PATH(s, t):
  dist[ ] <- +infinity
  parent[ ] <- -1
  dist[s] <- 0
  queue <- [s]

  while queue not empty:
    u <- pop front
    if u == t: break
    for v in Adj[u]:
      if dist[v] == +infinity:
        dist[v] <- dist[u] + 1
        parent[v] <- u
        push back v

  if dist[t] == +infinity:
    report unreachable
  else:
    reconstruct by walking parent[t] back to s
```

#### 3. COMMUNITIES (10 points)

Identify connected components in the undirected network.

Minimum requirements

- Output the number of components.
- Output the size of each component.
- Identify the largest component.

Suggested approach

Run BFS or DFS from each unvisited vertex and assign a component id.

```text
COMPONENTS():
  compId[ ] <- 0
  k <- 0
  for v in 0..n-1:
    if compId[v] == 0:
      k++
      BFS-MARK(v, k)
  return k
```

### Marking breakdown

- Graph construction and input validation: 10 points
- Friend recommendations: 15 points
- Distances and path reconstruction: 15 points
- Communities and reporting: 10 points

## Homework 2: Task Scheduler with Dependencies (50 points)

### Problem model

Tasks are vertices in a directed graph. A directed edge `a b` means task `a` must be completed before task `b`.

### Input format

```
n m
pre1 post1
pre2 post2
...
```

### Required capabilities

#### 1. Cycle detection (15 points)

You must detect circular dependencies using the three-colour DFS algorithm.

- If a cycle exists, output the tasks involved.
- Your output must be deterministic. If multiple cycles exist, report the first one encountered under your vertex iteration order.

Pseudocode

```text
HAS-CYCLE(G):
  colour[v] <- WHITE
  parent[v] <- -1

  for v in 0..n-1:
    if colour[v] == WHITE:
      if VISIT(v):
        return true
  return false

VISIT(v):
  colour[v] <- GREY
  for u in Adj[v]:
    if colour[u] == GREY:
      extract cycle by walking parent from v back to u
      return true
    if colour[u] == WHITE:
      parent[u] <- v
      if VISIT(u): return true
  colour[v] <- BLACK
  return false
```

#### 2. Topological ordering (15 points)

If the graph is acyclic, output a topological order.

- DFS post-order reversal is acceptable.
- Kahn's algorithm is also acceptable.

Correctness requirement

For every dependency edge `a -> b`, task `a` must appear before task `b`.

#### 3. Critical path analysis with unit task durations (10 points)

Assume each task takes exactly 1 time unit. Compute:

- The minimum total completion time (critical path length)
- At least one critical chain of tasks that achieves this length

Suggested dynamic programming on a DAG

Let `topo[0..n-1]` be a topological order. Define:

- `dp[v]` = length of the longest path ending at v (in vertices or in edges, but be consistent)
- `pred[v]` = predecessor that maximises dp

Algorithm

```text
CRITICAL-PATH(G, topo):
  dp[v] <- 1
  pred[v] <- -1

  for v in topo:
    for each edge v -> u:
      if dp[v] + 1 > dp[u]:
        dp[u] <- dp[v] + 1
        pred[u] <- v

  T <- max_v dp[v]
  end <- argmax_v dp[v]
  reconstruct chain by walking pred[end]
  return (T, chain)
```

### Marking breakdown

- Input model and validation: 10 points
- Cycle detection and reporting: 15 points
- Topological ordering: 15 points
- Critical path: 10 points

## Evaluation criteria and penalties

| Criterion | Points |
|---|---:|
| Functional correctness | 40 |
| Correct use of graph algorithms | 25 |
| Edge-case handling | 15 |
| Code quality and documentation | 10 |
| No compiler warnings under the expected flags | 10 |

Penalties

- Compiler warnings: -10 points
- Memory leaks (valgrind): -20 points
- Crashes on valid input: -30 points
- Plagiarism: automatic zero and disciplinary escalation

## Testing guidance

You are expected to construct both micro-tests and stress tests.

Micro-tests

- Single vertex, no edges
- Two vertices, one edge
- Disconnected graph with several components
- Graph with high-degree hub

Stress tests

- Random sparse graphs with controlled degree distribution
- Adversarial recommendation queries focused on high-degree vertices

A good submission contains test generators (not submitted) and evidence that your programme is stable under diverse inputs.

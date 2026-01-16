# Week 12: Graph Fundamentals, Connectivity and Shortest Paths

## Abstract

This laboratory introduces graphs as concrete data structures in C and as abstract objects in discrete mathematics. The focus is deliberately foundational: representation choices, traversal strategies and the inferential tasks that become possible once a graph can be explored systematically. You will implement algorithms that are, in effect, the assembly language of modern graph computation: breadth-first search for level-order exploration and shortest paths in unweighted networks, depth-first search for structural decomposition, cycle detection and topological ordering. The accompanying exercises are engineered to make algorithmic reasoning explicit and to make memory discipline unavoidable.

A secondary aim is methodological. By treating traversal procedures as constructive proofs, you will develop the habit of stating invariants, separating specification from implementation and validating behaviour through regression tests.

## Learning outcomes

After completing this week you should be able to:

1. Formalise graphs as ordered pairs \(G = (V, E)\) and reason about paths, cycles and connectedness.
2. Choose between adjacency matrices and adjacency lists by analysing asymptotic costs and constant factors.
3. Implement BFS and use its predecessor map to reconstruct shortest paths in unweighted graphs.
4. Implement DFS and use its recursion stack semantics to detect directed cycles.
5. Produce a topological ordering of a directed acyclic graph and explain why reverse post-order is sufficient.
6. Demonstrate correct dynamic memory management in C for pointer-rich graph structures.

## Repository map

The directory layout is deliberately simple so that attention stays on algorithmic structure rather than build tooling.

- `src/`
  - `example1.c` — complete reference demonstration covering representations, BFS, DFS, connected components, cycle detection and topological sorting.
  - `exercise1.c` — BFS shortest-path queries on an undirected graph.
  - `exercise2.c` — DFS traversal, cycle detection and topological sort on a directed graph.
- `data/`
  - `graph_small.txt`, `graph_medium.txt` — small undirected graphs for exploratory runs and debugging.
- `tests/`
  - `test1_input.txt`, `test1_expected.txt` — regression test for Exercise 1.
  - `test2_input.txt`, `test2_expected.txt` — regression test for Exercise 2.
- `teme/`
  - `homework-requirements.md` — assessed specifications for two homework problems.
  - `homework-extended.md` — optional extension challenges.
- `slides/`
  - `presentation-week12.html`, `presentation-comparativ.html` — lecture slides in HTML.

## Mathematical preliminaries

### Graph models

A graph is a pair \(G = (V, E)\) where \(V\) is a finite set of vertices and \(E\) is a set of edges.

- **Undirected graph:** each edge is an unordered pair \(\{u, v\}\).
- **Directed graph:** each edge is an ordered pair \((u, v)\) which may be read as a one-way dependency.

A **walk** is a sequence \(v_0, v_1, \dots, v_k\) such that each consecutive pair is linked by an edge. A **path** is a walk with no repeated vertices. A **cycle** is a non-empty walk where \(v_0 = v_k\) and all other vertices are distinct.

Connectivity is defined for undirected graphs: a graph is connected if every pair of vertices is linked by at least one path. A **connected component** is a maximal connected subgraph.

For directed graphs, the analogous notion is strong connectivity. This laboratory uses directed graphs primarily for dependency structures where cycles represent contradictions.

### Representations

Two representations dominate introductory implementations.

#### Adjacency matrix

An adjacency matrix is a \(|V| \times |V|\) array \(A\) where \(A[u][v] = 1\) if an edge exists from \(u\) to \(v\) and \(0\) otherwise.

- Space: \(\Theta(|V|^2)\)
- Edge query: \(\Theta(1)\)
- Neighbour iteration: \(\Theta(|V|)\)

This representation is conceptually clean and constant-time for adjacency tests but becomes prohibitive for sparse graphs.

#### Adjacency list

An adjacency list stores, for each vertex \(u\), a list of neighbours \(N(u)\). In C this laboratory represents lists as singly linked lists.

- Space: \(\Theta(|V| + |E|)\)
- Edge query: \(\Theta(\deg(u))\) in the worst case
- Neighbour iteration: \(\Theta(\deg(u))\)

The list representation is typically superior for sparse graphs and is therefore used in the exercise code.

### Complexity conventions

Throughout, \(|V|\) denotes the number of vertices and \(|E|\) the number of edges. When adjacency lists are used, BFS and DFS run in \(\Theta(|V| + |E|)\) time because each vertex is enqueued or pushed a bounded number of times and each adjacency list edge is scanned once.

## Exercise 1: BFS shortest paths in an unweighted undirected graph

### Specification

You are given an undirected graph and a set of queries \((s, t)\). For each query you must print one shortest path from \(s\) to \(t\) measured in number of edges.

The key requirement is not merely distance computation but path recovery.

### BFS as a shortest-path algorithm

In an unweighted graph, every edge has the same cost. BFS explores vertices in non-decreasing distance from the source. This is precisely the condition needed for shortest paths.

#### Pseudocode

```text
BFS-SHORTEST-PATHS(G, s):
  for each vertex v in V:
    dist[v]   <- +infinity
    parent[v] <- -1

  dist[s] <- 0
  Q <- empty queue
  enqueue(Q, s)

  while Q is not empty:
    u <- dequeue(Q)
    for each v in Adj[u]:
      if dist[v] == +infinity:
        dist[v]   <- dist[u] + 1
        parent[v] <- u
        enqueue(Q, v)

  return (dist, parent)
```

The algorithm yields a BFS tree rooted at \(s\) where `parent[v]` is the predecessor of `v` on a shortest path.

#### Path reconstruction

```text
RECONSTRUCT-PATH(parent, s, t):
  if parent[t] == -1 and t != s:
    return "no path"

  path <- empty list
  x <- t
  while x != -1:
    append x to path
    x <- parent[x]

  reverse(path)
  return path
```

### Implementation notes

- The queue is implemented as a fixed-capacity array sized to `MAX_VERTICES`. Under BFS, each vertex is enqueued at most once so the queue cannot overflow if the input respects the vertex bound.
- Adjacency lists are built by inserting each new neighbour at the head. This makes the implementation compact and deterministic which is important for regression tests that validate not only distances but also the exact printed path.

### Complexity

- Time: \(\Theta(|V| + |E|)\) per distinct source vertex used in queries.
- Space: \(\Theta(|V| + |E|)\) for the adjacency lists plus \(\Theta(|V|)\) for `dist`, `parent` and queue.

### Language sketches

The core algorithm is language-agnostic. The following sketches exist purely to reinforce the idea that correctness lives at the level of invariants.

#### C (schematic)

```c
void bfs(const Graph *g, int s, int dist[], int parent[]) {
    for (int v = 0; v < g->n; v++) { dist[v] = INF; parent[v] = -1; }
    Queue q; queue_init(&q);
    dist[s] = 0;
    queue_enqueue(&q, s);
    while (!queue_empty(&q)) {
        int u = queue_dequeue(&q);
        for (AdjNode *p = g->adj[u]; p; p = p->next) {
            int v = p->vertex;
            if (dist[v] == INF) {
                dist[v] = dist[u] + 1;
                parent[v] = u;
                queue_enqueue(&q, v);
            }
        }
    }
}
```

#### Python (schematic)

```python
from collections import deque

def bfs(adj, s):
    n = len(adj)
    dist = [None] * n
    parent = [-1] * n
    q = deque([s])
    dist[s] = 0
    while q:
        u = q.popleft()
        for v in adj[u]:
            if dist[v] is None:
                dist[v] = dist[u] + 1
                parent[v] = u
                q.append(v)
    return dist, parent
```

#### C++ (schematic)

```cpp
vector<int> dist(n, INF), parent(n, -1);
queue<int> q;
dist[s] = 0;
q.push(s);
while (!q.empty()) {
    int u = q.front(); q.pop();
    for (int v : adj[u]) {
        if (dist[v] == INF) {
            dist[v] = dist[u] + 1;
            parent[v] = u;
            q.push(v);
        }
    }
}
```

#### Java (schematic)

```java
int[] dist = new int[n];
int[] parent = new int[n];
Arrays.fill(dist, INF);
Arrays.fill(parent, -1);
ArrayDeque<Integer> q = new ArrayDeque<>();
dist[s] = 0;
q.add(s);
while (!q.isEmpty()) {
    int u = q.remove();
    for (int v : adj.get(u)) {
        if (dist[v] == INF) {
            dist[v] = dist[u] + 1;
            parent[v] = u;
            q.add(v);
        }
    }
}
```

## Exercise 2: DFS traversal, cycle detection and topological sorting

### DFS traversal as a graph decomposition

DFS constructs a forest of depth-first trees by exploring each vertex as deeply as possible before backtracking.

#### Pseudocode

```text
DFS-FULL(G):
  visited[v] <- false for all v
  for v from 0 to |V|-1:
    if not visited[v]:
      DFS-VISIT(G, v)

DFS-VISIT(G, v):
  visited[v] <- true
  output v
  for each u in Adj[v]:
    if not visited[u]:
      DFS-VISIT(G, u)
```

### Cycle detection via the three-colour method

For directed graphs, DFS carries an additional semantic layer: the recursion stack represents the current dependency chain. A back edge to an ancestor signals a cycle.

Colour states:

- `WHITE`: undiscovered
- `GREY`: discovered and currently active on the recursion stack
- `BLACK`: fully processed

#### Pseudocode

```text
HAS-CYCLE(G):
  colour[v] <- WHITE for all v
  parent[v] <- -1 for all v
  for v from 0 to |V|-1:
    if colour[v] == WHITE:
      if VISIT-CYCLE(G, v):
        return true
  return false

VISIT-CYCLE(G, v):
  colour[v] <- GREY
  for each u in Adj[v]:
    if colour[u] == GREY:
      report cycle by walking parent pointers from v back to u
      return true
    if colour[u] == WHITE:
      parent[u] <- v
      if VISIT-CYCLE(G, u):
        return true
  colour[v] <- BLACK
  return false
```

### Topological sorting by reverse post-order

A topological ordering exists if and only if the directed graph is acyclic. In a DAG, if DFS pushes a vertex onto a stack after exploring all its descendants then popping the stack yields a valid topological ordering.

#### Pseudocode

```text
TOPOLOGICAL-SORT(G):
  if HAS-CYCLE(G):
    return failure
  visited[v] <- false for all v
  S <- empty stack
  for v from 0 to |V|-1:
    if not visited[v]:
      TOPO-VISIT(G, v, visited, S)
  return pop-all(S)

TOPO-VISIT(G, v, visited, S):
  visited[v] <- true
  for each u in Adj[v]:
    if not visited[u]:
      TOPO-VISIT(G, u, visited, S)
  push(S, v)
```

### Complexity

- DFS traversal: \(\Theta(|V| + |E|)\)
- Cycle detection: \(\Theta(|V| + |E|)\)
- Topological sort: \(\Theta(|V| + |E|)\)

## Example programme

`src/example1.c` is a complete, runnable demonstration. It builds small graphs in memory, prints both matrix and list representations and then illustrates the algorithms in a narrative order suitable for live teaching.

## Building and testing

The Makefile aims to be explicit rather than clever.

```sh
make
make run        # demonstration programme
make run-ex1     # Exercise 1 using regression input
make run-ex2     # Exercise 2 using regression input
make test        # regression tests with diffs
```

The regression tests are intentionally strict. They validate not only correctness of distances and acyclicity but also output formatting and the determinism of the recovered path.

## Engineering notes: correctness and memory

### Determinism

Traversal order in adjacency-list graphs depends on neighbour list order. This repository inserts new neighbours at the head of each list, so the neighbour order is the reverse of the input edge order. The tests are written with that convention in mind.

### Memory discipline

Every adjacency list node is allocated exactly once and freed exactly once. The destroy routines traverse each list, free each node and reset pointers to `NULL`. You should treat these routines as part of the functional specification, not as an afterthought.


## Correctness sketches

### Breadth-first search and shortest paths

BFS is often presented as a traversal routine yet for unweighted graphs it is also a shortest-path algorithm. The point is not that BFS happens to work but that its control structure enforces a discipline on discovery that mirrors the structure of shortest paths.

Key invariants (informal but operational):

1. When a vertex v is first discovered, dist[v] is assigned exactly once and it is equal to dist[parent[v]] + 1.
2. The queue always contains vertices in non-decreasing dist order. More precisely, if x appears in the queue before y then dist[x] <= dist[y].
3. When a vertex u is dequeued, dist[u] is final: no later discovery can produce a strictly shorter path to u.

Correctness argument sketch:

- Existence: For every discovered vertex v, the parent pointers define a concrete path from the source s to v whose length equals dist[v]. This follows by construction because parent pointers decrease distance by exactly 1.
- Optimality: Suppose, for contradiction, that v is first discovered with dist[v] > delta(s, v) where delta is the true shortest path length. Consider a shortest path P from s to v and let w be the predecessor of v on P. Then delta(s, w) = delta(s, v) - 1. By induction on path length, w must be discovered with dist[w] = delta(s, w). When w is dequeued, the algorithm inspects edge (w, v) and would discover v with dist[w] + 1 = delta(s, v), contradicting the assumption that v was first discovered with a larger value.

These two steps jointly show dist[v] = delta(s, v) for all reachable vertices and that the reconstructed path is a shortest path.

### Cycle detection with the three-colour method

The three-colour method should be read as a formalisation of the recursion stack.

- WHITE vertices have not been visited.
- GREY vertices are precisely those that are on the active call stack of DFS-VISIT.
- BLACK vertices have returned from DFS-VISIT, meaning all outgoing edges have been explored.

Invariant: At any moment during the DFS, the GREY vertices form a simple directed path in the DFS tree rooted at the current DFS start vertex. Each GREY vertex (except the root of that DFS tree) has a well-defined parent pointer to the caller.

When an edge v -> u is scanned and u is GREY, u is an ancestor of v in the active recursion chain so there exists a directed path u -> ... -> v via parent pointers and v -> u closes a directed cycle. Conversely, if the graph contains a directed cycle then a DFS starting from any vertex on the cycle will eventually expose a back edge to a GREY ancestor, because one vertex on the cycle must be revisited before the cycle has been fully unwound.

### Topological ordering by reverse post-order

Let G be a directed acyclic graph. Consider a DFS that pushes each vertex onto a stack after exploring all descendants. When there is an edge u -> v, DFS cannot finish u before it finishes v, because exploring u entails exploring v unless v has already been finished in an earlier DFS tree. In either case, the finishing time of u is later than the finishing time of v. Therefore, if vertices are listed in decreasing finishing time then u appears before v for every edge u -> v which is exactly the definition of a topological ordering.

## Worked trace: BFS on the regression graph

The regression instance for Exercise 1 contains vertices 0..5 and edges:

- 0-1, 0-2
- 1-2, 1-3
- 2-4
- 3-4, 3-5

A BFS from source 0 proceeds as follows when adjacency lists are built by head insertion:

1. Start: dist[0] = 0, queue = [0]
2. Dequeue 0: discover 2 then 1, queue = [2, 1]
3. Dequeue 2: discover 4, queue = [1, 4]
4. Dequeue 1: discover 3, queue = [4, 3]
5. Dequeue 4: no new discoveries, queue = [3]
6. Dequeue 3: discover 5, queue = [5]
7. Dequeue 5: done

The parent pointers yield:

- parent[1] = 0, parent[3] = 1, parent[5] = 3 so path 0 -> 1 -> 3 -> 5
- parent[4] = 2, parent[2] = 0 so path 0 -> 2 -> 4

This trace clarifies a subtle but important point: BFS may discover vertices in many orders yet the distance labels are forced by the queue discipline.

## Common implementation pitfalls

1. Forgetting to reinitialise dist and parent arrays when the BFS source changes.
2. Using an adjacency matrix and then forgetting that neighbour iteration is Theta(|V|) per vertex, which becomes quadratic even for sparse graphs.
3. Printing paths in reverse because parent pointers point towards the source.
4. Treating the GREY state as equivalent to visited and thereby losing the ability to distinguish tree edges from back edges.
5. Omitting graph destruction routines, which hides memory leaks until the code is executed under a leak checker.

## Appendix: Dijkstra as the weighted analogue of BFS

BFS is sufficient for unweighted graphs because all edges contribute the same increment to path length. For weighted graphs with non-negative weights, the appropriate generalisation is Dijkstra's algorithm.

Conceptual shift:

- BFS maintains a FIFO queue and expands vertices in increasing number of edges.
- Dijkstra maintains a min-priority queue keyed by tentative distance and expands vertices in increasing current best distance.

Pseudocode (non-negative weights):

```text
DIJKSTRA(G, s):
  for each vertex v:
    dist[v] <- +infinity
    parent[v] <- -1
  dist[s] <- 0
  PQ <- min-priority-queue of (dist[v], v)
  insert (0, s) into PQ

  while PQ not empty:
    (d, u) <- extract-min(PQ)
    if d > dist[u]:
      continue
    for each edge (u, v) with weight w:
      if dist[u] + w < dist[v]:
        dist[v] <- dist[u] + w
        parent[v] <- u
        insert (dist[v], v) into PQ

  return (dist, parent)
```

The algorithm is structurally similar to BFS: a frontier data structure, a discovery rule and a parent map for reconstruction. The difference is the ordering discipline enforced by the data structure.

## Further directions

- Weighted shortest paths require relaxation rather than level-order exploration. Dijkstra's algorithm can be viewed as BFS generalised from unit weights to non-negative weights by replacing the FIFO queue with a priority queue and by relaxing edges in increasing tentative distance order.
- For DAG scheduling problems, longest-path computations become tractable via dynamic programming in topological order.
- Strongly connected components, articulation points and bridges arise naturally once you begin to ask what structural information DFS discovery times encode.

## References

| Reference (APA 7th) | DOI |
|---|---|
| Dijkstra, E. W. (1959). A note on two problems in connexion with graphs. *Numerische Mathematik, 1*(1), 269–271. | https://doi.org/10.1007/BF01386390 |
| Tarjan, R. E. (1972). Depth-first search and linear graph algorithms. *SIAM Journal on Computing, 1*(2), 146–160. | https://doi.org/10.1137/0201010 |
| Kahn, A. B. (1962). Topological sorting of large networks. *Communications of the ACM, 5*(11), 558–562. | https://doi.org/10.1145/368996.369025 |
| Lee, C. Y. (1961). An algorithm for path connections and its applications. *IRE Transactions on Electronic Computers, EC-10*(3), 346–365. | https://doi.org/10.1109/TEC.1961.5219222 |

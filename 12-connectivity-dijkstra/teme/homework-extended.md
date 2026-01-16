# Extended challenges for Week 12

## Purpose and scope

The assessed exercises for this week emphasise BFS, DFS, cycle detection and topological sorting. The challenges in this document broaden that surface area in a principled way. Each challenge is designed to force you to articulate a specification, select an appropriate graph representation and defend complexity claims.

These tasks are optional. They are best treated as research-oriented mini-projects: start with a clear statement of inputs and outputs, write invariants before code and validate behaviour on adversarial instances.

## Marking policy for optional challenges

Each fully correct challenge: **+10 bonus points**

A solution is considered fully correct only if it:

- Compiles cleanly with `-Wall -Wextra -std=c11`
- Produces correct results on hidden tests
- Manages memory correctly (no leaks and no invalid accesses)
- Contains explanatory comments that state invariants and justify key design decisions

---

## Challenge 1: Bipartite detection and odd-cycle certificates

### Problem

Given an undirected graph, determine whether it is bipartite. If it is bipartite, output a two-colouring. If it is not bipartite, output an explicit odd cycle certificate.

### Core method

The standard approach is BFS-based two-colouring. Each connected component is processed independently.

### Pseudocode

```text
IS-BIPARTITE(G):
  colour[v] <- UNCOLOURED for all v
  parent[v] <- -1 for all v

  for v in V:
    if colour[v] == UNCOLOURED:
      colour[v] <- 0
      BFS-COLOUR(v)
      if conflict found:
        return (false, ODD-CYCLE)

  return (true, COLOURING)

BFS-COLOUR(s):
  Q <- [s]
  while Q not empty:
    u <- pop front
    for w in Adj[u]:
      if colour[w] == UNCOLOURED:
        colour[w] <- 1 - colour[u]
        parent[w] <- u
        push back w
      else if colour[w] == colour[u]:
        record conflict edge (u, w)
        reconstruct odd cycle using parent pointers
```

### Notes on odd-cycle reconstruction

When the conflict edge (u, w) is found, both u and w are at the same parity level relative to the component root. The odd cycle can be obtained by tracing parent pointers from u and w to their least common ancestor.

### Suggested deliverable

- `challenge1_bipartite.c`

---

## Challenge 2: Strongly connected components and condensation graphs

### Problem

Given a directed graph, compute its strongly connected components (SCCs) and build the condensation graph in which each SCC is contracted to a single vertex. The condensation graph must be a DAG.

### Acceptable algorithms

- Kosaraju–Sharir (two DFS passes, requires transpose graph)
- Tarjan (single DFS with lowlink values)

### Pseudocode sketch: Kosaraju–Sharir

```text
KOSARAJU(G):
  S <- empty stack
  visited[v] <- false
  for v in V:
    if not visited[v]:
      DFS-FINISH(G, v, visited, S)

  GT <- TRANSPOSE(G)
  visited[v] <- false
  component_id <- 0
  while S not empty:
    v <- pop S
    if not visited[v]:
      component_id++
      DFS-COLLECT(GT, v, visited, component_id)

  return component assignment
```

### Deliverables

- `challenge2_scc.c`

---

## Challenge 3: Articulation points and bridges in undirected graphs

### Problem

Compute all articulation points (cut vertices) and bridges (cut edges). Provide discovery times, low values and a concise explanation of why each reported vertex or edge is critical.

### Core insight

A DFS tree encodes enough information to decide whether an edge is a bridge and whether a vertex is an articulation point via lowlink values.

### Key conditions

- An edge (u, v) where u is the parent of v in the DFS tree is a bridge if low[v] > disc[u].
- A non-root vertex u is an articulation point if it has a child v with low[v] >= disc[u].
- A root vertex is an articulation point if it has at least two DFS tree children.

### Deliverable

- `challenge3_articulation.c`

---

## Challenge 4: Graph diameter, radius and centres

### Problem

For an undirected graph, compute eccentricity for every vertex then report the diameter, radius and the set of centre vertices. For disconnected graphs, use an explicit representation of infinity.

### Algorithmic baseline

The straightforward approach is to run BFS from every vertex. This is O(|V| * (|V| + |E|)) and is acceptable only for modest n. If you want to go beyond, consider heuristics for diameter estimation or multi-source techniques.

### Deliverable

- `challenge4_diameter.c`

---

## Challenge 5: Graph serialisation, persistence and integrity checking

### Problem

Design a binary file format to store a graph efficiently then implement save and load functions. Include integrity checking via a checksum and validate that corrupted files are detected.

### Minimum format elements

- Magic number and version
- Vertex count and edge count
- Flags for directed or undirected and weighted or unweighted
- Edge list as pairs (or triples if weighted)
- CRC32 or another well-specified checksum

### Deliverable

- `challenge5_serialise.c`

---

## Challenge 6: Dijkstra's algorithm with path reconstruction

### Problem

Implement single-source shortest paths on a directed or undirected graph with non-negative edge weights. Output distances and reconstruct one shortest path to each queried target.

This challenge is intentionally aligned with the week theme: Dijkstra is the weighted analogue of BFS.

### Data structure requirement

Use a binary heap (priority queue). You may implement your own heap or use a carefully justified alternative. The key property is extract-min in logarithmic time.

### Pseudocode

```text
DIJKSTRA(G, s):
  dist[v] <- +infinity
  parent[v] <- -1
  dist[s] <- 0
  PQ <- heap
  push (0, s)

  while PQ not empty:
    (d, u) <- extract-min
    if d != dist[u]:
      continue
    for each (u, v, w) in Adj[u]:
      if dist[u] + w < dist[v]:
        dist[v] <- dist[u] + w
        parent[v] <- u
        push (dist[v], v)

RECONSTRUCT(parent, s, t):
  if dist[t] == +infinity:
    return no path
  follow parent pointers from t to s
  reverse the collected sequence
```

### Deliverable

- `challenge6_dijkstra.c`

---


---

## Challenge 7: Minimum spanning trees in weighted undirected graphs

### Problem

Given a connected undirected graph with non-negative edge weights, compute a minimum spanning tree (MST) and report its total weight and selected edges. If the graph is disconnected, compute a minimum spanning forest.

### Acceptable algorithms

- Prim using a min-priority queue (conceptually close to Dijkstra)
- Kruskal using a global edge sort and a disjoint-set union structure

### Pseudocode sketch: Prim

```text
PRIM(G, r):
  for each vertex v:
    key[v] <- +infinity
    parent[v] <- -1
  key[r] <- 0
  PQ <- heap of (key[v], v)

  while PQ not empty:
    u <- extract-min
    for each edge (u, v, w):
      if v still in PQ and w < key[v]:
        parent[v] <- u
        key[v] <- w
        decrease-key(PQ, v, w)
```

### Deliverable

- `challenge7_mst.c`

---

## Challenge 8: Offline connectivity queries with disjoint-set union

### Problem

You are given an undirected graph that evolves by edge insertions. Between insertions you receive queries of the form CONNECTED(u, v). Answer each query in near-constant amortised time.

This challenge is included because it sharpens the distinction between traversal-based connectivity (BFS or DFS) and structure-maintenance connectivity (union-find). Both are conceptually about components but their algorithmic affordances differ.

### Core data structure

Disjoint-set union with path compression and union by rank or size.

### Pseudocode

```text
MAKE-SET(x):
  parent[x] <- x
  rank[x] <- 0

FIND(x):
  if parent[x] != x:
    parent[x] <- FIND(parent[x])
  return parent[x]

UNION(x, y):
  rx <- FIND(x)
  ry <- FIND(y)
  if rx == ry:
    return
  if rank[rx] < rank[ry]:
    parent[rx] <- ry
  else if rank[rx] > rank[ry]:
    parent[ry] <- rx
  else:
    parent[ry] <- rx
    rank[rx] <- rank[rx] + 1
```

### Deliverable

- `challenge8_unionfind.c`

---

## Recommended reading

If you want a principled path beyond the lecture notes, start with the original papers. They are short, precise and they illustrate how algorithmic ideas were first expressed.

- Dijkstra (shortest paths and the discipline of greedy exploration)
- Tarjan (DFS as a unifying primitive)
- Kahn (topological ordering as a practical scheduling procedure)


## Submission guidance

Submit each challenge as a separate C file and include a short `README.txt` describing:

- The chosen representation and why it is appropriate
- Complexity claims in terms of |V| and |E|
- How you tested the code, including at least one adversarial case


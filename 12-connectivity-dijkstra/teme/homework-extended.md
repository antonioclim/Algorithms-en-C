# Extended Challenges - Week 12

## 🚀 Advanced Challenges (Optional)

Each correctly solved challenge: **+10 bonus points**

These challenges extend beyond the core curriculum and are designed for students seeking deeper understanding of graph algorithms. They require independent research and creative problem-solving.

---

## ⭐ Challenge 1: Bipartite Graph Detection (Difficulty: Medium)

### Description

A bipartite graph is one whose vertices can be divided into two disjoint sets such that every edge connects a vertex in one set to a vertex in the other. Implement a program that determines whether a given undirected graph is bipartite.

### Requirements

1. Read an undirected graph from input
2. Use BFS-based two-colouring algorithm:
   - Start from any unvisited vertex, colour it with colour 0
   - Colour all neighbours with colour 1
   - Continue alternating colours; if any neighbour has the same colour as current vertex, graph is not bipartite
3. Handle disconnected graphs (check all components)
4. If bipartite, output the two vertex sets
5. If not bipartite, output an odd-length cycle as proof

### Example Output
```
Graph is bipartite!
Set A: {0, 2, 4, 6}
Set B: {1, 3, 5, 7}

-- or --

Graph is NOT bipartite!
Odd cycle found: 0 -> 1 -> 2 -> 0
```

### Theoretical Background
- A graph is bipartite if and only if it contains no odd-length cycles
- Applications: job assignments, matching problems, conflict-free scheduling

### Bonus Points: +10

---

## ⭐ Challenge 2: Strongly Connected Components (Difficulty: Hard)

### Description

In a directed graph, a strongly connected component (SCC) is a maximal set of vertices such that there is a path from every vertex to every other vertex within the set. Implement Kosaraju's algorithm or Tarjan's algorithm to find all SCCs.

### Requirements

1. Implement one of:
   - **Kosaraju's Algorithm**: Two DFS passes with graph transposition
   - **Tarjan's Algorithm**: Single DFS with low-link values
2. Output each SCC as a group of vertices
3. Construct the condensation graph (DAG of SCCs)
4. Handle graphs with cycles and multiple SCCs

### Algorithm Sketch (Kosaraju's)
```
1. Perform DFS on original graph, recording finish times
2. Create transpose graph (reverse all edges)
3. Perform DFS on transpose in decreasing finish time order
4. Each DFS tree in step 3 is an SCC
```

### Example Output
```
Found 4 strongly connected components:
  SCC 1: {0, 1, 2}
  SCC 2: {3, 4}
  SCC 3: {5, 6, 7}
  SCC 4: {8}

Condensation graph edges:
  SCC1 -> SCC2
  SCC2 -> SCC3
  SCC3 -> SCC4
```

### Bonus Points: +10

---

## ⭐ Challenge 3: Articulation Points and Bridges (Difficulty: Hard)

### Description

An articulation point (or cut vertex) is a vertex whose removal disconnects the graph. A bridge is an edge whose removal disconnects the graph. These are critical for analysing network vulnerability.

### Requirements

1. Implement Tarjan's algorithm for finding articulation points
2. Extend to find all bridges
3. For each articulation point, report how many components result from its removal
4. Provide a "network vulnerability score" based on the number of critical points

### Key Concepts
```c
/*
 * For each vertex u, track:
 *   - disc[u]: Discovery time in DFS
 *   - low[u]: Lowest discovery time reachable from u's subtree
 *
 * u is an articulation point if:
 *   - u is root of DFS tree with 2+ children, OR
 *   - u is not root and has child v where low[v] >= disc[u]
 *
 * Edge (u,v) is a bridge if:
 *   - low[v] > disc[u]
 */
```

### Example Output
```
Articulation points: 2, 5
  Removing vertex 2 creates 3 components
  Removing vertex 5 creates 2 components

Bridges: (2,3), (5,6)

Network vulnerability: HIGH (2 critical points, 2 bridges)
```

### Bonus Points: +10

---

## ⭐ Challenge 4: Graph Diameter and Centre (Difficulty: Medium)

### Description

The diameter of a graph is the maximum shortest path distance between any two vertices. The centre consists of vertices that minimise the maximum distance to all other vertices (eccentricity).

### Requirements

1. Compute the eccentricity of each vertex (max distance to any other vertex)
2. Find the diameter (maximum eccentricity)
3. Find the radius (minimum eccentricity)
4. Identify all centre vertices (vertices with eccentricity = radius)
5. Handle disconnected graphs (infinite distances)

### Algorithm
```
For each vertex u:
    Run BFS from u
    eccentricity[u] = max(distance to all v)

diameter = max(eccentricity)
radius = min(eccentricity)
centre = {v : eccentricity[v] == radius}
```

### Example Output
```
Eccentricity of each vertex:
  Vertex 0: 4
  Vertex 1: 3
  Vertex 2: 3
  Vertex 3: 4
  Vertex 4: 3
  Vertex 5: 4

Diameter: 4 (between vertices 0 and 5)
Radius: 3
Centre vertices: {1, 2, 4}

Peripheral vertices (eccentricity = diameter): {0, 3, 5}
```

### Bonus Points: +10

---

## ⭐ Challenge 5: Graph Serialisation and Persistence (Difficulty: Medium)

### Description

Implement a complete graph persistence system that can save graphs to binary files and reload them efficiently. This simulates real-world scenarios where graph data must be stored and retrieved.

### Requirements

1. **Binary Format Design** (define your own format):
   - Magic number for file identification
   - Header with metadata (vertex count, edge count, graph type)
   - Compact edge storage

2. **Save Function**:
   - Serialise adjacency list to binary file
   - Include checksum for data integrity

3. **Load Function**:
   - Deserialise binary file back to adjacency list
   - Verify checksum and report corruption

4. **Comparison with Text Format**:
   - Measure file size reduction vs text format
   - Measure load time improvement

### Example Format
```
Bytes 0-3:   Magic number (0x47524150 = "GRAP")
Bytes 4-7:   Version number
Bytes 8-11:  Number of vertices (n)
Bytes 12-15: Number of edges (m)
Bytes 16-19: Flags (directed=bit0, weighted=bit1)
Bytes 20+:   Edge data (pairs of 4-byte integers)
Last 4:      CRC32 checksum
```

### Example Output
```
Saved graph to 'network.grp'
  Text format size:  145,234 bytes
  Binary format size: 48,012 bytes
  Compression ratio: 67% smaller

Loaded graph from 'network.grp'
  Checksum verified: OK
  Load time: 0.003s (vs 0.045s for text)
  Vertices: 10000, Edges: 50000
```

### Bonus Points: +10

---

## 📊 Bonus Point System

| Challenges Completed | Total Bonus |
|---------------------|-------------|
| 1 | +10 points |
| 2 | +20 points |
| 3 | +30 points |
| 4 | +40 points |
| All 5 | +50 points + "Graph Master" badge 🏆 |

---

## 📤 Submission Guidelines

1. Submit each challenge as a separate file:
   - `challenge1_bipartite.c`
   - `challenge2_scc.c`
   - `challenge3_articulation.c`
   - `challenge4_diameter.c`
   - `challenge5_serialise.c`

2. Include a `README.txt` explaining your approach for each challenge

3. Bonus challenges are due one week after the main homework deadline

---

## 📚 Recommended Resources

1. **Bipartite Graphs**: CLRS Chapter 22.2, Hungarian Algorithm
2. **Strongly Connected Components**: CLRS Chapter 22.5, Kosaraju & Tarjan papers
3. **Articulation Points**: Tarjan's original 1972 paper "Depth-First Search and Linear Graph Algorithms"
4. **Graph Centre**: Network analysis literature, centrality measures
5. **Serialisation**: Protocol Buffers documentation, binary file I/O in C

---

## 💪 Motivational Note

These challenges represent algorithms used daily in industry: Google's web graph analysis uses SCCs, network engineers use articulation points for fault tolerance, and social networks compute graph centres for influence measurement.

Completing even one challenge demonstrates advanced algorithmic thinking that employers value highly. Good luck! 🚀

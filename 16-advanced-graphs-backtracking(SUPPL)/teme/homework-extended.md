# Week 16: Extended Homework — Bonus Challenges

## Overview

These bonus challenges are optional and worth up to 50 additional points. They explore advanced topics beyond the core curriculum and are suitable for students seeking deeper understanding or preparing for competitive programming.

---

## Challenge 1: Minimum Spanning Arborescence (10 points)

### Problem

Implement Edmonds' algorithm (also known as Chu-Liu/Edmonds' algorithm) for finding the minimum spanning arborescence in a **directed** graph.

### Background

While MST algorithms work for undirected graphs, the directed version (finding a minimum cost tree rooted at a specific vertex that reaches all other vertices) requires a more sophisticated approach.

### Requirements

- Implement the full Edmonds' algorithm
- Handle the contraction of cycles
- Reconstruct the actual arborescence
- Support negative edge weights

### Example

```
Input:
4 5
0 1 1
0 2 4
1 2 2
2 3 3
3 1 5

Output:
Minimum Spanning Arborescence (root 0):
  0 -> 1 (cost: 1)
  0 -> 2 (cost: 4)
  2 -> 3 (cost: 3)
Total cost: 8
```

### Reference

- Edmonds, J. (1967). "Optimum branchings." *Journal of Research of the National Bureau of Standards*

---

## Challenge 2: 2-SAT Solver using SCC (10 points)

### Problem

Implement a 2-SAT (2-Satisfiability) solver using Strongly Connected Components.

### Background

2-SAT is a special case of the Boolean satisfiability problem where each clause has exactly 2 literals. Unlike general SAT (NP-complete), 2-SAT can be solved in polynomial time using the implication graph and SCC.

### Requirements

- Build implication graph from clauses
- Find SCCs using Kosaraju or Tarjan
- Determine satisfiability from SCC structure
- Extract a satisfying assignment if one exists

### Example

```
Input (CNF format):
4 4
1 2
-1 3
-2 -3
4 -1

Output:
Satisfiable: Yes
Assignment: x1=F, x2=T, x3=T, x4=T
```

### Explanation

For clause (a ∨ b), create implications ¬a → b and ¬b → a. The formula is satisfiable iff no variable and its negation are in the same SCC.

---

## Challenge 3: Exact Cover (Algorithm X) (10 points)

### Problem

Implement Donald Knuth's Algorithm X for the exact cover problem using Dancing Links (DLX).

### Background

Exact cover asks: given a collection of subsets, select subsets that partition the universe (each element appears exactly once). Many combinatorial problems reduce to exact cover: Sudoku, N-Queens, Pentominoes, etc.

### Requirements

- Implement the sparse matrix representation
- Implement the Dancing Links technique
- Solve standard exact cover problems
- Apply to Sudoku solving for comparison with basic backtracking

### Example

```
Input:
Universe: {1, 2, 3, 4, 5, 6, 7}
Subsets:
  A = {1, 4, 7}
  B = {1, 4}
  C = {4, 5, 7}
  D = {3, 5, 6}
  E = {2, 3, 6, 7}
  F = {2, 7}

Output:
Exact Cover: {B, D, F}
```

### Reference

- Knuth, D. (2000). "Dancing Links." *Millenial Perspectives in Computer Science*

---

## Challenge 4: Traveling Salesman (Branch and Bound) (10 points)

### Problem

Implement a branch-and-bound solution for the Traveling Salesman Problem (TSP).

### Background

TSP asks for the shortest route visiting all cities exactly once and returning to the start. While NP-hard, branch and bound with good lower bounds can solve moderately-sized instances optimally.

### Requirements

- Implement MST-based lower bound
- Implement 1-tree relaxation bound (bonus)
- Use priority queue for best-first search
- Compare with naive backtracking

### Example

```
Input:
5
0 10 15 20 25
10 0 35 25 30
15 35 0 30 20
20 25 30 0 15
25 30 20 15 0

Output:
Optimal Tour: 0 -> 1 -> 3 -> 4 -> 2 -> 0
Total Distance: 80
Nodes Explored: 23 (vs 120 for brute force)
```

### Metrics

Track and report:
- Nodes explored
- Nodes pruned
- Maximum queue size
- Time to optimal solution

---

## Challenge 5: Maximum Independent Set (10 points)

### Problem

Find the maximum independent set in a graph using backtracking with advanced pruning.

### Background

An independent set is a set of vertices with no edges between them. Finding the maximum independent set is NP-hard, but intelligent pruning makes moderate instances tractable.

### Requirements

- Implement basic backtracking
- Add pruning based on upper bound (e.g., greedy colouring gives upper bound)
- Implement vertex ordering heuristics (maximum degree first)
- Compare performance with/without pruning

### Example

```
Input:
6 7
0 1
0 2
1 2
1 3
2 4
3 4
4 5

Output:
Maximum Independent Set: {0, 3, 5}
Size: 3
Nodes Explored: 45
Nodes Pruned: 78
```

### Bonus

Implement the Bron-Kerbosch algorithm for finding all maximal independent sets (which is equivalent to finding all maximal cliques in the complement graph).

---

## Submission Guidelines

1. **Format:** Separate file for each challenge: `bonus1.c`, `bonus2.c`, etc.
2. **Documentation:** Include complexity analysis in comments
3. **Testing:** Provide test cases demonstrating correctness
4. **Deadline:** Same as regular homework

## Grading

Each challenge is graded independently:
- **Correctness:** 60%
- **Efficiency:** 25%
- **Code Quality:** 15%

Partial credit is available for partially working solutions with good documentation.

---

## Resources

### Books
- Papadimitriou & Steiglitz, *Combinatorial Optimization*
- Korte & Vygen, *Combinatorial Optimization: Theory and Algorithms*

### Online
- [Stanford CS261](https://theory.stanford.edu/~trevisan/cs261/) - Optimization lecture notes
- [Competitive Programming 3](https://cpbook.net/) - Implementation details

---

*These challenges are designed to extend your algorithmic thinking and prepare you for advanced courses and competitive programming.*

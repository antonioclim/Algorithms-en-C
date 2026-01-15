# Week 16: Homework Assignments

## Overview

This week's homework consists of two assignments totalling 100 points:
- **Homework 1:** Graph Algorithms Suite (50 points)
- **Homework 2:** Backtracking Applications (50 points)

---

## Homework 1: Graph Algorithms Suite (50 points)

### Description

Implement a comprehensive graph algorithms library that solves real-world network problems.

### Requirements

#### Part A: Minimum Cost Network (15 points)

Implement a network design tool that finds the minimum cost to connect all nodes.

**Input Format:**
```
N M
city1 city2 cost
city1 city3 cost
...
```

Where N is the number of cities, M is the number of possible connections.

**Output Format:**
```
Minimum Cost Network:
  Connection 1: CityA -- CityB (cost: X)
  Connection 2: CityC -- CityD (cost: Y)
  ...
Total Cost: Z
```

**Requirements:**
- Use Kruskal's algorithm with Union-Find (8 pts)
- Support city names (not just integers) (3 pts)
- Display MST edges in sorted order by cost (2 pts)
- Handle disconnected graphs gracefully (2 pts)

#### Part B: Course Prerequisite Scheduler (15 points)

Implement a course scheduler that produces a valid course order respecting prerequisites.

**Input Format:**
```
N M
CourseA
CourseB
...
CourseX CourseY  (means X is prerequisite for Y)
```

**Output Format:**
```
Valid Course Order:
  1. CourseA
  2. CourseB
  ...

Cycle detected: No valid order exists
```

**Requirements:**
- Implement both DFS and Kahn's topological sort (8 pts)
- Detect cycles and report appropriately (4 pts)
- Support course names with descriptions (3 pts)

#### Part C: Network Reliability Analysis (20 points)

Analyse a network to identify critical points of failure.

**Input Format:**
```
N M
node1 node2
node1 node3
...
```

**Output Format:**
```
Network Reliability Analysis:
  Articulation Points: A, B, C
  Bridges: (X, Y), (P, Q)
  
Network is 2-edge-connected: Yes/No
Network is 2-vertex-connected: Yes/No
```

**Requirements:**
- Find all articulation points using DFS (8 pts)
- Find all bridges using low-link values (8 pts)
- Determine connectivity properties (4 pts)

### Submission

- Single file: `homework1.c`
- Command-line interface: `./homework1 <mode> <input_file>`
  - Modes: `mst`, `topo`, `reliability`

---

## Homework 2: Backtracking Applications (50 points)

### Description

Implement backtracking solutions for classic constraint satisfaction problems.

### Requirements

#### Part A: Knight's Tour (15 points)

Find a Knight's Tour on an N×N chessboard.

**Requirements:**
- Implement basic backtracking solution (5 pts)
- Implement Warnsdorff's heuristic for efficiency (5 pts)
- Support both open and closed tours (3 pts)
- Visualise the tour with move numbers (2 pts)

**Example Output:**
```
Knight's Tour (8x8):
  ┌────┬────┬────┬────┬────┬────┬────┬────┐
  │  1 │ 38 │ 55 │ 34 │  3 │ 36 │ 19 │ 22 │
  ├────┼────┼────┼────┼────┼────┼────┼────┤
  │ 54 │ 47 │  2 │ 37 │ 20 │ 23 │  4 │ 17 │
  ...
```

#### Part B: Cryptarithmetic Solver (15 points)

Solve cryptarithmetic puzzles like SEND + MORE = MONEY.

**Input Format:**
```
SEND MORE MONEY
```

**Output Format:**
```
Solution:
  S=9, E=5, N=6, D=7, M=1, O=0, R=8, Y=2
  
Verification:
    9567
  + 1085
  ------
   10652 ✓
```

**Requirements:**
- Implement general solver for any puzzle (8 pts)
- Ensure leading digits are non-zero (3 pts)
- Count and display all solutions (2 pts)
- Implement pruning optimisations (2 pts)

#### Part C: Optimal Scheduling (20 points)

Solve a job scheduling problem with deadlines and profits.

**Input Format:**
```
N
job_name deadline profit
job_name deadline profit
...
```

**Output Format:**
```
Optimal Schedule:
  Slot 1: JobA (profit: 100)
  Slot 2: JobB (profit: 80)
  ...
Total Profit: 250
Jobs Skipped: JobC, JobD
```

**Requirements:**
- Implement greedy approach as baseline (5 pts)
- Implement backtracking for optimal solution (10 pts)
- Add branch-and-bound pruning (5 pts)

### Submission

- Single file: `homework2.c`
- Command-line interface: `./homework2 <problem> [options]`
  - Problems: `knight`, `crypto`, `schedule`

---

## Grading Rubric

| Criterion | Points | Description |
|-----------|--------|-------------|
| Correctness | 60% | Produces correct results for all test cases |
| Code Quality | 20% | Clean, well-documented, follows style guide |
| Efficiency | 15% | Appropriate time/space complexity |
| Error Handling | 5% | Graceful handling of edge cases |

### Penalties

- Compilation warnings: -5 points per warning
- Memory leaks (Valgrind): -10 points
- Late submission: -10% per day (max 3 days)
- Plagiarism: 0 points and academic misconduct report

---

## Submission Guidelines

1. **Deadline:** Friday, Week 17, 23:59
2. **Format:** Single `.c` file per homework
3. **Naming:** `homework1.c`, `homework2.c`
4. **Submission:** Upload to course portal

### Testing Your Code

```bash
# Compile with strict flags
gcc -Wall -Wextra -Wpedantic -std=c11 -o homework1 homework1.c -lm

# Run with sample input
./homework1 mst network.txt

# Check for memory leaks
valgrind --leak-check=full ./homework1 mst network.txt
```

---

## Tips for Success

1. **Start early** - Backtracking problems can be tricky to debug
2. **Test incrementally** - Verify each algorithm before combining
3. **Use visualisation** - Print intermediate states for debugging
4. **Profile your code** - Ensure pruning is effective
5. **Read the examples** - Study `example1.c` for patterns

---

*Good luck! This concludes the ATP Laboratory Course.*

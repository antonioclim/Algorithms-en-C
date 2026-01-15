# Week 13 Homework: Graph Algorithms - Shortest Path

## 📋 General Information

- **Deadline:** End of Week 14
- **Points:** 100 (10% of final grade)
- **Language:** C (C11 standard)
- **Compiler:** GCC with `-Wall -Wextra -std=c11 -lm`
- **Memory:** No memory leaks (verified with Valgrind)

---

## 📝 Homework 1: Multi-Source Shortest Paths (50 points)

### Description

Implement a program that finds the shortest paths from **multiple source vertices** simultaneously. Given a weighted directed graph and a set of source vertices, compute the minimum distance from **any** source to each vertex.

This is useful in scenarios such as finding the nearest hospital, fire station or warehouse to any location in a city.

### Requirements

1. **Read input** (5p)
   - First line: `V E K` (vertices, edges, number of sources)
   - Second line: K source vertex indices
   - Following E lines: `u v w` (directed edge from u to v with weight w)

2. **Implement multi-source Dijkstra** (20p)
   - Initialise all source vertices with distance 0
   - Use a priority queue (min-heap)
   - Handle non-negative weights only

3. **Track the nearest source** (10p)
   - For each vertex, record which source it is closest to
   - Store this in a separate array

4. **Output results** (10p)
   - For each vertex: `Vertex v: distance = d, nearest source = s`
   - If unreachable: `Vertex v: unreachable`

5. **Handle edge cases** (5p)
   - Empty graph
   - Disconnected components
   - Invalid input

### Example Usage

**Input:**
```
6 8 2
0 3
0 1 5
0 2 3
1 3 2
2 1 1
2 4 7
3 4 3
3 5 1
4 5 2
```

**Output:**
```
Vertex 0: distance = 0, nearest source = 0
Vertex 1: distance = 4, nearest source = 0
Vertex 2: distance = 3, nearest source = 0
Vertex 3: distance = 0, nearest source = 3
Vertex 4: distance = 3, nearest source = 3
Vertex 5: distance = 1, nearest source = 3
```

### File: `homework1_multisource.c`

---

## 📝 Homework 2: Network Delay Time (50 points)

### Description

Implement a program that simulates network packet propagation. Given a network of computers and communication links with latencies (delays), determine:

1. The time for a signal to reach all computers from a source
2. Which computers are unreachable
3. The critical path (slowest path that determines total time)

This models real-world scenarios like broadcast storms, network troubleshooting and distributed system synchronisation.

### Requirements

1. **Read network topology** (5p)
   - First line: `N M S` (computers, links, source computer)
   - Following M lines: `a b t` (link from a to b with delay t milliseconds)
   - Links are unidirectional

2. **Compute propagation times** (15p)
   - Use Dijkstra's algorithm
   - dist[v] represents time for signal to reach computer v

3. **Find total propagation time** (10p)
   - Maximum of all distances = time for signal to reach ALL computers
   - If any computer unreachable, output -1

4. **Identify critical path** (10p)
   - Find the computer that takes longest to receive the signal
   - Print the path from source to this computer

5. **Generate timing report** (10p)
   - List computers in order of signal arrival
   - Include timestamps and path lengths

### Example Usage

**Input:**
```
5 7 0
0 1 2
0 2 5
1 2 1
1 3 3
2 3 2
2 4 8
3 4 1
```

**Output:**
```
Network Delay Analysis
======================
Source: Computer 0
Total propagation time: 6 ms

Arrival order:
  Computer 0: 0 ms (source)
  Computer 1: 2 ms via 0 -> 1
  Computer 2: 3 ms via 0 -> 1 -> 2
  Computer 3: 5 ms via 0 -> 1 -> 3
  Computer 4: 6 ms via 0 -> 1 -> 3 -> 4

Critical path: 0 -> 1 -> 3 -> 4 (6 ms)
All computers reachable: YES
```

### File: `homework2_network.c`

---

## 📊 Evaluation Criteria

| Criterion | Points |
|-----------|--------|
| Functional correctness | 40 |
| Proper algorithm implementation | 25 |
| Edge case handling | 15 |
| Code quality and comments | 10 |
| No compiler warnings | 5 |
| No memory leaks | 5 |

### Penalties

- **-5p:** Each compiler warning
- **-10p:** Memory leaks (any leak detected by Valgrind)
- **-20p:** Crashes on valid input
- **-30p:** Incorrect algorithm (e.g. using BFS instead of Dijkstra)
- **-50p:** Plagiarism (automatic zero for all parties involved)

---

## 📤 Submission

1. Submit via the university platform
2. File naming: `homework1_multisource.c` and `homework2_network.c`
3. Include your name and student ID in file headers
4. Ensure code compiles without modifications

---

## 💡 Tips

1. **Start early** - These assignments require careful thought about data structures

2. **Test incrementally** - Build and test each component before integration

3. **Use the provided test cases** - They cover common edge cases

4. **Draw diagrams** - Visualise the graph before coding

5. **Check memory** - Run Valgrind after every significant change:
   ```bash
   valgrind --leak-check=full ./homework1_multisource < test_input.txt
   ```

6. **Review Dijkstra's properties** - Remember it only works with non-negative weights

7. **Consider using the heap from class** - The priority queue implementation from Exercise 1 is directly applicable

---

## 🔗 Resources

- Course slides: Week 13 presentation
- CLRS textbook: Chapter 24 (Single-Source Shortest Paths)
- Visualisation: [https://visualgo.net/en/sssp](https://visualgo.net/en/sssp)

# Week 12 Homework: Graph Fundamentals

## 📋 General Information
- **Deadline:** End of week 13
- **Points:** 100 (10% of final grade)
- **Language:** C (C11 standard)
- **Compiler:** GCC with `-Wall -Wextra -std=c11`

---

## 📝 Homework 1: Social Network Analysis (50 points)

### Description

You are tasked with analysing a simplified social network. Given a list of friendships (undirected edges), implement a program that can answer queries about the network's structure, including finding mutual friends, degrees of separation and connected communities.

### Requirements

1. **Graph Construction** (10p)
   - Read the network from a file (adjacency list format)
   - Support up to 10,000 users (vertices)
   - Validate input and handle errors gracefully

2. **Friend Recommendations** (15p)
   - Implement a function that suggests friends based on mutual connections
   - For a given user, find all users who share at least 2 mutual friends but are not already connected
   - Sort recommendations by number of mutual friends (descending)

3. **Degrees of Separation** (15p)
   - Implement BFS to find the shortest path between any two users
   - Report the chain of connections (e.g., "Alice → Bob → Charlie")
   - Handle disconnected users appropriately

4. **Community Detection** (10p)
   - Identify all connected components in the network
   - Report the size of each community
   - Find the largest community

### Input Format
```
n m
user1 user2
user1 user3
...
q
RECOMMEND user_id
DISTANCE user1 user2
COMMUNITIES
```

### Example Usage
```c
/* Expected output for RECOMMEND query */
Recommendations for user 5:
  User 12 (4 mutual friends)
  User 8 (3 mutual friends)
  User 15 (2 mutual friends)

/* Expected output for DISTANCE query */
Distance from 0 to 42: 3
Path: 0 → 7 → 23 → 42

/* Expected output for COMMUNITIES query */
Found 3 communities:
  Community 1: 156 users
  Community 2: 42 users
  Community 3: 2 users
```

### File: `homework1_social.c`

---

## 📝 Homework 2: Task Scheduler with Dependencies (50 points)

### Description

Implement a task scheduling system that respects dependencies between tasks. This is a classic application of topological sorting on directed acyclic graphs (DAGs). Your scheduler must detect circular dependencies and produce a valid execution order when possible.

### Requirements

1. **Task Graph Construction** (10p)
   - Each task has a unique ID and optional name
   - Support directed edges representing "must complete before" relationships
   - Validate that task IDs are within bounds

2. **Cycle Detection** (15p)
   - Implement the three-colour DFS algorithm
   - If a cycle is detected, report all tasks involved in the cycle
   - Provide a clear error message explaining why scheduling is impossible

3. **Topological Sort** (15p)
   - If the graph is acyclic, produce a valid execution order
   - Implement using DFS-based approach (post-order reversal)
   - Handle multiple valid orderings (any valid order is acceptable)

4. **Critical Path Analysis** (10p)
   - Assuming each task takes 1 time unit
   - Calculate the minimum total time to complete all tasks
   - Identify which tasks are on the critical path (cannot be delayed)

### Input Format
```
n m
task_id1 task_id2    # task_id1 must complete before task_id2
...
```

### Example Usage
```c
/* Input: 6 tasks with dependencies */
6 6
0 1
0 2
1 3
2 3
3 4
3 5

/* Expected output */
No cycles detected - scheduling possible

Execution order: 0 2 1 3 5 4
(or any valid topological order)

Critical path length: 4
Critical tasks: 0 -> 1 -> 3 -> 4
               or 0 -> 2 -> 3 -> 4
```

### File: `homework2_scheduler.c`

---

## 📊 Evaluation Criteria

| Criterion | Points |
|-----------|--------|
| Functional correctness | 40 |
| Proper use of graph algorithms (BFS/DFS) | 25 |
| Edge case handling | 15 |
| Code quality and documentation | 10 |
| No compiler warnings | 10 |

### Penalties
- -10p: Compiler warnings (any `-Wall -Wextra` warnings)
- -20p: Memory leaks (detected by Valgrind)
- -30p: Crashes on valid input
- -50p: Plagiarism (automatic zero and disciplinary action)

### Memory Management Requirements
- All dynamically allocated memory must be freed
- Run your code with `valgrind --leak-check=full` before submission
- Zero memory leaks and zero errors expected

---

## 📤 Submission

1. Submit two C source files: `homework1_social.c` and `homework2_scheduler.c`
2. Ensure files compile without warnings using: `gcc -Wall -Wextra -std=c11`
3. Include your name and student ID in the file header comment
4. Upload to the course portal before the deadline

---

## 💡 Tips

1. **Start with the data structures**: Before writing algorithms, ensure your graph representation (adjacency list) is solid and well-tested.

2. **Test incrementally**: Write and test each function separately before integrating. Use small graphs where you can verify results by hand.

3. **Use helper functions**: Functions like `print_graph()`, `print_path()` help with debugging. You can remove or disable them before submission.

4. **Handle edge cases first**: What happens with an empty graph? A single vertex? No edges? Disconnected components?

5. **Memory management matters**: Every `malloc` needs a corresponding `free`. Draw memory diagrams if helpful.

6. **Valgrind is your friend**: Run early and often with `valgrind --leak-check=full ./your_program`. Fix leaks immediately rather than at the end.

7. **Read error messages carefully**: The compiler and Valgrind provide precise information about problems. Don't ignore warnings.

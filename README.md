# 📚 Alternative Laboratory Kit for Programming Techniques and Algorithms

> **⚠️ IMPORTANT NOTICE**  
> This is an **alternative, non-formal learning kit** designed to supplement and extend the official "Programming Techniques and Algorithms" curriculum. It is **not** an official course material and should be used for self-study, practice, and deeper exploration of data structures and algorithms concepts.

---

## 🎯 Purpose and Scope

This comprehensive laboratory kit provides 14 weeks of structured learning materials covering fundamental data structures and algorithmic concepts. Each weekly module includes theoretical foundations, hands-on exercises, automated testing frameworks, and professional-grade HTML presentations designed for offline use.

### Target Audience

```
┌─────────────────────────────────────────────────────────────────────────┐
│  • First-year Computer Science students                                 │
│  • Self-learners preparing for technical interviews                     │
│  • Educators seeking supplementary teaching materials                   │
│  • Professionals refreshing algorithmic foundations                     │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 📋 Kit Contents Overview

### Weekly Module Structure

Each of the 14 weekly modules follows a consistent, pedagogically-sound structure:

```
week-NN-topic/
├── README.md                    # Comprehensive documentation (8-25KB each)
├── Makefile                     # Build automation with testing & Valgrind
│
├── slides/
│   ├── presentation-weekNN.html # Main lecture (1500-2100 lines, ~40 slides)
│   └── presentation-comparativ.html  # C vs Python side-by-side comparison
│
├── src/
│   ├── example1.c               # Complete working demonstration
│   ├── exercise1.c              # Guided exercise (TODOs)
│   └── exercise2.c              # Advanced exercise
│
├── data/
│   └── *.txt                    # Sample data files for testing
│
├── tests/
│   ├── test1_input.txt          # Automated test inputs
│   ├── test1_expected.txt       # Expected outputs
│   ├── test2_input.txt
│   └── test2_expected.txt
│
├── teme/
│   ├── homework-requirements.md # Two graded assignments (50pts each)
│   └── homework-extended.md     # 5 bonus challenges
│
└── solution/
    ├── exercise1_sol.c          # Exercise solutions
    ├── exercise2_sol.c
    ├── homework1_sol.c          # Homework solutions
    └── homework2_sol.c
```

### Complete Kit Statistics

| Metric | Count |
|--------|-------|
| Weekly Modules | 14 |
| C Source Files | 99 |
| Lines of C Code | ~49,000 |
| Markdown Documentation | 42 files |
| README Documentation Lines | ~6,500 |
| HTML Presentations | 28 |
| Presentation Lines (total) | ~50,000 |
| Test Cases | 56 |
| Homework Assignments | 28 |

---

## 📖 Curriculum Overview

### Week-by-Week Topic Map

```
┌───────┬────────────────────────────────────────────────────────────────────┐
│ WEEK  │ TOPIC                                                              │
├───────┼────────────────────────────────────────────────────────────────────┤
│  01   │ Pointers to Functions and Callbacks in C                           │
│       │   └─ Function pointer syntax, qsort(), bsearch(), dispatch tables  │
├───────┼────────────────────────────────────────────────────────────────────┤
│  02   │ Text File Processing in C                                          │
│       │   └─ File I/O, buffering, parsing, error handling                  │
├───────┼────────────────────────────────────────────────────────────────────┤
│  03   │ Binary Files in C                                                  │
│       │   └─ fread/fwrite, struct serialization, endianness               │
├───────┼────────────────────────────────────────────────────────────────────┤
│  04   │ Linked Lists                                                       │
│       │   └─ Singly/doubly linked, circular lists, operations             │
├───────┼────────────────────────────────────────────────────────────────────┤
│  05   │ Stacks — The LIFO Data Structure                                   │
│       │   └─ Array/linked implementations, expression evaluation          │
├───────┼────────────────────────────────────────────────────────────────────┤
│  06   │ Queues                                                             │
│       │   └─ Circular queues, deques, priority queues introduction        │
├───────┼────────────────────────────────────────────────────────────────────┤
│  07   │ Binary Trees                                                       │
│       │   └─ Tree terminology, traversals (pre/in/post), recursion        │
├───────┼────────────────────────────────────────────────────────────────────┤
│  08   │ Binary Search Trees (BST)                                          │
│       │   └─ BST property, search/insert/delete, complexity analysis       │
├───────┼────────────────────────────────────────────────────────────────────┤
│  09   │ AVL Trees                                                          │
│       │   └─ Balance factors, rotations (LL/RR/LR/RL), rebalancing        │
├───────┼────────────────────────────────────────────────────────────────────┤
│  10   │ Heaps and Priority Queues                                          │
│       │   └─ Heap property, heapify, heap sort, priority queue ADT        │
├───────┼────────────────────────────────────────────────────────────────────┤
│  11   │ Hash Tables                                                        │
│       │   └─ Hash functions, collision resolution, load factor            │
├───────┼────────────────────────────────────────────────────────────────────┤
│  12   │ Graph Fundamentals                                                 │
│       │   └─ Representations, BFS, DFS, applications                       │
├───────┼────────────────────────────────────────────────────────────────────┤
│  13   │ Graph Algorithms — Shortest Path                                   │
│       │   └─ Dijkstra, Bellman-Ford, weighted graphs                      │
├───────┼────────────────────────────────────────────────────────────────────┤
│  14   │ Advanced Topics and Comprehensive Review                           │
│       │   └─ Algorithm comparison, complexity summary, exam preparation   │
└───────┴────────────────────────────────────────────────────────────────────┘
```

### Learning Progression Diagram

```
                    ┌─────────────────────────────────────────┐
                    │         FOUNDATIONAL CONCEPTS           │
                    │     Week 01: Function Pointers          │
                    │     Week 02-03: File Processing         │
                    └──────────────────┬──────────────────────┘
                                       │
                    ┌──────────────────▼──────────────────────┐
                    │         LINEAR DATA STRUCTURES          │
                    │     Week 04: Linked Lists               │
                    │     Week 05: Stacks                     │
                    │     Week 06: Queues                     │
                    └──────────────────┬──────────────────────┘
                                       │
        ┌──────────────────────────────┴──────────────────────────────┐
        │                                                              │
        ▼                                                              ▼
┌───────────────────────────┐                    ┌─────────────────────────────┐
│   HIERARCHICAL STRUCTURES │                    │      ASSOCIATIVE DATA       │
│   Week 07: Binary Trees   │                    │   Week 11: Hash Tables      │
│   Week 08: BST            │                    └──────────────┬──────────────┘
│   Week 09: AVL Trees      │                                   │
│   Week 10: Heaps          │                                   │
└───────────┬───────────────┘                                   │
            │                                                   │
            └─────────────────────┬─────────────────────────────┘
                                  │
                    ┌─────────────▼───────────────────────────┐
                    │            GRAPH ALGORITHMS             │
                    │     Week 12: Graph Fundamentals         │
                    │     Week 13: Shortest Path              │
                    └──────────────────┬──────────────────────┘
                                       │
                    ┌──────────────────▼──────────────────────┐
                    │        SYNTHESIS AND REVIEW             │
                    │     Week 14: Comprehensive Review       │
                    └─────────────────────────────────────────┘
```

---

## 🔬 Detailed Week-by-Week Analysis

### Week 01: Pointers to Functions and Callbacks

**Cognitive Level:** Foundational → Applied

| Component | Description | Quality Assessment |
|-----------|-------------|-------------------|
| Theory | Function pointer syntax, callback pattern, qsort/bsearch contracts | ★★★★★ Comprehensive |
| Example | Calculator with dispatch tables, student sorting system | ★★★★★ Production-quality |
| Exercises | Dispatch table implementation, multi-field sorting | ★★★★☆ Appropriate difficulty |
| Homework | Generic search engine, event dispatcher | ★★★★★ Industry-relevant |

**Key Concepts Covered:**
- `int (*fptr)(int, int)` syntax and semantics
- Type-safe comparator patterns (avoiding overflow)
- Dispatch tables as alternative to switch statements
- Industrial applications: GTK, Win32, libuv

---

### Week 02: Text File Processing

**Cognitive Level:** Remember → Analyze

| Component | Description | Quality Assessment |
|-----------|-------------|-------------------|
| Theory | Buffered I/O, stdio.h internals, parsing strategies | ★★★★★ Excellent depth |
| Example | CSV parser with error recovery | ★★★★★ Robust implementation |
| Exercises | Log file analysis, configuration parser | ★★★★★ Real-world scenarios |
| Homework | Web log analyser, INI file manager | ★★★★★ Professional requirements |

**Industrial Relevance:**
```c
// Example: Safe file reading pattern demonstrated
while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    if (buffer[strlen(buffer)-1] != '\n' && !feof(fp)) {
        // Handle line too long
    }
}
```

---

### Week 03: Binary Files

**Cognitive Level:** Understand → Create

| Component | Description | Quality Assessment |
|-----------|-------------|-------------------|
| Theory | Binary vs text, endianness, struct padding | ★★★★★ Thorough |
| Example | Student database with binary storage | ★★★★☆ Good foundation |
| Exercises | Inventory system, record management | ★★★★☆ Appropriate |
| Homework | Database with indices, file merger | ★★★★★ Complex |

---

### Weeks 04-06: Linear Data Structures

**Cognitive Progression:** Linked Lists → Stacks → Queues

```
Complexity Analysis Summary:
┌────────────────┬──────────┬──────────┬──────────┬───────────┐
│ Operation      │ Array    │ Linked   │ Stack    │ Queue     │
├────────────────┼──────────┼──────────┼──────────┼───────────┤
│ Access [i]     │ O(1)     │ O(n)     │ N/A      │ N/A       │
│ Insert front   │ O(n)     │ O(1)     │ N/A      │ O(1)*     │
│ Insert back    │ O(1)*    │ O(1)†    │ O(1)     │ O(1)      │
│ Delete front   │ O(n)     │ O(1)     │ N/A      │ O(1)      │
│ Delete back    │ O(1)     │ O(n)‡    │ O(1)     │ N/A       │
│ Search         │ O(n)     │ O(n)     │ O(n)     │ O(n)      │
└────────────────┴──────────┴──────────┴──────────┴───────────┘
* amortized  † with tail pointer  ‡ without tail
```

---

### Weeks 07-10: Hierarchical Structures

**Tree Structure Evolution:**

```
Week 07: Binary Tree          Week 08: BST              Week 09: AVL
      A                            50                        50
     / \                          /  \                      /  \
    B   C                       30    70                  30    70
   / \   \                     /  \   / \                /  \   / \
  D   E   F                  20  40 60  80             20  40 60  80
                                                     BF=0 for all nodes

Week 10: Heap (Max)
         100
        /   \
      90     80
     /  \   /  \
   70   60 50  40
```

| Week | Focus | Key Algorithms | Complexity |
|------|-------|---------------|------------|
| 07 | Traversals | Pre/In/Post-order, Level-order | O(n) |
| 08 | BST Operations | Search, Insert, Delete | O(h) → O(log n) avg |
| 09 | Self-balancing | 4 rotation types, height maintenance | O(log n) guaranteed |
| 10 | Heap Operations | Heapify, Extract-Max, Heap-Sort | O(log n), O(n log n) |

---

### Week 11: Hash Tables

**Collision Resolution Strategies Covered:**

```
┌─────────────────────────────────────────────────────────────────────────┐
│  1. Chaining (Separate Chaining)                                        │
│     ┌───┐     ┌───┬───┐                                                 │
│  0  │ ●─┼────►│ A │ ●─┼───► NULL                                        │
│     ├───┤     └───┴───┘                                                 │
│  1  │NIL│                                                               │
│     ├───┤     ┌───┬───┐     ┌───┬───┐                                   │
│  2  │ ●─┼────►│ B │ ●─┼────►│ C │ ●─┼───► NULL                          │
│     └───┘     └───┴───┘     └───┴───┘                                   │
├─────────────────────────────────────────────────────────────────────────┤
│  2. Open Addressing (Linear/Quadratic Probing, Double Hashing)          │
│     ┌───┬───┬───┬───┬───┬───┬───┬───┐                                   │
│     │ A │   │ B │ C │   │   │ D │   │                                   │
│     └───┴───┴───┴───┴───┴───┴───┴───┘                                   │
│       ↑               ↑                                                 │
│     h(A)=0        h(B)=2, h(C)=2→3 (probe)                              │
└─────────────────────────────────────────────────────────────────────────┘
```

---

### Weeks 12-13: Graph Algorithms

**Representations Covered:**

```
Adjacency Matrix              Adjacency List
     0 1 2 3 4                0: → 1 → 2
  ┌─────────────┐             1: → 0 → 3
0 │ 0 1 1 0 0 │             2: → 0 → 3 → 4
1 │ 1 0 0 1 0 │             3: → 1 → 2 → 4
2 │ 1 0 0 1 1 │             4: → 2 → 3
3 │ 0 1 1 0 1 │
4 │ 0 0 1 1 0 │             Space: O(V + E)
  └─────────────┘
    Space: O(V²)
```

**Algorithms Implemented:**

| Algorithm | Purpose | Time | Space |
|-----------|---------|------|-------|
| BFS | Shortest path (unweighted) | O(V+E) | O(V) |
| DFS | Connectivity, cycles | O(V+E) | O(V) |
| Dijkstra | Single-source shortest path | O(E log V) | O(V) |
| Bellman-Ford | Negative weights allowed | O(VE) | O(V) |

---

## ⚠️ Curriculum Alignment Analysis

### Comparison with Official Syllabus

This kit was designed as an **alternative curriculum** focusing on **data structures**. The following table shows the alignment with the official "Programming Techniques and Algorithms" syllabus:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    OFFICIAL SYLLABUS vs KIT COVERAGE                        │
├────────────────────────────────────────┬───────────────────┬────────────────┤
│ Official Topic                         │ Kit Coverage      │ Status         │
├────────────────────────────────────────┼───────────────────┼────────────────┤
│ External file organization             │ Weeks 02-03       │ ✅ COVERED     │
│ Text files                             │ Week 02           │ ✅ COVERED     │
│ Sequential binary files                │ Week 03           │ ✅ COVERED     │
│ Relative binary files                  │ Week 03 (partial) │ ⚠️ PARTIAL    │
│ Indexed binary files                   │ Not covered       │ ❌ MISSING     │
├────────────────────────────────────────┼───────────────────┼────────────────┤
│ Algorithm complexity                   │ Week 08 (BST)     │ ⚠️ EMBEDDED   │
│ Divide et Impera method                │ Not explicit      │ ❌ MISSING     │
├────────────────────────────────────────┼───────────────────┼────────────────┤
│ Sorting algorithms (QuickSort, Shell,  │ Week 10 (HeapSort)│ ⚠️ PARTIAL    │
│ Counting, Radix, Bucket)               │                   │                │
├────────────────────────────────────────┼───────────────────┼────────────────┤
│ Greedy method                          │ Dijkstra/Prim     │ ⚠️ IMPLICIT   │
│ Backtracking method                    │ Not covered       │ ❌ MISSING     │
├────────────────────────────────────────┼───────────────────┼────────────────┤
│ Graphs: definitions, representations   │ Week 12           │ ✅ COVERED     │
│ Graph traversals (BFS/DFS)             │ Week 12           │ ✅ COVERED     │
│ Connectivity, paths                    │ Week 12-13        │ ✅ COVERED     │
│ Cut nodes, topological sorting         │ Not covered       │ ❌ MISSING     │
│ Weighted graphs, shortest paths        │ Week 13           │ ✅ COVERED     │
│ Minimum Spanning Trees (Kruskal, Prim) │ Not covered       │ ❌ MISSING     │
└────────────────────────────────────────┴───────────────────┴────────────────┘
```

### Topics UNIQUE to This Kit (Not in Official Syllabus)

These topics provide valuable foundational knowledge, typically covered in a dedicated "Data Structures" course:

| Topic | Week | Value Proposition |
|-------|------|-------------------|
| Linked Lists | 04 | Foundation for tree/graph implementations |
| Stacks | 05 | Expression evaluation, recursion simulation |
| Queues | 06 | BFS implementation, buffering |
| Binary Trees | 07 | Prerequisite for BST/AVL |
| BST | 08 | Efficient dynamic sets |
| AVL Trees | 09 | Guaranteed logarithmic operations |
| Heaps | 10 | Priority queues, heap sort |
| Hash Tables | 11 | O(1) average lookup |

---

## 🚀 Recommended Extensions (2 Additional Weeks)

To address gaps in both the official syllabus and advanced algorithm design, the following extensions are recommended:

### Proposed Week 15: Algorithmic Paradigms — Divide & Conquer, Greedy, and Dynamic Programming

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  Week 15: Algorithmic Paradigms                                             │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  PART A: Divide and Conquer                                                 │
│  ├── Theoretical framework: T(n) = aT(n/b) + f(n)                          │
│  ├── Master Theorem application                                             │
│  ├── Examples: MergeSort, QuickSort, Binary Search                         │
│  └── Practical: Closest pair of points, Strassen's matrix multiplication   │
│                                                                             │
│  PART B: Greedy Algorithms                                                  │
│  ├── Greedy choice property and optimal substructure                       │
│  ├── Activity selection problem                                             │
│  ├── Huffman coding                                                         │
│  └── Fractional knapsack                                                    │
│                                                                             │
│  PART C: Introduction to Dynamic Programming                                │
│  ├── Overlapping subproblems and memoization                               │
│  ├── Fibonacci: recursive → memoized → tabulated                           │
│  ├── 0/1 Knapsack problem                                                   │
│  └── Longest Common Subsequence (LCS)                                       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Proposed Week 16: Advanced Graph Algorithms and Backtracking

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  Week 16: Advanced Graph Algorithms and Backtracking                        │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  PART A: Minimum Spanning Trees                                             │
│  ├── MST properties and cut property                                        │
│  ├── Kruskal's algorithm (Union-Find data structure)                       │
│  ├── Prim's algorithm (priority queue implementation)                      │
│  └── Comparative analysis: when to use which                               │
│                                                                             │
│  PART B: Advanced Graph Topics                                              │
│  ├── Topological sorting (Kahn's algorithm, DFS-based)                     │
│  ├── Strongly connected components (Kosaraju, Tarjan)                      │
│  ├── Articulation points and bridges                                        │
│  └── All-pairs shortest paths (Floyd-Warshall)                             │
│                                                                             │
│  PART C: Backtracking                                                       │
│  ├── State-space tree and pruning                                          │
│  ├── N-Queens problem                                                       │
│  ├── Sudoku solver                                                          │
│  ├── Graph coloring                                                         │
│  └── Hamiltonian path                                                       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 🛠️ Technical Requirements

### Development Environment

| Component | Requirement |
|-----------|-------------|
| Compiler | GCC 9.0+ with C11 support |
| OS | Linux (Ubuntu 20.04+), WSL2, macOS |
| Build System | GNU Make 4.0+ |
| Memory Checker | Valgrind 3.15+ |
| Browser | Modern browser for HTML slides |

### Compilation Flags

All source files are compiled with strict warning flags:

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O0
LDFLAGS = -lm
```

### Quick Start

```bash
# Clone or extract the kit
cd week-01-intro-pointers/

# View available commands
make help

# Compile all sources
make

# Run the demonstration
make run

# Run automated tests
make test

# Check for memory leaks
make valgrind

# Clean build artifacts
make clean
```

---

## 📊 Quality Assessment Summary

### Strengths

| Aspect | Rating | Notes |
|--------|--------|-------|
| Documentation | ★★★★★ | Comprehensive README files with historical context |
| Code Quality | ★★★★★ | Well-commented, follows best practices |
| Visual Design | ★★★★★ | Modern HTML presentations with offline capability |
| Testing | ★★★★☆ | Automated tests with Valgrind integration |
| Pedagogy | ★★★★★ | Clear learning objectives, progressive difficulty |
| Practical Relevance | ★★★★★ | Industry-relevant homework assignments |

### Areas for Improvement

| Aspect | Current State | Recommendation |
|--------|---------------|----------------|
| Syllabus Alignment | Data Structures focus | Add D&I, Greedy, Backtracking modules |
| Indexed Files | Not covered | Add Week 04.5 for indexed binary files |
| Sorting Algorithms | Only HeapSort | Add comprehensive sorting module |
| MST Algorithms | Missing | Include Kruskal and Prim |

---

## 📝 Assessment Framework

### Per-Week Grade Distribution

```
┌────────────────────────────────────────────────────────────────┐
│  Component                                    │  Points        │
├────────────────────────────────────────────────────────────────┤
│  Laboratory Exercises (2 per week)            │  20 points     │
│  ├── Exercise 1                               │    10 points   │
│  └── Exercise 2                               │    10 points   │
├────────────────────────────────────────────────────────────────┤
│  Homework Assignments (2 per week)            │  80 points     │
│  ├── Homework 1                               │    50 points   │
│  └── Homework 2                               │    50 points   │
├────────────────────────────────────────────────────────────────┤
│  Extended Challenges (optional)               │  Bonus: +20    │
└────────────────────────────────────────────────────────────────┘
```

### Evaluation Criteria

| Criterion | Weight | Description |
|-----------|--------|-------------|
| Correctness | 40% | Passes all test cases |
| Code Quality | 20% | Readability, structure, documentation |
| Memory Safety | 20% | No leaks (Valgrind clean) |
| Error Handling | 10% | Graceful failure, informative messages |
| Efficiency | 10% | Appropriate algorithmic complexity |

---

## 🔗 Resource Links

### Reference Materials

| Resource | Type | URL |
|----------|------|-----|
| CLRS (Introduction to Algorithms) | Textbook | ISBN: 978-0262033848 |
| The Art of Computer Programming | Textbook | ISBN: 978-0201896831 |
| Visualgo | Visualisation | https://visualgo.net/ |
| cppreference.com | Documentation | https://en.cppreference.com/w/c |
| Compiler Explorer | Tool | https://godbolt.org/ |

### Online Practice

| Platform | Description |
|----------|-------------|
| LeetCode | Algorithm problems by topic |
| HackerRank | Data structures track |
| Codeforces | Competitive programming |

---

## 📄 License and Usage

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                                                                             │
│                        RESTRICTIVE LICENSE NOTICE                           │
│                                                                             │
│  © 2025 All Rights Reserved                                                 │
│                                                                             │
│  This educational material is provided for PERSONAL STUDY ONLY.             │
│                                                                             │
│  PROHIBITED without prior written consent:                                  │
│    • Publication in any form (print, digital, online)                      │
│    • Use in formal educational settings (courses, workshops)               │
│    • Commercial use or redistribution                                       │
│    • Derivative works for teaching purposes                                │
│                                                                             │
│  For licensing inquiries, contact the author.                               │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 📧 Contact

For questions, licensing requests, or feedback:

**Author:** Asistent (pe perioadă determinată) ing. dr. Antonio Clim

---

*Last updated: January 2025*

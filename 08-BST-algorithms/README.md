# Week 08: Binary Search Trees (BST)

## 🎯 Learning Objectives

Upon completion of this laboratory, students will be able to:

1. **Remember** the fundamental properties that distinguish a Binary Search Tree from a general binary tree, including the ordering invariant and structural constraints
2. **Understand** the relationship between tree balance and algorithmic complexity, explaining why degenerate trees lead to O(n) operations
3. **Apply** recursive and iterative algorithms for search, insertion and deletion operations on Binary Search Trees
4. **Analyse** the time and space complexity of BST operations under best-case, average-case and worst-case scenarios
5. **Evaluate** different deletion strategies (in-order predecessor vs successor replacement) and their implications for tree balance
6. **Create** a complete BST implementation with support for traversals, range queries and tree validation functions

---

## 📜 Historical Context

The Binary Search Tree emerged from the confluence of two powerful ideas in computer science: the efficiency of binary search and the flexibility of linked data structures. The concept crystallised in the late 1950s and early 1960s, as researchers sought data structures that could support efficient dynamic operations—insertion and deletion—whilst maintaining the logarithmic search times that made sorted arrays so attractive.

The formal study of tree-based searching gained momentum with the work of several pioneering researchers. Andrew Donald Booth and Kathleen Booth explored binary tree structures in their 1956 work on automatic digital calculators. However, it was the contributions of researchers at the RAND Corporation, Stanford University and MIT throughout the 1960s that established the theoretical foundations we use today. The analysis of average-case behaviour for random BSTs, showing expected O(log n) operations, represented a significant achievement in probabilistic analysis of algorithms.

The recognition that BSTs could degenerate into linear structures sparked decades of research into self-balancing variants. This led to the development of AVL trees by Georgy Adelson-Velsky and Evgenii Landis in 1962, Red-Black trees by Rudolf Bayer in 1972 and numerous other balanced tree structures. The humble BST thus served as both a practical data structure and a launching point for more sophisticated algorithms.

### Key Figure: Donald Knuth (1938–)

Donald Ervin Knuth, born in Milwaukee, Wisconsin, stands as one of the most influential figures in the analysis of algorithms and data structures. His monumental work, *The Art of Computer Programming*, begun in 1962 and still ongoing, provides the most comprehensive treatment of fundamental algorithms ever assembled. Volume 3, *Sorting and Searching*, published in 1973, contains exhaustive analysis of tree-based data structures including Binary Search Trees.

Knuth's contributions extend far beyond algorithmic analysis. He created the TeX typesetting system and the METAFONT font description language, revolutionising technical publishing. His analysis of BST behaviour under random insertions established the mathematical foundations that generations of computer scientists have built upon.

> *"Premature optimisation is the root of all evil."*
> — Donald Knuth, *Structured Programming with go to Statements* (1974)

---

## 📚 Theoretical Foundations

### 1. The Binary Search Tree Property

A Binary Search Tree is a binary tree that satisfies the **BST invariant**: for every node *x*, all keys in the left subtree of *x* are strictly less than *x*'s key, and all keys in the right subtree are strictly greater than *x*'s key. This property must hold recursively throughout the entire tree.

```
        Valid BST              Invalid BST (violation at node 3)
           50                        50
          /  \                      /  \
        30    70                  30    70
       /  \   / \                /  \   / \
      20  40 60  80            20  40 60  80
                                   /
                                  35  ← violates: 35 > 30 but in left subtree
```

The BST property enables binary search in a tree structure:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  SEARCH(node, key):                                                         │
│    if node is NULL:                                                         │
│        return NOT_FOUND                                                     │
│    if key == node.key:                                                      │
│        return node                                                          │
│    if key < node.key:                                                       │
│        return SEARCH(node.left, key)    ← eliminate right subtree           │
│    else:                                                                    │
│        return SEARCH(node.right, key)   ← eliminate left subtree            │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2. Node Structure and Memory Layout

Each BST node contains three essential components: a key (and optionally associated data), a pointer to the left child and a pointer to the right child. Some implementations include a parent pointer for simplified deletion and traversal operations.

```c
typedef struct BSTNode {
    int key;                    // The search key
    void *data;                 // Optional: associated data
    struct BSTNode *left;       // Left child (smaller keys)
    struct BSTNode *right;      // Right child (larger keys)
    struct BSTNode *parent;     // Optional: parent pointer
} BSTNode;
```

Memory layout for a BST with keys {20, 10, 30, 5, 15, 25, 35}:

```
                    Stack                         Heap
              ┌──────────────┐           ┌─────────────────────────┐
   root ────────────────────────────────►│ key: 20                 │
              │              │           │ left ──────────────┐    │
              │              │           │ right ─────────────│────│─┐
              │              │           └─────────────────────│───┘ │
              │              │                                 │     │
              │              │           ┌─────────────────────▼───┐ │
              │              │           │ key: 10                 │ │
              │              │           │ left ───────────┐       │ │
              │              │           │ right ──────────│───┐   │ │
              │              │           └──────────────────│───│──┘ │
              │              │                              │   │    │
              └──────────────┘           ┌──────────────────▼─┐ │    │
                                         │ key: 5             │ │    │
                                         │ left: NULL         │ │    │
                                         │ right: NULL        │ │    │
                                         └────────────────────┘ │    │
                                         ┌──────────────────────▼──┐ │
                                         │ key: 15                 │ │
                                         │ left: NULL              │ │
                                         │ right: NULL             │ │
                                         └─────────────────────────┘ │
                                         ┌───────────────────────────▼─┐
                                         │ key: 30                     │
                                         │ left ───► [25]              │
                                         │ right ───► [35]             │
                                         └─────────────────────────────┘
```

### 3. Complexity Analysis

The efficiency of BST operations depends critically on tree height. A perfectly balanced tree has height ⌊log₂(n)⌋, whilst a completely degenerate tree (effectively a linked list) has height n-1.

| Operation | Best Case | Average Case | Worst Case | Space |
|-----------|-----------|--------------|------------|-------|
| Search    | O(1)      | O(log n)     | O(n)       | O(1)  |
| Insert    | O(1)      | O(log n)     | O(n)       | O(1)  |
| Delete    | O(1)      | O(log n)     | O(n)       | O(1)  |
| Traversal | O(n)      | O(n)         | O(n)       | O(h)* |
| Find Min  | O(1)      | O(log n)     | O(n)       | O(1)  |
| Find Max  | O(1)      | O(log n)     | O(n)       | O(1)  |

*Where h = tree height; O(log n) for balanced trees, O(n) for degenerate trees

**Probability Analysis**: When n keys are inserted in random order, the expected height of the resulting BST is approximately 2.99 × log₂(n). This remarkable result, proved by Knuth and others, demonstrates that random BSTs behave well on average even without explicit balancing.

```
Tree Shape vs Performance:

Balanced (h = 3):              Degenerate (h = 6):
       4                              1
      / \                              \
     2   6                              2
    / \ / \                              \
   1  3 5  7                              3
                                           \
Operations: O(log n)                        4
                                             \
                                              5
                                               \
                                                6
                                    Operations: O(n)
```

---

## 🏭 Industrial Applications

Binary Search Trees and their balanced variants form the backbone of numerous production systems. Understanding their implementation provides insight into how modern software achieves efficient data management.

### 1. Database Indexing (PostgreSQL B-Trees)

Database management systems rely heavily on tree-based indices. PostgreSQL's B-Tree index, a generalisation of the BST concept, accelerates queries by orders of magnitude:

```c
/* Simplified representation of how database queries use tree indices */
typedef struct IndexEntry {
    Datum key;              /* Indexed column value */
    ItemPointer tid;        /* Pointer to actual row (tuple ID) */
    struct IndexEntry *left;
    struct IndexEntry *right;
} IndexEntry;

/* Query: SELECT * FROM users WHERE age = 25 */
IndexEntry *find_index_entry(IndexEntry *root, Datum search_key) {
    while (root != NULL) {
        int cmp = compare_datums(search_key, root->key);
        if (cmp == 0) {
            return root;  /* Found: follow tid to retrieve full row */
        }
        root = (cmp < 0) ? root->left : root->right;
    }
    return NULL;  /* Not found: no matching row exists */
}
```

### 2. Memory Allocators (Linux Kernel Red-Black Trees)

The Linux kernel uses Red-Black trees (self-balancing BSTs) extensively for managing memory regions:

```c
/* From Linux kernel: include/linux/rbtree.h (simplified) */
struct rb_node {
    unsigned long rb_parent_color;
    struct rb_node *rb_right;
    struct rb_node *rb_left;
};

struct vm_area_struct {
    unsigned long vm_start;     /* Start address of memory region */
    unsigned long vm_end;       /* End address */
    struct rb_node vm_rb;       /* Red-Black tree node for fast lookup */
    /* ... other fields ... */
};

/* Finding a memory region containing a given address: O(log n) */
struct vm_area_struct *find_vma(struct mm_struct *mm, unsigned long addr);
```

### 3. Standard Library Implementations (C++ std::map and std::set)

The C++ Standard Library's ordered associative containers use Red-Black trees internally:

```cpp
// C++ std::map is typically implemented as a Red-Black tree
#include <map>

std::map<std::string, int> word_counts;
word_counts["algorithm"] = 42;  // O(log n) insertion
word_counts["binary"] = 17;
word_counts["tree"] = 23;

// O(log n) lookup
auto it = word_counts.find("binary");

// In-order traversal: keys appear in sorted order
for (const auto& [word, count] : word_counts) {
    std::cout << word << ": " << count << std::endl;
}
// Output: algorithm: 42, binary: 17, tree: 23
```

### 4. File Systems (Btrfs Extent Trees)

Modern file systems use B-trees and variants for metadata management:

```c
/* Btrfs uses B+ trees for file extent mapping (simplified concept) */
typedef struct ExtentNode {
    uint64_t logical_offset;    /* Logical position in file */
    uint64_t physical_block;    /* Physical location on disc */
    uint64_t length;            /* Extent length */
    struct ExtentNode *left;
    struct ExtentNode *right;
} ExtentNode;

/* Finding which disc block contains file offset X: O(log n) */
ExtentNode *find_extent(ExtentNode *root, uint64_t file_offset);
```

### 5. Network Routing Tables (Longest Prefix Match)

Network routers use tree structures for IP address lookup:

```c
/* Simplified routing table using BST principles */
typedef struct RouteEntry {
    uint32_t network;           /* Network address */
    uint32_t mask;              /* Subnet mask */
    uint32_t next_hop;          /* Next router IP */
    struct RouteEntry *left;    /* More specific routes */
    struct RouteEntry *right;   /* Less specific routes */
} RouteEntry;

/* Determine next hop for destination IP: O(log n) average */
uint32_t lookup_route(RouteEntry *root, uint32_t dest_ip);
```

---

## 💻 Laboratory Exercises

### Exercise 1: Basic BST Operations

Implement a complete Binary Search Tree supporting fundamental operations.

**Requirements:**
1. Define a BST node structure with key, left child and right child pointers
2. Implement `bst_create_node()` for dynamic node allocation
3. Implement iterative `bst_search()` that returns the node or NULL
4. Implement recursive `bst_insert()` maintaining the BST property
5. Implement `bst_find_min()` and `bst_find_max()` functions
6. Implement all three traversal orders: in-order, pre-order, post-order
7. Implement `bst_free()` to deallocate the entire tree
8. Create a validation function `bst_is_valid()` to verify the BST property

**Input Format:**
```
[number of operations]
[operation] [arguments]
...
```

**Operations:**
- `INSERT key` - Insert a new key
- `SEARCH key` - Search for a key
- `MIN` - Find minimum key
- `MAX` - Find maximum key
- `INORDER` - Print in-order traversal
- `PREORDER` - Print pre-order traversal
- `POSTORDER` - Print post-order traversal

### Exercise 2: BST Deletion and Advanced Operations

Extend the BST implementation with deletion and additional functionality.

**Requirements:**
1. Implement `bst_delete()` handling all three cases:
   - Deleting a leaf node
   - Deleting a node with one child
   - Deleting a node with two children (using in-order successor)
2. Implement `bst_height()` to compute tree height
3. Implement `bst_count_nodes()` to count total nodes
4. Implement `bst_count_leaves()` to count leaf nodes
5. Implement `bst_range_search()` to find all keys in range [low, high]
6. Implement `bst_kth_smallest()` using augmented nodes or traversal
7. Implement `bst_lca()` to find lowest common ancestor of two nodes
8. Implement `bst_print_tree()` for visual tree representation

**Input Format:**
```
[initial tree from space-separated keys]
[number of operations]
[operation] [arguments]
...
```

---

## 🔧 Compilation and Execution

```bash
# Build all targets
make

# Build specific target
make example1
make exercise1
make exercise2

# Run the example demonstration
make run

# Run automated tests
make test

# Check for memory leaks with Valgrind
make valgrind

# Build and run solutions (instructor only)
make solutions

# Clean build artifacts
make clean

# Display help
make help
```

**Compiler flags used:**
- `-Wall` — Enable all common warnings
- `-Wextra` — Enable additional warnings
- `-std=c11` — Use C11 standard
- `-g` — Include debugging symbols
- `-fsanitize=address` — (optional) Enable address sanitiser

---

## 📁 Directory Structure

```
week-08-binary-search-trees/
├── README.md                           # This documentation
├── Makefile                            # Build automation
│
├── slides/
│   ├── presentation-week08.html        # Main lecture (38 slides)
│   └── presentation-comparativ.html    # Pseudocode → C → Python (12 slides)
│
├── src/
│   ├── example1.c                      # Complete BST demonstration
│   ├── exercise1.c                     # Basic operations exercise
│   └── exercise2.c                     # Deletion and advanced exercise
│
├── data/
│   ├── sample_keys.txt                 # Sample key data for testing
│   └── operations.txt                  # Sample operation sequences
│
├── tests/
│   ├── test1_input.txt                 # Test input for exercise 1
│   ├── test1_expected.txt              # Expected output for exercise 1
│   ├── test2_input.txt                 # Test input for exercise 2
│   └── test2_expected.txt              # Expected output for exercise 2
│
├── teme/
│   ├── homework-requirements.md        # Two homework assignments
│   └── homework-extended.md            # Five bonus challenges
│
└── solution/
    ├── exercise1_sol.c                 # Solution for exercise 1
    ├── exercise2_sol.c                 # Solution for exercise 2
    ├── homework1_sol.c                 # Solution for homework 1
    └── homework2_sol.c                 # Solution for homework 2
```

---

## 📖 Recommended Reading

### Essential
- Cormen, T.H., Leiserson, C.E., Rivest, R.L., Stein, C. — *Introduction to Algorithms*, Chapter 12: Binary Search Trees
- Sedgewick, R., Wayne, K. — *Algorithms*, Section 3.2: Binary Search Trees
- Knuth, D.E. — *The Art of Computer Programming, Vol. 3: Sorting and Searching*, Section 6.2

### Advanced
- Adelson-Velsky, G., Landis, E. — "An algorithm for the organisation of information" (1962) — The original AVL paper
- Bayer, R. — "Symmetric binary B-Trees: Data structure and maintenance algorithms" (1972) — Red-Black trees origin
- Sleator, D., Tarjan, R. — "Self-Adjusting Binary Search Trees" (1985) — Splay trees

### Online Resources
- Visualgo BST: https://visualgo.net/en/bst — Interactive BST visualisation
- USFCA BST Visualisation: https://www.cs.usfca.edu/~galles/visualization/BST.html
- MIT OpenCourseWare 6.006 Lecture 5: https://ocw.mit.edu/courses/6-006-introduction-to-algorithms-spring-2020/

---

## ✅ Self-Assessment Checklist

Before completing this laboratory, verify that you can:

- [ ] Explain the BST property and identify whether a given tree satisfies it
- [ ] Implement iterative and recursive search in a BST
- [ ] Insert a new key whilst maintaining the BST property
- [ ] Delete a node handling all three cases (leaf, one child, two children)
- [ ] Perform in-order, pre-order and post-order traversals both recursively and iteratively
- [ ] Analyse the time complexity of BST operations and explain worst-case scenarios
- [ ] Implement tree utility functions (height, count, validation)
- [ ] Use Valgrind to verify that your implementation has no memory leaks
- [ ] Explain the relationship between tree balance and operation efficiency
- [ ] Describe real-world applications of BST structures

---

## 💼 Interview Preparation

Common interview questions on Binary Search Trees:

1. **Validity Check**: "Given a binary tree, write a function to determine if it is a valid Binary Search Tree." (LeetCode #98)
   - Key insight: Track valid range [min, max] for each node during traversal

2. **Kth Smallest Element**: "Find the kth smallest element in a BST." (LeetCode #230)
   - Key insight: In-order traversal visits nodes in sorted order

3. **Lowest Common Ancestor**: "Given two nodes in a BST, find their lowest common ancestor." (LeetCode #235)
   - Key insight: LCA is where the two nodes' paths diverge

4. **Convert Sorted Array to BST**: "Given a sorted array, create a height-balanced BST." (LeetCode #108)
   - Key insight: Recursively choose middle element as root

5. **BST Iterator**: "Implement an iterator that returns the next smallest element in O(1) amortised time." (LeetCode #173)
   - Key insight: Use a stack to simulate in-order traversal

---

## 🔗 Next Week Preview

**Week 09: AVL Trees**

Building upon your understanding of Binary Search Trees, we shall explore AVL trees—the first self-balancing BST structure ever invented. Named after its creators Adelson-Velsky and Landis, AVL trees maintain strict balance through rotation operations. You will learn to:

- Calculate and maintain balance factors at each node
- Implement single and double rotations (LL, RR, LR, RL)
- Analyse the guaranteed O(log n) complexity for all operations
- Compare AVL trees with other balancing schemes

The insights gained from understanding unbalanced BSTs this week will illuminate why self-balancing structures are essential for guaranteed performance.

---

*Laboratory materials prepared for ATP Course, Academy of Economic Studies - CSIE Bucharest*

# Week 09: AVL Trees

## 🎯 Learning Objectives

Upon successful completion of this laboratory session, students will be able to:

1. **Remember** the definition and properties of AVL trees, including the balance factor invariant and the four rotation types required for rebalancing operations.

2. **Understand** the relationship between tree height and node count in AVL trees, explaining why the O(log n) guarantee holds and how rotations preserve the binary search tree property.

3. **Apply** AVL tree insertion and deletion algorithms by implementing complete, working C code that correctly maintains balance after each modification operation.

4. **Analyse** the time and space complexity of AVL operations, comparing them with standard BST operations and identifying scenarios where AVL trees provide significant performance advantages.

5. **Evaluate** the trade-offs between AVL trees and other self-balancing structures (Red-Black trees, B-trees), selecting the appropriate data structure based on operational requirements.

6. **Create** a fully functional AVL tree library in C with comprehensive insert, delete, search and traversal operations, including memory management and debugging utilities.

---

## 📜 Historical Context

The AVL tree stands as one of the earliest self-balancing binary search tree data structures ever invented, predating its more famous cousin, the Red-Black tree, by nearly two decades. The structure emerged from the Soviet Union during the height of the Cold War, a period when Soviet computer science was developing largely in isolation from Western advances yet producing remarkably sophisticated theoretical contributions.

The year was 1962 when Georgy Maximovich Adelson-Velsky and Evgenii Mikhailovich Landis, two mathematicians working at the Institute of Theoretical and Experimental Physics in Moscow, published their seminal paper "An algorithm for the organisation of information" in the Soviet journal *Doklady Akademii Nauk SSSR*. This publication introduced what they termed "height-balanced binary search trees" but which the computing community would come to know as AVL trees—the name derived from the authors' initials.

The fundamental insight of Adelson-Velsky and Landis was elegant in its simplicity: by enforcing that the heights of the left and right subtrees of any node differ by at most one, and by providing efficient local transformations (rotations) to restore this property after insertions and deletions, one could guarantee that the tree height would never exceed approximately 1.44 log₂(n). This guarantee transformed the worst-case performance of binary search trees from O(n)—possible when inserting sorted data into an unbalanced tree—to O(log n) for all operations.

The timing of this invention was particularly significant. The early 1960s marked a period of explosive growth in computing applications, and the need for efficient data retrieval mechanisms was becoming acute. Traditional arrays offered O(1) access but O(n) insertion; linked lists provided O(1) insertion but O(n) search; hash tables delivered average-case O(1) but had troublesome worst cases and didn't support ordered operations. The AVL tree offered a compelling middle ground: O(log n) for search, insertion and deletion, whilst maintaining sorted order and supporting range queries.

### Key Figure: Georgy Adelson-Velsky (1922–2014)

Georgy Maximovich Adelson-Velsky was born in Samara, Russia, and developed into one of the most influential Soviet computer scientists of the twentieth century. His contributions extended far beyond the AVL tree; he was instrumental in developing computer chess programs and made fundamental contributions to game theory and artificial intelligence.

Adelson-Velsky led the team that created Kaissa, which in 1974 became the first program to win the World Computer Chess Championship. His work on game-playing algorithms influenced generations of AI researchers. He also contributed to the development of bitboards, a technique for representing chess positions that remains standard in modern chess engines.

His collaboration with Landis on the AVL tree represents one of those rare moments in computer science where a theoretical insight immediately translates into practical utility. The paper was remarkably concise—only a few pages—yet it established concepts that remain foundational to computer science education worldwide.

> *"The complexity of the algorithm is determined not by the problem itself, but by the structure we impose upon its solution."*
> — G.M. Adelson-Velsky

---

## 📚 Theoretical Foundations

### 1. AVL Tree Definition and Properties

An AVL tree is a binary search tree (BST) that maintains the following invariant: for every node in the tree, the heights of its left and right subtrees differ by at most one. This seemingly simple constraint has profound implications for the tree's structure and performance characteristics.

**Formal Definition:**
```
A binary search tree T is an AVL tree if and only if:
  ∀ node n ∈ T: |height(left(n)) - height(right(n))| ≤ 1
```

The **balance factor** of a node is defined as:
```
balance_factor(n) = height(left(n)) - height(right(n))
```

In a valid AVL tree, the balance factor of every node must be in the set {-1, 0, +1}:
- **+1**: Left-heavy (left subtree is one level taller)
- **0**: Perfectly balanced (both subtrees have equal height)
- **-1**: Right-heavy (right subtree is one level taller)

```
         AVL Tree Structure
         ==================

              50 (bf=0)
             /  \
       30 (bf=+1)  70 (bf=-1)
       /          / \
    20 (bf=0)  60    80
                      \
                      90

  bf = balance factor
  height(null) = -1 by convention
```

### 2. Height Bounds and the Golden Ratio

The most remarkable property of AVL trees is the guaranteed height bound. For an AVL tree with n nodes, the height h satisfies:

```
log₂(n + 1) - 1 ≤ h < 1.4405 × log₂(n + 2) - 0.3277
```

This bound arises from the structure of **minimally-balanced AVL trees**—trees that have the minimum number of nodes for a given height. These trees, sometimes called Fibonacci trees, have a node count that follows the Fibonacci sequence:

```
N(h) = N(h-1) + N(h-2) + 1

Where N(h) is the minimum nodes in an AVL tree of height h:
  N(0) = 1
  N(1) = 2
  N(2) = 4
  N(3) = 7
  N(4) = 12
  ...
```

The golden ratio φ = (1 + √5)/2 ≈ 1.618 appears naturally in this analysis, as:

```
N(h) ≈ φʰ⁺² / √5

Therefore: h ≈ 1.44 × log₂(n)
```

| Tree Type | Worst-Case Height | Constant Factor |
|-----------|-------------------|-----------------|
| Perfect BST | log₂(n) | 1.0 |
| AVL Tree | 1.44 × log₂(n) | 1.44 |
| Red-Black Tree | 2 × log₂(n) | 2.0 |
| Unbalanced BST | n | n/log₂(n) |

### 3. The Four Rotation Types

When an insertion or deletion causes a node's balance factor to become ±2, a rotation operation restores balance. There are four cases, determined by the balance factors of the unbalanced node and its child:

#### Right Rotation (LL Case)
Applied when a node becomes left-heavy (+2) and its left child is left-heavy or balanced (+1 or 0):

```
        y                             x
       / \                           / \
      x   C    Right Rotate(y)      A   y
     / \       ===============>        / \
    A   B                             B   C

  Precondition: bf(y) = +2, bf(x) ∈ {+1, 0}
  Postcondition: bf(y) ∈ {0, -1}, bf(x) ∈ {0, -1}
```

#### Left Rotation (RR Case)
Applied when a node becomes right-heavy (-2) and its right child is right-heavy or balanced (-1 or 0):

```
      x                               y
     / \                             / \
    A   y      Left Rotate(x)       x   C
       / \     ===============>    / \
      B   C                       A   B

  Precondition: bf(x) = -2, bf(y) ∈ {-1, 0}
  Postcondition: bf(x) ∈ {0, +1}, bf(y) ∈ {0, +1}
```

#### Left-Right Rotation (LR Case)
Applied when a node becomes left-heavy (+2) but its left child is right-heavy (-1):

```
      z                    z                       y
     / \                  / \                     / \
    x   D   Left(x)      y   D   Right(z)       x   z
   / \      =======>    / \      =========>    / \ / \
  A   y                x   C                  A  B C  D
     / \              / \
    B   C            A   B

  Two rotations required: Left on left child, then Right on root
```

#### Right-Left Rotation (RL Case)
Applied when a node becomes right-heavy (-2) but its right child is left-heavy (+1):

```
    x                      x                         y
   / \                    / \                       / \
  A   z    Right(z)      A   y     Left(x)        x   z
     / \   =======>         / \    =========>    / \ / \
    y   D                  B   z                A  B C  D
   / \                        / \
  B   C                      C   D

  Two rotations required: Right on right child, then Left on root
```

---

## 🏭 Industrial Applications

### 1. Database Index Structures

Modern database management systems extensively employ self-balancing trees for index structures. PostgreSQL, MySQL and SQLite all use variants of B-trees (which generalise the AVL balancing concept to multi-way trees) for their primary indexes:

```c
/* Simplified B-tree node structure (PostgreSQL-style) */
typedef struct BTreeNode {
    int num_keys;
    int keys[MAX_KEYS];
    void *values[MAX_KEYS];
    struct BTreeNode *children[MAX_KEYS + 1];
    bool is_leaf;
} BTreeNode;

/* AVL trees inspire the balance maintenance in B-trees */
void btree_split_child(BTreeNode *parent, int index) {
    BTreeNode *full_child = parent->children[index];
    BTreeNode *new_sibling = btree_create_node(full_child->is_leaf);
    
    /* Redistribute keys whilst maintaining balance */
    int mid = (MAX_KEYS - 1) / 2;
    /* ... split operation preserves B-tree balance invariant ... */
}
```

### 2. Memory Allocators

High-performance memory allocators like jemalloc and tcmalloc use balanced tree structures to manage free memory blocks. The AVL property ensures that allocation and deallocation operations complete in logarithmic time:

```c
/* Memory block managed by AVL tree (simplified) */
typedef struct MemBlock {
    size_t size;
    void *address;
    int height;
    struct MemBlock *left;
    struct MemBlock *right;
} MemBlock;

/* Find best-fit block in O(log n) */
void *allocator_best_fit(MemBlock *root, size_t requested) {
    if (root == NULL) return NULL;
    
    if (root->size == requested) {
        return root->address;
    } else if (root->size > requested) {
        void *left_result = allocator_best_fit(root->left, requested);
        return left_result ? left_result : root->address;
    } else {
        return allocator_best_fit(root->right, requested);
    }
}
```

### 3. File System Directory Structures

Modern file systems like ext4 and NTFS use balanced tree variants for directory indexing. The ext4 htree (hash tree) structure maintains balance to ensure consistent lookup times:

```c
/* Linux kernel style directory entry (simplified) */
struct ext4_dir_entry {
    __le32 inode;           /* Inode number */
    __le16 rec_len;         /* Directory entry length */
    __u8 name_len;          /* Name length */
    __u8 file_type;         /* File type */
    char name[EXT4_NAME_LEN]; /* File name */
};

/* Directory lookup uses balanced tree for O(log n) access */
struct inode *ext4_lookup(struct inode *dir, struct dentry *dentry) {
    /* htree (based on balanced tree principles) provides 
       logarithmic lookup in large directories */
}
```

### 4. Network Routing Tables

High-speed routers use balanced tree structures for IP routing table lookups. The requirement for consistent worst-case performance makes AVL-style balancing essential:

```c
/* Routing table entry */
typedef struct RouteEntry {
    uint32_t network;       /* Network address */
    uint32_t netmask;       /* Subnet mask */
    uint32_t gateway;       /* Next hop */
    int metric;             /* Route priority */
    struct RouteEntry *left;
    struct RouteEntry *right;
    int height;
} RouteEntry;

/* Longest prefix match in O(log n) */
RouteEntry *route_lookup(RouteEntry *root, uint32_t dest_ip) {
    RouteEntry *best_match = NULL;
    
    while (root != NULL) {
        if ((dest_ip & root->netmask) == root->network) {
            best_match = root;
        }
        if (dest_ip < root->network) {
            root = root->left;
        } else {
            root = root->right;
        }
    }
    return best_match;
}
```

### 5. Compiler Symbol Tables

Compilers use balanced trees for symbol table management, ensuring efficient variable and function lookup during parsing and code generation:

```c
/* Symbol table entry (GCC-inspired) */
typedef struct Symbol {
    char *name;
    enum { SYM_VARIABLE, SYM_FUNCTION, SYM_TYPE } kind;
    struct Type *type;
    int scope_level;
    int offset;             /* Stack offset or address */
    struct Symbol *left;
    struct Symbol *right;
    int height;
} Symbol;

/* Symbol lookup during semantic analysis */
Symbol *symbol_lookup(Symbol *scope, const char *name) {
    if (scope == NULL) return NULL;
    
    int cmp = strcmp(name, scope->name);
    if (cmp == 0) return scope;
    if (cmp < 0) return symbol_lookup(scope->left, name);
    return symbol_lookup(scope->right, name);
}
```

---

## 💻 Laboratory Exercises

### Exercise 1: AVL Tree Foundation

Implement a complete AVL tree with basic operations:

**Requirements:**
1. Define the `AVLNode` structure with key, height and child pointers
2. Implement `height()` and `balance_factor()` utility functions
3. Implement right rotation (`rotate_right`)
4. Implement left rotation (`rotate_left`)
5. Implement `rebalance()` function that selects and applies the correct rotation
6. Implement `avl_insert()` that maintains the AVL property
7. Implement in-order traversal to verify BST property
8. Test with sequences that trigger all four rotation types

**Test Sequences:**
- LL case: Insert 30, 20, 10
- RR case: Insert 10, 20, 30
- LR case: Insert 30, 10, 20
- RL case: Insert 10, 30, 20

### Exercise 2: AVL Tree Advanced Operations

Extend the AVL tree with deletion and additional operations:

**Requirements:**
1. Implement `avl_find_min()` and `avl_find_max()`
2. Implement `avl_search()` for key lookup
3. Implement `avl_delete()` with proper rebalancing
4. Implement `avl_predecessor()` and `avl_successor()`
5. Implement `avl_range_query()` to find all keys in [low, high]
6. Implement `avl_height()` and `avl_size()` for tree statistics
7. Implement memory cleanup with `avl_destroy()`
8. Add validation function to verify AVL property

**Additional Challenges:**
- Handle deletion of nodes with two children (find inorder successor)
- Ensure memory is properly freed with no leaks
- Verify tree remains valid after each deletion

---

## 🔧 Compilation and Execution

```bash
# Build all targets
make all

# Build specific target
make example1
make exercise1
make exercise2

# Run the demonstration example
make run

# Run automated tests
make test

# Check for memory leaks
make valgrind

# Clean build artifacts
make clean

# Display help
make help
```

### Compiler Flags Explained

| Flag | Purpose |
|------|---------|
| `-Wall` | Enable all standard warnings |
| `-Wextra` | Enable additional warnings |
| `-std=c11` | Use C11 standard |
| `-g` | Include debugging symbols |
| `-O2` | Optimisation level 2 (for release) |
| `-fsanitize=address` | Runtime memory error detection |

---

## 📁 Directory Structure

```
week-09-avl-trees/
├── README.md                           # This documentation
├── Makefile                            # Build automation
│
├── slides/
│   ├── presentation-week09.html        # Main lecture (40 slides)
│   └── presentation-comparativ.html    # Pseudocode-C-Python comparison
│
├── src/
│   ├── example1.c                      # Complete AVL demonstration
│   ├── exercise1.c                     # Basic AVL implementation
│   └── exercise2.c                     # Advanced operations
│
├── data/
│   ├── random_keys.txt                 # Random test data
│   └── sorted_keys.txt                 # Worst-case test data
│
├── tests/
│   ├── test1_input.txt                 # Test input for exercise 1
│   ├── test1_expected.txt              # Expected output
│   ├── test2_input.txt                 # Test input for exercise 2
│   └── test2_expected.txt              # Expected output
│
├── teme/
│   ├── homework-requirements.md        # Homework specifications
│   └── homework-extended.md            # Bonus challenges
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
- Cormen, T.H. et al. *Introduction to Algorithms* (4th ed.), Chapter 13: Red-Black Trees (for comparison), Chapter 12: Binary Search Trees
- Sedgewick, R. and Wayne, K. *Algorithms* (4th ed.), Section 3.3: Balanced Search Trees
- Adelson-Velsky, G.M. and Landis, E.M. "An algorithm for the organisation of information" (1962) — the original paper

### Advanced
- Knuth, D.E. *The Art of Computer Programming*, Vol. 3: Sorting and Searching, Section 6.2.3
- Tarjan, R.E. *Data Structures and Network Algorithms*, SIAM, 1983
- Brass, P. *Advanced Data Structures*, Cambridge University Press, 2008

### Online Resources
- Visualgo AVL Tree Visualisation: https://visualgo.net/en/bst
- GeeksforGeeks AVL Tree Tutorial: https://www.geeksforgeeks.org/avl-tree-set-1-insertion/
- MIT OpenCourseWare 6.006: https://ocw.mit.edu/courses/6-006-introduction-to-algorithms/

---

## ✅ Self-Assessment Checklist

Before submitting your laboratory work, verify that you can:

- [ ] Define the AVL tree balance invariant and explain why it guarantees O(log n) height
- [ ] Calculate the balance factor of any node given its subtree heights
- [ ] Identify which rotation type is needed given a balance factor of ±2
- [ ] Perform right and left rotations on paper, tracking all pointer changes
- [ ] Explain why double rotations (LR and RL) require two single rotations
- [ ] Implement AVL insertion that correctly rebalances the tree
- [ ] Implement AVL deletion handling all cases (leaf, one child, two children)
- [ ] Use Valgrind to verify no memory leaks in your implementation
- [ ] Trace through an insertion/deletion sequence showing balance factors
- [ ] Compare AVL trees with Red-Black trees in terms of balance strictness and rotation frequency

---

## 💼 Interview Preparation

### Common Interview Questions on AVL Trees

1. **What is an AVL tree and why is it useful?**
   *Expected answer: Self-balancing BST with height difference ≤1 between subtrees. Guarantees O(log n) operations even with sorted input, unlike standard BSTs which degrade to O(n).*

2. **How do you determine which rotation to perform?**
   *Expected answer: Check balance factor of unbalanced node and its taller child. BF=+2 with left child +1/0 → Right rotation. BF=-2 with right child -1/0 → Left rotation. BF=+2 with left child -1 → Left-Right. BF=-2 with right child +1 → Right-Left.*

3. **What is the maximum height of an AVL tree with n nodes?**
   *Expected answer: Approximately 1.44 × log₂(n). Derivation involves Fibonacci numbers and the golden ratio.*

4. **Compare AVL trees with Red-Black trees.**
   *Expected answer: AVL stricter balance (height diff ≤1 vs factor of 2), more rotations on insert/delete, faster lookups. Red-Black: fewer rotations, faster modifications, used in most standard libraries (C++ std::map, Java TreeMap).*

5. **How would you implement an AVL tree iterator for in-order traversal?**
   *Expected answer: Use a stack to simulate recursion, or implement with parent pointers. Push all left children, pop and process, move to right child, repeat.*

---

## 🔗 Next Week Preview

**Week 10: Heaps and Priority Queues**

Next week we explore heap data structures, which provide a different approach to maintaining partial order. Key topics include:
- Binary heap property and array representation
- Heapify operations and heap construction
- Heap sort algorithm
- Priority queue implementation
- Applications in Dijkstra's algorithm and event simulation

The heap's array-based implementation offers interesting trade-offs compared to the pointer-based AVL tree—prepare to compare and contrast these approaches.

---

*Last updated: January 2025*
*Course: Algorithms and Programming Techniques (ATP)*
*Institution: Academy of Economic Studies, Bucharest*

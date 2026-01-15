# Week 07: Binary Trees

## 🎯 Learning Objectives

Upon completion of this laboratory, students will be able to:

1. **Remember** the fundamental terminology of binary trees including nodes, edges, root, leaves, depth, height and subtrees
2. **Understand** the recursive nature of tree structures and how this property enables elegant algorithmic solutions
3. **Apply** the three classical traversal algorithms (preorder, inorder, postorder) to process tree nodes systematically
4. **Analyse** the time and space complexity of tree operations, recognising O(n) traversal and O(h) search characteristics
5. **Evaluate** different tree representations (pointer-based vs array-based) for specific problem domains
6. **Create** complete binary tree implementations with insertion, deletion, search and traversal capabilities

---

## 📜 Historical Context

The binary tree data structure emerged from the pioneering work in computer science during the 1950s and 1960s, a period when researchers were formalising the mathematical foundations of algorithms and data organisation. The concept draws from graph theory, which itself originated with Leonhard Euler's solution to the Seven Bridges of Königsberg problem in 1736.

Binary trees gained prominence through the work of several key figures. John McCarthy's development of LISP in 1958 demonstrated the power of recursive data structures, whilst Donald Knuth's magisterial treatise *The Art of Computer Programming* (Volume 1, 1968) provided the definitive formalisation of tree algorithms. The notation and terminology we use today—preorder, inorder, postorder—were standardised during this era.

The recursive decomposition principle underlying binary trees—that a complex structure can be expressed as simpler instances of itself—represents one of the most profound insights in computer science. This principle connects binary trees to diverse domains: expression parsing, decision systems, hierarchical databases and the very structure of programming language syntax itself.

### Key Figure: Donald E. Knuth (1938–)

Donald Ervin Knuth, Professor Emeritus at Stanford University, stands as one of the most influential figures in the history of computer science. His multi-volume work *The Art of Computer Programming*, begun in 1962 and still ongoing, established rigorous mathematical analysis as the foundation of algorithm study. Knuth invented the TeX typesetting system and developed the concept of literate programming.

> *"An algorithm must be seen to be believed."* — Donald E. Knuth

Knuth's treatment of tree structures in TAOCP Volume 1 remains the authoritative reference, introducing the standard traversal algorithms and establishing the asymptotic analysis techniques now universal in the field.

---

## 📚 Theoretical Foundations

### 1. Binary Tree Terminology and Structure

A **binary tree** is a hierarchical data structure wherein each node contains at most two children, conventionally designated as the *left child* and *right child*. This constraint distinguishes binary trees from general trees, enabling specialised algorithms of remarkable elegance.

```
                    ┌───────────────────────────────────────┐
                    │           BINARY TREE ANATOMY         │
                    └───────────────────────────────────────┘

                                    ┌───┐
                           Root ──▶ │ A │ ◀── Level 0
                                    └─┬─┘
                           ┌──────────┴──────────┐
                           ▼                     ▼
                        ┌───┐                 ┌───┐
                        │ B │                 │ C │ ◀── Level 1
                        └─┬─┘                 └─┬─┘
                     ┌────┴────┐           ┌────┴────┐
                     ▼         ▼           ▼         ▼
                  ┌───┐     ┌───┐       ┌───┐     ┌───┐
                  │ D │     │ E │       │ F │     │ G │ ◀── Level 2
                  └───┘     └───┘       └───┘     └───┘
                    │                     │
                 Leaves ───────────────── Leaves

        Height = 2 (longest path from root to leaf)
        Size = 7 (total number of nodes)
        
        Parent of E: B          Children of C: F, G
        Siblings: (D,E), (F,G)  Ancestors of G: C, A
        Subtree rooted at B: {B, D, E}
```

**Key Properties:**

| Property | Definition | Example (above tree) |
|----------|------------|---------------------|
| **Root** | The topmost node with no parent | Node A |
| **Leaf** | A node with no children | Nodes D, E, F, G |
| **Internal node** | A node with at least one child | Nodes A, B, C |
| **Depth** | Distance from root to node | Depth of E = 2 |
| **Height** | Longest path from node to leaf | Height of tree = 2 |
| **Degree** | Number of children | Degree of B = 2 |

### 2. The Recursive Nature of Trees

The fundamental insight enabling elegant tree algorithms is the **recursive definition**:

> A binary tree is either **empty** (NULL) or consists of a **root node** with two binary trees as its **left subtree** and **right subtree**.

This self-referential structure means every subtree is itself a valid binary tree. Consequently, any operation on a tree can be expressed as:

1. Handle the base case (empty tree)
2. Process the current node
3. Recursively process left and right subtrees

```c
/* The canonical recursive tree structure in C */
typedef struct TreeNode {
    int data;                    /* Node payload */
    struct TreeNode *left;       /* Left subtree (may be NULL) */
    struct TreeNode *right;      /* Right subtree (may be NULL) */
} TreeNode;

/* Recursive pattern template */
ReturnType tree_operation(TreeNode *node) {
    /* Base case: empty tree */
    if (node == NULL) {
        return BASE_VALUE;
    }
    
    /* Recursive case: combine results from subtrees */
    ReturnType left_result = tree_operation(node->left);
    ReturnType right_result = tree_operation(node->right);
    
    return combine(node->data, left_result, right_result);
}
```

### 3. The Three Classical Traversals

Traversal algorithms visit every node exactly once in a systematic order. The three fundamental traversals differ only in *when* they process the current node relative to its subtrees:

```
        ┌────────────────────────────────────────────────────────────┐
        │              THE THREE TRAVERSALS VISUALISED               │
        └────────────────────────────────────────────────────────────┘

                                    ┌───┐
                                    │ 1 │
                                    └─┬─┘
                             ┌────────┴────────┐
                             ▼                 ▼
                          ┌───┐             ┌───┐
                          │ 2 │             │ 3 │
                          └─┬─┘             └─┬─┘
                       ┌────┴────┐       ┌────┴────┐
                       ▼         ▼       ▼         ▼
                    ┌───┐     ┌───┐   ┌───┐     ┌───┐
                    │ 4 │     │ 5 │   │ 6 │     │ 7 │
                    └───┘     └───┘   └───┘     └───┘

        ╔════════════════╦═══════════════════════╦════════════════════╗
        ║   TRAVERSAL    ║        ORDER          ║      SEQUENCE      ║
        ╠════════════════╬═══════════════════════╬════════════════════╣
        ║   Preorder     ║  Node → Left → Right  ║  1, 2, 4, 5, 3, 6, 7 ║
        ║   Inorder      ║  Left → Node → Right  ║  4, 2, 5, 1, 6, 3, 7 ║
        ║   Postorder    ║  Left → Right → Node  ║  4, 5, 2, 6, 7, 3, 1 ║
        ╚════════════════╩═══════════════════════╩════════════════════╝

        Mnemonic: "Pre" = node first, "Post" = node last, "In" = node in middle
```

**Traversal Implementations:**

```c
/* Preorder: process node BEFORE children */
void preorder(TreeNode *node) {
    if (node == NULL) return;
    process(node->data);        /* Node first */
    preorder(node->left);       /* Then left */
    preorder(node->right);      /* Then right */
}

/* Inorder: process node BETWEEN children */
void inorder(TreeNode *node) {
    if (node == NULL) return;
    inorder(node->left);        /* Left first */
    process(node->data);        /* Node in middle */
    inorder(node->right);       /* Right last */
}

/* Postorder: process node AFTER children */
void postorder(TreeNode *node) {
    if (node == NULL) return;
    postorder(node->left);      /* Left first */
    postorder(node->right);     /* Right second */
    process(node->data);        /* Node last */
}
```

### 4. Complexity Analysis

| Operation | Time Complexity | Space Complexity | Notes |
|-----------|-----------------|------------------|-------|
| Traversal | O(n) | O(h) | h = height, stack frames |
| Search | O(n) | O(h) | Worst case for general tree |
| Insertion | O(1) | O(1) | After finding position |
| Height calculation | O(n) | O(h) | Must visit all nodes |
| Node counting | O(n) | O(h) | Must visit all nodes |

**Space Complexity Insight:** Recursive traversals consume O(h) stack space where h is the tree height. For a balanced tree, h = O(log n); for a degenerate tree (linked-list-like), h = O(n).

```
    ┌────────────────────────────────────────────────────────────────┐
    │                  TREE SHAPE AND COMPLEXITY                     │
    └────────────────────────────────────────────────────────────────┘

    BALANCED (h = log n)              DEGENERATE (h = n)
    
           ○                                 ○
          / \                                 \
         ○   ○                                 ○
        / \ / \                                 \
       ○  ○ ○  ○                                 ○
                                                 \
    Height = 2 for 7 nodes                        ○
    Operations: O(log n)
                                         Height = 3 for 4 nodes
                                         Operations: O(n)
```

---

## 🏭 Industrial Applications

### 1. Expression Trees (Compilers)

Compilers represent mathematical expressions as binary trees, enabling evaluation and optimisation:

```c
/* Expression tree for: (3 + 4) * 5 */
/*
           *
          / \
         +   5
        / \
       3   4
       
   Inorder:  3 + 4 * 5  (ambiguous without parentheses)
   Postorder: 3 4 + 5 * (Reverse Polish Notation)
   Preorder:  * + 3 4 5 (Polish Notation)
*/

typedef struct ExprNode {
    char operator;
    int value;
    int is_operator;
    struct ExprNode *left, *right;
} ExprNode;

int evaluate(ExprNode *node) {
    if (!node->is_operator) return node->value;
    
    int left = evaluate(node->left);
    int right = evaluate(node->right);
    
    switch (node->operator) {
        case '+': return left + right;
        case '-': return left - right;
        case '*': return left * right;
        case '/': return left / right;
    }
    return 0;
}
```

### 2. DOM Tree (Web Browsers)

Web browsers parse HTML into a binary tree structure (first-child/next-sibling representation):

```c
/* Simplified DOM node structure */
typedef struct DOMNode {
    char tag[32];
    char *content;
    struct DOMNode *first_child;
    struct DOMNode *next_sibling;
} DOMNode;

/* Recursive rendering */
void render(DOMNode *node, int indent) {
    if (node == NULL) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    printf("<%s>\n", node->tag);
    
    render(node->first_child, indent + 1);
    render(node->next_sibling, indent);
}
```

### 3. File System Hierarchy (Unix/Linux)

Directory structures form trees, traversed recursively:

```c
/* Recursive directory size calculation */
#include <dirent.h>
#include <sys/stat.h>

long calculate_size(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    
    if (!S_ISDIR(st.st_mode)) return st.st_size;
    
    long total = st.st_size;
    DIR *dir = opendir(path);
    struct dirent *entry;
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || 
            strcmp(entry->d_name, "..") == 0) continue;
        
        char child_path[PATH_MAX];
        snprintf(child_path, sizeof(child_path), "%s/%s", path, entry->d_name);
        total += calculate_size(child_path);  /* Recursive call */
    }
    closedir(dir);
    return total;
}
```

### 4. Decision Trees (Machine Learning)

Classification algorithms use binary trees for decision making:

```c
typedef struct DecisionNode {
    char feature[64];
    double threshold;
    char *classification;  /* NULL if not leaf */
    struct DecisionNode *yes_branch;
    struct DecisionNode *no_branch;
} DecisionNode;

const char* classify(DecisionNode *node, double *features, int feature_count) {
    if (node->classification != NULL) {
        return node->classification;  /* Leaf: return class */
    }
    
    /* Internal node: descend based on feature value */
    int feature_idx = get_feature_index(node->feature);
    if (features[feature_idx] <= node->threshold) {
        return classify(node->yes_branch, features, feature_count);
    } else {
        return classify(node->no_branch, features, feature_count);
    }
}
```

### 5. GTK+ Widget Hierarchy

GUI frameworks organise widgets in tree structures:

```c
/* GTK widget tree traversal example */
void print_widget_tree(GtkWidget *widget, int depth) {
    for (int i = 0; i < depth; i++) printf("  ");
    printf("%s\n", G_OBJECT_TYPE_NAME(widget));
    
    if (GTK_IS_CONTAINER(widget)) {
        GList *children = gtk_container_get_children(GTK_CONTAINER(widget));
        for (GList *l = children; l != NULL; l = l->next) {
            print_widget_tree(GTK_WIDGET(l->data), depth + 1);
        }
        g_list_free(children);
    }
}
```

---

## 💻 Laboratory Exercises

### Exercise 1: Tree Construction and Traversal

Implement a complete binary tree with the following capabilities:

**Requirements:**
1. Define a `TreeNode` structure with integer data and left/right pointers
2. Implement `create_node()` to allocate and initialise a new node
3. Implement all three traversals: `preorder()`, `inorder()`, `postorder()`
4. Implement `count_nodes()` to return total node count
5. Implement `calculate_height()` to determine tree height
6. Implement `count_leaves()` to count leaf nodes
7. Implement `tree_sum()` to calculate sum of all node values
8. Implement `free_tree()` to properly deallocate memory

**Input:** Series of integers to insert into the tree
**Output:** Traversal sequences, statistics and verification of correct deallocation

### Exercise 2: Expression Tree Evaluator

Build an expression tree from postfix notation and evaluate it:

**Requirements:**
1. Parse postfix expressions (e.g., "3 4 + 5 *")
2. Construct expression tree using stack-based algorithm
3. Implement `evaluate()` using postorder traversal
4. Implement `to_infix()` to convert back to infix notation with parentheses
5. Implement `to_prefix()` to generate prefix notation
6. Handle the four basic operators: +, -, *, /
7. Detect and report division by zero
8. Support multi-digit integers

**Input:** Postfix expression string
**Output:** Expression tree visualisation, evaluation result and alternative notations

---

## 🔧 Compilation and Execution

```bash
# Build all targets
make

# Build specific target
make example1
make exercise1
make exercise2

# Run example demonstration
make run

# Execute automated tests
make test

# Check for memory leaks
make valgrind

# Clean build artefacts
make clean

# Display help
make help
```

**Compiler flags explained:**
- `-Wall` : Enable all standard warnings
- `-Wextra` : Enable additional warnings
- `-std=c11` : Use C11 standard
- `-g` : Include debugging symbols (for GDB/Valgrind)

---

## 📁 Directory Structure

```
week-07-binary-trees/
├── README.md                           # This documentation
├── Makefile                            # Build automation
│
├── slides/
│   ├── presentation-week07.html        # Main lecture (37 slides)
│   └── presentation-comparativ.html    # Pseudocode → C → Python comparison
│
├── src/
│   ├── example1.c                      # Complete binary tree demonstration
│   ├── exercise1.c                     # Tree construction exercise (12 TODOs)
│   └── exercise2.c                     # Expression tree exercise (14 TODOs)
│
├── data/
│   ├── tree_data.txt                   # Sample tree construction data
│   └── expressions.txt                 # Sample postfix expressions
│
├── tests/
│   ├── test1_input.txt                 # Test input for exercise 1
│   ├── test1_expected.txt              # Expected output for exercise 1
│   ├── test2_input.txt                 # Test input for exercise 2
│   └── test2_expected.txt              # Expected output for exercise 2
│
├── teme/
│   ├── homework-requirements.md        # 2 homework assignments (50 pts each)
│   └── homework-extended.md            # 5 bonus challenges (+10 pts each)
│
└── solution/
    ├── exercise1_sol.c                 # Complete solution for exercise 1
    ├── exercise2_sol.c                 # Complete solution for exercise 2
    ├── homework1_sol.c                 # Solution for homework 1
    └── homework2_sol.c                 # Solution for homework 2
```

---

## 📖 Recommended Reading

### Essential
- Knuth, D.E. (1997). *The Art of Computer Programming, Volume 1: Fundamental Algorithms*, 3rd ed., §2.3 "Trees"
- Cormen, T.H. et al. (2022). *Introduction to Algorithms*, 4th ed., Chapter 12 "Binary Search Trees"
- Sedgewick, R. & Wayne, K. (2011). *Algorithms*, 4th ed., §3.2 "Binary Search Trees"

### Advanced
- Tarjan, R.E. (1983). *Data Structures and Network Algorithms*, SIAM
- Aho, A.V., Lam, M.S., Sethi, R. & Ullman, J.D. (2006). *Compilers: Principles, Techniques, and Tools*, 2nd ed. (expression trees)
- Skiena, S.S. (2020). *The Algorithm Design Manual*, 3rd ed., §3.4 "Binary Search Trees"

### Online Resources
- Visualgo.net - Binary Tree Visualisation: https://visualgo.net/en/bst
- NIST Dictionary of Algorithms - Binary Tree: https://xlinux.nist.gov/dads/HTML/binarytree.html
- GeeksforGeeks Tree Traversals: https://www.geeksforgeeks.org/tree-traversals-inorder-preorder-and-postorder/

---

## ✅ Self-Assessment Checklist

Before submitting your work, verify that you can:

- [ ] Define a binary tree node structure with appropriate fields
- [ ] Implement recursive tree creation with proper memory allocation
- [ ] Write correct base cases for recursive tree functions
- [ ] Implement preorder, inorder and postorder traversals
- [ ] Calculate tree height using recursion
- [ ] Count total nodes, leaves and internal nodes
- [ ] Properly deallocate tree memory without leaks
- [ ] Build an expression tree from postfix notation
- [ ] Evaluate expression trees using recursive descent
- [ ] Convert between prefix, infix and postfix notations

---

## 💼 Interview Preparation

### Common Binary Tree Interview Questions

1. **Implement level-order (breadth-first) traversal**
   - Requires: Queue data structure
   - Key insight: Process nodes level by level using FIFO ordering

2. **Find the lowest common ancestor of two nodes**
   - Requires: Understanding of tree paths
   - Key insight: Traverse from root, split point is LCA

3. **Check if a binary tree is balanced**
   - Definition: Heights of subtrees differ by at most 1
   - Key insight: Recursive height check with early termination

4. **Serialise and deserialise a binary tree**
   - Requires: Preorder with NULL markers
   - Key insight: Preorder uniquely identifies structure with NULLs

5. **Mirror (invert) a binary tree**
   - Classic recursive swap problem
   - Key insight: Swap children recursively at each node

---

## 🔗 Next Week Preview

**Week 08: Binary Search Trees (BST)**

Building upon the binary tree foundation, we shall explore the *ordered* binary tree variant that enables O(log n) search, insertion and deletion. Topics include:

- BST property: left < node < right
- Search, insertion and deletion algorithms
- In-order traversal yields sorted sequence
- Balancing considerations and degenerate cases

The BST represents the gateway to self-balancing trees (AVL, Red-Black) covered in subsequent weeks.

---

*Laboratory materials prepared for ASE-CSIE Algorithms and Programming Techniques course.*
*Week 07 of 14 | Binary Trees | British English Edition*

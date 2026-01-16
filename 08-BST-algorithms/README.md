# Week 08: Binary Search Trees in C (C11)

## 1. Scope and intellectual objectives

This laboratory develops a principled implementation of a **Binary Search Tree (BST)** and uses it to practise three habits of mind that are indispensable in low level algorithm engineering:

1. **Invariant thinking**: the BST ordering predicate is treated as a formal constraint on the pointer graph rather than as an informal slogan.
2. **Complexity reasoning**: the cost of each operation is analysed in terms of height and then translated into best case, average case and worst case bounds.
3. **Ownership discipline**: every allocation implies a deallocation path and every structural update is justified in terms of both correctness and memory safety.

A BST is not merely a binary tree with a convenient search routine. It is a data structure whose **semantic content** is the set of stored keys and whose **representation** is a directed acyclic pointer graph constrained by an ordering relation. The central pedagogical claim of the week is that once the ordering relation is written down precisely, the implementation becomes an exercise in maintaining that relation under mutation.

The repository is structured as a small, testable artefact rather than as a collection of loosely related fragments. Each student exercise is validated by a deterministic output contract under `make test`.

## 2. Repository layout and artefact roles

```
08-BST-algorithms/
  src/
    example1.c        Demonstration programme covering the full BST toolchain
    exercise1.c       Student exercise: basic BST operations (insert, search, traversal)
    exercise2.c       Student exercise: deletion and advanced BST queries
  solution/
    exercise1_sol.c   Instructor reference for Exercise 1
    exercise2_sol.c   Instructor reference for Exercise 2
    homework1_sol.c   Instructor reference: contact directory using a BST of strings
    homework2_sol.c   Instructor reference: expression tree evaluator built from postfix
  tests/
    test1_input.txt   Input script for Exercise 1
    test1_expected.txt Golden output for Exercise 1
    test2_input.txt   Input script for Exercise 2
    test2_expected.txt Golden output for Exercise 2
  data/
    sample_keys.txt   Representative key sets for experimentation
    operations.txt    Representative operation scripts for manual exploration
  slides/
    presentation-week08.html       Lecture slides
    presentation-comparativ.html   Comparative notes and examples
  teme/
    homework-requirements.md       Homework specification
    homework-extended.md           Optional extension challenges
```

The separation between `src/` and `solution/` exists to support a teaching workflow in which students implement the required operations whilst instructors retain a reference implementation for marking and debugging.

## 3. Formal model of a BST

### 3.1 Structural model

A BST is a rooted binary tree with unique keys. Each node stores a key and has two outgoing child pointers `left` and `right` that are either `NULL` or point to other nodes.

The canonical C representation used throughout the week is:

```c
typedef struct BSTNode {
    int key;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;
```

This model is intentionally minimal: it makes pointer manipulations explicit and keeps the proof obligations local.

### 3.2 Ordering invariant

Let `Keys(T)` be the set of keys stored in tree `T` and let `Left(x)` and `Right(x)` denote the left and right subtrees of node `x`. The BST invariant can be written as:

- For every node `x` in the tree:
  - for all keys `k` in `Keys(Left(x))`, `k < x.key`
  - for all keys `k` in `Keys(Right(x))`, `k > x.key`

Equivalently, the tree is a valid BST if and only if there exists an assignment of an interval `(min, max)` to each node such that the node key lies strictly inside its interval and each child interval is a strict subinterval induced by the parent key.

### 3.3 Duplicate policy

The student exercises implement the common textbook policy:

- if an insertion key is already present then insertion performs no structural change

This policy simplifies validation because the strict inequality form of the invariant can be used without additional bookkeeping.

## 4. Algorithmic core

Throughout this section, let `h` be the height of the tree and let `n` be the number of stored keys.

### 4.1 Search

Search exploits the invariant by eliminating half of the remaining search space at each step, provided the tree is reasonably balanced.

#### Iterative search (preferred in C)

Pseudocode:

```
SEARCH(root, key):
    current ← root
    while current ≠ NULL:
        if key = current.key:
            return current
        else if key < current.key:
            current ← current.left
        else:
            current ← current.right
    return NULL
```

Properties:
- time complexity: `O(h)`
- auxiliary space: `O(1)`

Iterative search is preferable in C because it avoids recursion depth concerns and imposes no additional stack usage.

### 4.2 Insertion

Insertion is a constructive operation: it modifies the pointer graph so that the BST invariant holds after the modification.

Pseudocode (recursive insertion with duplicate suppression):

```
INSERT(root, key):
    if root = NULL:
        return new_node(key)
    if key < root.key:
        root.left ← INSERT(root.left, key)
    else if key > root.key:
        root.right ← INSERT(root.right, key)
    else:
        do nothing
    return root
```

Properties:
- time complexity: `O(h)`
- auxiliary space: `O(h)` due to recursion

A fully iterative insertion is possible and often preferable for production code. The recursive form is pedagogically useful because it matches the tree’s inductive structure and makes the proof of invariant preservation almost mechanical.

### 4.3 Traversals

Traversals are representation dependent but semantics preserving. They do not change the set of stored keys but they expose different structural views.

- **In order** (left, node, right) yields keys in ascending order for a BST.
- **Pre order** (node, left, right) is useful for copying and serialisation.
- **Post order** (left, right, node) is essential for safe deallocation.

Pseudocode for in order traversal:

```
INORDER(node):
    if node = NULL: return
    INORDER(node.left)
    visit(node)
    INORDER(node.right)
```

### 4.4 Minimum and maximum

The minimum key is located at the leftmost node and the maximum at the rightmost node.

Pseudocode:

```
FIND_MIN(root):
    if root = NULL: return NULL
    while root.left ≠ NULL:
        root ← root.left
    return root
```

Time: `O(h)` and in the best case `O(1)` when the root is already the minimum.

### 4.5 Deletion (Hibbard style successor replacement)

Deletion is the operation that most clearly exposes the need for an invariant centred perspective. The repository implements the classical three case deletion using the in order successor.

Let `x` be the node whose key equals the deletion key.

- **Case A (leaf)**: remove `x` and set the corresponding parent pointer to `NULL`.
- **Case B (one child)**: splice out `x` by linking its parent directly to its only child.
- **Case C (two children)**:
  1. find the in order successor `s` of `x` (the minimum node in `x.right`)
  2. copy `s.key` into `x.key` (this preserves the BST ordering constraints around `x`)
  3. delete `s` recursively from `x.right` (now `s` is in Case A or Case B)

Pseudocode:

```
DELETE(root, key):
    if root = NULL: return NULL
    if key < root.key:
        root.left ← DELETE(root.left, key)
        return root
    if key > root.key:
        root.right ← DELETE(root.right, key)
        return root

    # key = root.key, delete this node
    if root.left = NULL:
        temp ← root.right
        free(root)
        return temp
    if root.right = NULL:
        temp ← root.left
        free(root)
        return temp

    successor ← FIND_MIN(root.right)
    root.key ← successor.key
    root.right ← DELETE(root.right, successor.key)
    return root
```

The algorithm above is correct because successor replacement preserves the sortedness of the in order sequence. However it is not balance preserving: over long sequences of operations the tree may drift towards degeneracy. This is precisely why self balancing variants exist and why one should avoid treating a plain BST as a universal index.

### 4.6 Range search

Range search demonstrates how invariants can be used to prune work beyond simple equality search.

Pseudocode:

```
RANGE_SEARCH(node, low, high):
    if node = NULL: return
    if node.key > low:
        RANGE_SEARCH(node.left, low, high)
    if low ≤ node.key ≤ high:
        output(node.key)
    if node.key < high:
        RANGE_SEARCH(node.right, low, high)
```

This procedure performs an in order traversal restricted to nodes that could possibly fall inside the interval. The pruning conditions are logically forced by the BST invariant.

### 4.7 Kth smallest element

The repository implements kth smallest using an in order traversal with a counter. This is optimal in space and simplicity but has worst case `O(n)` time even for a balanced tree when `k` is large.

Pseudocode:

```
KTH(node, k, count, result):
    if node = NULL or result is set: return
    KTH(node.left, k, count, result)
    count ← count + 1
    if count = k:
        result ← node.key
        return
    KTH(node.right, k, count, result)
```

Alternative design (not implemented here): store subtree sizes at each node so that kth can be found in `O(h)` time. That design requires more complex invariant maintenance during insertion and deletion.

### 4.8 Lowest common ancestor in a BST

For a BST, the LCA of two keys is the unique node at which their search paths diverge.

Pseudocode:

```
LCA(root, a, b):
    if root = NULL: return NULL
    if a > b: swap(a, b)
    if b < root.key:
        return LCA(root.left, a, b)
    if a > root.key:
        return LCA(root.right, a, b)
    return root
```

This implementation assumes both keys exist in the tree. In a production setting one would validate existence before reporting an LCA to avoid returning a misleading node.

### 4.9 Validation by range propagation

Validation is most naturally written as a range propagation problem.

Pseudocode:

```
IS_VALID(node, min, max):
    if node = NULL: return true
    if node.key ≤ min or node.key ≥ max: return false
    return IS_VALID(node.left, min, node.key) and IS_VALID(node.right, node.key, max)
```

In C, `INT_MIN` and `INT_MAX` are used as sentinel bounds. This is safe for the strict inequality formulation when keys are ordinary integers, but for general key types or for policies that allow duplicates one must adjust the predicate accordingly.

### 4.10 Visual printing

The rotated printing method is a presentation algorithm rather than a data structure algorithm. It is nonetheless a useful debugging tool because it reveals the tree shape and therefore offers evidence about balance or degeneracy.

The algorithm prints the right subtree first so that it appears above the root on the terminal.

```
PRINT(node, space, indent):
    if node = NULL: return
    space ← space + indent
    PRINT(node.right, space, indent)
    print newline and (space - indent) spaces and node.key
    PRINT(node.left, space, indent)
```

## 5. Complexity analysis

### 5.1 Height as the controlling parameter

Most BST operations are `O(h)` where `h` is the tree height. The relationship between `h` and `n` depends on shape:

- perfectly balanced BST: `h = floor(log2 n)`
- degenerate BST (linked list): `h = n - 1`

### 5.2 Best case, average case and worst case

- Best case: `O(1)` when the root already satisfies the query (searching for the root key, finding min when root is min and similar).
- Worst case: `O(n)` for search, insertion and deletion when the tree degenerates.
- Expected case under random insertion order: height grows logarithmically with `n` and therefore the expected cost of the fundamental operations is `O(log n)`.

A classical quantitative statement is that the expected height of a random BST is asymptotically `α ln n` with `α ≈ 4.311` which corresponds to approximately `2.99 log2 n`.

## 6. Exercisable command languages

The student programmes are small interpreters that execute operation scripts.

### 6.1 Exercise 1 grammar

Input format:

- first line: integer `n`, the number of operations
- next `n` lines: one operation per line

Commands:

- `INSERT k`
- `SEARCH k`
- `INORDER`
- `PREORDER`
- `POSTORDER`
- `MIN`
- `MAX`

The output is intentionally strict because tests compare against golden files.

### 6.2 Exercise 2 grammar

Input format:

- first line: initial keys separated by spaces
- second line: integer `n`, the number of operations
- next `n` lines: one operation per line

Commands:

- `HEIGHT`
- `COUNT`
- `LEAVES`
- `DELETE k`
- `INORDER`
- `RANGE low high`
- `KTH k`
- `LCA a b`
- `PRINT`
- `VALID`

## 7. Testing methodology

### 7.1 Deterministic regression testing

The repository uses golden file testing:

- `tests/test1_input.txt` drives `exercise1`
- `tests/test1_expected.txt` is the required output
- `tests/test2_input.txt` drives `exercise2`
- `tests/test2_expected.txt` is the required output

Run:

```bash
make clean
make test
```

The goal is not only to obtain the correct set of keys but also to produce a stable interface. For this reason the traversal functions in the exercises print a trailing space after each key. This design choice makes the output format trivial to generate and also reduces the risk of off by one errors in printing logic.

### 7.2 Memory correctness

A tree is a heap allocated graph and therefore memory correctness is part of functional correctness. The canonical free routine is post order and should be treated as non optional.

Where available, Valgrind can be used via `make valgrind`.

## 8. Common failure modes and diagnostic strategies

1. **Incorrect deletion in Case C**: failure to delete the successor after copying it produces a duplicate key and violates the strict BST invariant.
2. **Memory leaks**: freeing the parent before children loses reachability of allocated subtrees.
3. **Range search without pruning**: a naive traversal over the entire tree ignores the BST property and becomes needlessly expensive.
4. **kth smallest without early termination**: continuing traversal after finding the kth node wastes time and can overwrite the result.
5. **Invalid LCA assumptions**: reporting an LCA without verifying key existence can produce plausible but wrong answers.

A disciplined debugging approach is to validate the BST after each mutation using the range based validator. If validation fails, the most recent structural change is the prime suspect.

## 9. Cross-language correspondence (conceptual notational equivalence)

The algorithms in this week are language independent. Only the memory model differs.

### 9.1 C (manual ownership)

```c
BSTNode *insert(BSTNode *root, int key) {
    if (!root) return new_node(key);
    if (key < root->key) root->left = insert(root->left, key);
    else if (key > root->key) root->right = insert(root->right, key);
    return root;
}
```

### 9.2 C++ (RAII via smart pointers)

```cpp
struct Node {
    int key;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};
```

The structural recursion is identical but deallocation is implicit.

### 9.3 Java (garbage collection)

```java
class Node {
    int key;
    Node left;
    Node right;
}
```

Garbage collection eliminates explicit frees, but it does not eliminate logical errors in pointer updates.

### 9.4 Python (dynamic objects)

```python
class Node:
    def __init__(self, key):
        self.key = key
        self.left = None
        self.right = None
```

The main conceptual difference is that recursion depth is more likely to be a practical limitation in Python for large degenerate trees.


## 10. Correctness sketches and invariant preservation

The unit tests shipped with this repository are **behavioural oracles** in the narrow sense that they validate observable output for two concrete input scripts. They are therefore necessary but not sufficient for a full mathematical argument of correctness. The purpose of this section is to provide a minimal but rigorous scaffold that connects implementation structure to the BST invariant and to show where correctness obligations actually arise.

### 10.1 Insertion preserves the BST invariant

Let a BST be defined by the predicate:

- for every node `x`, every key in `x.left` is strictly smaller than `x.key`
- for every node `x`, every key in `x.right` is strictly greater than `x.key`

The implementation in `src/exercise1.c` and `src/exercise2.c` performs insertion by descending left when `key < root->key` and right when `key > root->key`. It terminates only when it reaches a null position, at which point it allocates a new node.

**Proof sketch (structural induction on the recursion).**

- Base case: inserting into an empty tree returns a singleton node whose left and right pointers are null, hence the predicate holds vacuously.
- Inductive step: assume the predicate holds for the existing tree rooted at `root`. If `key < root->key`, insertion recurses into `root.left`. By the inductive hypothesis, the recursive call returns a BST whose keys remain strictly smaller than `root->key` because the call does not modify any value in the right subtree and because all comparisons were performed against `root->key` before descent. Symmetrically for the right case. Duplicate keys are ignored by design, which avoids the need to specify an equality policy.

The crucial implementation detail is not the recursion itself but the **assignment** back to the child pointer:

```c
root->left = bst_insert(root->left, key);
```

This ensures the constructed subtree becomes part of the parent structure without losing references, hence maintaining both semantic correctness (the key becomes reachable) and memory safety (no orphaned allocations).

### 10.2 Deletion and Hibbard style successor replacement

Deletion is the first operation in which pointer reassignment can disconnect a non-trivial subtree if performed incorrectly. The algorithm implemented in `src/exercise2.c` is the standard three-case deletion scheme sometimes referred to as *successor replacement*:

1. **Leaf node**: free the node and return `NULL`.
2. **One child**: free the node and return the non-null child pointer.
3. **Two children**: find the in-order successor (minimum in the right subtree), copy its key into the deleted node position then recursively delete the successor in the right subtree.

#### 10.2.1 Why the successor key is valid

Let `x` be the node to delete and `s` be the minimum element in `x.right`. By definition:

- `s.key` is greater than `x.key` because it lies in the right subtree of `x`.
- `s.key` is smaller than or equal to every key in `x.right` because it is the minimum in that subtree.

After copying `s.key` into `x.key`, the left subtree of `x` still contains keys strictly smaller than the old `x.key` and therefore still smaller than `s.key` because `s.key` was drawn from the right side. The right subtree of `x` still contains keys strictly greater than or equal to `s.key` by minimality. The subsequent recursive deletion removes the duplicate successor node, restoring uniqueness.

#### 10.2.2 Equivalent predecessor strategy and symmetry

A deletion strategy based on the in-order predecessor is symmetric: replace with the maximum element in the left subtree. The two strategies differ mainly in constant factors and in how they bias structural changes. Neither strategy guarantees balance. In an educational repository such as this one, successor replacement is preferable because `bst_find_min` is already a canonical primitive and its correctness is visually intuitive.

### 10.3 Range query correctness

The range search function prints all keys `k` with `low ≤ k ≤ high` in sorted order. Its correctness rests on two observations:

- If `root->key ≤ low`, then every key in the left subtree is strictly smaller than `root->key` and therefore strictly smaller than or equal to `low`. The left subtree can be skipped without losing any admissible keys.
- If `root->key ≥ high`, then every key in the right subtree is strictly larger than `root->key` and therefore strictly larger than or equal to `high`. The right subtree can be skipped.

The implementation therefore performs an in-order style traversal but guards each recursive descent with a pruning predicate. This produces sorted output because the visit order is identical to in-order traversal on the induced subtree of admissible keys.

### 10.4 The kth smallest element and two design points

The provided `bst_kth_smallest` uses a counter during in-order traversal. It is intentionally simple and aligned with the week’s focus on reasoning rather than augmentation.

- Time complexity is `Θ(h + k)` for a balanced tree when `k` is small and `Θ(n)` in the worst case.
- Space complexity is `Θ(h)` because recursion depth equals height.

A production-quality variant augments each node with `subtree_size`, enabling selection in `O(h)` time:

```
SELECT(node, k):
  left_size = size(node.left)
  if k == left_size + 1: return node.key
  if k <= left_size: return SELECT(node.left, k)
  return SELECT(node.right, k - left_size - 1)
```

The augmentation requires additional invariant maintenance during insertion and deletion, which is why it is not the default teaching implementation.

### 10.5 Lowest common ancestor correctness in a BST

The LCA algorithm exploits ordered paths. Let `a` and `b` be the two keys with `a ≤ b` after normalisation.

- If `b < root->key`, both keys lie in the left subtree, so the LCA must lie in the left subtree.
- If `a > root->key`, both keys lie in the right subtree.
- Otherwise the search paths diverge at `root` or one key equals `root->key`, hence `root` is the deepest common ancestor.

This argument is correct only if both keys exist. If existence is not guaranteed, the algorithm must be coupled with membership checks.

## 11. Worked traces for the included exercises

This repository is designed so that each assessed programme has an explicitly stated command language and a deterministic transcript. The following traces match `tests/test1_input.txt` and `tests/test2_input.txt`.

### 11.1 Exercise 1 trace: fundamental operations

Input script:

```
12
INSERT 50
INSERT 30
INSERT 70
INSERT 20
INSERT 40
INSERT 60
INSERT 80
SEARCH 40
SEARCH 100
INORDER
MIN
MAX
```

State after the insertions:

```
        50
      /    \
    30      70
   /  \    /  \
 20   40  60   80
```

Key properties exercised:

- `SEARCH 40` demonstrates the pruning property: the right subtree of 50 is ignored immediately.
- `INORDER` demonstrates that in-order traversal is a sorted enumeration because of the BST invariant.
- `MIN` and `MAX` demonstrate that extremal selection is a single-sided descent.

### 11.2 Exercise 2 trace: deletion and advanced queries

Initial keys are read from a single line, hence the construction order is deterministic.

Input script:

```
50 30 70 20 40 60 80
8
HEIGHT
COUNT
LEAVES
DELETE 30
INORDER
RANGE 25 65
KTH 3
LCA 20 60
```

Deletion of key 30 triggers **case 3** (two children). The in-order successor of 30 is 40. After replacement and successor deletion, the tree is:

```
        50
      /    \
    40      70
   /       /  \
 20      60   80
```

Observations:

- Height remains 2 because the longest root to leaf path length is unchanged.
- Leaf count becomes 3 if computed after deletion, but the test queries leaves before deletion, hence 4.
- Range query `[25, 65]` prunes the entire left branch below 20 and the entire right branch above 70.

## 12. Optional augmentations and self-balancing outlook

A plain BST is structurally fragile under adversarial insertion orders. In a laboratory setting this fragility is useful because it makes the connection between height and complexity explicit. In systems work, the fragility motivates balancing mechanisms.

### 12.1 Rotations as local restructuring primitives

A rotation changes shape without changing the in-order sequence of keys. The following pseudocode expresses the right rotation used in AVL and Red-Black balancing.

```
RIGHT_ROTATE(y):
  x = y.left
  T2 = x.right
  x.right = y
  y.left = T2
  return x
```

The corresponding left rotation is symmetric. If parent pointers are maintained, the rotation must additionally rewire parent links. The key correctness fact is that rotations preserve the in-order sequence and therefore preserve the BST invariant.

### 12.2 Augmentations: subtree sizes, parent pointers and cached heights

Augmentations are additional fields stored per node, each of which introduces an invariant maintenance cost:

- `parent` pointers simplify upward traversals and certain deletions but increase update surface.
- `subtree_size` enables order statistics such as rank and selection.
- cached `height` enables AVL balance checks.

In all cases, the engineering question is whether the asymptotic improvement justifies the additional invariant burden and the larger constant factors.

## 13. Implementation audit checklist

Use the checklist below as a disciplined method for reviewing tree code. It is designed to catch errors that often survive superficial testing.

1. **Ownership**: every `malloc` has exactly one reachable `free` path.
2. **No subtree loss**: in insertion and deletion, child pointers are always assigned from recursive returns.
3. **Deletion correctness**: case analysis covers leaf, one-child and two-child nodes without fall-through ambiguity.
4. **Duplicate policy**: equality is handled explicitly and consistently.
5. **Validation**: `bst_is_valid` uses strict inequalities and correctly narrows ranges.
6. **Extremal functions**: `find_min` and `find_max` are one-sided and handle empty trees.
7. **Traversal formatting**: unit tests in this repository require trailing spaces in traversal output.
8. **Recursion depth**: for large degenerate trees recursion can overflow the call stack; iterative variants should be considered when scalability matters.
9. **Determinism**: output should be stable and free of debugging banners when run under automated tests.

## 14. References

The following references provide primary or canonical treatments of searching trees, balanced variants and average case analysis.

| Reference (APA 7th ed) | DOI |
|---|---|
| Bayer, R., & McCreight, E. M. (1972). Organization and maintenance of large ordered indexes. *Acta Informatica, 1*, 173–189. | https://doi.org/10.1007/BF00288683 |
| Hoare, C. A. R. (1962). Quicksort. *The Computer Journal, 5*(1), 10–16. | https://doi.org/10.1093/comjnl/5.1.10 |
| Sleator, D. D., & Tarjan, R. E. (1985). Amortized efficiency of list update and paging rules. *Communications of the ACM, 28*(2), 202–208. | https://doi.org/10.1145/2786.2793 |
| Sethi, R., & Ullman, J. D. (1970). The generation of optimal code for arithmetic expressions. *Journal of the ACM, 17*(4), 715–728. | https://doi.org/10.1145/321607.321620 |
| Michael, M. M., & Scott, M. L. (1996). Simple, fast and practical non-blocking and blocking concurrent queue algorithms. In *Proceedings of the Fifteenth Annual ACM Symposium on Principles of Distributed Computing* (pp. 267–275). | https://doi.org/10.1145/248052.248106 |

The selection includes both direct BST related material and supporting foundations. In particular, Hibbard style deletion is a BST topic but it is often presented in textbooks rather than as a single canonical DOI indexed paper, hence the emphasis on broader primary sources.

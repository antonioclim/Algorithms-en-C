# Extended challenges for Week 08

## 1. Purpose and grading semantics

The challenges below are optional extensions designed to deepen your understanding of BST engineering beyond the core homework tasks. Each completed challenge can earn up to **10 bonus points**, with a maximum bonus of **50 points** across the set.

You should treat each challenge as a mini-research exercise: state your invariants, document algorithmic design decisions, justify complexity and ensure that memory ownership is unambiguous.

Unless a challenge explicitly requests interactive behaviour, design the programme so it can be tested non-interactively.

---

## 2. Challenge 1: Balance analysis and degeneracy detection

### 2.1 Problem statement

Implement functions that measure structural imbalance and detect when a BST has become effectively linear.

### 2.2 Required API

```c
int bst_balance_factor(BSTNode *node);
bool bst_is_balanced(BSTNode *root);
void bst_report_imbalances(BSTNode *root);
bool bst_is_degenerate(BSTNode *root);
```

### 2.3 Definitions

- **Height** is defined as in the laboratory: `height(NULL) = -1` and `height(leaf) = 0`.
- The **balance factor** of a node is `height(left) - height(right)`.
- A tree is **AVL-balanced** if `|balance_factor(node)| ≤ 1` for every node.
- A tree is **degenerate** if each node has at most one non-null child.

### 2.4 Algorithmic guidance

A naïve implementation recomputes height at every node leading to `O(n^2)` in the worst case. A better design computes both height and balance validity in a single post-order pass.

#### Post-order aggregation pseudocode

```
CHECK(node):
  if node == NULL: return (height=-1, ok=true)
  (hl, okL) = CHECK(node.left)
  (hr, okR) = CHECK(node.right)
  h = 1 + max(hl, hr)
  ok = okL and okR and abs(hl - hr) <= 1
  return (h, ok)
```

The reporting function can reuse the same traversal while emitting nodes whose balance factor exceeds 1 in magnitude.

---

## 3. Challenge 2: In-order iterator without recursion

### 3.1 Problem statement

Implement an iterator that returns one key at a time in ascending order without recursion. This can be done using an explicit stack that simulates the call stack of in-order traversal.

### 3.2 Required API

```c
typedef struct BSTIterator BSTIterator;

BSTIterator *bst_iterator_create(BSTNode *root);
bool bst_iterator_has_next(BSTIterator *it);
int bst_iterator_next(BSTIterator *it);
void bst_iterator_free(BSTIterator *it);
```

### 3.3 Correctness invariant

At all times, the stack stores a path of nodes whose left subtrees have been fully processed, while their own keys have not yet been emitted.

### 3.4 Complexity target

- `has_next`: `O(1)`
- `next`: `O(1)` amortised
- memory: `O(h)` where `h` is height

#### Iterator initialisation pseudocode

```
INIT(root):
  while root != NULL:
    push(root)
    root = root.left
```

#### Next operation pseudocode

```
NEXT():
  node = pop()
  key = node.key
  x = node.right
  while x != NULL:
    push(x)
    x = x.left
  return key
```

---

## 4. Challenge 3: Build a height-balanced BST from a sorted array

### 4.1 Problem statement

Given a sorted array of distinct integers, construct a BST whose height is asymptotically optimal. Use the median element as the root and recurse on halves.

### 4.2 Required API

```c
BSTNode *bst_from_sorted_array(const int *a, int n);
```

### 4.3 Pseudocode

```
BUILD(a, lo, hi):
  if lo > hi: return NULL
  mid = (lo + hi) // 2
  node = new(a[mid])
  node.left  = BUILD(a, lo, mid-1)
  node.right = BUILD(a, mid+1, hi)
  return node
```

### 4.4 Verification expectations

- `bst_is_valid(root)` returns true
- `bst_height(root)` is near `log2(n)`

---

## 5. Challenge 4: Threaded binary search tree

### 5.1 Problem statement

Implement a threaded BST that replaces null pointers with predecessor or successor threads, enabling in-order traversal without recursion and without an auxiliary stack.

### 5.2 Node model

```c
typedef struct ThreadedNode {
  int key;
  struct ThreadedNode *left;
  struct ThreadedNode *right;
  bool left_is_thread;
  bool right_is_thread;
} ThreadedNode;
```

### 5.3 Traversal idea

If `right_is_thread` is true, the `right` pointer is the in-order successor and can be followed in `O(1)`. If it is false, the successor is the leftmost node in the right subtree.

Threaded trees reduce auxiliary space but increase insertion complexity because predecessor and successor links must be updated carefully.

---

## 6. Challenge 5: Order statistics via subtree sizes

### 6.1 Problem statement

Augment each node with the size of its subtree so that rank and selection operations run in `O(h)` time.

### 6.2 API suggestions

```c
typedef struct OSTNode {
  int key;
  int size;              /* size of subtree rooted here */
  struct OSTNode *left;
  struct OSTNode *right;
} OSTNode;

int ost_rank(OSTNode *root, int key);
int ost_select(OSTNode *root, int k);
```

### 6.3 Invariant

For every node `x`, `x.size = 1 + size(x.left) + size(x.right)`.

### 6.4 Consequence

If `left_size = size(x.left)`, then:
- `rank(x.key) = left_size` within the subtree
- `select(k)` compares `k` with `left_size + 1` as shown in the repository README

---

## 7. Submission expectations for extended work

If you submit any challenge, include:
- a short technical report as comments at the top of the `.c` file
- invariants for each data structure
- a note on complexity
- a brief description of how you tested the code

Extended solutions should compile without warnings and should free all allocated memory.


---

## 8. Engineering notes and common edge cases

This section is not part of the marking rubric but it is included to make the challenges technically meaningful rather than merely syntactic.

### 8.1 Height conventions and their consequences

The laboratory uses `height(NULL) = -1` and `height(leaf) = 0`. This convention ensures that

- the recurrence `height(node) = 1 + max(height(left), height(right))` works without special cases
- the height of a singleton tree is 0 which aligns with the graph theoretic definition of path length in edges

When you combine height with balance factors, be explicit about whether you are measuring height in nodes or height in edges. Mixing conventions is one of the most common sources of off-by-one errors in balance diagnostics.

### 8.2 Degeneracy detection

A tree is degenerate if every node has at most one child. A direct recursive characterisation is:

```
DEGENERATE(node):
  if node == NULL: return true
  if node.left != NULL and node.right != NULL: return false
  return DEGENERATE(node.left) and DEGENERATE(node.right)
```

This property is orthogonal to balance: a perfectly degenerate tree is maximally unbalanced but a non-degenerate tree can still have catastrophic height.

### 8.3 Iterator robustness and amortised analysis

The in-order iterator is a standard example in which worst-case and amortised costs differ. A single `next` call can traverse a path of length `h` when it descends the left spine of a right subtree. Over a full traversal, each node is pushed at most once and popped at most once, hence the total stack operations are `O(n)` and the amortised cost per `next` is `O(1)`.

A useful invariant to assert during debugging is that the stack is always strictly decreasing in key order from bottom to top when the iterator is used on a valid BST.

### 8.4 Threaded BST insertion outline

Threaded insertion is error-prone because it must preserve both the BST ordering predicate and the predecessor/successor threading predicate. One systematic approach is to treat the thread flags as part of the invariant and to write insertion as a careful case analysis.

A high-level outline for inserting `key` into a right-threaded structure is:

```
INSERT(root, key):
  if root == NULL: return new_threaded_node(key)

  cur = root
  while true:
    if key < cur.key:
      if cur.left_is_thread:
        /* Insert as cur.left. cur.left currently points to predecessor. */
        n = new_node(key)
        n.left = cur.left
        n.left_is_thread = true
        n.right = cur
        n.right_is_thread = true
        cur.left = n
        cur.left_is_thread = false
        break
      else:
        cur = cur.left

    else if key > cur.key:
      if cur.right_is_thread:
        /* Insert as cur.right. cur.right currently points to successor. */
        n = new_node(key)
        n.right = cur.right
        n.right_is_thread = true
        n.left = cur
        n.left_is_thread = true
        cur.right = n
        cur.right_is_thread = false
        break
      else:
        cur = cur.right

    else:
      /* Duplicate policy: either ignore or update payload. */
      break

  return root
```

The correctness obligation is to show that the newly inserted node is threaded to its predecessor and successor and that all previously valid threads remain valid.

### 8.5 Order statistics maintenance during updates

If you augment nodes with `size`, you must update sizes on the recursion unwinding path during insertion and deletion. A typical insertion skeleton is:

```c
root = insert(root, key);
root->size = 1 + size(root->left) + size(root->right);
```

Deletion is more subtle because structural cases can return a child pointer directly. You should update size after the structural case has been resolved, not before.

---

## 9. Testing strategies for extended work

For extended tasks, simple example-based testing is rarely sufficient. Combine at least two of the following approaches.

1. **Property checks**: after each update, assert `bst_is_valid(root)` for non-threaded BSTs.
2. **Metamorphic testing**: insert a set of keys, record in-order output then delete the same keys in a random order and assert that the final tree is empty.
3. **Cross-validation**: compare results against a reference container in another language (for example Python `bisect` on a sorted list) for randomly generated sequences.
4. **Memory checks**: use a leak detector (Valgrind or sanitizers) and also ensure you free partial structures on error paths.

A minimal random test harness in C can be built around `rand()` but if you do so, fix the seed with `srand(0)` to keep your tests reproducible.

# Extended Challenges: AVL Trees

## Orientation

The following challenges are optional and are intended for students who wish to deepen both their implementational competence and their analytical fluency with balanced search trees.

Unless your course handbook states otherwise, each challenge is assessed independently and yields up to **+10 bonus points**. Partial credit is possible when you can demonstrate a correct invariant set, clean memory management and a coherent test methodology.

### General standards (apply to every challenge)

Your submission must:

- compile under ISO C11 with `-Wall -Wextra` and without warnings
- be memory safe (no leaks and no invalid accesses)
- expose deterministic output suitable for transcript-based testing
- include a brief write-up of invariants, algorithms and complexity

Where you modify the base AVL implementation, state explicitly which height convention you adopt and how you validate that stored heights remain correct.

---

## Challenge 1: Parent pointers and inorder iterators

### Motivation

A conventional binary search tree node stores child pointers only. Successor and predecessor queries are then typically implemented by descending into a subtree and possibly climbing back up via recursion. Adding an explicit `parent` pointer makes upward traversal explicit. This permits:

- iterative inorder traversal without an auxiliary stack
- iterator-like `next` and `prev` operations that run in `O(h)` time where `h` is the tree height

### Data model

```c
typedef struct AVLNodeP {
    int key;
    int height;
    struct AVLNodeP *left;
    struct AVLNodeP *right;
    struct AVLNodeP *parent;
} AVLNodeP;
```

### Invariants

In addition to the standard AVL invariants, you must maintain:

- for any non-null node `v`, if `v->left` is non-null then `v->left->parent == v`
- for any non-null node `v`, if `v->right` is non-null then `v->right->parent == v`
- the root satisfies `root->parent == NULL`

Rotations are the critical stress point: a single missed parent update introduces aliasing bugs that can remain latent until a long traversal triggers a cycle.

### Algorithms

#### Successor via parent pointers

If `x` has a right subtree, the successor is the minimum node in that right subtree. Otherwise, climb to the first ancestor for which `x` lies in its left subtree.

Pseudocode:

```
SUCCESSOR(x):
  if x.right != NIL:
    return MINIMUM(x.right)
  y := x.parent
  while y != NIL and x == y.right:
    x := y
    y := y.parent
  return y
```

The predecessor is symmetric.

### Testing expectations

A minimal iterator test is:

1. insert a known set of keys
2. compute `min = MINIMUM(root)`
3. repeatedly apply `NEXT(min)` until `NULL`
4. verify the printed sequence is strictly increasing and has the correct cardinality

---

## Challenge 2: Serialisation and deserialisation

### Motivation

Serialisation forces you to distinguish *tree content* (the key set) from *tree shape* (the structure induced by balancing and insertion order). A content-only dump is trivial but does not preserve shape. This challenge requires shape preservation.

### Required behaviour

Implement:

- `avl_serialize(root, FILE *out)`
- `avl_deserialize(FILE *in)`

such that deserialising immediately after serialising yields a tree that is isomorphic to the original one, including identical keys and identical structure.

### Proposed binary format

One workable representation is a preorder stream augmented with child-existence flags:

```
uint32_t node_count
repeat node_count times:
  int32_t key
  uint8_t flags   // bit0: has_left, bit1: has_right
```

The preorder order ensures that, when you read a node, you can recursively decide whether to read a left child and whether to read a right child.

### Pseudocode

```
SERIALISE(v):
  if v == NIL: return
  write(v.key)
  flags := (v.left != NIL) + 2*(v.right != NIL)
  write(flags)
  SERIALISE(v.left)
  SERIALISE(v.right)

DESERIALISE():
  if stream exhausted: error
  key := read_int
  flags := read_byte
  v := new_node(key)
  if flags bit0 set: v.left  := DESERIALISE(); v.left.parent := v
  if flags bit1 set: v.right := DESERIALISE(); v.right.parent := v
  update_height(v)
  return v
```

### Complexity

Both procedures are linear in the number of nodes and in the output size. The principal engineering challenge is robust error handling: unexpected EOF, corrupt flags and memory allocation failure.

---

### Portability and reproducibility notes

When you serialise to a binary stream, you implicitly choose a representation for integers. To prevent accidental non-portability, you should:

- use fixed-width types (`uint32_t`, `int32_t`) and document the intended byte order
- define a small header, for example a 4-byte magic string and a 16-bit format version, so that future changes are detectable
- validate `node_count` against reasonable limits before allocating memory or recursing

A disciplined strategy is to treat the binary format as an API: state preconditions, postconditions and failure modes.

### Minimal validation suite

A robust serialisation implementation is easy to test mechanically:

1. build a tree from a known key set
2. serialise to a file and then deserialise from that file
3. validate AVL invariants and then serialise again
4. compare the two byte streams for equality

If you wish to test structural isomorphism directly, print a preorder traversal that includes explicit `NIL` markers and compare the resulting strings.

---

## Challenge 3: Order statistics

### Motivation

Order statistics extend a balanced BST with two operations:

- `SELECT(i)`: return the i-th smallest key (1-indexed)
- `RANK(x)`: return the number of keys strictly smaller than `x`

### Augmentation

Add a field `size` storing the cardinality of the subtree rooted at the node:

```c
typedef struct AVLNodeOS {
    int key;
    int height;
    int size;
    struct AVLNodeOS *left;
    struct AVLNodeOS *right;
} AVLNodeOS;
```

Maintain `size` under insertion, deletion and rotations:

```
size(v) = 1 + size(v.left) + size(v.right)
```

### Algorithms

#### Select

```
SELECT(v, i):
  if v == NIL: return NIL
  L := size(v.left)
  if i == L + 1: return v
  if i <= L:     return SELECT(v.left, i)
  else:          return SELECT(v.right, i - L - 1)
```

#### Rank

```
RANK(v, x):
  if v == NIL: return 0
  if x <= v.key: return RANK(v.left, x)
  else: return size(v.left) + 1 + RANK(v.right, x)
```

Both run in `O(h)` time and therefore in `O(log n)` worst case for AVL trees.

### Validation

Extend your validation routine to check that every stored `size` equals the computed subtree size. This catches the common rotation bug where heights are updated but sizes are not.

---

## Challenge 4: Join and split

### Motivation

Set operations over ordered dictionaries are more efficient when you can combine and separate balanced trees without inserting elements one by one.

Define:

- `JOIN(T1, k, T2)` where all keys in `T1` are `< k` and all keys in `T2` are `> k`
- `SPLIT(T, k)` returning two trees `(L, R)` where all keys in `L` are `< k` and all keys in `R` are `> k` (you may also return whether `k` existed)

### High-level approach

A standard join strategy is:

1. if `height(T1) > height(T2) + 1`, recurse into `T1.right` and join there
2. if `height(T2) > height(T1) + 1`, recurse into `T2.left` and join there
3. otherwise create a new root with key `k`, attach `T1` and `T2` as children and rebalance

This yields `O(|height(T1) − height(T2)|)` rebalancing cost.

---

## Challenge 5: Empirical study of update costs

### Aim

Design a small experimental study that measures:

- height evolution under different insertion sequences
- number of rotations per update
- amortised behaviour under mixed insertions and deletions

You may compare your AVL implementation against:

- an unbalanced BST
- a red-black tree implementation (if you have one)

The emphasis is methodological: describe the workload distributions, justify metrics and report results with clear plots or tables.

---

## References

| Reference (APA 7th) | DOI |
|---|---|
| Amani, M., Lai, K. A., & Tarjan, R. E. (2016). *Amortized rotation cost in AVL trees*. *Information Processing Letters, 116*(5), 327–330. | https://doi.org/10.1016/j.ipl.2015.12.009 |
| Driscoll, J. R., Sarnak, N., Sleator, D. D., & Tarjan, R. E. (1989). Making data structures persistent. *Journal of Computer and System Sciences, 38*(1), 86–124. | https://doi.org/10.1016/0022-0000(89)90034-2 |
| Guibas, L. J., & Sedgewick, R. (1978). *A dichromatic framework for balanced trees* (FOCS 1978, pp. 8–21). IEEE. | https://doi.org/10.1109/SFCS.1978.3 |
| Tarjan, R. E. (1983). Updating a balanced search tree in O(1) rotations. *Information Processing Letters, 16*(5), 253–257. | https://doi.org/10.1016/0020-0190(83)90099-6 |

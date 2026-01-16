# Week 09: AVL Trees

## Synopsis

This teaching unit develops an implementational and analytical understanding of AVL trees as height-balanced binary search trees. The project provides three student-facing C programmes and four reference solutions:

- `src/example1.c` is a didactic demonstration of insertion with rebalancing and a verbose depiction of rotations.
- `src/exercise1.c` is a foundation implementation for insertion and validation that is assessed through transcript testing.
- `src/exercise2.c` extends the foundation to deletion and order-aware queries and is likewise assessed through transcript testing.

The `solution/` directory contains fully worked implementations for the exercises and two larger homeworks.

The intent is twofold: to consolidate the theory of height balancing and to make explicit the algorithmic contracts that connect mathematical invariants to concrete pointer manipulations.

## Repository topology

```
09-AVL Trees/
├── Makefile
├── README.md
├── data/
│   ├── random_keys.txt
│   └── sorted_keys.txt
├── slides/
│   ├── presentation-comparativ.html
│   └── presentation-week09.html
├── src/
│   ├── example1.c
│   ├── exercise1.c
│   └── exercise2.c
├── solution/
│   ├── exercise1_sol.c
│   ├── exercise2_sol.c
│   ├── homework1_sol.c
│   └── homework2_sol.c
├── teme/
│   ├── homework-extended.md
│   └── homework-requirements.md
└── tests/
    ├── test1_input.txt
    ├── test1_expected.txt
    ├── test2_input.txt
    ├── test2_expected.txt
```

## Build and execution protocol

### Compilation

The Makefile assumes GCC and the C11 dialect.

```bash
make            # builds student programmes: example1, exercise1, exercise2
make solutions  # builds reference solutions
```

### Running

```bash
make run        # runs the verbose demonstration (example1)
make run-ex1    # runs exercise1 (expects scripted input on stdin)
make run-ex2    # runs exercise2 (expects scripted input on stdin)
```

### Deterministic transcript tests

Two transcript tests are provided. Each test feeds a script on standard input and compares programme output against a golden file.

```bash
make test

# Optional: run solution self-tests (they print their own internal transcripts)
make test-solutions
```

The student programmes are deliberately strict about their printed contract. This models a real-world situation in which downstream tooling consumes the output and a single spurious diagnostic line constitutes a functional regression.

## Mathematical model

### Binary search tree property

An AVL tree is a binary search tree (BST) with a balance invariant. The BST property is an order constraint:

For every node `v` with key `k(v)`:

- for every key `x` in the left subtree of `v`, `x < k(v)`
- for every key `y` in the right subtree of `v`, `k(v) < y`

In the project code we adopt a strict ordering and ignore duplicate insertions. This policy keeps the invariant crisp and avoids the need for multiset semantics.

### Height and balance factor

Let `height(v)` denote the height of the subtree rooted at node `v`. This repository uses the conventional empty-tree height of `-1`:

- `height(NULL) = -1`
- `height(leaf) = 0`

The balance factor at `v` is:

```
bf(v) = height(left(v)) − height(right(v))
```

The AVL constraint requires `bf(v) ∈ {−1, 0, +1}` for every node.

This constraint is local yet its consequences are global: it implies a logarithmic height bound and hence logarithmic search time.

### Height bound and worst-case shape

Define `N(h)` as the minimum number of nodes in any AVL tree of height `h` under the above height convention. The AVL constraint implies the recurrence:

```
N(−1) = 0
N(0)  = 1
N(h)  = 1 + N(h−1) + N(h−2)   for h ≥ 1
```

The intuition is that the worst case, in terms of minimal nodes for a given height, occurs when one child has height `h−1` and the other has height `h−2`. The recurrence is essentially Fibonacci and yields:

```
N(h) ≥ F(h+2) − 1
```

where `F` is the Fibonacci sequence with `F(0)=0` and `F(1)=1`. Consequently:

```
h ≤ log_φ(n + 2) − 2
```

where `φ = (1 + √5)/2` is the golden ratio and `n` is the number of nodes. Converting to base 2 yields the well-known constant:

```
h < 1.44042 · log₂(n + 2)  (up to additive constants)
```

The significant point for implementation is that all navigational operations are `O(h)` and hence `O(log n)` in the worst case.

## Algorithmic core

This section describes the algorithms as implemented in `exercise1.c` and `exercise2.c` and as illustrated verbosely in `example1.c`.

### Rotation primitives

Rotations are local transformations that preserve in-order key sequence and therefore preserve the BST property. Their purpose is to restore balance factors while changing as little structure as possible.

#### Right rotation

A right rotation is applied when a node `y` is left-heavy and its left child `x` is not right-heavy beyond the threshold.

Pseudocode:

```
RIGHT-ROTATE(y):
    x  ← y.left
    T2 ← x.right

    x.right ← y
    y.left  ← T2

    UPDATE-HEIGHT(y)
    UPDATE-HEIGHT(x)

    return x
```

Conceptually, `x` becomes the new root of the rotated subtree and `y` becomes its right child.

#### Left rotation

Symmetrically:

```
LEFT-ROTATE(x):
    y  ← x.right
    T2 ← y.left

    y.left  ← x
    x.right ← T2

    UPDATE-HEIGHT(x)
    UPDATE-HEIGHT(y)

    return y
```

### Rebalancing logic

Rebalancing is performed bottom-up after a structural modification. In an insertion this is on the search path from the inserted leaf back to the root. In a deletion it is on the search path from the deleted node back to the root.

Pseudocode (single-node rebalance):

```
REBALANCE(v):
    UPDATE-HEIGHT(v)

    bf ← BALANCE-FACTOR(v)

    if bf > 1 then
        if BALANCE-FACTOR(v.left) < 0 then
            v.left ← LEFT-ROTATE(v.left)      # LR pre-rotation
        end if
        return RIGHT-ROTATE(v)                 # LL or LR final rotation

    else if bf < -1 then
        if BALANCE-FACTOR(v.right) > 0 then
            v.right ← RIGHT-ROTATE(v.right)    # RL pre-rotation
        end if
        return LEFT-ROTATE(v)                  # RR or RL final rotation

    else
        return v                               # already balanced
    end if
```

The decision tree is a precise classification of the four canonical imbalance configurations:

- LL: left subtree of left child is heavy
- RR: right subtree of right child is heavy
- LR: right subtree of left child is heavy
- RL: left subtree of right child is heavy

The two double-rotation cases (LR and RL) are not distinct primitives. They are compositions of the single rotations.

### Insertion

Insertion preserves the BST property by following the search path and inserting at a null pointer position. Balance is restored during unwinding.

Pseudocode:

```
AVL-INSERT(v, key):
    if v = NULL then
        return NEW-NODE(key)

    if key < v.key then
        v.left ← AVL-INSERT(v.left, key)

    else if key > v.key then
        v.right ← AVL-INSERT(v.right, key)

    else
        return v           # duplicates ignored

    return REBALANCE(v)
```

`exercise1.c` implements exactly this logic and validates the result against three coupled invariants:

- global BST ordering
- local AVL balance
- stored heights match recomputed heights

### Deletion

Deletion is structurally more complex because removing an internal node requires preserving in-order sequence. The canonical approach replaces the key with the in-order successor (minimum in the right subtree) and then deletes that successor node.

Pseudocode:

```
AVL-DELETE(v, key):
    if v = NULL then
        return NULL

    if key < v.key then
        v.left ← AVL-DELETE(v.left, key)

    else if key > v.key then
        v.right ← AVL-DELETE(v.right, key)

    else
        # v is the node to be deleted
        if v.left = NULL and v.right = NULL then
            FREE(v)
            return NULL

        else if v.left = NULL then
            tmp ← v.right
            FREE(v)
            return tmp

        else if v.right = NULL then
            tmp ← v.left
            FREE(v)
            return tmp

        else
            s ← MIN(v.right)         # in-order successor
            v.key ← s.key
            v.right ← AVL-DELETE(v.right, s.key)
        end if
    end if

    return REBALANCE(v)
```

The subtlety is that deletion can cause a cascade of rebalancing events on the path to the root. Unlike insertion, which requires at most a constant number of rotations, deletion may require `Θ(log n)` rotations in the worst case.

### Successor and predecessor

The successor of key `k` in a BST is the smallest key strictly greater than `k`. The predecessor is the largest key strictly smaller than `k`.

For successor:

- if the node `k` exists and has a right subtree then successor is `MIN(right-subtree)`
- otherwise successor is the lowest ancestor for which the path to `k` goes through its left child

Pseudocode (successor):

```
SUCCESSOR(root, k):
    succ ← NULL
    cur  ← root

    while cur ≠ NULL do
        if k < cur.key then
            succ ← cur
            cur  ← cur.left
        else if k > cur.key then
            cur  ← cur.right
        else
            if cur.right ≠ NULL then
                return MIN(cur.right)
            end if
            break
        end if
    end while

    return succ
```

Predecessor is symmetric.

### Range query

A range query enumerates all keys in `[low, high]` in sorted order. The BST property permits pruning: an entire subtree can be skipped if its keys are provably out of range.

Pseudocode:

```
RANGE-QUERY(v, low, high):
    if v = NULL then return

    if v.key > low then
        RANGE-QUERY(v.left, low, high)

    if low ≤ v.key ≤ high then
        OUTPUT(v.key)

    if v.key < high then
        RANGE-QUERY(v.right, low, high)
```

In the worst case the complexity is `O(h + m)` where `m` is the number of reported keys. This bound is optimal in the comparison model because producing `m` outputs already requires `Ω(m)` time.

### Validation as a first-class algorithm

Both student exercises provide a validator that checks:

- BST ordering using global min/max constraints rather than only parent-child comparisons
- AVL balance factors based on recomputed subtree heights
- stored heights are consistent with recomputed heights

A convenient formulation is a recursive function that returns the computed height and updates a boolean flag on violation:

```
VALIDATE(v, minKey, maxKey):
    if v = NULL then return -1

    if v.key ≤ minKey or v.key ≥ maxKey then
        ok ← false
        return -1

    hl ← VALIDATE(v.left, minKey, v.key)
    hr ← VALIDATE(v.right, v.key, maxKey)

    if |hl − hr| > 1 then
        ok ← false

    expected ← 1 + max(hl, hr)
    if v.height ≠ expected then
        ok ← false

    return expected
```

Treating validation as an explicit algorithm is pedagogically useful. It makes the hidden assumptions of the data structure observable and it provides a principled debugging oracle when pointer manipulations go wrong.

## Programme-by-programme functional analysis

### `src/example1.c`

**Role.** A verbose demonstration of AVL insertion and rebalancing.

**Core behaviours.**

- Defines an integer-key AVL node structure.
- Implements insertion with balancing and prints the tree structure after each insertion.
- Reports which imbalance case occurred and which rotations were executed.
- Demonstrates searching and validation utilities.

**Algorithmic notes.**

The programme is intentionally diagnostic. It prints balance factors and heights at each node which makes the relationship between the mathematical definitions and the concrete data fields explicit.

Because the output is exploratory, it is not compared against a golden file.

### `src/exercise1.c`

**Role.** A minimal insertion-only implementation that is assessed by deterministic transcript tests (`tests/test1_input.txt`).

**Core behaviours.**

- Reads a sequence of test cases from standard input.
- For each case builds an AVL tree by repeated insertion.
- Prints the in-order traversal, validation status and key statistics.

**Notable design choices.**

- Duplicate keys are ignored. This yields a strict BST invariant, simplifies validation and avoids ambiguous semantics.
- The validator checks ordering globally and checks height fields, not only balance factors.

**Testing contract.**

The output is designed to match `tests/test1_expected.txt` exactly. Any extra diagnostic printing should be considered a functional regression for the purposes of automated assessment.

### `src/exercise2.c`

**Role.** A command-driven AVL toolkit that includes deletion and order-aware operations.

**Core behaviours.**

- Parses an operation script from standard input.
- Supports: BUILD, SEARCH, MIN, MAX, SUCCESSOR, PREDECESSOR, RANGE and DELETE.
- After destructive operations prints the resulting in-order traversal and re-validates the tree.

**Algorithmic notes.**

Deletion re-uses successor replacement in the two-child case. This choice is standard because it preserves in-order ordering while limiting structural disturbance to a single root-to-leaf path.

**Testing contract.**

The transcript is matched against `tests/test2_expected.txt` exactly.

### `solution/exercise1_sol.c` and `solution/exercise2_sol.c`

These programmes include more verbose demonstrations and contain internal self-tests that print a success banner. Their pedagogical objective is to provide a high-visibility reference for rotation logic and for the runtime shape of AVL trees.

### `solution/homework1_sol.c`

This reference solution implements a dictionary (string keys, integer values) using an AVL tree. The key ordering is defined by `strcmp` and keys are dynamically allocated.

The significant algorithmic change relative to integer keys is that comparisons become `O(|key|)` in the worst case. As a result the total complexity of an operation is `O(|key| · log n)` in the adversarial case, although average behaviour is often better when keys are short.

### `solution/homework2_sol.c`

This reference solution implements a file-driven visualiser for AVL operations with ASCII rendering and statistics. It illustrates the engineering questions that surround a theoretically simple data structure:

- robust parsing
- careful memory ownership
- deterministic instrumentation
- meaningful diagnostic output that does not perturb algorithmic complexity

## Cross-language correspondence

This repository is implemented in C. The algorithms are language-agnostic but pointer manipulation is not. The following sketches show how the same invariants typically map to C++, Python and Java without changing the underlying logic.

### C++ (value semantics and RAII)

```cpp
struct Node {
    int key;
    int height;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};

// Rotations become ownership-preserving pointer rewrites.
// The rebalance logic is identical to the C version.
```

The conceptual shift is not the algorithm but the ownership model: `std::unique_ptr` makes deallocation implicit, which reduces the surface area for leaks but makes rotations slightly more verbose.

### Python (reference semantics)

```python
class Node:
    __slots__ = ("key", "height", "left", "right")
    def __init__(self, key):
        self.key = key
        self.height = 0
        self.left = None
        self.right = None

# Rotations are assignment of object references.
# Height maintenance and balance checks are as in C.
```

Python simplifies memory management but obscures cost models. It is still important to distinguish `O(log n)` pointer traversals from the constant factors introduced by dynamic dispatch and object allocation.

### Java (managed heap and generics)

```java
final class Node<K extends Comparable<K>, V> {
    K key;
    V value;
    int height;
    Node<K,V> left;
    Node<K,V> right;
}

// Rotations and rebalancing match the same decision tree.
```

In Java, `Comparable` drives ordering and memory is garbage-collected. Nevertheless, the programmer remains responsible for preserving invariants and for preventing accidental aliasing that breaks tree structure.

## Recommended investigative experiments

The `data/` directory contains key sequences intended to provoke contrasting behaviours:

- `sorted_keys.txt` approximates the adversarial case for an unbalanced BST and motivates balancing.
- `random_keys.txt` approximates average-case behaviour.

A sound experimental protocol is:

1. build a plain BST and record height after each insertion
2. build an AVL tree on the same sequence and record height
3. compare the trajectories and quantify the effect of balancing

A careful extension is to measure the number of rotations, comparisons and allocations under varying distributions.

## Rigour notes: invariants, proofs and failure modes

### Invariants stated as predicates

In an academic setting it is useful to make invariants explicit as predicates over the heap graph. Let `T` be the (finite) set of nodes reachable from `root` by following `left` and `right` pointers.

1. **Acyclicity and ownership**: the pointer graph induced by `left` and `right` is a rooted directed acyclic graph and each node has in-degree at most one. In C this is not enforced by the type system and is therefore a programme obligation.
2. **Strict ordering**: for every node `v` and every node `u` in the left subtree of `v`, `key(u) < key(v)` and for every node `w` in the right subtree of `v`, `key(w) > key(v)`. This global statement is stronger than local parent–child comparisons.
3. **Stored height correctness**: for every node `v`, `height(v) = 1 + max(height(left(v)), height(right(v)))` where the height of the empty tree is `-1`.
4. **AVL balance**: for every node `v`, `|height(left(v)) − height(right(v))| ≤ 1`.

The validation routine in `src/exercise1.c` and `src/exercise2.c` checks all four conditions simultaneously using a single post-order traversal that propagates allowable key intervals.

### Height bound as a recurrence

A classical worst-case argument for AVL height proceeds by considering the minimum number of nodes `N(h)` that an AVL tree of height `h` can contain. If a tree has height `h` then one child subtree can have height `h−1` and the other can have height `h−2` in the most node-sparse but still balanced configuration. This yields the recurrence:

```
N(-1) = 0
N(0)  = 1
N(h)  = 1 + N(h-1) + N(h-2)    for h ≥ 1
```

The recurrence is Fibonacci-like, hence `N(h) ≥ F(h+2) − 1` and therefore `h = O(log n)`. In base-2 logarithms one obtains the widely cited constant `1 / log2(φ) ≈ 1.44042`, hence `h ≤ 1.44042 · log2(n + 2) − 2` for all `n ≥ 1`.

### Typical implementation hazards in C

Even a conceptually correct rebalancing algorithm can be invalidated by low-level defects. The most common failure modes in student submissions are:

- **stale heights**: forgetting to update heights after a rotation, which invalidates subsequent balance computations
- **incorrect child relinking**: swapping the wrong pointers in a rotation, which can create cycles or detach subtrees
- **use-after-free in deletion**: freeing a node and then reading its fields to decide how to rebalance
- **inconsistent duplicate policy**: inserting duplicates without a clear policy can break strict ordering and make search ambiguous

The transcript tests catch several of these errors because the inorder output serves as an oracle for ordering and the validation output enforces balance and height correctness. For deeper assurance, you can add sanitiser builds (`-fsanitize=address,undefined`) and randomised differential tests against a trusted reference implementation.


### Determinism as an engineering constraint

For automated assessment, determinism is not a cosmetic preference but a contractual requirement. The programmes in this repository avoid time-dependent output, address-dependent output and locale-dependent formatting. In particular, traversal printers produce canonical spacing and terminate lines explicitly so that transcript diffs correspond to semantic changes rather than incidental whitespace.

## References

| Reference (APA 7th) | DOI |
|---|---|
| Amani, M., Lai, K. A., & Tarjan, R. E. (2016). *Amortized rotation cost in AVL trees*. *Information Processing Letters, 116*(5), 327–330. | https://doi.org/10.1016/j.ipl.2015.12.009 |
| Bayer, R. (1972). Symmetric binary B-Trees: Data structure and maintenance algorithms. *Acta Informatica, 1*, 290–306. | https://doi.org/10.1007/BF00289509 |
| Brown, M. R. (1979). A partial analysis of random height-balanced trees. *SIAM Journal on Computing, 8*(1), 33–41. | https://doi.org/10.1137/0208003 |
| Guibas, L. J., & Sedgewick, R. (1978). *A dichromatic framework for balanced trees* (FOCS 1978, pp. 8–21). IEEE. | https://doi.org/10.1109/SFCS.1978.3 |
| Karlton, P. L., Fuller, S. H., Scroggs, R. E., & Kaehler, E. B. (1976). Performance of height-balanced trees. *Communications of the ACM, 19*(1), 23–28. | https://doi.org/10.1145/359970.359989 |


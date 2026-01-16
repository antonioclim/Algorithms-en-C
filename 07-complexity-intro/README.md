# Week 07: Binary Trees and Expression Trees

## Abstract

This laboratory unit consolidates three tightly coupled strands of algorithmic literacy: (i) the **structural** view of computation in which recursive data define recursive algorithms, (ii) the **operational** view in which tree traversals are analysed as disciplined state transitions and (iii) the **complexity** view in which every design choice is justified in asymptotic and constant-factor terms. Concretely, the repository provides a complete worked example of binary-tree manipulation (`src/example1.c`) and two assessed exercises: a pointer-based binary tree with classical traversals and statistics (`src/exercise1.c`) and an expression tree constructed from postfix notation using a stack (`src/exercise2.c`).

The material is written for C11 and assumes a disciplined memory model: **every dynamic allocation has a single, explicit owner and every owned allocation is eventually released**. Where recursion is used, the call stack is treated as an algorithmic resource and not as an unbounded convenience.

## Learning outcomes

On completion, a student should be able to:

1. Define a binary tree formally and implement it in C using a self-referential structure.
2. Derive preorder, inorder and postorder traversals from the recursive definition of a tree.
3. Implement level-order traversal as a breadth-first search using a FIFO queue.
4. Compute canonical tree statistics (height, size, leaf count and sum) and justify their asymptotic bounds.
5. Construct an expression tree from postfix (Reverse Polish) notation using a stack-based algorithm and prove that the resulting tree is unique for well-formed binary operators.
6. Evaluate an expression tree and perform notation conversions (prefix, infix with parentheses and postfix) as direct instances of traversal schemes.

## Repository layout

```
07-complexity-intro/
├── README.md
├── Makefile
├── src/
│   ├── example1.c
│   ├── exercise1.c
│   └── exercise2.c
├── solution/
│   ├── exercise1_sol.c
│   ├── exercise2_sol.c
│   ├── homework1_sol.c
│   └── homework2_sol.c
├── data/
│   ├── tree_data.txt
│   └── expressions.txt
├── tests/
│   ├── test1_input.txt
│   ├── test1_expected.txt
│   ├── test2_input.txt
│   └── test2_expected.txt
├── teme/
│   ├── homework-requirements.md
│   └── homework-extended.md
└── slides/
    ├── presentation-week07.html
    └── presentation-comparativ.html
```

The `solution/` directory contains instructor reference implementations that mirror the student-facing specifications. The `tests/` directory implements **golden-output regression tests** for the student binaries.

## Build and test workflow

### Compilation

```bash
make
```

The default target builds:

- `example1` – comprehensive demonstration of binary trees
- `exercise1` – tree traversals and statistics
- `exercise2` – expression tree construction and evaluation

### Automated tests

```bash
make test
```

The tests run the student binaries and compare their complete standard output against reference files in `tests/` using `diff`. Output determinism is therefore part of the correctness contract.

If you change any formatting or add diagnostic prints, you must also decide whether the change is pedagogically justified and whether the test oracle should be regenerated. The intended discipline is:

- **Do not** modify `tests/*_expected.txt` in order to hide a bug.
- **Do** regenerate `tests/*_expected.txt` when a previously incorrect oracle has been corrected, after validating the new behaviour against the formal specification.

### Static compilation checks

```bash
make check
```

This target invokes the compiler in syntax-only mode and is a lightweight guard against accidental non-C11 features and uninitialised identifiers.

## Conceptual foundations

### 1. Formal definition of a binary tree

A **binary tree** is a finite set of nodes defined inductively.

- The empty tree is a binary tree.
- If `L` and `R` are binary trees and `x` is a value then the triple `(x, L, R)` is a binary tree.

This definition is not decorative. It is the template from which correct algorithms follow. In C, the definition is represented as a tagged pointer structure where the empty tree is `NULL`.

```c
typedef struct TreeNode {
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;
```

The key invariant is structural: every `TreeNode` either has a `NULL` child pointer (representing an empty subtree) or a pointer to a valid `TreeNode` that is itself the root of a subtree.

### 2. Traversals as recursion schemes

A traversal is a total function from nodes to an output sequence such that each node is visited exactly once. The difference between preorder, inorder and postorder is the location of the visit action relative to the recursive descent.

Let `visit(n)` denote the action performed at node `n`.

- **Preorder**: `visit` then traverse left then traverse right.
- **Inorder**: traverse left then `visit` then traverse right.
- **Postorder**: traverse left then traverse right then `visit`.

#### Traversal pseudocode

```
TRAVERSE(node, order):
    if node == NULL:
        return

    if order == PRE:
        visit(node)

    TRAVERSE(node.left, order)

    if order == IN:
        visit(node)

    TRAVERSE(node.right, order)

    if order == POST:
        visit(node)
```

This generic scheme becomes a concrete C function once `visit` is instantiated, typically as a `printf` or an accumulator update.

#### Complexity of traversal

For any traversal:

- Time: **O(n)**, because each node is processed once.
- Auxiliary space: **O(h)** for recursion depth, where `h` is tree height.

The distinction between `n` and `h` is essential. A balanced tree has `h = Θ(log n)` while a degenerate tree has `h = Θ(n)`.

### 3. Height, size and leaf count

The statistics functions in `exercise1.c` are direct corollaries of the recursive definition.

#### Height

Adopting the convention that the empty tree has height `-1` makes the recurrence algebraically clean:

- `height(NULL) = -1`
- `height(node) = 1 + max(height(node.left), height(node.right))`

Pseudocode:

```
HEIGHT(node):
    if node == NULL:
        return -1
    return 1 + max(HEIGHT(node.left), HEIGHT(node.right))
```

#### Size (node count)

- `size(NULL) = 0`
- `size(node) = 1 + size(left) + size(right)`

#### Leaf count

A leaf is characterised by the predicate `node.left == NULL and node.right == NULL`.

- `leaves(NULL) = 0`
- `leaves(leaf) = 1`
- `leaves(internal) = leaves(left) + leaves(right)`

### 4. Level-order traversal as BFS

Level-order traversal is not naturally expressed as a simple recursion scheme because it is fundamentally **queue-driven**.

Pseudocode:

```
LEVEL_ORDER(root):
    if root == NULL:
        return

    Q = empty queue
    enqueue(Q, root)

    while Q not empty:
        u = dequeue(Q)
        visit(u)
        if u.left  != NULL: enqueue(Q, u.left)
        if u.right != NULL: enqueue(Q, u.right)
```

Time is **O(n)** and the additional space is **O(w)** where `w` is the maximum width of the tree, bounded by `O(n)` and equal to `Θ(n)` in the worst case.

## Expression trees

### 1. Postfix notation and uniqueness

For binary operators, a well-formed postfix expression defines a unique binary tree because token order determines a unique pairing of operator nodes with their left and right operand subtrees. The construction algorithm is stack-based and deterministic.

### 2. Construction algorithm from postfix

Given a space-separated postfix string where tokens are either integers or one-character operators `+ - * /`:

- If the token is an operand, create a leaf and push it.
- If the token is an operator, pop the right subtree then pop the left subtree, create a new operator node and push it.
- After consuming all tokens, the stack contains exactly one item, the root.

Pseudocode:

```
BUILD_FROM_POSTFIX(tokens):
    S = empty stack

    for t in tokens:
        if t is integer:
            push(S, new_operand(t))
        else if t is operator:
            R = pop(S)
            L = pop(S)
            push(S, new_operator(t, L, R))
        else:
            error

    return pop(S)
```

#### Correctness sketch

Maintain the invariant that after processing the first `k` tokens, the stack contains roots of exactly those partial expression trees whose postfix serialisations are a partition of the first `k` tokens. An operand adds a new singleton tree. An operator merges the two most recent trees, matching the semantics of postfix evaluation.

#### Complexity

Let `m` be the number of tokens.

- Time: **O(m)**
- Space: **O(m)** in the worst case for the stack and the tree nodes

### 3. Evaluation and conversions

Evaluation is a postorder computation.

- Leaf: return its numeric value.
- Internal operator: evaluate left and right and apply the operator.

Prefix and postfix prints are preorder and postorder traversals respectively. Infix print is inorder with explicit parentheses to disambiguate operator precedence.

## File-by-file implementation notes

### `Makefile`

The build system defines a compact correctness pipeline: compile with warnings enabled, run golden tests with `diff` and provide a static syntax check. The `test` target asserts textual determinism which is desirable in an instructional setting because it makes regressions observable.

### `src/example1.c`

A fully worked demonstration that includes:

- recursive traversals
- queue-based level-order traversal
- structural statistics (height, size, leaves, internal nodes, sum, min, max)
- mirroring (tree inversion)
- deep copy and structural equality
- rotated and connector-based visualisations
- disciplined postorder deallocation

The file is intentionally verbose because it is a lecture artefact.

### `src/exercise1.c`

Implements the canonical tree from the specification:

```
              50
            /    \
           30     70
          / \    /  \
         20  40 60   80
```

and prints traversals and statistics in a strict format required by `tests/test1_expected.txt`.

### `src/exercise2.c`

Constructs three expression trees from postfix strings, prints each tree rotated, evaluates it and prints infix, prefix and postfix notations. Output is defined exactly by `tests/test2_expected.txt`.

The printing routine is a structural visualisation and is not a semantic requirement. It should be treated as a debugging aid whose role is to make the tree shape legible.

### `solution/*.c`

Instructor implementations that can be used as ground truth during marking and as reference during self-study.

## Cross-language correspondences

The algorithms are language-agnostic. Only memory management and representation syntax change.

### Python

```python
class Node:
    def __init__(self, x, left=None, right=None):
        self.x = x
        self.left = left
        self.right = right

def preorder(node):
    if node is None:
        return
    print(node.x, end=" ")
    preorder(node.left)
    preorder(node.right)
```

### C++

```cpp
struct Node {
    int x;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};

void preorder(const Node* n) {
    if (!n) return;
    std::cout << n->x << ' ';
    preorder(n->left.get());
    preorder(n->right.get());
}
```

The algorithm is identical to C. The ownership model changes because `std::unique_ptr` encodes deallocation in the type system.

### Java

```java
final class Node {
    final int x;
    Node left;
    Node right;
    Node(int x) { this.x = x; }
}

static void preorder(Node n) {
    if (n == null) return;
    System.out.print(n.x + " ");
    preorder(n.left);
    preorder(n.right);
}
```

Java removes explicit deallocation but does not remove the need to reason about reachability and recursion depth.


## Engineering considerations

### 1. Representation choices and invariants

This repository adopts a **pointer-based** representation because it exposes the memory model explicitly which is pedagogically essential in C. It is nevertheless important to understand that the same abstract data type admits multiple concrete representations whose asymptotic bounds can be identical while constant factors and failure modes differ materially.

**Pointer-based representation (used here).** Each node stores payload and two child pointers. The empty tree is represented by the null pointer. The fundamental invariants are:

- **Reachability invariant:** every non-NULL child pointer must either point to a valid allocated node or be NULL.
- **Acyclicity invariant:** following left or right pointers repeatedly must never return to a previously visited node.
- **Ownership invariant:** each allocated node is owned by exactly one parent edge or by a designated root variable and ownership is not aliased as mutable shared state.

The third invariant is a software engineering constraint rather than a mathematical necessity. It simplifies deallocation and makes reasoning about lifetime tractable.

**Array-based representation (not implemented).** For complete or nearly complete trees an array representation can be preferable. Using 0-based indexing, a node at index `i` has children at `2*i + 1` and `2*i + 2` when those indices are within bounds. This representation has excellent cache locality and eliminates pointer chasing. Its weakness is sparsity: a degenerate tree wastes memory because indices grow exponentially relative to the number of occupied nodes.

A precise way to state the trade-off is:

- Pointer-based: space is Θ(n) nodes plus pointer overhead with no wasted structural slots.
- Array-based: space is Θ(m) where `m` is the largest occupied index plus one which can be Θ(2^h) even when `n` is small.

### 2. Recursion as a resource and iterative variants

Recursive tree algorithms are conceptually aligned with the inductive definition of a tree. In C, recursion consumes call frames and can fail by stack exhaustion when `h` is large. This is not an abstract concern: a degenerate tree can force `h = n`.

An iterative traversal replaces the implicit call stack with an explicit stack. The trade is explicit bookkeeping in exchange for predictable memory usage patterns.

**Iterative preorder (explicit stack).**

```
PREORDER_ITER(root):
    if root == NULL: return
    S = empty stack
    push(S, root)
    while S not empty:
        u = pop(S)
        visit(u)
        if u.right != NULL: push(S, u.right)
        if u.left  != NULL: push(S, u.left)
```

Right is pushed before left so that left is processed first under LIFO discipline.

**Iterative inorder (explicit stack).**

```
INORDER_ITER(root):
    S = empty stack
    u = root
    while u != NULL or S not empty:
        while u != NULL:
            push(S, u)
            u = u.left
        u = pop(S)
        visit(u)
        u = u.right
```

This algorithm is a canonical example of a loop invariant: at each iteration, `S` contains exactly the ancestors of `u` whose left subtree has been fully processed while their own visit has not yet occurred.

### 3. Queue design in `example1.c`

The queue used for breadth-first traversal is implemented as a fixed-size circular buffer. The circular design ensures that enqueue and dequeue are constant-time operations with no shifting.

Let the queue state be `(front, rear, count)`. The implementation maintains the invariants:

- `0 ≤ front < MAX_QUEUE_SIZE` and `-1 ≤ rear < MAX_QUEUE_SIZE`
- `0 ≤ count ≤ MAX_QUEUE_SIZE`
- after each enqueue: `rear = (rear + 1) mod MAX_QUEUE_SIZE`
- after each dequeue: `front = (front + 1) mod MAX_QUEUE_SIZE`

This is sufficient to prove that the queue forms a correct FIFO discipline provided that overflow is prevented. The current implementation emits an error message on overflow which is appropriate for a teaching example. In production code one would typically reallocate to a larger buffer or refuse the operation as a recoverable error.

### 4. Deterministic testing and golden files

The `tests/` directory implements a lightweight regression framework based on **golden outputs**. Each programme is executed and its combined standard output and standard error are redirected to a temporary file. The file is compared to the expected output using `diff`.

This style of testing is deliberately strict and has three consequences:

1. Output formatting becomes part of the public interface of the programme.
2. Refactors that change formatting require an explicit update of the expected outputs.
3. Hidden nondeterminism (for example printing pointer addresses, time stamps or locale-dependent representations) is rejected automatically.

If a change is intended, regenerate the expected output in a controlled manner. For `exercise2` this is as simple as:

```bash
./exercise2 > tests/test2_expected.txt
```

A rigorous workflow treats the expected file as an artefact requiring review, not as an automatic by-product.

### 5. Common failure modes and how the code prevents them

The exercises embody a small set of deliberate defensive choices.

- **Dangling pointers after deallocation:** `exercise1.c` assigns `root = NULL` after freeing. This does not prevent all lifetime errors but it prevents accidental reuse of a freed root.
- **Memory leaks:** deallocation is implemented as a postorder traversal. This is not stylistic. If a parent node were freed before its children, the only remaining references to the children would be lost and the heap allocations would become unreachable.
- **Incorrect operand ordering in expression trees:** when processing an operator token in postfix, the first pop is the right subtree and the second pop is the left subtree. Reversing this produces a syntactically valid but semantically incorrect tree for non-commutative operators.
- **Operator precedence in infix printing:** infix output is fully parenthesised. This avoids the need for precedence tables and guarantees that the printed string represents exactly the same tree.

### 6. Extension tasks and research-grade questions

Once the baseline algorithms are mastered, the natural next step is to state and prove properties rather than merely implement functions.

Examples:

- Prove by structural induction that preorder, inorder and postorder visit each node exactly once.
- Prove that `tree_height` terminates for all finite trees and characterise its worst-case recursion depth.
- Characterise the set of postfix strings that yield well-formed binary expression trees in terms of a stack-height invariant: scanning left to right, let `s` be operands minus operators, then the expression is well formed if `s` never falls below 1 and ends at 1.

These questions are small but genuinely mathematical. They sharpen the distinction between an implementation that works empirically and an implementation that is correct by construction.


### 7. Tokenisation and numeric corner cases in `exercise2.c`

The postfix parser is intentionally minimalistic yet its design still illustrates several non-obvious points about turning a character stream into a sequence of semantic tokens.

1. **Token boundary definition.** The current implementation treats a single ASCII space as a separator because test inputs are provided in this format. For general use, `strtok(expr, " \t\n\r")` would be preferable because it accepts any run of whitespace.
2. **Ambiguity of '-'.** A hyphen can denote unary negation in an integer literal (for example `-5`) or the binary subtraction operator. Disambiguation is achieved here by a simple syntactic rule: a token is treated as an operator only if it has length exactly one and that character is in `{+, -, *, /}`. Under this rule, `-5` is unambiguously a number.
3. **Conversion to integer.** `atoi` is used for clarity. A production-grade implementation would prefer `strtol` so that overflow and conversion failure can be detected without invoking undefined behaviour.

A robust specification for conversion can be stated as a contract.

- **Precondition:** token matches the regular language `[+-]?[0-9]+`.
- **Postcondition:** result equals the integer denoted by the token if it lies within representable range otherwise an error is reported.

### 8. Integer overflow, division semantics and defined behaviour

The evaluation routine uses the built-in integer operators of C. This has two important consequences.

- **Division truncates toward zero** for C99 and later. This matters if negative operands are admitted.
- **Signed overflow is undefined behaviour** in ISO C. If expressions can exceed `INT_MAX` in magnitude then either a wider integer type must be used (for example `int64_t`) or overflow must be detected and handled.

In a teaching setting, these points are often omitted. In an academic setting they are part of the model: algorithmic correctness is defined relative to a machine semantics.

### 9. From demonstration code to library-quality code

The code is intentionally executable as standalone programmes. Turning it into a reusable library would require at least:

- a public header that exposes the abstract data type while hiding representation details
- explicit error reporting contracts rather than `fprintf` plus `exit`
- separation of parsing from evaluation so that expressions can be provided as streams rather than fixed strings
- tests that validate properties rather than exact formatting, for example a traversal that returns a list rather than printing

A useful intermediate step is to keep the existing programmes as integration tests while also writing unit tests for each function.

### 10. Minimal formal proof obligations

For each core function it is possible to write a compact proof obligation that can be checked by structural induction.

- `count_nodes`: prove that for every finite tree `T`, `count_nodes(T)` equals the cardinality of the node set of `T`.
- `free_tree`: prove that every allocated node is freed exactly once assuming the acyclicity invariant.
- `build_from_postfix`: prove that the algorithm terminates and that the produced tree's postfix traversal equals the input token sequence.

These are not merely theoretical exercises. They force the developer to identify hidden assumptions such as acyclicity, token well-formedness and bounded recursion depth.

## References

The following primary sources and archival publications underpin the theory used in this laboratory. DOI links are given as canonical resolvers.

| Reference (APA 7th ed) | DOI |
|---|---|
| Bayer, R., & McCreight, E. (1972). Organization and maintenance of large ordered indexes. *Acta Informatica, 1*, 173–189. | `https://doi.org/10.1007/BF00288683` |
| Dijkstra, E. W. (1968). The structure of the “THE”-multiprogramming system. *Communications of the ACM, 11*(5), 341–346. | `https://doi.org/10.1145/363095.363143` |
| Hoare, C. A. R. (1969). An axiomatic basis for computer programming. *Communications of the ACM, 12*(10), 576–580. | `https://doi.org/10.1145/363235.363259` |
| Pugh, W. (1990). Skip lists: A probabilistic alternative to balanced trees. *Communications of the ACM, 33*(6), 668–676. | `https://doi.org/10.1145/78973.78977` |
| Sethi, R., & Ullman, J. D. (1970). The generation of optimal code for arithmetic expressions. *Journal of the ACM, 17*(4), 715–728. | `https://doi.org/10.1145/321607.321620` |


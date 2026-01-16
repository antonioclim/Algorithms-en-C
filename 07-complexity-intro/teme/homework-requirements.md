# Week 07 Homework Specification: Binary Trees

## 1. Administrative information

- **Submission window:** the laboratory session of Week 08.
- **Assessed weight:** 100 points.
- **Implementation language:** ISO C11.
- **Toolchain expectations:** GCC with `-Wall -Wextra -std=c11` and a zero-warning policy.
- **Memory discipline:** every dynamically allocated object must be freed exactly once and no use-after-free or out-of-bounds access is acceptable.

This document specifies two programming assignments whose purpose is to move from syntactic familiarity with tree traversals to **specification-driven implementation**. Each task is stated with explicit preconditions and postconditions. You are expected to justify complexity claims concisely in comments and to demonstrate correctness through targeted tests.

## 2. Shared definitions and conventions

### 2.1. Base data structure

Unless the problem statement explicitly requires a different payload type, the canonical node definition is:

```c
typedef struct TreeNode {
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;
```

Interpretation:

- `NULL` denotes the empty tree.
- A node with `left == NULL` and `right == NULL` is a leaf.
- The representation must remain acyclic.

### 2.2. Defensive programming requirements

1. All public functions must handle `NULL` inputs gracefully.
2. All allocations must be checked for failure.
3. All output must be deterministic.
4. All helper routines must be declared `static` unless explicitly required elsewhere.

### 2.3. Complexity reporting

For each homework, include in a header comment:

- worst-case time complexity in Θ-notation
- auxiliary space complexity excluding the tree itself

Do not conflate recursion depth with constant space.

---

## 3. Homework 1: Binary tree serialisation and deserialisation (50 points)

### 3.1. Problem statement

Design a reversible mapping between a binary tree and a textual representation.

- **Serialisation** converts a tree into a string.
- **Deserialisation** reconstructs a tree from that string.

The mapping must be **lossless**: for any finite tree `T`, `deserialise(serialise(T))` must yield a tree that is structurally identical to `T` and has identical payload values at corresponding nodes.

### 3.2. Required format

Use preorder traversal with an explicit marker for empty subtrees.

- Integers are printed in base 10.
- Tokens are separated by a comma.
- The marker for the empty subtree is `#`.

Example:

```
            1
           / \
          2   3
             / \
            4   5

serialise(T) = "1,2,#,#,3,4,#,#,5,#,#"
```

### 3.3. Required API

Implement the following functions:

1. `char* serialise(const TreeNode *root)`
2. `TreeNode* deserialise(const char *data)`
3. `void free_tree(TreeNode *root)`
4. `int trees_identical(const TreeNode *a, const TreeNode *b)` (a small test helper is sufficient)

### 3.4. Specification

#### 3.4.1. `serialise`

- **Precondition:** `root` is either `NULL` or points to the root of an acyclic binary tree.
- **Postcondition:** returns a heap-allocated, NUL-terminated string that encodes the tree.
- **Memory contract:** the caller owns the returned string and must `free` it.

A correct implementation must handle:

- the empty tree (must return `"#"`)
- single-node trees
- multi-digit and negative integers

#### 3.4.2. `deserialise`

- **Precondition:** `data` is a NUL-terminated string in the specified format.
- **Postcondition:** returns the root of a newly allocated tree that decodes `data`.
- **Memory contract:** the caller owns the tree and must free it.

If you choose to implement input validation, invalid encodings may return `NULL` but must not leak memory.

### 3.5. Algorithmic guidance

#### 3.5.1. Serialisation pseudocode

```
SERIALISE(node):
    if node == NULL:
        emit("#")
        return

    emit(to_string(node.data))
    emit(",")
    SERIALISE(node.left)
    emit(",")
    SERIALISE(node.right)
```

In C, direct repeated concatenation is inefficient. A robust approach is a two-pass strategy:

1. compute an upper bound for output length
2. allocate once
3. write into the buffer with an index

#### 3.5.2. Deserialisation pseudocode

A standard technique is to scan tokens left-to-right while carrying a mutable cursor.

```
DESERIALISE(tokens, i):
    if tokens[i] == "#":
        i = i + 1
        return (NULL, i)

    x = parse_int(tokens[i])
    i = i + 1
    (L, i) = DESERIALISE(tokens, i)
    (R, i) = DESERIALISE(tokens, i)
    return (new_node(x, L, R), i)
```

The recursion works because preorder with null markers is a complete description of the tree.

### 3.6. Marking scheme

- Correctness of serialisation output: 20 points
- Correctness of reconstruction: 20 points
- Memory and ownership correctness: 10 points

---

## 4. Homework 2: Lowest Common Ancestor in a binary tree (50 points)

### 4.1. Problem statement

Given a binary tree and two target values `p` and `q`, compute their **lowest common ancestor** (LCA): the deepest node that lies on both root-to-`p` and root-to-`q` paths.

A node may be a descendant of itself which implies that if `p` is an ancestor of `q` then `LCA(p, q) = p`.

### 4.2. Required API

Implement:

1. `TreeNode* find_lca(TreeNode *root, int p, int q)`
2. `int contains_value(const TreeNode *root, int target)` (or an equivalent helper)
3. `void print_path_between(const TreeNode *root, int p, int q)`

You may choose between two families of approaches:

- a single-pass recursive LCA computation combined with membership checks
- explicit path construction followed by path comparison

Your code must return `NULL` when either `p` or `q` is absent.

### 4.3. Algorithmic guidance

#### 4.3.1. Canonical recursive LCA

```
LCA(root, p, q):
    if root == NULL: return NULL
    if root.data == p or root.data == q: return root

    left = LCA(root.left, p, q)
    right = LCA(root.right, p, q)

    if left != NULL and right != NULL:
        return root
    else:
        return (left if left != NULL else right)
```

To satisfy the requirement to return `NULL` when either value is absent, combine the above with membership checks or carry presence information in the recursion.

#### 4.3.2. Path-based strategy

Compute explicit root-to-node paths, then identify the last common prefix.

```
PATH(root, target, out):
    if root == NULL: return false
    append(out, root)
    if root.data == target: return true
    if PATH(root.left, target, out): return true
    if PATH(root.right, target, out): return true
    remove_last(out)
    return false

LCA_BY_PATH(root, p, q):
    P = []
    Q = []
    if not PATH(root, p, P): return NULL
    if not PATH(root, q, Q): return NULL

    i = 0
    while i < len(P) and i < len(Q) and P[i] == Q[i]:
        i = i + 1
    return P[i-1]
```

This approach is easy to reason about and is often acceptable for educational work. Its auxiliary space is linear in height.

### 4.4. Marking scheme

- Correctness on representative cases: 35 points
- Correct handling of absence and ancestor cases: 10 points
- Memory correctness and code quality: 5 points

---

## 5. Submission and reproducibility requirements

1. One `.c` file per homework as specified in the assignment brief.
2. A `main()` function that executes at least five test cases per homework.
3. Compilation must succeed without warnings.
4. The programme output should be stable between runs.

Suggested compilation commands:

```bash
gcc -Wall -Wextra -std=c11 -O2 -o hw1 homework1_serialisation.c
gcc -Wall -Wextra -std=c11 -O2 -o hw2 homework2_lca.c
```

## 6. References

The following references are provided for scholarly completeness and to encourage engagement with primary sources.

| Reference (APA 7th ed) | DOI |
|---|---|
| Bayer, R., & McCreight, E. (1972). Organization and maintenance of large ordered indexes. *Acta Informatica, 1*, 173–189. | `https://doi.org/10.1007/BF00288683` |
| Sethi, R., & Ullman, J. D. (1970). The generation of optimal code for arithmetic expressions. *Journal of the ACM, 17*(4), 715–728. | `https://doi.org/10.1145/321607.321620` |

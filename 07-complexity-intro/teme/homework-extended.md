# Extended challenges: Week 07 (Binary Trees)

## 1. Purpose and scope

The tasks in this document are optional extensions intended for students who have completed the core homework and wish to deepen their understanding of asymptotic reasoning, invariant-based design and careful memory discipline in C. Each challenge is phrased as a small research problem: you are asked not merely to produce a programme that happens to work but to produce an implementation whose complexity and corner-case behaviour you can explain with precision.

Unless otherwise stated, the baseline node type is:

```c
typedef struct TreeNode {
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;
```

## 2. Grading policy

Each correctly solved challenge awards **+10 points** applied to the Week 07 homework component. The total bonus is capped so that the overall homework score cannot exceed 100%.

Your submission must include:

- exactly one `.c` file per challenge
- a `main()` routine that exercises the implementation on non-trivial cases
- a brief complexity statement in comments


### 2.1. Methodological principles

The extended tasks are intentionally phrased to reward **method**, not merely output. A correct submission should make the following reasoning steps explicit in comments or in an accompanying note:

1. **Specification first:** state preconditions and postconditions for every public function. For instance, specify whether a function accepts `NULL` and how it interprets it.
2. **Invariant-driven recursion:** when using recursion, state the invariant maintained by the recursive call and identify the base case as an explicit termination argument.
3. **Complexity accounting:** justify the time bound by arguing how many times each node is processed. For O(n) tasks, you should be able to point to a single postorder or preorder pass rather than nested traversals.
4. **Ownership and lifetime:** define which function owns which allocation. If a function allocates memory for a return value, specify who frees it.
5. **Failure modes:** identify at least one malformed or extreme input for each challenge and show how your implementation behaves.

A useful pattern is to write a small internal helper that returns a richer result than the required interface. For example, a balance check can return a height when the subtree is balanced and a sentinel value when it is not. This separates the algorithmic idea from presentation and reduces duplication.

### 2.2. Testing guidance

Your tests should be structural rather than incidental. Prefer trees that exercise distinct shapes:

- the empty tree and the single-node tree
- a strictly left-leaning chain and a strictly right-leaning chain
- a perfectly balanced tree of height at least 3
- a tree whose left subtree is balanced but whose right subtree triggers the failure condition

When a function returns an array, verify both the array content and the reported length. When a function mutates the tree in-place, verify the post-state by traversal and by local pointer checks.

## 3. Challenge 1: Tree diameter (O(n) required)

### 3.1. Definition

The **diameter** of a tree is the length of the longest simple path between any two nodes measured in **edges**. The path may or may not pass through the root.

### 3.2. Required interface

```c
int tree_diameter(TreeNode *root);
```

### 3.3. Algorithmic requirement

A naive solution recomputes heights for many subtrees and typically runs in O(n²). You are required to produce an O(n) solution.

One robust approach is to compute, for each node, the pair:

- `height(node)`: height of subtree rooted at node
- `diameter(node)`: diameter of subtree rooted at node

and to combine them in a single postorder traversal.

### 3.4. Pseudocode

```
POSTORDER_DIAMETER(node):
    if node == NULL:
        return (height = -1, diameter = 0)

    (hl, dl) = POSTORDER_DIAMETER(node.left)
    (hr, dr) = POSTORDER_DIAMETER(node.right)

    height = 1 + max(hl, hr)

    through = hl + hr + 2      // number of edges on path via node
    diameter = max(through, dl, dr)

    return (height, diameter)
```

The public function returns `diameter` from the tuple computed at the root.

### 3.5. Corner cases

- empty tree: diameter 0
- single node: diameter 0
- degenerate chain: diameter equals `n - 1`

## 4. Challenge 2: Height balance in one pass

### 4.1. Definition

A binary tree is **height-balanced** if, for every node, the heights of the left and right subtrees differ by at most 1.

### 4.2. Required interface

```c
bool is_balanced(TreeNode *root);
```

### 4.3. Algorithmic requirement

The preferred solution computes height and balance simultaneously, short-circuiting as soon as an imbalance is detected.

### 4.4. Pseudocode (sentinel height)

```
CHECK(node):
    if node == NULL:
        return 0

    hl = CHECK(node.left)
    if hl == -1: return -1

    hr = CHECK(node.right)
    if hr == -1: return -1

    if abs(hl - hr) > 1:
        return -1

    return 1 + max(hl, hr)

IS_BALANCED(root):
    return CHECK(root) != -1
```

The sentinel value `-1` is not a height. It is an error code that propagates failure without additional data structures.

## 5. Challenge 3: Right side view

### 5.1. Definition

The **right side view** is the sequence of node values visible when the tree is observed from the right. Equivalently, it is the last node encountered at each depth in a level-order traversal.

### 5.2. Required interface

```c
int* right_side_view(TreeNode *root, int *size);
```

The function returns a heap-allocated array whose length is written to `*size`. The caller owns the array and must free it.

### 5.3. Two acceptable approaches

1. **BFS**: traverse level by level and record the final node of each level.
2. **DFS**: traverse right-first and record the first node seen at each depth.

### 5.4. DFS pseudocode (right-first)

```
RIGHT_VIEW(node, depth, seen, out):
    if node == NULL:
        return

    if depth == seen:
        out.append(node.data)
        seen = seen + 1

    RIGHT_VIEW(node.right, depth + 1, seen, out)
    RIGHT_VIEW(node.left,  depth + 1, seen, out)
```

This approach records the first node encountered at each depth under a right-first traversal.

## 6. Challenge 4: Flatten to a right-linked list in-place

### 6.1. Definition

Transform the tree so that it becomes a linked list along right pointers in preorder sequence. All left pointers must become `NULL`. No new nodes may be allocated.

### 6.2. Required interface

```c
void flatten(TreeNode *root);
```

### 6.3. Morris-style algorithm

The key idea is to splice subtrees using the rightmost node of the left subtree.

```
FLATTEN(u):
    while u != NULL:
        if u.left != NULL:
            p = u.left
            while p.right != NULL:
                p = p.right

            p.right = u.right
            u.right = u.left
            u.left = NULL

        u = u.right
```

This algorithm is O(n) time and O(1) auxiliary space because it reuses existing pointers.

## 7. Challenge 5: Construct a tree from preorder and inorder traversals

### 7.1. Statement

Given arrays `preorder` and `inorder` representing traversals of a binary tree with unique values, reconstruct the original tree.

### 7.2. Required interface

```c
TreeNode* build_tree(const int *preorder, int pre_size, const int *inorder, int in_size);
```

### 7.3. Complexity requirement

A naive solution searches for the root in the inorder array at each recursion step and typically runs in O(n²). A full-credit solution uses an index structure (hash table or sorted map) so that the root position is found in O(1) expected time, yielding O(n) expected time overall.

### 7.4. Pseudocode

```
BUILD(preL, preR, inL, inR):
    if preL > preR:
        return NULL

    rootVal = preorder[preL]
    k = index[rootVal]          // position in inorder

    leftSize = k - inL

    root = new_node(rootVal)
    root.left  = BUILD(preL + 1, preL + leftSize, inL, k - 1)
    root.right = BUILD(preL + leftSize + 1, preR, k + 1, inR)

    return root
```

## 8. Submission and review checklist

- provide tests that exercise empty trees, single-node trees and highly unbalanced trees
- state time and space complexity and ensure that the implementation matches the statement
- keep ownership explicit: every allocation must be freed
- avoid global state unless it is part of the specification

## 9. References

| Reference (APA 7th ed) | DOI |
|---|---|
| Pugh, W. (1990). Skip lists: A probabilistic alternative to balanced trees. *Communications of the ACM, 33*(6), 668–676. | `https://doi.org/10.1145/78973.78977` |
| Sethi, R., & Ullman, J. D. (1970). The generation of optimal code for arithmetic expressions. *Journal of the ACM, 17*(4), 715–728. | `https://doi.org/10.1145/321607.321620` |

# Week 07 Homework: Binary Trees

## 📋 General Information

- **Deadline:** End of Week 08 (submit before laboratory)
- **Total Points:** 100 (10% of final grade)
- **Language:** C (C11 standard)
- **Compiler:** GCC with `-Wall -Wextra -std=c11`
- **Memory:** All solutions must pass Valgrind without leaks

---

## 📝 Homework 1: Binary Tree Serialisation (50 points)

### Description

Implement functions to **serialise** a binary tree to a string and **deserialise** it back to a tree. This is a common interview question and fundamental for tree persistence.

**Serialisation** converts a tree structure to a linear string format.
**Deserialisation** reconstructs the original tree from the string.

### Serialisation Format

Use **preorder traversal** with a special marker for NULL nodes:
- Numbers are separated by commas
- NULL nodes are represented by `#`

**Example:**
```
Tree:        Serialised: "1,2,#,#,3,4,#,#,5,#,#"
    1
   / \
  2   3
     / \
    4   5
```

### Requirements

1. **TreeNode Structure** (5 points)
   - Standard binary tree node with integer data

2. **`char* serialise(TreeNode *root)`** (20 points)
   - Convert tree to string using preorder with NULL markers
   - Handle empty tree (return "#")
   - Dynamically allocate result string
   - Handle multi-digit and negative numbers

3. **`TreeNode* deserialise(char *data)`** (20 points)
   - Reconstruct tree from serialised string
   - Handle empty tree input ("#")
   - Use recursive approach with index tracking
   - Properly allocate all nodes

4. **Memory Management** (5 points)
   - Free serialised string after use
   - Free reconstructed tree after use
   - No memory leaks (Valgrind clean)

### Example Usage

```c
/* Original tree */
TreeNode *root = create_node(1);
root->left = create_node(2);
root->right = create_node(3);
root->right->left = create_node(4);
root->right->right = create_node(5);

/* Serialise */
char *data = serialise(root);
printf("Serialised: %s\n", data);  /* "1,2,#,#,3,4,#,#,5,#,#" */

/* Deserialise */
TreeNode *reconstructed = deserialise(data);

/* Verify trees are identical */
assert(trees_identical(root, reconstructed));

/* Clean up */
free(data);
free_tree(root);
free_tree(reconstructed);
```

### File: `homework1_serialisation.c`

---

## 📝 Homework 2: Lowest Common Ancestor (50 points)

### Description

Implement the **Lowest Common Ancestor (LCA)** algorithm for a binary tree. The LCA of two nodes p and q is the deepest node that has both p and q as descendants (a node can be a descendant of itself).

This is a classic interview question asked at companies like Google, Amazon and Meta.

### Requirements

1. **TreeNode Structure** (5 points)
   - Standard binary tree node with integer data

2. **`TreeNode* find_lca(TreeNode *root, int p, int q)`** (25 points)
   - Find the lowest common ancestor of nodes with values p and q
   - Return NULL if either p or q is not in the tree
   - Handle edge cases:
     - One node is ancestor of the other
     - Both nodes are the same
     - Nodes in different subtrees

3. **`bool path_to_node(TreeNode *root, int target, int *path, int *len)`** (10 points)
   - Find the path from root to a node with given value
   - Store node values in the path array
   - Update len with path length
   - Return true if found, false otherwise

4. **`void print_path(TreeNode *root, int p, int q)`** (5 points)
   - Print the path from node p to node q through LCA
   - Format: "p -> ... -> LCA -> ... -> q"

5. **Memory Management** (5 points)
   - No memory leaks
   - Valgrind clean

### Algorithm Hint

**Recursive approach:**
```
LCA(root, p, q):
    if root is NULL: return NULL
    if root.data == p or root.data == q: return root
    
    left_lca = LCA(root.left, p, q)
    right_lca = LCA(root.right, p, q)
    
    if both left_lca and right_lca are non-NULL:
        return root  // p and q are in different subtrees
    
    return whichever is non-NULL (or NULL if both NULL)
```

### Example Usage

```c
/*
        3
       / \
      5   1
     / \ / \
    6  2 0  8
      / \
     7   4
*/

TreeNode *root = build_example_tree();

TreeNode *lca1 = find_lca(root, 5, 1);
printf("LCA(5, 1) = %d\n", lca1->data);  /* 3 */

TreeNode *lca2 = find_lca(root, 5, 4);
printf("LCA(5, 4) = %d\n", lca2->data);  /* 5 */

TreeNode *lca3 = find_lca(root, 6, 4);
printf("LCA(6, 4) = %d\n", lca3->data);  /* 5 */

print_path(root, 6, 4);  /* "6 -> 5 -> 2 -> 4" */
```

### File: `homework2_lca.c`

---

## 📊 Evaluation Criteria

| Criterion | Homework 1 | Homework 2 |
|-----------|------------|------------|
| **Functional Correctness** | 35 pts | 35 pts |
| **Edge Case Handling** | 5 pts | 5 pts |
| **Code Quality** | 5 pts | 5 pts |
| **Memory Management** | 5 pts | 5 pts |
| **Total** | 50 pts | 50 pts |

### Detailed Breakdown

**Functional Correctness:**
- Serialise produces correct output (15 pts)
- Deserialise reconstructs correctly (15 pts)
- Round-trip works (serialise → deserialise = original) (5 pts)

**Edge Cases:**
- Empty tree (1 pt)
- Single node (1 pt)
- Left/right-only paths (1 pt)
- Negative numbers (1 pt)
- Large trees (1 pt)

**Code Quality:**
- Clear variable names (1 pt)
- Proper comments (2 pts)
- Consistent formatting (1 pt)
- No magic numbers (1 pt)

### Penalties

| Issue | Penalty |
|-------|---------|
| Compiler warnings | -5 pts |
| Memory leaks (Valgrind) | -10 pts |
| Crashes on valid input | -15 pts |
| Incorrect output format | -5 pts |
| Late submission (per day) | -10 pts |
| Plagiarism | -100% + disciplinary |

---

## 📤 Submission Instructions

1. **File Naming:**
   - `homework1_serialisation.c`
   - `homework2_lca.c`

2. **Header Comment:**
   ```c
   /**
    * Week 07 Homework X: [Title]
    * Student: [Your Name]
    * Group: [Your Group]
    * Date: [Submission Date]
    */
   ```

3. **Compilation Test:**
   ```bash
   gcc -Wall -Wextra -std=c11 -o hw1 homework1_serialisation.c
   gcc -Wall -Wextra -std=c11 -o hw2 homework2_lca.c
   ```

4. **Memory Test:**
   ```bash
   valgrind --leak-check=full ./hw1
   valgrind --leak-check=full ./hw2
   ```

5. **Submit via:** University portal before deadline

---

## 💡 Tips for Success

1. **Start with the structure:** Define TreeNode and helper functions first

2. **Test incrementally:** Test serialise before deserialise

3. **Draw trees:** Visualise your test cases on paper

4. **Use Valgrind early:** Don't wait until the end to check memory

5. **Handle edge cases:** Empty tree, single node, degenerate trees

6. **Read the examples carefully:** They clarify the expected behaviour

7. **Ask questions:** Use the forum or office hours if stuck

---

## 📚 Recommended Resources

- Knuth, TAOCP Vol. 1, §2.3 (Tree traversals)
- LeetCode #297 (Serialise and Deserialise Binary Tree)
- LeetCode #236 (Lowest Common Ancestor of a Binary Tree)
- GeeksforGeeks: Tree Serialisation, LCA algorithms

---

*Good luck! Remember: clean code and zero memory leaks are as important as correct output.*

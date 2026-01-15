# Extended Challenges - Week 07: Binary Trees

## 🚀 Advanced Challenges (Optional)

Each correctly solved challenge awards **+10 bonus points** to your homework grade.

Maximum bonus: **+50 points** (capped at 100% total homework score)

---

## ⭐ Challenge 1: Tree Diameter (Difficulty: Medium)

### Description

The **diameter** (or width) of a binary tree is the length of the longest path between any two nodes. The path may or may not pass through the root.

**Important:** The length is measured in **edges**, not nodes.

### Requirements

1. Implement `int tree_diameter(TreeNode *root)`
2. Return the longest path length (number of edges)
3. Handle all tree shapes efficiently
4. Time complexity: O(n)

### Examples

```
Tree 1:         Diameter = 4 (path: 4 → 2 → 1 → 3 → 6)
       1
      / \
     2   3
    /     \
   4       6

Tree 2:         Diameter = 4 (path: D → B → A → C → G)
       A
      / \
     B   C
    /   / \
   D   E   G
          / \
         H   I

Tree 3:         Diameter = 2
       1
      / \
     2   3
```

### Hint

The diameter is the maximum of:
- Diameter of left subtree
- Diameter of right subtree
- Longest path through root (left_height + right_height + 2)

### File: `challenge1_diameter.c`
### Bonus Points: +10

---

## ⭐ Challenge 2: Check if Tree is Balanced (Difficulty: Medium)

### Description

A **height-balanced** binary tree is defined as a tree in which the depth of the two subtrees of every node never differs by more than 1.

### Requirements

1. Implement `bool is_balanced(TreeNode *root)`
2. Return true if tree is height-balanced
3. **Optimised:** Single traversal (O(n) time)
4. Early termination when imbalance detected

### Examples

```
Balanced:           Not Balanced:
       3                  1
      / \                  \
     9  20                  2
       /  \                  \
      15   7                  3
                               \
                                4
```

### Naive vs Optimised

**Naive (O(n²)):** Check height difference at each node
```c
bool is_balanced_naive(TreeNode *root) {
    if (root == NULL) return true;
    int diff = abs(height(root->left) - height(root->right));
    return diff <= 1 && 
           is_balanced_naive(root->left) && 
           is_balanced_naive(root->right);
}
```

**Optimised (O(n)):** Compute height and check balance in one pass
- Return -1 if subtree is unbalanced (early exit)
- Otherwise return actual height

### File: `challenge2_balanced.c`
### Bonus Points: +10

---

## ⭐ Challenge 3: Right Side View (Difficulty: Medium)

### Description

Given a binary tree, imagine yourself standing on the **right side** of it. Return the values of the nodes you can see, ordered from top to bottom.

This is equivalent to returning the **last node at each level** in level-order traversal.

### Requirements

1. Implement `int* right_side_view(TreeNode *root, int *size)`
2. Return array of visible node values
3. Set *size to the number of visible nodes
4. Caller is responsible for freeing the returned array

### Examples

```
Tree:               Right Side View: [1, 3, 4]
       1
      / \
     2   3
      \   \
       5   4

Tree:               Right Side View: [1, 3]
       1
        \
         3

Tree:               Right Side View: [1, 2, 4]
       1
      /
     2
    /
   4
```

### Approaches

1. **Level-order (BFS):** Track last node at each level
2. **Modified preorder (DFS):** Visit right before left, track depth

### File: `challenge3_rightview.c`
### Bonus Points: +10

---

## ⭐ Challenge 4: Flatten Tree to Linked List (Difficulty: Medium-Hard)

### Description

Given a binary tree, **flatten** it to a "linked list" in-place:
- Use the TreeNode's **right** pointer as the "next" pointer
- Use the TreeNode's **left** pointer set to NULL
- Order: preorder traversal order

### Requirements

1. Implement `void flatten(TreeNode *root)`
2. Modify tree **in-place** (no new nodes)
3. All left pointers become NULL
4. Right pointers form the linked list

### Example

```
Before:                 After:
       1                1
      / \                \
     2   5                2
    / \   \                \
   3   4   6                3
                             \
                              4
                               \
                                5
                                 \
                                  6
```

### Algorithm Hint

**Morris-style approach:**
1. If current node has left child:
   - Find rightmost node of left subtree
   - Attach current's right subtree to that rightmost node
   - Move left subtree to right
   - Set left to NULL
2. Move to right child
3. Repeat

### File: `challenge4_flatten.c`
### Bonus Points: +10

---

## ⭐ Challenge 5: Construct Tree from Traversals (Difficulty: Hard)

### Description

Given **preorder** and **inorder** traversal arrays of a binary tree, construct and return the original tree.

**Assumption:** All values in the tree are unique.

### Requirements

1. Implement `TreeNode* build_tree(int *preorder, int pre_size, int *inorder, int in_size)`
2. Handle trees of any shape
3. Return NULL for empty arrays
4. Efficient implementation (O(n) with hash map for index lookup, or O(n²) naive)

### Key Insight

- **Preorder** first element is always the root
- **Inorder** elements left of root are in left subtree, elements right are in right subtree

### Example

```
preorder = [3, 9, 20, 15, 7]
inorder  = [9, 3, 15, 20, 7]

Result:
       3
      / \
     9  20
       /  \
      15   7

Step-by-step:
1. preorder[0] = 3 is root
2. In inorder, 3 is at index 1
3. inorder[0:1] = [9] is left subtree
4. inorder[2:5] = [15, 20, 7] is right subtree
5. Recurse...
```

### Algorithm

```
buildTree(preorder, inorder):
    if empty: return NULL
    
    root = create_node(preorder[0])
    idx = find index of preorder[0] in inorder
    
    root.left = buildTree(preorder[1:idx+1], inorder[0:idx])
    root.right = buildTree(preorder[idx+1:], inorder[idx+1:])
    
    return root
```

### File: `challenge5_construct.c`
### Bonus Points: +10

---

## 📊 Bonus Point System

| Challenges Completed | Total Bonus |
|---------------------|-------------|
| 1 challenge | +10 points |
| 2 challenges | +20 points |
| 3 challenges | +30 points |
| 4 challenges | +40 points |
| All 5 challenges | +50 points + 🏆 **"Tree Master"** badge |

### Badge Recognition

Students who complete all 5 challenges will receive:
- Special mention in the course hall of fame
- Priority consideration for teaching assistant positions
- Reference letter mention (upon request)

---

## 📋 Submission Guidelines

1. **One file per challenge**
2. **Include main() with test cases**
3. **Compile without warnings**
4. **Pass Valgrind memory check**
5. **Document your approach in comments**

### File Header

```c
/**
 * Week 07 Challenge X: [Title]
 * Student: [Your Name]
 * Group: [Your Group]
 * Approach: [Brief description of your algorithm]
 * Time Complexity: O(?)
 * Space Complexity: O(?)
 */
```

---

## 💡 Tips for Challenges

1. **Challenge 1 (Diameter):** Think about how to avoid recomputing heights

2. **Challenge 2 (Balanced):** Use -1 as a sentinel for "unbalanced"

3. **Challenge 3 (Right View):** Level-order is more intuitive; DFS is more elegant

4. **Challenge 4 (Flatten):** Draw the transformation step by step

5. **Challenge 5 (Construct):** The key is understanding the relationship between traversals

---

## 🔗 Reference Problems

- LeetCode #543: Diameter of Binary Tree
- LeetCode #110: Balanced Binary Tree
- LeetCode #199: Binary Tree Right Side View
- LeetCode #114: Flatten Binary Tree to Linked List
- LeetCode #105: Construct Binary Tree from Preorder and Inorder Traversal

---

*These challenges represent common interview questions at top tech companies. Mastering them will significantly boost your problem-solving skills and interview preparedness.*

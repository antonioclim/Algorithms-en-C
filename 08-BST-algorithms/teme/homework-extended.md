# Extended Challenges - Week 08: Binary Search Trees

## 🚀 Advanced Challenges (Optional)

Each correctly solved challenge: **+10 bonus points**

Maximum bonus: **+50 points** (all 5 challenges)

These challenges are designed to deepen your understanding of Binary Search Trees and prepare you for technical interviews. They go beyond the basic homework requirements.

---

## ⭐ Challenge 1: Self-Balancing Detection (Difficulty: Medium)

### Description

Implement functions to analyse tree balance and detect when a BST has become severely unbalanced.

### Requirements

1. **Balance Factor Calculation**
   ```c
   int balance_factor(BSTNode *node);
   ```
   - Calculate balance factor: height(left) - height(right)
   - Return 0 for NULL nodes

2. **Balance Check**
   ```c
   bool is_balanced(BSTNode *root);
   ```
   - Return true if |balance_factor| <= 1 for ALL nodes
   - Use recursive approach

3. **Imbalance Report**
   ```c
   void report_imbalances(BSTNode *root);
   ```
   - Print all nodes where |balance_factor| > 1
   - Include node key and balance factor

4. **Degenerate Detection**
   ```c
   bool is_degenerate(BSTNode *root);
   ```
   - Return true if tree resembles a linked list
   - (Every node has at most one child)

### Example Output

```
Balance Analysis:
  Node 50: balance_factor = -2 (UNBALANCED!)
  Node 70: balance_factor = -1
  Node 80: balance_factor = -1
Tree is NOT balanced.
```

### Bonus Points: +10

---

## ⭐ Challenge 2: BST Iterator (Difficulty: Medium-Hard)

### Description

Implement an iterator that allows in-order traversal without recursion, returning one element at a time. This is a common interview question (LeetCode #173).

### Requirements

1. **Iterator Structure**
   ```c
   typedef struct BSTIterator {
       /* Your implementation - hint: use a stack */
   } BSTIterator;
   ```

2. **Initialisation**
   ```c
   BSTIterator *bst_iterator_create(BSTNode *root);
   ```
   - Create and initialise the iterator
   - Push all left nodes onto stack

3. **Next Element**
   ```c
   int bst_iterator_next(BSTIterator *iter);
   ```
   - Return the next smallest element
   - Update internal state
   - O(1) amortised time complexity

4. **Has Next**
   ```c
   bool bst_iterator_has_next(BSTIterator *iter);
   ```
   - Return true if more elements exist
   - O(1) time complexity

5. **Cleanup**
   ```c
   void bst_iterator_free(BSTIterator *iter);
   ```

### Example Usage

```c
BSTNode *root = /* tree with 1,2,3,4,5 */;
BSTIterator *iter = bst_iterator_create(root);

while (bst_iterator_has_next(iter)) {
    printf("%d ", bst_iterator_next(iter));
}
/* Output: 1 2 3 4 5 */

bst_iterator_free(iter);
```

### Bonus Points: +10

---

## ⭐ Challenge 3: Build Balanced BST from Sorted Array (Difficulty: Medium)

### Description

Given a sorted array, construct a height-balanced BST. This is the reverse of in-order traversal and appears in LeetCode #108.

### Requirements

1. **Main Function**
   ```c
   BSTNode *sorted_array_to_bst(int *arr, int n);
   ```
   - Input: sorted array of n integers
   - Output: root of height-balanced BST
   - All elements should be in the tree

2. **Helper Function** (recursive approach)
   ```c
   BSTNode *build_balanced(int *arr, int start, int end);
   ```
   - Pick middle element as root
   - Recursively build left subtree from left half
   - Recursively build right subtree from right half

3. **Verification**
   - The resulting tree must be a valid BST
   - Height should be ⌊log₂(n)⌋ or ⌈log₂(n)⌉

### Example

```
Input array: [1, 2, 3, 4, 5, 6, 7]

Output tree:
       4
      / \
     2   6
    / \ / \
   1  3 5  7

Height: 2 (optimal for 7 nodes)
```

### Bonus Points: +10

---

## ⭐ Challenge 4: Threaded Binary Search Tree (Difficulty: Hard)

### Description

Implement a threaded BST where NULL pointers are replaced with pointers to in-order predecessor/successor. This allows in-order traversal without recursion or stack.

### Requirements

1. **Modified Node Structure**
   ```c
   typedef struct ThreadedNode {
       int key;
       struct ThreadedNode *left;
       struct ThreadedNode *right;
       bool left_is_thread;   /* true if left points to predecessor */
       bool right_is_thread;  /* true if right points to successor */
   } ThreadedNode;
   ```

2. **Insertion**
   ```c
   ThreadedNode *threaded_insert(ThreadedNode *root, int key);
   ```
   - Maintain thread pointers during insertion
   - Update predecessor/successor links

3. **In-order Traversal (Iterative)**
   ```c
   void threaded_inorder(ThreadedNode *root);
   ```
   - Use threads to traverse without recursion
   - No stack needed!
   - O(n) time, O(1) space

4. **Find Successor**
   ```c
   ThreadedNode *inorder_successor(ThreadedNode *node);
   ```
   - Return the in-order successor using threads
   - O(1) time if right_is_thread is true

### Example Structure

```
For tree: 1 - 2 - 3 - 4 - 5

Normal:          Threaded:
    2               2
   / \             /|\
  1   4           1 | 4
     / \         /| |/|\
    3   5       X 3-+ 5-X

X = NULL, arrows show thread links
```

### Bonus Points: +10

---

## ⭐ Challenge 5: BST Reconstruction from Traversals (Difficulty: Hard)

### Description

Reconstruct a BST given its pre-order traversal (or post-order traversal). This tests deep understanding of BST properties and traversal orders.

### Requirements

1. **From Pre-order**
   ```c
   BSTNode *bst_from_preorder(int *preorder, int n);
   ```
   - Input: pre-order traversal array
   - Output: reconstructed BST
   - Use BST property for efficient O(n) solution

2. **From Post-order**
   ```c
   BSTNode *bst_from_postorder(int *postorder, int n);
   ```
   - Same as above but from post-order

3. **Verification Function**
   ```c
   bool verify_reconstruction(BSTNode *original, BSTNode *reconstructed);
   ```
   - Compare two trees for structural equality

### Algorithm Hints

**For pre-order [50, 30, 20, 40, 70, 60, 80]:**
- First element (50) is root
- Elements < 50 form left subtree: [30, 20, 40]
- Elements > 50 form right subtree: [70, 60, 80]
- Recursively apply to subtrees

**Optimised approach uses upper bound:**
```c
BSTNode *build(int *pre, int n, int *idx, int bound);
```
- Track current index
- Use bound to determine when subtree ends
- O(n) time complexity

### Example

```
Input pre-order: [50, 30, 20, 40, 70, 60, 80]

Reconstructed tree:
       50
      /  \
    30    70
   /  \  /  \
  20  40 60  80
```

### Bonus Points: +10

---

## 📊 Bonus Point System

| Challenges Completed | Total Bonus | Special Recognition |
|---------------------|-------------|---------------------|
| 1 | +10 points | — |
| 2 | +20 points | — |
| 3 | +30 points | — |
| 4 | +40 points | — |
| All 5 | +50 points | 🏆 "BST Master" badge |

---

## 📤 Submission Guidelines

1. **File Naming:** `challenge[1-5]_[short_name].c`
   - Example: `challenge1_balance.c`
   - Example: `challenge2_iterator.c`

2. **Include Test Cases:** Each submission should include a `main()` function demonstrating the functionality.

3. **Documentation:** Comment your code explaining the algorithm and time/space complexity.

4. **Deadline:** Same as main homework (end of Week 09)

---

## 💡 Interview Connection

These challenges directly relate to common technical interview questions:

| Challenge | Related Interview Questions |
|-----------|---------------------------|
| Balance Detection | "Check if a binary tree is balanced" |
| BST Iterator | LeetCode #173 - BST Iterator |
| Array to BST | LeetCode #108 - Convert Sorted Array to BST |
| Threaded BST | "Morris Traversal" variant |
| Reconstruction | LeetCode #1008 - Construct BST from Preorder |

Completing these challenges will significantly improve your interview preparation!

---

*"The best way to learn is by doing. Challenge yourself!"*

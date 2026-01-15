# Extended Challenges - Week 09: AVL Trees

## 🚀 Advanced Challenges (Optional)

Each correctly solved challenge: **+10 bonus points**

These challenges extend beyond the basic homework requirements and are designed for students who want to deepen their understanding of AVL trees and self-balancing data structures.

---

## ⭐ Challenge 1: AVL Tree with Parent Pointers (Difficulty: Medium)

### Description

Modify the AVL tree implementation to include parent pointers. This enables efficient successor/predecessor operations without recursion and simplifies certain algorithms.

### Requirements

1. Add a `parent` pointer to each node
2. Maintain parent pointers during insertion and deletion
3. Update parent pointers correctly during rotations
4. Implement iterative (non-recursive) inorder traversal using parent pointers
5. Implement `next()` and `prev()` iterator functions

### Interface

```c
typedef struct AVLNodeP {
    int key;
    int height;
    struct AVLNodeP *left;
    struct AVLNodeP *right;
    struct AVLNodeP *parent;
} AVLNodeP;

AVLNodeP *avl_insert_p(AVLNodeP *root, int key);
AVLNodeP *avl_delete_p(AVLNodeP *root, int key);
AVLNodeP *avl_next(AVLNodeP *node);     /* Inorder successor */
AVLNodeP *avl_prev(AVLNodeP *node);     /* Inorder predecessor */
void avl_iterate(AVLNodeP *root);        /* Iterative inorder */
```

### Testing

```c
/* Should print 1 2 3 4 5 in order without recursion */
AVLNodeP *min = avl_find_min(root);
for (AVLNodeP *n = min; n != NULL; n = avl_next(n)) {
    printf("%d ", n->key);
}
```

### Bonus Points: +10

---

## ⭐ Challenge 2: AVL Tree Serialisation (Difficulty: Medium)

### Description

Implement functions to serialise an AVL tree to a file and deserialise it back. The tree structure must be preserved exactly.

### Requirements

1. Serialise tree to binary file (compact format)
2. Deserialise tree from binary file
3. Handle empty trees
4. Verify tree is valid AVL after deserialisation
5. Support trees up to 1 million nodes

### File Format

```
[4 bytes: node count]
[For each node in preorder:]
  [4 bytes: key]
  [1 byte: flags (has_left, has_right)]
```

### Interface

```c
int avl_save(AVLNode *root, const char *filename);
AVLNode *avl_load(const char *filename);
```

### Testing

```c
AVLNode *original = /* build tree */;
avl_save(original, "tree.dat");
AVLNode *loaded = avl_load("tree.dat");
assert(trees_equal(original, loaded));
```

### Bonus Points: +10

---

## ⭐ Challenge 3: Augmented AVL Tree - Order Statistics (Difficulty: Hard)

### Description

Augment the AVL tree to support order statistics queries in O(log n) time. This enables finding the kth smallest element and determining the rank of any element.

### Requirements

1. Add a `size` field to each node (number of nodes in subtree)
2. Maintain size during insertion and deletion
3. Update size correctly during rotations
4. Implement `select(k)` - find kth smallest element
5. Implement `rank(key)` - count elements less than key

### Interface

```c
typedef struct AVLNodeOS {
    int key;
    int height;
    int size;       /* Number of nodes in subtree */
    struct AVLNodeOS *left;
    struct AVLNodeOS *right;
} AVLNodeOS;

AVLNodeOS *avl_select(AVLNodeOS *root, int k);   /* k is 1-indexed */
int avl_rank(AVLNodeOS *root, int key);
```

### Example

```c
/* Tree contains: 10, 20, 30, 40, 50 */
avl_select(root, 1)->key;   /* Returns 10 (1st smallest) */
avl_select(root, 3)->key;   /* Returns 30 (3rd smallest) */
avl_rank(root, 35);         /* Returns 3 (three elements < 35) */
```

### Bonus Points: +10

---

## ⭐ Challenge 4: AVL Tree Merge and Split (Difficulty: Hard)

### Description

Implement efficient merge and split operations for AVL trees. These are fundamental operations for implementing more advanced data structures like ropes and persistent trees.

### Requirements

1. **Split**: Given a tree and a key k, split into two trees:
   - Left tree contains all keys < k
   - Right tree contains all keys >= k
   
2. **Merge**: Given two trees where all keys in T1 < all keys in T2, merge into one balanced tree

3. Both operations should be O(log n)

### Interface

```c
typedef struct SplitResult {
    AVLNode *left;
    AVLNode *right;
} SplitResult;

SplitResult avl_split(AVLNode *root, int key);
AVLNode *avl_merge(AVLNode *left, AVLNode *right);
```

### Testing

```c
/* Original: 1, 2, 3, 4, 5, 6, 7 */
SplitResult sr = avl_split(root, 4);
/* sr.left: 1, 2, 3 */
/* sr.right: 4, 5, 6, 7 */

AVLNode *merged = avl_merge(sr.left, sr.right);
/* merged: 1, 2, 3, 4, 5, 6, 7 */
```

### Bonus Points: +10

---

## ⭐ Challenge 5: AVL Tree vs Red-Black Tree Benchmark (Difficulty: Medium)

### Description

Implement a Red-Black tree and conduct a comprehensive performance comparison with AVL trees. This provides insight into why different libraries choose different self-balancing trees.

### Requirements

1. Implement a basic Red-Black tree (insert, delete, search)
2. Create a benchmarking framework
3. Test with various workloads:
   - Random insertions
   - Sorted insertions
   - Mixed insert/delete/search
   - Read-heavy workload
   - Write-heavy workload
4. Measure:
   - Number of rotations
   - Total comparisons
   - Wall-clock time
5. Generate a comparison report

### Output Format

```
=== AVL vs Red-Black Tree Benchmark ===

Test: 100,000 random insertions
                    AVL         Red-Black
  Rotations:        45,231      31,456
  Comparisons:      1,723,456   1,834,212
  Time (ms):        45          42
  Final Height:     17          19

Test: 100,000 sorted insertions
  ...

Test: 80% search, 10% insert, 10% delete
  ...

Conclusion:
  AVL performs better for: read-heavy workloads
  Red-Black performs better for: write-heavy workloads
```

### Bonus Points: +10

---

## 📊 Bonus Point System

| Challenges Completed | Total Bonus |
|---------------------|-------------|
| 1 | +10 points |
| 2 | +20 points |
| 3 | +30 points |
| 4 | +40 points |
| All 5 | +50 points + "AVL Master" badge 🏆 |

---

## 📝 Submission Guidelines

1. Create separate files for each challenge:
   - `challenge1_parent.c`
   - `challenge2_serialise.c`
   - `challenge3_orderstats.c`
   - `challenge4_mergesplit.c`
   - `challenge5_benchmark.c`

2. Include a `CHALLENGES.md` file explaining:
   - Which challenges you attempted
   - Your approach for each
   - Any known limitations

3. All code must compile without warnings

4. Include test cases demonstrating functionality

---

## 💡 Hints

### Challenge 1 (Parent Pointers)
- The tricky part is updating parents during rotations
- Draw diagrams showing before/after pointer states
- Test with single rotations before double rotations

### Challenge 2 (Serialisation)
- Preorder traversal preserves structure when deserialising
- Be careful with endianness if targeting multiple platforms
- Consider using a text format first, then optimise to binary

### Challenge 3 (Order Statistics)
- The size of a node = 1 + size(left) + size(right)
- For select(k): compare k with size(left)+1
- For rank: sum sizes of all left subtrees along the search path

### Challenge 4 (Merge/Split)
- Split is recursive: split left or right based on key
- Merge uses the "join" operation with the smaller tree's root
- AVL join is O(|height difference|)

### Challenge 5 (Benchmark)
- Use `clock()` or `gettimeofday()` for timing
- Run each test multiple times and average
- Control for system load (close other applications)

---

## 📚 References

For those attempting the advanced challenges, these resources may be helpful:

1. **Order Statistics Trees**: CLRS Chapter 14.1
2. **Red-Black Trees**: CLRS Chapter 13
3. **Tree Merge/Split**: "Purely Functional Data Structures" by Okasaki
4. **Benchmarking Methodology**: "The Art of Computer Programming" Vol 3

---

*"The expert in anything was once a beginner." — Helen Hayes*

*Good luck with the challenges! 🚀*

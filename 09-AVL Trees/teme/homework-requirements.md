# Week 09 Homework: AVL Trees

## 1. Administrative parameters

- **Submission deadline**: end of Week 10 (local course time)
- **Maximum score**: 100 points (10% of the final module grade)
- **Implementation language**: C (ISO C11)
- **Toolchain**: GCC or Clang with warnings enabled (`-Wall -Wextra -std=c11`)
- **Runtime requirements**: no crashes on valid input and no memory leaks on representative test suites

The assessment is designed to reward correctness, robustness, clarity and disciplined engineering practice. Programmes that compile with warnings are considered defect-prone and will be penalised.

## 2. Learning objectives

On completion of the homework you should be able to:

1. implement a height-balanced binary search tree that maintains both ordering and balance invariants under insertion and deletion
2. reason precisely about algorithmic complexity, especially the relationship between height bounds and operation cost
3. design deterministic testable command-line programmes that separate core data structure logic from input parsing and output formatting
4. apply memory safety discipline in C, including ownership, allocation, deallocation and failure handling

## 3. Homework 1: AVL dictionary with string keys (50 points)

### 3.1 Problem statement

Implement a dictionary (key–value store) backed by an AVL tree, where:

- keys are null-terminated strings
- values are integers
- the ordering relation is lexicographic order induced by `strcmp`

The data structure shall support insertion, lookup and deletion in `O(log n)` worst-case time measured in the number of comparisons, where `n` is the number of stored key–value pairs.

### 3.2 Functional specification

#### 3.2.1 Node representation

Each node stores:

- an owning pointer to a dynamically allocated copy of the key
- the integer value
- the node height under the same convention used in the weekly exercises (empty tree height `-1`)
- two child pointers

```c
typedef struct DictNode {
    char *key;                 /* owning pointer */
    int value;
    int height;
    struct DictNode *left;
    struct DictNode *right;
} DictNode;
```

#### 3.2.2 Public interface

Your submission must implement at least the following functions:

```c
DictNode *dict_insert(DictNode *root, const char *key, int value);
int       dict_get(const DictNode *root, const char *key);
DictNode *dict_delete(DictNode *root, const char *key);
void      dict_destroy(DictNode *root);
void      dict_print_inorder(const DictNode *root);
```

**Semantics**:

- `dict_insert` inserts the pair `(key, value)`.
  - If `key` is absent, allocate a new node with a deep copy of `key`.
  - If `key` already exists, update the stored value and do not allocate a new key.
  - Rebalancing must be applied on the search path back to the root.
- `dict_get` returns the value associated with `key` or `-1` if absent.
- `dict_delete` removes `key` if present and returns the (possibly new) root.
  - The key string must be freed when its node is removed.
  - The tree must remain AVL-balanced.
- `dict_destroy` frees the entire tree including all key strings.
- `dict_print_inorder` prints keys in sorted order and may include values.

### 3.3 Algorithmic requirements

You are expected to reuse the AVL logic from `src/exercise1.c` and `src/exercise2.c` but adapt comparisons to `strcmp`. The rotation patterns are identical. Only the ordering primitive changes.

#### 3.3.1 Pseudocode: lexicographic insertion

```
function DICT_INSERT(node, key, value):
    if node = NULL:
        return NEW_NODE(strdup(key), value)

    cmp ← strcmp(key, node.key)

    if cmp < 0:
        node.left  ← DICT_INSERT(node.left,  key, value)
    else if cmp > 0:
        node.right ← DICT_INSERT(node.right, key, value)
    else:
        node.value ← value
        return node

    UPDATE_HEIGHT(node)
    return REBALANCE(node)
```

The only subtlety is ownership: if you call `strdup` eagerly then discover an existing key you will leak memory. Allocate the copy only when you actually create a new node.

#### 3.3.2 Pseudocode: deletion with two children

```
function DICT_DELETE(node, key):
    if node = NULL:
        return NULL

    cmp ← strcmp(key, node.key)

    if cmp < 0:
        node.left  ← DICT_DELETE(node.left,  key)
    else if cmp > 0:
        node.right ← DICT_DELETE(node.right, key)
    else:
        if node.left = NULL or node.right = NULL:
            child ← (node.left ≠ NULL) ? node.left : node.right
            FREE(node.key)
            FREE(node)
            return child
        else:
            succ ← MIN_NODE(node.right)
            SWAP(node.key,   succ.key)
            SWAP(node.value, succ.value)
            node.right ← DICT_DELETE(node.right, key)   // key now stored in succ

    UPDATE_HEIGHT(node)
    return REBALANCE(node)
```

The swap-based strategy avoids deep-copying strings during deletion but requires careful pointer ownership: after swapping, each key pointer is still freed exactly once.

### 3.4 Assessment criteria (Homework 1)

| Criterion | Points | Typical evidence |
|---|---:|---|
| Correct AVL invariants under insertion | 15 | sorted inorder output and successful validation on adversarial sequences |
| Correct AVL invariants under deletion | 15 | correct handling of 0-child, 1-child and 2-child cases with rebalancing |
| Correct string ownership and memory safety | 10 | Valgrind or sanitiser reports are clean on representative tests |
| Clean API design and code quality | 10 | decomposition, comments, consistent conventions, no undefined behaviour |

## 4. Homework 2: command-driven AVL visualiser (50 points)

### 4.1 Problem statement

Implement a command-line programme that reads a script of operations from a file, applies them to an AVL tree and prints a human-readable structural depiction of the tree after each modifying operation.

The aim is to consolidate understanding of how rotations localise structural repair and how balance factors evolve.

### 4.2 Input language

The input file is a sequence of lines. Each non-empty line begins with a command token:

- `INSERT <int>`
- `DELETE <int>`
- `SEARCH <int>`
- `PRINT`
- `CLEAR`

Whitespace is insignificant except as a separator. Malformed lines must not crash the programme. The recommended behaviour is to report an error on `stderr` and continue.

### 4.3 Output requirements

Your output must be deterministic. You may choose an ASCII tree layout of your own design but it must satisfy the following:

- each node must display its key and height
- each node must display its balance factor
- whenever a rotation occurs, print a line that identifies its type (LL, RR, LR or RL) and the pivot key

A minimal example is:

```
> INSERT 50
Tree:
---[50](h=0,bf=0)

> INSERT 30
Tree:
    /--[50](h=1,bf=1)
---[30](h=0,bf=0)
```

### 4.4 Statistics

Maintain counters for:

- total operations
- successful insertions and deletions
- key comparisons
- rotations (count single rotations and each constituent rotation in a double rotation)

At end of file print a summary block.

### 4.5 Suggested architecture

A maintainable design separates concerns:

1. **data structure module**: insertion, deletion, rotation, traversal
2. **parsing module**: line tokenisation and argument validation
3. **presentation module**: deterministic tree printing and statistics formatting

This separation reduces the risk that a formatting change accidentally changes the tree logic.

### 4.6 Assessment criteria (Homework 2)

| Criterion | Points | Typical evidence |
|---|---:|---|
| Correct command parsing and error handling | 10 | robustness to blank lines, malformed tokens and missing files |
| Correct execution semantics | 15 | search reports are correct and tree state matches operations |
| Clear and informative visual output | 15 | readable layout and correct balance factor display |
| Correct statistics with sensible definitions | 10 | counters match observable behaviour on supplied scripts |

## 5. Penalties and academic integrity

### 5.1 Technical penalties

- **Compiler warnings**: −10 points (aggregate) unless justified and documented
- **Memory leaks**: up to −20 points depending on severity and frequency
- **Crashes on valid input**: up to −30 points
- **Undefined behaviour**: substantial penalty even if output appears correct

### 5.2 Academic integrity

Plagiarism and unauthorised collaboration are incompatible with the learning goals. Submissions will be compared for structural similarity. Cases of misconduct are handled under the institutional regulations.

## 6. Submission format

Submit a single ZIP archive named:

`ATP_Week09_<YourName>.zip`

The archive must contain:

- `homework1_dictionary.c`
- `homework2_visualiser.c`
- a brief `README.txt` describing your design choices, key invariants and test strategy

Your code must compile from a clean directory with a single command line invocation. If you provide a Makefile then document the targets.

## 7. Practical guidance

1. Begin from the Week 09 exercises and refactor rather than rewriting from scratch.
2. Make invariants executable by implementing a validation routine and calling it after each operation in debug builds.
3. Use sanitiser builds during development:

```bash
gcc -std=c11 -Wall -Wextra -fsanitize=address,undefined -g -O1 -o hw1 homework1_dictionary.c
./hw1
```

4. Test edge cases explicitly: empty tree, single-node tree, deletion of missing keys, duplicate insertions and extremely long keys.

## References

| Reference (APA 7th) | DOI |
|---|---|
| Amani, M., Lai, K. A., & Tarjan, R. E. (2016). *Amortized rotation cost in AVL trees*. *Information Processing Letters, 116*(5), 327–330. | https://doi.org/10.1016/j.ipl.2015.12.009 |
| Bayer, R. (1972). Symmetric binary B-Trees: Data structure and maintenance algorithms. *Acta Informatica, 1*, 290–306. | https://doi.org/10.1007/BF00289509 |
| Karlton, P. L., Fuller, S. H., Scroggs, R. E., & Kaehler, E. B. (1976). Performance of height-balanced trees. *Communications of the ACM, 19*(1), 23–28. | https://doi.org/10.1145/359970.359989 |

# Week 09 Homework: AVL Trees

## 📋 General Information

- **Deadline:** End of Week 10
- **Points:** 100 (10% of final grade)
- **Language:** C (C11 standard)
- **Compiler:** GCC with `-Wall -Wextra -std=c11`
- **Memory:** No memory leaks (verified with Valgrind)

---

## 📝 Homework 1: AVL Tree Dictionary (50 points)

### Description

Implement a dictionary (key-value store) using an AVL tree where keys are strings and values are integers. This simulates a word frequency counter or a simple database index.

### Requirements

1. **Node Structure** (5 points)
   - Store a string key (dynamically allocated)
   - Store an integer value
   - Maintain AVL tree structure (height, children)

2. **String Comparison** (5 points)
   - Use `strcmp()` for key ordering
   - Handle case sensitivity correctly

3. **Insertion** (15 points)
   - Insert new key-value pairs
   - If key exists, update the value
   - Maintain AVL balance after insertion

4. **Search** (10 points)
   - Look up value by key
   - Return -1 if key not found
   - Efficient O(log n) lookup

5. **Deletion** (10 points)
   - Remove entries by key
   - Free string memory
   - Maintain AVL balance after deletion

6. **Memory Management** (5 points)
   - No memory leaks
   - Proper cleanup of all strings
   - Pass Valgrind check

### Interface

```c
typedef struct DictNode {
    char *key;
    int value;
    int height;
    struct DictNode *left;
    struct DictNode *right;
} DictNode;

DictNode *dict_insert(DictNode *root, const char *key, int value);
int dict_get(DictNode *root, const char *key);
DictNode *dict_delete(DictNode *root, const char *key);
void dict_destroy(DictNode *root);
void dict_print(DictNode *root);  /* Inorder traversal */
```

### Example Usage

```c
DictNode *dict = NULL;

dict = dict_insert(dict, "apple", 5);
dict = dict_insert(dict, "banana", 3);
dict = dict_insert(dict, "cherry", 8);

printf("%d\n", dict_get(dict, "banana"));  /* Output: 3 */
printf("%d\n", dict_get(dict, "grape"));   /* Output: -1 */

dict = dict_delete(dict, "banana");
printf("%d\n", dict_get(dict, "banana"));  /* Output: -1 */

dict_destroy(dict);
```

### File: `homework1_dictionary.c`

---

## 📝 Homework 2: AVL Tree Visualiser (50 points)

### Description

Create a programme that reads operations from a file, executes them on an AVL tree and produces a visual representation of the tree after each operation.

### Requirements

1. **File Parsing** (10 points)
   - Read operations from input file
   - Support: INSERT, DELETE, SEARCH, PRINT, CLEAR
   - Handle malformed input gracefully

2. **Operation Execution** (15 points)
   - Execute each operation correctly
   - Print operation result
   - Show tree state after modifications

3. **Visual Output** (15 points)
   - Display tree structure with ASCII art
   - Show balance factors at each node
   - Indicate rotations when they occur

4. **Statistics** (5 points)
   - Track number of rotations
   - Track number of comparisons
   - Display final statistics

5. **Error Handling** (5 points)
   - Handle file not found
   - Handle invalid operations
   - Handle memory allocation failures

### Input File Format

```
INSERT 50
INSERT 30
INSERT 70
PRINT
INSERT 20
INSERT 10
PRINT
DELETE 30
PRINT
SEARCH 50
SEARCH 99
CLEAR
```

### Expected Output Format

```
> INSERT 50
  Created node 50
  Tree:
  ---[50](h=0,bf=0)

> INSERT 30
  Created node 30
  Tree:
      /--[50](h=1,bf=1)
  ---[30](h=0,bf=0)

> INSERT 20
  LL Case detected at 50
  Performing RIGHT rotation on 50
  Tree:
      /--[50](h=0,bf=0)
  ---[30](h=1,bf=0)
      \--[20](h=0,bf=0)

...

Statistics:
  Total operations: 12
  Insertions: 5
  Deletions: 1
  Rotations: 2
  Comparisons: 18
```

### File: `homework2_visualiser.c`

---

## 📊 Evaluation Criteria

| Criterion | Points |
|-----------|--------|
| **Homework 1** | |
| Correct AVL implementation | 20 |
| String handling | 10 |
| Memory management | 10 |
| Code quality | 10 |
| **Homework 2** | |
| File parsing | 10 |
| Operation execution | 15 |
| Visual output | 15 |
| Statistics and error handling | 10 |
| **Total** | **100** |

### Penalties

| Issue | Penalty |
|-------|---------|
| Compiler warnings | -10 points |
| Memory leaks (Valgrind) | -20 points |
| Crashes on valid input | -30 points |
| Late submission (per day) | -10 points |
| Plagiarism | -100 points + disciplinary action |

---

## 📤 Submission

1. Create a ZIP archive named `ATP_Week09_[YourName].zip`
2. Include:
   - `homework1_dictionary.c`
   - `homework2_visualiser.c`
   - `README.txt` (brief description of your approach)
3. Submit via the course portal before the deadline
4. Ensure both files compile without warnings

---

## 💡 Tips

1. **Start with the provided exercise solutions** — they contain all the AVL logic you need

2. **Test with edge cases:**
   - Empty tree operations
   - Single node tree
   - Sorted input (worst case for BST)
   - Duplicate keys

3. **Use Valgrind early and often:**
   ```bash
   valgrind --leak-check=full --show-leak-kinds=all ./homework1
   ```

4. **For string keys**, remember to:
   - Allocate memory with `strdup()` or `malloc()`+`strcpy()`
   - Free strings when deleting nodes
   - Compare with `strcmp()`, not `==`

5. **Debug rotations** by printing the tree before and after each rotation

6. **Test systematically:**
   - First test with integer keys (like the exercises)
   - Then adapt to string keys
   - Finally add file I/O

---

## 📚 Resources

- Lecture slides: `slides/presentation-week09.html`
- Working example: `src/example1.c`
- Exercise solutions: `solution/` directory
- Online visualiser: https://visualgo.net/en/bst

---

*Good luck! Remember: a well-balanced tree is a happy tree. 🌳*

# Extended Challenges - Week 04

## 🚀 Advanced Challenges (Optional)

Each correctly solved challenge: **+10 bonus points**

These challenges are designed for students who want to deepen their understanding of linked lists and explore more advanced applications. They require creative problem-solving and a solid grasp of pointer manipulation.

---

## ⭐ Challenge 1: LRU Cache Implementation (Difficulty: Medium)

### Description

Implement a **Least Recently Used (LRU) Cache** using a combination of a doubly linked list and a hash table. An LRU cache evicts the least recently accessed item when the cache reaches capacity.

### Requirements

```c
typedef struct LRUCache {
    int capacity;
    int size;
    /* Your data structures here */
} LRUCache;

LRUCache* create_lru_cache(int capacity);
int lru_get(LRUCache *cache, int key);           /* Returns value or -1 */
void lru_put(LRUCache *cache, int key, int value); /* Insert or update */
void display_cache(LRUCache *cache);             /* Show cache state */
void free_lru_cache(LRUCache *cache);
```

### Operations

- `get(key)`: Return the value if key exists, move to front (most recent). Return -1 if not found.
- `put(key, value)`: Insert new key-value pair at front. If key exists, update and move to front. If cache is full, remove the least recently used item (tail).

### Expected Complexity

- Both `get` and `put` should be **O(1)** average time

### Example

```c
LRUCache *cache = create_lru_cache(3);

lru_put(cache, 1, 100);  /* Cache: [1:100] */
lru_put(cache, 2, 200);  /* Cache: [2:200, 1:100] */
lru_put(cache, 3, 300);  /* Cache: [3:300, 2:200, 1:100] */

lru_get(cache, 1);       /* Returns 100, Cache: [1:100, 3:300, 2:200] */

lru_put(cache, 4, 400);  /* Evicts 2, Cache: [4:400, 1:100, 3:300] */

lru_get(cache, 2);       /* Returns -1 (evicted) */
```

### Bonus Points: +10

---

## ⭐ Challenge 2: Skip List Implementation (Difficulty: Hard)

### Description

Implement a **Skip List**, a probabilistic data structure that allows O(log n) search, insertion and deletion on average. Skip lists use multiple levels of linked lists to achieve efficiency similar to balanced trees.

### Requirements

```c
#define MAX_LEVEL 16

typedef struct SkipNode {
    int key;
    int value;
    struct SkipNode **forward;  /* Array of forward pointers */
    int level;
} SkipNode;

typedef struct SkipList {
    SkipNode *header;
    int level;
    int size;
} SkipList;

SkipList* create_skip_list(void);
void skip_insert(SkipList *list, int key, int value);
int skip_search(SkipList *list, int key, int *value);  /* Returns 1 if found */
int skip_delete(SkipList *list, int key);
void display_skip_list(SkipList *list);  /* Show all levels */
void free_skip_list(SkipList *list);
```

### Level Generation

Use randomisation to determine the level of each new node:

```c
int random_level(void) {
    int level = 1;
    while ((rand() % 2) == 0 && level < MAX_LEVEL) {
        level++;
    }
    return level;
}
```

### Expected Output

```
Skip List (5 elements):
Level 3: HEAD --> 10 ------------> 50 --> NULL
Level 2: HEAD --> 10 --> 30 --> 50 --> NULL
Level 1: HEAD --> 10 --> 20 --> 30 --> 40 --> 50 --> NULL
```

### Bonus Points: +10

---

## ⭐ Challenge 3: Circular Buffer with Linked List (Difficulty: Medium)

### Description

Implement a **circular buffer** (ring buffer) using a circular linked list. This is commonly used in producer-consumer scenarios and audio/video streaming.

### Requirements

```c
typedef struct CircularBuffer {
    struct BufferNode *head;
    struct BufferNode *tail;
    struct BufferNode *read_ptr;
    struct BufferNode *write_ptr;
    int capacity;
    int count;
    int overwrite_mode;  /* 0 = block when full, 1 = overwrite oldest */
} CircularBuffer;

CircularBuffer* create_circular_buffer(int capacity, int overwrite_mode);
int buffer_write(CircularBuffer *buf, int data);  /* Returns 1 on success */
int buffer_read(CircularBuffer *buf, int *data);  /* Returns 1 on success */
int buffer_peek(CircularBuffer *buf, int *data);  /* Read without removing */
int is_buffer_full(CircularBuffer *buf);
int is_buffer_empty(CircularBuffer *buf);
void display_buffer(CircularBuffer *buf);
void free_circular_buffer(CircularBuffer *buf);
```

### Features

- Configurable capacity
- Two modes: blocking (reject writes when full) or overwriting (oldest data lost)
- Visual display showing buffer state with read/write positions

### Example

```c
CircularBuffer *buf = create_circular_buffer(5, 1);  /* Overwrite mode */

buffer_write(buf, 10);
buffer_write(buf, 20);
buffer_write(buf, 30);

display_buffer(buf);
/* [10] [20] [30] [ ] [ ]
    R         W           */

int value;
buffer_read(buf, &value);  /* value = 10 */

display_buffer(buf);
/* [ ] [20] [30] [ ] [ ]
        R    W           */
```

### Bonus Points: +10

---

## ⭐ Challenge 4: Self-Organising List (Difficulty: Medium)

### Description

Implement a **self-organising list** that improves search performance by reorganising elements based on access patterns. Implement three different reorganisation strategies.

### Requirements

```c
typedef enum {
    MOVE_TO_FRONT,   /* Move accessed element to front */
    TRANSPOSE,       /* Swap accessed element with predecessor */
    COUNT            /* Order by access frequency */
} ReorgStrategy;

typedef struct SOList {
    struct SONode *head;
    ReorgStrategy strategy;
    int comparisons;  /* Track total comparisons for analysis */
} SOList;

SOList* create_so_list(ReorgStrategy strategy);
void so_insert(SOList *list, int key, const char *data);
char* so_search(SOList *list, int key);  /* Returns data or NULL */
void so_delete(SOList *list, int key);
void display_so_list(SOList *list);
void print_statistics(SOList *list);
void free_so_list(SOList *list);
```

### Analysis

After a series of operations, compare the total comparisons needed for each strategy with a fixed access pattern (e.g., 80-20 rule: 20% of items accessed 80% of the time).

### Bonus Points: +10

---

## ⭐ Challenge 5: Multilevel Undo System (Difficulty: Medium-Hard)

### Description

Implement a text editor's **multilevel undo/redo system** using linked lists. Support unlimited undo levels and the ability to branch the history.

### Requirements

```c
typedef enum {
    OP_INSERT,    /* Insert text at position */
    OP_DELETE,    /* Delete n characters at position */
    OP_REPLACE    /* Replace text at position */
} OperationType;

typedef struct Operation {
    OperationType type;
    int position;
    char *old_text;     /* For undo */
    char *new_text;     /* For redo */
    struct Operation *prev;
    struct Operation *next;
} Operation;

typedef struct UndoSystem {
    Operation *current;  /* Current position in history */
    Operation *head;     /* Oldest operation */
    char *document;      /* Current document state */
    int doc_length;
} UndoSystem;

UndoSystem* create_undo_system(const char *initial_text);
void perform_insert(UndoSystem *sys, int pos, const char *text);
void perform_delete(UndoSystem *sys, int pos, int length);
void perform_replace(UndoSystem *sys, int pos, int length, const char *text);
int undo(UndoSystem *sys);           /* Returns 1 if successful */
int redo(UndoSystem *sys);           /* Returns 1 if successful */
void display_document(UndoSystem *sys);
void display_history(UndoSystem *sys);
void free_undo_system(UndoSystem *sys);
```

### Features

- Each operation stores enough information to undo and redo
- Redo history is cleared when a new operation is performed after undo
- Support for grouping operations (e.g., "Replace" = Delete + Insert)

### Example

```c
UndoSystem *editor = create_undo_system("Hello World");

perform_insert(editor, 5, ",");           /* "Hello, World" */
perform_replace(editor, 7, 5, "Universe"); /* "Hello, Universe" */

undo(editor);  /* Back to "Hello, World" */
undo(editor);  /* Back to "Hello World" */
redo(editor);  /* Forward to "Hello, World" */

perform_delete(editor, 5, 1);  /* "Hello World" - clears redo history */
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
| All 5 | +50 points + "Linked List Master" badge 🏆 |

---

## 📋 Submission Guidelines

1. Create a separate file for each challenge: `challenge1_lru.c`, `challenge2_skip.c`, etc.
2. Include comprehensive test cases demonstrating all features
3. Document your approach in comments
4. Ensure no memory leaks (verify with Valgrind)
5. Submit along with your regular homework

---

## 🎯 Learning Outcomes

By completing these challenges, you will:

- Master advanced pointer manipulation
- Understand real-world applications of linked lists
- Learn to combine data structures for optimal performance
- Develop debugging skills for complex linked structures
- Prepare for technical interviews at top companies

---

*These challenges are designed to push your understanding. Don't be discouraged if they take time—the learning process is valuable!*

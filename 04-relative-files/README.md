# Week 04: Linked Lists

## 🎯 Learning Objectives

By the end of this week, you will be able to:

1. **Remember** the fundamental structure of singly and doubly linked list nodes, including the role of data fields and pointer members in establishing sequential connectivity.

2. **Understand** the conceptual differences between contiguous (array-based) and non-contiguous (pointer-based) memory allocation strategies, articulating the trade-offs inherent in each approach.

3. **Apply** dynamic memory allocation functions (`malloc`, `calloc`, `free`) to construct, extend and dismantle linked list structures whilst maintaining pointer integrity throughout the node lifecycle.

4. **Analyse** the time and space complexity of fundamental linked list operations—insertion, deletion, traversal and search—comparing these metrics against equivalent array-based implementations.

5. **Evaluate** appropriate data structure selection criteria, determining when linked lists offer performance or flexibility advantages over arrays and vice versa.

6. **Create** robust, memory-safe linked list implementations featuring comprehensive error handling, proper resource deallocation and defensive programming practices.

---

## 📜 Historical Context

The linked list stands as one of the foundational pillars of computer science data structures, emerging during the formative years of high-level programming language development. The concept arose organically from the need to manage collections of data whose size could not be predetermined at compile time—a severe limitation of static arrays that plagued early computing systems.

The earliest implementations of linked lists appeared in the late 1950s, developed independently by several research groups working on list processing languages. The Information Processing Language (IPL), created by Allen Newell, Cliff Shaw and Herbert A. Simon at the RAND Corporation and Carnegie Mellon University between 1956 and 1958, incorporated linked lists as a fundamental construct. This language, designed for artificial intelligence research, pioneered many concepts that would become standard in subsequent decades.

LISP (List Processing), developed by John McCarthy at MIT in 1958, elevated the linked list to paradigmatic status. In LISP, the fundamental data structure is the "cons cell"—a pair of pointers that can reference either atomic values or other cons cells, enabling the construction of arbitrarily complex nested structures. This elegant recursive definition profoundly influenced programming language design and established linked lists as the canonical example of self-referential data structures.

### Key Figure: John McCarthy (1927–2011)

John McCarthy, widely regarded as one of the founding fathers of artificial intelligence, made seminal contributions to computer science that extended far beyond his invention of LISP. A mathematician by training, McCarthy received his PhD from Princeton University in 1951 and subsequently held positions at Stanford, Dartmouth and MIT. He coined the term "artificial intelligence" in 1955 whilst organising the famous Dartmouth Conference of 1956, considered the birth of AI as a discipline.

McCarthy's design of LISP introduced numerous innovations: automatic garbage collection, the conditional expression (`if-then-else`), recursion as a fundamental programming construct and the read-eval-print loop (REPL) for interactive development. His theoretical work on the lambda calculus provided the mathematical foundation for functional programming languages.

> *"The only way to rectify our reasonings is to make them as tangible as those of the Mathematicians, so that we can find our error at a glance, and when there are disputes among persons, we can simply say: Let us calculate, without further ado, to see who is right."*
> — Gottfried Wilhelm Leibniz (a quotation McCarthy frequently cited to illustrate the relationship between formal reasoning and computation)

---

## 📚 Theoretical Foundations

### 1. Node Architecture and Memory Layout

A linked list node comprises two conceptually distinct regions: the **data payload** and the **linkage mechanism**. The data payload stores the information of interest—integers, floating-point values, structures or pointers to more complex objects. The linkage mechanism consists of one or more pointers that establish relationships with other nodes in the sequence.

```
┌─────────────────────────────────────────────────────────────────────┐
│                    SINGLY LINKED LIST NODE                         │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│    ┌──────────────────────┬───────────────────────┐                │
│    │     DATA FIELD       │    NEXT POINTER       │                │
│    │     (payload)        │    (linkage)          │                │
│    │                      │                       │                │
│    │   [user data]        │   [address of next]   │                │
│    │                      │   [node or NULL]      │                │
│    └──────────────────────┴───────────────────────┘                │
│                                                                     │
│    Memory: sizeof(data) + sizeof(pointer)                          │
│    Typical: 4 bytes + 8 bytes = 12 bytes (64-bit system)          │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

The C language implementation leverages structures with self-referential pointers:

```c
/* Singly linked list node */
typedef struct Node {
    int data;               /* Data payload */
    struct Node *next;      /* Pointer to successor node */
} Node;

/* Doubly linked list node */
typedef struct DNode {
    int data;               /* Data payload */
    struct DNode *prev;     /* Pointer to predecessor node */
    struct DNode *next;     /* Pointer to successor node */
} DNode;
```

### 2. Traversal Patterns and Iterator Design

Linked list traversal follows the pointer chain from the head node until encountering the sentinel `NULL` value. This pattern, whilst conceptually simple, requires careful implementation to avoid dereferencing null pointers.

```
┌─────────────────────────────────────────────────────────────────────┐
│                    TRAVERSAL VISUALISATION                          │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  HEAD                                                               │
│   │                                                                 │
│   ▼                                                                 │
│  ┌────┬────┐    ┌────┬────┐    ┌────┬────┐    ┌────┬────┐          │
│  │ 10 │  ──┼───►│ 20 │  ──┼───►│ 30 │  ──┼───►│ 40 │NULL│          │
│  └────┴────┘    └────┴────┘    └────┴────┘    └────┴────┘          │
│                                                                     │
│  Step 1: current = head       (points to node containing 10)       │
│  Step 2: current = current->next  (points to node containing 20)   │
│  Step 3: current = current->next  (points to node containing 30)   │
│  Step 4: current = current->next  (points to node containing 40)   │
│  Step 5: current = current->next  (current == NULL, loop exits)    │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

The canonical traversal idiom in C:

```c
void traverse(Node *head) {
    Node *current = head;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
}
```

### 3. Complexity Analysis

The following table summarises the asymptotic time complexity of fundamental operations:

| Operation              | Array (unsorted) | Array (sorted) | Singly Linked | Doubly Linked |
|------------------------|------------------|----------------|---------------|---------------|
| Access by index        | O(1)             | O(1)           | O(n)          | O(n)          |
| Search                 | O(n)             | O(log n)       | O(n)          | O(n)          |
| Insert at beginning    | O(n)             | O(n)           | O(1)          | O(1)          |
| Insert at end          | O(1)*            | O(n)           | O(n)**        | O(1)***       |
| Insert at position     | O(n)             | O(n)           | O(n)          | O(n)          |
| Delete at beginning    | O(n)             | O(n)           | O(1)          | O(1)          |
| Delete at end          | O(1)             | O(1)           | O(n)          | O(1)          |
| Delete at position     | O(n)             | O(n)           | O(n)          | O(n)          |

*With amortised dynamic array resizing  
**O(1) if tail pointer maintained  
***With tail pointer

Space complexity considerations:

- **Arrays**: O(n) for n elements, contiguous allocation
- **Singly linked lists**: O(n) for n elements, plus O(n) pointer overhead
- **Doubly linked lists**: O(n) for n elements, plus O(2n) pointer overhead

---

## 🏭 Industrial Applications

### 1. Operating System Process Scheduling

Operating systems maintain process control blocks (PCBs) in linked list structures to facilitate efficient scheduling operations. The Linux kernel's `task_struct` employs doubly linked lists extensively:

```c
/* Simplified representation of Linux task list linkage */
struct task_struct {
    volatile long state;        /* Process state */
    void *stack;                /* Kernel stack pointer */
    
    /* Linked list of all processes */
    struct list_head tasks;
    
    /* Linked list of children */
    struct list_head children;
    struct list_head sibling;
    
    pid_t pid;
    /* ... additional fields ... */
};

/* Linux kernel list_head structure */
struct list_head {
    struct list_head *next, *prev;
};
```

### 2. Memory Allocator Free Lists

Dynamic memory allocators such as `malloc` implementations maintain free lists—linked lists of available memory blocks. The Doug Lea allocator (dlmalloc), upon which glibc's malloc is based, organises free chunks into bins:

```c
/* Simplified free chunk structure */
struct malloc_chunk {
    size_t prev_size;           /* Size of previous chunk (if free) */
    size_t size;                /* Size in bytes, including overhead */
    
    struct malloc_chunk *fd;    /* Forward pointer (next free chunk) */
    struct malloc_chunk *bk;    /* Backward pointer (prev free chunk) */
};
```

### 3. Browser History Navigation

Web browsers implement navigation history using doubly linked lists, enabling efficient forward and backward traversal:

```c
/* Browser history entry */
typedef struct HistoryEntry {
    char *url;
    char *title;
    time_t visit_time;
    struct HistoryEntry *prev;
    struct HistoryEntry *next;
} HistoryEntry;

typedef struct BrowserHistory {
    HistoryEntry *current;
    HistoryEntry *head;
    HistoryEntry *tail;
} BrowserHistory;
```

### 4. Undo/Redo Functionality in Text Editors

Text editors and document processors implement undo/redo stacks using linked lists of operations:

```c
typedef enum { INSERT, DELETE, REPLACE } OperationType;

typedef struct EditOperation {
    OperationType type;
    size_t position;
    char *old_text;
    char *new_text;
    struct EditOperation *prev;
    struct EditOperation *next;
} EditOperation;
```

### 5. Polynomial Arithmetic

Computer algebra systems represent sparse polynomials as linked lists of non-zero terms, enabling efficient arithmetic operations:

```c
typedef struct Term {
    double coefficient;
    int exponent;
    struct Term *next;
} Term;

typedef struct Polynomial {
    Term *head;
    int degree;
} Polynomial;
```

---

## 💻 Laboratory Exercises

### Exercise 1: Student Records Management System

Implement a singly linked list to manage a collection of student records with the following capabilities:

**Requirements:**
- Define a `Student` structure containing: `id` (integer), `name` (string, max 50 characters), `gpa` (float)
- Implement `create_student()` to allocate and initialise a new student node
- Implement `insert_sorted()` to maintain students in ascending order by ID
- Implement `delete_student()` to remove a student by ID
- Implement `find_student()` to search for a student by ID
- Implement `display_all()` to print all students
- Implement `free_list()` to deallocate all nodes
- Handle all edge cases: empty list, single element, insertion at head/tail

**Input format:** Commands from file (`data/students.txt`)
**Output format:** Formatted student records

### Exercise 2: Polynomial Calculator

Implement a polynomial representation using linked lists and support arithmetic operations:

**Requirements:**
- Define a `Term` structure containing: `coefficient` (double), `exponent` (integer), `next` (pointer)
- Implement `add_term()` to insert a term in descending order by exponent
- Implement `add_polynomials()` to compute the sum of two polynomials
- Implement `multiply_polynomials()` to compute the product of two polynomials
- Implement `evaluate_polynomial()` to compute the value at a given x
- Implement `display_polynomial()` to print in mathematical notation
- Implement `free_polynomial()` to deallocate all terms
- Combine like terms automatically

**Input format:** Polynomial coefficients from file (`data/polynomials.txt`)
**Output format:** Polynomial expressions and computed values

---

## 🔧 Compilation and Execution

```bash
# Build all targets
make

# Build specific files
make example1
make exercise1
make exercise2

# Run the demonstration example
make run

# Execute automated tests
make test

# Check for memory leaks with Valgrind
make valgrind

# Build and run solutions (instructor only)
make solutions

# Clean build artifacts
make clean

# Display help information
make help
```

**Compiler flags explained:**
- `-Wall`: Enable all common warnings
- `-Wextra`: Enable additional warnings
- `-std=c11`: Use C11 standard
- `-g`: Include debugging symbols for GDB/Valgrind

---

## 📁 Directory Structure

```
week-04-linked-lists/
├── README.md                           # This documentation file
├── Makefile                            # Build automation
│
├── slides/
│   ├── presentation-week04.html        # Main lecture (40 slides)
│   └── presentation-comparativ.html    # Language comparison (12 slides)
│
├── src/
│   ├── example1.c                      # Complete working demonstration
│   ├── exercise1.c                     # Student records (12 TODOs)
│   └── exercise2.c                     # Polynomial calculator (15 TODOs)
│
├── data/
│   ├── students.txt                    # Sample student data
│   └── polynomials.txt                 # Sample polynomial coefficients
│
├── tests/
│   ├── test1_input.txt                 # Test input for exercise 1
│   ├── test1_expected.txt              # Expected output for exercise 1
│   ├── test2_input.txt                 # Test input for exercise 2
│   └── test2_expected.txt              # Expected output for exercise 2
│
├── teme/
│   ├── homework-requirements.md        # Homework assignments (100 points)
│   └── homework-extended.md            # Bonus challenges (+50 points)
│
└── solution/
    ├── exercise1_sol.c                 # Solution for exercise 1
    ├── exercise2_sol.c                 # Solution for exercise 2
    ├── homework1_sol.c                 # Solution for homework 1
    └── homework2_sol.c                 # Solution for homework 2
```

---

## 📖 Recommended Reading

### Essential Resources

1. **Kernighan, B.W. & Ritchie, D.M.** (1988). *The C Programming Language* (2nd ed.), Chapter 6: Structures. Prentice Hall.

2. **Sedgewick, R.** (1998). *Algorithms in C* (3rd ed.), Part 1: Fundamentals, Chapters 3-4. Addison-Wesley.

3. **Cormen, T.H., Leiserson, C.E., Rivest, R.L. & Stein, C.** (2009). *Introduction to Algorithms* (3rd ed.), Chapter 10: Elementary Data Structures. MIT Press.

### Advanced Resources

4. **Knuth, D.E.** (1997). *The Art of Computer Programming, Volume 1: Fundamental Algorithms* (3rd ed.), Section 2.2: Linear Lists. Addison-Wesley.

5. **Weiss, M.A.** (2014). *Data Structures and Algorithm Analysis in C* (2nd ed.), Chapter 3: Lists, Stacks, and Queues. Pearson.

### Online Resources

6. **GeeksforGeeks**: Linked List Data Structure  
   https://www.geeksforgeeks.org/data-structures/linked-list/

7. **Visualgo**: Linked List Visualisation  
   https://visualgo.net/en/list

8. **Linux Kernel Documentation**: Circular doubly linked list  
   https://www.kernel.org/doc/html/latest/core-api/kernel-api.html

---

## ✅ Self-Assessment Checklist

Before proceeding to Week 5, ensure you can confidently:

- [ ] Define and explain the structure of singly and doubly linked list nodes
- [ ] Articulate why `struct Node *next` (not `struct Node next`) is required
- [ ] Implement node creation with proper `malloc` and null checking
- [ ] Traverse a linked list without memory errors or infinite loops
- [ ] Insert nodes at the beginning, end and arbitrary positions
- [ ] Delete nodes whilst maintaining list integrity and freeing memory
- [ ] Search for elements and return appropriate indicators
- [ ] Compare linked list operations complexity with array equivalents
- [ ] Use Valgrind to verify absence of memory leaks
- [ ] Debug linked list code using GDB with pointer inspection

---

## 💼 Interview Preparation

### Common Interview Questions on Linked Lists

1. **Reversal**: How would you reverse a singly linked list in-place? What is the time and space complexity?

2. **Cycle Detection**: Given a linked list, how can you determine if it contains a cycle? Explain Floyd's tortoise and hare algorithm.

3. **Middle Element**: Find the middle element of a linked list in a single pass. What technique would you use?

4. **Merge Sorted Lists**: Given two sorted linked lists, merge them into a single sorted list. Can you do this without creating new nodes?

5. **Nth from End**: Find the nth node from the end of a linked list in a single pass. What is the two-pointer technique?

### Practical Coding Challenge

Implement a function to detect and return the starting node of a cycle in a linked list:

```c
Node* detect_cycle_start(Node *head);
/* Returns pointer to cycle start node, or NULL if no cycle exists */
```

---

## 🔗 Next Week Preview

**Week 05: Stacks**

Building upon our understanding of linked lists, we shall explore the stack abstract data type—a Last-In-First-Out (LIFO) structure fundamental to function call management, expression evaluation and backtracking algorithms.

Topics to be covered:
- Stack ADT specification and invariants
- Array-based and linked list-based implementations
- Applications: balanced parentheses, postfix evaluation, function calls
- The system call stack and stack frames
- Stack overflow and underflow conditions

---

*© 2025 Algorithms and Programming Techniques Course | Academy of Economic Studies*

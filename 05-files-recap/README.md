# Week 05: Stacks — The LIFO Data Structure

## 🎯 Learning Objectives

Upon successful completion of this laboratory, students will be able to:

1. **Remember** the fundamental properties of stacks and the LIFO (Last-In-First-Out) principle that governs their behaviour
2. **Understand** the distinction between array-based and linked-list-based stack implementations, including their respective trade-offs
3. **Apply** stack operations (push, pop, peek, isEmpty) to solve computational problems such as expression evaluation and bracket matching
4. **Analyse** the time and space complexity of stack operations across different implementation strategies
5. **Evaluate** when a stack is the appropriate data structure for a given problem versus alternatives such as queues or deques
6. **Create** custom stack implementations with extended functionality, including dynamic resizing and generic element storage

---

## 📜 Historical Context

The stack data structure emerged from the earliest days of computing, deeply intertwined with the development of programming language theory and computer architecture. The concept crystallised during the 1950s when computer scientists grappled with the challenges of expression evaluation, subroutine management and memory allocation.

The term "stack" derives from the physical metaphor of a stack of plates or books—one may only add to or remove from the top. This seemingly simple constraint proves extraordinarily powerful, forming the backbone of function call management, parsing algorithms and undo mechanisms across virtually all modern software systems.

Friedrich Ludwig Bauer and Klaus Samelson are credited with formalising the stack concept in 1955 during their work on the sequencing of operations in computing machines. Their insights led directly to the development of stack-based architectures and the ubiquitous call stack that manages function invocation in contemporary programming languages.

### Key Figure: Friedrich Ludwig Bauer (1924–2015)

Friedrich Bauer was a pioneering German computer scientist whose contributions span compiler construction, programming language design and software engineering methodology. As a professor at the Technical University of Munich, he helped establish computer science as an academic discipline in Germany.

Bauer's work on the stack concept earned him the IEEE Computer Pioneer Award in 1988. He is equally renowned for coining the term "software engineering" and for his development of the ALGOL programming language alongside an international committee of computing luminaries.

> *"Software engineering is the part of computer science which is too difficult for the computer scientist."*
> — Friedrich Ludwig Bauer

---

## 📚 Theoretical Foundations

### 1. The LIFO Principle

A stack operates according to the Last-In-First-Out (LIFO) principle: the most recently added element is the first to be removed. This constraint defines the stack's behaviour and distinguishes it from other linear data structures.

```
    PUSH 10     PUSH 20     PUSH 30      POP        POP
    ┌─────┐     ┌─────┐     ┌─────┐     ┌─────┐     ┌─────┐
    │     │     │     │     │  30 │ ←   │     │     │     │
    │     │     │  20 │ ←   │  20 │     │  20 │ ←   │     │
    │  10 │ ←   │  10 │     │  10 │     │  10 │     │  10 │ ←
    └─────┘     └─────┘     └─────┘     └─────┘     └─────┘
     top=0       top=1       top=2       top=1       top=0
```

The arrow (←) indicates the current top of the stack. Each push operation increments the top index; each pop operation decrements it.

### 2. Core Operations and Their Semantics

| Operation | Description | Time Complexity | Precondition |
|-----------|-------------|-----------------|--------------|
| `push(x)` | Add element x to the top of the stack | O(1) amortised | Stack not full (bounded) |
| `pop()` | Remove and return the top element | O(1) | Stack not empty |
| `peek()` / `top()` | Return the top element without removal | O(1) | Stack not empty |
| `isEmpty()` | Return true if stack contains no elements | O(1) | None |
| `isFull()` | Return true if stack is at capacity | O(1) | Bounded stack only |
| `size()` | Return the number of elements | O(1) | None |

### 3. Array-Based vs Linked-List Implementation

**Array-Based Implementation:**

```c
typedef struct {
    int *data;       /* Dynamic array of elements */
    int top;         /* Index of top element (-1 if empty) */
    int capacity;    /* Maximum number of elements */
} ArrayStack;
```

Advantages:
- Cache-friendly memory layout (contiguous allocation)
- O(1) indexed access to elements (useful for debugging)
- No pointer overhead per element

Disadvantages:
- Fixed capacity requires resizing (amortised O(1), worst-case O(n))
- May waste memory if capacity exceeds actual usage

**Linked-List Implementation:**

```c
typedef struct Node {
    int data;
    struct Node *next;
} Node;

typedef struct {
    Node *top;       /* Pointer to top node (NULL if empty) */
    int size;        /* Current number of elements */
} LinkedStack;
```

Advantages:
- Truly dynamic size with no resizing overhead
- Memory usage proportional to actual elements stored

Disadvantages:
- Pointer overhead per element (typically 8 bytes on 64-bit systems)
- Poor cache locality due to non-contiguous allocation
- Each push/pop requires memory allocation/deallocation

**Complexity Comparison:**

| Operation | Array Stack | Linked Stack |
|-----------|-------------|--------------|
| Push | O(1) amortised | O(1) |
| Pop | O(1) | O(1) |
| Peek | O(1) | O(1) |
| Space | O(capacity) | O(n) + pointer overhead |

---

## 🏭 Industrial Applications

### 1. Function Call Stack (Operating Systems)

Every modern programming language uses a stack to manage function calls. When a function is invoked, its return address, parameters and local variables are pushed onto the call stack. Upon return, this frame is popped.

```c
/* Conceptual view of call stack during recursion */
void factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);  /* Stack frame pushed here */
}

/* Call stack for factorial(4):
   ┌──────────────────┐
   │ factorial(1)     │  ← Top (current execution)
   │ factorial(2)     │
   │ factorial(3)     │
   │ factorial(4)     │
   │ main()           │  ← Bottom
   └──────────────────┘
*/
```

### 2. Expression Evaluation (Compilers and Calculators)

The shunting-yard algorithm (Edsger Dijkstra, 1961) uses two stacks to convert infix expressions to postfix notation and evaluate them:

```c
/* Evaluating postfix expression: 3 4 + 2 * */
/* Stack trace:
   Token  Action          Stack
   3      push(3)         [3]
   4      push(4)         [3, 4]
   +      pop, pop, push  [7]       (3 + 4 = 7)
   2      push(2)         [7, 2]
   *      pop, pop, push  [14]      (7 * 2 = 14)
   Result: 14
*/
```

### 3. Undo/Redo Functionality (GUI Applications)

Applications such as text editors, graphics software and IDEs implement undo functionality using a stack of operations:

```c
/* GTK-style command pattern with undo stack */
typedef struct {
    void (*execute)(void *data);
    void (*undo)(void *data);
    void *data;
} Command;

typedef struct {
    Command **commands;
    int top;
    int capacity;
} UndoStack;
```

### 4. Bracket Matching (Code Editors and Linters)

Syntax highlighters and compilers use stacks to verify that brackets match correctly:

```c
/* Matching: {[()]} → Valid
   Stack trace:
   Char  Action          Stack
   {     push('{')       ['{']
   [     push('[')       ['{', '[']
   (     push('(')       ['{', '[', '(']
   )     pop, match '('  ['{', '[']
   ]     pop, match '['  ['{']
   }     pop, match '{'  []         ← Empty = Balanced!
*/
```

### 5. Browser History Navigation (Web Browsers)

The back and forward buttons in web browsers operate using two stacks:

```c
typedef struct {
    Stack *back_stack;     /* Pages visited before current */
    Stack *forward_stack;  /* Pages visited after pressing Back */
    char *current_url;
} BrowserHistory;

/* Navigate to new page: push current to back_stack, clear forward_stack */
/* Back button: push current to forward_stack, pop from back_stack */
/* Forward button: push current to back_stack, pop from forward_stack */
```

---

## 💻 Laboratory Exercises

### Exercise 1: Array-Based Stack with Dynamic Resizing

Implement a complete array-based stack with the following features:

**Requirements:**
- Define an `ArrayStack` structure with dynamic capacity
- Implement `stack_create(initial_capacity)` to allocate the stack
- Implement `stack_push(stack, value)` with automatic resizing (double capacity when full)
- Implement `stack_pop(stack)` returning the removed value
- Implement `stack_peek(stack)` to view the top element
- Implement `stack_is_empty(stack)` and `stack_size(stack)`
- Implement `stack_destroy(stack)` to free all memory
- All operations must handle error conditions gracefully

**Test Scenario:**
Push values 1 through 20, then pop and print all values in reverse order.

### Exercise 2: Balanced Brackets Validator

Using a linked-list-based stack, implement a programme that validates whether an expression has balanced brackets.

**Requirements:**
- Support three bracket types: `()`, `[]`, `{}`
- Define a `LinkedStack` structure using dynamically allocated nodes
- Implement push, pop and isEmpty operations for the linked stack
- Read expressions from standard input (one per line)
- Output "VALID" or "INVALID: [reason]" for each expression
- Handle nested and interleaved brackets correctly
- Free all memory after processing each expression

**Test Cases:**
```
Input: {[()]}
Output: VALID

Input: {[(])}
Output: INVALID: Mismatched bracket at position 4

Input: {[()]
Output: INVALID: Unclosed bracket '{'
```

---

## 🔧 Compilation and Execution

```bash
# Build all targets
make

# Build and run the complete example
make run

# Build and run a specific exercise
make exercise1
./exercise1

# Run automated tests
make test

# Check for memory leaks
make valgrind

# Clean build artefacts
make clean

# Display help
make help
```

**Compiler Flags Explained:**

| Flag | Purpose |
|------|---------|
| `-Wall` | Enable all common warnings |
| `-Wextra` | Enable extra warnings |
| `-std=c11` | Use C11 standard |
| `-g` | Include debugging symbols |
| `-fsanitize=address` | Enable AddressSanitizer (optional) |

---

## 📁 Directory Structure

```
week-05-stacks/
├── README.md                           # This documentation file
├── Makefile                            # Build automation
│
├── slides/
│   ├── presentation-week05.html        # Main lecture (38 slides)
│   └── presentation-comparativ.html    # Pseudocode/C/Python comparison
│
├── src/
│   ├── example1.c                      # Complete stack demonstration
│   ├── exercise1.c                     # Array stack exercise (10 TODOs)
│   └── exercise2.c                     # Bracket validator (12 TODOs)
│
├── data/
│   ├── expressions.txt                 # Sample bracket expressions
│   └── postfix_expressions.txt         # Postfix notation test data
│
├── tests/
│   ├── test1_input.txt                 # Test input for exercise 1
│   ├── test1_expected.txt              # Expected output for exercise 1
│   ├── test2_input.txt                 # Test input for exercise 2
│   └── test2_expected.txt              # Expected output for exercise 2
│
├── teme/
│   ├── homework-requirements.md        # Two homework assignments
│   └── homework-extended.md            # Five bonus challenges
│
└── solution/
    ├── exercise1_sol.c                 # Solution for exercise 1
    ├── exercise2_sol.c                 # Solution for exercise 2
    ├── homework1_sol.c                 # Solution for homework 1
    └── homework2_sol.c                 # Solution for homework 2
```

---

## 📖 Recommended Reading

### Essential
- Sedgewick, R. & Wayne, K. (2011). *Algorithms*, 4th ed., Chapter 1.3: Bags, Queues, and Stacks
- Cormen, T. et al. (2009). *Introduction to Algorithms*, 3rd ed., Chapter 10.1: Stacks and Queues
- Kernighan, B. & Ritchie, D. (1988). *The C Programming Language*, 2nd ed., Chapter 5: Pointers and Arrays

### Advanced
- Knuth, D. (1997). *The Art of Computer Programming*, Vol. 1, Section 2.2.1: Stacks, Queues, and Deques
- Okasaki, C. (1998). *Purely Functional Data Structures*, Chapter 2: Persistence

### Online Resources
- [Visualgo Stack Visualisation](https://visualgo.net/en/stack)
- [GeeksforGeeks: Stack Data Structure](https://www.geeksforgeeks.org/stack-data-structure/)
- [C Standard Library Reference](https://en.cppreference.com/w/c)

---

## ✅ Self-Assessment Checklist

Before submitting your work, verify that you can:

- [ ] Explain the LIFO principle and contrast it with FIFO (queues)
- [ ] Implement a stack using an array with proper bounds checking
- [ ] Implement a stack using a linked list with proper memory management
- [ ] Analyse the time complexity of push and pop for both implementations
- [ ] Identify when dynamic resizing occurs and its amortised cost
- [ ] Use a stack to evaluate postfix expressions step by step
- [ ] Apply stacks to solve the balanced brackets problem
- [ ] Debug stack-related issues using GDB and Valgrind
- [ ] Recognise stack overflow and underflow conditions
- [ ] Describe real-world applications of stacks in system software

---

## 💼 Interview Preparation

Common technical interview questions on stacks:

1. **How would you implement a stack that supports getMin() in O(1) time?**
   *Hint: Use an auxiliary stack to track minimum values.*

2. **Design a stack that supports push, pop and getMax in O(1) time.**
   *Hint: Similar to min-stack, maintain a parallel max-stack.*

3. **How can you implement a queue using two stacks?**
   *Hint: Use one stack for enqueue operations and another for dequeue.*

4. **Given a string of brackets, find the longest valid substring.**
   *Hint: Use a stack to track indices of unmatched brackets.*

5. **Evaluate an arithmetic expression given in infix notation.**
   *Hint: Convert to postfix using the shunting-yard algorithm, then evaluate.*

---

## 🔗 Next Week Preview

**Week 06: Queues and Circular Buffers**

Next week, we explore the queue data structure, which operates on the First-In-First-Out (FIFO) principle. We shall implement both linear and circular queues, examine the priority queue abstraction and investigate real-world applications in scheduling, breadth-first search and producer-consumer systems.

---

*Algorithms and Programming Techniques — Faculty of Cybernetics, Statistics and Economic Informatics*
*Academic Year 2024–2025*

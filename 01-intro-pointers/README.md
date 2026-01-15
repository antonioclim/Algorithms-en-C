# Week 01: Pointers to Functions and Callbacks in C

## 🎯 Learning Objectives

By the end of this laboratory session, students will be able to:

1. **Remember** the syntax for declaring function pointers in C
2. **Understand** why function pointers enable generic programming
3. **Apply** `qsort()` and `bsearch()` with custom comparator functions
4. **Analyze** the trade-offs between different comparator implementations
5. **Evaluate** when to use dispatch tables vs switch statements
6. **Create** callback-based solutions for real-world problems

---

## 📜 Historical Context

### The Birth of C (1969-1973)

Dennis Ritchie developed C at Bell Labs alongside the Unix operating system. His genius was creating a language that mapped directly to machine operations while remaining readable and portable. Function pointers in C directly mirror assembly language's **indirect jump instructions**—the syntax `(*fptr)()` compiles to loading an address into a register and executing an indirect call.

### Key Figure: Dennis Ritchie (1941-2011)

- Creator of C programming language
- Co-creator of Unix (with Ken Thompson)
- Turing Award recipient (1983)
- Author of "The C Programming Language" (K&R)

> *"C is not a 'very high level' language, nor a 'big' one, and is not specialized to any particular area of application. But its absence of restrictions and its generality make it more convenient and effective for many tasks than supposedly more powerful languages."*
> — K&R, Preface to First Edition

---

## 📚 Theoretical Foundations

### 1. What is a Function Pointer?

A **function pointer** is a variable that stores the memory address of a function. Just like data pointers point to data in memory, function pointers point to executable code.

```
┌─────────────────┐     ┌─────────────────┐
│   Code Segment  │     │   Stack/Data    │
├─────────────────┤     ├─────────────────┤
│ 0x4005f0: add() │ ←── │ fptr: 0x4005f0  │
│ 0x400610: sub() │     │                 │
│ 0x400630: mul() │     │                 │
└─────────────────┘     └─────────────────┘
```

### 2. Declaration Syntax

```c
// Basic syntax
return_type (*pointer_name)(parameter_types);

// Examples
int (*binary_op)(int, int);           // Takes 2 ints, returns int
void (*callback)(void);               // No params, no return
int (*comparator)(const void*, const void*);  // qsort-style
```

**Critical:** The parentheses around `*pointer_name` are mandatory!

| Declaration | Meaning |
|-------------|---------|
| `int (*fp)(int)` | Pointer to function taking int, returning int |
| `int *fp(int)` | Function taking int, returning pointer to int |

### 3. The Callback Pattern

A **callback** is a function passed as an argument to another function, to be "called back" later:

```
Your Code → Library Function → Callback (Your Code)
```

This enables:
- **Generic algorithms** (sort any data type)
- **Event handling** (GUI, network)
- **Inversion of Control** (frameworks)

### 4. qsort() and bsearch()

From `<stdlib.h>`:

```c
void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *));

void *bsearch(const void *key, const void *base, size_t nmemb,
              size_t size, int (*compar)(const void *, const void *));
```

**Comparator Contract (POSIX):**
- Return `< 0` if first element comes before second
- Return `0` if elements are equal
- Return `> 0` if first element comes after second

**Complexity:**
| Function | Time Complexity | Space Complexity |
|----------|-----------------|------------------|
| qsort() | O(n log n) average, O(n²) worst | O(log n) |
| bsearch() | O(log n) | O(1) |

### 5. Safe Comparator Pattern

**Warning:** Simple subtraction can overflow with large integers!

```c
// UNSAFE for large values
int cmp_unsafe(const void *a, const void *b) {
    return *(int*)a - *(int*)b;  // Can overflow!
}

// SAFE pattern
int cmp_safe(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}
```

---

## 🏭 Industrial Applications

### GTK/GLib (Linux GUI Toolkit)
```c
g_signal_connect(button, "clicked", G_CALLBACK(on_button_click), data);
```

### Win32 API (Windows)
```c
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
```

### Unix Signal Handling
```c
signal(SIGINT, handle_interrupt);
```

### Node.js / libuv
The entire Node.js event loop is implemented in C using function pointer callbacks for asynchronous I/O operations.

---

## 💻 Laboratory Exercises

### Exercise 1: Calculator with Dispatch Table
Implement a calculator using an array of function pointers.

**Requirements:**
- Support operations: +, -, *, /, %
- Handle division by zero gracefully
- Use a dispatch table (not switch statement)

**File:** `src/exercise1.c`

### Exercise 2: Student Database with Sorting
Create a student management system using qsort() and bsearch().

**Requirements:**
- Define a Student structure (id, name, grade)
- Implement comparators for sorting by each field
- Search for students by name using bsearch()
- Display top N students by grade

**File:** `src/exercise2.c`

---

## 🔧 Build & Run

```bash
# Compile all sources
make

# Run all executables
make run

# Run automated tests
make test

# Check for memory leaks
make valgrind

# Clean build artifacts
make clean

# Show help
make help
```

---

## 📁 Directory Structure

```
week-01-pointers-callbacks/
├── README.md                 # This file
├── Makefile                  # Build automation
├── slides/
│   ├── presentation-week01.html      # Main lecture slides
│   └── presentation-comparativ.html  # C vs Python comparison
├── src/
│   ├── example1.c            # Complete working example
│   ├── exercise1.c           # Calculator exercise (TODO)
│   └── exercise2.c           # Student database (TODO)
├── data/
│   ├── students.txt          # Sample student data
│   └── numbers.txt           # Sample numbers for sorting
├── tests/
│   ├── test1_input.txt       # Test input for exercise 1
│   ├── test1_expected.txt    # Expected output for exercise 1
│   ├── test2_input.txt       # Test input for exercise 2
│   └── test2_expected.txt    # Expected output for exercise 2
├── teme/
│   ├── teme-cerinte.md       # Homework requirements
│   └── teme-extinse.md       # Extended challenges
└── solution/
    ├── exercise1_sol.c       # Solution for exercise 1
    ├── exercise2_sol.c       # Solution for exercise 2
    ├── tema1_sol.c           # Homework 1 solution
    └── tema2_sol.c           # Homework 2 solution
```

---

## 📖 Recommended Reading

### Essential
- **K&R** Chapter 5.11: Pointers to Functions
- **man qsort** - System documentation
- **man bsearch** - Binary search documentation

### Advanced
- **Modern C** by Jens Gustedt - Chapter 11
- **CLRS** - Introduction to Algorithms (sorting theory)
- **Expert C Programming** by Peter van der Linden

### Online Resources
- [cppreference.com](https://en.cppreference.com/w/c) - C standard library
- [cdecl.org](https://cdecl.org/) - Decode complex C declarations
- [godbolt.org](https://godbolt.org/) - Compiler Explorer

---

## ✅ Self-Assessment Checklist

After completing this lab, verify you can:

- [ ] Declare a function pointer with correct syntax
- [ ] Assign a function to a pointer and call it
- [ ] Explain the difference between `int (*f)(int)` and `int *f(int)`
- [ ] Write a comparator function for qsort()
- [ ] Use bsearch() on a sorted array
- [ ] Implement a dispatch table with function pointer array
- [ ] Explain why callbacks enable generic programming
- [ ] Pass all automated tests: `make test`

---

## 💼 Interview Preparation

Common questions about function pointers:

1. **Declare** a pointer to a function taking two ints and returning int
2. **Implement** a dispatch table for a calculator
3. **Explain** `int (*fp)(int)` vs `int *fp(int)`
4. **Write** a comparator for sorting structures by multiple fields
5. **Debug** why bsearch() returns NULL (hint: is array sorted?)

---

## 🔗 Next Week Preview

**Week 02: Text Files in C**
- File I/O with stdio.h
- Buffering strategies
- Text parsing techniques
- Error handling patterns

---

*ATP Course | ASE-CSIE | Computer Science Department*

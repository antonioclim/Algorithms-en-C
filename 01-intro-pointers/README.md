# Week 01: Function Pointers, Callbacks and Indirect Control Flow in ISO C (C11)

## Abstract

This teaching unit introduces function pointers as first-class control-flow values in ISO C and develops them into three practically consequential patterns: callbacks for library driven execution, comparator based generic algorithms (`qsort` and `bsearch`) and dispatch tables for constant-time operation selection. The accompanying source files are deliberately small yet conceptually dense. They emphasise how a function pointer links two design layers: a static type signature that constrains call compatibility and a runtime code address that enables indirection, late binding and inversion of control.

The repository contains a complete worked example (`src/example1.c`) and two exercises (`src/exercise1.c`, `src/exercise2.c`) that are now implemented end-to-end so that the automated test harness can compile and validate behavioural properties. A separate `solution/` directory keeps instructor reference implementations for the laboratory exercises and for the two weekly coursework tasks.

## Learning outcomes

On completion students should be able to:

1. Formally derive the type of a function pointer from a declarator and identify common precedence traps.
2. Explain how indirect calls are represented at the machine level and why indirect calls are the substrate of callbacks.
3. Implement callback based APIs that respect the comparator and predicate contracts required by the C standard library.
4. Use `qsort` to sort arrays of primitive types and structures by multiple criteria while maintaining a safe, total ordering.
5. Use `bsearch` correctly by establishing and maintaining the sortedness precondition.
6. Design and implement dispatch tables as an alternative to long `switch` statements and analyse their algorithmic and microarchitectural trade-offs.
7. Construct a minimal test plan that covers functional correctness, error handling and edge cases such as division by zero and malformed CSV lines.

## Conceptual background

### Function pointers as typed code addresses

A function pointer is a value that denotes a callable code location. In ISO C a function designator in an expression context decays to a pointer to that function (with carefully specified exceptions). Calling through the pointer performs an indirect call provided that the pointer is non-null and points to a function with a compatible type.

Two observations matter for rigorous reasoning.

1. **Typing discipline is semantic not cosmetic.** A function pointer type encodes the full calling interface: return type, parameter types and the presence of a prototype. Calling a function through an incompatible pointer yields undefined behaviour even when the machine level calling convention appears to match.
2. **Indirection is a control-flow operation.** A function pointer is not merely an alternative spelling of a function name. It is an explicit data representation of a future call. That representation can be stored, passed, returned, selected and composed.

A minimal example illustrates the model:

```c
typedef int (*BinaryOp)(int, int);

int add(int x, int y) { return x + y; }

int apply(BinaryOp op, int x, int y) {
    return op(x, y); /* indirect call */
}
```

### Precedence traps and declarator parsing

The core syntactic hazard is that `*` binds less tightly than `()` in declarators.

- `int (*fp)(int)` denotes a pointer to a function that takes an `int` and returns an `int`.
- `int *fp(int)` denotes a function that takes an `int` and returns a pointer to `int`.

In this repository the recommended pedagogical technique is to introduce `typedef` names for function pointer types and then use those names consistently to reduce cognitive load.

### Callbacks and inversion of control

A callback is a function pointer passed into another function so that the callee can invoke user-supplied logic at a time and in a context determined by the callee. This is a concrete instance of inversion of control: instead of your code calling library code that then returns, the library calls your code and integrates its result into a larger algorithm.

The C standard library exemplifies this style in `qsort` and `bsearch`. These algorithms accept a comparator callback that defines a total ordering over elements.

## Generic algorithms in this unit

### `qsort`: comparator contracts and algorithmic consequences

`qsort` sorts an array of `nmemb` elements of size `size` bytes located at `base`. The comparator must impose a strict weak ordering (and in practice should be a total preorder for predictable behaviour).

A comparator must satisfy the following for all elements `a`, `b` and `c`:

- **Antisymmetry:** `cmp(a, b)` and `cmp(b, a)` have opposite signs.
- **Transitivity:** if `a < b` and `b < c` then `a < c`.
- **Consistency:** if `cmp(a, b) == 0` then `a` and `b` are equivalent for sorting.

The C standard does not mandate a particular sorting algorithm or stability property. Implementations commonly use quicksort variants, introsort or mergesort hybrids depending on the platform and libc. Therefore the only portable claims are those derived from the comparator contract and the pre and postconditions of the API.

#### Safe integer comparator pattern

Subtracting integers in a comparator may overflow and overflow of signed integers is undefined behaviour in ISO C. A safe pattern avoids subtraction:

```c
int cmp_int_asc(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);
}
```

### `bsearch`: sortedness as a semantic precondition

`bsearch` performs binary search over an array that must already be sorted according to the same comparator used for the search. The most frequent failure mode is to call `bsearch` after sorting by a different key or after mutating elements in a way that violates the ordering.

A disciplined use pattern is:

1. Choose the key and its comparator.
2. Sort the array with that comparator.
3. Search with the same comparator.
4. Avoid mutating the key field between steps 2 and 3.

### Dispatch tables: constant time selection by direct indexing

A dispatch table is an array indexed by an operation symbol, an opcode or any small integer. Each slot contains a function pointer implementing the corresponding behaviour. This replaces a branching structure with direct indexing.

A minimal calculator dispatch table is:

```c
typedef int (*Operation)(int, int);
static Operation dispatch[256] = {0};

void init(void) {
    dispatch['+'] = op_add;
    dispatch['-'] = op_subtract;
    dispatch['*'] = op_multiply;
    dispatch['/'] = op_divide;
    dispatch['%'] = op_modulo;
}
```

The lookup cost is constant time, the extension cost is localised and the code remains structurally flat.

#### Microarchitectural note

A long `switch` statement may compile into a jump table, a binary decision tree or a sequence of conditional branches depending on density and compiler heuristics. A manual dispatch table makes the mapping explicit and predictable but introduces an indirect call. Indirect calls can be less predictable for branch predictors than direct branches. In small teaching programmes this effect is negligible but it is a useful lens for later systems discussions.

## Repository structure

```
01-intro-pointers/
├── Makefile
├── README.md
├── data/
│   ├── numbers.txt
│   └── students.txt
├── slides/
│   ├── presentation-week01.html
│   └── presentation-comparativ.html
├── src/
│   ├── example1.c
│   ├── exercise1.c
│   └── exercise2.c
├── tests/
│   ├── test1_input.txt
│   ├── test1_expected.txt
│   ├── test2_input.txt
│   └── test2_expected.txt
├── teme/
│   ├── teme-cerinte.md
│   └── teme-extinse.md
└── solution/
    ├── exercise1_sol.c
    ├── exercise2_sol.c
    ├── tema1_sol.c
    └── tema2_sol.c
```

## File-by-file technical orientation

### `Makefile`

The build system follows a deliberately transparent pattern.

- Every `src/*.c` file is compiled into an executable of the same basename.
- `make test` runs a black-box regression test for the calculator and a minimal load test for the student database.
- `make solutions` compiles instructor reference implementations into `solution/` prefixed executables.

The Makefile is also a live artefact for discussing the build graph, implicit rules and the role of warnings (`-Wall -Wextra`) as a low-cost static analysis step.

### `src/example1.c`

A complete demonstration programme covering:

- Function pointer declaration and invocation.
- Callback driven higher-order functions (`apply_operation`, `apply_to_array`).
- Comparator functions used with `qsort` on a `Student` structure.
- Correct use of `bsearch` including the requirement that the array be sorted by the search key.
- A dispatch table for arithmetic operations.
- Comparator design for integer arrays including safe ordering and bespoke criteria such as parity prioritisation.

### `src/exercise1.c`

A calculator implemented with a dispatch table. The programme reads triples of the form `int op int` from standard input until EOF. It demonstrates:

- A dedicated function pointer type `Operation`.
- A table of 256 slots indexed by the operator character.
- Centralised error handling for unknown operators.
- Division by zero detection in division and modulo operations.

A small but important engineering detail is present: error paths flush `stdout` before printing to `stderr`. This preserves the intuitive ordering of messages when a test harness redirects both streams into a single file, a common situation in continuous integration.

### `src/exercise2.c`

A student database that loads a CSV file, prints it and demonstrates sorting and searching.

- CSV parsing uses a conservative `sscanf` pattern with field width limits and defensive NUL termination.
- Sorting uses `qsort` and three comparators (ID, name and grade).
- Searching uses `bsearch` with a comparator where the first argument is the key.

### `solution/*`

Reference implementations. They are intended for staff use and are compiled with `make solutions`.

## Build, run and test

### Standard build

```bash
make
```

### Run all executables

```bash
make run
```

### Automated tests

```bash
make test
```

### Additional robustness checks

Valgrind is not always available on all platforms. A portable alternative is to compile and run with sanitiser instrumentation where supported:

```bash
gcc -Wall -Wextra -std=c11 -g -O0 \
    -fsanitize=address,undefined -fno-omit-frame-pointer \
    -o exercise1_san src/exercise1.c
./exercise1_san < tests/test1_input.txt
```

## Pseudocode appendix

### Dispatch-table calculator

```
procedure initialise_dispatch(dispatch)
    for each slot in dispatch do
        slot <- null
    dispatch['+'] <- op_add
    dispatch['-'] <- op_subtract
    dispatch['*'] <- op_multiply
    dispatch['/'] <- op_divide
    dispatch['%'] <- op_modulo
end procedure

procedure calculate(a, op, b)
    if dispatch[op] is null then
        signal error "unknown operator"
        return 0
    result <- dispatch[op](a, b)
    if division_error_flag is set then
        signal error "division by zero"
    return result
end procedure

procedure main_loop
    initialise_dispatch(dispatch)
    while read (a, op, b) succeeds do
        (result, err) <- calculate(a, op, b)
        if err is false then
            print a, op, b, result
        end if
    end while
end procedure
```

### CSV loading and defensive parsing

```
procedure load_students(file)
    count <- 0
    line_number <- 0
    for each line in file do
        line_number <- line_number + 1
        strip trailing newline
        if line_number == 1 and line begins with "id" then
            continue
        if line is empty then
            continue
        if parse "id,name,grade" succeeds then
            students[count] <- (id, name, grade)
            count <- count + 1
        else
            emit warning with line_number
        end if
    end for
    return count
end procedure
```

## References

The following sources support the historical and algorithmic components of this unit. Where a standard or manual page is the most direct primary source, it is discussed in the text but not listed here because many standards are not distributed with DOIs.

| Reference (APA 7th) | DOI |
|---|---|
| Hoare, C. A. R. (1962). Quicksort. *The Computer Journal, 5*(1), 10–16. | https://doi.org/10.1093/comjnl/5.1.10 |
| Ritchie, D. M. (1993). The development of the C language. *ACM SIGPLAN Notices, 28*(3), 201–208. | https://doi.org/10.1145/155360.155580 |
| Bentley, J. L., & McIlroy, M. D. (1993). Engineering a sort function. *Software: Practice and Experience, 23*(11), 1249–1265. | https://doi.org/10.1002/spe.4380231105 |

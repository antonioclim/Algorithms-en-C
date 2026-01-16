# Week 05: Stacks as LIFO Structures in C (C11)

## Module positioning and intended learning outcomes

This laboratory develops the stack abstract data type as a disciplined mechanism for controlling evaluation order, delimiting nested structure and supporting reversible computation. The week is structured around two complementary implementations and a validation application that forces a precise contract between algorithmic reasoning and concrete I/O.

By the end of the week a student who has engaged with the code and the assessment tasks should be able to:

1. Specify the stack abstract data type as an interface whose semantics are independent of representation.
2. Derive representation invariants for both array-backed and linked structures and use those invariants to justify partial correctness.
3. Implement push, pop and peek operations with explicit error handling and well scoped memory ownership.
4. Explain amortised constant time for geometrically resized arrays using a potential argument.
5. Apply a stack to validate bracketed structure and report informative counterexamples.
6. Compare stack encodings in C, C++ and Java and articulate the consequences for safety, performance and expressivity.

The material is intentionally practical yet it is not merely procedural. Every programme in this repository is small enough to read end to end and sufficiently constrained to enable regression testing via exact output matching.

---

## Repository layout and build system

The repository is organised into a small number of directories with a single make-driven build surface.

- `src/` contains the primary student programmes.
  - `example1.c` is a fully worked demonstration programme that surveys common stack applications.
  - `exercise1.c` implements an array-based integer stack with dynamic resizing.
  - `exercise2.c` implements a linked stack of bracket tokens then applies it to bracket validation.
- `solution/` contains instructor reference implementations for the exercises and homework.
- `tests/` contains test inputs and exact expected outputs. The test harness is a diff-based oracle.
- `data/` contains sample inputs used by the run targets.
- `slides/` contains HTML slide decks supporting the laboratory narrative.
- `teme/` contains homework specifications.

### Build and execution

From the repository root:

```bash
make
make run-exercise1
make run-exercise2
make test
```

The `make test` target compiles `exercise1` and `exercise2` then executes each programme under a fixed input stream and compares the resulting output byte for byte against the corresponding file in `tests/`. This forces the programmes to be deterministic and it also forces error messages to be treated as part of the public contract.

A `make valgrind` target is included for leak checking. In some environments `valgrind` may not be installed. In that case the `make debug` target provides a practical alternative by enabling AddressSanitizer and UndefinedBehaviourSanitizer. You should interpret a clean sanitiser run as evidence that the stack operations respect memory ownership and do not perform invalid pointer arithmetic although it is not a substitute for a full leak profiler.

---

## The stack abstract data type

### Interface and semantic contract

A stack is a last-in first-out container supporting a restricted set of operations.

- `push(x)`: inserts `x` at the top.
- `pop()`: removes and returns the top element.
- `peek()`: returns the top element without removal.
- `is_empty()`: returns whether the stack contains zero elements.
- `size()`: returns the number of stored elements.

The contract is partial in the sense that `pop()` and `peek()` are defined only when the stack is non-empty. The laboratory material explores two approaches to handling this partiality:

1. Programmes that treat underflow as a fatal error and exit with a diagnostic.
2. APIs that return a Boolean success flag and write results through out-parameters.

Both approaches are valid. The first is pedagogically useful because it removes error propagation from the core algorithm and highlights the invariant obligations. The second is essential in robust systems because it maintains control flow and makes failure explicit.

### Representation invariants

A representation invariant is a predicate over the concrete state that must hold at all stable points in the programme. In this week two invariants are central.

**Array-backed stack (Exercise 1)**

Let `top` be the index of the last valid element and `capacity` be the allocation size in elements.

- `-1 ≤ top < capacity`
- `data` points to an array of length `capacity` when the structure is live
- the abstract size equals `top + 1`

**Linked stack (Exercise 2)**

Let `top` be a pointer to the head node and each node contain `(bracket, position, next)`.

- `top == NULL` if and only if `size == 0`
- `size` equals the number of nodes reachable from `top` by repeated `next`
- the list is acyclic

These invariants are not decorative. Each operation is correct only relative to them. For instance `pop()` on the linked stack is correct because it preserves acyclicity and decreases size by exactly one when the precondition `top != NULL` holds.

---

## Exercise 1: array-based stack with dynamic resizing

### Programme purpose

`src/exercise1.c` implements an integer stack whose storage is a dynamically allocated array. When the stack reaches the end of its allocation it grows by a constant factor (two). The programme includes a deterministic self-test that pushes a fixed range of integers, prints intermediate states and then pops all values.

The deterministic output is a deliberate constraint. It supports the regression test in `tests/test1_expected.txt` and it allows you to reason about the precise point at which resizing occurs.

### Growth strategy: grow on fill rather than grow on overflow

Many textbooks describe an array stack that grows when an insertion would overflow the current capacity. In this repository the observed contract is slightly different: the structure grows immediately when a `push` operation fills the last available slot. Concretely, after a `push` the stack is never left in a full state. This yields a distinctive placement of the diagnostic line:

- initial capacity 4 grows after pushing the fourth element rather than before pushing the fifth
- capacity 8 grows after pushing the eighth element rather than before pushing the ninth

This variant has two motivations.

1. It makes the relationship between capacity and size explicit in the printed trace because the resize line appears exactly at the boundary where `size == capacity` would otherwise hold.
2. It maintains the invariant that a subsequent `push` will not require a resize in the common case which simplifies some teaching demonstrations.

The asymptotic performance remains unchanged.

### Algorithmic specification

**Pseudocode**

```text
procedure PUSH(S, x):
    require S is a live stack

    if FULL(S):
        RESIZE(S, 2 * S.capacity)

    S.top <- S.top + 1
    S.data[S.top] <- x

    if FULL(S):
        RESIZE(S, 2 * S.capacity)

procedure FULL(S):
    return (S.top = S.capacity - 1)

procedure RESIZE(S, new_capacity):
    new_data <- realloc(S.data, new_capacity)
    if new_data is NULL:
        signal allocation failure
    S.data <- new_data
    S.capacity <- new_capacity
```

The first `FULL` check is defensive. Under the intended invariant it is not triggered because the stack does not remain full after successful pushes. It remains as a robustness measure and it makes the code resilient to future modifications.

### Correctness sketch

For a stack whose invariant holds at entry:

- the first defensive resize preserves the invariant by increasing `capacity` without changing `top`
- the write `data[top] = x` is in bounds because either there was spare capacity already or a resize was performed
- the second check triggers precisely when `top == capacity - 1` after insertion which is equivalent to `size == capacity`
- the second resize therefore restores `top < capacity - 1` and establishes the postcondition that the structure is not full

### Amortised analysis using a potential function

Let the actual cost of a normal push be 1 unit and let the cost of a resize from capacity `k` to `2k` be `k` units to model the element copies that `realloc` may perform. Define the potential function

```text
Φ(S) = 2 * size(S) - capacity(S)
```

For a non-resizing push the actual cost is 1 and the potential increases by 2 because size increases by one and capacity is unchanged. The amortised cost is therefore at most 3.

For a resizing push that triggers growth after inserting the `k`th element into a stack of capacity `k`:

- actual cost is `1 + k` because we insert then potentially copy `k` elements
- capacity increases by `k` and size becomes `k`
- potential before the push is `2*(k-1) - k = k - 2` and after resizing it becomes `2*k - 2k = 0`

Thus the potential drops by approximately `k` which pays for the copy. The amortised cost remains constant. The specific constant depends on how the cost model treats `realloc` but the qualitative conclusion is stable: geometric resizing yields amortised O(1) push.

### Cross-language comparison

The same structure appears in most mainstream languages albeit with different default policies.

**Python (list as a stack)**

```python
s = []
s.append(x)     # push
x = s.pop()     # pop
x = s[-1]       # peek
```

CPython uses over-allocation for lists so that repeated `append` has amortised constant time.

**C++ (std::vector and std::stack)**

```cpp
std::vector<int> v;
v.push_back(x);
int y = v.back();
v.pop_back();

std::stack<int> s;
s.push(x);
int y2 = s.top();
s.pop();
```

`std::vector` typically grows geometrically. `std::stack` is an adaptor, often backed by `std::deque`.

**Java (ArrayDeque as a stack)**

```java
Deque<Integer> s = new ArrayDeque<>();
s.push(x);
int y = s.peek();
int z = s.pop();
```

Java’s `Stack` class is legacy. `ArrayDeque` is preferred and it resizes its internal buffer.

---

## Exercise 2: balanced brackets validator using a linked stack

### Programme purpose

`src/exercise2.c` reads lines from standard input and validates bracket structure for the bracket classes `()`, `[]` and `{}`. Non-bracket characters are ignored which makes the tool usable on source code fragments such as `int main() { return 0; }`.

The programme reports one of three invalidity classes:

1. **Unmatched closing bracket**: a closing bracket appears when the stack is empty.
2. **Mismatched bracket**: a closing bracket appears but it does not correspond to the most recent unmatched opening bracket.
3. **Unclosed opening bracket**: the input ends while the stack remains non-empty.

Positions are reported as zero-indexed character offsets in the input line. This convention is chosen because it aligns with C array indexing and makes the trace easy to verify mechanically.

### Algorithmic specification

**Pseudocode**

```text
procedure VALIDATE(expr):
    S <- empty stack

    for i from 0 to |expr|-1:
        c <- expr[i]

        if c is an opening bracket:
            PUSH(S, (c, i))

        else if c is a closing bracket:
            if EMPTY(S):
                report unmatched closing at i
                return INVALID

            (open, pos) <- POP(S)
            if open != MATCHING_OPEN(c):
                report mismatch at i with expected c and found MATCHING_CLOSE(open)
                return INVALID

    if not EMPTY(S):
        (open, pos) <- PEEK(S)
        report unclosed open at pos
        return INVALID

    return VALID
```

The validator is correct because the stack always contains exactly the unmatched opening brackets encountered so far in strictly increasing position order. A well-formed expression requires that every closing bracket matches the most recent unmatched opening bracket, hence the use of LIFO.

### Linked implementation notes

The linked stack stores tokens as nodes allocated per push. This choice is didactic: it forces explicit ownership and it makes underflow handling explicit via `bool` return values rather than fatal exits.

- `stack_push` allocates a node and prepends it to the list.
- `stack_pop` removes the head node, copies payload fields to out-parameters and frees the node.
- `stack_destroy` traverses and frees all remaining nodes which ensures no leaks even when an early error is reported.

### Deterministic output as a test contract

The validator prints a fixed header, a separator line and then per-expression diagnostics. The repository includes `tests/test2_input.txt` and `tests/test2_expected.txt` as a reproducible oracle. The output contract is intentionally strict because it forces you to treat diagnostic text as a public interface which is a realistic constraint in tooling and grading environments.

---

## Test strategy and reproducibility

### Regression tests

The tests in `tests/` are designed as black-box checks. They assume no internal knowledge of the implementation. Each exercise programme is executed as a subprocess and the produced output is compared against an expected file using `diff`.

This approach has two advantages.

1. It validates the complete user-visible behaviour including formatting and error messages.
2. It is independent of the compiler’s internal diagnostics and does not require a unit testing framework.

The limitation is that it does not localise failures. When a diff occurs you should run `make test-verbose` and interpret the unified diff as a guide to the first divergence.

### Memory correctness

The build system provides two complementary approaches.

- `make valgrind` executes each programme under Valgrind. This is the most direct way to detect leaks and use-after-free in C.
- `make debug` enables compiler sanitizers and rebuilds. This is often faster to run in teaching environments and it detects a broad class of undefined behaviour.

A memory clean run is not a guarantee of correctness but it is a strong negative signal. If a stack leaks memory or writes out of bounds then either the representation invariant was violated or ownership was not clearly assigned.

---

## Data files and slides

- `data/expressions.txt` contains sample expressions for interactive demonstration.
- `data/postfix_expressions.txt` supports the postfix calculator in the broader week narrative.
- `slides/presentation-week05.html` and `slides/presentation-comparativ.html` provide lecture material and comparisons.

The slides are not executed as part of the build. They are static HTML documents and may be opened locally in a browser.

---

## Homework

The homework specifications are maintained in `teme/`.

- `homework-requirements.md` specifies two main tasks: postfix evaluation and infix-to-postfix conversion via the shunting-yard method.
- `homework-extended.md` provides optional advanced challenges that extend the stack paradigm to richer problems.

You should read the specifications as formal requirements. In particular you should treat input format, error messages and memory ownership as part of the assessed contract.

---

## Change notes for this repository state

The current repository state includes completed implementations for both exercise programmes and a validated regression test run.

- Exercise 1 implements array resizing and reproduces the expected trace.
- Exercise 2 implements a linked stack and reproduces the expected diagnostics.
- Instructor solution programmes are aligned with the same output oracle for ease of demonstration.

If you modify the output format you must update the corresponding expected files and document the change. If you modify the resizing strategy you should re-derive the amortised argument because the constant factors and the diagnostic trace will change.

# Week 01 Coursework: Function Pointers, Callbacks and Table Driven Evaluation (ISO C11)

## 1. Administrative information

- **Recommended submission window:** end of Week 2 (teaching week numbering)
- **Total available mark:** 100 points
- **Language and toolchain:** ISO C (C11) compiled with GCC or an equivalent C11 compiler
- **Baseline compiler options:** `-Wall -Wextra -std=c11`

The coursework is designed to reinforce the laboratory material by requiring you to implement two small systems that expose the same deep idea: *behaviour as data*. In both tasks you will represent an operation by a function pointer and then organise those pointers so that the surrounding algorithm is generic, structurally flat and easily extensible.

A submission is considered correct only if it compiles without warnings under the baseline options and passes a reasonable black box test plan including edge cases. Where error handling is required the programme must not crash on valid input and must emit diagnostic messages that are specific enough to support debugging.

## 2. Coursework task 1: Generic filtering system (50 points)

### 2.1 Problem statement

Implement a generic, callback driven filtering function capable of selecting elements from an input array into a pre-allocated output array. The function must be type-agnostic: it must work for arrays of integers and arrays of user-defined structures.

This task models a foundational higher-order pattern in systems programming: a library function performs the iteration and memory movement while a user-supplied predicate encodes the decision logic.

### 2.2 Required data model

Define a `Product` structure:

```c
typedef struct {
    int id;
    char name[50];
    float price;
    int quantity;
} Product;
```

The choice of fixed-length `name` is deliberate: it makes memory layout explicit and allows the exercise to focus on pointer arithmetic, byte-wise copying and defensive string handling without requiring dynamic allocation.

### 2.3 Required API

Implement the generic filter:

```c
int filter_array(void *src, void *dest, int n, size_t elem_size,
                 int (*predicate)(const void *));
```

- `src` points to the first element of the input array.
- `dest` points to the first element of the output array.
- `n` is the number of elements in `src`.
- `elem_size` is the size in bytes of one element.
- `predicate` returns `1` if the element is to be retained and `0` otherwise.

Return the number of retained elements.

### 2.4 Algorithmic requirements

Your implementation must:

1. Iterate once over the `n` input elements.
2. Apply the predicate to each element.
3. Copy retained elements into `dest` in their original relative order.
4. Perform copying without knowing the element type at compile time.

The canonical solution uses `unsigned char *` to perform byte-wise pointer arithmetic and `memcpy` to copy `elem_size` bytes at a time.

#### Reference pseudocode

```
procedure filter_array(src, dest, n, elem_size, predicate)
    count <- 0
    for i from 0 to n-1 do
        current <- src + i * elem_size
        if predicate(current) == 1 then
            memcpy(dest + count * elem_size, current, elem_size)
            count <- count + 1
        end if
    end for
    return count
end procedure
```

### 2.5 Required predicates

Implement integer predicates:

- `is_positive(const void *elem)` returns `1` if the integer is strictly positive
- `is_even(const void *elem)` returns `1` if the integer is even
- `is_greater_than_10(const void *elem)` returns `1` if the integer is greater than 10

Implement product predicates:

- `is_in_stock(const void *elem)` returns `1` if `quantity > 0`
- `is_expensive(const void *elem)` returns `1` if `price > 100.0f`
- `is_low_stock(const void *elem)` returns `1` if `quantity > 0` and `quantity < 5`

### 2.6 Demonstration programme

Provide a `main` function that demonstrates filtering over:

- an array of at least ten integers
- an array of at least five `Product` values

The output format is not rigidly prescribed but it must be readable and must clearly show the relationship between the input and filtered output.

### 2.7 Marking scheme for task 1

| Component | Points |
|---|---:|
| Correct `Product` definition and safe string handling | 10 |
| Correct generic filter implementation and correct use of the predicate callback | 15 |
| Integer predicates behave correctly across edge cases (0, negative values) | 10 |
| Product predicates behave correctly across edge cases (0 quantity, boundary prices) | 10 |
| Demonstration `main` and basic test coverage | 5 |

## 3. Coursework task 2: Mathematical expression parser and evaluator (50 points)

### 3.1 Problem statement

Implement a small interactive expression evaluator that reads strings of the form:

```
<integer> <operator> <integer>
```

The evaluator must parse the expression, validate it and compute the result using a dispatch table rather than a `switch` statement. The purpose is to force a clean separation between parsing, semantic checking and evaluation.

### 3.2 Required data model

Define an expression record:

```c
typedef struct {
    int operand1;
    char operator;
    int operand2;
} Expression;
```

### 3.3 Supported operations

Implement at least the following operators:

- `+` addition
- `-` subtraction
- `*` multiplication
- `/` integer division with division by zero detection
- `%` integer modulo with division by zero detection
- `^` exponentiation with a well-defined behaviour for non-negative exponents

The evaluator must handle error conditions without undefined behaviour. In particular you must address division by zero and overflow where reasonable. A robust strategy is to return an error code and avoid committing to a result value when an error occurs.

### 3.4 Dispatch-table design

Use a function pointer type that supports error propagation:

```c
typedef int (*Operation)(int a, int b, int *error);
```

Your dispatch table should be an array indexed by the operator character. Use a full 256 slot table and index by `(unsigned char)op`.

#### Reference pseudocode

```
procedure init_dispatch(dispatch)
    for each i in 0..255 do
        dispatch[i] <- null
    dispatch['+'] <- op_add
    dispatch['-'] <- op_subtract
    dispatch['*'] <- op_multiply
    dispatch['/'] <- op_divide
    dispatch['%'] <- op_modulo
    dispatch['^'] <- op_power
end procedure

procedure evaluate(expr)
    op <- expr.operator
    if dispatch[op] is null then
        signal error "unknown operator"
        return failure
    return dispatch[op](expr.operand1, expr.operand2)
end procedure
```

### 3.5 Parsing requirements

Implement:

```c
int parse_expression(const char *str, Expression *expr);
```

The parser must:

- accept multiple spaces or tab characters between tokens
- accept leading and trailing whitespace
- reject malformed input (missing tokens, unknown operator, non-numeric operands)

A recommended approach is to use `strtol` for numeric conversion and to implement a `skip_whitespace` helper. This yields more predictable error detection than `scanf` because you can inspect where parsing stopped.

#### Minimal grammar

```
expr  := ws? int ws op ws int ws?
ws    := (space | tab | newline)+
int   := ['+'|'-']? digit+
op    := one of + - * / % ^
```

### 3.6 Exponentiation algorithm

For `a ^ b` with `b >= 0` the recommended algorithm is exponentiation by squaring, which runs in `O(log b)` multiplications rather than `O(b)`.

#### Reference pseudocode

```
procedure power(base, exp)
    if exp < 0 then error
    result <- 1
    while exp > 0 do
        if exp is odd then
            result <- result * base
        base <- base * base
        exp <- exp // 2
    end while
    return result
end procedure
```

### 3.7 Interactive interface

Your main programme must:

1. Print a short banner describing the input format.
2. Read a line at a time from standard input.
3. Terminate on `exit` or EOF.
4. For each line either print the computed result or print an error message and continue.

### 3.8 Marking scheme for task 2

| Component | Points |
|---|---:|
| Correct parsing function with robust whitespace handling | 15 |
| Correct dispatch-table design and operation registration | 15 |
| Correct evaluation with division by zero and unknown operator handling | 10 |
| Correct exponentiation implementation and basic overflow awareness | 5 |
| Interactive interface and systematic tests | 5 |

## 4. General assessment criteria

| Criterion | Points |
|---|---:|
| Functional correctness across normal and edge cases | 40 |
| Correct and idiomatic use of function pointers and callbacks | 25 |
| Handling of special cases and defensiveness against malformed input | 15 |
| Code quality (naming, structure, comments that explain intent rather than restating syntax) | 10 |
| Compiles without warnings under the baseline options | 10 |

### Common penalties

- Compiles with warnings: up to −10 points
- Memory errors or leaks detectable with sanitiser instrumentation: up to −20 points
- Crashes on valid input: up to −30 points
- Plagiarism or uncredited copying: severe penalties per course regulations

## 5. Submission packaging

1. Submit `tema1_filter.c` and `tema2_parser.c`.
2. Provide a short `README` comment block at the top of each file describing how to compile and run it.
3. Ensure each file can compile independently.

Recommended commands:

```bash
gcc -Wall -Wextra -std=c11 -g -O0 -o tema1 tema1_filter.c
./tema1

gcc -Wall -Wextra -std=c11 -g -O0 -o tema2 tema2_parser.c
./tema2
```

## 6. Short bibliographic pointers

| Reference (APA 7th) | DOI |
|---|---|
| Hoare, C. A. R. (1962). Quicksort. *The Computer Journal, 5*(1), 10–16. | https://doi.org/10.1093/comjnl/5.1.10 |
| Bentley, J. L., & McIlroy, M. D. (1993). Engineering a sort function. *Software: Practice and Experience, 23*(11), 1249–1265. | https://doi.org/10.1002/spe.4380231105 |

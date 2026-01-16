# Week 04: Linked Lists and Relative File Workflows

## Purpose and scope

This unit consolidates pointer based data structures in C by moving from singly linked lists to higher level linked representations that remain explicit about allocation, ownership, lifetimes and invariants. The week is organised around two complementary themes:

1. **Linked structure design**: how to represent a collection in non contiguous memory while preserving predictable semantics for insertion, deletion, traversal and search.
2. **Deterministic file driven execution**: how to structure command files and relative paths so that programs are testable, reproducible and portable across machines without hard coded absolute paths.

The material assumes familiarity with C11, basic pointer semantics, stack vs heap allocation and the compilation model of translation units. Where relevant, the text makes the implicit invariants explicit, because linked structures are dominated by what must always remain true rather than by what is convenient to compute.

## Repository layout

The directory structure is intentionally simple so that relative paths remain meaningful when the project is built from the repository root.

```
.
├── Makefile
├── README.md
├── data
│   ├── polynomials.txt
│   └── students.txt
├── slides
│   ├── presentation-week04.html
│   └── presentation-comparativ.html
├── src
│   ├── example1.c
│   ├── exercise1.c
│   └── exercise2.c
├── solution
│   ├── exercise1_sol.c
│   ├── exercise2_sol.c
│   ├── homework1_sol.c
│   └── homework2_sol.c
└── tests
    ├── test1_input.txt
    ├── test1_expected.txt
    ├── test2_input.txt
    └── test2_expected.txt
```

Two design choices matter for both pedagogy and engineering practice:

- **Command files live under `tests/` and `data/`**. This forces programs to accept relative paths at runtime, which is the common case in continuous integration pipelines and in educational autograding.
- **Tests are file driven and output is deterministic**. If a program has a random component, it must be explicitly seeded and documented. This week avoids randomness in the assessed executables.

## Build and execution workflow

### Build

From the repository root:

```bash
make
```

This builds the student targets:

- `example1` (demonstration programme)
- `exercise1` (student record management)
- `exercise2` (polynomial calculator)

To build instructor solutions:

```bash
make solutions
```

### Run

```bash
make run
make run-exercise1
make run-exercise2
```

File driven execution uses relative paths, for example:

```bash
./exercise1 tests/test1_input.txt
./exercise2 tests/test2_input.txt
```

The most important practical point is that the **current working directory** is part of the specification. If you execute a binary from a different directory, the relative path you pass is resolved against that directory, not against the directory that contains the binary.

### Test

```bash
make test
```

This runs:

- `exercise1` with `tests/test1_input.txt` and diffs against `tests/test1_expected.txt`
- `exercise2` with `tests/test2_input.txt` and diffs against `tests/test2_expected.txt`

The tests are intentionally strict because the aim is to train attention to specification, including whitespace, ordering and numerical formatting.

## Conceptual foundations

### Memory model and node representation

A singly linked list stores elements as heap allocated nodes with an explicit successor pointer. In C, the conventional representation is a self referential structure:

```c
typedef struct Node {
    int data;
    struct Node *next;
} Node;
```

On a typical 64 bit platform, `sizeof(Node)` is at least `sizeof(int) + sizeof(void*)` and is often larger due to alignment. It is therefore unsafe to infer node size from a simple arithmetic sum, because the compiler may insert padding so that the pointer member is aligned.

A disciplined way to reason about node correctness is to state the invariant and then prove that each operation preserves it:

- **Reachability invariant**: every node in the list is reachable from the head by following `next` pointers.
- **Termination invariant**: the final node has `next == NULL`.
- **Ownership invariant**: every node is either owned by exactly one list or is not in any list and will be freed.

### Traversal schema

The canonical traversal schema is:

```
current <- head
while current != NULL:
    visit(current)
    current <- current.next
```

In C this becomes:

```c
for (Node *current = head; current != NULL; current = current->next) {
    /* visit current */
}
```

The fundamental hazard is dereferencing `current` after it becomes `NULL`. A secondary hazard is dereferencing a pointer after its owning node has been freed.

## File: `src/example1.c` (demonstration programme)

### Role

`example1.c` is a complete reference implementation that demonstrates typical singly linked list operations without the additional complication of parsing command files. It exists to separate conceptual learning (pointer manipulations, ownership and invariants) from input parsing and formatting.

### Functional overview

The demonstration typically includes:

- node creation with `malloc`
- insertion at head and tail
- traversal and formatted printing
- deletion patterns
- full deallocation of the list

### Algorithmic notes

Even when the interface looks trivial, each operation is governed by one of a small number of pointer update archetypes:

1. **Head insertion**

Pseudocode:

```
function push_front(head, value):
    node <- allocate Node
    node.data <- value
    node.next <- head
    head <- node
    return head
```

2. **Deletion after a predecessor pointer is known**

Pseudocode:

```
function delete_after(prev):
    target <- prev.next
    if target == NULL:
        return
    prev.next <- target.next
    free(target)
```

The programme can be used as a baseline for debugging: if an operation fails in `exercise1` or `exercise2`, implement the same logic first in `example1` where there are fewer moving parts.

## File: `src/exercise1.c` (student records management)

### Specification

`exercise1.c` implements a student record list with the following properties:

- nodes are stored in ascending order by student identifier
- insertion rejects duplicate identifiers
- search benefits from early termination because the list is sorted
- deletion frees memory and preserves the sort order invariant
- file driven mode interprets a command grammar defined below

The node type is:

```c
typedef struct StudentNode {
    int id;
    char name[MAX_NAME_LENGTH];
    float gpa;
    struct StudentNode *next;
} StudentNode;
```

The **core invariant** is:

- for any two consecutive nodes `u -> v`, `u.id < v.id`

From this invariant several properties follow:

- insertion can stop at the first node with id greater than the inserted id
- search can stop at the first node with id greater than the target id
- the list has a unique canonical representation for a given set of ids

### Operation: create node

`create_student(id, name, gpa)` allocates a node, copies fixed width textual data and initialises `next` to `NULL`.

Pseudocode:

```
function create_student(id, name, gpa):
    node <- malloc(sizeof(StudentNode))
    if node == NULL:
        return NULL
    node.id <- id
    node.gpa <- gpa
    node.name <- bounded_copy(name, MAX_NAME_LENGTH)
    node.next <- NULL
    return node
```

Key engineering detail: `strncpy` does not guarantee null termination when the source is long. The implementation therefore explicitly sets `name[MAX_NAME_LENGTH - 1] = '\0'`.

### Operation: sorted insertion

`insert_sorted(head_ref, id, name, gpa)` must preserve the sort invariant and must reject duplicates.

Pseudocode:

```
function insert_sorted(head_ref, id, name, gpa):
    new <- create_student(id, name, gpa)
    if new == NULL:
        return false

    if head_ref.value == NULL or head_ref.value.id > id:
        new.next <- head_ref.value
        head_ref.value <- new
        return true

    if head_ref.value.id == id:
        free(new)
        return false

    cur <- head_ref.value
    while cur.next != NULL and cur.next.id < id:
        cur <- cur.next

    if cur.next != NULL and cur.next.id == id:
        free(new)
        return false

    new.next <- cur.next
    cur.next <- new
    return true
```

Complexity:

- worst case time: `O(n)` comparisons and pointer traversals
- best case time: `O(1)` when inserting before the head
- additional space: `O(1)` beyond the new node

### Operation: search with early termination

Because the list is sorted by id, `find_student(head, id)` may stop early.

Pseudocode:

```
function find_student(head, id):
    cur <- head
    while cur != NULL:
        if cur.id == id:
            return cur
        if cur.id > id:
            return NULL
        cur <- cur.next
    return NULL
```

This is a simple but pedagogically important example of how a structural invariant yields an algorithmic improvement.

### Operation: deletion by id

Deletion requires a predecessor pointer so that the list can be re linked around the removed node.

Pseudocode:

```
function delete_student(head_ref, id):
    if head_ref.value == NULL:
        return false

    if head_ref.value.id == id:
        victim <- head_ref.value
        head_ref.value <- victim.next
        free(victim)
        return true

    cur <- head_ref.value
    while cur.next != NULL and cur.next.id < id:
        cur <- cur.next

    if cur.next != NULL and cur.next.id == id:
        victim <- cur.next
        cur.next <- victim.next
        free(victim)
        return true

    return false
```

### File driven command grammar

In file mode, the programme reads a command file line by line. Empty lines and lines beginning with `#` are ignored.

Grammar (informal):

```
COMMAND := ADD ID NAME GPA
        | FIND ID
        | DELETE ID
        | DISPLAY
        | COUNT

ID   := integer
NAME := token without whitespace (underscores may be used)
GPA  := floating point number
```

The execution model is a classic read parse dispatch loop:

```
open file
for each line:
    normalise line
    if comment or blank: continue
    parse first token as command
    switch command:
        case ADD: parse args, call insert_sorted, print status
        case FIND: parse id, call find_student, print status
        case DELETE: parse id, call delete_student, print status
        case DISPLAY: display table
        case COUNT: print count
close file
```

### Cross language correspondence

The core ideas translate directly although the mechanisms differ.

Python sketch (reference semantics and garbage collection):

```python
class StudentNode:
    def __init__(self, sid: int, name: str, gpa: float):
        self.sid = sid
        self.name = name
        self.gpa = gpa
        self.next: 'StudentNode | None' = None

def insert_sorted(head: StudentNode | None, node: StudentNode) -> StudentNode:
    if head is None or head.sid > node.sid:
        node.next = head
        return node
    cur = head
    while cur.next is not None and cur.next.sid < node.sid:
        cur = cur.next
    if cur.next is not None and cur.next.sid == node.sid:
        raise ValueError('duplicate')
    node.next = cur.next
    cur.next = node
    return head
```

C++ sketch (ownership via `std::unique_ptr`):

```cpp
struct Node {
    int id;
    std::string name;
    float gpa;
    std::unique_ptr<Node> next;
};
```

Java sketch (reference type, garbage collected):

```java
final class Node {
    int id;
    String name;
    float gpa;
    Node next;
}
```

The algorithmic structure remains the same, but the failure modes shift: in C the dominant risks are leaks and use after free, while in garbage collected languages the dominant risks are inadvertent aliasing and unintended retention.

## File: `src/exercise2.c` (polynomial calculator)

### Representation

A sparse polynomial is represented as a linked list of non zero terms in descending exponent order. Each term is a pair `(coefficient, exponent)`.

```c
typedef struct Term {
    double coefficient;
    int exponent;
    struct Term *next;
} Term;

typedef struct Polynomial {
    Term *head;
    char name;
} Polynomial;
```

Core invariants:

- exponents are strictly descending along the list
- there is at most one term for each exponent
- coefficients are non zero up to a small threshold `EPSILON`

These invariants matter because they simplify every other operation. Most importantly, they guarantee that addition and multiplication can be implemented using the same primitive insertion routine.

### Primitive operation: add term with combination

`add_term(poly, coeff, exp)` inserts a term while maintaining the invariants. If a term with the same exponent exists, coefficients are combined and the term is removed if the sum becomes effectively zero.

Pseudocode:

```
function add_term(poly, coeff, exp):
    if abs(coeff) < EPSILON: return

    if poly.head == NULL or poly.head.exp < exp:
        insert new head
        return

    if poly.head.exp == exp:
        poly.head.coeff += coeff
        if abs(poly.head.coeff) < EPSILON:
            remove head
        return

    cur <- poly.head
    while cur.next != NULL and cur.next.exp > exp:
        cur <- cur.next

    if cur.next != NULL and cur.next.exp == exp:
        cur.next.coeff += coeff
        if abs(cur.next.coeff) < EPSILON:
            remove cur.next
    else:
        insert new node after cur
```

This function is the algebraic analogue of sorted insertion in `exercise1`, except that the equality case triggers a merge rather than a rejection.

### Polynomial addition

Addition is implemented by creating an empty result polynomial and adding each term from the operands.

Pseudocode:

```
function add_polynomials(p1, p2):
    r <- empty polynomial
    for each term t in p1:
        add_term(r, t.coeff, t.exp)
    for each term t in p2:
        add_term(r, t.coeff, t.exp)
    return r
```

A more specialised merge algorithm exists because the lists are ordered. It can compute the result in a single pass without repeated search, but the present implementation intentionally demonstrates the power of a well specified primitive.

### Polynomial multiplication

Multiplication uses the distributive law by multiplying each term of the first polynomial by each term of the second and inserting into the result.

Pseudocode:

```
function multiply_polynomials(p1, p2):
    r <- empty polynomial
    for each term a in p1:
        for each term b in p2:
            add_term(r, a.coeff * b.coeff, a.exp + b.exp)
    return r
```

Complexity:

- let `m` be the number of non zero terms in `p1` and `n` in `p2`
- multiplication performs `m * n` elementary products
- each `add_term` performs a search whose cost depends on the current length of `r`

In the worst case, naive multiplication can become quadratic in the number of output terms. This is acceptable for instructional purposes and is also a faithful reflection of why sparse polynomial libraries often employ more sophisticated data structures such as balanced trees or hash maps.

### Evaluation

The programme uses direct evaluation:

```
P(x) = sum_{(c,e) in terms} c * x^e
```

Pseudocode:

```
function evaluate(poly, x):
    acc <- 0
    for each term t in poly:
        acc += t.coeff * pow(x, t.exp)
    return acc
```

Alternative (not required but useful to know): Horner's method reduces repeated exponentiation for dense polynomials. For sparse polynomials it is less straightforward because gaps in exponents must be handled explicitly.

### Derivative

The derivative operator is linear and can be implemented termwise.

```
d/dx (c x^e) = (c * e) x^(e-1) for e > 0
```

Pseudocode:

```
function derivative(poly):
    r <- empty
    for each term t in poly:
        if t.exp > 0:
            add_term(r, t.coeff * t.exp, t.exp - 1)
    return r
```

### File driven mode

In file mode `exercise2` acts as a small interpreter. The input format is designed to be human writable and machine parsable. As with `exercise1`, blank lines and `#` comments are ignored.

Supported commands:

```
POLY X: c1 e1 c2 e2 ...   # define polynomial X
DISPLAY X                  # print X
ADD A B                    # print A + B
MULTIPLY A B               # print A * B
EVAL A x                   # print A(x)
```

The interpreter stores polynomials in a fixed array indexed by name. This choice is appropriate here because polynomial identifiers are single letters and the number of defined polynomials is small.

Pseudocode:

```
polys[26] <- all NULL
for each line:
    if POLY:
        parse name and remainder
        polys[name] <- parse_polynomial(remainder)
    else if DISPLAY:
        display polys[name]
    else if ADD:
        tmp <- add_polynomials(polys[a], polys[b])
        print expression tmp
        free tmp
    else if MULTIPLY:
        tmp <- multiply_polynomials(...)
        print expression tmp
        free tmp
    else if EVAL:
        print evaluate(polys[a], x)
end
free all polys
```

The output formatting is intentionally stable so that tests can diff expected output against actual output.

## Data files

### `data/students.txt`

A sample command script for `exercise1`. It illustrates adding, searching, deleting and displaying while relying on relative paths.

### `data/polynomials.txt`

A sample polynomial script for `exercise2`. It illustrates defining multiple polynomials and applying operations.

## Slides

The HTML slide decks in `slides/` provide a narrative complement to the repository. They are not required for compilation but they are a useful reference when revising invariants and pointer update patterns.

## Testing philosophy

The tests in `tests/` are deliberately small, because the unit tests are intended to validate invariants and formatting rather than to stress performance. For performance evaluation, the correct approach is to generate synthetic inputs (large command files) and to measure asymptotic behaviour.

A recommended testing progression is:

1. compile with warnings enabled and treat warnings as errors during development
2. run `make test` after each logical change
3. add additional local tests that exercise boundary cases (empty list, singleton list, duplicate insertion, deletion of head and tail)
4. use sanitiser builds where available (`-fsanitize=address,undefined`) to detect use after free and out of bounds writes

## References

The following references provide the conceptual and historical basis for the selected topics. Entries are formatted in APA 7th edition style and each includes a DOI link.

| Topic | Reference (APA 7th ed) | DOI |
|---|---|---|
| List processing and symbolic computation | McCarthy, J. (1960). Recursive functions of symbolic expressions and their computation by machine, Part I. *Communications of the ACM, 3*(4), 184-195. | https://doi.org/10.1145/367177.367199 |
| Probabilistic linked structures | Pugh, W. (1990). Skip lists: A probabilistic alternative to balanced trees. *Communications of the ACM, 33*(6), 668-676. | https://doi.org/10.1145/78973.78977 |
| Sparse matrices as a research artefact | Davis, T. A. and Hu, Y. (2011). The University of Florida sparse matrix collection. *ACM Transactions on Mathematical Software, 38*(1), Article 1. | https://doi.org/10.1145/2049662.2049663 |
| Random permutation algorithm | Durstenfeld, R. (1964). Algorithm 235: Random permutation. *Communications of the ACM, 7*(7), 420. | https://doi.org/10.1145/364520.364540 |

## Appendix A: Correctness arguments as invariants

The central methodological point of this week is that linked list programming is best understood as *invariant preservation* rather than as ad hoc pointer juggling. The following sketches indicate what must be shown for each core operation.

### A1. Singly linked list sortedness invariant

Invariant statement:

For a list with head pointer `H`, define the sequence of nodes obtained by repeated dereference of `.next` starting at `H` until `NULL` is reached. The list is **sorted by id** if for every consecutive pair `(u, v)` in this sequence we have `u.id < v.id`.

Proof obligation for each mutating operation:

1. **Preservation**: if the invariant holds before the operation and the operation returns success then the invariant holds afterwards.
2. **Safety**: no dereference of a `NULL` pointer occurs and no dereference of a freed pointer occurs.
3. **Ownership**: every allocated node is either reachable from `H` after the operation or is freed exactly once before the operation returns.

### A2. `insert_sorted` proof sketch

Case analysis corresponds exactly to the implementation structure:

1. **Empty list or insertion before head**: the new node becomes the head and its successor is the old head. Sortedness holds because either the list was empty or the old head id was strictly greater than the inserted id.
2. **Duplicate at head**: the operation returns failure after freeing the allocated node, therefore no observable state change occurs.
3. **Insertion in the interior**: traversal stops at the unique predecessor `p` such that `p.id < id` and either `p.next == NULL` or `p.next.id > id`. Linking `p.next <- new` and `new.next <- old(p.next)` preserves the local ordering and therefore the global invariant.

The sortedness invariant is therefore preserved by construction rather than by post hoc checking.

### A3. `delete_student` proof sketch

The essential trick is again to separate head deletion from interior deletion:

- if the head matches then the new head is the old head successor which is a node in the old list, therefore reachability and termination are preserved
- otherwise the predecessor traversal stops at the unique node whose successor may match the target id because the list is sorted, therefore only one relinking is required

In both cases the freed node is no longer reachable from the head and is freed exactly once.

### A4. Polynomial invariants and algebraic normal form

The polynomial representation enforces a normal form: exponents are unique and strictly decreasing and coefficients that are near zero are eliminated. `add_term` is the single operation that enforces this normal form, therefore every higher level operator is correct if it can be reduced to repeated applications of `add_term`.

An instructive way to phrase the invariant is:

For any exponent `e >= 0` there exists at most one term in the list with exponent `e` and the coefficient of that term is not within `EPSILON` of zero.

Once this holds, polynomial equality is structural equality rather than algebraic equivalence.

## Appendix B: Command files as executable specifications

Command files serve two roles: they are human readable scenarios and they are machine executable specifications. The benefit is that correctness can be assessed without interactive input.

### B1. Exercise 1 minimal scenario

Input (`tests/test1_input.txt`):

```text
ADD 1003 Charlie 3.50
ADD 1001 Alice 3.75
ADD 1002 Bob 3.85
DISPLAY
FIND 1002
DELETE 1001
DISPLAY
COUNT
```

Observations:

- the insertion order is deliberately not sorted
- the first display therefore validates the sort invariant
- the delete operation removes the head which validates the head special case

### B2. Exercise 2 minimal scenario

Input (`tests/test2_input.txt`):

```text
POLY P: 2 2 3 1 -1 0
DISPLAY P
POLY Q: 1 1 2 0
DISPLAY Q
ADD P Q
EVAL P 1
EVAL P 2
```

The `POLY` command provides a compact representation that is unambiguous for a parser. A deliberate consequence is that the input file is insensitive to whitespace variations, while the output is intentionally strict.

## Appendix C: Portability notes

### C1. Relative paths

The repository assumes that the current working directory is the repository root when running the examples as written. If you run `./exercise1` from another directory then `tests/test1_input.txt` is no longer a valid relative path. In automated assessment environments, the working directory is usually set explicitly, therefore this design is a feature rather than an inconvenience.

### C2. Floating point formatting

In `exercise2` the file mode prints evaluation results with two digits after the decimal point. This is a formatting contract rather than a numerical claim. Computation uses double precision and comparisons against zero use an `EPSILON` threshold, because the result of repeated arithmetic operations is generally not exactly representable in binary floating point.

# Week 04 Homework: Linked Structures in C

## Administrative parameters

- **Deadline**: end of Week 05
- **Weight**: 100 points (10% of the final grade)
- **Language and standard**: C (C11)
- **Compilation contract**: `gcc -Wall -Wextra -std=c11` for all targets and `-lm` where `<math.h>` is required

The homework is designed to make you demonstrate that you can reason about pointer based invariants, implement them safely and communicate the design choices. Your primary deliverable is working code whose behaviour matches the specification. Your secondary deliverable is a concise technical note that explains the invariants you maintain, the algorithms you chose and the tests you ran.

## Submission artefacts

Submit the following source files:

1. `homework1_playlist.c`
2. `homework2_sparse.c`

Also include a short `README.txt` (plain text) that covers:

- how to compile and run your code
- a description of your representation invariants
- a brief complexity discussion for the main operations
- a list of additional tests beyond the provided examples

## Global programming contract

### Memory ownership and lifetime

- Every node that you allocate with `malloc` must be freed exactly once.
- After freeing a node you must not access any field of that node.
- Functions that return pointers must document whether ownership is transferred to the caller.

### Numerical comparisons

Where floating point values are involved, treat values with absolute magnitude smaller than a small threshold `EPSILON` as zero. You must define and use a single `EPSILON` constant consistently.

### Determinism and randomness

If you introduce randomised behaviour (for example in playlist shuffling) you must document how you seed the pseudo random number generator. For testing you should be able to force deterministic behaviour by using a fixed seed.

### Output formatting

Output does not need to match a specific aesthetic style but it must be:

- unambiguous
- stable across runs for deterministic modes
- sufficiently documented that an assessor can interpret it without reading the source code

## Homework 1: Music Playlist Manager (50 points)

### Problem statement

Implement a playlist manager backed by a **doubly linked list**. The list represents a sequence of songs and the programme maintains a *current* pointer that models the song that is currently playing.

The playlist must support:

- insertion at both ends
- insertion after a named song
- removal by title
- sequential navigation with wrap around
- reversing the list
- shuffling the list
- formatted display of the full playlist and the current song

### Data structures

You must implement at least the following structures.

```c
#define MAX_TITLE  100
#define MAX_ARTIST 50

typedef struct Song {
    char title[MAX_TITLE];
    char artist[MAX_ARTIST];
    int duration_seconds;
    struct Song *prev;
    struct Song *next;
} Song;

typedef struct Playlist {
    Song *head;
    Song *tail;
    Song *current;
    char name[MAX_TITLE];
    int total_songs;
} Playlist;
```

### Representation invariants

You are expected to state these invariants in comments and preserve them in every operation.

1. If the playlist is empty then `head == NULL`, `tail == NULL`, `current == NULL` and `total_songs == 0`.
2. If the playlist is non empty then `head->prev == NULL` and `tail->next == NULL`.
3. For any node `u`, if `u->next == v` then `v->prev == u`.
4. `total_songs` equals the number of reachable nodes when traversing from `head` by repeated `next` pointers.
5. `current` is either `NULL` (empty playlist) or points to a node that is reachable from `head`.

### Required operations and semantics

#### 1. Node construction

- `Song* create_song(const char *title, const char *artist, int duration_seconds)`

Requirements:

- allocate a node and copy bounded strings
- reject invalid durations (non positive values)

Pseudocode:

```
create_song(title, artist, duration):
    if duration <= 0: return NULL
    s <- malloc(sizeof(Song))
    if s == NULL: return NULL
    copy title and artist with explicit null termination
    s.prev <- NULL
    s.next <- NULL
    s.duration_seconds <- duration
    return s
```

#### 2. Insertions

- `add_song_end`
- `add_song_beginning`
- `insert_song_after`

Asymptotic expectations:

- insertion at ends should be `O(1)` because you maintain both `head` and `tail`
- insertion after a given title is `O(n)` because it requires a search

Edge cases you must handle:

- inserting into an empty playlist must set `head`, `tail` and `current`
- inserting after the tail must update `tail`

#### 3. Removal

- `remove_song(playlist, title)`

The operation must:

- remove exactly one matching song (the first match is sufficient)
- update `head` and `tail` when removing boundary nodes
- update `current` when the removed song is the current song

Pseudocode (schematic):

```
remove_song(list, title):
    u <- find node with given title
    if u == NULL: return false
    if u == head: head <- u.next
    if u == tail: tail <- u.prev
    if u.prev != NULL: u.prev.next <- u.next
    if u.next != NULL: u.next.prev <- u.prev
    if current == u: current <- (u.next != NULL ? u.next : u.prev)
    free(u)
    total_songs--
    if total_songs == 0: current <- NULL
    return true
```

#### 4. Playback navigation

- `play_next` moves `current` to `current->next` and wraps to `head` when at the tail.
- `play_previous` moves `current` to `current->prev` and wraps to `tail` when at the head.

These operations must be `O(1)`.

#### 5. Reverse

- `reverse_playlist`

Reversal must be done in place by swapping `prev` and `next` for every node and then swapping the `head` and `tail` pointers.

Pseudocode:

```
reverse(list):
    u <- head
    while u != NULL:
        swap(u.prev, u.next)
        u <- u.prev   # because prev was next before the swap
    swap(head, tail)
```

#### 6. Shuffle

- `shuffle_playlist`

A robust approach is to extract pointers to nodes into an array, apply the Durstenfeld variant of Fisher Yates in place and then rebuild the links. The asymptotic cost is `O(n)` time and `O(n)` additional memory.

Pseudocode:

```
shuffle(list):
    if n < 2: return
    A <- array of Song* of length n
    fill A by traversal
    for i from n-1 down to 1:
        j <- random integer in [0, i]
        swap(A[i], A[j])
    rebuild prev/next links in the order of A
    head <- A[0]
    tail <- A[n-1]
```

### Marking scheme (50 points)

- Data structures and invariants: 10
- Core operations (insert, remove, find): 20
- Playback operations, reverse and shuffle: 15
- Display, formatting and memory hygiene: 5

## Homework 2: Sparse Matrix Operations (50 points)

### Problem statement

Implement a sparse matrix representation using linked structures so that matrices with many zeros are stored efficiently. You will represent only non zero elements and you must support basic linear algebra operations.

### Required representation

You must implement a sparse matrix with row and column access, typically via cross linked lists:

```c
typedef struct MatrixNode {
    int row;
    int col;
    double value;
    struct MatrixNode *next_in_row;
    struct MatrixNode *next_in_col;
} MatrixNode;

typedef struct SparseMatrix {
    int rows;
    int cols;
    MatrixNode **row_heads;
    MatrixNode **col_heads;
    int nonzero_count;
} SparseMatrix;
```

### Representation invariants

1. For each row `r`, `row_heads[r]` points to a list sorted by increasing column index.
2. For each column `c`, `col_heads[c]` points to a list sorted by increasing row index.
3. Every stored node appears in exactly one row list and exactly one column list.
4. `nonzero_count` equals the total number of stored nodes.

These invariants are the sparse matrix analogue of sortedness in `exercise1` and normal form in `exercise2`.

### Core operations

#### 1. Creation

- `create_sparse_matrix(rows, cols)` allocates the structure and initialises the row and column head arrays.

Complexity: `O(rows + cols)` for initialisation.

#### 2. Element update

- `set_element(matrix, row, col, value)` inserts or updates a node.

Semantics:

- if `abs(value) < EPSILON`, the element is treated as zero and any existing node at that position must be removed
- otherwise, if a node exists, update its value
- otherwise, create a new node and insert it into both the row list and the column list while preserving sortedness

The critical engineering detail is that insertion and removal must update *both* linked dimensions.

Pseudocode (schematic):

```
set_element(M, r, c, v):
    if abs(v) < EPSILON:
        remove node (r,c) from row r list and column c list if present
        return
    if node (r,c) exists:
        node.value <- v
        return
    node <- allocate
    insert node into row list r in sorted position by col
    insert node into col list c in sorted position by row
    nonzero_count++
```

#### 3. Query

- `get_element(matrix, row, col)` should return 0 when no node is stored.

A straightforward approach is to traverse the row list `row_heads[row]` until the column index matches or exceeds the target.

#### 4. Loading from file

Matrix file format:

```text
<rows> <cols>
<r0> <c0> <v0>
<r1> <c1> <v1>
...
```

Each triplet defines a non zero element. You must validate bounds and you must either reject duplicates or treat them as updates.

### Matrix operations

You must implement at least:

- addition `C = A + B`
- multiplication `C = A * B`
- transpose
- scalar multiplication

#### Addition

A safe pattern is to create `C` empty, then insert all terms of `A` and then insert all terms of `B` using `set_element` with update semantics.

Complexity depends on traversal order and on the cost of insertion into the result, but for matrices with `k` total non zero elements the overall time is typically `O(k * alpha)` where `alpha` is the average insertion traversal length in the affected row.

#### Multiplication

A sparse multiplication strategy should exploit sparsity:

- for each row `i` in `A`, traverse its non zero elements `(i, k, a_ik)`
- for each such element, traverse column `k` in `B` or row `k` in `B` depending on how you organise access
- accumulate contributions into the result

A simple approach that remains linked list based is:

```
for each row i in A:
    for each node a in row i:
        for each node b in row a.col in B:
            C[i, b.col] += a.value * b.value
```

This is not asymptotically optimal in all cases but it illustrates the key idea: iterate only over non zero entries.

### Display and analysis utilities

You must provide:

- `display_sparse` that prints only stored elements in `(row, col) = value` form
- `display_dense` for small matrices (print all entries)
- `sparsity_ratio` defined as the percentage of entries that are zero

### Marking scheme (50 points)

- Data structures and invariants: 10
- Correctness of update and query operations: 15
- Correctness of matrix operations: 15
- Display functions and analysis utilities: 5
- Memory safety and robustness: 5

## Penalties

- Compiler warnings: minus 10 points
- Memory leaks or invalid memory accesses: minus 20 points
- Crashes on valid input: minus 30 points
- Plagiarism: automatic fail

## Suggested test plan

A minimal but adequate test plan should include:

1. empty structures (no songs, no non zero entries)
2. singleton structures
3. head and tail operations
4. removal of the currently playing song
5. repeated insert update cycles for the same matrix position
6. algebraic identities such as `A + 0 = A` and `A * I = A` for small matrices

## References

| Topic | Reference (APA 7th ed) | DOI |
|---|---|---|
| Random permutation algorithm | Durstenfeld, R. (1964). Algorithm 235: Random permutation. *Communications of the ACM, 7*(7), 420. | https://doi.org/10.1145/364520.364540 |
| Sparse matrix benchmark collection | Davis, T. A. and Hu, Y. (2011). The University of Florida sparse matrix collection. *ACM Transactions on Mathematical Software, 38*(1), Article 1. | https://doi.org/10.1145/2049662.2049663 |


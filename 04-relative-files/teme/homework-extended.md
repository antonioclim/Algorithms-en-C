# Extended Challenges for Week 04

## Rationale

The core week focuses on singly linked lists and on algebraic linked representations (sparse polynomials). The optional challenges extend these ideas into data structures that are common in systems software, databases and performance critical applications. The intent is not only to implement the structures but also to justify invariants, analyse complexity and design tests that expose pointer handling errors.

Each completed challenge can earn **up to 10 bonus points**. A solution is credited only if it is correct, memory safe and documented with clear representation invariants and test evidence.

## Global implementation contract

- Implement in C11.
- Use `-Wall -Wextra -std=c11` and treat warnings as errors during development.
- Manage ownership explicitly: every allocation must have a corresponding free.
- Document invariants at the top of each source file.
- Provide a non interactive test harness or a command file mode that demonstrates each feature.

## Challenge 1: LRU cache (doubly linked list plus hash table)

### Concept

An LRU cache retains a fixed number of key value pairs. On every successful access the accessed item becomes the most recently used. When capacity is exceeded, the least recently used item is evicted.

The conventional constant time design is:

- a **doubly linked list** that stores items ordered by recency (head = most recent, tail = least recent)
- a **hash table** from key to list node

The list gives O(1) removal and O(1) insertion at the head, while the hash table gives O(1) expected lookup.

### Data structures

```c
typedef struct CacheNode {
    int key;
    int value;
    struct CacheNode *prev;
    struct CacheNode *next;
} CacheNode;

typedef struct HashEntry {
    int key;
    CacheNode *node;
    struct HashEntry *next; /* chaining */
} HashEntry;

typedef struct LRUCache {
    int capacity;
    int size;
    CacheNode *head;
    CacheNode *tail;
    HashEntry **table;
    int table_size;
} LRUCache;
```

### Core invariants

- `head->prev == NULL` and `tail->next == NULL` when the list is non empty
- every node in the list has a corresponding hash entry and vice versa
- `size` equals the number of nodes reachable from `head`

### Pseudocode

Lookup with promotion:

```
function get(cache, key):
    node <- table_lookup(key)
    if node == NULL: return MISS
    detach(node)
    attach_front(node)
    return node.value
```

Insertion with eviction:

```
function put(cache, key, value):
    node <- table_lookup(key)
    if node != NULL:
        node.value <- value
        detach(node)
        attach_front(node)
        return

    if cache.size == cache.capacity:
        victim <- cache.tail
        detach(victim)
        table_delete(victim.key)
        free(victim)
        cache.size--

    node <- allocate(key, value)
    attach_front(node)
    table_insert(key, node)
    cache.size++
```

### Testing focus

- eviction order under repeated access
- update of existing keys without increasing size
- integrity of `prev` and `next` links after every operation

## Challenge 2: Skip list (probabilistic multi level linked structure)

### Concept

A skip list maintains multiple linked levels. Level 1 is an ordinary sorted linked list. Higher levels contain a subset of keys that act as express lanes. Search proceeds from the top level and drops levels when it would overshoot.

Expected search, insertion and deletion cost is O(log n) under a geometric level distribution.

### Structures

```c
#define MAX_LEVEL 16

typedef struct SkipNode {
    int key;
    int value;
    struct SkipNode **forward;
    int level;
} SkipNode;

typedef struct SkipList {
    SkipNode *header;
    int level;
    int size;
} SkipList;
```

### Pseudocode (search)

```
function search(list, key):
    x <- list.header
    for i from list.level downto 1:
        while x.forward[i] != NULL and x.forward[i].key < key:
            x <- x.forward[i]
    x <- x.forward[1]
    if x != NULL and x.key == key: return x.value
    else: return NOT_FOUND
```

### Testing focus

- preservation of sorted order at every level
- deletion of head keys and tail keys
- deterministic tests by seeding the level generator

## Challenge 3: Circular buffer implemented as a circular linked list

### Concept

A circular buffer provides bounded storage with constant time insertion and removal. In array based designs, wrap around is handled by modular arithmetic. In linked designs, wrap around is represented by a cycle in pointers.

### Invariants

- when buffer is empty: `count == 0` and read pointer equals write pointer
- when buffer is full: `count == capacity`
- the underlying list has exactly `capacity` nodes and is circular

### Pseudocode (write)

```
function write(buf, value):
    if buf.count == buf.capacity:
        if buf.overwrite_mode == 0: return FULL
        else:
            buf.read_ptr <- buf.read_ptr.next
            buf.count--

    buf.write_ptr.data <- value
    buf.write_ptr <- buf.write_ptr.next
    buf.count++
    return OK
```

### Testing focus

- wrap around behaviour after more than `capacity` writes
- overwrite mode semantics

## Challenge 4: Self organising list (adaptive reordering)

### Concept

A self organising list attempts to reduce average search cost by reordering elements based on access. Three classical strategies are:

- move to front
- transpose
- count (order by access frequency)

### Pseudocode (move to front)

```
function search_mtf(head, key):
    if head == NULL: return NULL
    if head.key == key: return head
    prev <- head
    cur <- head.next
    while cur != NULL:
        if cur.key == key:
            prev.next <- cur.next
            cur.next <- head
            head <- cur
            return head
        prev <- cur
        cur <- cur.next
    return NULL
```

### Testing focus

- comparisons counted before and after reordering
- stability of transpose strategy under repeated accesses

## Challenge 5: Multi level undo and redo history (linked operations)

### Concept

Undo systems are histories of operations where each operation stores enough information to invert itself. A common representation is a doubly linked list of operations with a cursor that marks the current position.

### Invariants

- the history list is linear (no cycles)
- `current` always points to the operation that produced the current document state
- if a new operation is executed when not at the end of history then all redo nodes after `current` are discarded and freed

### Pseudocode (undo)

```
function undo(sys):
    if sys.current == NULL: return FAIL
    apply_inverse(sys.current)
    sys.current <- sys.current.prev
    return OK
```

### Testing focus

- redo chain reset after branching edits
- memory reclamation of discarded redo nodes

## Submission format

Provide one C source file per challenge (`challenge1_lru.c` and so on). Each file must include:

- a short header comment that states representation invariants
- a `main` function that runs a deterministic test scenario
- a `free_*` function that frees every allocation

## Appendix: Design notes and recurrent pitfalls

The challenges are deliberately phrased at the interface level. The following notes make several implicit constraints explicit so that you can design an implementation that remains correct under stress.

### A1. Avoiding accidental cycles

Cycles are a frequent source of non terminating traversals when nodes are re linked. A robust approach is to state a cycle freedom invariant and then validate it during testing using Floyd's tortoise and hare algorithm.

Pseudocode:

```
function has_cycle(head):
    slow <- head
    fast <- head
    while fast != NULL and fast.next != NULL:
        slow <- slow.next
        fast <- fast.next.next
        if slow == fast:
            return true
    return false
```

This check is cheap enough for debug builds and catches a large class of pointer errors.

### A2. Deterministic randomness

Several challenges use randomisation (skip list levels and playlist or cache shuffling). For assessment you should prefer deterministic runs. The simplest technique is to seed the pseudo random number generator with a fixed constant in `main` and to allow an optional command line seed for exploratory testing.

### A3. Sentinel nodes and simplifying edge cases

Many linked structures become simpler if you introduce sentinel nodes (also called dummy header or dummy tail nodes). A sentinel is not a real element but it makes empty structure handling uniform because operations no longer need to special case `NULL` at the boundary. The trade off is that you must ensure the sentinel is never exposed via the public interface.

### A4. Complexity is a design constraint not an afterthought

For each challenge you should articulate the target complexity and then derive the representation from it. As an example, an LRU cache that uses only a list cannot guarantee `O(1)` access by key, therefore a hash table is not an optional embellishment but a necessary companion structure.

### A5. Micro test scenarios that isolate invariants

Design tests that isolate one invariant at a time. For instance:

- for an LRU cache, write a test that repeatedly accesses a single key and verify that no other key is evicted
- for a skip list, insert a strictly increasing sequence and verify that every forward pointer chain remains sorted
- for a self organising list, run a fixed 80/20 access pattern and compare the number of comparisons between strategies

### A6. Suggested empirical evaluation protocol

Where the challenge asks for analysis (for example the self organising list) you should separate *algorithmic cost* from *implementation overhead*. A simple protocol is:

1. fix a random seed and generate a dataset once
2. run each strategy on the identical access trace
3. record both the number of comparisons and the wall clock time
4. interpret time only as a secondary check because it depends strongly on compiler, platform and cache behaviour

### A7. Summary complexity table

The table below summarises the intended asymptotic behaviour when the recommended representations are used.

| Challenge | Representative operations | Intended average complexity |
|---|---|---|
| LRU cache | `get`, `put` | `O(1)` |
| Skip list | search, insert, delete | `O(log n)` |
| Circular buffer | read, write | `O(1)` |
| Self organising list | search | between `O(1)` and `O(n)` depending on locality |
| Undo system | undo, redo | `O(1)` per operation plus cost of applying edits |

## References

| Topic | Reference (APA 7th ed) | DOI |
|---|---|---|
| Skip lists | Pugh, W. (1990). Skip lists: A probabilistic alternative to balanced trees. *Communications of the ACM, 33*(6), 668-676. | https://doi.org/10.1145/78973.78977 |
| Paging and LRU analysis | Sleator, D. D. and Tarjan, R. E. (1985). Amortized efficiency of list update and paging rules. *Communications of the ACM, 28*(2), 202-208. | https://doi.org/10.1145/2786.2793 |
| Random permutation algorithm | Durstenfeld, R. (1964). Algorithm 235: Random permutation. *Communications of the ACM, 7*(7), 420. | https://doi.org/10.1145/364520.364540 |

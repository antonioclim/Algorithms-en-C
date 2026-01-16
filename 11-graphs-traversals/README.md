# Week 11: Hash Tables

## Context and pedagogical intent

This laboratory develops a disciplined understanding of hash tables as practical data structures and as probabilistic performance models. The aim is not merely to obtain a working programme but to build the capacity to justify design decisions under explicit assumptions about key distributions, memory constraints and adversarial inputs.

The repository contains three complementary strands:

- **A complete worked example** (`src/example1.c`) that demonstrates multiple hashing strategies, collision resolution policies, rehashing and instrumentation.
- **Exercise 1** (`src/exercise1.c`): a chained hash table for student records. The programme reads records from standard input and prints a deterministic transcript used by automated tests.
- **Exercise 2** (`src/exercise2.c`): an open addressing hash table using double hashing to compute word frequencies from a text file. The programme records probe statistics to connect empirical behaviour to the underlying model.

The code is written in ISO C11 and is intentionally explicit about memory ownership, failure modes and invariants. In C, correctness is frequently dominated by these concerns rather than by the high level algorithmic idea.

## Learning outcomes

On successful completion a student should be able to:

1. Define the hash table abstraction in terms of a key universe, a table domain and collision resolution semantics.
2. Implement and compare non-cryptographic string hash functions (djb2 and FNV-1a) and explain their structural properties and limitations.
3. Implement separate chaining and open addressing, stating representation invariants and describing how those invariants are preserved by each operation.
4. Compute and interpret the load factor, justify a rehashing threshold under a stated cost model and explain why thresholds differ between chaining and open addressing.
5. Relate theoretical expectations to empirical observations via probe statistics, chain-length distributions and controlled experiments.
6. Demonstrate correct memory management for dynamically stored keys and nodes including defensive error handling and total deallocation.

## File map

- `src/example1.c`   A comprehensive demonstration programme.
- `src/exercise1.c`  Chained hash table for student records.
- `src/exercise2.c`  Open addressing word frequency counter.
- `data/`            Input material used for demonstrations.
- `tests/`           Deterministic input and expected output for automated checks.
- `teme/`            Homework requirements and optional extended challenges.

## Conceptual foundations

### 1. The abstract model

Let:

- U be a universe of keys
- m be the table size (number of buckets or slots)
- h be a function mapping keys to indices in the range 0 to m minus 1

A hash table stores a finite set of key value associations and supports the operations:

- insert(k, v)
- find(k)
- delete(k)

The central difficulty is that h is not injective. Collisions are inevitable because the universe is large and the table is finite.

The guiding question is therefore not whether collisions occur but how the implementation behaves when they do.

### 2. Load factor and its role

The load factor is the ratio:

- alpha = n / m

where n is the number of stored entries.

Alpha is not a mere diagnostic. It is the primary control parameter in the expected cost model. For a fixed hash function and collision policy, increasing alpha increases the probability that a lookup encounters collisions.

For separate chaining, alpha may exceed 1 and performance degrades gradually. For open addressing, alpha must remain strictly below 1 and the degradation as alpha approaches 1 is severe.

### 3. Hash functions as mixing processes

A practical hash function for strings is typically a short recurrence that combines characters into an integer. In this laboratory two classic non-cryptographic functions are used:

- djb2 (Daniel J Bernstein): a polynomial accumulator with multiplier 33.
- FNV-1a (Fowler Noll Vo): xor then multiply by a prime.

Both are designed for speed and acceptable distribution on typical text keys. Neither is cryptographic and neither provides protection against targeted collision attacks.

When the table size is a power of two, the low bits of the hash dominate index selection if one uses index = hash mod m. In that regime hash functions with weak low-bit diffusion can produce clustering. Prime table sizes or additional mixing reduce this risk.

## Collision resolution strategies

### Strategy A: Separate chaining

**Representation.** Each bucket stores a pointer to the head of a linked list. Each node stores a key value pair and a pointer to the next node.

**Invariants.**

1. Every node is reachable from exactly one bucket.
2. Within a chain, each node belongs to keys whose hash index is that bucket.
3. The global count equals the total number of nodes across all buckets.

**Operational consequences.**

- Insertion can be implemented as insertion at the head of the chain.
- Successful search terminates as soon as the key is found.
- Deletion requires tracking the predecessor pointer.

Chaining is conceptually simple and has stable performance at moderately high load factors but it allocates per entry node overhead and it is sensitive to allocator behaviour and pointer chasing costs.

### Strategy B: Open addressing

**Representation.** All entries reside in a single array. A collision is resolved by probing alternative slots according to a probe sequence.

A probe sequence must satisfy two requirements:

1. It must be deterministic for a fixed key and table size.
2. It must visit sufficiently many slots to guarantee termination if the table has free space.

Open addressing requires careful deletion. If a key is removed and the slot is marked empty, probe chains for other keys may be broken and searches may become incorrect. The standard mitigation is to use tombstones: a deleted marker that indicates that probing must continue.

Open addressing typically improves locality of reference because the table is a contiguous array. It also has lower memory overhead for small entries but its performance is highly sensitive to alpha.

## Algorithms and pseudocode

The pseudocode in this section is written to mirror the C implementations closely. The goal is to make the state transitions explicit.

### 1. djb2 string hash

Properties:

- Simple recurrence
- Very fast
- Not cryptographic

Pseudocode:

```
function djb2(key):
    hash <- 5381
    for each byte c in key:
        hash <- hash * 33 + c
    return hash
```

In C, multiplication by 33 is often written as (hash shifted left by 5) plus hash because shifting is usually cheaper than multiplication.

### 2. FNV-1a string hash

Properties:

- Xor then multiply
- Good diffusion for many practical strings
- Widely used as a baseline

Pseudocode:

```
function fnv1a(key):
    hash <- 2166136261
    for each byte c in key:
        hash <- hash xor c
        hash <- hash * 16777619
    return hash
```

### 3. Separate chaining operations

#### Insert with update on duplicate key

```
function chain_insert(table, key, value):
    i <- hash(key) mod table.size

    node <- table.buckets[i]
    while node is not null:
        if node.key equals key:
            node.value <- value
            return i
        node <- node.next

    new <- allocate node
    new.key <- copy key
    new.value <- value
    new.next <- table.buckets[i]
    table.buckets[i] <- new
    table.count <- table.count + 1
    return i
```

#### Search

```
function chain_find(table, key):
    i <- hash(key) mod table.size
    node <- table.buckets[i]
    while node is not null:
        if node.key equals key:
            return node.value
        node <- node.next
    return not found
```

#### Delete

```
function chain_delete(table, key):
    i <- hash(key) mod table.size
    node <- table.buckets[i]
    prev <- null

    while node is not null:
        if node.key equals key:
            if prev is null:
                table.buckets[i] <- node.next
            else:
                prev.next <- node.next
            free node
            table.count <- table.count - 1
            return true
        prev <- node
        node <- node.next

    return false
```

### 4. Open addressing with double hashing

A double hashing probe sequence is:

- index_0 = h1(key) mod m
- step   = 1 + (h2(key) mod (m - 1))
- index_i = (index_0 + i * step) mod m

If m is prime then any step in the range 1 to m minus 1 is coprime to m and the probe sequence visits the full table.

#### Insert or increment

This version includes tombstones and instrumentation via a probe counter.

```
function oa_insert(table, key):
    if effective_load_factor(table) > threshold:
        rehash(table)

    index <- h1(key) mod table.size
    step  <- secondary_step(key, table.size)

    first_tombstone <- none
    probes <- 0

    loop:
        probes <- probes + 1
        slot <- table.entries[index]

        if slot.state is EMPTY:
            target <- if first_tombstone exists then first_tombstone else index
            insert key into table.entries[target]
            record probes
            return probes

        if slot.state is DELETED:
            if first_tombstone does not exist:
                first_tombstone <- index
        else:
            if slot.key equals key:
                slot.count <- slot.count + 1
                record probes
                return probes

        index <- (index + step) mod table.size
```

#### Search

```
function oa_search(table, key):
    index <- h1(key) mod table.size
    step  <- secondary_step(key, table.size)
    probes <- 0

    while true:
        probes <- probes + 1
        slot <- table.entries[index]

        if slot.state is EMPTY:
            return not found

        if slot.state is OCCUPIED and slot.key equals key:
            return slot

        index <- (index + step) mod table.size
```

#### Delete

```
function oa_delete(table, key):
    index <- h1(key) mod table.size
    step  <- secondary_step(key, table.size)

    while true:
        slot <- table.entries[index]

        if slot.state is EMPTY:
            return false

        if slot.state is OCCUPIED and slot.key equals key:
            free slot.key
            slot.state <- DELETED
            table.count <- table.count - 1
            table.tombstones <- table.tombstones + 1
            return true

        index <- (index + step) mod table.size
```

### 5. Rehashing and amortised cost

Rehashing replaces the existing table with a larger table and reinserts all occupied entries. The direct cost is linear in n because each entry is moved.

Under the standard strategy of doubling the table size when alpha exceeds a constant threshold the amortised cost of insertion remains constant. Intuitively, the expensive linear rebuild occurs only after a linear number of successful inserts.

A practical caveat is that open addressing with tombstones benefits from rehashing even if alpha is not high because accumulated tombstones increase the effective load factor and thereby increase search cost.

## Implementation notes for this repository

### 1. `src/example1.c`

The example programme is designed as a reference implementation rather than as an assessment artefact. It includes:

- multiple hash functions including djb2 and FNV-1a
- separate chaining and open addressing
- explicit load factor tracking
- optional rehashing logic
- basic performance measurement

Students should treat it as a source of implementation patterns, particularly with respect to defensive allocation and clean deallocation.

### 2. `src/exercise1.c` (chaining)

The student record is stored as a fixed-size structure to keep the focus on pointer manipulation rather than on deep-copy string management.

Key properties:

- The table stores nodes allocated with `malloc`.
- Each bucket is a singly linked list.
- Duplicate IDs update in place.
- The programme prints a deterministic transcript that can be diffed line by line.

The function `ht_print_all_sorted` collects pointers to all stored students, sorts them by ID and prints them. This is not required for correctness but it decouples display order from bucket order which makes testing and manual inspection easier.

### 3. `src/exercise2.c` (open addressing)

The word frequency counter uses a conservative tokeniser: only contiguous alphabetic sequences are treated as words. Each word is lowercased and truncated to a fixed maximum length to protect against unbounded input sequences.

Key properties:

- Keys are heap-allocated strings owned by the table.
- Deletion uses tombstones.
- Rehashing is triggered by effective load factor, defined as (occupied + tombstones) / size.
- Probe instrumentation records the number of slot inspections performed by each insert operation.

The table is initialised to size 127, a prime that provides good behaviour for double hashing.

## Building and testing

From the repository root:

```bash
make all
make test
```

Useful interactive targets:

```bash
make run-example
make run-ex1
make run-ex2
```

For memory and undefined behaviour diagnostics on a development system that supports sanitizers:

```bash
gcc -std=c11 -Wall -Wextra -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -o ex2 src/exercise2.c
./ex2 tests/test2_input.txt
```

## Practical guidance and typical errors

1. **Key ownership**: a hash table that stores string keys must either copy keys or guarantee that the caller provides stable storage. In this repository open addressing copies keys into owned heap storage. Chaining in Exercise 1 stores fixed arrays inside nodes.
2. **Do not free borrowed pointers**: if the table does not own a pointer it must not free it. Establish ownership rules before coding.
3. **Deletion in open addressing**: marking a deleted slot as empty is a correctness bug because it may stop a search early. Use tombstones.
4. **Secondary step must be non-zero**: if the step is 0 then the probe sequence never advances. The secondary hash must be mapped to the range 1 to m minus 1.
5. **Probe statistics must have a definition**: decide whether a probe counts an inspected slot, an extra displacement beyond the first slot or something else. Record that definition alongside the numbers.

## Cross-language perspective

Hash tables are ubiquitous in high level languages. Connecting the C implementation details to those abstractions helps clarify what the language runtime is doing.

- **Python** uses a dictionary type that employs open addressing with perturbation and resizing rules.
- **Java** uses `HashMap` which historically used separate chaining and in modern versions may treeify buckets under heavy collision.
- **C++** provides `unordered_map` with implementation-defined collision strategy.

In C, none of these policies are hidden. You decide them, you implement them and you therefore become responsible for their consequences.

## Further reading

The modern theory of hashing includes universal hashing, dynamic perfect hashing and defences against hash-flooding attacks. The implementations in this week are deliberately non-cryptographic because the focus is data structure mechanics. If a production system must be robust under adversarial input then the choice of hashing scheme becomes a security decision rather than a purely performance decision.

### 3. Hash functions as computable surjections

A string hash function maps an arbitrarily long byte sequence to a fixed width integer. In C implementations this integer is usually an unsigned 32-bit quantity, either by explicit type choice or by overflow semantics. The table index is obtained by reduction modulo m or by a bit mask when m is a power of two.

A useful pedagogical separation is:

1. A mixing stage that produces a machine-size hash value H(k)
2. A reduction stage that maps H(k) to a table index i in 0..m-1

Reduction by modulo has a clear mathematical meaning but its interaction with the low bits of H(k) matters. If m is a power of two then only the low log2(m) bits of H(k) contribute to the index. A hash function whose low bits are poorly mixed will then produce clustering. This is why many textbook discussions recommend either prime m or hash functions with demonstrably strong low-bit diffusion.

The present week uses two widely deployed non-cryptographic hashes to illustrate these issues:

- djb2, a polynomial hash with multiplier 33
- FNV-1a, a multiply XOR hash with a prime multiplier and an offset basis

Neither is cryptographic. They are suitable for pedagogy and for benign workloads but they are not suitable as a defence against adversarially chosen keys.

#### 3.1 djb2

djb2 can be written as:

- initialise hash to 5381
- for each byte c: hash = hash * 33 + c

Because multiplication by 33 is multiplication by 32 plus 1, an efficient implementation uses a shift and an addition.

Pseudocode:

```
function DJB2(key_bytes):
    h := 5381
    for each byte c in key_bytes:
        h := (h << 5) + h
        h := h + c
    return h
```

Key observations:

- The function is fast because it uses simple integer operations.
- It tends to perform well on common identifier-like strings.
- It does not provide any security guarantees.

#### 3.2 FNV-1a

FNV-1a is constructed as repeated XOR and multiplication by a fixed prime.

Pseudocode:

```
function FNV1A(key_bytes):
    h := 2166136261
    for each byte c in key_bytes:
        h := h XOR c
        h := h * 16777619
    return h
```

Compared to djb2, FNV-1a is often described as having good avalanche properties in practice for short strings although it remains non-cryptographic.

### 4. Collision resolution strategies

The laboratory focuses on two collision resolution policies because they exhibit complementary trade-offs and because they force different invariants.

#### 4.1 Separate chaining

Representation:

- The table is an array of m pointers
- Each pointer refers to a singly linked list of nodes
- Each node stores one key value pair

Invariant:

- For each occupied node x stored in bucket b, hash(x.key) reduced modulo m equals b

Operations:

Insertion pseudocode:

```
procedure CHAIN_INSERT(table, key, value):
    b := REDUCE(HASH(key), table.size)
    for node in table.bucket[b]:
        if node.key == key:
            node.value := value
            return
    new_node := allocate node
    new_node.key := copy(key)
    new_node.value := value
    new_node.next := table.bucket[b]
    table.bucket[b] := new_node
    table.count := table.count + 1
```

Search pseudocode:

```
function CHAIN_FIND(table, key):
    b := REDUCE(HASH(key), table.size)
    node := table.bucket[b]
    while node != null:
        if node.key == key:
            return node
        node := node.next
    return null
```

Deletion pseudocode:

```
function CHAIN_DELETE(table, key):
    b := REDUCE(HASH(key), table.size)
    prev := null
    node := table.bucket[b]
    while node != null:
        if node.key == key:
            if prev == null:
                table.bucket[b] := node.next
            else:
                prev.next := node.next
            free(node)
            table.count := table.count - 1
            return true
        prev := node
        node := node.next
    return false
```

The key step in deletion is the maintenance of the singly linked list structure. The special case is deletion of the head element.

Expected cost under a uniform hashing assumption:

- Expected chain length is alpha
- Successful search inspects about 1 plus alpha over 2 nodes
- Unsuccessful search inspects about 1 plus alpha nodes

These expressions follow by modelling the bucket occupancy as a balls-into-bins process.

#### 4.2 Open addressing with double hashing

Representation:

- The table is an array of m slots
- Each slot is in one of three states: EMPTY, OCCUPIED or DELETED
- Each occupied slot stores one key value pair

Core invariant:

- If a key k is present then it appears somewhere along the probe sequence defined by k and the current table size

The fundamental algorithmic idea is that a collision is resolved by moving to a different slot determined by a probing rule.

Double hashing defines a probe sequence:

- h1 is the primary hash reduced modulo m
- h2 is the step size, constrained to be non-zero and to be coprime to m
- the ith probe visits (h1 + i * h2) modulo m

Pseudocode for search under tombstones:

```
function OA_FIND(table, key):
    i := REDUCE(H1(key), table.size)
    step := STEP(H2(key), table.size)
    probes := 0
    while true:
        probes := probes + 1
        slot := table[i]
        if slot.state == EMPTY:
            return (null, probes)
        if slot.state == OCCUPIED and slot.key == key:
            return (slot, probes)
        i := (i + step) modulo table.size
```

Insertion must handle three cases: the key is already present, an empty slot is found or a tombstone is found.

Pseudocode:

```
procedure OA_INSERT(table, key, value):
    if EFFECTIVE_LOAD_FACTOR(table) > threshold:
        REHASH(table)

    i := REDUCE(H1(key), table.size)
    step := STEP(H2(key), table.size)
    first_tombstone := none
    probes := 0

    while true:
        probes := probes + 1
        slot := table[i]

        if slot.state == EMPTY:
            target := first_tombstone if defined else i
            table[target] := (key, value, OCCUPIED)
            table.count := table.count + 1
            record probes
            return

        if slot.state == DELETED and first_tombstone is none:
            first_tombstone := i

        if slot.state == OCCUPIED and slot.key == key:
            slot.value := UPDATE(slot.value, value)
            record probes
            return

        i := (i + step) modulo table.size
```

Deletion sets state to DELETED rather than EMPTY. This preserves the search invariant: an element further along the probe chain must remain reachable.

### 5. Rehashing and amortised analysis

A rehash operation creates a new table, typically larger and reinserts all occupied entries. Its worst-case time is linear in n, the number of stored entries, because each entry must be moved. If the table size grows geometrically then this cost can be amortised over many insertions.

A standard potential argument is:

- Suppose the table doubles when the effective load factor exceeds a constant c less than 1
- Between two rehash events, at least proportional to the new capacity insertions occur
- Therefore the amortised insertion cost remains constant even though rehash is linear

In practice, the constants matter. Open addressing becomes sharply inefficient as alpha approaches 1 because probe sequences lengthen rapidly. This is why the threshold for open addressing is typically around 0.65 to 0.75 whereas chaining can tolerate higher alpha, provided memory overhead is acceptable.

## Implementation notes for this repository

### 1. Worked example: src/example1.c

The worked example is a demonstration programme rather than a test target. It contains several components that are useful as reference material when implementing the exercises and homework:

- multiple string hash functions
- a chained hash table with full CRUD
- an open addressing table with double hashing
- load factor monitoring and rehashing
- empirical timing and statistics collection

The file is intentionally long because it aims to show an end-to-end style that includes diagnostics and safe cleanup rather than only the core algorithm.

### 2. Exercise 1: src/exercise1.c

Exercise 1 implements separate chaining for student records.

Data structures:

- Student: fixed-size arrays for id and name together with a floating-point grade
- HashNode: a node storing a Student and a next pointer
- HashTable: an array of bucket heads together with size and count

Design choices:

- Fixed-size arrays avoid per-field allocation and simplify ownership.
- Insertion updates an existing record if a duplicate id is encountered.
- Printing of all records is performed in sorted order by id to produce a deterministic transcript independent of bucket placement.

Input and output contract:

- Input is read from standard input as lines of: id name grade
- Output is a deterministic transcript used by the tests in tests/test1_expected.txt

### 3. Exercise 2: src/exercise2.c

Exercise 2 implements open addressing with double hashing for word frequency counting.

Tokenisation:

- Words are maximal contiguous sequences of alphabetic characters
- Each alphabetic character is mapped to lowercase
- The parser caps word length to MAX_WORD_LEN minus one characters

The tokenisation strategy is character-driven rather than whitespace-driven because it gives predictable behaviour in the presence of punctuation.

Probe statistics:

- Each insert operation records the number of inspected slots including the first slot
- Total probes is the sum over all processed word tokens
- The average probes per operation is total probes divided by total operations

This instrumentation is not part of the abstract specification of a hash table but it is essential when relating theoretical expectations to the behaviour observed on concrete key streams.

## Building, running and testing

The Makefile defines targets that mirror the intended workflow.

- Build all targets:

```
make all
```

- Run Exercise 1 with the supplied dataset:

```
make run-ex1
```

- Run Exercise 2 with the supplied dataset:

```
make run-ex2
```

- Run the deterministic tests:

```
make test
```

The tests compare programme output against expected transcripts. If you change the printed transcript you must update the expected files accordingly. In production systems one would typically test semantic properties rather than exact formatting but transcript-based tests are useful for introductory laboratories because they make the control flow explicit.

## Engineering checklist

Before submitting coursework based on this repository, confirm the following.

1. All allocations are checked for failure.
2. All owned memory is freed exactly once.
3. Keys stored in the table are owned by the table, not borrowed from temporary buffers.
4. Deletion in open addressing uses tombstones, not clearing to empty.
5. The secondary hash never yields a step of zero.
6. Any rehash procedure transfers ownership safely and does not leak the previous key allocations.

## Common implementation pitfalls

1. Borrowed pointers: storing a pointer to a transient buffer as a key will eventually produce dangling references. In this repository keys are copied into owned storage.
2. Incorrect deletion in open addressing: setting a deleted slot to EMPTY breaks probe chains. Tombstones are required.
3. Step size equal to zero: in double hashing this yields an infinite loop. The step must be at least 1.
4. Non-coprime step: probe sequences may not cover the full table. Prime table sizes or odd steps for power-of-two sizes are standard mitigations.
5. Unbounded token growth: word parsers must cap buffer length and handle long sequences safely.

## Further reading

The wider theory of hashing includes universal hashing, dynamic perfect hashing and defences against algorithmic complexity attacks. The present week provides the conceptual tools required to read that literature critically.

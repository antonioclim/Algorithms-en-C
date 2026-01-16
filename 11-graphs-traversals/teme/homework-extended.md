# Extended Challenges: Week 11 Hash Tables

## Rationale

The core laboratory and homework focus on two canonical collision resolution strategies: separate chaining and open addressing with double hashing. In industrial systems these building blocks are often embedded within richer data structures that impose additional invariants such as recency ordering, eviction policies, concurrency requirements or worst-case lookup guarantees.

The extended challenges below are optional. They are intended for students who want to explore these richer settings in a controlled way. Each challenge is specified so that a correct solution can be reasoned about formally. You are encouraged to write small proofs of invariants in comments or in a short accompanying report.

Unless explicitly stated otherwise, assume ISO C11 and the same compilation flags as the laboratory.

## General requirements

1. Your solution must compile without warnings under -Wall -Wextra -std=c11.
2. All dynamic memory must be freed. Ownership rules must be explicit.
3. Every public operation must be total: it must behave deterministically for every valid input including empty strings and repeated updates.
4. If an allocation fails, the programme must exit cleanly or return an error code without leaking previously allocated memory.

## Challenge 1: LRU cache using a hash table and a doubly linked list

### Problem statement

Implement a fixed-capacity Least Recently Used cache. The cache stores key value pairs where keys are strings and values are integers. When the cache is full and a new key is inserted, the least recently accessed key must be evicted.

### Required interface

Implement at least the following functions:

- `LRUCache *lru_create(int capacity);`
- `int lru_get(LRUCache *cache, const char *key);`  Returns -1 if absent
- `void lru_put(LRUCache *cache, const char *key, int value);`
- `void lru_destroy(LRUCache *cache);`

### Data structure design

You must combine two components:

1. A hash table mapping `key` to a pointer to a node
2. A doubly linked list storing nodes ordered by recency

Each node must be present in both structures. The list head represents the most recently used element and the list tail represents the least recently used element.

### Representation invariants

At all times:

- Every node in the list is reachable from the hash table and vice versa
- The list contains no cycles and has consistent prev and next pointers
- The cache size equals the number of nodes and does not exceed capacity

### Algorithms

The key property is that both operations must be constant time on average:

- `get` performs a hash lookup then moves the node to the head of the list
- `put` either updates an existing node then moves it to head or inserts a new node at head and evicts the tail when required

Pseudocode for `get`:

1. node <- hash_find(key)
2. if node is null then return -1
3. detach node from its current position
4. insert node at head
5. return node.value

Pseudocode for `put`:

1. node <- hash_find(key)
2. if node exists then
   2.1 node.value <- value
   2.2 move node to head
   2.3 return
3. if cache is full then
   3.1 victim <- tail
   3.2 remove victim from list
   3.3 delete victim.key from hash table
   3.4 free victim
4. allocate new node and copy key
5. insert new node at head
6. insert key -> node into hash table

### Testing guidance

Construct tests that:

- Insert exactly capacity keys then insert one more and verify eviction
- Access a middle key and verify it becomes most recent
- Update an existing key and verify it is not duplicated
- Alternate insert and get operations to exercise pointer manipulation

## Challenge 2: Incremental rehashing to avoid latency spikes

### Motivation

Classic resizing reinserts all elements in a single step, which has O(n) time cost. In interactive systems this causes latency spikes. Incremental rehashing spreads the work across many operations.

### Specification

Extend a chained hash table to support incremental resizing. Maintain two tables: an old table and a new table. When resizing begins, insertions occur into the new table and a bounded amount of work is performed to migrate a small number of buckets from the old table to the new table.

### Core invariant

At any time each key exists in exactly one of the two tables. A lookup must search both tables until migration completes.

### Suggested mechanism

Maintain an integer `rehash_index` that marks how many buckets have been migrated from the old table.

On each operation perform:

- the operation itself
- then migrate K buckets (for example K equals 1 or 2) starting at `rehash_index`

When `rehash_index == old_size` the migration completes, the old table is freed and the new table becomes the active table.

### Pseudocode for migration step

Input: old_table, new_table, rehash_index, K

for t in 1..K:
  if rehash_index == old_size: stop
  take the list at old_table[rehash_index]
  set old_table[rehash_index] to null
  for each node in that list:
    insert node into new_table using new hash
  rehash_index <- rehash_index + 1

### Evaluation

A correct solution should show that no single user operation performs more than O(1 + alpha) expected work plus the bounded migration cost.

## Challenge 3: Robin Hood hashing for open addressing

### Motivation

In open addressing, performance is dominated not only by the average probe count but also by its variance. Robin Hood hashing reduces variance by equalising probe distances.

### Rule

On insertion, if the new key has probed further than the resident key in the current slot then the new key steals the slot and the displaced key continues probing.

### What you must implement

1. Store probe distance metadata for each occupied slot or recompute it on demand.
2. Implement insertion with the swap rule.
3. Adjust deletion strategy. Backward shifting deletion is one option but tombstones are also possible.
4. Record the distribution of probe distances before and after applying Robin Hood hashing.

### Pseudocode sketch for insertion

Let d(x) be the number of probes from x's home position to its current position.

index <- home(new_key)
dist <- 0
while slot[index] is occupied:
  if slot[index].key equals new_key then update and return
  if d(slot[index].key) < dist then
     swap(new_key, slot[index].key)
     swap(payload)
     dist <- d(slot[index].key)
  index <- next(index)
  dist <- dist + 1
insert new_key at slot[index]

## Challenge 4: Cuckoo hashing with two hash functions

### Motivation

Cuckoo hashing provides worst-case constant time lookups by ensuring each key is stored in one of two possible positions. Insertions may trigger a bounded relocation process.

### Requirements

- Implement two independent hash functions h1 and h2
- Each key may reside in position h1(k) or h2(k)
- On insertion, if both slots are occupied, evict one key and relocate it to its alternate position, repeating for a bounded number of steps
- If relocation exceeds a threshold, rebuild the table with new hash function seeds

This challenge is algorithmically richer than the others because it requires careful reasoning about cycles and rehash triggers.

## Challenge 5: A Bloom filter front end for membership queries

### Motivation

A Bloom filter is a compact probabilistic structure that supports membership queries with false positives but no false negatives. It is often used as a front end to reduce the number of expensive hash table lookups.

### Requirements

- Implement a bit array of size M
- Choose k independent hash functions derived from two base hashes using the standard double-hashing technique
- Support insert and query
- Empirically estimate the false positive rate on random data and compare it to the theoretical approximation

## Deliverable expectations

For any challenge you attempt provide:

1. Source code and a short driver programme demonstrating behaviour.
2. A small test suite that targets edge cases.
3. A short report describing invariants and complexity.

A strong submission will also include empirical measurements of probe counts, chain lengths or latency distributions.

## Challenge 4: Cuckoo hashing mini-study

### Motivation

Open addressing with probing has excellent cache behaviour but probe lengths can vary, particularly when the load factor increases. Cuckoo hashing is an alternative that maintains worst-case constant lookup time by storing each key in one of a small number of possible locations. Insertions may relocate existing keys and can occasionally fail, requiring a table rebuild.

### Requirements

1. Implement two independent hash functions h1 and h2 producing candidate positions.
2. Store each key in either position h1(k) or h2(k) and guarantee that lookups check at most two locations.
3. Implement insertion via bounded displacement. If the bound is exceeded, rebuild the table with a new size or new hash seeds.
4. Provide instrumentation: number of displacements per insertion and number of rebuilds.

### Suggested pseudocode

InsertCuckoo(k, v):

1. for t from 1 to MAX_DISPLACEMENTS:
   a. if slot h1(k) is empty then place (k, v) there and return success
   b. if slot h2(k) is empty then place (k, v) there and return success
   c. choose one of the two positions, swap (k, v) with the resident entry and set (k, v) to the displaced entry
2. return failure

A rebuild typically doubles the table size and reinserts all keys.

### Discussion

This challenge is not expected to be perfect. The learning objective is to reason about termination, to make the displacement bound explicit and to measure empirically how load factor influences rebuild frequency.

## Challenge 5: Bloom filter adjunct for negative lookups

### Motivation

In large workloads many lookups are negative. A Bloom filter is a probabilistic structure that can answer "definitely not present" quickly and can be used as a pre-filter before a hash table. False positives are possible but false negatives are not.

### Requirements

1. Implement a bit array of size B bits and a family of k hash functions.
2. Support operations add(key) and might_contain(key).
3. Integrate the Bloom filter into a hash table so that negative lookups can be rejected without probing the table.
4. Measure the false positive rate on a controlled dataset.

### Suggested pseudocode

AddToBloom(key):

1. for i from 1 to k:
   a. p <- hi(key) mod B
   b. set bit p

MightContain(key):

1. for i from 1 to k:
   a. p <- hi(key) mod B
   b. if bit p is 0 then return false
2. return true

## Challenge 6: String interning table

### Motivation

Compilers and interpreters often store each distinct identifier exactly once and represent subsequent occurrences by a pointer to the interned string. This reduces memory and accelerates comparisons.

### Requirements

1. Implement intern(const char *s) returning a stable pointer to an interned copy of s.
2. Guarantee that pointer equality corresponds to string equality for interned strings.
3. Provide reference counting or a global destroy function.
4. Demonstrate usage by tokenising a source-like input and interning each identifier.

## Marking guidance for extended work

If you submit any extended challenge, accompany it with a short EXTENDED.txt document describing:

- The additional invariants introduced and how they are preserved
- The empirical measurements you collected
- Limitations and failure cases

A strong submission is one that is honest about constraints and demonstrates careful testing.

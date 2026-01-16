# Week 11 Homework: Hash Tables

## Purpose

This homework consolidates the laboratory material on hashing by requiring you to design two small but fully engineered hash-table based utilities. The focus is on correctness under edge cases, robust memory ownership in C and transparent reasoning about time and space behaviour. The markscheme rewards clean interfaces, explicit invariants and demonstrable testing.

The two tasks are intentionally structured so that you exercise both collision resolution families:

- Homework 1 requires separate chaining and therefore a clear plan for node allocation and pointer manipulation.
- Homework 2 requires open addressing and therefore correct tombstone handling, probe sequencing and resizing.

## Administrative information

- Deadline: End of Week 12 (Sunday 23:59)
- Total points: 100
- Language: ISO C11
- Compiler: GCC using the flags -Wall -Wextra -std=c11
- Memory checking: your submission must run without leaks and without invalid memory accesses. The absence of leaks must be supported by a reproducible command transcript (Valgrind or address sanitizer are acceptable)

## Submission package

Submit a single zip archive containing at minimum:

- src/ directory with your C sources and headers
- Makefile that builds the required executables and runs your tests
- REPORT.txt containing the required technical report
- tests/ directory containing your test inputs and a short description of how to run them

Your code must compile and run on a standard Linux system with GCC.

## General technical requirements

### 1. Memory ownership rules

1. Every dynamically allocated object must have a single, unambiguous owner responsible for freeing it.
2. For keyed data structures, assume that incoming strings are borrowed. If your structure needs the string after the call returns then you must copy it into owned memory.
3. Destructors must free all memory associated with the structure including keys, values, nodes and the bucket or slot array.

### 2. Error handling policy

Your code must define a consistent approach to allocation failure and invalid inputs. A typical policy is:

- Public API functions validate arguments and return an error code or NULL pointer on failure.
- Allocation failures propagate to the caller rather than aborting the process.
- On failure, data structure invariants remain true.

### 3. Performance expectations

You are expected to justify your time complexity under a stated model (for example simple uniform hashing). You are not required to provide formal proofs but you are required to provide a technically coherent argument.

Avoid accidental quadratic behaviour such as repeated linear scans during resizing or sorting the entire dataset on every insertion.

### 4. Style and correctness requirements

- Use meaningful names and avoid hidden global state.
- Declare representation invariants as comments at the top of each module.
- Compile with -Wall and -Wextra and treat warnings as defects.
- Provide at least one test that reaches a non-trivial load factor and triggers a resize.

## Homework 1: Phone Directory using separate chaining (50 points)

### Problem statement

Implement a phone directory that maps contact names to phone numbers.

- Key: contact name
- Value: phone number

The directory must support inserting contacts, updating an existing contact, searching, deleting and listing all contacts in alphabetical order.

### Data constraints

- Name: at most 50 visible characters (excluding the terminator)
- Phone: at most 15 visible characters

### Required API

Implement at minimum the following functions:

- directory_create(initial_size)
- directory_destroy(dir)
- directory_put(dir, name, phone)
- directory_get(dir, name)
- directory_delete(dir, name)
- directory_list_sorted(dir)

Names must be matched case-insensitively. The recommended implementation is to normalise names to lowercase on insertion and on lookup.

### Recommended hashing strategy

Use djb2 for string hashing. The table index is the hash reduced modulo the current table size. Use a prime table size and a resizing policy when the load factor exceeds 0.75.

### Output requirements

Provide a small command line interface that accepts simple commands from standard input. A minimal example command set is:

- PUT name phone
- GET name
- DEL name
- LIST
- QUIT

Your interface may differ but it must be documented and testable.

### Markscheme (50 points)

- Data structures and invariants: 10
- Hash function and index computation: 5
- Correct insertion and update semantics: 10
- Correct search semantics and case-insensitive matching: 10
- Correct deletion semantics and memory reclamation: 5
- Sorted listing implemented efficiently and correctly: 5
- Resizing and rehashing correctness: 5

## Homework 2: Word frequency counter using open addressing (50 points)

### Problem statement

Implement a word frequency counter that reads a UTF-8 text file and counts occurrences of alphabetic words. You may treat alphabetic as the ASCII letters A to Z and a to z. Words are case-insensitive.

The programme must store each unique word exactly once and maintain a counter.

### Collision resolution requirements

- Use open addressing
- Use double hashing
- Use tombstones for deletion

### Required behaviour

1. Parse the file and tokenise it into words.
2. For each word, insert if absent or increment the counter if present.
3. Print statistics and the top N most frequent words.

### Required statistics

At minimum print:

- Table size
- Number of unique words
- Load factor
- Probe statistics under your chosen definition (document it clearly)

### Markscheme (50 points)

- Correct tokenisation and normalisation: 10
- Correct double hashing probe sequence and non-zero step: 10
- Correct insertion and update semantics: 10
- Correct resizing and reinsertion policy: 10
- Correct memory reclamation and tombstone handling: 5
- Clear statistics and reproducible tests: 5

## Testing requirements

Your tests must be reproducible and must cover:

- Collision behaviour for both chaining and open addressing
- Deletion behaviour for both structures
- Resizing behaviour for both structures
- Idempotence of update operations
- Boundary cases: empty strings, very long words, repeated deletions

At minimum include one test for Homework 2 that performs at least 10,000 word insertions and lookups so that accidental quadratic behaviour is likely to be exposed.

## Required report content (REPORT.txt)

Your report must contain:

1. A precise description of your data structures and their representation invariants.
2. A description of your hash functions and how you reduce to table indices.
3. A justification of your resizing policy and a brief amortised argument.
4. A discussion of failure modes (allocation failure, invalid input, empty file) and how you handle them.
5. A summary of your tests including which invariants each test targets.

A concise report is acceptable but it must be technically specific.

## Appendix A: Suggested pseudocode skeletons

The following pseudocode is intentionally abstract. It is not a requirement to follow it literally but your implementation should satisfy the same invariants.

### A.1 Separate chaining insert or update

Input: table T with m buckets, key k, value v
Output: either inserts (k, v) or updates existing (k, _) to (k, v)

Algorithm:

1. i <- index(k)  (compute hash and reduce to [0, m-1])
2. for node in T.bucket[i]:
      if node.key == k:
         node.value <- v
         return UPDATED
3. allocate new node
4. new.key <- copy of k
5. new.value <- v
6. new.next <- T.bucket[i]
7. T.bucket[i] <- new
8. T.count <- T.count + 1
9. return INSERTED

Correctness hinge: the chain is a complete representation of all keys whose indices equal i so a linear scan is sufficient.

### A.2 Open addressing insert or increment using double hashing

Input: table T with m slots, key k
Output: increments T[k] if present otherwise creates T[k] = 1

1. if effective_load_factor(T) > threshold:
      rehash(T)
2. h1 <- primary_hash(k) mod m
3. h2 <- secondary_hash(k, m)  (must be in [1, m-1])
4. first_tombstone <- NONE
5. probes <- 0
6. repeat:
      probes <- probes + 1
      slot <- T[h1]
      if slot.state == EMPTY:
           target <- (first_tombstone != NONE) ? first_tombstone : h1
           write key and initial count at target
           update statistics
           return probes
      if slot.state == DELETED and first_tombstone == NONE:
           first_tombstone <- h1
      if slot.state == OCCUPIED and slot.key == k:
           slot.count <- slot.count + 1
           update statistics
           return probes
      h1 <- (h1 + h2) mod m

Correctness hinge: EMPTY terminates unsuccessful search because a probe sequence for k would have stopped at the first EMPTY position if k had been inserted.

## Appendix B: Minimum acceptance checklist

A submission is unlikely to pass if any of the following are violated:

- All heap allocations are checked for failure
- All owned strings are eventually freed
- Open addressing deletion uses tombstones rather than setting EMPTY
- Rehashing preserves all previously inserted keys
- The programme does not assume that input lines are well-formed


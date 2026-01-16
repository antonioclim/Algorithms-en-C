# Week 03: Binary Files in C

## Abstract

This laboratory develops a record-oriented model of persistence in C by treating a file as an addressable sequence of bytes rather than a textual stream. Students implement two complementary designs:

1. A fixed-length binary record store supporting sequential access, bulk loading and in-place updates.
2. A data file coupled to a separate index file enabling asymptotically faster point lookups through binary search.

The work foregrounds the non-trivial boundary between an in-memory representation and an on-disk representation, with particular emphasis on alignment, padding, determinism, robustness under I/O failure and the epistemology of testing when the primary observable is a byte-level artefact.

## Learning outcomes

On successful completion of the laboratory a student should be able to:

1. Distinguish text mode from binary mode with respect to newline translation, encoding assumptions and platform-defined behaviour.
2. Use `fread`, `fwrite`, `fseek` and `ftell` to implement sequential and random access operations over fixed-size records.
3. Explain how alignment and padding affect `sizeof(struct)` and why this matters when serialising memory layouts.
4. Design an on-disk record format with explicit invariants and validate those invariants through deterministic tests.
5. Construct an auxiliary index file that maps keys to byte offsets and use binary search to obtain O(log n) lookup.
6. Implement tombstone-based deletion and justify its correctness and limitations.
7. Develop a reproducible verification workflow combining snapshot testing with memory diagnostics.

## Repository structure

The laboratory is intentionally small and mechanically explicit. The directory tree below is therefore part of the specification rather than an incidental organisational choice.

```
03-binary-files/
  Makefile
  README.md
  src/
    example1.c
    exercise1.c
    exercise2.c
  tests/
    test1_input.txt
    test1_expected.txt
    test2_input.txt
    test2_expected.txt
  data/
    students_sample.txt
    products_sample.txt
  solution/
    exercise1_sol.c
    exercise2_sol.c
    homework1_sol.c
    homework2_sol.c
  teme/
    homework-requirements.md
    homework-extended.md
  slides/
    presentation-week03.html
    presentation-comparativ.html
```

### Build and execution workflow

The `Makefile` is designed to encode the intended development workflow:

- `make` builds `example1`, `exercise1` and `exercise2`.
- `make test` executes regression tests and compares complete programme output against reference transcripts.
- `make valgrind` runs a lightweight memory-leak check on the built executables.
- `make solutions` builds instructor reference implementations and is included for completeness.

The reference tests are intentionally strict. They treat the entire console transcript as the observable behaviour. This style of test is a pedagogical choice: it forces students to reason about determinism, formatting and the consequences of undefined or implementation-defined behaviour.

## Conceptual foundations

### Text mode and binary mode are not merely cosmetic

C exposes a single `FILE *` stream abstraction but the mode string supplied to `fopen` changes how the implementation is permitted to transform bytes. In text mode an implementation may:

- translate newline sequences between in-memory and on-disk representations
- treat control characters as end-of-file markers on particular platforms
- apply implementation-defined buffering or encoding behaviour

In binary mode the implementation is required to expose a byte-preserving view of the file. The most important consequence for this laboratory is that binary mode makes `fseek` and `ftell` meaningful for random access to record offsets because the mapping between file positions and bytes becomes stable.

### Binary serialisation is a contract not an accident

A binary record format is a contract between two components:

- the producer that writes bytes
- the consumer that interprets bytes

In this laboratory the producer and consumer are the same programme at different times. This makes it tempting to treat `fwrite(&record, sizeof(record), 1, fp)` as self-evidently correct. It is correct only under a set of assumptions that should be stated explicitly.

#### Alignment and padding

For a structure `struct S { ... }` the C standard permits the compiler to insert padding bytes between fields and at the end of the structure. Padding exists to satisfy alignment constraints that enable efficient memory access. The existence of padding means that:

- `sizeof(S)` is not necessarily the sum of field sizes
- a binary file written by `fwrite(&s, sizeof(S), 1, fp)` encodes both payload fields and padding bytes
- a file written on one machine may be misinterpreted on another if structure layout differs

Exercise 1 makes this phenomenon concrete by using `MAX_NAME_LENGTH = 50`, which induces padding under typical alignment rules. The student implementation therefore forces a packed layout for the `Student` record so that the record size matches the mathematically expected sum of the field sizes.

A more robust industrial approach is to define an explicit serialisation, for example by writing each field individually in a specified byte order. This laboratory acknowledges that approach in the discussions and homework specification but uses packed records to keep the core logic accessible.

#### Endianness and floating-point representation

Even with a fixed layout, binary records can be non-portable if they embed multi-byte integers or floating-point values without specifying endianness and representation.

- Integers in C are typically stored in two’s complement and may be little-endian or big-endian.
- Floating-point values are commonly IEEE 754 but the standard does not require IEEE 754.

The laboratory tests assume a typical modern environment (little-endian two’s complement, IEEE 754 for `float` and `double`). Students should recognise that this is an assumption and should be able to articulate what would need to change to make the format portable.

## Example 1: Practical byte-level navigation

`src/example1.c` is a compact demonstration of the mechanics of binary I/O. It illustrates how to:

- open a stream in binary mode
- write a fixed-size record
- rewind or seek to a known offset
- interpret `ftell` as the next write position

The pedagogical aim is to make the file position an explicit state variable that can be reasoned about.

## Exercise 1: Student records database

### File model

The database is a single file `students.bin` containing a sequence of fixed-size `Student` records. Each record has the fields:

- `id` (32-bit signed integer)
- `name` (fixed-length character array of 50 bytes)
- `gpa` (`float`)
- `year` (32-bit signed integer)

The record layout is packed to ensure a deterministic on-disk size of 62 bytes per record.

### Operations and algorithms

The exercise implements the following operations:

1. **Append record** (`save_student`): open file in append mode and write a single record.
2. **Bulk load** (`load_students`): read the entire file into a dynamically allocated array.
3. **Sequential search** (`find_student_by_id`): linear scan of the loaded array.
4. **In-place update** (`update_student`): seek to `index * sizeof(Student)` and overwrite the record.
5. **Count records** (`count_students`): compute `filesize / sizeof(Student)` without loading.
6. **Presentation** (`print_student_table`): produce a deterministic table matching the reference transcript.

Although the task is small, it contains most of the elements of a database storage engine in miniature: a physical layout, an access pattern and a set of invariants.

### Correctness invariants

The key invariants can be stated formally.

Let `R` be the record size in bytes and let `F` be the length of the file in bytes.

- **Well-formedness**: `F mod R = 0`. If violated then the file contains a partial record and the database is corrupt.
- **Record count**: `N = F / R`.
- **Record addressability**: the `i`-th record begins at byte offset `i * R`.

The implementation assumes a well-formed file and uses the second and third invariants to compute counts and to perform random access.

### Pseudocode

#### Append a record

```
procedure SAVE_STUDENT(filename, student_record):
    fp <- fopen(filename, "ab")
    if fp == NULL:
        return FAILURE

    written <- fwrite(address(student_record), sizeof(Student), 1, fp)
    fclose(fp)

    if written != 1:
        return FAILURE
    return SUCCESS
```

#### Bulk load all records

```
procedure LOAD_STUDENTS(filename):
    fp <- fopen(filename, "rb")
    if fp == NULL:
        return (EMPTY_ARRAY, 0)

    fseek(fp, 0, SEEK_END)
    F <- ftell(fp)
    fseek(fp, 0, SEEK_SET)

    if F == 0:
        fclose(fp)
        return (EMPTY_ARRAY, 0)

    N <- F / sizeof(Student)
    arr <- malloc(N * sizeof(Student))
    if arr == NULL:
        fclose(fp)
        return FAILURE

    read <- fread(arr, sizeof(Student), N, fp)
    fclose(fp)

    return (arr, read)
```

#### Sequential search

```
procedure FIND_BY_ID(students, N, id):
    for i from 0 to N-1:
        if students[i].id == id:
            return address(students[i])
    return NULL
```

#### In-place update

```
procedure UPDATE_STUDENT(filename, index, new_record):
    fp <- fopen(filename, "r+b")
    if fp == NULL:
        return FAILURE

    offset <- index * sizeof(Student)
    if fseek(fp, offset, SEEK_SET) != 0:
        fclose(fp)
        return FAILURE

    written <- fwrite(address(new_record), sizeof(Student), 1, fp)
    fflush(fp)
    fclose(fp)

    if written != 1:
        return FAILURE
    return SUCCESS
```

### Complexity analysis

| Operation | Time complexity | Space complexity | Rationale |
|---|---:|---:|---|
| Append record | O(1) | O(1) | single `fwrite` at end of file |
| Bulk load | O(N) | O(N) | reads each record once and stores them |
| Sequential search | O(N) | O(1) additional | linear scan |
| In-place update | O(1) | O(1) | one seek and one write |
| Count records | O(1) | O(1) | uses file size |

## Exercise 2: Indexed product database

### Design rationale

Exercise 2 introduces a separation of concerns between:

- a **data file** that stores the authoritative sequence of `Product` records
- an **index file** that stores a sorted mapping from product code to byte offset in the data file

This is a microcosm of the way many storage engines are constructed: the data file is append-friendly and the index is optimised for lookup.

### Data file layout

The data file `products.bin` is a sequence of fixed-size `Product` records. Each record includes a tombstone field:

- `deleted` equals `0` for active records
- `deleted` equals `-1` for deleted records

This is a simple implementation of a tombstone approach. It preserves the history of insertions and deletions without requiring expensive compaction on each delete.

### Index file layout

The index file `products.idx` is a sequence of `IndexEntry` records:

- `code` is the search key
- `offset` is the byte position of the corresponding `Product` in the data file

The index is maintained in sorted order by `code` so that lookups can be performed with binary search.

### Algorithms and invariants

The index must satisfy the following invariants.

- **Sortedness**: entries are sorted non-decreasing by `code`.
- **Soundness**: for each entry `(code, offset)` the data file at `offset` contains a record with `record.code = code`.
- **Consistency under deletion**: an entry may exist for a record whose tombstone is set. In that case `search_product` must treat the record as absent.

### Pseudocode

#### Add product

```
procedure ADD_PRODUCT(product):
    fp_data <- fopen(DATA_FILE, "ab")
    if fp_data == NULL:
        return FAILURE

    offset <- ftell(fp_data)
    fwrite(product, sizeof(Product), 1, fp_data)
    fclose(fp_data)

    (index, n) <- LOAD_INDEX()
    index <- realloc(index, (n+1) * sizeof(Index_ENTRY))
    index[n] <- (product.code, offset)
    n <- n + 1

    sort(index by code)
    SAVE_INDEX(index, n)
    free(index)
    return SUCCESS
```

#### Search product via index

```
procedure SEARCH_PRODUCT(code):
    (index, n) <- LOAD_INDEX()
    if n == 0:
        return NOT_FOUND

    entry <- BINARY_SEARCH(index, n, key=code)
    free(index)
    if entry == NULL:
        return NOT_FOUND

    fp <- fopen(DATA_FILE, "rb")
    fseek(fp, entry.offset, SEEK_SET)
    read product
    fclose(fp)

    if product.deleted == TOMBSTONE:
        return NOT_FOUND
    return product
```

#### Update price (random access)

```
procedure UPDATE_PRICE(code, new_price):
    entry <- LOOKUP_IN_INDEX(code)
    if entry == NULL:
        return FAILURE

    fp <- fopen(DATA_FILE, "r+b")
    fseek(fp, entry.offset, SEEK_SET)
    read product

    if product.deleted == TOMBSTONE:
        fclose(fp)
        return FAILURE

    product.price <- new_price
    fseek(fp, entry.offset, SEEK_SET)
    fwrite(product)
    fflush(fp)
    fclose(fp)
    return SUCCESS
```

#### Tombstone deletion

```
procedure DELETE_PRODUCT(code):
    entry <- LOOKUP_IN_INDEX(code)
    if entry == NULL:
        return FAILURE

    fp <- fopen(DATA_FILE, "r+b")
    fseek(fp, entry.offset, SEEK_SET)
    read product

    product.deleted <- TOMBSTONE
    fseek(fp, entry.offset, SEEK_SET)
    fwrite(product)
    fflush(fp)
    fclose(fp)
    return SUCCESS
```

#### Rebuild index

```
procedure REBUILD_INDEX():
    fp <- fopen(DATA_FILE, "rb")
    if fp == NULL:
        remove INDEX_FILE
        return SUCCESS

    index <- empty array
    while read product from fp succeeds:
        offset <- current record offset
        if product.deleted != TOMBSTONE:
            append (product.code, offset) to index

    sort(index by code)
    SAVE_INDEX(index)
    free(index)
    fclose(fp)
    return SUCCESS
```

### Complexity analysis

| Operation | Time complexity | Space complexity | Notes |
|---|---:|---:|---|
| Add product | O(n log n) | O(n) | index is loaded and resorted on each insert in this simple design |
| Search product | O(log n) | O(n) transient | O(n) memory is used to load index then freed |
| Update price | O(log n) | O(n) transient | same index lookup then O(1) write |
| Delete product | O(log n) | O(n) transient | deletion is O(1) on data file after lookup |
| Rebuild index | O(N log N) | O(N) | scans all records and sorts active keys |

The design is intentionally not optimal for high insertion rates. Its purpose is to make the role of an index explicit. In advanced work an index would be maintained incrementally without full reload and re-sort on each insert.

## Testing and verification

### Regression tests as behavioural specifications

The `tests/` directory contains input fixtures and expected outputs. The `make test` target executes both exercises and diffs the produced output against the reference transcripts.

This style of test has two benefits in a teaching setting:

- It reduces the degrees of freedom so that students learn to reason about deterministic formatting and error handling.
- It makes incorrect assumptions visible, for example relying on an uninitialised padding byte that changes across runs.

### Memory diagnostics

`make valgrind` runs each executable under Valgrind and reports a compact summary. Passing Valgrind is not a proof of correctness but it is an essential sanity check in a language where memory safety is not guaranteed by the runtime.

## Cross-language perspectives

Binary record I/O is a recurring idea across ecosystems although the idioms differ.

### Python

Python’s `struct` module provides explicit packing, endianness and field sizes.

```python
import struct

# little-endian: int32, 50-byte string, float32, int32
fmt = "<i50sfi"
packed = struct.pack(fmt, 1001, b"Alice Johnson".ljust(50, b"\0"), 3.75, 2024)
```

### C++

C++ streams can be used in binary mode but the same caveat applies: writing a raw `struct` is only safe if the layout is stable and agreed.

```cpp
std::ofstream out("students.bin", std::ios::binary | std::ios::app);
out.write(reinterpret_cast<const char*>(&student), sizeof(Student));
```

### Java

Java’s `RandomAccessFile` exposes explicit seek and read/write operations, which map cleanly onto the random-access patterns used in Exercise 2.

```java
RandomAccessFile raf = new RandomAccessFile("products.bin", "rw");
raf.seek(offset);
raf.writeInt(code);
```

In Java, the portability problem is not removed but it is moved: one typically writes an explicit byte order via `ByteBuffer`.

## Suggested extensions

Students who wish to push beyond the laboratory requirements may consider:

- adding a header with a magic number and a version field to each binary file
- implementing compaction to remove tombstones and rebuild a dense file
- maintaining the index incrementally without full reload and re-sort
- implementing a multi-level index structure such as a B-tree stored on disk
- implementing a write-ahead log that makes updates crash-resilient

Each extension forces a deeper engagement with invariants and failure models which is the central intellectual aim of working with binary files.

## Implementation notes that matter in practice

The laboratory code is intentionally small but its central theme is subtle: when a programme writes binary bytes it is not merely producing output but asserting a long-lived contract. The discussion below makes the contract explicit and identifies the points at which naive implementations typically fail.

### Deterministic output is part of the behavioural specification

The regression tests compare the entire console transcript and therefore treat formatting as observable behaviour. This is not an aesthetic constraint. Deterministic output is a proxy for deterministic reasoning. If two runs of the same programme with the same input produce different output then one of the following is occurring:

- uninitialised memory is being printed or written to disk
- an implementation-defined detail such as struct padding is leaking into the output
- the programme is reading a file with a non-deterministic initial state

The box-drawing tables used in the exercises also highlight an operational detail: the output is UTF-8 and therefore the terminal environment must be able to render box-drawing characters. The tests assume a UTF-8 capable environment and a byte-for-byte match of the output stream.

### Byte offsets and record layouts

Binary file programming becomes simpler when the byte layout is written down as a table rather than held implicitly in the programmer’s head.

#### Student record layout (packed)

The student database uses a packed record of size 62 bytes.

| Field | Type | Offset (bytes) | Length (bytes) | Notes |
|---|---|---:|---:|---|
| `id` | `int32_t` | 0 | 4 | two’s complement on typical platforms |
| `name` | `char[50]` | 4 | 50 | NUL-terminated if shorter than 50 |
| `gpa` | `float` | 54 | 4 | IEEE 754 binary32 on typical platforms |
| `year` | `int32_t` | 58 | 4 | stored as signed integer |

The packed layout removes the padding that would otherwise appear after the 50-byte `name` field. The pedagogical point is that padding is not an error in C but it is a hazard for naive serialisation.

#### Product record layout (default alignment)

The product database uses a 64-byte record under the default alignment rules expected by the test environment.

| Field | Type | Offset (bytes) | Length (bytes) | Notes |
|---|---|---:|---:|---|
| `deleted` | `int32_t` | 0 | 4 | 0 = active, -1 = tombstone |
| `code` | `int32_t` | 4 | 4 | primary key |
| `name` | `char[40]` | 8 | 40 | fixed-length name buffer |
| `price` | `double` | 48 | 8 | IEEE 754 binary64 on typical platforms |
| `quantity` | `int32_t` | 56 | 4 | units in stock |
| padding | bytes | 60 | 4 | tail padding to satisfy 8-byte alignment |

The final padding is often ignored in informal discussions yet it is present in the bytes written by `fwrite` when the programme writes `sizeof(Product)` bytes.

### Index semantics and binary search correctness

Exercise 2 uses `qsort` to maintain a sorted index and `bsearch` to look up keys. The correctness of binary search depends on two conditions:

1. the index is sorted according to the same comparator used by the search
2. the comparator defines a total order (no contradictory results)

The implementation therefore uses a comparator that returns -1, 0 or 1 based on explicit comparisons rather than subtraction. Subtraction-based comparators are common in small examples but they can overflow when keys are far apart, which can violate transitivity and thereby break the assumptions of `qsort` and `bsearch`.

Binary search can be written as the following canonical procedure.

```
procedure BINARY_SEARCH(A, n, key):
    lo <- 0
    hi <- n - 1
    while lo <= hi:
        mid <- lo + floor((hi - lo) / 2)
        if A[mid].code == key:
            return A[mid]
        else if A[mid].code < key:
            lo <- mid + 1
        else:
            hi <- mid - 1
    return NOT_FOUND
```

The crucial detail is the computation of `mid`: writing `mid <- (lo + hi) / 2` may overflow for large indices and is therefore replaced by `lo + (hi - lo)/2`.

### Failure models and recovery strategies

The moment an index file is introduced the system has two persistent artefacts and therefore a new class of failure becomes possible: the artefacts can become inconsistent. Typical causes include programme termination between writing the data record and updating the index or index corruption due to an external process.

The laboratory includes `rebuild_index` as a deliberately simple recovery procedure. It treats the data file as the source of truth, scans records and regenerates a fresh index containing only active entries. This is a standard pattern in storage engineering: keep recovery simple by ensuring there exists at least one artefact from which the others can be derived.

In more advanced designs one would add journalling or a write-ahead log so that the index can be updated atomically with the data record. On POSIX systems this typically involves `fsync` in addition to `fflush` because `fflush` only flushes user-space buffers.

### Why tombstones exist and when they are insufficient

Tombstone deletion is a space-time trade-off.

- It makes deletes O(1) because a record is modified in place.
- It preserves audit information because deleted records remain present.
- It increases file size over time and therefore eventually requires compaction.

The correct way to reason about tombstones is to treat them as a logical predicate on records. A record exists in the logical database if and only if its tombstone flag is not set. All algorithms that read records must apply this predicate.

### Limits of the laboratory model

The programmes in this repository are single-process and assume exclusive access to the underlying files. They also assume that input is well-formed. These assumptions are appropriate for an introductory laboratory but they should be named explicitly because removing them changes the problem qualitatively.

If concurrent access were permitted, even a read-only search would need to consider the possibility that the index is being updated concurrently. If the input were adversarial rather than pedagogical, the programme would need stronger validation, for example checking that `file_size % record_size == 0` and rejecting corrupt files.

## References

| Reference | DOI |
|---|---|
| Bayer, R., & McCreight, E. (1972). Organization and maintenance of large ordered indexes. *Acta Informatica, 1*, 173–189. | https://doi.org/10.1007/BF00288683 |
| Hoare, C. A. R. (1962). Quicksort. *The Computer Journal, 5*(1), 10–16. | https://doi.org/10.1093/comjnl/5.1.10 |
| Codd, E. F. (1970). A relational model of data for large shared data banks. *Communications of the ACM, 13*(6), 377–387. | https://doi.org/10.1145/362384.362685 |
| Graefe, G. (2012). A survey of B-tree logging and recovery techniques. *ACM Transactions on Database Systems, 37*(1), Article 1. | https://doi.org/10.1145/2109196.2109197 |
| IEEE. (2019). *IEEE Standard for Floating-Point Arithmetic (IEEE 754-2019)*. *IEEE Std 754-2019*. | https://doi.org/10.1109/IEEESTD.2019.8766229 |
| Mohan, C., Haderle, D. J., Lindsay, B. G., Pirahesh, H., & Schwarz, P. M. (1992). ARIES: A transaction recovery method supporting fine-granularity locking and partial rollbacks using write-ahead logging. *ACM Transactions on Database Systems, 17*(1), 94–162. | https://doi.org/10.1145/128765.128770 |
| Sears, R., & Brewer, E. (2009). Segment-based recovery: Write-ahead logging revisited. *Proceedings of the VLDB Endowment, 2*(1), 490–501. | https://doi.org/10.14778/1687627.1687683 |

# Extended challenges for Week 03: Binary files, indices and recovery

## Scope and intent

The optional challenges below are designed for students who wish to deepen their understanding of persistent data structures, failure models and performance trade-offs. Each challenge is specified at the level of an engineering problem rather than a toy exercise: you are expected to define invariants, justify algorithmic choices and demonstrate reproducibility.

Each successfully completed challenge earns **+10 bonus points** subject to the constraints in the course handbook. You may complete more than one challenge but the maximum bonus applied to the course total is determined by the module coordinator.

The challenges are independent although several compose naturally. If you choose to combine them then your submission must clearly separate the contributions so that marking remains feasible.

---

## Challenge 1: On-disk B-tree index

### 1.1 Problem statement

Implement an on-disk B-tree index for efficient key to offset lookup. In contrast to a flat sorted array stored in memory, a B-tree is designed to minimise I/O by keeping node fan-out high and tree height small. The goal is not merely to implement a search tree but to implement a search tree whose *layout and update strategy* respect the fact that the primary cost is disk access.

### 1.2 Data model

- Keys are 32-bit signed integers.
- Values are 64-bit file offsets into an external data file.
- The B-tree is stored in a single binary file and nodes are addressed by byte offsets.
- Node size is fixed and chosen to align with a convenient block size (for example 4096 bytes) so that a node can be read with a single I/O request.

### 1.3 Suggested node layout

The layout below is intentionally explicit. You may change it if you explain why.

```c
#define BTREE_ORDER 64  /* example: high fan-out to reduce height */

typedef struct {
    int32_t  is_leaf;
    int32_t  num_keys;
    int32_t  keys[BTREE_ORDER - 1];
    int64_t  values[BTREE_ORDER - 1];   /* data file offsets */
    int64_t  children[BTREE_ORDER];     /* child node offsets, -1 if none */
} BTreeNode;
```

### 1.4 Invariants

Your implementation must maintain the standard B-tree invariants:

- Keys in a node are stored in strictly increasing order.
- Every internal node with `k` keys has `k + 1` children.
- All leaves appear at the same depth.
- Every node except the root has between `ceil(order/2) - 1` and `order - 1` keys.

The on-disk representation adds further invariants:

- Every child pointer is either `-1` or a valid node offset that is aligned to the node size.
- A node can be read and written atomically at the level of a single `fread` and `fwrite` of `sizeof(BTreeNode)` bytes.

### 1.5 Algorithms and pseudocode

#### Search

```
procedure BTREE_SEARCH(root_offset, key):
    node_offset <- root_offset
    while node_offset != -1:
        node <- READ_NODE(node_offset)
        i <- LOWER_BOUND(node.keys, node.num_keys, key)
        if i < node.num_keys and node.keys[i] == key:
            return node.values[i]
        if node.is_leaf:
            return NOT_FOUND
        node_offset <- node.children[i]
    return NOT_FOUND
```

#### Insert with splitting

```
procedure BTREE_INSERT(root_offset, key, value):
    root <- READ_NODE(root_offset)
    if root is full:
        new_root_offset <- ALLOCATE_NODE()
        new_root <- empty internal node
        new_root.children[0] <- root_offset
        SPLIT_CHILD(new_root, 0)
        WRITE_NODE(new_root_offset, new_root)
        root_offset <- new_root_offset
    INSERT_NONFULL(root_offset, key, value)
    return root_offset
```

Your split procedure must be carefully specified because it is the core of the data structure.

### 1.6 Evaluation criteria

- Correct B-tree invariants over a non-trivial workload: 4
- Search correctness and asymptotic behaviour: 2
- Insert correctness including splitting and root growth: 3
- File format documentation and recovery strategy for a corrupted index file: 1

---

## Challenge 2: Compaction and vacuuming for tombstones

### 2.1 Problem statement

Implement a compaction procedure for a tombstone-based record file such as `products.bin`. The procedure should create a new dense file containing only active records and should rebuild the corresponding index so that it points into the new file.

### 2.2 Correctness requirements

- The compacted file contains exactly the records whose tombstone flag is not set.
- The relative order of active records is preserved unless you justify an alternative.
- The index points to the correct offsets in the compacted file.

### 2.3 Atomic replacement

A key engineering requirement is to avoid leaving the system in a state where both the old and new files are unusable. A standard approach is:

1. write `products.bin.tmp` and `products.idx.tmp`
2. flush and close both
3. rename `products.bin` to `products.bin.bak`
4. rename `products.bin.tmp` to `products.bin`
5. rename `products.idx.tmp` to `products.idx`

Explain the failure cases at each step and how the `.bak` file supports recovery.

---

## Challenge 3: Write-ahead logging and crash recovery

### 3.1 Problem statement

Add a write-ahead log (WAL) to a record store so that updates become recoverable after a crash. The WAL is an append-only file. Each update is first written to the WAL and only then applied to the data file.

### 3.2 Minimal log record design

Define a log record format that contains at minimum:

- an operation code (insert, update, delete)
- a key
- the new record payload or the delta required to reconstruct it
- a monotonically increasing sequence number

### 3.3 Recovery procedure

On startup the programme must:

1. read the last committed sequence number from the data file header (or reconstruct it)
2. scan the WAL and apply any operations with a larger sequence number
3. truncate or checkpoint the WAL

Discuss idempotence: applying the same log record twice must not corrupt the database.

---

## Challenge 4: Portability layer for binary serialisation

### 4.1 Problem statement

Remove dependence on compiler-specific struct layouts by implementing an explicit serialisation layer that writes fields in a defined byte order. The goal is that a file written on a little-endian machine can be read correctly on a big-endian machine.

### 4.2 Suggested approach

- define `write_int32_le`, `read_int32_le`, `write_double_le` and similar primitives
- write each field in a fixed order
- define a versioned header so that future changes do not silently break compatibility

This challenge is primarily about specification discipline: the byte order and field widths must be stated clearly.

---

## Challenge 5: Integrity checks and defensive validation

### 5.1 Problem statement

Augment any of the previous challenges by adding explicit integrity checks to your file formats. The purpose is to make corruption detectable rather than silent. You are not required to implement cryptographic security but you are expected to implement *engineering-grade* checks that detect common classes of failure.

### 5.2 Requirements

- Add a versioned header with a magic number, record size and record count.
- Add a per-record checksum (for example CRC32) or a per-block checksum if you use fixed-size blocks.
- On read, validate that `file_size` is compatible with the declared record size and that `record_count` matches the derived count.
- If a mismatch is detected, fail with a clear diagnostic and, where appropriate, offer a recovery path (for example index rebuild or file scan).

### 5.3 Discussion

The conceptual aim is to distinguish three statements that are often conflated in beginner code:

- the file exists
- the file is well-formed
- the file encodes the intended logical state

Integrity checks address the second statement directly. They do not prove the third but they provide a principled basis for rejecting bytes that cannot be interpreted safely.

---

## Submission guidance

For optional challenges the quality of the argument matters as much as the code. Provide:

- a brief design document describing file formats, invariants and failure models
- a reproducible test harness with deterministic output
- a short complexity analysis that separates CPU cost from I/O cost
- evidence of memory safety (Valgrind summary or equivalent)

Where your implementation choices differ from the suggested designs you must justify the differences in writing.

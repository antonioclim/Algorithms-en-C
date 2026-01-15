# Extended Challenges - Week 03: Binary Files

## 🚀 Advanced Challenges (Optional)

These challenges are designed for students who want to deepen their understanding of binary file handling and explore advanced concepts. Each correctly solved challenge earns **+10 bonus points**.

---

## ⭐ Challenge 1: B-Tree Index (Difficulty: Hard)

### Description

Implement a B-tree index structure stored in a binary file for efficient key-value lookups. Unlike a simple sorted array index, a B-tree maintains balance and supports efficient insertions without rebuilding.

### Requirements

```c
#define BTREE_ORDER 4  /* Maximum children per node */

typedef struct BTreeNode {
    int32_t  is_leaf;
    int32_t  num_keys;
    int32_t  keys[BTREE_ORDER - 1];
    long     values[BTREE_ORDER - 1];    /* Data file offsets */
    long     children[BTREE_ORDER];       /* Child node offsets */
} BTreeNode;
```

1. Implement `btree_insert()` with node splitting
2. Implement `btree_search()` with O(log n) complexity
3. Store the entire B-tree in a single binary file
4. Support at least 1000 keys without performance degradation

### Evaluation

- Correct B-tree structure: 4 points
- Efficient search: 3 points
- Correct insertion with splitting: 3 points

### Bonus Points: +10

---

## ⭐ Challenge 2: Copy-on-Write Transactions (Difficulty: Medium)

### Description

Implement a copy-on-write (CoW) mechanism for safe updates. Instead of modifying records in place, write new versions and update pointers atomically.

### Requirements

```c
typedef struct {
    uint32_t version;           /* Record version number */
    long     previous_offset;   /* Pointer to previous version */
    int32_t  is_current;        /* 1 if this is the current version */
    /* ... actual data fields ... */
} VersionedRecord;
```

1. Implement `cow_update()` that writes a new version without modifying the old one
2. Implement `cow_read()` that finds the current version
3. Implement `cow_rollback()` to revert to a previous version
4. Implement `cow_vacuum()` to remove old versions and compact the file

### Evaluation

- Correct versioning: 4 points
- Working rollback: 3 points
- Vacuum implementation: 3 points

### Bonus Points: +10

---

## ⭐ Challenge 3: Memory-Mapped File I/O (Difficulty: Medium)

### Description

Use `mmap()` system call to map a binary file directly into memory, enabling array-like access to file contents without explicit read/write calls.

### Requirements

```c
#include <sys/mman.h>

typedef struct {
    void   *mapped_region;
    size_t  file_size;
    int     fd;
} MappedDatabase;
```

1. Implement `mmap_open()` to map a file into memory
2. Implement `mmap_close()` to unmap and sync
3. Implement record access using pointer arithmetic
4. Demonstrate performance improvement over fread/fwrite for random access
5. Handle file growth (remapping)

### Evaluation

- Correct mmap usage: 4 points
- Safe error handling: 3 points
- Performance comparison: 3 points

### Bonus Points: +10

---

## ⭐ Challenge 4: Cross-Platform Binary Format (Difficulty: Medium)

### Description

Design and implement a portable binary file format that works correctly across different architectures (32-bit, 64-bit, little-endian, big-endian).

### Requirements

1. Define explicit byte-order conversion functions:
   ```c
   uint32_t to_little_endian_32(uint32_t value);
   uint32_t from_little_endian_32(uint32_t value);
   uint64_t to_little_endian_64(uint64_t value);
   ```

2. Use packed structures or byte-by-byte serialisation:
   ```c
   void serialize_int32(uint8_t *buffer, int32_t value);
   int32_t deserialize_int32(const uint8_t *buffer);
   ```

3. Include a format descriptor in the file header:
   ```c
   typedef struct {
       uint8_t  byte_order_mark;   /* 0x01 = little, 0x02 = big */
       uint8_t  pointer_size;      /* 4 or 8 bytes */
       uint8_t  alignment;         /* Structure alignment used */
       uint8_t  version;           /* Format version */
   } FormatDescriptor;
   ```

4. Demonstrate reading a file created on a different (simulated) architecture

### Evaluation

- Correct byte-order handling: 4 points
- Explicit serialisation: 3 points
- Format detection: 3 points

### Bonus Points: +10

---

## ⭐ Challenge 5: Write-Ahead Logging (WAL) (Difficulty: Hard)

### Description

Implement a write-ahead logging system for crash recovery. All modifications are first written to a log file before being applied to the main data file.

### Requirements

```c
typedef struct {
    uint64_t sequence_number;
    uint32_t operation;          /* INSERT, UPDATE, DELETE */
    long     record_offset;
    uint32_t record_size;
    uint8_t  old_data[256];      /* For undo */
    uint8_t  new_data[256];      /* For redo */
    uint32_t checksum;
} WALEntry;
```

1. Implement `wal_begin_transaction()`
2. Implement `wal_log_operation()` to write before modifying data
3. Implement `wal_commit()` to mark transaction complete
4. Implement `wal_recover()` to replay committed transactions after crash
5. Implement `wal_checkpoint()` to apply all pending changes and truncate log

### Evaluation

- Correct logging: 3 points
- Transaction semantics: 3 points
- Recovery implementation: 4 points

### Bonus Points: +10

---

## 📊 Bonus Point System

| Challenges Completed | Total Bonus | Badge |
|---------------------|-------------|-------|
| 1 | +10 points | Binary Apprentice |
| 2 | +20 points | File Handler |
| 3 | +30 points | Storage Expert |
| 4 | +40 points | I/O Master |
| All 5 | +50 points | 🏆 **Binary Wizard** |

---

## 📤 Submission Guidelines

1. Create separate source files for each challenge:
   - `challenge1_btree.c`
   - `challenge2_cow.c`
   - `challenge3_mmap.c`
   - `challenge4_portable.c`
   - `challenge5_wal.c`

2. Include a `README.md` explaining:
   - Which challenges you attempted
   - How to compile and test
   - Any design decisions or limitations

3. Provide test cases demonstrating functionality

4. Submit in a folder named `Week03_Bonus_[YourName]`

---

## 💡 Implementation Tips

### Challenge 1 (B-Tree)
- Start with a simple 2-3 tree before generalising
- Draw the tree structure on paper during insertion
- Test with sequential and random key patterns

### Challenge 2 (Copy-on-Write)
- Think of it like Git commits
- Consider how to efficiently find the "current" version
- Vacuum is essentially garbage collection

### Challenge 3 (mmap)
- Remember to use `msync()` to ensure data persistence
- Handle `SIGBUS` for mapped file access errors
- Start with read-only mapping before read-write

### Challenge 4 (Portable Format)
- Study how PNG or TIFF handle cross-platform issues
- Network byte order (big-endian) is a common choice
- Test with endianness detection: `int test = 1; char *p = (char*)&test;`

### Challenge 5 (WAL)
- Study how SQLite or PostgreSQL implement WAL
- Checksum is critical for detecting corruption
- Consider what happens if crash occurs during log write

---

## 📖 Recommended Reading

- *Database Internals* by Alex Petrov (Chapters on B-Trees and WAL)
- SQLite File Format documentation
- POSIX.1-2017 `mmap()` specification
- Endianness on Wikipedia

---

*These challenges represent real-world techniques used in production database systems. Mastering them provides valuable skills for systems programming careers.*

**Good luck, and may your bytes be ever aligned! 🎯**

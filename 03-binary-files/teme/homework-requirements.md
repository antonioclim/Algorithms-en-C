# Week 03 Homework: Binary Files and Persistent Data Structures

## 1. Administrative information

- **Deadline:** end of Week 04 (local time for your cohort)
- **Weighting:** 10% of the final grade
- **Total points:** 100
- **Language standard:** C11
- **Toolchain:** GCC or Clang with warnings enabled (`-Wall -Wextra -Wpedantic`)
- **Submission artefacts:** source code, a short technical report and a minimal reproducibility package (input files plus a `Makefile` or a build script)

### 1.1 Expectations on technical quality

This homework is assessed as a small systems engineering artefact rather than as a purely algorithmic exercise. In particular:

- Your code should compile without warnings under the specified flags.
- Your programme should fail gracefully when a file operation fails.
- All dynamic allocations must be paired with a corresponding `free` and no memory leaks should remain under Valgrind.
- You should document on-disk formats as explicit contracts including record sizes, invariants and versioning rules.

The intent is that you practise writing programmes whose correctness depends on a byte-level interface, where errors are often silent and therefore must be prevented by design.

---

## 2. Homework 1: Library book catalogue with index and transaction log (50 points)

### 2.1 Problem statement

Design and implement a binary file-based catalogue for a small library. The system stores book records, supports random-access updates and provides efficient search through an auxiliary index file. In addition it maintains an append-only transaction log that can be inspected for audit purposes and can be used to recover basic state after an abnormal termination.

### 2.2 File formats

You must define at least two persistent files:

1. **Catalogue data file** (authoritative store of records).
2. **Index file** (maps a search key to a byte offset in the data file).

A third file is required for transaction logging:

3. **Transaction log** (append-only record of operations).

#### 2.2.1 Catalogue header

The catalogue file must begin with a header that makes the format self-describing.

```c
#define MAGIC_NUMBER "LIBR"  /* 4-byte identifier */
#define FILE_VERSION 1

typedef struct {
    char     magic[4];        /* must equal MAGIC_NUMBER */
    uint32_t version;         /* must equal FILE_VERSION */
    uint32_t record_count;    /* logical number of records */
    uint32_t record_size;     /* sizeof(Book) or explicit value */
    time_t   created;         /* creation timestamp */
    time_t   modified;        /* last modification timestamp */
    uint8_t  reserved[32];    /* future extension, must be zeroed */
} CatalogueHeader;
```

**Invariants you should enforce**

- `magic` matches the expected value.
- `version` is supported.
- `record_size` matches your compiled definition.
- `record_count` is consistent with file size under your record layout.

#### 2.2.2 Book record

```c
typedef struct {
    int64_t  isbn;              /* unique identifier */
    char     title[100];        /* fixed-length UTF-8 buffer */
    char     author[60];        /* fixed-length UTF-8 buffer */
    int32_t  year_published;
    double   price;
    int32_t  copies_available;
    int32_t  times_borrowed;
} Book;
```

You may extend the structure but you must justify the extension and update the header version if you change the on-disk representation.

#### 2.2.3 Index file

The index file must support O(log n) lookup by ISBN. A minimal index entry is:

```c
typedef struct {
    int64_t isbn;
    long    offset;   /* byte offset of the corresponding record */
} BookIndex;
```

The index must be sorted by `isbn` and searched via binary search.

#### 2.2.4 Transaction log

The transaction log should be append-only and human-inspectable at the level of fields, not necessarily at the level of bytes.

A minimal log entry might include:

- timestamp
- operation type (`ADD`, `BORROW`, `RETURN`, `UPDATE_PRICE`)
- primary key (`isbn`)
- optional payload (for example the new price)

You may store the log as text or as binary. If you store it as binary you must define a header and versioning rules.

### 2.3 Required operations

You must implement at least the following operations.

1. **Initialise catalogue**
   - Create the data file if it does not exist.
   - Initialise a valid header.

2. **Add a book**
   - Append a new record to the data file.
   - Update `record_count` and `modified` in the header.
   - Insert an entry into the index.
   - Write a log entry.

3. **Search by ISBN**
   - Load the index and locate the record via binary search.
   - Seek to the corresponding offset and read the record.

4. **Borrow and return**
   - Update `copies_available` and `times_borrowed` using random access.
   - Enforce the precondition that a book cannot be borrowed if `copies_available == 0`.
   - Write a log entry for each operation.

5. **Rebuild index**
   - Scan the entire data file and rebuild the index.
   - This should be used for recovery from index corruption.

### 2.4 Algorithms and pseudocode

#### 2.4.1 Binary search over the index

```
procedure LOOKUP_ISBN(index, n, key_isbn):
    lo <- 0
    hi <- n - 1
    while lo <= hi:
        mid <- lo + (hi - lo) / 2
        if index[mid].isbn == key_isbn:
            return index[mid]
        else if index[mid].isbn < key_isbn:
            lo <- mid + 1
        else:
            hi <- mid - 1
    return NOT_FOUND
```

#### 2.4.2 Random-access update of a record

```
procedure UPDATE_RECORD_AT_OFFSET(fp, offset, record_size, patch_function):
    fseek(fp, offset, SEEK_SET)
    read record
    patch_function(record)
    fseek(fp, offset, SEEK_SET)
    write record
    fflush(fp)
```

### 2.5 Marking scheme (50 points)

- File formats and invariants (header, versioning, validation): 12
- Correct index implementation and O(log n) lookup: 12
- Correct random-access updates and borrow/return logic: 12
- Transaction log design and correctness: 8
- Engineering quality (error handling, memory safety, reproducibility): 6

---

## 3. Homework 2: Binary image processor for PGM (P5) images (50 points)

### 3.1 Problem statement

Implement a command-line tool that reads and writes binary PGM (Portable GrayMap) images in the P5 variant. Your tool must parse the PGM header correctly, allocate a pixel buffer and implement a set of image transformations and analysis functions.

### 3.2 Required features

1. **Input parser**
   - Support comments introduced by `#` and arbitrary whitespace as permitted by the PGM specification.
   - Validate `width`, `height` and `max_value`.
   - Support at least `max_value <= 255` (8-bit images).

2. **Output writer**
   - Write a correct P5 header.
   - Write the pixel buffer in binary form.

3. **Transformations**
   - Inversion: `p <- max_value - p`
   - Thresholding at a user-specified value
   - Horizontal flip
   - Rotation by 90 degrees (clockwise or anticlockwise) with correct reshaping

4. **Analysis**
   - Histogram computation for 0–255 values
   - Mean brightness
   - A simple contrast metric (for example max minus min)

### 3.3 Pseudocode for reading a P5 PGM file

```
procedure READ_PGM(filename):
    fp <- fopen(filename, "rb")
    read magic ("P5")
    skip whitespace and comments
    read width
    skip whitespace and comments
    read height
    skip whitespace and comments
    read max_value
    consume one byte of whitespace after max_value

    allocate pixels[width * height]
    read exactly width * height bytes into pixels
    return image
```

### 3.4 Marking scheme (50 points)

- Correct parsing and validation of P5 headers: 15
- Correct binary I/O of pixel buffers: 10
- Transformations (correctness and edge cases): 15
- Analysis functions and complexity discussion: 6
- Engineering quality (memory safety and error handling): 4

---

## 4. Submission checklist

Before submission you should be able to answer yes to the following.

- Does the programme build cleanly with warnings enabled?
- Can you reproduce the same output for the same input?
- Have you documented the file formats including record sizes and invariants?
- Have you run Valgrind and resolved any leaks?
- Does your index rebuild procedure recover from a deleted or corrupted index file?


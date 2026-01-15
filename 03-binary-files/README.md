# Week 03: Binary Files in C

## 🎯 Learning Objectives

Upon successful completion of this laboratory, students will be able to:

1. **Remember** the fundamental differences between text and binary file modes, including the role of buffering and newline translation across operating systems
2. **Understand** the internal representation of data types in memory and how this representation maps directly to binary file storage
3. **Apply** the standard library functions `fread()`, `fwrite()`, `fseek()` and `ftell()` to implement persistent data storage for structured records
4. **Analyse** the trade-offs between text and binary file formats in terms of portability, human readability, storage efficiency and access patterns
5. **Evaluate** different serialisation strategies for complex data structures, including fixed-length records, length-prefixed strings and index files
6. **Create** a complete binary file-based database system with support for sequential and random access operations, including record insertion, modification, deletion and searching

---

## 📜 Historical Context

The distinction between text and binary file handling emerged from the fundamental architectural differences between early computing systems. In the 1960s and 1970s, mainframe computers from IBM used EBCDIC encoding and record-oriented file systems, whilst Unix systems adopted ASCII encoding and stream-oriented files. This divergence created lasting implications for how programmers must handle file I/O across platforms.

The C programming language, developed by Dennis Ritchie at Bell Labs between 1969 and 1973, introduced a unified abstraction for file handling through the `FILE*` stream concept. The ANSI C standardisation effort in 1989 (C89/C90) codified the distinction between text and binary modes, mandating that implementations support both but allowing platform-specific behaviours for text mode operations. This design decision balanced portability with the reality of diverse operating system file semantics.

Binary file I/O became particularly crucial with the rise of database systems and scientific computing in the 1980s. Systems like dBASE III (1984) popularised fixed-length binary record formats for efficient random access, whilst the HDF (Hierarchical Data Format) developed at NCSA in 1987 demonstrated sophisticated approaches to portable binary data storage. These early decisions continue to influence modern data storage formats, from SQLite's file structure to Protocol Buffers and Apache Parquet.

### Key Figure: Dennis Ritchie (1941–2011)

Dennis MacAlistair Ritchie, known affectionately as "dmr" in the Unix community, created the C programming language and co-developed the Unix operating system alongside Ken Thompson. His design of C's file I/O abstraction layer—providing both low-level byte-stream access and high-level formatted operations—established patterns that persist in virtually every systems programming language today.

Ritchie received the Turing Award in 1983 (jointly with Thompson) and the National Medal of Technology in 1998. His philosophy of creating simple, composable tools that "do one thing well" profoundly shaped software engineering practice.

> *"The only way to learn a new programming language is by writing programs in it."*
> — Dennis Ritchie

---

## 📚 Theoretical Foundations

### 1. Text Mode vs Binary Mode

When opening files in C, the mode string determines how the runtime library handles data translation:

```
Text Mode ("r", "w", "a")           Binary Mode ("rb", "wb", "ab")
┌─────────────────────────────┐    ┌─────────────────────────────┐
│  Application Data           │    │  Application Data           │
│  "Hello\nWorld"             │    │  "Hello\nWorld"             │
└──────────────┬──────────────┘    └──────────────┬──────────────┘
               │                                  │
               ▼                                  ▼
┌─────────────────────────────┐    ┌─────────────────────────────┐
│  C Runtime Translation      │    │  No Translation             │
│  • \n → \r\n (Windows)      │    │  • Bytes pass through       │
│  • EOF handling (Ctrl+Z)    │    │  • No EOF interpretation    │
│  • Character encoding       │    │  • Exact byte preservation  │
└──────────────┬──────────────┘    └──────────────┬──────────────┘
               │                                  │
               ▼                                  ▼
┌─────────────────────────────┐    ┌─────────────────────────────┐
│  File on Disk               │    │  File on Disk               │
│  "Hello\r\nWorld" (Win)     │    │  "Hello\nWorld"             │
│  "Hello\nWorld" (Unix)      │    │  (identical everywhere)     │
└─────────────────────────────┘    └─────────────────────────────┘
```

**Critical Implications:**
- Binary mode guarantees byte-for-byte fidelity between memory and disk
- Text mode behaviour varies by platform (Windows vs Unix vs legacy systems)
- Mixing modes leads to data corruption: never read a binary file in text mode
- The `fseek()` function has limited guarantees in text mode

### 2. Memory Layout and Serialisation

Understanding how C represents data types in memory is essential for binary file operations:

```
Structure: Student Record
┌─────────────────────────────────────────────────────────────────┐
│  struct Student {                                               │
│      int id;              // 4 bytes (typically)                │
│      char name[32];       // 32 bytes                           │
│      float gpa;           // 4 bytes (typically)                │
│      int year;            // 4 bytes (typically)                │
│  };                                                             │
└─────────────────────────────────────────────────────────────────┘

Memory Layout (assuming no padding, 44 bytes total):
┌────────────┬────────────────────────────────┬────────────┬────────────┐
│   id (4B)  │         name (32B)             │  gpa (4B)  │  year (4B) │
├────────────┼────────────────────────────────┼────────────┼────────────┤
│ 0x00001234 │ "Alice Johnson\0\0\0..."       │ 0x40533333 │ 0x000007E8 │
│  (4660)    │                                │   (3.3)    │   (2024)   │
└────────────┴────────────────────────────────┴────────────┴────────────┘
Byte offset:  0          4                      36          40         44
```

**Serialisation Concerns:**
- **Endianness**: x86 uses little-endian; network protocols often use big-endian
- **Alignment and Padding**: Compilers may insert padding bytes for alignment
- **Sizeof Variations**: `int` may be 2, 4, or 8 bytes depending on platform
- **Floating-Point Format**: IEEE 754 is near-universal but not guaranteed

### 3. File Positioning and Random Access

Binary files support efficient random access through positioning functions:

```
File Position Model:
                    
     ftell() returns current position
              │
              ▼
┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
│ R0  │ R1  │ R2  │ R3  │ R4  │ R5  │ R6  │ EOF │  Records in file
└─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
  ↑                 ↑                         ↑
  │                 │                         │
SEEK_SET=0    SEEK_SET + n*sizeof(record)   SEEK_END=0

fseek() positioning modes:
┌──────────────┬──────────────────────────────────────────┐
│ SEEK_SET     │ Absolute position from file beginning   │
│ SEEK_CUR     │ Relative to current position            │
│ SEEK_END     │ Relative to file end (usually negative) │
└──────────────┴──────────────────────────────────────────┘

Random Access Formula:
    position = record_index × sizeof(record)
    fseek(fp, position, SEEK_SET);
    fread(&record, sizeof(record), 1, fp);
```

**Complexity Analysis:**

| Operation         | Sequential File | Random Access Binary |
|-------------------|-----------------|----------------------|
| Read record n     | O(n)            | O(1)                 |
| Append record     | O(1)            | O(1)                 |
| Update record n   | O(n) rebuild    | O(1)                 |
| Delete record n   | O(n) rebuild    | O(1) with tombstone  |
| Search (unsorted) | O(n)            | O(n)                 |
| Search (sorted)   | O(n)            | O(log n) binary      |

---

## 🏭 Industrial Applications

### 1. SQLite Database Engine

SQLite, the world's most deployed database engine, uses a sophisticated binary file format:

```c
/* SQLite-style page-based file access */
#define PAGE_SIZE 4096

typedef struct {
    uint32_t page_number;
    uint8_t  data[PAGE_SIZE - sizeof(uint32_t)];
} DatabasePage;

int read_page(FILE *db, uint32_t page_num, DatabasePage *page) {
    long offset = (long)page_num * PAGE_SIZE;
    if (fseek(db, offset, SEEK_SET) != 0) return -1;
    if (fread(page, PAGE_SIZE, 1, db) != 1) return -1;
    return 0;
}
```

### 2. Game Save Systems

Modern games store complex state in binary formats:

```c
/* Game save file structure */
typedef struct {
    char     magic[4];          /* "SAVE" identifier */
    uint32_t version;           /* Save format version */
    uint32_t checksum;          /* Data integrity check */
    time_t   timestamp;         /* When saved */
    
    /* Player state */
    float    position[3];       /* x, y, z coordinates */
    float    health;
    uint32_t inventory_count;
    /* Variable-length inventory follows... */
} SaveFileHeader;

int save_game(const char *filename, const GameState *state) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return -1;
    
    SaveFileHeader header = {
        .magic = {'S','A','V','E'},
        .version = SAVE_VERSION,
        .timestamp = time(NULL)
    };
    /* ... populate and write ... */
    fwrite(&header, sizeof(header), 1, fp);
    fclose(fp);
    return 0;
}
```

### 3. Image File Formats (BMP)

The Windows Bitmap format demonstrates binary file structure:

```c
/* BMP file headers (packed structures) */
#pragma pack(push, 1)
typedef struct {
    uint16_t type;              /* 0x4D42 = "BM" */
    uint32_t size;              /* File size in bytes */
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;            /* Offset to pixel data */
} BMPFileHeader;

typedef struct {
    uint32_t size;              /* Header size (40) */
    int32_t  width;
    int32_t  height;
    uint16_t planes;            /* Always 1 */
    uint16_t bits_per_pixel;    /* 1, 4, 8, 24, or 32 */
    uint32_t compression;
    /* ... additional fields ... */
} BMPInfoHeader;
#pragma pack(pop)
```

### 4. Scientific Data: HDF5 Pattern

High-performance computing relies on binary formats:

```c
/* Simplified HDF5-style chunked data storage */
typedef struct {
    size_t dimensions[3];       /* Data dimensions */
    size_t chunk_size[3];       /* Chunk dimensions */
    int    compression_type;    /* 0=none, 1=gzip, 2=lz4 */
    off_t  data_offset;         /* Offset to data in file */
} DatasetDescriptor;

/* Write a chunk of data */
int write_chunk(FILE *fp, const DatasetDescriptor *desc,
                const size_t chunk_index[3], const void *data) {
    /* Calculate offset based on chunk coordinates */
    off_t offset = calculate_chunk_offset(desc, chunk_index);
    fseek(fp, offset, SEEK_SET);
    size_t chunk_bytes = desc->chunk_size[0] * desc->chunk_size[1] * 
                         desc->chunk_size[2] * sizeof(float);
    return fwrite(data, chunk_bytes, 1, fp) == 1 ? 0 : -1;
}
```

### 5. Embedded Systems: Configuration Storage

Microcontrollers often store settings in binary format:

```c
/* EEPROM-style configuration storage */
typedef struct {
    uint16_t magic;             /* 0xCF96 validity marker */
    uint8_t  version;
    uint8_t  flags;
    int16_t  calibration[8];    /* Sensor calibration values */
    uint32_t serial_number;
    uint16_t crc16;             /* Data integrity check */
} DeviceConfig;

/* Calculate CRC and save configuration */
int save_config(const char *path, DeviceConfig *config) {
    config->magic = 0xCF96;
    config->crc16 = calculate_crc16(config, sizeof(*config) - 2);
    
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;
    int result = fwrite(config, sizeof(*config), 1, fp) == 1 ? 0 : -1;
    fclose(fp);
    return result;
}
```

---

## 💻 Laboratory Exercises

### Exercise 1: Student Records Database

Implement a binary file-based student records system with the following capabilities:

**Requirements:**
1. Define a `Student` structure with: ID (int), name (char[50]), GPA (float), year (int)
2. Implement `save_student()` to append a record to a binary file
3. Implement `load_students()` to read all records from the file
4. Implement `find_student_by_id()` using sequential search
5. Implement `update_student()` to modify a specific record in place
6. Implement `count_students()` using file size calculation
7. Display formatted output with proper alignment
8. Handle all file operation errors gracefully

**Input Format:**
```
3
1001 Alice_Johnson 3.75 2024
1002 Bob_Smith 3.42 2023
1003 Carol_Williams 3.91 2024
```

**Expected Output:**
```
╔════════════════════════════════════════════════════════════════╗
║                    STUDENT RECORDS DATABASE                    ║
╚════════════════════════════════════════════════════════════════╝

[INFO] Saving 3 students to database...
[OK] Student 1001 saved successfully
[OK] Student 1002 saved successfully
[OK] Student 1003 saved successfully

[INFO] Loading all students from database...
┌────────┬──────────────────────┬───────┬──────┐
│   ID   │        Name          │  GPA  │ Year │
├────────┼──────────────────────┼───────┼──────┤
│   1001 │ Alice Johnson        │  3.75 │ 2024 │
│   1002 │ Bob Smith            │  3.42 │ 2023 │
│   1003 │ Carol Williams       │  3.91 │ 2024 │
└────────┴──────────────────────┴───────┴──────┘

Total records: 3
File size: 204 bytes
```

### Exercise 2: Binary File Index System

Implement an indexed binary file system with random access capabilities:

**Requirements:**
1. Create a `Product` structure with: code (int), name (char[40]), price (double), quantity (int)
2. Implement a separate index file mapping product codes to file offsets
3. Implement `add_product()` that updates both data and index files
4. Implement `search_product()` using the index for O(1) lookup
5. Implement `update_price()` using random access
6. Implement `delete_product()` using a tombstone marker
7. Implement `compact_file()` to remove deleted records
8. Implement `rebuild_index()` to regenerate the index from data file
9. Track and display statistics: total records, active records, deleted records
10. Implement binary search on sorted index for O(log n) code lookup

**Bonus Challenges:**
- Implement a B-tree index structure
- Add transaction logging for crash recovery
- Implement record locking for concurrent access simulation

---

## 🔧 Compilation and Execution

```bash
# Build all targets
make

# Build specific targets
make example1          # Complete demonstration
make exercise1         # Student records exercise
make exercise2         # Indexed file exercise

# Run examples
make run               # Execute all examples

# Run with Valgrind memory checking
make valgrind          # Check for memory leaks

# Run automated tests
make test              # Compare outputs with expected results

# Clean build artefacts
make clean

# Display help
make help
```

**Compiler Flags Explained:**
- `-Wall`: Enable all common warnings
- `-Wextra`: Enable additional warnings
- `-std=c11`: Use C11 standard for modern features
- `-g`: Include debugging symbols for GDB/Valgrind
- `-O2`: Optimisation level 2 (for release builds)

---

## 📁 Directory Structure

```
week-03-binary-files/
├── README.md                           # This documentation
├── Makefile                            # Build automation
│
├── slides/
│   ├── presentation-week03.html        # Main lecture (35-40 slides)
│   └── presentation-comparativ.html    # Pseudocode→C→Python comparison
│
├── src/
│   ├── example1.c                      # Complete binary file demonstration
│   ├── exercise1.c                     # Student records (8 TODOs)
│   └── exercise2.c                     # Indexed file system (12 TODOs)
│
├── data/
│   ├── students_sample.txt             # Sample student data
│   └── products_sample.txt             # Sample product data
│
├── tests/
│   ├── test1_input.txt                 # Test input for exercise 1
│   ├── test1_expected.txt              # Expected output for exercise 1
│   ├── test2_input.txt                 # Test input for exercise 2
│   └── test2_expected.txt              # Expected output for exercise 2
│
├── teme/
│   ├── homework-requirements.md        # Homework 1 & 2 specifications
│   └── homework-extended.md            # Bonus challenges
│
└── solution/
    ├── exercise1_sol.c                 # Solution for exercise 1
    ├── exercise2_sol.c                 # Solution for exercise 2
    ├── homework1_sol.c                 # Solution for homework 1
    └── homework2_sol.c                 # Solution for homework 2
```

---

## 📖 Recommended Reading

### Essential References

1. **Kernighan, B.W. & Ritchie, D.M.** (1988). *The C Programming Language*, 2nd Edition. Chapter 7: Input and Output. Prentice Hall. ISBN: 0-13-110362-8

2. **ISO/IEC 9899:2011** — C11 Standard, Section 7.21: Input/Output `<stdio.h>`

3. **Plauger, P.J.** (1992). *The Standard C Library*. Chapters 12-13: `<stdio.h>` Implementation. Prentice Hall.

### Advanced Topics

4. **Bryant, R.E. & O'Hallaron, D.R.** (2015). *Computer Systems: A Programmer's Perspective*, 3rd Edition. Chapter 10: System-Level I/O. Pearson.

5. **Folk, M.J., Zoellick, B. & Riccardi, G.** (1998). *File Structures: An Object-Oriented Approach with C++*, 3rd Edition. Chapters 4-6: Secondary Storage and File Organisation.

6. **SQLite Documentation**: File Format Description — https://www.sqlite.org/fileformat.html

### Online Resources

7. **cppreference.com** — `<stdio.h>` reference: https://en.cppreference.com/w/c/io

8. **GNU C Library Manual** — File System Interface: https://www.gnu.org/software/libc/manual/html_node/File-System-Interface.html

9. **Beej's Guide to C Programming** — Chapter on File I/O: https://beej.us/guide/bgc/html/split/

---

## ✅ Self-Assessment Checklist

Before submitting your work, verify that you can:

- [ ] Explain the difference between text mode and binary mode file operations
- [ ] Use `fopen()` with appropriate mode strings ("rb", "wb", "ab", "r+b", "w+b")
- [ ] Calculate file sizes using `fseek()` and `ftell()`
- [ ] Read and write structures using `fread()` and `fwrite()`
- [ ] Implement random access to specific records using `fseek()` with `SEEK_SET`
- [ ] Handle endianness issues when designing portable binary formats
- [ ] Understand structure padding and its implications for binary files
- [ ] Implement an index file for efficient record lookup
- [ ] Design a binary file format with magic numbers and version fields
- [ ] Use Valgrind to verify no memory leaks in file handling code

---

## 💼 Interview Preparation

Common technical interview questions on binary files:

1. **"What happens if you read a binary file in text mode on Windows?"**
   - *Expected answer*: Newline translation corrupts binary data; 0x1A (Ctrl+Z) may be interpreted as EOF; byte counts become unreliable.

2. **"How would you implement a simple database with fast lookups using binary files?"**
   - *Expected answer*: Use fixed-size records for O(1) position calculation; create a separate sorted index file; use binary search on the index; consider B-tree for large datasets.

3. **"Explain the portability issues with using `fwrite()` on structures."**
   - *Expected answer*: Structure padding varies by compiler; sizeof(int) varies by platform; endianness differs between architectures; solution is explicit serialisation of each field.

4. **"How does `fseek()` differ between text and binary modes?"**
   - *Expected answer*: In binary mode, offset is exact byte count; in text mode, only `fseek(fp, 0, SEEK_SET)`, `fseek(fp, 0, SEEK_END)`, and `fseek(fp, ftell_value, SEEK_SET)` are guaranteed to work.

5. **"Design a binary file format for storing a variable-length string array."**
   - *Expected answer*: Header with count and total size; length-prefixed strings (4-byte length + chars); alternatively, offset table pointing to null-terminated strings; consider alignment requirements.

---

## 🔗 Next Week Preview

**Week 04: Dynamic Data Structures — Linked Lists**

Building on this week's file I/O foundation, next week introduces dynamic memory allocation and linked data structures. You will learn to:

- Allocate and deallocate memory with `malloc()`, `calloc()`, `realloc()` and `free()`
- Implement singly and doubly linked lists
- Serialise linked structures to binary files
- Compare array-based vs linked implementations for common operations

The file handling skills from this week directly support persistent storage of linked structures—a common requirement in real-world applications.

---

*© 2024 Academy of Economic Studies — Algorithms and Programming Techniques Course*
*This material is licensed under CC BY-NC-SA 4.0*

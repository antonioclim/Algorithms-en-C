# Week 03 Homework: Binary Files

## 📋 General Information

- **Deadline:** End of Week 04
- **Total Points:** 100 (10% of final grade)
- **Language:** C (C11 standard)
- **Compiler:** GCC with `-Wall -Wextra -std=c11`
- **Submission:** Upload source files to the course portal

---

## 📝 Homework 1: Library Book Catalogue (50 points)

### Description

Implement a binary file-based library catalogue system that stores book records with support for multiple search criteria and transaction logging.

### Book Structure

```c
typedef struct {
    int32_t  isbn;              /* Unique ISBN number */
    char     title[100];        /* Book title */
    char     author[60];        /* Author name */
    int32_t  year_published;    /* Publication year */
    double   price;             /* Book price */
    int32_t  copies_available;  /* Number of copies in library */
    int32_t  times_borrowed;    /* Borrow count for popularity tracking */
} Book;
```

### Requirements

1. **File Operations** (15 points)
   - Create a binary catalogue file with a proper header containing magic number, version, and record count
   - Implement `add_book()` to append new books
   - Implement `load_catalogue()` to read all books
   - Handle file errors gracefully with meaningful error messages

2. **Search Functionality** (15 points)
   - Implement `find_by_isbn()` using the primary index
   - Implement `find_by_author()` returning all books by an author
   - Implement `find_by_year_range()` for books published between two years
   - All searches should use binary search where applicable

3. **Index Management** (10 points)
   - Create and maintain a sorted index file for ISBN lookups
   - Implement `rebuild_index()` to regenerate from data file
   - Index must support O(log n) search performance

4. **Transaction Logging** (10 points)
   - Log all add/update/delete operations to a separate text file
   - Include timestamp, operation type, and book details
   - Format: `[YYYY-MM-DD HH:MM:SS] OPERATION: details`

### Example Usage

```c
Book book;
init_book(&book, 9780131103627, "The C Programming Language", 
          "Kernighan & Ritchie", 1988, 45.99, 5, 0);

add_book("library.bin", &book);

Book *found = find_by_isbn("library.bin", 9780131103627);
if (found) {
    printf("Found: %s by %s\n", found->title, found->author);
}
```

### File: `homework1_library.c`

---

## 📝 Homework 2: Binary Image Processor (50 points)

### Description

Implement a programme that reads, processes, and writes binary PGM (Portable Grey Map) image files. PGM is a simple greyscale image format.

### PGM File Format

```
P5                    # Magic number (binary PGM)
# Optional comments
width height          # Image dimensions
max_value             # Maximum grey value (usually 255)
[binary pixel data]   # width × height bytes
```

### Image Structure

```c
typedef struct {
    char     magic[3];       /* "P5\0" */
    int32_t  width;          /* Image width in pixels */
    int32_t  height;         /* Image height in pixels */
    int32_t  max_value;      /* Maximum grey value (1-255) */
    uint8_t *pixels;         /* Pixel data (width × height bytes) */
} PGMImage;
```

### Requirements

1. **File I/O** (15 points)
   - Implement `read_pgm()` to load a PGM file into the structure
   - Implement `write_pgm()` to save the structure to a file
   - Parse the text header correctly, then read binary pixel data
   - Handle comments in the header (lines starting with #)

2. **Image Processing** (20 points)
   - Implement `invert_image()` to create a negative (255 - pixel value)
   - Implement `threshold_image()` to convert to black/white based on threshold
   - Implement `rotate_90()` to rotate the image clockwise
   - Implement `flip_horizontal()` to mirror the image

3. **Histogram Analysis** (10 points)
   - Implement `compute_histogram()` returning frequency of each grey level
   - Implement `find_brightness()` returning average pixel value
   - Implement `find_contrast()` returning the range (max - min)

4. **Memory Management** (5 points)
   - Properly allocate and free pixel data
   - Check for allocation failures
   - No memory leaks (verified with Valgrind)

### Example Usage

```c
PGMImage img;
if (read_pgm("input.pgm", &img) == 0) {
    invert_image(&img);
    write_pgm("inverted.pgm", &img);
    
    int histogram[256];
    compute_histogram(&img, histogram);
    printf("Average brightness: %.2f\n", find_brightness(&img));
    
    free_pgm(&img);
}
```

### File: `homework2_pgm.c`

---

## 📊 Evaluation Criteria

| Criterion | Points |
|-----------|--------|
| **Homework 1** | |
| Functional correctness | 20 |
| Proper binary file handling | 10 |
| Index implementation | 10 |
| Transaction logging | 10 |
| **Homework 2** | |
| PGM read/write correctness | 15 |
| Image processing functions | 20 |
| Histogram analysis | 10 |
| Memory management | 5 |
| **Both** | |
| Code quality and style | 0 (no penalty if good) |

### Penalties

| Issue | Penalty |
|-------|---------|
| Compiler warnings | -10 points |
| Memory leaks (Valgrind) | -20 points |
| Crashes on valid input | -30 points |
| Missing error handling | -10 points |
| Plagiarism | -100 points + disciplinary action |

---

## 📤 Submission Instructions

1. Create a folder named `Week03_[YourName]_[StudentID]`
2. Include the following files:
   - `homework1_library.c`
   - `homework2_pgm.c`
   - `README.txt` with compilation instructions and any notes
3. Compress as ZIP file
4. Upload to course portal before deadline

---

## 💡 Tips for Success

1. **Start with the structure definitions** and ensure they have predictable sizes using `stdint.h` types.

2. **Test incrementally**: Write and test one function at a time rather than the entire programme at once.

3. **Use hexdump or xxd** to inspect your binary files and verify they contain what you expect.

4. **Handle edge cases**: Empty files, single-record files, corrupted data.

5. **Check return values** from all file operations—this is a major source of bugs.

6. **Use Valgrind early and often**: `valgrind --leak-check=full ./your_programme`

7. **Create simple test files** manually to verify your read functions before testing with complex data.

8. **Document your code**: Comments explaining the binary file format and function behaviour.

---

## 📖 Relevant Reading

- K&R Chapter 7 (Input and Output)
- C11 Standard, Section 7.21 (`<stdio.h>`)
- PGM Format Specification: http://netpbm.sourceforge.net/doc/pgm.html

---

*Good luck! Remember: binary files are just sequences of bytes—understand the format, and the code writes itself.*

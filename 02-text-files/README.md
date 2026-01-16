# Week 02: Text File Processing in C

## Learning Objectives

By the end of this laboratory session, students will be able to:

1. **Remember** the fundamental file I/O functions provided by the C standard library (`fopen`, `fclose`, `fgets`, `fprintf`, `fscanf`) and their appropriate use cases
2. **Understand** the distinction between text mode and binary mode file operations, including platform-specific line ending conventions and buffering strategies
3. **Apply** file handling techniques to read, parse and process structured data from CSV and delimited text files
4. **Analyse** the performance implications of different buffering modes (`_IOFBF`, `_IOLBF`, `_IONBF`) and their impact on I/O throughput
5. **Evaluate** error handling strategies for file operations, including proper resource cleanup using defensive programming patterns
6. **Create** robust file processing utilities that handle malformed input gracefully and produce correctly formatted output

---

## Historical Context

The evolution of file I/O in C reflects the broader history of operating system design. When Dennis Ritchie and Ken Thompson developed C at Bell Labs between 1969 and 1973, they designed it to be the systems programming language for Unix. The elegant abstraction of "everything is a file" in Unix directly influenced how C handles I/O operations.

The C standard library's `stdio.h` header, standardised in ANSI C (1989), provided a portable abstraction layer over operating system-specific file operations. This abstraction introduced the concept of *streams*—a sequence of characters flowing between a program and an external device. The `FILE` pointer became one of the most recognisable opaque types in programming history.

Text file processing gained particular importance during the early computing era when human-readable formats were essential for configuration files, data interchange and logging. The CSV (Comma-Separated Values) format, though not formally standardised until RFC 4180 in 2005, traces its origins to early IBM Fortran implementations in the 1960s. Today, despite the proliferation of JSON and XML, plain text formats remain ubiquitous due to their simplicity, debuggability and universal tool support.

### Key Figure: Dennis Ritchie (1941–2011)

Dennis MacAlistair Ritchie, known affectionately as "dmr", created the C programming language and co-developed the Unix operating system with Ken Thompson. His work on C's I/O system established patterns still used across virtually all modern programming languages. Ritchie received the Turing Award in 1983, shared with Thompson, for their contributions to operating systems theory.

> *"Unix is basically a simple operating system, but you have to be a genius to understand the simplicity."* — Dennis Ritchie

---

## Theoretical Foundations

### 1. The Stream Abstraction

In C, a *stream* represents a logical connection between a program and a file or device. The `FILE` type (defined in `<stdio.h>`) is an opaque structure containing:

```
┌─────────────────────────────────────────────────────────────────┐
│                     FILE Structure (Conceptual)                  │
├─────────────────────────────────────────────────────────────────┤
│  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐        │
│  │ Buffer Pointer│  │ Buffer Size   │  │ Current Pos   │        │
│  └───────────────┘  └───────────────┘  └───────────────┘        │
│  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐        │
│  │ File Descriptor│ │ Error Flag    │  │ EOF Flag      │        │
│  └───────────────┘  └───────────────┘  └───────────────┘        │
│  ┌─────────────────────────────────────────────────────┐        │
│  │                Internal Buffer                       │        │
│  └─────────────────────────────────────────────────────┘        │
└─────────────────────────────────────────────────────────────────┘
```

Three standard streams are automatically available:

| Stream   | File Pointer | Default Device | Typical Use            |
|----------|--------------|----------------|------------------------|
| `stdin`  | Predefined   | Keyboard       | Reading user input     |
| `stdout` | Predefined   | Terminal       | Normal program output  |
| `stderr` | Predefined   | Terminal       | Error messages         |

### 2. File Opening Modes

The `fopen()` function accepts mode strings that control how the file is accessed:

```c
FILE *fopen(const char *filename, const char *mode);
```

| Mode   | Description                                    | File Must Exist | Creates File |
|--------|------------------------------------------------|-----------------|--------------|
| `"r"`  | Read text                                      | Yes             | No           |
| `"w"`  | Write text (truncates existing)                | No              | Yes          |
| `"a"`  | Append text                                    | No              | Yes          |
| `"r+"` | Read and write text                            | Yes             | No           |
| `"w+"` | Read and write text (truncates)                | No              | Yes          |
| `"a+"` | Read and append text                           | No              | Yes          |

Adding `"b"` suffix (e.g., `"rb"`, `"wb"`) enables binary mode—essential on Windows where text mode performs newline translation (`\n` ↔ `\r\n`).

### 3. Buffering Strategies

The C standard library employs buffering to reduce the number of system calls:

```
Program                  Buffer                      Disk
   │                       │                           │
   │   fprintf(fp, ...)    │                           │
   │ ─────────────────────►│                           │
   │                       │                           │
   │   fprintf(fp, ...)    │                           │
   │ ─────────────────────►│                           │
   │                       │      fflush() or          │
   │                       │      buffer full          │
   │                       │ ─────────────────────────►│
   │                       │      write() syscall      │
   │                       │                           │
```

Three buffering modes can be set using `setvbuf()`:

```c
int setvbuf(FILE *stream, char *buffer, int mode, size_t size);
```

| Mode      | Constant  | Behaviour                                        |
|-----------|-----------|--------------------------------------------------|
| Full      | `_IOFBF`  | Data written when buffer fills                   |
| Line      | `_IOLBF`  | Data written when newline encountered or buffer fills |
| Unbuffered| `_IONBF`  | Data written immediately (each character)        |

**Performance comparison** (writing 1 million integers):

| Buffering Mode | Time (typical) | System Calls |
|----------------|----------------|--------------|
| Full (8KB)     | ~0.05s         | ~125         |
| Line           | ~0.15s         | ~1,000,000   |
| Unbuffered     | ~2.50s         | ~1,000,000   |

---

## Industrial Applications

### Application 1: Log File Parsing (Unix/Linux System Administration)

```c
/* Parsing Apache access logs */
void parse_apache_log(const char *logfile) {
    FILE *fp = fopen(logfile, "r");
    if (!fp) {
        perror("Failed to open log file");
        return;
    }
    
    char line[4096];
    char ip[64], method[16], path[2048];
    int status;
    
    while (fgets(line, sizeof(line), fp)) {
        /* Common Log Format: IP - - [date] "METHOD PATH" STATUS SIZE */
        if (sscanf(line, "%63s %*s %*s %*s \"%15s %2047s", 
                   ip, method, path) == 3) {
            /* Process entry... */
        }
    }
    fclose(fp);
}
```

### Application 2: Configuration File Processing (GTK/GNOME Applications)

```c
/* Simple INI-style configuration reader */
typedef struct {
    char section[64];
    char key[64];
    char value[256];
} ConfigEntry;

int read_config(const char *filename, ConfigEntry *entries, int max_entries) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;
    
    char line[512], current_section[64] = "";
    int count = 0;
    
    while (fgets(line, sizeof(line), fp) && count < max_entries) {
        /* Skip comments and empty lines */
        if (line[0] == '#' || line[0] == ';' || line[0] == '\n')
            continue;
        
        /* Section header: [section_name] */
        if (line[0] == '[') {
            sscanf(line, "[%63[^]]]", current_section);
            continue;
        }
        
        /* Key=Value pair */
        char key[64], value[256];
        if (sscanf(line, " %63[^=] = %255[^\n]", key, value) == 2) {
            strncpy(entries[count].section, current_section, 63);
            strncpy(entries[count].key, key, 63);
            strncpy(entries[count].value, value, 255);
            count++;
        }
    }
    
    fclose(fp);
    return count;
}
```

### Application 3: CSV Data Export (Database/Spreadsheet Interoperability)

```c
/* Exporting query results to CSV format */
void export_to_csv(const char *filename, Student *students, int count) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Cannot create CSV file");
        return;
    }
    
    /* Write header row */
    fprintf(fp, "ID,Name,Year,Programme,Average\n");
    
    /* Write data rows, handling special characters */
    for (int i = 0; i < count; i++) {
        /* Escape fields containing commas or quotes */
        fprintf(fp, "%d,\"%s\",%d,%s,%.2f\n",
                students[i].id,
                students[i].name,
                students[i].year,
                students[i].programme,
                students[i].average);
    }
    
    fclose(fp);
}
```

### Application 4: Financial Data Processing (Banking/Trading Systems)

```c
/* Reading market data feed files */
typedef struct {
    char symbol[16];
    double open, high, low, close;
    long volume;
} StockQuote;

int read_market_data(const char *filename, StockQuote *quotes, int max) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;
    
    char line[256];
    int count = 0;
    
    /* Skip header line */
    fgets(line, sizeof(line), fp);
    
    while (fgets(line, sizeof(line), fp) && count < max) {
        if (sscanf(line, "%15[^,],%lf,%lf,%lf,%lf,%ld",
                   quotes[count].symbol,
                   &quotes[count].open,
                   &quotes[count].high,
                   &quotes[count].low,
                   &quotes[count].close,
                   &quotes[count].volume) == 6) {
            count++;
        }
    }
    
    fclose(fp);
    return count;
}
```

---

## Laboratory Exercises

### Exercise 1: Student Grade Processor

Design and implement a programme that reads student records from a text file and performs statistical analysis.

**Requirements:**
- Read student data from `data/studgrades.txt` in the format: `ID Name Year Programme Grade1 Grade2 ... GradeN`
- Calculate each student's average grade
- Identify the top-performing student
- Count students by programme
- Write a summary report to `output/report.txt`

**Input format:**
```
1122334 Antonio Clim 2022 1050 5 6 7 8 9 10 7 6 6 5
222222 Badass Vader 2022 1051 9 10 10 10 10 9 6 10 10 9
```

**Expected output capabilities:**
- Per-student statistics (average, min, max grades)
- Programme-level aggregation
- Formatted report with headers and alignment

### Exercise 2: CSV Transformer

Create a utility that reads CSV files, performs data transformations and outputs in multiple formats.

**Requirements:**
- Parse CSV files with proper handling of quoted fields
- Support filtering by column values
- Support sorting by specified column
- Output in CSV, tab-separated, or fixed-width format
- Handle malformed input gracefully with error reporting

---

## Compilation and Execution

### Building the Project

```bash
# Build all targets
make all

# Build specific target
make example1
make exercise1
make exercise2

# Build with debugging symbols
make debug
```

### Running Programmes

```bash
# Run the complete example
make run

# Run with sample data
./example1

# Run specific exercise
./exercise1 < data/studgrades.txt

# Run with Valgrind memory checking
make valgrind
```

### Testing

```bash
# Run automated tests
make test

# Verbose test output
make test-verbose

# Run smoke tests for instructor reference solutions
make test-solutions
```

The regression harness used by `make test` is intentionally strict: it executes
the student-facing binaries with deterministic flags and compares stdout against
the canonical snapshots under `tests/` using `diff -u`. This arrangement
privileges reproducibility over cosmetic freedom, which is pedagogically useful
when discussing experimental control, buffering and output normalisation.

In particular the exercises implement a `--test` mode that suppresses decorative
banners and other non-essential output so that the remaining lines constitute a
stable behavioural contract.

---

## Command-Line Interface (Reproducible Experiments)

Although the laboratory sheet can be followed with the default bundled data
files, both exercises expose a minimal command-line interface in order to
support automated testing and to facilitate controlled comparisons.

### Exercise 1 (Student Grade Processor)

```bash
# Default run (reads data/studgrades.txt and writes output/report.txt)
./exercise1

# Deterministic run for regression
./exercise1 --input tests/test1_input.txt --output output/report.txt --test
```

Supported options:

- `--input <path>` selects the input record file
- `--output <path>` selects the report path
- `--test` produces minimal, deterministic stdout

### Exercise 2 (CSV Transformer)

```bash
# Default run (reads data/products.csv, filters Electronics)
./exercise2

# Deterministic run for regression
./exercise2 --input tests/test2_input.txt --category TestCat --test
```

Supported options:

- `--input <path>` selects the CSV file
- `--category <name>` selects the category string used for filtering
- `--test` produces deterministic stdout suitable for snapshot diffs

---

## Algorithmic Deep Dive (Parsing, Aggregation and Sorting)

This section makes explicit the computational content that is easy to gloss
over when focusing solely on C syntax. The exercises are, at their core,
instances of the same pattern: **tokenise → validate → transform → aggregate →
report**.

### 1. Record Grammars (Operational EBNF)

For rigorous reasoning, the line formats can be described with a lightweight,
operational grammar. The following EBNF is deliberately pragmatic: it mirrors
what the implementation accepts rather than asserting an idealised standard.

#### 1.1 Student records (space-delimited)

```text
student_line  ::= id SP name SP year SP programme (SP grade)*
id            ::= DIGIT+
name          ::= NONSPACE+
year          ::= DIGIT{4}
programme     ::= DIGIT+
grade         ::= DIGIT+ ('.' DIGIT+)?
SP            ::= ' ' { ' ' | '\t' }
```

The salient feature is the **unbounded suffix** `(SP grade)*`, which prevents a
single `sscanf` call from being sufficient when the number of grades is not
fixed.

#### 1.2 Product records (CSV, unquoted fields)

```text
csv_line      ::= field ',' field ',' field ',' field ',' field
field         ::= { CHAR - { ',', '\r', '\n' } }
```

The laboratory parser uses `strtok` for pedagogical clarity. This choice
imposes the usual restrictions: embedded commas and quoted fields are not
treated as part of a single field. The optional challenges discuss how one
might implement an RFC 4180 compatible scanner.

### 2. Exercise 1: Parsing with `sscanf` and the `%n` cursor

The core technique is the `%n` conversion specifier which records the number of
characters consumed so far. This turns `sscanf` into a cursor-based lexer: once
the prefix is parsed, the remainder of the string is iteratively scanned.

#### Pseudocode (C-like)

```text
function parse_student_line(line):
    offset <- 0
    matched <- sscanf(line, "%d %s %d %d%n", id, name, year, programme, offset)
    if matched != 4:
        return failure

    grades <- empty list
    cursor <- line + offset
    while length(grades) < MAX_GRADES and sscanf(cursor, "%f%n", g, delta) == 1:
        append(grades, g)
        cursor <- cursor + delta

    average <- mean(grades)
    return Student(id, name, year, programme, grades, average)
```

#### Complexity and invariants

- Let *L* be the character length of a line and *k* the number of grades.
  The scan is **O(L)** time because the cursor advances monotonically and each
  character is visited a constant number of times.
- The memory footprint is **O(1)** beyond the fixed arrays because grades are
  stored in a bounded buffer.
- The loop invariant is: `cursor` always points to the first unconsumed
  character and `grades` contains exactly the grades parsed from the prefix of
  the suffix.

### 3. Exercise 2: CSV tokenisation, validation and stable filtering

#### 3.1 Tokenisation with destructive scanning

`strtok` replaces delimiters with `\0` and returns pointers into the same buffer.
Therefore parsing must operate on a mutable copy of the original line.

```text
function parse_csv_line(line):
    strip line endings
    fields <- []
    token <- strtok(line, ",")
    while token != NULL and len(fields) < MAX_FIELDS:
        trim(token)
        append(fields, token)
        token <- strtok(NULL, ",")
    return fields
```

#### 3.2 Numeric validation with `strtod` and `strtol`

The conversion functions are used with an `endptr` so that lexical validity can
be tested. This is preferable to `atof` and `atoi` because failures can be
distinguished from legitimate zeros.

#### 3.3 Sorting and filtering as separate transformations

The demonstration flow in `exercise2` intentionally keeps the original array
intact and sorts a copy. This makes the data pipeline easier to reason about:
filtering is then **stable with respect to input order** rather than being an
artefact of the previous sort.

```text
products       <- read_products(...)
products_sorted <- copy(products)
qsort(products_sorted, compare_by_price)

filtered <- [p in products where lower(p.category) == lower(category_filter)]
```

Time complexity is **O(n log n)** for sorting and **O(n)** for filtering where
*n* is the number of products.

### 4. Cross-language parallels (for conceptual transfer)

The underlying algorithms are not tied to C. The following sketches make the
dataflow explicit without focusing on syntax.

#### Python (illustrative)

```python
import csv

with open('products.csv', newline='') as f:
    rows = list(csv.DictReader(f))

rows_sorted = sorted(rows, key=lambda r: float(r['Price']))
filtered = [r for r in rows if r['Category'].casefold() == 'Electronics'.casefold()]

total_value = sum(float(r['Price']) * int(r['Stock']) for r in rows)
```

#### C++ (illustrative)

```cpp
std::vector<Product> v = read_csv(...);
auto sorted = v;
std::sort(sorted.begin(), sorted.end(), [](auto& a, auto& b){ return a.price < b.price; });
auto filtered = v | std::views::filter([&](auto& p){ return iequals(p.category, cat); });
```

The pedagogical point is that the core operations are portable: scanning,
validation, aggregation and ordering.

---

## Directory Structure

```
week-02-text-files/
├── README.md                           # This documentation file
├── Makefile                            # Build automation
│
├── slides/
│   ├── presentation-week02.html        # Main lecture presentation
│   └── presentation-comparativ.html    # Language comparison slides
│
├── src/
│   ├── example1.c                      # Complete demonstration
│   ├── exercise1.c                     # Student grade processor
│   └── exercise2.c                     # CSV transformer
│
├── data/
│   ├── studgrades.txt                  # Student records (space-delimited)
│   ├── products.csv                    # Product catalogue (CSV)
│   └── malformed.txt                   # Test file with errors
│
├── tests/
│   ├── test1_input.txt                 # Exercise 1 test input
│   ├── test1_expected.txt              # Exercise 1 expected output
│   ├── test2_input.txt                 # Exercise 2 test input
│   └── test2_expected.txt              # Exercise 2 expected output
│
├── teme/
│   ├── homework-requirements.md        # Homework specifications
│   └── homework-extended.md            # Bonus challenges
│
└── solution/
    ├── exercise1_sol.c                 # Exercise 1 solution
    ├── exercise2_sol.c                 # Exercise 2 solution
    ├── homework1_sol.c                 # Homework 1 solution
    └── homework2_sol.c                 # Homework 2 solution
```

---

## Recommended Reading

### Essential References

1. **Kernighan, B. & Ritchie, D.** (1988). *The C Programming Language* (2nd ed.). Chapter 7: Input and Output. Prentice Hall.
2. **ISO/IEC 9899:2011**. Section 7.21: Input/output `<stdio.h>`. Available at: https://www.open-std.org/jtc1/sc22/wg14/

### Advanced Topics

3. **Stevens, W. R. & Rago, S. A.** (2013). *Advanced Programming in the UNIX Environment* (3rd ed.). Chapter 5: Standard I/O Library. Addison-Wesley.
4. **Plauger, P. J.** (1992). *The Standard C Library*. Chapters 10-12. Prentice Hall.

### Online Resources

5. **cppreference.com**: File input/output — https://en.cppreference.com/w/c/io
6. **GNU C Library Manual**: I/O on Streams — https://www.gnu.org/software/libc/manual/html_node/I_002fO-on-Streams.html
7. **RFC 4180**: Common Format and MIME Type for CSV Files — https://tools.ietf.org/html/rfc4180

---

## Self-Assessment Checklist

Before completing this week's laboratory, ensure you can:

- [ ] Explain the purpose and usage of `fopen()`, `fclose()`, `fgets()`, `fputs()`, `fprintf()` and `fscanf()`
- [ ] Differentiate between text mode and binary mode file operations
- [ ] Implement proper error handling for all file operations using `ferror()` and `perror()`
- [ ] Parse delimited text files using `sscanf()` and `strtok()` appropriately
- [ ] Handle end-of-file conditions correctly using `feof()` (after read operations)
- [ ] Configure buffering behaviour using `setvbuf()` for performance tuning
- [ ] Read files line-by-line and character-by-character
- [ ] Write formatted output to files matching specified formats
- [ ] Implement defensive programming patterns for resource cleanup
- [ ] Debug file I/O issues using appropriate diagnostic techniques

---

## Interview Preparation

### Common Interview Questions

1. **What is the difference between `fgets()` and `gets()`? Why was `gets()` removed from the C standard?**

   *Expected answer:* `gets()` provides no way to limit input length, creating buffer overflow vulnerabilities. `fgets()` accepts a size parameter to prevent overflows. `gets()` was deprecated in C99 and removed entirely in C11.

2. **What happens if you forget to call `fclose()` on a file?**

   *Expected answer:* Buffered data may not be written to disk (data loss). File descriptors are leaked (limited system resource). On programme termination, the OS typically closes handles, but relying on this is poor practice.

3. **How would you safely read a file of unknown size into memory?**

   *Expected answer:* Use `fseek()`/`ftell()` to determine file size, allocate buffer with `malloc()`, then `fread()`. Alternatively, read in chunks and dynamically grow the buffer with `realloc()`.

4. **Explain the difference between `printf()` and `fprintf()`.**

   *Expected answer:* `printf()` writes to `stdout` only. `fprintf()` writes to any specified `FILE *` stream, including files. `printf(...)` is equivalent to `fprintf(stdout, ...)`.

5. **What is the purpose of `fflush()` and when should you use it?**

   *Expected answer:* `fflush()` forces buffered output data to be written to the underlying file. Use it before expecting output to appear (e.g., before blocking input), after important writes, or when mixing buffered and unbuffered I/O.

---

## Next Week Preview

**Week 03: Binary File Processing**

Next week we shall explore binary file operations, covering `fread()` and `fwrite()` for efficient data serialisation, the `fseek()`/`ftell()` functions for random access and techniques for designing portable binary file formats. We shall examine struct padding considerations and implement a simple database engine with indexed record access.

---

*Laboratory materials prepared for Algorithms and Programming Techniques (ATP)*  
*Academy of Economic Studies, Bucharest — Faculty of Economic Cybernetics*

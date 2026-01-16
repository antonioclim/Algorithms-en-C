# Extended Challenges - Week 02

## Advanced Challenges (Optional)

Each correctly solved challenge earns **+10 bonus points** (maximum +50 points).

These challenges are designed for students who want to push beyond the core requirements and develop more advanced skills.

---

## Challenge 1: Multi-Format Data Converter (Difficulty: Medium)

### Description

Create a universal data converter that can read data from one format and output to another. Support CSV, TSV (tab-separated), JSON and fixed-width formats.

### Requirements

- Read from any supported format
- Write to any supported format
- Preserve data types where possible (numbers vs strings)
- Handle format-specific edge cases (quoted CSV fields, JSON escaping)

### Example Usage

```bash
./converter input.csv output.json
./converter input.json output.tsv
./converter input.tsv output.csv
```

### Sample Input (CSV)

```csv
name,age,city
"Smith, John",30,London
Jane Doe,25,"New York"
```

### Sample Output (JSON)

```json
[
  {"name": "Smith, John", "age": 30, "city": "London"},
  {"name": "Jane Doe", "age": 25, "city": "New York"}
]
```

### Technical Requirements

1. Implement proper CSV parsing with quote handling
2. Generate valid JSON with proper escaping
3. Support command-line format specification
4. Handle large files efficiently (streaming approach)

### Bonus Points: +10

---

## Challenge 2: Diff Utility Clone (Difficulty: Medium-Hard)

### Description

Implement a simplified version of the Unix `diff` command that compares two text files and shows the differences.

### Requirements

- Compare files line by line
- Identify added, removed and changed lines
- Output in unified diff format (similar to `diff -u`)
- Handle files of any size

### Example Usage

```bash
./mydiff file1.txt file2.txt
```

### Sample Output

```diff
--- file1.txt
+++ file2.txt
@@ -1,5 +1,5 @@
 This line is the same.
-This line was removed.
+This line was added.
 Another unchanged line.
-Old content here.
+New content here.
 Final line.
```

### Technical Requirements

1. Implement a longest common subsequence (LCS) algorithm
2. Generate context around changes
3. Handle binary files (detect and refuse)
4. Support `-c` flag for context diff format

### Bonus Points: +10

---

## Challenge 3: Real-Time Log Tail (Difficulty: Medium)

### Description

Create a `tail -f` equivalent that monitors a file for changes and displays new content in real-time.

### Requirements

- Watch a file for appended content
- Display new lines as they are added
- Support multiple files simultaneously
- Include timestamp for each new line

### Example Usage

```bash
./logtail /var/log/app.log
./logtail --follow log1.txt log2.txt
```

### Technical Requirements

1. Use `fseek`/`ftell` to track file position
2. Implement efficient polling (sleep between checks)
3. Handle file truncation (log rotation)
4. Support graceful termination (Ctrl+C)

### Hints

```c
/* Basic approach */
fseek(fp, 0, SEEK_END);
long last_pos = ftell(fp);

while (running) {
    fseek(fp, 0, SEEK_END);
    long current_pos = ftell(fp);
    
    if (current_pos > last_pos) {
        /* New content available */
        fseek(fp, last_pos, SEEK_SET);
        /* Read and display new content */
        last_pos = current_pos;
    }
    
    usleep(100000);  /* 100ms delay */
}
```

### Bonus Points: +10

---

## Challenge 4: Structured Log Parser with Regex (Difficulty: Hard)

### Description

Create a log parser that uses POSIX regular expressions to extract structured data from various log formats.

### Requirements

- Support user-defined patterns for log parsing
- Extract named fields from log entries
- Output parsed data in structured format (JSON/CSV)
- Include pattern library for common log formats

### Example Usage

```bash
# Parse Apache logs
./logparse --pattern apache access.log output.json

# Parse custom format with user regex
./logparse --regex '(\d+) - (\w+) - (.*)' custom.log output.csv
```

### Pattern Examples

```c
/* Apache Common Log Format */
const char *APACHE_PATTERN = 
    "^([^ ]+) - - \\[([^\\]]+)\\] \"([A-Z]+) ([^ ]+) ([^\"]+)\" (\\d+) (\\d+)";

/* Syslog format */
const char *SYSLOG_PATTERN =
    "^([A-Za-z]+ +\\d+ \\d+:\\d+:\\d+) ([^ ]+) ([^:]+): (.*)";
```

### Technical Requirements

1. Use POSIX `regex.h` functions (`regcomp`, `regexec`, `regfree`)
2. Support capture groups for field extraction
3. Handle regex compilation errors gracefully
4. Implement pattern caching for efficiency

### Bonus Points: +10

---

## Challenge 5: File Indexer and Search Engine (Difficulty: Hard)

### Description

Build a simple search engine that indexes text files and supports keyword searches.

### Requirements

- Build an inverted index from text files
- Support multiple search operators (AND, OR, NOT)
- Rank results by relevance
- Save and load index to/from file

### Example Usage

```bash
# Build index
./indexer --build ./documents/ index.dat

# Search
./indexer --search index.dat "programming AND algorithms"
./indexer --search index.dat "data OR information"
./indexer --search index.dat "file NOT binary"
```

### Index Structure

```
Word -> [(doc1, freq1, positions), (doc2, freq2, positions), ...]

Example:
"algorithm" -> [("file1.txt", 5, [10, 45, 89, 120, 201]), 
                ("file3.txt", 2, [33, 156])]
```

### Technical Requirements

1. Tokenise documents into words (handle punctuation)
2. Build inverted index with term frequencies
3. Implement TF-IDF ranking (optional)
4. Binary file format for index persistence
5. Efficient search with posting list intersection

### Bonus Points: +10

---

## Bonus Point System

| Challenges Completed | Total Bonus | Special Recognition |
|---------------------|-------------|---------------------|
| 1 challenge | +10 points | — |
| 2 challenges | +20 points | — |
| 3 challenges | +30 points | "File I/O Expert" badge |
| 4 challenges | +40 points | "File I/O Expert" badge |
| All 5 challenges | +50 points | "Text Processing Master" badge |

### Badge Benefits

Students earning badges receive:
- Recognition in the course hall of fame
- Priority consideration for teaching assistant positions
- Letter of recommendation upon request

---

## Submission Guidelines for Challenges

1. **Separate submission** from regular homework
2. **Include README** explaining your approach
3. **Provide test cases** demonstrating functionality
4. **Comment complex algorithms** thoroughly

### File Naming

```
challenge1_converter.c
challenge2_diff.c
challenge3_logtail.c
challenge4_logparse.c
challenge5_indexer.c
```

---

## General Tips

1. **Start simple** — Get basic functionality working before optimising.

2. **Research algorithms** — For Challenge 2 (diff), look up Longest Common Subsequence. For Challenge 5, study inverted indexes.

3. **Test extensively** — Edge cases matter. Test with empty files, huge files and special characters.

4. **Profile your code** — Use `time` command to measure performance on large inputs.

5. **Ask questions** — These are challenging problems; discussing approaches is encouraged (but implement independently).

---

These optional tasks are most valuable when treated as small research problems.
State your assumptions explicitly, justify algorithmic choices with complexity
arguments and validate behaviour with adversarial test cases.

---

## Appendix: Algorithmic notes and pseudocode sketches

The following material is optional but it is intended to raise the technical
ceiling of the challenges. The pseudocode uses C-like conventions while
remaining language-agnostic.

### A. Quote-aware CSV parsing (finite-state scanner)

The minimal `strtok` approach used in the laboratory fails on inputs such as
`"Smith, John"` because commas may occur inside quoted fields. A robust parser
is most naturally expressed as a single-pass state machine.

**State variables**

- `in_quotes`: boolean
- `field_start`: pointer or index into the line buffer

**Pseudocode**

```text
function split_csv_quoted(line):
  fields <- []
  in_quotes <- false
  field <- empty string builder

  for each character c in line:
    if c == '"':
        in_quotes <- not in_quotes
        continue
    if c == ',' and not in_quotes:
        fields.append(field.trim())
        field.clear()
        continue
    field.append(c)

  fields.append(field.trim())
  return fields
```

This scanner is **O(L)** in the line length and does not require recursion. To
support escaped quotes (`""` within a quoted field) refine the quote transition
to detect the doubled quote pattern.

### B. Unified diff generation (LCS and Myers style reasoning)

The human-friendly `diff -u` format requires a mapping from two line sequences
to an edit script. Two classical routes are widely discussed:

- Dynamic programming for Longest Common Subsequence (LCS), typically **O(nm)**
  time and **O(nm)** space unless optimised
- Myers' O(ND) algorithm where *D* is the edit distance, often faster in
  practice for similar files

**LCS core recurrence**

```text
if A[i] == B[j]:
    dp[i][j] = dp[i-1][j-1] + 1
else:
    dp[i][j] = max(dp[i-1][j], dp[i][j-1])
```

From the completed `dp` table an edit script is reconstructed by backtracking.

### C. Large file handling (external sorting)

For challenges where the working set exceeds memory, use external sorting:

```text
function external_sort(file, chunk_bytes):
  runs <- []
  while not EOF(file):
      chunk <- read up to chunk_bytes
      sort(chunk)
      run_file <- write chunk to temporary file
      runs.append(run_file)
  return k_way_merge(runs)
```

The k-way merge is naturally implemented with a min-heap over the current head
element of each run.

### D. Inverted index construction (for the file indexer)

An inverted index maps tokens to a postings list of document identifiers. A
simple implementation is:

```text
for each document d:
  tokens <- normalise(tokenise(read(d)))
  for each token t in tokens:
      index[t].add(d)
```

If you want to keep the index on disk, consider writing postings in sorted
order with delta encoding for compression.

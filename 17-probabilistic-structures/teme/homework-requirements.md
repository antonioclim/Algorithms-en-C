# Week 17: Homework Requirements

## Probabilistic Data Structures for Big Data

### Submission Guidelines

- **Deadline:** As announced on the course platform
- **Format:** C source files with Makefile
- **Naming:** `homework1.c`, `homework2.c`
- **Compilation:** Must compile with `gcc -Wall -Wextra -std=c11 -pedantic`

---

## Homework 1: URL Deduplication System (50 points)

### Context

Web crawlers must efficiently track which URLs have already been visited to avoid redundant fetches. With billions of URLs, exact storage becomes impractical. Bloom filters offer a space-efficient solution with controlled false positive rates.

### Objective

Implement a URL deduplication system using Bloom filters that processes a stream of URLs and identifies duplicates.

### Requirements

#### Part A: Bloom Filter Implementation (20 points)

Implement the following functions:

```c
typedef struct {
    uint8_t *bits;
    size_t num_bits;
    size_t num_hashes;
    size_t num_items;
} BloomFilter;

/* Create Bloom filter with specified parameters */
BloomFilter *bloom_create(size_t num_bits, size_t num_hashes);

/* Create optimal Bloom filter given expected items and FP rate */
BloomFilter *bloom_create_optimal(size_t expected_items, double fp_rate);

/* Free Bloom filter resources */
void bloom_free(BloomFilter *bf);

/* Insert element into Bloom filter */
void bloom_insert(BloomFilter *bf, const void *data, size_t len);

/* Query Bloom filter for membership */
bool bloom_query(const BloomFilter *bf, const void *data, size_t len);
```

**Optimal parameter formulas:**

```
m = -n × ln(p) / (ln(2))²    [bits]
k = (m/n) × ln(2)            [hash functions]
```

Where:
- `n` = expected number of items
- `p` = desired false positive probability
- `m` = number of bits
- `k` = number of hash functions

#### Part B: URL Stream Processing (15 points)

Implement a URL processing pipeline:

1. Read URLs from input file (one per line)
2. Normalise URLs (lowercase, remove trailing slashes)
3. Check if URL is a duplicate using Bloom filter
4. Track statistics: total URLs, unique URLs, duplicates

#### Part C: Comparison with Exact Method (10 points)

Implement an exact hash set for comparison:

1. Process same URL stream with hash set
2. Count actual unique URLs and duplicates
3. Calculate false positive rate of Bloom filter
4. Report memory usage of both approaches

#### Part D: Analysis Report (5 points)

Output a summary including:
- Total URLs processed
- Unique URLs (Bloom estimate vs exact)
- False positive count and rate
- Memory usage comparison
- Space savings factor

### Expected Output Format

```
╔═══════════════════════════════════════════════════════════════╗
║           URL DEDUPLICATION SYSTEM RESULTS                    ║
╠═══════════════════════════════════════════════════════════════╣
║  Total URLs processed:        100,000                         ║
║  Unique URLs (Bloom):         72,341                          ║
║  Unique URLs (exact):         72,156                          ║
║  False positives:             185                             ║
║  False positive rate:         0.26%                           ║
║  Bloom memory:                87.5 KB                         ║
║  Hash set memory:             2,847.3 KB                      ║
║  Space savings:               32.5x                           ║
╚═══════════════════════════════════════════════════════════════╝
```

### Test Cases

Your implementation will be tested with:
1. Small dataset (1,000 URLs) — correctness verification
2. Medium dataset (100,000 URLs) — performance check
3. High duplicate rate (50%) — FP rate validation
4. Low duplicate rate (5%) — memory efficiency check

---

## Homework 2: Database Query Optimiser Simulator (50 points)

### Context

Modern databases use probabilistic data structures to accelerate query execution. Bloom filters help skip unnecessary disk reads, whilst Skip Lists provide efficient sorted access for range queries.

### Objective

Build a database query optimiser simulator demonstrating how probabilistic structures improve query performance.

### Requirements

#### Part A: Skip List Implementation (20 points)

Implement a Skip List for sorted index operations:

```c
typedef struct SkipNode {
    int key;
    char value[64];
    int level;
    struct SkipNode **forward;
} SkipNode;

typedef struct {
    SkipNode *header;
    int level;
    size_t num_items;
} SkipList;

/* Create empty skip list */
SkipList *skip_create(void);

/* Free skip list */
void skip_free(SkipList *sl);

/* Search for key */
SkipNode *skip_search(const SkipList *sl, int key);

/* Insert key-value pair */
bool skip_insert(SkipList *sl, int key, const char *value);

/* Range query: find all keys in [low, high] */
size_t skip_range_query(const SkipList *sl, int low, int high,
                        int *results, size_t max_results);
```

**Implementation requirements:**
- Maximum level: 16
- Probability for level increase: 0.5
- Expected O(log n) search time

#### Part B: Query Optimisation (15 points)

Implement optimised query operations:

1. **Point Query with Bloom Pre-check:**
   - Check Bloom filter before index lookup
   - Skip index access if Bloom says "not present"

2. **Range Query with Skip List:**
   - Use skip list for efficient range scans
   - Return all matching keys in sorted order

3. **Join Optimisation:**
   - Use Bloom filter on inner table's primary key
   - Skip inner loop iterations for non-matching keys

#### Part C: Benchmark Framework (10 points)

Implement benchmarking for:
1. Point queries: Bloom-optimised vs full scan
2. Range queries: Skip list vs linear scan
3. Joins: Bloom-optimised vs nested loop

Track metrics:
- Rows examined
- Index/Bloom lookups
- Execution time
- False positive count

#### Part D: Analysis Report (5 points)

Generate performance comparison showing:
- Query speedup factors
- Memory overhead
- Trade-off analysis

### Expected Output Format

```
┌────────────────────────────────────────────────────────────────┐
│                 JOIN BENCHMARK                                 │
├────────────────────────────────────────────────────────────────┤
│  Outer table: 50,000 rows                                      │
│  Inner table: 10,000 rows                                      │
├────────────────────────────────────────────────────────────────┤
│  Bloom-Optimised:                                              │
│    Rows examined:       127,453                                │
│    Time:                12.3 ms                                │
├────────────────────────────────────────────────────────────────┤
│  Naive Nested Loop:                                            │
│    Rows examined:       500,000,000                            │
│    Time:                8,547.2 ms                             │
├────────────────────────────────────────────────────────────────┤
│  Speedup:               694x fewer rows, 695x faster           │
└────────────────────────────────────────────────────────────────┘
```

---

## Grading Criteria

### Homework 1 (50 points)

| Component | Points | Criteria |
|-----------|--------|----------|
| Bloom Filter Implementation | 20 | Correctness, optimal parameters |
| URL Processing | 15 | Normalisation, stream handling |
| Exact Comparison | 10 | Accurate FP measurement |
| Analysis Report | 5 | Complete statistics |

### Homework 2 (50 points)

| Component | Points | Criteria |
|-----------|--------|----------|
| Skip List Implementation | 20 | Correctness, O(log n) performance |
| Query Optimisation | 15 | Proper Bloom integration |
| Benchmark Framework | 10 | Accurate measurements |
| Analysis Report | 5 | Clear comparison |

### Deductions

- **Compilation warnings:** -2 points each (max -10)
- **Memory leaks:** -5 points per leak
- **Incorrect output format:** -5 points
- **Missing documentation:** -5 points
- **Late submission:** -10% per day

---

## Resources

### Recommended Reading

1. **Bloom Filters:** Bloom, B. H. (1970). "Space/Time Trade-offs in Hash Coding with Allowable Errors"
2. **Skip Lists:** Pugh, W. (1990). "Skip Lists: A Probabilistic Alternative to Balanced Trees"
3. **Database Optimisation:** Ramakrishnan & Gehrke, "Database Management Systems", Chapter 13

### Hash Function Reference

Use MurmurHash or similar for consistent hashing:

```c
uint64_t murmur_hash64(const void *data, size_t len, uint64_t seed);
```

### Useful Mathematical Constants

```c
#define LN2     0.693147180559945309  /* ln(2) */
#define LN2_SQ  0.480453013918201424  /* (ln(2))² */
```

---

## Submission Checklist

Before submitting, verify:

- [ ] Code compiles without warnings
- [ ] All test cases pass
- [ ] Memory checked with Valgrind (no leaks)
- [ ] Output matches expected format
- [ ] Analysis report included
- [ ] Code is well-documented
- [ ] Makefile included with appropriate targets

---

*ATP Course — Academy of Economic Studies Bucharest*
*Week 17: Probabilistic Data Structures for Big Data*

# Week 17: Probabilistic Data Structures for Big Data

## 🎯 Learning Objectives

By the end of this laboratory session, students will be able to:

1. **Remember** the fundamental definitions and parameters of probabilistic data structures, recalling the key trade-offs between accuracy, space efficiency and query time inherent in Bloom filters, Count-Min Sketches and HyperLogLog estimators.

2. **Understand** how hash functions underpin probabilistic structures, explaining the relationship between false positive rates and memory usage in membership testing, and articulating why approximate answers are acceptable in Big Data contexts.

3. **Apply** Bloom filters for efficient set membership testing, implementing the structure with optimal parameter selection based on desired false positive probability and expected element count.

4. **Analyse** the error bounds and space-time trade-offs of Count-Min Sketch for frequency estimation, comparing theoretical guarantees with empirical performance on streaming data.

5. **Evaluate** when probabilistic approaches are appropriate versus exact methods, considering application requirements for precision, available memory and query latency constraints.

6. **Create** complete implementations of Bloom filter, Count-Min Sketch, HyperLogLog and Skip List in C, integrating them into practical applications such as spell-checking, network traffic analysis and cardinality estimation.

---

## 📜 Historical Context

The emergence of probabilistic data structures represents a paradigm shift in computer science—the deliberate acceptance of controlled error in exchange for dramatic improvements in space and time efficiency. This trade-off became essential as datasets grew beyond the capacity of exact methods.

**Burton Howard Bloom** introduced the Bloom filter in 1970 whilst working at Computer Usage Company. His seminal paper "Space/Time Trade-offs in Hash Coding with Allowable Errors" appeared in *Communications of the ACM* and described a structure that could test set membership using far less space than storing the actual elements, at the cost of occasional false positives. Bloom filters found immediate application in spell-checkers and database systems, and remain ubiquitous in modern distributed systems including web browsers (safe browsing lists), databases (LSM trees) and distributed caches.

The early 2000s witnessed an explosion of research into streaming algorithms and probabilistic counting. **Philippe Flajolet**, a French computer scientist renowned for his work in analytic combinatorics, developed increasingly sophisticated cardinality estimation algorithms. His collaboration with Nigel Martin produced the probabilistic counting paper in 1985, which evolved through FM sketches into LogLog (2003) and finally **HyperLogLog** (2007). These structures enable counting billions of unique elements using mere kilobytes of memory—a remarkable achievement that powers analytics systems at Google, Facebook and countless other organisations.

**Graham Cormode** and **S. Muthukrishnan** introduced the Count-Min Sketch in 2005, providing a simple yet powerful method for frequency estimation in data streams. Their work enabled efficient detection of heavy hitters (frequent items) and answered frequency queries with provable error bounds, becoming essential infrastructure for network monitoring and database query optimisation.

### Key Figure: Philippe Flajolet (1948–2011)

Philippe Flajolet was a French computer scientist and director of research at INRIA (Institut National de Recherche en Informatique et en Automatique). His contributions to the analysis of algorithms and combinatorics earned him international recognition, including the Grand Prize of the French Academy of Sciences. Beyond HyperLogLog, Flajolet pioneered analytical methods for algorithm complexity and data structure analysis.

> *"Randomisation is a versatile tool in algorithm design. It introduces a degree of unpredictability that can be harnessed to achieve efficiency that deterministic approaches cannot match."*
> — Philippe Flajolet, Lecture at ICALP 2006

### Additional Key Figures

**Burton H. Bloom** — Inventor of the Bloom filter (1970), demonstrating that probabilistic answers can enable otherwise infeasible computations.

**William Pugh** (1963–) — Professor at the University of Maryland who invented the Skip List in 1990, providing a probabilistic alternative to balanced trees with comparable expected performance but simpler implementation. Skip Lists are employed in Redis and LevelDB.

---

## 📚 Theoretical Foundations

### 1. The Probabilistic Trade-off

Traditional data structures provide exact answers but often require space proportional to the data size. Probabilistic structures accept small, bounded error in exchange for dramatic space reduction.

```
    ═══════════════════════════════════════════════════════════════════════
                    EXACT vs PROBABILISTIC TRADE-OFF
    ═══════════════════════════════════════════════════════════════════════

    Task: Store 1 billion elements for membership testing

    ┌────────────────────────────────────────────────────────────────────┐
    │  EXACT: Hash Set                                                   │
    │  • Storage: ~8 GB (8 bytes per element + overhead)                │
    │  • Query time: O(1) amortised                                     │
    │  • Error: None                                                     │
    └────────────────────────────────────────────────────────────────────┘

    ┌────────────────────────────────────────────────────────────────────┐
    │  PROBABILISTIC: Bloom Filter                                       │
    │  • Storage: ~1.2 GB (10 bits per element for 1% FP rate)         │
    │  • Query time: O(k) where k = number of hash functions            │
    │  • Error: 1% false positives, NO false negatives                  │
    └────────────────────────────────────────────────────────────────────┘

    Space reduction: 6.7× with acceptable error for most applications
```

### 2. Hash Function Requirements

Probabilistic structures depend critically on high-quality hash functions. The requirements include:

```
    ╔═══════════════════════════════════════════════════════════════════╗
    ║  HASH FUNCTION PROPERTIES                                         ║
    ╠═══════════════════════════════════════════════════════════════════╣
    ║                                                                   ║
    ║  1. UNIFORMITY                                                    ║
    ║     Output should be uniformly distributed across range           ║
    ║     P(h(x) = y) ≈ 1/m for any x and output y in [0, m)           ║
    ║                                                                   ║
    ║  2. INDEPENDENCE (or pairwise independence)                       ║
    ║     For distinct x₁, x₂: h(x₁) and h(x₂) behave independently    ║
    ║                                                                   ║
    ║  3. EFFICIENCY                                                    ║
    ║     Computable in O(|x|) time where |x| is input size            ║
    ║                                                                   ║
    ║  4. DETERMINISM                                                   ║
    ║     Same input always produces same output                        ║
    ║                                                                   ║
    ╚═══════════════════════════════════════════════════════════════════╝
```

**Generating Multiple Hash Functions:**

Rather than implementing k independent hash functions, the Kirsch-Mitzenmacher technique generates k hashes from just two base hashes:

```c
/* Kirsch-Mitzenmacher optimisation for generating k hashes */
uint64_t get_hash(const void *data, size_t len, int i, 
                  uint64_t h1, uint64_t h2) {
    return h1 + i * h2;
}
```

### 3. Bloom Filter

A Bloom filter is a bit array of m bits with k hash functions. It supports two operations: INSERT and QUERY.

```
    ═══════════════════════════════════════════════════════════════════════
                              BLOOM FILTER STRUCTURE
    ═══════════════════════════════════════════════════════════════════════

    m bits, k hash functions

    ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
    │ 0 │ 0 │ 1 │ 0 │ 1 │ 0 │ 0 │ 1 │ 0 │ 0 │ 1 │ 0 │ 0 │ 1 │ 0 │ 0 │
    └───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘
      0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15

    INSERT("cat"):
        h₁("cat") mod 16 = 2   →  Set bit 2
        h₂("cat") mod 16 = 7   →  Set bit 7
        h₃("cat") mod 16 = 13  →  Set bit 13

    QUERY("dog"):
        h₁("dog") mod 16 = 4   →  Bit 4 is 1  ✓
        h₂("dog") mod 16 = 10  →  Bit 10 is 1 ✓
        h₃("dog") mod 16 = 15  →  Bit 15 is 0 ✗
        Result: DEFINITELY NOT in set

    QUERY("rat"):
        h₁("rat") mod 16 = 2   →  Bit 2 is 1  ✓
        h₂("rat") mod 16 = 4   →  Bit 4 is 1  ✓
        h₃("rat") mod 16 = 7   →  Bit 7 is 1  ✓
        Result: PROBABLY in set (could be false positive)
```

**False Positive Probability:**

```
    After inserting n elements into an m-bit filter with k hash functions:

    Probability a specific bit is 0:  (1 - 1/m)^(kn) ≈ e^(-kn/m)

    False positive probability:  P(FP) = (1 - e^(-kn/m))^k

    Optimal number of hash functions:  k = (m/n) × ln(2) ≈ 0.693 × (m/n)

    For 1% false positive rate:  m/n ≈ 9.6 bits per element
```

### 4. Count-Min Sketch

A Count-Min Sketch maintains d arrays of w counters each, using d hash functions. It estimates frequency of elements in a data stream.

```
    ═══════════════════════════════════════════════════════════════════════
                           COUNT-MIN SKETCH STRUCTURE
    ═══════════════════════════════════════════════════════════════════════

    d rows (depth), w columns (width)

         0     1     2     3     4     5     6     7     (w=8)
       ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
    h₁ │  0  │  3  │  0  │  2  │  0  │  1  │  0  │  0  │  Row 0
       ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
    h₂ │  1  │  0  │  0  │  3  │  2  │  0  │  0  │  0  │  Row 1
       ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
    h₃ │  0  │  0  │  2  │  0  │  0  │  0  │  3  │  1  │  Row 2
       ├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
    h₄ │  0  │  1  │  0  │  0  │  3  │  0  │  2  │  0  │  Row 3
       └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
                                                          (d=4)

    UPDATE(x, c):  For each row i: table[i][hᵢ(x) mod w] += c

    QUERY(x):  Return min over all rows i of table[i][hᵢ(x) mod w]
```

**Error Guarantees:**

```
    With width w = ⌈e/ε⌉ and depth d = ⌈ln(1/δ)⌉:

    For any element x with true frequency fₓ:
        f̂ₓ ≥ fₓ                    (never underestimates)
        f̂ₓ ≤ fₓ + ε × N            with probability ≥ 1-δ

    where N is total count of all updates

    Example: ε = 0.001, δ = 0.01
        w = ⌈e/0.001⌉ = 2719
        d = ⌈ln(100)⌉ = 5
        Space: 2719 × 5 × 4 bytes ≈ 54 KB
```

### 5. HyperLogLog

HyperLogLog estimates the cardinality (number of distinct elements) of a multiset using only O(log log n) bits per register.

```
    ═══════════════════════════════════════════════════════════════════════
                            HYPERLOGLOG ALGORITHM
    ═══════════════════════════════════════════════════════════════════════

    Core Insight: For uniformly distributed hash values, the position of
    the leftmost 1-bit follows a geometric distribution that encodes
    information about the number of distinct elements seen.

    ┌────────────────────────────────────────────────────────────────────┐
    │  Hash an element to a binary string:                              │
    │                                                                    │
    │  h("apple") = 0001011010110101...                                 │
    │               ↑                                                    │
    │               First 1 at position 4 (ρ = 4)                       │
    │                                                                    │
    │  If we observe ρ = 4, we estimate ~2⁴ = 16 distinct elements     │
    └────────────────────────────────────────────────────────────────────┘

    To reduce variance, HyperLogLog uses m = 2^p registers:

    1. Use first p bits of hash to select register
    2. Store max ρ(remaining bits) in that register
    3. Estimate = α_m × m² / (Σ 2^(-M[j]))  (harmonic mean)

    ┌──────────────────────────────────────────────────────────────────┐
    │  With m = 16384 registers (p = 14):                              │
    │  • Space: 16384 × 6 bits ≈ 12 KB                                │
    │  • Standard error: 1.04 / √m ≈ 0.81%                            │
    │  • Can count billions of distinct elements                       │
    └──────────────────────────────────────────────────────────────────┘
```

### 6. Skip List

A Skip List is a probabilistic data structure that provides expected O(log n) search, insertion and deletion by maintaining multiple levels of linked lists.

```
    ═══════════════════════════════════════════════════════════════════════
                              SKIP LIST STRUCTURE
    ═══════════════════════════════════════════════════════════════════════

    Level 3:  HEAD ─────────────────────────────────────→ 50 ───────→ NIL
                    │                                     │
    Level 2:  HEAD ─────────→ 20 ─────────────────────→ 50 ───────→ NIL
                    │         │                           │
    Level 1:  HEAD ─────────→ 20 ───────→ 30 ─────────→ 50 → 60 ──→ NIL
                    │         │           │               │    │
    Level 0:  HEAD → 10 → 20 → 25 → 30 → 40 → 50 → 60 → 70 ──→ NIL

    Search(45):
    1. Start at HEAD, Level 3
    2. Can reach 50? 50 > 45, so go down to Level 2
    3. Can reach 20? 20 < 45, so move right to 20
    4. Can reach 50? 50 > 45, so go down to Level 1
    5. Can reach 30? 30 < 45, so move right to 30
    6. Can reach 50? 50 > 45, so go down to Level 0
    7. Can reach 40? 40 < 45, so move right to 40
    8. Can reach 50? 50 > 45, not found
    Result: 45 not in list (would be between 40 and 50)

    Expected levels: log₂(n)
    Expected search time: O(log n)
```

---

## 🏭 Industrial Applications

### 1. Web Browser Safe Browsing

Google Chrome uses Bloom filters to test URLs against a list of known malicious sites without downloading the entire database.

```c
/* Simplified safe browsing check using Bloom filter */
typedef struct {
    BloomFilter *malicious_urls;
    size_t version;
} SafeBrowsingDB;

bool check_url_safety(SafeBrowsingDB *db, const char *url) {
    /* Fast local check with Bloom filter */
    if (!bloom_filter_query(db->malicious_urls, url, strlen(url))) {
        return true;  /* Definitely safe */
    }
    
    /* Possible match - verify with server (rare) */
    return verify_with_server(url);
}
```

### 2. Database Query Optimisation

Databases use Bloom filters to avoid expensive disk reads when testing key existence in SSTables.

```c
/* LSM tree level with Bloom filter */
typedef struct {
    BloomFilter *key_filter;
    FILE *data_file;
    size_t num_keys;
} SSTable;

bool sstable_might_contain(SSTable *sst, const char *key) {
    return bloom_filter_query(sst->key_filter, key, strlen(key));
}

/* Query avoids disk read if Bloom filter returns false */
Value *query_key(SSTable *tables[], int num_tables, const char *key) {
    for (int i = 0; i < num_tables; i++) {
        if (sstable_might_contain(tables[i], key)) {
            Value *v = sstable_read(tables[i], key);  /* Disk I/O */
            if (v) return v;
        }
    }
    return NULL;
}
```

### 3. Network Traffic Analysis

Count-Min Sketch enables real-time detection of heavy hitters (high-frequency items) in network traffic.

```c
/* Real-time heavy hitter detection */
typedef struct {
    CountMinSketch *sketch;
    uint64_t total_packets;
    double threshold;  /* e.g., 0.001 = 0.1% of traffic */
} TrafficMonitor;

void process_packet(TrafficMonitor *tm, uint32_t src_ip) {
    cms_update(tm->sketch, &src_ip, sizeof(src_ip), 1);
    tm->total_packets++;
}

bool is_heavy_hitter(TrafficMonitor *tm, uint32_t ip) {
    uint64_t count = cms_query(tm->sketch, &ip, sizeof(ip));
    return count > tm->threshold * tm->total_packets;
}
```

### 4. Unique Visitor Counting

HyperLogLog enables counting unique visitors across billions of page views using minimal memory.

```c
/* Analytics: unique visitors per page */
typedef struct {
    char *page_url;
    HyperLogLog *visitor_hll;
} PageStats;

void record_visit(PageStats *stats, const char *visitor_id) {
    hyperloglog_add(stats->visitor_hll, visitor_id, strlen(visitor_id));
}

uint64_t get_unique_visitors(PageStats *stats) {
    return hyperloglog_count(stats->visitor_hll);
}

/* Memory: ~12 KB per page vs potentially GB for exact counting */
```

---

## 💻 Laboratory Exercises

### Exercise 1: Spell Checker with Bloom Filter

Implement a spell-checking system using a Bloom filter to store a dictionary of valid words. Your implementation must achieve a false positive rate below 1% whilst minimising memory usage.

**Requirements:**
- Implement Bloom filter with configurable size and hash count
- Calculate optimal parameters given dictionary size and desired FP rate
- Load dictionary from text file (one word per line)
- Check words from input text and report suspected misspellings
- Measure and report actual false positive rate
- Compare memory usage with a hash set implementation

**Input Format (data/dictionary.txt):**
```
a
abandon
ability
able
about
above
...
```

**File:** `src/exercise1.c`

### Exercise 2: Network Traffic Analysis with Count-Min Sketch and HyperLogLog

Process a simulated network traffic log to identify heavy hitters (frequent IPs) and count unique source addresses. Compare your probabilistic results with exact counting.

**Requirements:**
- Implement Count-Min Sketch with configurable (ε, δ) parameters
- Implement HyperLogLog with configurable precision
- Process streaming log data (simulated packets)
- Detect heavy hitters exceeding a threshold frequency
- Estimate cardinality of unique source IPs
- Report memory usage and accuracy versus exact methods

**Input Format (data/traffic_log.txt):**
```
timestamp,src_ip,dst_ip,bytes
1704067200,192.168.1.100,10.0.0.1,1500
1704067201,192.168.1.101,10.0.0.2,500
1704067201,192.168.1.100,10.0.0.1,1200
...
```

**File:** `src/exercise2.c`

---

## 🔧 Compilation and Execution

```bash
# Build all targets
make all

# Run the complete demonstration
make run

# Execute Exercise 1 (Spell Checker)
make run-ex1

# Execute Exercise 2 (Traffic Analysis)
make run-ex2

# Run automated tests
make test

# Check for memory leaks
make valgrind

# Build and run in Docker
make docker

# Run benchmarks
make benchmark

# Clean build artefacts
make clean

# Display help
make help
```

---

## 📁 Directory Structure

```
17-probabilistic-structures/
├── README.md                           # This documentation
├── Makefile                            # Build automation
├── Dockerfile                          # Container definition
├── docker-compose.yml                  # Multi-container setup
│
├── slides/
│   ├── presentation-week17.html        # Main lecture (~42 slides)
│   └── presentation-comparativ.html    # Language comparison (~12 slides)
│
├── src/
│   ├── example1.c                      # Complete probabilistic structures demo
│   ├── exercise1.c                     # Spell checker exercise
│   └── exercise2.c                     # Traffic analysis exercise
│
├── data/
│   ├── dictionary.txt                  # English dictionary (~100K words)
│   ├── traffic_log.txt                 # Simulated network traffic
│   └── test_words.txt                  # Words for spell-checking test
│
├── tests/
│   ├── test1_input.txt                 # Test input for Exercise 1
│   ├── test1_expected.txt              # Expected output for Exercise 1
│   ├── test2_input.txt                 # Test input for Exercise 2
│   └── test2_expected.txt              # Expected output for Exercise 2
│
├── python_comparison/
│   ├── bloom_filter.py                 # Python Bloom filter for comparison
│   ├── count_min_sketch.py             # Python CMS implementation
│   └── hyperloglog.py                  # Python HyperLogLog
│
├── teme/
│   ├── homework-requirements.md        # Main homework assignments
│   └── homework-extended.md            # Bonus challenges
│
└── solution/
    ├── exercise1_sol.c                 # Solution for Exercise 1
    ├── exercise2_sol.c                 # Solution for Exercise 2
    ├── homework1_sol.c                 # Solution for Homework 1
    └── homework2_sol.c                 # Solution for Homework 2
```

---

## 📖 Recommended Reading

### Essential
- Cormen, T.H. et al. *Introduction to Algorithms*, 4th ed., Chapter 11 (Hash Tables) provides foundation for hash functions
- Mitzenmacher, M. & Upfal, E. *Probability and Computing*, Chapter 5 (Balls, Bins, and Random Graphs)
- Bloom, B.H. "Space/Time Trade-offs in Hash Coding with Allowable Errors", *Communications of the ACM* 13(7), 1970

### Advanced
- Flajolet, P. et al. "HyperLogLog: The Analysis of a Near-Optimal Cardinality Estimation Algorithm", *AOFA* 2007
- Cormode, G. & Muthukrishnan, S. "An Improved Data Stream Summary: The Count-Min Sketch and its Applications", *Journal of Algorithms* 55(1), 2005
- Pugh, W. "Skip Lists: A Probabilistic Alternative to Balanced Trees", *Communications of the ACM* 33(6), 1990

### Online Resources
- Redis documentation on probabilistic data structures: https://redis.io/docs/data-types/probabilistic/
- Streaming algorithms course: http://www.cs.dartmouth.edu/~ac/Teach/CS35-Spring20/
- Visualisation of Bloom filters: https://www.jasondavies.com/bloomfilter/

---

## ✅ Self-Assessment Checklist

Before submitting your laboratory work, verify that you can:

- [ ] Explain the false positive probability formula for Bloom filters
- [ ] Calculate optimal Bloom filter parameters given n and desired FP rate
- [ ] Implement bit manipulation for setting and testing bits efficiently
- [ ] Describe the error guarantees of Count-Min Sketch
- [ ] Explain why HyperLogLog uses harmonic mean for aggregation
- [ ] Implement the Kirsch-Mitzenmacher technique for generating multiple hashes
- [ ] Compare space requirements of exact vs probabilistic counting
- [ ] Design a Skip List with expected O(log n) operations
- [ ] Identify real-world applications suitable for each structure
- [ ] Analyse the trade-offs between accuracy and resource usage

---

## 💼 Interview Preparation

**Question 1:** *"How does a Bloom filter work, and what are its limitations?"*

A Bloom filter uses k hash functions to map elements to k positions in a bit array. To insert, set all k bits; to query, check if all k bits are set. False positives occur when unrelated insertions happen to set all k bits for a non-member. Limitations: (1) No deletion (unless using counting variant), (2) Cannot enumerate members, (3) False positive rate increases as filter fills. Key insight: false positive rate is (1 - e^(-kn/m))^k where n is elements, m is bits, k is hash functions.

**Question 2:** *"How would you count unique visitors to a website with 1 billion page views per day?"*

HyperLogLog is ideal: it uses ~12 KB to estimate cardinality with ~1% error. Each visitor ID is hashed; the position of the first 1-bit in the hash determines which register to update. The cardinality estimate combines all registers using a harmonic mean formula. This uses constant memory regardless of actual cardinality, whereas exact methods would require memory proportional to unique visitors.

**Question 3:** *"What is a Count-Min Sketch and when would you use it?"*

Count-Min Sketch is a frequency estimation structure using d arrays of w counters. To update, increment counters at positions h₁(x), h₂(x), ..., h_d(x). To query, return the minimum of these d counters. It never underestimates but may overestimate. Use cases: finding heavy hitters in network traffic, approximate frequency counts in databases and trending topic detection. Parameters w = e/ε and d = ln(1/δ) give error guarantee that estimate ≤ true_count + εN with probability 1-δ.

**Question 4:** *"Why might you choose a Skip List over a balanced BST?"*

Skip Lists offer: (1) Simpler implementation—no complex rotations, (2) Lock-free concurrent versions are easier to implement, (3) Good cache locality when levels are stored together, (4) Range queries are natural. Trade-offs: slightly higher space overhead and probabilistic rather than worst-case guarantees. Used in Redis (sorted sets) and LevelDB (in-memory table).

**Question 5:** *"How do you choose between exact and probabilistic data structures?"*

Consider: (1) Is the exact answer critical (financial calculations) or is approximation acceptable (analytics)? (2) What are memory constraints—probabilistic structures often use 10-100× less memory. (3) Is the operation latency-sensitive? (4) What error rate is acceptable? For web analytics, 1% error in unique visitor counts is usually acceptable; for billing, exact counts are required.

---

## 🔗 Next Week Preview

**Week 18: Machine Learning Fundamentals in C**

We shall explore fundamental machine learning algorithms implemented in pure C, including linear regression, k-nearest neighbours, k-means clustering and the perceptron. This module bridges the gap between algorithmic thinking and data-driven approaches, demonstrating how classical techniques form the foundation of modern ML systems. Topics include gradient descent optimisation, distance metrics and the mathematics of learning from data.

---

## 🔌 Real Hardware Extension (Optional)

> **Note:** This section is for students who wish to experiment with physical hardware.
> Arduino kits (ESP32, Arduino Due) are available for borrowing from the faculty library.

Probabilistic data structures are particularly valuable on resource-constrained embedded systems where memory is limited but approximate answers are acceptable.

**ESP32 Implementation Opportunities:**

1. **IoT Sensor Deduplication**: Use a Bloom filter to track which sensor readings have been transmitted, avoiding duplicate messages.

2. **Edge Analytics**: HyperLogLog for counting unique events locally before aggregation at a central server.

3. **Memory-Efficient Logging**: Count-Min Sketch for tracking error frequencies without storing individual events.

**Getting Started:**
- Arduino IDE with ESP32 board support
- Adapt the C implementations (replace malloc with static allocation)
- Use hardware random number generator for hash seeding

---

*Laboratory materials prepared for ADSC Course*
*Academy of Economic Studies — CSIE Bucharest*
*Alternative learning kit for non-formal education*

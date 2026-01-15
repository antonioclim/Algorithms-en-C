# Week 17: Probabilistic Data Structures — Extended Challenges

## 🎯 Overview

These extended challenges are designed for students who wish to explore probabilistic data structures beyond the standard curriculum. Each challenge builds upon the foundational concepts covered in the laboratory exercises whilst introducing more advanced techniques used in production systems.

**Prerequisites:**
- Completion of Exercise 1 (Bloom Filter Spell Checker)
- Completion of Exercise 2 (Network Traffic Analysis)
- Solid understanding of hash functions and probability theory
- Familiarity with memory-efficient data representation

**Time Estimate:** Each challenge requires approximately 8-12 hours of focused work.

---

## 🏆 Challenge 1: Cuckoo Filter Implementation

### Background

Cuckoo filters, introduced by Fan et al. (2014), offer several advantages over Bloom filters: they support deletion, provide higher lookup performance and achieve better space efficiency for applications requiring low false positive rates (less than 3%).

The name derives from the cuckoo hashing technique, where items can "kick out" existing items to find their place, much like cuckoo birds displacing eggs from other nests.

### Requirements

1. **Core Implementation (40 points)**
   - Implement a cuckoo filter with configurable fingerprint size (8, 16, or 32 bits)
   - Support insert, lookup and delete operations
   - Implement the partial-key cuckoo hashing technique
   - Handle insertion failures gracefully with a maximum kick count

2. **Performance Comparison (30 points)**
   - Benchmark against your Bloom filter implementation
   - Measure and compare:
     - Insertion throughput (items/second)
     - Lookup throughput (items/second)
     - False positive rate at various load factors
     - Memory consumption per item
   - Generate comparison plots (ASCII or external tool)

3. **Semi-Sorting Optimisation (30 points)**
   - Implement the semi-sorted buckets variant
   - Demonstrate space savings through fingerprint compression
   - Compare performance with the basic implementation

### Expected Output

```
═══════════════════════════════════════════════════════════════════
                    CUCKOO FILTER BENCHMARK
═══════════════════════════════════════════════════════════════════

Configuration:
  Fingerprint size: 12 bits
  Bucket size: 4 entries
  Number of buckets: 262144
  Target capacity: 1,000,000 items

Insertion Results:
  Items inserted: 950,234
  Load factor: 95.02%
  Insertion failures: 0
  Average kicks per insert: 1.23

Lookup Performance:
  True positives: 950,234 / 950,234 (100.00%)
  False positives: 487 / 100,000 (0.49%)
  Lookup throughput: 12.4M ops/sec

Comparison with Bloom Filter (same FP rate):
  Structure          Memory    Lookup     Insert     Delete
  ────────────────────────────────────────────────────────────
  Cuckoo Filter      1.52 MB   12.4M/s    8.7M/s     Yes
  Bloom Filter       1.78 MB   10.2M/s    9.1M/s     No
═══════════════════════════════════════════════════════════════════
```

### References

- Fan, B., Andersen, D. G., Kaminsky, M., & Mitzenmacher, M. (2014). "Cuckoo Filter: Practically Better Than Bloom"
- Pagh, R., & Rodler, F. F. (2004). "Cuckoo Hashing"

---

## 🏆 Challenge 2: MinHash for Set Similarity

### Background

MinHash is a locality-sensitive hashing technique for estimating the Jaccard similarity between sets. Originally developed by Andrei Broder (1997) for detecting duplicate web pages at AltaVista, MinHash enables efficient similarity detection in large document collections.

The Jaccard similarity of sets A and B is defined as:
```
J(A, B) = |A ∩ B| / |A ∪ B|
```

MinHash provides an unbiased estimator of this value using only O(k) space per set, where k is the number of hash functions.

### Requirements

1. **MinHash Signature Generation (35 points)**
   - Implement signature generation with configurable number of hash functions
   - Support both word-level and character n-gram shingling
   - Optimise using the permutation technique (one hash function, k permutations)

2. **Document Similarity System (35 points)**
   - Build a system to detect near-duplicate documents
   - Process a collection of at least 1000 documents
   - Output pairs with similarity above a configurable threshold
   - Report precision and recall against exact Jaccard computation

3. **Locality-Sensitive Hashing (30 points)**
   - Implement LSH banding technique for efficient candidate generation
   - Analyse the trade-off between bands (b) and rows (r)
   - Demonstrate scalability with larger document collections

### Expected Output

```
═══════════════════════════════════════════════════════════════════
                    MINHASH DOCUMENT SIMILARITY
═══════════════════════════════════════════════════════════════════

Configuration:
  Signature size (k): 128 hash functions
  Shingle size: 3 words
  LSH bands: 16 (8 rows each)
  Documents processed: 1,247

Similarity Detection Results:
  Candidate pairs generated: 3,891
  True similar pairs found: 156
  Similarity threshold: 0.80

Sample Similar Documents:
  ┌─────────────────────────────────────────────────────────────┐
  │ doc_0042.txt ←→ doc_0891.txt                               │
  │ MinHash estimate: 0.867                                     │
  │ Exact Jaccard:    0.854                                     │
  │ Error:            1.52%                                     │
  └─────────────────────────────────────────────────────────────┘

Accuracy Metrics:
  Mean absolute error: 0.023
  Precision at 0.80: 94.3%
  Recall at 0.80: 91.7%

Performance:
  Signature generation: 1,247 docs in 2.3 seconds
  LSH candidate generation: 0.4 seconds
  Full pairwise comparison would require: 777,531 comparisons
  Actual comparisons made: 3,891 (0.50%)
═══════════════════════════════════════════════════════════════════
```

### References

- Broder, A. Z. (1997). "On the resemblance and containment of documents"
- Leskovec, J., Rajaraman, A., & Ullman, J. D. "Mining of Massive Datasets", Chapter 3

---

## 🏆 Challenge 3: Quotient Filter with Metadata

### Background

Quotient filters, introduced by Bender et al. (2012), offer an alternative to Bloom filters with several advantages: cache-friendly linear probing, support for deletion and efficient merging of filters. They achieve this by storing fingerprints in a compact hash table using the Robin Hood hashing variant.

### Requirements

1. **Basic Quotient Filter (35 points)**
   - Implement with quotient and remainder separation
   - Support insert, lookup and delete operations
   - Handle cluster management with proper metadata bits:
     - is_occupied: slot is the canonical slot for some key
     - is_continuation: slot is part of a run
     - is_shifted: element is not in its canonical slot

2. **Counting Extension (35 points)**
   - Extend to support counting (track frequency)
   - Implement efficient increment/decrement operations
   - Handle counter overflow gracefully

3. **Merge Operation (30 points)**
   - Implement filter merging for distributed systems
   - Demonstrate merging filters from multiple nodes
   - Verify correctness after merge operations

### Expected Output

```
═══════════════════════════════════════════════════════════════════
                    QUOTIENT FILTER DEMONSTRATION
═══════════════════════════════════════════════════════════════════

Filter Configuration:
  Quotient bits (q): 20
  Remainder bits (r): 8
  Total slots: 1,048,576
  Maximum load factor: 95%

Insertion Test:
  Items inserted: 900,000
  Current load: 85.8%
  Average cluster length: 2.3
  Maximum cluster length: 12

Counting Extension Test:
  ┌─────────────────────────────────────────────────────────────┐
  │ Item             Insertions    Deletions    Final Count     │
  ├─────────────────────────────────────────────────────────────┤
  │ "apple"          5             2            3               │
  │ "banana"         10            10           0 (removed)     │
  │ "cherry"         1             0            1               │
  └─────────────────────────────────────────────────────────────┘

Merge Operation Test:
  Filter A: 500,000 items
  Filter B: 500,000 items
  Merged filter: 847,293 unique items
  Merge time: 0.23 seconds

Comparison with Bloom Filter:
  Operation        Quotient Filter    Bloom Filter
  ───────────────────────────────────────────────
  Insert           8.2M ops/sec       9.1M ops/sec
  Lookup           11.3M ops/sec      10.2M ops/sec
  Delete           Yes                No
  Merge            Yes                No (union only)
  Space (1M items) 3.5 MB             1.8 MB
═══════════════════════════════════════════════════════════════════
```

### References

- Bender, M. A., et al. (2012). "Don't Thrash: How to Cache Your Hash on Flash"
- Pandey, P., et al. (2017). "A General-Purpose Counting Filter"

---

## 🏆 Challenge 4: t-Digest for Streaming Percentiles

### Background

t-Digest, developed by Ted Dunning (2019), is a probabilistic data structure for accurate estimation of percentiles from streaming data. Unlike traditional methods that require storing all data points, t-Digest maintains a compressed representation that preserves accuracy at the tails of the distribution.

### Requirements

1. **Core t-Digest Implementation (40 points)**
   - Implement the centroid-based representation
   - Support add, merge and quantile operations
   - Use the scaling function for adaptive compression
   - Implement the merge algorithm for combining digests

2. **Streaming Percentile Computation (30 points)**
   - Process a stream of 10 million values
   - Compute percentiles: 50th (median), 90th, 95th, 99th, 99.9th
   - Compare accuracy against exact computation on a sample
   - Measure memory consumption

3. **Distributed Aggregation (30 points)**
   - Simulate multiple data streams (e.g. from different servers)
   - Merge t-Digests from multiple sources
   - Verify accuracy of merged percentiles
   - Analyse error bounds

### Expected Output

```
═══════════════════════════════════════════════════════════════════
                    T-DIGEST STREAMING PERCENTILES
═══════════════════════════════════════════════════════════════════

Configuration:
  Compression parameter (δ): 100
  Maximum centroids: ~200

Stream Processing:
  Values processed: 10,000,000
  Distribution: Log-normal (μ=0, σ=1)
  Processing time: 4.7 seconds
  Throughput: 2.1M values/sec

Percentile Accuracy:
  ┌─────────────────────────────────────────────────────────────┐
  │ Percentile    t-Digest      Exact         Error (%)        │
  ├─────────────────────────────────────────────────────────────┤
  │ 50th          1.000         1.000         0.00%            │
  │ 90th          3.597         3.602         0.14%            │
  │ 95th          5.142         5.151         0.17%            │
  │ 99th          10.203        10.240        0.36%            │
  │ 99.9th        20.847        20.959        0.53%            │
  └─────────────────────────────────────────────────────────────┘

Memory Usage:
  t-Digest: 4.2 KB (208 centroids)
  Exact (sorted): 76.3 MB
  Compression ratio: 18,143:1

Distributed Test (8 streams merged):
  Per-stream values: 1,250,000
  Merge time: 0.003 seconds
  Combined 99th percentile error: 0.41%
═══════════════════════════════════════════════════════════════════
```

### References

- Dunning, T. & Ertl, O. (2019). "Computing Extremely Accurate Quantiles Using t-Digests"
- Apache DataSketches library documentation

---

## 🏆 Challenge 5: Distributed HyperLogLog with Redis Protocol

### Background

This challenge combines the HyperLogLog algorithm with network programming to build a distributed cardinality estimation system. You will implement a subset of the Redis HyperLogLog commands (PFADD, PFCOUNT, PFMERGE) and demonstrate distributed counting across multiple simulated nodes.

### Requirements

1. **Enhanced HyperLogLog (30 points)**
   - Implement HyperLogLog with sparse representation for small cardinalities
   - Switch to dense representation when threshold exceeded
   - Implement bias correction for improved accuracy
   - Support serialisation/deserialisation for network transfer

2. **Network Protocol (40 points)**
   - Implement a simple TCP server accepting commands:
     - `PFADD key element [element ...]` — Add elements
     - `PFCOUNT key [key ...]` — Get cardinality (merge if multiple keys)
     - `PFMERGE destkey sourcekey [sourcekey ...]` — Merge HLLs
   - Support multiple concurrent clients using select() or poll()
   - Implement proper protocol parsing and error handling

3. **Distributed Simulation (30 points)**
   - Run 4 server instances on different ports
   - Partition data across servers using consistent hashing
   - Implement client that aggregates results from all servers
   - Demonstrate correct cardinality estimation for distributed data

### Expected Output

```
═══════════════════════════════════════════════════════════════════
              DISTRIBUTED HYPERLOGLOG SYSTEM
═══════════════════════════════════════════════════════════════════

Server Configuration:
  Nodes: 4 (ports 6379-6382)
  HLL precision: 14 bits (16384 registers)
  Sparse threshold: 3000 elements

Client Session:
  > PFADD visitors user001 user002 user003
  (integer) 1

  > PFCOUNT visitors
  (integer) 3

  > PFADD visitors user001 user004 user005
  (integer) 1

  > PFCOUNT visitors
  (integer) 5

Distributed Test:
  ┌─────────────────────────────────────────────────────────────┐
  │ Node        Elements Added    Local Estimate                │
  ├─────────────────────────────────────────────────────────────┤
  │ :6379       2,512,847         2,498,234                     │
  │ :6380       2,487,153         2,491,872                     │
  │ :6381       2,501,234         2,507,891                     │
  │ :6382       2,498,766         2,502,103                     │
  └─────────────────────────────────────────────────────────────┘

  Total unique elements (actual): 10,000,000
  Merged estimate: 9,987,234
  Error: 0.13%

  PFMERGE all_visitors node0 node1 node2 node3
  OK

  PFCOUNT all_visitors
  (integer) 9987234

Network Performance:
  PFADD throughput: 125,000 ops/sec
  PFCOUNT latency: 0.12 ms
  PFMERGE (4 nodes): 0.34 ms
═══════════════════════════════════════════════════════════════════
```

### References

- Flajolet, P., et al. (2007). "HyperLogLog: the analysis of a near-optimal cardinality estimation algorithm"
- Redis PFADD/PFCOUNT/PFMERGE documentation
- Heule, S., Nunkesser, M., & Hall, A. (2013). "HyperLogLog in Practice"

---

## 📋 Submission Guidelines

### For Each Challenge

1. **Source Code**
   - Well-documented C11 code
   - Compiles with `gcc -Wall -Wextra -std=c11 -pedantic`
   - No memory leaks (verified with Valgrind)

2. **Documentation**
   - README explaining your implementation choices
   - Complexity analysis (time and space)
   - Known limitations

3. **Test Cases**
   - Unit tests for core operations
   - Integration tests for complete workflows
   - Performance benchmarks

4. **Report**
   - 2-3 pages describing your approach
   - Performance results with analysis
   - Comparison with theoretical expectations

### Grading Criteria

| Criterion | Weight |
|-----------|--------|
| Correctness | 40% |
| Performance | 25% |
| Code quality | 20% |
| Documentation | 15% |

### Bonus Points

- Implementation in a second language (Python, Rust) for comparison: +10%
- Visualisation of data structure behaviour: +5%
- Integration with real-world dataset: +5%

---

## 🔗 Additional Resources

### Online Tools and Visualisations

- [Bloom Filter Calculator](https://hur.st/bloomfilter/)
- [HyperLogLog Demo](https://djhworld.github.io/hyperloglog/)
- [Probabilistic Data Structures Visualised](https://github.com/tylertreat/BoomFilters)

### Research Papers

1. Kirsch, A., & Mitzenmacher, M. (2006). "Less Hashing, Same Performance"
2. Dillinger, P. C., & Manolios, P. (2004). "Bloom Filters in Probabilistic Verification"
3. Cormode, G., & Muthukrishnan, S. (2005). "An Improved Data Stream Summary: The Count-Min Sketch and its Applications"

### Open Source Implementations

- Google Guava (Java): BloomFilter, HyperLogLog
- Redis (C): HyperLogLog implementation
- Apache DataSketches (Java): Comprehensive library

---

*Extended challenges prepared for ATP Course*
*Academy of Economic Studies - CSIE Bucharest*
*Alternative learning kit for non-formal education*

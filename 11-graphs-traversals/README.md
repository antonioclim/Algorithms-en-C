# Week 11: Hash Tables

## 🎯 Learning Objectives

Upon successful completion of this laboratory, students will be able to:

1. **Remember** the fundamental terminology of hash tables including buckets, load factor, collision and hash function, recalling their definitions and standard implementations in C.

2. **Understand** the mathematical principles underlying hash function design, explaining why certain functions distribute keys uniformly whilst others produce clustering.

3. **Apply** chaining and open addressing collision resolution strategies to implement functional hash tables capable of insert, search and delete operations in O(1) average time.

4. **Analyse** the performance characteristics of different hash functions and collision resolution methods, determining load factor thresholds that trigger rehashing.

5. **Evaluate** trade-offs between chaining and open addressing, selecting the appropriate collision resolution strategy based on memory constraints and expected access patterns.

6. **Create** a complete hash table implementation featuring dynamic resizing, custom hash functions and efficient memory management suitable for production use.

---

## 📜 Historical Context

The concept of hashing emerged from the practical necessity of rapid data retrieval in computing systems of the 1950s. The fundamental insight—that arithmetic transformations could map arbitrary keys to array indices—revolutionised how programmers thought about data organisation. Unlike sequential or tree-based structures requiring logarithmic search times, hashing promised constant-time access, a property that seemed almost magical to early computer scientists.

The seminal contributions to hash table theory arrived from multiple directions simultaneously. Hans Peter Luhn at IBM developed chaining techniques around 1953 for his document indexing systems. Arnold Dumey published the first formal analysis of hash table performance in 1956, establishing the mathematical foundations that remain relevant today. The term "hashing" itself derives from the notion of "chopping" keys into smaller, more manageable pieces—a culinary metaphor that captured the essence of the transformation process.

The evolution of collision resolution strategies represents a fascinating chapter in algorithmic history. Linear probing, proposed by Amdahl, Boehme and Rochester in 1954 for IBM's assembly language, offered simplicity but suffered from primary clustering. Quadratic probing and double hashing emerged as refinements addressing these deficiencies. The theoretical breakthrough came with the development of universal hashing by Carter and Wegman in 1979, which provided probabilistic guarantees against adversarial inputs—a concern that has become increasingly relevant in the era of algorithmic complexity attacks.

### Key Figure: Hans Peter Luhn (1896–1964)

Hans Peter Luhn, a German-American researcher at IBM, stands as one of the founding figures of information science. His contributions extend far beyond hashing to include the Luhn algorithm for checksum validation (still used in credit card verification), keyword in context (KWIC) indexing and early work on automatic abstracting.

Luhn's approach to hashing emerged from his work on mechanised library systems. He recognised that the explosion of scientific literature demanded automated retrieval systems capable of locating documents within seconds rather than hours. His chained hash table implementation for the IBM 701 demonstrated that practical systems could achieve near-constant lookup times, fundamentally changing expectations for database performance.

> *"The acquisition of knowledge is not just a matter of volume but of accessibility. A fact unretrievable is a fact unknown."*
> — Hans Peter Luhn

---

## 📚 Theoretical Foundations

### 1. The Hash Function: Mathematical Foundation

A hash function h: U → {0, 1, ..., m-1} maps keys from a universe U to indices in a table of size m. The ideal hash function exhibits three properties: determinism (identical keys always produce identical indices), uniformity (keys distribute evenly across buckets) and efficiency (computation requires constant time).

```
Universe of Keys (U)                    Hash Table (size m)
┌─────────────────────┐                ┌───┬───────────────┐
│  "apple"            │ ──h(k)──────►  │ 0 │               │
│  "banana"           │ ──h(k)──────►  │ 1 │ → "banana"    │
│  "cherry"           │ ──h(k)──────►  │ 2 │               │
│  "date"             │ ──h(k)──┐      │ 3 │ → "apple"     │
│  "elderberry"       │ ─────┐  │      │ 4 │ → "date"      │
│  ...                │      │  └────► │ 5 │               │
└─────────────────────┘      │         │ 6 │ → "cherry"    │
                             └───────► │ 7 │ → "elderberry"│
                                       └───┴───────────────┘
```

The division method computes h(k) = k mod m, where selecting m as a prime number distant from powers of two minimises clustering. The multiplication method h(k) = ⌊m(kA mod 1)⌋, where A ≈ (√5 - 1)/2 ≈ 0.6180339887 (the golden ratio's fractional part), provides excellent distribution regardless of m.

**Common Hash Functions for Strings:**

```c
/* Division method with polynomial accumulation */
unsigned int hash_division(const char *key, int table_size) {
    unsigned int hash = 0;
    while (*key) {
        hash = hash * 31 + *key++;  /* 31 is prime, good distribution */
    }
    return hash % table_size;
}

/* djb2 by Daniel J. Bernstein - excellent empirical performance */
unsigned int hash_djb2(const char *key) {
    unsigned int hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;  /* hash * 33 + c */
    }
    return hash;
}

/* FNV-1a (Fowler-Noll-Vo) - fast and well-distributed */
unsigned int hash_fnv1a(const char *key) {
    unsigned int hash = 2166136261u;  /* FNV offset basis */
    while (*key) {
        hash ^= (unsigned char)*key++;
        hash *= 16777619u;  /* FNV prime */
    }
    return hash;
}
```

### 2. Collision Resolution: Chaining vs Open Addressing

When two keys k₁ ≠ k₂ satisfy h(k₁) = h(k₂), a collision occurs. Two fundamental strategies address this inevitability:

**Chaining (Separate Chaining):**
Each bucket contains a linked list of entries sharing that hash value. Operations traverse the list at the target bucket.

```
Hash Table with Chaining (m = 7)
┌───┬──────────────────────────────────────────────┐
│ 0 │ → NULL                                        │
├───┼──────────────────────────────────────────────┤
│ 1 │ → ["bob":42] → ["eve":28] → NULL             │
├───┼──────────────────────────────────────────────┤
│ 2 │ → ["alice":35] → NULL                        │
├───┼──────────────────────────────────────────────┤
│ 3 │ → NULL                                        │
├───┼──────────────────────────────────────────────┤
│ 4 │ → ["carol":19] → ["dave":51] → NULL          │
├───┼──────────────────────────────────────────────┤
│ 5 │ → NULL                                        │
├───┼──────────────────────────────────────────────┤
│ 6 │ → ["frank":44] → NULL                        │
└───┴──────────────────────────────────────────────┘
```

**Open Addressing:**
All entries reside within the table itself. Upon collision, a probing sequence locates an alternative slot.

- **Linear Probing:** h(k, i) = (h(k) + i) mod m — simple but causes primary clustering
- **Quadratic Probing:** h(k, i) = (h(k) + c₁i + c₂i²) mod m — reduces clustering but may not probe all slots
- **Double Hashing:** h(k, i) = (h₁(k) + i·h₂(k)) mod m — excellent distribution, h₂(k) must be coprime to m

```
Open Addressing with Linear Probing (m = 7)
Insert sequence: "alice"(h=2), "bob"(h=1), "carol"(h=4), "dave"(h=2), "eve"(h=1)

Step 1-3: Direct insertions
┌───┬─────────┐
│ 0 │  empty  │
│ 1 │  "bob"  │
│ 2 │ "alice" │
│ 3 │  empty  │
│ 4 │ "carol" │
│ 5 │  empty  │
│ 6 │  empty  │
└───┴─────────┘

Step 4: "dave" hashes to 2 (occupied) → probe to 3
Step 5: "eve" hashes to 1 (occupied) → probe to 2 (occupied) → probe to 3 (occupied) → slot 5

Final state:
┌───┬─────────┐
│ 0 │  empty  │
│ 1 │  "bob"  │
│ 2 │ "alice" │
│ 3 │ "dave"  │  ← displaced from slot 2
│ 4 │ "carol" │
│ 5 │  "eve"  │  ← displaced from slots 1, 2, 3, 4
│ 6 │  empty  │
└───┴─────────┘
```

### 3. Performance Analysis and Load Factor

The load factor α = n/m (entries divided by table size) determines expected performance:

| Metric | Chaining | Open Addressing |
|--------|----------|-----------------|
| Successful search | 1 + α/2 | (1/α)·ln(1/(1-α)) |
| Unsuccessful search | 1 + α | 1/(1-α) |
| Insert | 1 + α | 1/(1-α) |
| Space overhead | O(n) for pointers | None |
| Maximum α | Any (degrades gracefully) | Must be < 1 |
| Recommended α | 0.75–1.0 | 0.5–0.7 |

**Rehashing** doubles the table size and reinserts all entries when α exceeds a threshold. This amortised O(n) operation maintains O(1) average performance.

```
Load Factor Impact on Probe Count (Open Addressing)
α     | Expected probes (unsuccessful)
------+-------------------------------
0.50  | 2.0
0.70  | 3.3
0.80  | 5.0
0.90  | 10.0
0.95  | 20.0
0.99  | 100.0
```

---

## 🏭 Industrial Applications

### 1. Database Indexing (PostgreSQL)

PostgreSQL employs hash indexes for equality comparisons on large tables. The implementation uses extensible hashing with overflow pages.

```c
/* Simplified PostgreSQL-style hash bucket structure */
typedef struct HashBucket {
    uint32_t      hashvalue;
    ItemPointer   tuple_ptr;    /* Points to actual row in heap */
    struct HashBucket *next;
} HashBucket;

/* Hash index lookup (simplified) */
bool hash_index_search(HashIndex *index, Datum key, ItemPointer *result) {
    uint32_t hashval = hash_any((unsigned char *)&key, sizeof(key));
    uint32_t bucket_num = hashval & index->bucket_mask;
    
    HashBucket *bucket = index->buckets[bucket_num];
    while (bucket != NULL) {
        if (bucket->hashvalue == hashval) {
            *result = bucket->tuple_ptr;
            return true;
        }
        bucket = bucket->next;
    }
    return false;
}
```

### 2. Symbol Tables in Compilers (GCC)

The GNU Compiler Collection uses hash tables for symbol lookup during parsing and code generation.

```c
/* GCC-style identifier hash table entry */
struct ident_hash_entry {
    const char *name;
    size_t length;
    unsigned int hash;
    struct tree_node *decl;     /* Declaration node */
    struct ident_hash_entry *next;
};

/* Interning an identifier */
struct ident_hash_entry *intern_identifier(const char *name, size_t len) {
    unsigned int hash = hash_string(name, len);
    unsigned int slot = hash % IDENT_HASH_SIZE;
    
    struct ident_hash_entry *entry = ident_hash_table[slot];
    while (entry) {
        if (entry->hash == hash && entry->length == len &&
            memcmp(entry->name, name, len) == 0) {
            return entry;  /* Already interned */
        }
        entry = entry->next;
    }
    
    /* Create new entry */
    entry = allocate_entry();
    entry->name = intern_string(name, len);
    entry->length = len;
    entry->hash = hash;
    entry->decl = NULL;
    entry->next = ident_hash_table[slot];
    ident_hash_table[slot] = entry;
    return entry;
}
```

### 3. Network Routing Tables (Linux Kernel)

The Linux kernel uses hash tables for efficient route lookup in the networking stack.

```c
/* Simplified Linux FIB (Forwarding Information Base) hash lookup */
struct fib_table {
    struct hlist_head *hash;
    unsigned int       hash_mask;
};

struct fib_node *fib_lookup(struct fib_table *table, uint32_t dest_ip) {
    unsigned int hash = jhash_1word(dest_ip, 0) & table->hash_mask;
    struct hlist_node *node;
    struct fib_node *fib;
    
    hlist_for_each_entry(fib, node, &table->hash[hash], hlist) {
        if (fib->prefix == (dest_ip & fib->mask)) {
            return fib;
        }
    }
    return NULL;  /* No route found */
}
```

### 4. In-Memory Caching (Redis/Memcached Style)

Modern caching systems rely heavily on hash tables for O(1) key-value access.

```c
/* Memcached-style cache entry */
typedef struct cache_entry {
    char                *key;
    void                *value;
    size_t               value_size;
    time_t               expiry;
    struct cache_entry  *next;
} cache_entry_t;

typedef struct {
    cache_entry_t **buckets;
    size_t          size;
    size_t          count;
    pthread_rwlock_t lock;
} cache_t;

/* Thread-safe cache retrieval */
void *cache_get(cache_t *cache, const char *key) {
    pthread_rwlock_rdlock(&cache->lock);
    
    unsigned int idx = hash_djb2(key) % cache->size;
    cache_entry_t *entry = cache->buckets[idx];
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            if (entry->expiry > time(NULL)) {
                void *result = entry->value;
                pthread_rwlock_unlock(&cache->lock);
                return result;
            }
            break;  /* Expired */
        }
        entry = entry->next;
    }
    
    pthread_rwlock_unlock(&cache->lock);
    return NULL;
}
```

### 5. Spell Checkers and Bloom Filters

Hash tables underpin spell-checking algorithms, whilst Bloom filters (probabilistic hash structures) enable efficient membership testing.

```c
/* Bloom filter using multiple hash functions */
typedef struct {
    uint8_t *bits;
    size_t   size;        /* Number of bits */
    int      num_hashes;  /* Number of hash functions */
} bloom_filter_t;

void bloom_add(bloom_filter_t *bf, const char *word) {
    for (int i = 0; i < bf->num_hashes; i++) {
        /* Different seed for each hash function */
        unsigned int hash = hash_murmur(word, strlen(word), i);
        size_t idx = hash % bf->size;
        bf->bits[idx / 8] |= (1 << (idx % 8));
    }
}

bool bloom_might_contain(bloom_filter_t *bf, const char *word) {
    for (int i = 0; i < bf->num_hashes; i++) {
        unsigned int hash = hash_murmur(word, strlen(word), i);
        size_t idx = hash % bf->size;
        if (!(bf->bits[idx / 8] & (1 << (idx % 8)))) {
            return false;  /* Definitely not present */
        }
    }
    return true;  /* Possibly present (may be false positive) */
}
```

---

## 💻 Laboratory Exercises

### Exercise 1: Chained Hash Table for Student Records

Implement a hash table using separate chaining to store student records. The system must support insertion, searching by student ID and deletion operations.

**Requirements:**
1. Define a `Student` structure with fields: `id` (string), `name` (string), `grade` (float)
2. Implement `hash_function()` using the djb2 algorithm
3. Create functions: `ht_create()`, `ht_insert()`, `ht_search()`, `ht_delete()`, `ht_destroy()`
4. Handle collisions using linked lists at each bucket
5. Compute and display load factor after each modification
6. Read student data from `data/students.txt`

**Input Format (students.txt):**
```
S001 Alice 9.5
S002 Bob 7.8
S003 Carol 8.2
...
```

**Expected Operations:**
```
> insert S004 Dave 6.9
Inserted: S004 at bucket 3 (chain length: 2)
Load factor: 0.57

> search S002
Found: Bob (grade: 7.80) at bucket 1

> delete S001
Deleted: Alice from bucket 5
Load factor: 0.43
```

### Exercise 2: Open Addressing with Double Hashing

Implement a hash table using open addressing with double hashing for a word frequency counter. The system must handle rehashing when the load factor exceeds 0.7.

**Requirements:**
1. Implement primary hash using FNV-1a
2. Implement secondary hash ensuring coprimality with table size
3. Track probe counts for performance analysis
4. Implement automatic rehashing when α > 0.7
5. Handle deletion using tombstone markers
6. Process text file and count word occurrences
7. Display top 10 most frequent words

**Input:** `data/text_sample.txt` (any English text)

**Expected Output:**
```
Hash Table Statistics:
  Table size: 127
  Entries: 87
  Load factor: 0.69
  Total probes: 142
  Average probes per insert: 1.63

Top 10 Words:
  1. the     (45 occurrences)
  2. and     (32 occurrences)
  3. to      (28 occurrences)
  ...
```

---

## 🔧 Compilation and Execution

```bash
# Build all targets
make

# Build specific exercise
make exercise1
make exercise2

# Run complete example demonstration
make run

# Execute with test data
make test

# Check for memory leaks
make valgrind

# Clean build artefacts
make clean

# Display available targets
make help
```

**Manual Compilation:**
```bash
gcc -Wall -Wextra -std=c11 -g -o example1 src/example1.c
gcc -Wall -Wextra -std=c11 -g -o exercise1 src/exercise1.c
gcc -Wall -Wextra -std=c11 -g -o exercise2 src/exercise2.c
```

---

## 📁 Directory Structure

```
week-11-hash-tables/
├── README.md                           # This documentation
├── Makefile                            # Build automation
│
├── slides/
│   ├── presentation-week11.html        # Main lecture (35 slides)
│   └── presentation-comparativ.html    # Language comparison (12 slides)
│
├── src/
│   ├── example1.c                      # Complete hash table demonstration
│   ├── exercise1.c                     # Chained hash table exercise
│   └── exercise2.c                     # Open addressing exercise
│
├── data/
│   ├── students.txt                    # Student records for exercise 1
│   └── text_sample.txt                 # Text corpus for exercise 2
│
├── tests/
│   ├── test1_input.txt                 # Test commands for exercise 1
│   ├── test1_expected.txt              # Expected output for exercise 1
│   ├── test2_input.txt                 # Test commands for exercise 2
│   └── test2_expected.txt              # Expected output for exercise 2
│
├── teme/
│   ├── homework-requirements.md        # Homework assignments (100 points)
│   └── homework-extended.md            # Bonus challenges (+50 points)
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

1. **Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C.** (2009). *Introduction to Algorithms* (3rd ed.). MIT Press. Chapter 11: Hash Tables.

2. **Sedgewick, R., & Wayne, K.** (2011). *Algorithms* (4th ed.). Addison-Wesley. Section 3.4: Hash Tables.

3. **Knuth, D. E.** (1998). *The Art of Computer Programming, Volume 3: Sorting and Searching* (2nd ed.). Addison-Wesley. Section 6.4: Hashing.

### Advanced Topics

4. **Pagh, R., & Rodler, F. F.** (2004). Cuckoo Hashing. *Journal of Algorithms*, 51(2), 122-144.

5. **Dietzfelbinger, M., et al.** (1994). Dynamic Perfect Hashing: Upper and Lower Bounds. *SIAM Journal on Computing*, 23(4), 738-761.

6. **Carter, J. L., & Wegman, M. N.** (1979). Universal Classes of Hash Functions. *Journal of Computer and System Sciences*, 18(2), 143-154.

### Online Resources

7. **Hash Functions** - https://www.partow.net/programming/hashfunctions/
8. **SMHasher** (Hash function test suite) - https://github.com/rurban/smhasher
9. **Bloom Filter Calculator** - https://hur.st/bloomfilter/

---

## ✅ Self-Assessment Checklist

Before submitting your laboratory work, verify:

- [ ] I can explain the difference between a hash function and a hash table
- [ ] I understand why prime table sizes improve distribution
- [ ] I can implement chaining with proper memory management (no leaks)
- [ ] I can implement open addressing with correct deletion handling
- [ ] I understand the load factor's impact on performance
- [ ] I can trigger and implement rehashing correctly
- [ ] I can analyse collision patterns using probe counts
- [ ] I know when to choose chaining versus open addressing
- [ ] My code compiles without warnings using `-Wall -Wextra`
- [ ] Valgrind reports no memory leaks or errors

---

## 💼 Interview Preparation

**Question 1:** *What is the time complexity of hash table operations, and when might they degrade?*

Operations achieve O(1) average case. Worst case degrades to O(n) when all keys collide (poor hash function or adversarial input). Universal hashing provides probabilistic guarantees against such attacks.

**Question 2:** *How would you design a hash function for a custom object type?*

Combine hash values of constituent fields using techniques like XOR with rotation or polynomial accumulation. Ensure all fields contributing to equality participate in hashing (consistency with equals()).

**Question 3:** *Explain the difference between open addressing and chaining. When would you choose each?*

Chaining handles high load factors gracefully and simplifies deletion. Open addressing offers better cache locality and avoids pointer overhead. Choose chaining for unknown or high n/m ratios; open addressing when memory is constrained and load factor remains below 0.7.

**Question 4:** *What is the birthday problem, and how does it relate to hash collisions?*

The birthday paradox states that in a group of 23 people, there's a 50% probability two share a birthday. Similarly, with n insertions into m buckets, collisions become likely when n ≈ √(2m). This motivates proper table sizing.

**Question 5:** *How does Python's dict handle hash collisions internally?*

CPython uses open addressing with pseudo-random probing (based on the hash value). It maintains a sparse table alongside a dense array of entries for iteration order (since 3.7). Rehashing occurs at 2/3 load factor.

---

## 🔗 Next Week Preview

**Week 12: Graphs — Representation and Basic Algorithms**

Building upon our hash table foundations, we shall explore graph data structures—networks of vertices connected by edges. We shall implement both adjacency matrix and adjacency list representations, with the latter employing hash tables for efficient neighbour lookup in sparse graphs.

Key topics include:
- Graph terminology: vertices, edges, paths, cycles, connectivity
- Adjacency matrix versus adjacency list trade-offs
- Depth-First Search (DFS) and its applications
- Breadth-First Search (BFS) and shortest path in unweighted graphs
- Topological sorting of directed acyclic graphs

The transition from hash tables to graphs represents a natural progression: whilst hash tables excel at key-value associations, graphs model relationships between entities—social networks, road systems, dependency hierarchies and countless other real-world structures.

---

*Algorithms and Programming Techniques — Week 11 Laboratory Materials*
*Academy of Economic Studies, Bucharest — Computer Science Faculty*

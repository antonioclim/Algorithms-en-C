# Extended Challenges — Week 11: Hash Tables

## 🚀 Advanced Challenges (Optional)

These challenges are designed for students who want to deepen their understanding of hash tables and explore advanced concepts. Each correctly solved challenge earns **+10 bonus points**.

---

## ⭐ Challenge 1: LRU Cache (Difficulty: Medium)

### Description

Implement a Least Recently Used (LRU) cache that combines a hash table with a doubly-linked list. The cache has a fixed capacity and evicts the least recently used item when full.

### Requirements

1. **Data structure:**
   - Hash table for O(1) key lookup
   - Doubly-linked list for O(1) eviction order management
   - Each node appears in both structures

2. **Operations (all O(1) time):**
   ```c
   LRUCache *lru_create(int capacity);
   int lru_get(LRUCache *cache, const char *key);  // -1 if not found
   void lru_put(LRUCache *cache, const char *key, int value);
   void lru_destroy(LRUCache *cache);
   ```

3. **Behaviour:**
   - `get()` moves accessed item to front of list
   - `put()` adds new item to front; if capacity exceeded, remove tail
   - `put()` updates existing key and moves to front

### Example

```c
LRUCache *cache = lru_create(3);

lru_put(cache, "a", 1);  // Cache: [a]
lru_put(cache, "b", 2);  // Cache: [b, a]
lru_put(cache, "c", 3);  // Cache: [c, b, a]

lru_get(cache, "a");     // Returns 1, Cache: [a, c, b]

lru_put(cache, "d", 4);  // Evicts "b", Cache: [d, a, c]

lru_get(cache, "b");     // Returns -1 (evicted)

lru_destroy(cache);
```

### Bonus Points: +10

---

## ⭐ Challenge 2: Consistent Hashing (Difficulty: Hard)

### Description

Implement consistent hashing, a technique used in distributed systems to minimise key redistribution when servers are added or removed.

### Requirements

1. **Virtual nodes:**
   - Each physical server maps to multiple virtual nodes on a ring
   - Use at least 100 virtual nodes per server

2. **Ring structure:**
   - Hash ring with positions 0 to 2^32 - 1
   - Keys assigned to the nearest server clockwise

3. **Operations:**
   ```c
   ConsistentHash *ch_create(void);
   void ch_add_server(ConsistentHash *ch, const char *server_id);
   void ch_remove_server(ConsistentHash *ch, const char *server_id);
   const char *ch_get_server(ConsistentHash *ch, const char *key);
   void ch_destroy(ConsistentHash *ch);
   ```

4. **Analysis:**
   - Measure key distribution across servers
   - Show that adding/removing a server only affects ~1/n of keys

### Example

```c
ConsistentHash *ch = ch_create();

ch_add_server(ch, "server1");
ch_add_server(ch, "server2");
ch_add_server(ch, "server3");

// Distribute 1000 keys
for (int i = 0; i < 1000; i++) {
    char key[20];
    sprintf(key, "key_%d", i);
    const char *server = ch_get_server(ch, key);
    // Count assignments per server
}

// Remove server2 - only ~1/3 of keys should move
ch_remove_server(ch, "server2");
```

### Bonus Points: +10

---

## ⭐ Challenge 3: Bloom Filter (Difficulty: Medium)

### Description

Implement a Bloom filter — a space-efficient probabilistic data structure for membership testing with no false negatives but possible false positives.

### Requirements

1. **Parameters:**
   - Configurable number of bits (m)
   - Configurable number of hash functions (k)
   - Calculate optimal k given m and expected n

2. **Hash functions:**
   - Generate k hash functions using double hashing:
     - h_i(x) = (h1(x) + i × h2(x)) mod m

3. **Operations:**
   ```c
   BloomFilter *bf_create(int num_bits, int num_hashes);
   BloomFilter *bf_create_optimal(int expected_items, float false_positive_rate);
   void bf_add(BloomFilter *bf, const char *item);
   bool bf_might_contain(BloomFilter *bf, const char *item);
   float bf_false_positive_rate(BloomFilter *bf);
   void bf_destroy(BloomFilter *bf);
   ```

4. **Analysis:**
   - Test actual vs theoretical false positive rate
   - Compare memory usage with hash set for same data

### Example

```c
// Create filter for 1000 items with 1% false positive rate
BloomFilter *bf = bf_create_optimal(1000, 0.01);

// Add items
for (int i = 0; i < 1000; i++) {
    char item[20];
    sprintf(item, "item_%d", i);
    bf_add(bf, item);
}

// Test membership
bf_might_contain(bf, "item_500");  // true (definitely)
bf_might_contain(bf, "item_9999"); // false or true (might be FP)

// Measure actual FP rate
int fp_count = 0;
for (int i = 1000; i < 2000; i++) {
    char item[20];
    sprintf(item, "item_%d", i);
    if (bf_might_contain(bf, item)) fp_count++;
}
float actual_fp_rate = (float)fp_count / 1000;

bf_destroy(bf);
```

### Bonus Points: +10

---

## ⭐ Challenge 4: Cuckoo Hashing (Difficulty: Hard)

### Description

Implement cuckoo hashing, which guarantees O(1) worst-case lookup time using two hash functions and two tables.

### Requirements

1. **Structure:**
   - Two hash tables of equal size
   - Two independent hash functions

2. **Insert algorithm:**
   - Try to insert in table 1 using h1
   - If occupied, evict existing element and try to insert it in table 2
   - Continue "cuckoo" displacement until empty slot found
   - If cycle detected (max iterations), trigger rehash

3. **Operations:**
   ```c
   CuckooTable *cuckoo_create(int size);
   bool cuckoo_insert(CuckooTable *ct, const char *key, int value);
   int *cuckoo_search(CuckooTable *ct, const char *key);  // O(1) worst case
   bool cuckoo_delete(CuckooTable *ct, const char *key);
   void cuckoo_destroy(CuckooTable *ct);
   ```

4. **Rehashing:**
   - Detect cycles during insertion
   - Double table size and choose new hash functions
   - Reinsert all elements

### Example

```c
CuckooTable *ct = cuckoo_create(16);

cuckoo_insert(ct, "alice", 100);
cuckoo_insert(ct, "bob", 200);
cuckoo_insert(ct, "carol", 300);

// Lookup is always O(1) - check at most 2 positions
int *val = cuckoo_search(ct, "bob");  // *val = 200

cuckoo_destroy(ct);
```

### Bonus Points: +10

---

## ⭐ Challenge 5: Hash Table with Robin Hood Hashing (Difficulty: Medium)

### Description

Implement Robin Hood hashing, an open addressing variant that improves worst-case probe sequences by "stealing from the rich."

### Requirements

1. **Probe sequence distance (PSD):**
   - Track how far each element is from its ideal position
   - During insertion, swap with existing element if new element has higher PSD

2. **Backward shift deletion:**
   - When deleting, shift subsequent elements backward
   - No tombstones needed

3. **Operations:**
   ```c
   RobinHoodTable *rh_create(int size);
   int rh_insert(RobinHoodTable *rht, const char *key, int value);  // Returns probes
   int *rh_search(RobinHoodTable *rht, const char *key);
   bool rh_delete(RobinHoodTable *rht, const char *key);
   void rh_destroy(RobinHoodTable *rht);
   ```

4. **Statistics:**
   - Track maximum probe sequence distance
   - Compare variance with standard linear probing

### Example

```c
RobinHoodTable *rht = rh_create(16);

// Insert with Robin Hood: poor elements steal from rich
rh_insert(rht, "alice", 1);  // Ideal slot
rh_insert(rht, "bob", 2);    // Collision, probes to slot+1
rh_insert(rht, "carol", 3);  // May swap with bob if carol's PSD > bob's PSD

// Maximum probe distance is minimised
printf("Max PSD: %d\n", rht->max_psd);

rh_destroy(rht);
```

### Bonus Points: +10

---

## 📊 Bonus Point System

| Challenges Completed | Total Bonus |
|---------------------|-------------|
| 1 challenge | +10 points |
| 2 challenges | +20 points |
| 3 challenges | +30 points |
| 4 challenges | +40 points |
| All 5 challenges | +50 points + **"Hash Master"** badge 🏆 |

---

## 📤 Submission Guidelines

1. **File naming:** `challenge1_lru_cache.c`, `challenge2_consistent_hash.c`, etc.

2. **Documentation:** Include comments explaining your approach

3. **Testing:** Provide test cases demonstrating correctness

4. **Analysis:** Include a brief report (text file or comments) discussing:
   - Time complexity analysis
   - Space complexity analysis
   - Comparison with standard approaches

---

## 💡 Implementation Tips

### Challenge 1 (LRU Cache)
- Use dummy head and tail nodes to simplify edge cases
- The hash table stores pointers to list nodes

### Challenge 2 (Consistent Hashing)
- Store virtual nodes in a sorted array or balanced BST
- Use binary search to find the next server

### Challenge 3 (Bloom Filter)
- Optimal k ≈ (m/n) × ln(2)
- Use bit array with bitwise operations

### Challenge 4 (Cuckoo Hashing)
- Limit displacement iterations (e.g., 500)
- Use two completely different hash functions

### Challenge 5 (Robin Hood)
- Swap during forward probing, shift during backward deletion
- The variance of probe distances is minimised

---

## 📚 References

1. **LRU Cache:** LeetCode Problem #146
2. **Consistent Hashing:** Karger et al., 1997
3. **Bloom Filters:** Burton H. Bloom, 1970
4. **Cuckoo Hashing:** Pagh & Rodler, 2004
5. **Robin Hood Hashing:** Celis, 1986

---

*These challenges represent real-world techniques used in production systems.*
*Completing them demonstrates deep understanding of hash table variants.*

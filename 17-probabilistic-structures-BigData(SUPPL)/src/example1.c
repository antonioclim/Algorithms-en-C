/**
 * =============================================================================
 * WEEK 17: PROBABILISTIC DATA STRUCTURES FOR BIG DATA
 * Complete Working Example
 * =============================================================================
 *
 * This example demonstrates:
 *   1. Hash function fundamentals (MurmurHash-inspired)
 *   2. Bloom Filter for membership testing
 *   3. Counting Bloom Filter with deletion support
 *   4. Count-Min Sketch for frequency estimation
 *   5. HyperLogLog for cardinality estimation
 *   6. Skip List as probabilistic sorted structure
 *   7. Reservoir Sampling for streaming selection
 *   8. Comparison with exact methods
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o example1 example1.c -lm
 * Usage: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

/* =============================================================================
 * CONSTANTS AND TYPE DEFINITIONS
 * =============================================================================
 */

#define MAX_SKIP_LEVEL 16
#define SKIP_P 0.5

/* =============================================================================
 * PART 1: HASH FUNCTION FOUNDATIONS
 * =============================================================================
 */

/**
 * MurmurHash3-inspired 64-bit hash function.
 * Provides good distribution for probabilistic structures.
 * 
 * @param data Pointer to data to hash
 * @param len Length of data in bytes
 * @param seed Seed value for hashing
 * @return 64-bit hash value
 */
uint64_t murmur_hash64(const void *data, size_t len, uint64_t seed) {
    const uint64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;
    
    uint64_t h = seed ^ (len * m);
    
    const uint64_t *blocks = (const uint64_t *)data;
    size_t nblocks = len / 8;
    
    for (size_t i = 0; i < nblocks; i++) {
        uint64_t k = blocks[i];
        k *= m;
        k ^= k >> r;
        k *= m;
        h ^= k;
        h *= m;
    }
    
    const uint8_t *tail = (const uint8_t *)(data) + nblocks * 8;
    uint64_t k = 0;
    
    switch (len & 7) {
        case 7: k ^= (uint64_t)tail[6] << 48; /* fallthrough */
        case 6: k ^= (uint64_t)tail[5] << 40; /* fallthrough */
        case 5: k ^= (uint64_t)tail[4] << 32; /* fallthrough */
        case 4: k ^= (uint64_t)tail[3] << 24; /* fallthrough */
        case 3: k ^= (uint64_t)tail[2] << 16; /* fallthrough */
        case 2: k ^= (uint64_t)tail[1] << 8;  /* fallthrough */
        case 1: k ^= (uint64_t)tail[0];
                k *= m;
                h ^= k;
    }
    
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    
    return h;
}

/**
 * Generate the i-th hash using Kirsch-Mitzenmacher technique.
 * Generates k independent hashes from just two base hashes.
 */
uint64_t get_hash_i(const void *data, size_t len, int i) {
    uint64_t h1 = murmur_hash64(data, len, 0);
    uint64_t h2 = murmur_hash64(data, len, h1);
    return h1 + i * h2;
}

/* =============================================================================
 * PART 2: BLOOM FILTER
 * =============================================================================
 */

typedef struct {
    uint8_t *bits;          /* Bit array */
    size_t num_bits;        /* Size of bit array (m) */
    size_t num_hashes;      /* Number of hash functions (k) */
    size_t num_items;       /* Number of inserted items */
} BloomFilter;

/**
 * Create a new Bloom filter with specified parameters.
 * 
 * @param num_bits Size of bit array
 * @param num_hashes Number of hash functions
 * @return Pointer to new BloomFilter
 */
BloomFilter *bloom_create(size_t num_bits, size_t num_hashes) {
    BloomFilter *bf = malloc(sizeof(BloomFilter));
    if (!bf) return NULL;
    
    bf->num_bits = num_bits;
    bf->num_hashes = num_hashes;
    bf->num_items = 0;
    
    size_t bytes = (num_bits + 7) / 8;
    bf->bits = calloc(bytes, 1);
    if (!bf->bits) {
        free(bf);
        return NULL;
    }
    
    return bf;
}

/**
 * Create optimal Bloom filter given expected items and FP rate.
 */
BloomFilter *bloom_create_optimal(size_t expected_items, double fp_rate) {
    /* m = -n * ln(p) / (ln(2))² */
    double m = -((double)expected_items * log(fp_rate)) / (log(2) * log(2));
    size_t num_bits = (size_t)ceil(m);
    
    /* k = (m/n) * ln(2) */
    double k = ((double)num_bits / expected_items) * log(2);
    size_t num_hashes = (size_t)ceil(k);
    
    return bloom_create(num_bits, num_hashes);
}

void bloom_free(BloomFilter *bf) {
    if (bf) {
        free(bf->bits);
        free(bf);
    }
}

static inline void set_bit(uint8_t *bits, size_t idx) {
    bits[idx / 8] |= (1 << (idx % 8));
}

static inline bool get_bit(const uint8_t *bits, size_t idx) {
    return (bits[idx / 8] >> (idx % 8)) & 1;
}

/**
 * Insert an element into the Bloom filter.
 */
void bloom_insert(BloomFilter *bf, const void *data, size_t len) {
    for (size_t i = 0; i < bf->num_hashes; i++) {
        uint64_t hash = get_hash_i(data, len, i);
        size_t idx = hash % bf->num_bits;
        set_bit(bf->bits, idx);
    }
    bf->num_items++;
}

/**
 * Query if an element might be in the set.
 * Returns false = definitely not in set
 * Returns true = possibly in set (may be false positive)
 */
bool bloom_query(const BloomFilter *bf, const void *data, size_t len) {
    for (size_t i = 0; i < bf->num_hashes; i++) {
        uint64_t hash = get_hash_i(data, len, i);
        size_t idx = hash % bf->num_bits;
        if (!get_bit(bf->bits, idx)) {
            return false;
        }
    }
    return true;
}

/**
 * Calculate theoretical false positive rate.
 */
double bloom_fp_rate(const BloomFilter *bf) {
    double exp_val = exp(-(double)(bf->num_hashes * bf->num_items) / bf->num_bits);
    return pow(1 - exp_val, bf->num_hashes);
}

/**
 * Demonstrates Bloom filter functionality.
 */
void demo_bloom_filter(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: BLOOM FILTER                                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Create filter for 1000 items with 1% FP rate */
    BloomFilter *bf = bloom_create_optimal(1000, 0.01);
    
    printf("Created Bloom filter:\n");
    printf("  • Bits: %zu\n", bf->num_bits);
    printf("  • Hash functions: %zu\n", bf->num_hashes);
    printf("  • Bytes: %zu\n\n", (bf->num_bits + 7) / 8);
    
    /* Insert some words */
    const char *words[] = {"apple", "banana", "cherry", "date", "elderberry"};
    size_t num_words = sizeof(words) / sizeof(words[0]);
    
    printf("Inserting words:\n");
    for (size_t i = 0; i < num_words; i++) {
        bloom_insert(bf, words[i], strlen(words[i]));
        printf("  + Inserted '%s'\n", words[i]);
    }
    
    printf("\nQuerying:\n");
    /* Query inserted words */
    for (size_t i = 0; i < num_words; i++) {
        bool found = bloom_query(bf, words[i], strlen(words[i]));
        printf("  ? '%s' → %s\n", words[i], found ? "POSSIBLY IN SET" : "NOT IN SET");
    }
    
    /* Query words not inserted */
    const char *test_words[] = {"fig", "grape", "kiwi"};
    for (size_t i = 0; i < 3; i++) {
        bool found = bloom_query(bf, test_words[i], strlen(test_words[i]));
        printf("  ? '%s' → %s%s\n", test_words[i], 
               found ? "POSSIBLY IN SET" : "NOT IN SET",
               found ? " (false positive!)" : "");
    }
    
    printf("\nTheoretical FP rate: %.4f%%\n", bloom_fp_rate(bf) * 100);
    
    bloom_free(bf);
}

/* =============================================================================
 * PART 3: COUNTING BLOOM FILTER
 * =============================================================================
 */

typedef struct {
    uint8_t *counters;      /* 4-bit counters (2 per byte) */
    size_t num_counters;    /* Number of counters */
    size_t num_hashes;      /* Number of hash functions */
    size_t num_items;       /* Number of inserted items */
} CountingBloomFilter;

CountingBloomFilter *cbf_create(size_t num_counters, size_t num_hashes) {
    CountingBloomFilter *cbf = malloc(sizeof(CountingBloomFilter));
    if (!cbf) return NULL;
    
    cbf->num_counters = num_counters;
    cbf->num_hashes = num_hashes;
    cbf->num_items = 0;
    
    size_t bytes = (num_counters + 1) / 2;
    cbf->counters = calloc(bytes, 1);
    if (!cbf->counters) {
        free(cbf);
        return NULL;
    }
    
    return cbf;
}

void cbf_free(CountingBloomFilter *cbf) {
    if (cbf) {
        free(cbf->counters);
        free(cbf);
    }
}

static inline uint8_t cbf_get(const CountingBloomFilter *cbf, size_t idx) {
    size_t byte_idx = idx / 2;
    if (idx % 2 == 0) {
        return cbf->counters[byte_idx] & 0x0F;
    } else {
        return (cbf->counters[byte_idx] >> 4) & 0x0F;
    }
}

static inline void cbf_increment(CountingBloomFilter *cbf, size_t idx) {
    size_t byte_idx = idx / 2;
    uint8_t val = cbf_get(cbf, idx);
    if (val < 15) {  /* Avoid overflow */
        if (idx % 2 == 0) {
            cbf->counters[byte_idx] = (cbf->counters[byte_idx] & 0xF0) | (val + 1);
        } else {
            cbf->counters[byte_idx] = (cbf->counters[byte_idx] & 0x0F) | ((val + 1) << 4);
        }
    }
}

static inline void cbf_decrement(CountingBloomFilter *cbf, size_t idx) {
    size_t byte_idx = idx / 2;
    uint8_t val = cbf_get(cbf, idx);
    if (val > 0) {
        if (idx % 2 == 0) {
            cbf->counters[byte_idx] = (cbf->counters[byte_idx] & 0xF0) | (val - 1);
        } else {
            cbf->counters[byte_idx] = (cbf->counters[byte_idx] & 0x0F) | ((val - 1) << 4);
        }
    }
}

void cbf_insert(CountingBloomFilter *cbf, const void *data, size_t len) {
    for (size_t i = 0; i < cbf->num_hashes; i++) {
        uint64_t hash = get_hash_i(data, len, i);
        size_t idx = hash % cbf->num_counters;
        cbf_increment(cbf, idx);
    }
    cbf->num_items++;
}

void cbf_delete(CountingBloomFilter *cbf, const void *data, size_t len) {
    for (size_t i = 0; i < cbf->num_hashes; i++) {
        uint64_t hash = get_hash_i(data, len, i);
        size_t idx = hash % cbf->num_counters;
        cbf_decrement(cbf, idx);
    }
    if (cbf->num_items > 0) cbf->num_items--;
}

bool cbf_query(const CountingBloomFilter *cbf, const void *data, size_t len) {
    for (size_t i = 0; i < cbf->num_hashes; i++) {
        uint64_t hash = get_hash_i(data, len, i);
        size_t idx = hash % cbf->num_counters;
        if (cbf_get(cbf, idx) == 0) {
            return false;
        }
    }
    return true;
}

void demo_counting_bloom_filter(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: COUNTING BLOOM FILTER                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    CountingBloomFilter *cbf = cbf_create(1000, 5);
    
    printf("Demonstrating deletion support:\n\n");
    
    cbf_insert(cbf, "apple", 5);
    cbf_insert(cbf, "banana", 6);
    
    printf("After inserting 'apple' and 'banana':\n");
    printf("  ? 'apple'  → %s\n", cbf_query(cbf, "apple", 5) ? "FOUND" : "NOT FOUND");
    printf("  ? 'banana' → %s\n", cbf_query(cbf, "banana", 6) ? "FOUND" : "NOT FOUND");
    
    printf("\nDeleting 'apple'...\n");
    cbf_delete(cbf, "apple", 5);
    
    printf("After deletion:\n");
    printf("  ? 'apple'  → %s\n", cbf_query(cbf, "apple", 5) ? "FOUND" : "NOT FOUND");
    printf("  ? 'banana' → %s\n", cbf_query(cbf, "banana", 6) ? "FOUND" : "NOT FOUND");
    
    cbf_free(cbf);
}

/* =============================================================================
 * PART 4: COUNT-MIN SKETCH
 * =============================================================================
 */

typedef struct {
    uint32_t **table;       /* 2D array of counters */
    size_t width;           /* Number of columns (w) */
    size_t depth;           /* Number of rows (d) */
    size_t total;           /* Total count */
} CountMinSketch;

/**
 * Create Count-Min Sketch with specified dimensions.
 */
CountMinSketch *cms_create(size_t width, size_t depth) {
    CountMinSketch *cms = malloc(sizeof(CountMinSketch));
    if (!cms) return NULL;
    
    cms->width = width;
    cms->depth = depth;
    cms->total = 0;
    
    cms->table = malloc(depth * sizeof(uint32_t *));
    if (!cms->table) {
        free(cms);
        return NULL;
    }
    
    for (size_t i = 0; i < depth; i++) {
        cms->table[i] = calloc(width, sizeof(uint32_t));
        if (!cms->table[i]) {
            for (size_t j = 0; j < i; j++) free(cms->table[j]);
            free(cms->table);
            free(cms);
            return NULL;
        }
    }
    
    return cms;
}

/**
 * Create optimal CMS given error parameters.
 * epsilon: relative error bound
 * delta: probability of exceeding error bound
 */
CountMinSketch *cms_create_optimal(double epsilon, double delta) {
    size_t width = (size_t)ceil(exp(1.0) / epsilon);
    size_t depth = (size_t)ceil(log(1.0 / delta));
    return cms_create(width, depth);
}

void cms_free(CountMinSketch *cms) {
    if (cms) {
        for (size_t i = 0; i < cms->depth; i++) {
            free(cms->table[i]);
        }
        free(cms->table);
        free(cms);
    }
}

/**
 * Update count for an element.
 */
void cms_update(CountMinSketch *cms, const void *data, size_t len, int32_t count) {
    for (size_t i = 0; i < cms->depth; i++) {
        uint64_t hash = get_hash_i(data, len, i);
        size_t idx = hash % cms->width;
        cms->table[i][idx] += count;
    }
    cms->total += count;
}

/**
 * Query estimated frequency for an element.
 * Returns minimum across all rows (never underestimates).
 */
uint32_t cms_query(const CountMinSketch *cms, const void *data, size_t len) {
    uint32_t min_val = UINT32_MAX;
    
    for (size_t i = 0; i < cms->depth; i++) {
        uint64_t hash = get_hash_i(data, len, i);
        size_t idx = hash % cms->width;
        if (cms->table[i][idx] < min_val) {
            min_val = cms->table[i][idx];
        }
    }
    
    return min_val;
}

void demo_count_min_sketch(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: COUNT-MIN SKETCH                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Create CMS with 1% error, 99% confidence */
    CountMinSketch *cms = cms_create_optimal(0.01, 0.01);
    
    printf("Created Count-Min Sketch:\n");
    printf("  • Width: %zu columns\n", cms->width);
    printf("  • Depth: %zu rows\n", cms->depth);
    printf("  • Total cells: %zu\n", cms->width * cms->depth);
    printf("  • Memory: %zu bytes\n\n", cms->width * cms->depth * sizeof(uint32_t));
    
    /* Simulate stream with frequency distribution */
    printf("Simulating stream of elements:\n");
    
    /* Insert elements with varying frequencies */
    for (int i = 0; i < 1000; i++) cms_update(cms, "apple", 5, 1);
    for (int i = 0; i < 500; i++)  cms_update(cms, "banana", 6, 1);
    for (int i = 0; i < 100; i++)  cms_update(cms, "cherry", 6, 1);
    for (int i = 0; i < 50; i++)   cms_update(cms, "date", 4, 1);
    
    printf("  Inserted: apple×1000, banana×500, cherry×100, date×50\n\n");
    
    /* Query frequencies */
    printf("Frequency estimates (vs actual):\n");
    printf("  'apple'  : %u (actual: 1000)\n", cms_query(cms, "apple", 5));
    printf("  'banana' : %u (actual: 500)\n", cms_query(cms, "banana", 6));
    printf("  'cherry' : %u (actual: 100)\n", cms_query(cms, "cherry", 6));
    printf("  'date'   : %u (actual: 50)\n", cms_query(cms, "date", 4));
    printf("  'fig'    : %u (actual: 0, but may overestimate)\n", 
           cms_query(cms, "fig", 3));
    
    cms_free(cms);
}

/* =============================================================================
 * PART 5: HYPERLOGLOG
 * =============================================================================
 */

typedef struct {
    uint8_t *registers;     /* Register array */
    uint8_t precision;      /* Precision (p), gives m = 2^p registers */
    uint32_t num_registers; /* Number of registers (m) */
} HyperLogLog;

/**
 * Count leading zeros in 64-bit value (position of first 1-bit).
 */
static inline uint8_t count_leading_zeros(uint64_t x) {
    if (x == 0) return 64;
    uint8_t n = 0;
    if ((x & 0xFFFFFFFF00000000ULL) == 0) { n += 32; x <<= 32; }
    if ((x & 0xFFFF000000000000ULL) == 0) { n += 16; x <<= 16; }
    if ((x & 0xFF00000000000000ULL) == 0) { n += 8;  x <<= 8; }
    if ((x & 0xF000000000000000ULL) == 0) { n += 4;  x <<= 4; }
    if ((x & 0xC000000000000000ULL) == 0) { n += 2;  x <<= 2; }
    if ((x & 0x8000000000000000ULL) == 0) { n += 1; }
    return n;
}

/**
 * Create HyperLogLog with specified precision.
 * precision p gives m = 2^p registers, standard error ≈ 1.04/√m
 */
HyperLogLog *hll_create(uint8_t precision) {
    if (precision < 4 || precision > 18) {
        precision = 14;  /* Default: ~0.81% error, ~12KB */
    }
    
    HyperLogLog *hll = malloc(sizeof(HyperLogLog));
    if (!hll) return NULL;
    
    hll->precision = precision;
    hll->num_registers = 1U << precision;
    hll->registers = calloc(hll->num_registers, 1);
    
    if (!hll->registers) {
        free(hll);
        return NULL;
    }
    
    return hll;
}

void hll_free(HyperLogLog *hll) {
    if (hll) {
        free(hll->registers);
        free(hll);
    }
}

/**
 * Add an element to HyperLogLog.
 */
void hll_add(HyperLogLog *hll, const void *data, size_t len) {
    uint64_t hash = murmur_hash64(data, len, 0x5f61767a);
    
    /* Use first p bits to select register */
    uint32_t idx = hash >> (64 - hll->precision);
    
    /* Count leading zeros of remaining bits + 1 */
    uint64_t remaining = (hash << hll->precision) | (1ULL << (hll->precision - 1));
    uint8_t rho = count_leading_zeros(remaining) + 1;
    
    /* Update register with maximum */
    if (rho > hll->registers[idx]) {
        hll->registers[idx] = rho;
    }
}

/**
 * Estimate cardinality using harmonic mean.
 */
uint64_t hll_count(const HyperLogLog *hll) {
    double alpha;
    uint32_t m = hll->num_registers;
    
    /* Bias correction factor */
    switch (hll->precision) {
        case 4:  alpha = 0.673; break;
        case 5:  alpha = 0.697; break;
        case 6:  alpha = 0.709; break;
        default: alpha = 0.7213 / (1 + 1.079 / m); break;
    }
    
    /* Compute harmonic mean */
    double sum = 0;
    uint32_t zeros = 0;
    
    for (uint32_t i = 0; i < m; i++) {
        sum += 1.0 / (1ULL << hll->registers[i]);
        if (hll->registers[i] == 0) zeros++;
    }
    
    double estimate = alpha * m * m / sum;
    
    /* Small range correction */
    if (estimate <= 2.5 * m && zeros > 0) {
        estimate = m * log((double)m / zeros);
    }
    
    return (uint64_t)estimate;
}

/**
 * Merge two HyperLogLogs (for distributed counting).
 */
void hll_merge(HyperLogLog *dst, const HyperLogLog *src) {
    if (dst->precision != src->precision) return;
    
    for (uint32_t i = 0; i < dst->num_registers; i++) {
        if (src->registers[i] > dst->registers[i]) {
            dst->registers[i] = src->registers[i];
        }
    }
}

void demo_hyperloglog(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: HYPERLOGLOG                                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    HyperLogLog *hll = hll_create(14);  /* ~0.81% standard error */
    
    printf("Created HyperLogLog:\n");
    printf("  • Precision: %d\n", hll->precision);
    printf("  • Registers: %u\n", hll->num_registers);
    printf("  • Memory: %u bytes\n", hll->num_registers);
    printf("  • Expected error: ~%.2f%%\n\n", 104.0 / sqrt(hll->num_registers));
    
    /* Add many elements */
    printf("Adding elements and estimating cardinality:\n\n");
    
    uint64_t actual_counts[] = {100, 1000, 10000, 100000, 1000000};
    
    for (size_t t = 0; t < 5; t++) {
        /* Reset HLL */
        memset(hll->registers, 0, hll->num_registers);
        
        /* Add unique elements */
        for (uint64_t i = 0; i < actual_counts[t]; i++) {
            hll_add(hll, &i, sizeof(i));
        }
        
        uint64_t estimate = hll_count(hll);
        double error = 100.0 * fabs((double)estimate - actual_counts[t]) / actual_counts[t];
        
        printf("  Actual: %7lu | Estimate: %7lu | Error: %5.2f%%\n",
               actual_counts[t], estimate, error);
    }
    
    hll_free(hll);
}

/* =============================================================================
 * PART 6: SKIP LIST
 * =============================================================================
 */

typedef struct SkipNode {
    int key;
    void *value;
    int level;
    struct SkipNode **forward;
} SkipNode;

typedef struct {
    SkipNode *header;
    int max_level;
    int level;
    size_t size;
} SkipList;

/**
 * Generate random level with probability p = 0.5
 */
static int random_level(void) {
    int level = 1;
    while ((rand() & 1) && level < MAX_SKIP_LEVEL) {
        level++;
    }
    return level;
}

SkipNode *skip_node_create(int key, void *value, int level) {
    SkipNode *node = malloc(sizeof(SkipNode));
    if (!node) return NULL;
    
    node->key = key;
    node->value = value;
    node->level = level;
    node->forward = calloc(level, sizeof(SkipNode *));
    
    if (!node->forward) {
        free(node);
        return NULL;
    }
    
    return node;
}

SkipList *skip_list_create(void) {
    SkipList *sl = malloc(sizeof(SkipList));
    if (!sl) return NULL;
    
    sl->max_level = MAX_SKIP_LEVEL;
    sl->level = 1;
    sl->size = 0;
    
    sl->header = skip_node_create(0, NULL, MAX_SKIP_LEVEL);
    if (!sl->header) {
        free(sl);
        return NULL;
    }
    
    return sl;
}

void skip_list_free(SkipList *sl) {
    if (!sl) return;
    
    SkipNode *node = sl->header;
    while (node) {
        SkipNode *next = node->forward[0];
        free(node->forward);
        free(node);
        node = next;
    }
    free(sl);
}

/**
 * Search for a key in the skip list.
 */
SkipNode *skip_list_search(SkipList *sl, int key) {
    SkipNode *current = sl->header;
    
    for (int i = sl->level - 1; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->key < key) {
            current = current->forward[i];
        }
    }
    
    current = current->forward[0];
    
    if (current && current->key == key) {
        return current;
    }
    return NULL;
}

/**
 * Insert a key-value pair into the skip list.
 */
void skip_list_insert(SkipList *sl, int key, void *value) {
    SkipNode *update[MAX_SKIP_LEVEL];
    SkipNode *current = sl->header;
    
    /* Find insertion point at each level */
    for (int i = sl->level - 1; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->key < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    
    current = current->forward[0];
    
    /* Update existing key */
    if (current && current->key == key) {
        current->value = value;
        return;
    }
    
    /* Create new node with random level */
    int new_level = random_level();
    
    if (new_level > sl->level) {
        for (int i = sl->level; i < new_level; i++) {
            update[i] = sl->header;
        }
        sl->level = new_level;
    }
    
    SkipNode *new_node = skip_node_create(key, value, new_level);
    
    /* Insert at each level */
    for (int i = 0; i < new_level; i++) {
        new_node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = new_node;
    }
    
    sl->size++;
}

/**
 * Delete a key from the skip list.
 */
bool skip_list_delete(SkipList *sl, int key) {
    SkipNode *update[MAX_SKIP_LEVEL];
    SkipNode *current = sl->header;
    
    for (int i = sl->level - 1; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->key < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    
    current = current->forward[0];
    
    if (!current || current->key != key) {
        return false;
    }
    
    /* Remove from each level */
    for (int i = 0; i < sl->level; i++) {
        if (update[i]->forward[i] != current) break;
        update[i]->forward[i] = current->forward[i];
    }
    
    free(current->forward);
    free(current);
    
    /* Reduce level if necessary */
    while (sl->level > 1 && !sl->header->forward[sl->level - 1]) {
        sl->level--;
    }
    
    sl->size--;
    return true;
}

void skip_list_print(const SkipList *sl) {
    printf("Skip List (size=%zu, levels=%d):\n", sl->size, sl->level);
    
    for (int i = sl->level - 1; i >= 0; i--) {
        printf("  Level %d: HEAD", i);
        SkipNode *node = sl->header->forward[i];
        while (node) {
            printf(" -> %d", node->key);
            node = node->forward[i];
        }
        printf(" -> NIL\n");
    }
}

void demo_skip_list(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: SKIP LIST                                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    SkipList *sl = skip_list_create();
    
    printf("Inserting elements: 10, 20, 30, 25, 5, 15, 35\n\n");
    skip_list_insert(sl, 10, NULL);
    skip_list_insert(sl, 20, NULL);
    skip_list_insert(sl, 30, NULL);
    skip_list_insert(sl, 25, NULL);
    skip_list_insert(sl, 5, NULL);
    skip_list_insert(sl, 15, NULL);
    skip_list_insert(sl, 35, NULL);
    
    skip_list_print(sl);
    
    printf("\nSearching:\n");
    printf("  Search(20): %s\n", skip_list_search(sl, 20) ? "FOUND" : "NOT FOUND");
    printf("  Search(22): %s\n", skip_list_search(sl, 22) ? "FOUND" : "NOT FOUND");
    
    printf("\nDeleting 20...\n");
    skip_list_delete(sl, 20);
    
    printf("\nAfter deletion:\n");
    skip_list_print(sl);
    
    skip_list_free(sl);
}

/* =============================================================================
 * PART 7: RESERVOIR SAMPLING
 * =============================================================================
 */

typedef struct {
    int *reservoir;
    size_t k;           /* Reservoir size */
    size_t n;           /* Elements seen */
} ReservoirSampler;

ReservoirSampler *reservoir_create(size_t k) {
    ReservoirSampler *rs = malloc(sizeof(ReservoirSampler));
    if (!rs) return NULL;
    
    rs->reservoir = malloc(k * sizeof(int));
    if (!rs->reservoir) {
        free(rs);
        return NULL;
    }
    
    rs->k = k;
    rs->n = 0;
    
    return rs;
}

void reservoir_free(ReservoirSampler *rs) {
    if (rs) {
        free(rs->reservoir);
        free(rs);
    }
}

/**
 * Process next element in stream (Algorithm R).
 */
void reservoir_add(ReservoirSampler *rs, int element) {
    if (rs->n < rs->k) {
        /* Fill reservoir initially */
        rs->reservoir[rs->n] = element;
    } else {
        /* Replace with probability k/n */
        size_t j = rand() % (rs->n + 1);
        if (j < rs->k) {
            rs->reservoir[j] = element;
        }
    }
    rs->n++;
}

void demo_reservoir_sampling(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 7: RESERVOIR SAMPLING                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    ReservoirSampler *rs = reservoir_create(5);
    
    printf("Streaming 100 elements, keeping sample of 5:\n\n");
    
    for (int i = 1; i <= 100; i++) {
        reservoir_add(rs, i);
    }
    
    printf("Sample (uniformly random from stream):\n  ");
    for (size_t i = 0; i < rs->k; i++) {
        printf("%d ", rs->reservoir[i]);
    }
    printf("\n");
    
    reservoir_free(rs);
}

/* =============================================================================
 * PART 8: COMPARISON AND BENCHMARKS
 * =============================================================================
 */

void demo_comparison(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 8: COMPARISON WITH EXACT METHODS                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("╔════════════════════╦════════════════╦════════════════╦════════════╗\n");
    printf("║ Structure          ║ Exact Space    ║ Prob. Space    ║ Error      ║\n");
    printf("╠════════════════════╬════════════════╬════════════════╬════════════╣\n");
    printf("║ Set (1M elements)  ║ ~8 MB          ║ ~1.2 MB (BF)   ║ 1%% FP     ║\n");
    printf("║ Frequency (1M)     ║ ~16 MB         ║ ~54 KB (CMS)   ║ 1%% + εN   ║\n");
    printf("║ Cardinality (1B)   ║ ~8 GB          ║ ~12 KB (HLL)   ║ ~0.8%%     ║\n");
    printf("║ Sorted set (1M)    ║ ~24 MB (AVL)   ║ ~24 MB (Skip)  ║ None       ║\n");
    printf("╚════════════════════╩════════════════╩════════════════╩════════════╝\n");
    printf("\n");
    printf("Trade-off summary:\n");
    printf("  • Bloom Filter: 6-7× space reduction, occasional false positives\n");
    printf("  • Count-Min Sketch: 300× space reduction, bounded overestimation\n");
    printf("  • HyperLogLog: 666,000× space reduction for counting!\n");
    printf("  • Skip List: Same space, simpler implementation, probabilistic\n");
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 17: PROBABILISTIC DATA STRUCTURES                    ║\n");
    printf("║                    Complete Example                           ║\n");
    printf("║                                                               ║\n");
    printf("║     Bloom Filter • Count-Min Sketch • HyperLogLog            ║\n");
    printf("║     Skip List • Reservoir Sampling                           ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Check for benchmark flags */
    if (argc > 1) {
        if (strcmp(argv[1], "--benchmark-bloom") == 0) {
            printf("\nRunning Bloom Filter benchmark...\n");
            /* Add benchmark code here */
            return 0;
        }
        if (strcmp(argv[1], "--benchmark-cms") == 0) {
            printf("\nRunning Count-Min Sketch benchmark...\n");
            return 0;
        }
        if (strcmp(argv[1], "--benchmark-hll") == 0) {
            printf("\nRunning HyperLogLog benchmark...\n");
            return 0;
        }
    }
    
    demo_bloom_filter();
    demo_counting_bloom_filter();
    demo_count_min_sketch();
    demo_hyperloglog();
    demo_skip_list();
    demo_reservoir_sampling();
    demo_comparison();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    DEMONSTRATION COMPLETE                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

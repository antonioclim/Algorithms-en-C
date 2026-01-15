/**
 * =============================================================================
 * WEEK 17: PROBABILISTIC DATA STRUCTURES FOR BIG DATA
 * Homework 2 SOLUTION: Database Query Optimiser Simulator
 * =============================================================================
 *
 * This solution implements a database query optimiser using:
 *   - Bloom filters for table membership testing
 *   - Skip Lists for sorted index operations
 *   - Demonstrates join optimisation using probabilistic structures
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c -lm
 * Usage: ./homework2_sol
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

#define MAX_KEY_LENGTH 64
#define MAX_SKIP_LEVEL 16
#define SKIP_P 0.5

/* =============================================================================
 * PART 1: HASH FUNCTIONS
 * =============================================================================
 */

uint64_t murmur_hash64(const void *data, size_t len, uint64_t seed) {
    const uint64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;
    uint64_t h = seed ^ (len * m);
    const uint64_t *blocks = (const uint64_t *)data;
    size_t nblocks = len / 8;
    
    for (size_t i = 0; i < nblocks; i++) {
        uint64_t k = blocks[i];
        k *= m; k ^= k >> r; k *= m;
        h ^= k; h *= m;
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
        case 1: k ^= (uint64_t)tail[0]; k *= m; h ^= k;
    }
    h ^= h >> r; h *= m; h ^= h >> r;
    return h;
}

uint64_t get_hash_i(const void *data, size_t len, int i) {
    uint64_t h1 = murmur_hash64(data, len, 0);
    uint64_t h2 = murmur_hash64(data, len, h1);
    return h1 + i * h2;
}

/* =============================================================================
 * PART 2: BLOOM FILTER IMPLEMENTATION
 * =============================================================================
 */

typedef struct {
    uint8_t *bits;
    size_t num_bits;
    size_t num_hashes;
    size_t num_items;
} BloomFilter;

BloomFilter *bloom_create(size_t num_bits, size_t num_hashes) {
    BloomFilter *bf = malloc(sizeof(BloomFilter));
    if (!bf) return NULL;
    
    bf->num_bits = num_bits;
    bf->num_hashes = num_hashes;
    bf->num_items = 0;
    bf->bits = calloc((num_bits + 7) / 8, 1);
    if (!bf->bits) { free(bf); return NULL; }
    
    return bf;
}

BloomFilter *bloom_create_optimal(size_t n, double fp_rate) {
    double m = -((double)n * log(fp_rate)) / (log(2) * log(2));
    size_t num_bits = (size_t)ceil(m);
    double k = ((double)num_bits / n) * log(2);
    size_t num_hashes = (size_t)ceil(k);
    if (num_hashes < 1) num_hashes = 1;
    return bloom_create(num_bits, num_hashes);
}

void bloom_free(BloomFilter *bf) {
    if (bf) { free(bf->bits); free(bf); }
}

static inline void set_bit(uint8_t *bits, size_t idx) {
    bits[idx / 8] |= (1 << (idx % 8));
}

static inline bool get_bit(const uint8_t *bits, size_t idx) {
    return (bits[idx / 8] >> (idx % 8)) & 1;
}

void bloom_insert(BloomFilter *bf, const void *data, size_t len) {
    for (size_t i = 0; i < bf->num_hashes; i++) {
        uint64_t hash = get_hash_i(data, len, i);
        set_bit(bf->bits, hash % bf->num_bits);
    }
    bf->num_items++;
}

bool bloom_query(const BloomFilter *bf, const void *data, size_t len) {
    for (size_t i = 0; i < bf->num_hashes; i++) {
        uint64_t hash = get_hash_i(data, len, i);
        if (!get_bit(bf->bits, hash % bf->num_bits)) return false;
    }
    return true;
}

size_t bloom_memory_bytes(const BloomFilter *bf) {
    return (bf->num_bits + 7) / 8 + sizeof(BloomFilter);
}

/* =============================================================================
 * PART 3: SKIP LIST IMPLEMENTATION
 * =============================================================================
 */

typedef struct SkipNode {
    int key;
    char value[MAX_KEY_LENGTH];
    int level;
    struct SkipNode **forward;
} SkipNode;

typedef struct {
    SkipNode *header;
    int level;
    size_t num_items;
} SkipList;

/**
 * Generate random level with geometric distribution
 */
int skip_random_level(void) {
    int level = 1;
    while (((double)rand() / RAND_MAX) < SKIP_P && level < MAX_SKIP_LEVEL) {
        level++;
    }
    return level;
}

/**
 * Create new skip list node
 */
SkipNode *skip_node_create(int key, const char *value, int level) {
    SkipNode *node = malloc(sizeof(SkipNode));
    if (!node) return NULL;
    
    node->forward = calloc(level, sizeof(SkipNode *));
    if (!node->forward) { free(node); return NULL; }
    
    node->key = key;
    strncpy(node->value, value ? value : "", MAX_KEY_LENGTH - 1);
    node->value[MAX_KEY_LENGTH - 1] = '\0';
    node->level = level;
    
    return node;
}

/**
 * Create empty skip list
 */
SkipList *skip_create(void) {
    SkipList *sl = malloc(sizeof(SkipList));
    if (!sl) return NULL;
    
    sl->header = skip_node_create(0, NULL, MAX_SKIP_LEVEL);
    if (!sl->header) { free(sl); return NULL; }
    
    sl->level = 1;
    sl->num_items = 0;
    
    return sl;
}

/**
 * Free skip list
 */
void skip_free(SkipList *sl) {
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
 * Search skip list for key
 */
SkipNode *skip_search(const SkipList *sl, int key) {
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
 * Insert key-value pair into skip list
 */
bool skip_insert(SkipList *sl, int key, const char *value) {
    SkipNode *update[MAX_SKIP_LEVEL];
    SkipNode *current = sl->header;
    
    /* Find position */
    for (int i = sl->level - 1; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->key < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    
    current = current->forward[0];
    
    /* Update if key exists */
    if (current && current->key == key) {
        strncpy(current->value, value, MAX_KEY_LENGTH - 1);
        return true;
    }
    
    /* Insert new node */
    int level = skip_random_level();
    if (level > sl->level) {
        for (int i = sl->level; i < level; i++) {
            update[i] = sl->header;
        }
        sl->level = level;
    }
    
    SkipNode *node = skip_node_create(key, value, level);
    if (!node) return false;
    
    for (int i = 0; i < level; i++) {
        node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = node;
    }
    
    sl->num_items++;
    return true;
}

/**
 * Range query: find all keys in [low, high]
 */
size_t skip_range_query(const SkipList *sl, int low, int high,
                        int *results, size_t max_results) {
    SkipNode *current = sl->header;
    
    /* Find starting position */
    for (int i = sl->level - 1; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->key < low) {
            current = current->forward[i];
        }
    }
    
    current = current->forward[0];
    
    /* Collect results */
    size_t count = 0;
    while (current && current->key <= high && count < max_results) {
        results[count++] = current->key;
        current = current->forward[0];
    }
    
    return count;
}

size_t skip_memory_bytes(const SkipList *sl) {
    size_t mem = sizeof(SkipList);
    SkipNode *node = sl->header;
    while (node) {
        mem += sizeof(SkipNode) + node->level * sizeof(SkipNode *);
        node = node->forward[0];
    }
    return mem;
}

/* =============================================================================
 * PART 4: DATABASE TABLE SIMULATION
 * =============================================================================
 */

typedef struct {
    int id;
    char name[32];
    int foreign_key;
    double value;
} TableRow;

typedef struct {
    char name[32];
    TableRow *rows;
    size_t num_rows;
    BloomFilter *pk_bloom;      /* Bloom filter on primary key */
    BloomFilter *fk_bloom;      /* Bloom filter on foreign key */
    SkipList *pk_index;         /* Skip list index on primary key */
} DatabaseTable;

/**
 * Create database table with synthetic data
 */
DatabaseTable *table_create(const char *name, size_t num_rows, 
                            int pk_start, int fk_range) {
    DatabaseTable *table = malloc(sizeof(DatabaseTable));
    if (!table) return NULL;
    
    strncpy(table->name, name, sizeof(table->name) - 1);
    table->num_rows = num_rows;
    
    table->rows = malloc(num_rows * sizeof(TableRow));
    if (!table->rows) { free(table); return NULL; }
    
    /* Create Bloom filters */
    table->pk_bloom = bloom_create_optimal(num_rows, 0.01);
    table->fk_bloom = bloom_create_optimal(num_rows, 0.01);
    
    /* Create skip list index */
    table->pk_index = skip_create();
    
    /* Generate synthetic data */
    for (size_t i = 0; i < num_rows; i++) {
        table->rows[i].id = pk_start + (int)i;
        snprintf(table->rows[i].name, sizeof(table->rows[i].name),
                 "record_%zu", i);
        table->rows[i].foreign_key = rand() % fk_range;
        table->rows[i].value = (double)rand() / RAND_MAX * 1000;
        
        /* Add to Bloom filters */
        bloom_insert(table->pk_bloom, &table->rows[i].id, sizeof(int));
        bloom_insert(table->fk_bloom, &table->rows[i].foreign_key, sizeof(int));
        
        /* Add to skip list index */
        skip_insert(table->pk_index, table->rows[i].id, table->rows[i].name);
    }
    
    return table;
}

void table_free(DatabaseTable *table) {
    if (!table) return;
    free(table->rows);
    bloom_free(table->pk_bloom);
    bloom_free(table->fk_bloom);
    skip_free(table->pk_index);
    free(table);
}

/* =============================================================================
 * PART 5: QUERY OPERATIONS
 * =============================================================================
 */

typedef struct {
    size_t bloom_lookups;
    size_t bloom_positives;
    size_t bloom_false_positives;
    size_t index_lookups;
    size_t full_scans;
    size_t rows_examined;
    size_t rows_matched;
    double time_ms;
} QueryStats;

/**
 * Point query using Bloom filter pre-check
 */
TableRow *query_point_bloom(DatabaseTable *table, int key, QueryStats *stats) {
    stats->bloom_lookups++;
    
    /* Check Bloom filter first */
    if (!bloom_query(table->pk_bloom, &key, sizeof(int))) {
        return NULL;  /* Definitely not present */
    }
    
    stats->bloom_positives++;
    
    /* Bloom says maybe present - do index lookup */
    stats->index_lookups++;
    SkipNode *node = skip_search(table->pk_index, key);
    
    if (node) {
        /* Find actual row */
        for (size_t i = 0; i < table->num_rows; i++) {
            stats->rows_examined++;
            if (table->rows[i].id == key) {
                stats->rows_matched++;
                return &table->rows[i];
            }
        }
    }
    
    stats->bloom_false_positives++;
    return NULL;
}

/**
 * Point query with full scan (no optimisation)
 */
TableRow *query_point_scan(DatabaseTable *table, int key, QueryStats *stats) {
    stats->full_scans++;
    
    for (size_t i = 0; i < table->num_rows; i++) {
        stats->rows_examined++;
        if (table->rows[i].id == key) {
            stats->rows_matched++;
            return &table->rows[i];
        }
    }
    
    return NULL;
}

/**
 * Range query using skip list
 */
size_t query_range_index(DatabaseTable *table, int low, int high,
                         int *results, size_t max_results, QueryStats *stats) {
    stats->index_lookups++;
    return skip_range_query(table->pk_index, low, high, results, max_results);
}

/**
 * Nested loop join with Bloom filter optimisation
 */
size_t join_bloom_optimised(DatabaseTable *outer, DatabaseTable *inner,
                            QueryStats *stats) {
    clock_t start = clock();
    size_t matches = 0;
    
    for (size_t i = 0; i < outer->num_rows; i++) {
        int fk = outer->rows[i].foreign_key;
        stats->bloom_lookups++;
        
        /* Check Bloom filter on inner table's PK */
        if (!bloom_query(inner->pk_bloom, &fk, sizeof(int))) {
            continue;  /* Definitely no match */
        }
        
        stats->bloom_positives++;
        
        /* Bloom says maybe - do actual lookup */
        for (size_t j = 0; j < inner->num_rows; j++) {
            stats->rows_examined++;
            if (inner->rows[j].id == fk) {
                matches++;
                stats->rows_matched++;
                break;
            }
        }
    }
    
    stats->time_ms = (double)(clock() - start) * 1000 / CLOCKS_PER_SEC;
    return matches;
}

/**
 * Nested loop join without optimisation
 */
size_t join_naive(DatabaseTable *outer, DatabaseTable *inner,
                  QueryStats *stats) {
    clock_t start = clock();
    size_t matches = 0;
    
    for (size_t i = 0; i < outer->num_rows; i++) {
        int fk = outer->rows[i].foreign_key;
        stats->full_scans++;
        
        for (size_t j = 0; j < inner->num_rows; j++) {
            stats->rows_examined++;
            if (inner->rows[j].id == fk) {
                matches++;
                stats->rows_matched++;
                break;
            }
        }
    }
    
    stats->time_ms = (double)(clock() - start) * 1000 / CLOCKS_PER_SEC;
    return matches;
}

/* =============================================================================
 * PART 6: BENCHMARK AND ANALYSIS
 * =============================================================================
 */

void print_query_stats(const char *name, const QueryStats *stats) {
    printf("\n  %s:\n", name);
    printf("    Bloom lookups:       %zu\n", stats->bloom_lookups);
    printf("    Bloom positives:     %zu\n", stats->bloom_positives);
    printf("    False positives:     %zu\n", stats->bloom_false_positives);
    printf("    Index lookups:       %zu\n", stats->index_lookups);
    printf("    Full scans:          %zu\n", stats->full_scans);
    printf("    Rows examined:       %zu\n", stats->rows_examined);
    printf("    Rows matched:        %zu\n", stats->rows_matched);
    printf("    Time:                %.3f ms\n", stats->time_ms);
}

void benchmark_point_queries(DatabaseTable *table, int num_queries) {
    printf("\n");
    printf("┌────────────────────────────────────────────────────────────────┐\n");
    printf("│                 POINT QUERY BENCHMARK                          │\n");
    printf("├────────────────────────────────────────────────────────────────┤\n");
    printf("│  Table: %-20s  Rows: %-10zu               │\n", 
           table->name, table->num_rows);
    printf("│  Queries: %-10d                                          │\n",
           num_queries);
    printf("└────────────────────────────────────────────────────────────────┘\n");
    
    QueryStats stats_bloom = {0};
    QueryStats stats_scan = {0};
    
    clock_t start_bloom = clock();
    for (int i = 0; i < num_queries; i++) {
        int key = rand() % ((int)table->num_rows * 2);  /* 50% miss rate */
        query_point_bloom(table, key, &stats_bloom);
    }
    stats_bloom.time_ms = (double)(clock() - start_bloom) * 1000 / CLOCKS_PER_SEC;
    
    clock_t start_scan = clock();
    for (int i = 0; i < num_queries; i++) {
        int key = rand() % ((int)table->num_rows * 2);
        query_point_scan(table, key, &stats_scan);
    }
    stats_scan.time_ms = (double)(clock() - start_scan) * 1000 / CLOCKS_PER_SEC;
    
    print_query_stats("Bloom-Optimised", &stats_bloom);
    print_query_stats("Full Scan", &stats_scan);
    
    printf("\n  Speedup: %.2fx fewer rows examined\n",
           stats_scan.rows_examined > 0 ? 
           (double)stats_scan.rows_examined / stats_bloom.rows_examined : 0);
}

void benchmark_range_queries(DatabaseTable *table, int num_queries, int range_size) {
    printf("\n");
    printf("┌────────────────────────────────────────────────────────────────┐\n");
    printf("│                 RANGE QUERY BENCHMARK                          │\n");
    printf("├────────────────────────────────────────────────────────────────┤\n");
    printf("│  Table: %-20s  Range size: %-10d           │\n", 
           table->name, range_size);
    printf("└────────────────────────────────────────────────────────────────┘\n");
    
    QueryStats stats = {0};
    int results[1000];
    size_t total_results = 0;
    
    clock_t start = clock();
    for (int i = 0; i < num_queries; i++) {
        int low = rand() % (int)table->num_rows;
        int high = low + range_size;
        size_t count = query_range_index(table, low, high, results, 1000, &stats);
        total_results += count;
    }
    double time_ms = (double)(clock() - start) * 1000 / CLOCKS_PER_SEC;
    
    printf("\n  Skip List Index Results:\n");
    printf("    Queries executed:    %d\n", num_queries);
    printf("    Total results:       %zu\n", total_results);
    printf("    Avg results/query:   %.2f\n", (double)total_results / num_queries);
    printf("    Time:                %.3f ms\n", time_ms);
}

void benchmark_joins(DatabaseTable *orders, DatabaseTable *customers) {
    printf("\n");
    printf("┌────────────────────────────────────────────────────────────────┐\n");
    printf("│                 JOIN BENCHMARK                                 │\n");
    printf("├────────────────────────────────────────────────────────────────┤\n");
    printf("│  Outer: %-15s  (%zu rows)                           │\n", 
           orders->name, orders->num_rows);
    printf("│  Inner: %-15s  (%zu rows)                           │\n", 
           customers->name, customers->num_rows);
    printf("└────────────────────────────────────────────────────────────────┘\n");
    
    QueryStats stats_bloom = {0};
    QueryStats stats_naive = {0};
    
    size_t matches_bloom = join_bloom_optimised(orders, customers, &stats_bloom);
    size_t matches_naive = join_naive(orders, customers, &stats_naive);
    
    printf("\n  ─── BLOOM-OPTIMISED JOIN ───\n");
    printf("    Matches found:       %zu\n", matches_bloom);
    printf("    Bloom lookups:       %zu\n", stats_bloom.bloom_lookups);
    printf("    Bloom positives:     %zu\n", stats_bloom.bloom_positives);
    printf("    Rows examined:       %zu\n", stats_bloom.rows_examined);
    printf("    Time:                %.3f ms\n", stats_bloom.time_ms);
    
    printf("\n  ─── NAIVE NESTED LOOP JOIN ───\n");
    printf("    Matches found:       %zu\n", matches_naive);
    printf("    Full scans:          %zu\n", stats_naive.full_scans);
    printf("    Rows examined:       %zu\n", stats_naive.rows_examined);
    printf("    Time:                %.3f ms\n", stats_naive.time_ms);
    
    printf("\n  ─── COMPARISON ───\n");
    printf("    Rows examined saved: %.2fx\n",
           stats_naive.rows_examined > 0 ?
           (double)stats_naive.rows_examined / stats_bloom.rows_examined : 0);
    printf("    Time saved:          %.2fx\n",
           stats_bloom.time_ms > 0 ?
           stats_naive.time_ms / stats_bloom.time_ms : 0);
}

void print_memory_usage(DatabaseTable *table) {
    printf("\n  Memory usage for '%s':\n", table->name);
    printf("    Row data:            %zu bytes\n", 
           table->num_rows * sizeof(TableRow));
    printf("    PK Bloom filter:     %zu bytes\n", 
           bloom_memory_bytes(table->pk_bloom));
    printf("    FK Bloom filter:     %zu bytes\n", 
           bloom_memory_bytes(table->fk_bloom));
    printf("    Skip list index:     %zu bytes\n", 
           skip_memory_bytes(table->pk_index));
}

/* =============================================================================
 * PART 7: MAIN PROGRAMME
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║      HOMEWORK 2: DATABASE QUERY OPTIMISER SIMULATOR           ║\n");
    printf("║         Using Bloom Filters and Skip Lists                    ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    srand(42);  /* Fixed seed for reproducibility */
    
    /* Create simulated database tables */
    printf("\n► Creating simulated database tables...\n");
    
    DatabaseTable *customers = table_create("customers", 10000, 1, 10000);
    DatabaseTable *orders = table_create("orders", 50000, 100001, 10000);
    DatabaseTable *products = table_create("products", 5000, 1, 1000);
    
    if (!customers || !orders || !products) {
        fprintf(stderr, "Error: Failed to create tables\n");
        return 1;
    }
    
    printf("  ✓ Created 'customers' table with %zu rows\n", customers->num_rows);
    printf("  ✓ Created 'orders' table with %zu rows\n", orders->num_rows);
    printf("  ✓ Created 'products' table with %zu rows\n", products->num_rows);
    
    /* Memory usage analysis */
    printf("\n");
    printf("┌────────────────────────────────────────────────────────────────┐\n");
    printf("│                 MEMORY USAGE ANALYSIS                          │\n");
    printf("└────────────────────────────────────────────────────────────────┘\n");
    
    print_memory_usage(customers);
    print_memory_usage(orders);
    print_memory_usage(products);
    
    /* Benchmark point queries */
    benchmark_point_queries(customers, 10000);
    
    /* Benchmark range queries */
    benchmark_range_queries(customers, 1000, 100);
    
    /* Benchmark joins */
    benchmark_joins(orders, customers);
    
    /* Summary */
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    KEY FINDINGS                               ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║                                                               ║\n");
    printf("║  1. Bloom filters eliminate unnecessary lookups               ║\n");
    printf("║     - Pre-check membership before expensive operations        ║\n");
    printf("║     - ~1%% false positive rate with optimal parameters        ║\n");
    printf("║                                                               ║\n");
    printf("║  2. Skip lists provide efficient sorted access                ║\n");
    printf("║     - O(log n) expected time for point queries                ║\n");
    printf("║     - Efficient range queries with sequential access          ║\n");
    printf("║                                                               ║\n");
    printf("║  3. Join optimisation with Bloom filters                      ║\n");
    printf("║     - Reduces rows examined by order of magnitude             ║\n");
    printf("║     - Most effective when join selectivity is low             ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Clean up */
    table_free(customers);
    table_free(orders);
    table_free(products);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    SIMULATION COMPLETE                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

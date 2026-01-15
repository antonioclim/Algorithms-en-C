/**
 * =============================================================================
 * WEEK 17: PROBABILISTIC DATA STRUCTURES FOR BIG DATA
 * Homework 1 SOLUTION: URL Deduplication System
 * =============================================================================
 *
 * This solution implements a URL deduplication system using Bloom filters
 * to efficiently track seen URLs in a streaming context.
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c -lm
 * Usage: ./homework1_sol <url_stream_file>
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

#define MAX_URL_LENGTH 2048
#define INITIAL_HASHSET_SIZE 10007

/* =============================================================================
 * PART 1: HASH FUNCTIONS
 * =============================================================================
 */

/**
 * MurmurHash64 implementation for probabilistic structures
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
 * Kirsch-Mitzenmacher technique: generate k hashes from 2 base hashes
 */
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

/**
 * Create Bloom filter with specified parameters
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
 * Create optimal Bloom filter given expected items and FP rate
 * 
 * Formulas:
 *   m = -n × ln(p) / (ln(2))²   [optimal bits]
 *   k = (m/n) × ln(2)           [optimal hash functions]
 */
BloomFilter *bloom_create_optimal(size_t expected_items, double fp_rate) {
    double m = -((double)expected_items * log(fp_rate)) / (log(2) * log(2));
    size_t num_bits = (size_t)ceil(m);
    
    double k = ((double)num_bits / expected_items) * log(2);
    size_t num_hashes = (size_t)ceil(k);
    if (num_hashes < 1) num_hashes = 1;
    
    return bloom_create(num_bits, num_hashes);
}

/**
 * Free Bloom filter resources
 */
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
 * Insert element into Bloom filter
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
 * Query Bloom filter for membership
 * Returns true if element MIGHT be in set (possible false positive)
 * Returns false if element is DEFINITELY NOT in set
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
 * Calculate theoretical false positive rate
 * FP ≈ (1 - e^(-kn/m))^k
 */
double bloom_fp_rate(const BloomFilter *bf) {
    double exp_val = exp(-(double)(bf->num_hashes * bf->num_items) / bf->num_bits);
    return pow(1 - exp_val, bf->num_hashes);
}

/**
 * Get memory usage in bytes
 */
size_t bloom_memory_usage(const BloomFilter *bf) {
    return (bf->num_bits + 7) / 8 + sizeof(BloomFilter);
}

/* =============================================================================
 * PART 3: HASH SET FOR EXACT COMPARISON
 * =============================================================================
 */

typedef struct HashSetNode {
    char *url;
    struct HashSetNode *next;
} HashSetNode;

typedef struct {
    HashSetNode **buckets;
    size_t num_buckets;
    size_t num_items;
} HashSet;

HashSet *hashset_create(size_t num_buckets) {
    HashSet *hs = malloc(sizeof(HashSet));
    if (!hs) return NULL;
    
    hs->buckets = calloc(num_buckets, sizeof(HashSetNode *));
    if (!hs->buckets) {
        free(hs);
        return NULL;
    }
    
    hs->num_buckets = num_buckets;
    hs->num_items = 0;
    return hs;
}

void hashset_free(HashSet *hs) {
    if (!hs) return;
    
    for (size_t i = 0; i < hs->num_buckets; i++) {
        HashSetNode *node = hs->buckets[i];
        while (node) {
            HashSetNode *next = node->next;
            free(node->url);
            free(node);
            node = next;
        }
    }
    free(hs->buckets);
    free(hs);
}

bool hashset_contains(const HashSet *hs, const char *url) {
    uint64_t hash = murmur_hash64(url, strlen(url), 0);
    size_t idx = hash % hs->num_buckets;
    
    HashSetNode *node = hs->buckets[idx];
    while (node) {
        if (strcmp(node->url, url) == 0) {
            return true;
        }
        node = node->next;
    }
    return false;
}

bool hashset_insert(HashSet *hs, const char *url) {
    if (hashset_contains(hs, url)) {
        return false; /* Already exists */
    }
    
    uint64_t hash = murmur_hash64(url, strlen(url), 0);
    size_t idx = hash % hs->num_buckets;
    
    HashSetNode *node = malloc(sizeof(HashSetNode));
    if (!node) return false;
    
    node->url = strdup(url);
    if (!node->url) {
        free(node);
        return false;
    }
    
    node->next = hs->buckets[idx];
    hs->buckets[idx] = node;
    hs->num_items++;
    return true;
}

/**
 * Estimate memory usage of hash set
 */
size_t hashset_memory_usage(const HashSet *hs) {
    size_t mem = sizeof(HashSet) + hs->num_buckets * sizeof(HashSetNode *);
    
    for (size_t i = 0; i < hs->num_buckets; i++) {
        HashSetNode *node = hs->buckets[i];
        while (node) {
            mem += sizeof(HashSetNode) + strlen(node->url) + 1;
            node = node->next;
        }
    }
    return mem;
}

/* =============================================================================
 * PART 4: URL NORMALISATION
 * =============================================================================
 */

/**
 * Normalise URL for consistent hashing
 * - Convert to lowercase
 * - Remove trailing slashes
 * - Remove default ports
 */
void normalise_url(char *url) {
    /* Convert to lowercase */
    for (int i = 0; url[i]; i++) {
        if (url[i] >= 'A' && url[i] <= 'Z') {
            url[i] = url[i] + 32;
        }
    }
    
    /* Remove trailing slash */
    size_t len = strlen(url);
    while (len > 0 && url[len - 1] == '/') {
        url[--len] = '\0';
    }
    
    /* Remove :80 for HTTP and :443 for HTTPS */
    char *port80 = strstr(url, ":80/");
    if (port80) {
        memmove(port80, port80 + 3, strlen(port80 + 3) + 1);
    }
    
    char *port443 = strstr(url, ":443/");
    if (port443) {
        memmove(port443, port443 + 4, strlen(port443 + 4) + 1);
    }
}

/* =============================================================================
 * PART 5: URL STREAM GENERATOR
 * =============================================================================
 */

/**
 * Generate synthetic URL stream with configurable duplicate rate
 */
void generate_url_stream(const char *filename, size_t num_urls, double dup_rate) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Cannot create URL stream file\n");
        return;
    }
    
    const char *domains[] = {
        "example.com", "test.org", "sample.net", "demo.io",
        "website.co.uk", "mysite.edu", "data.gov", "news.info"
    };
    const char *paths[] = {
        "/home", "/about", "/products", "/services", "/contact",
        "/blog", "/news", "/faq", "/help", "/login", "/signup",
        "/article", "/post", "/page", "/category", "/tag"
    };
    int num_domains = sizeof(domains) / sizeof(domains[0]);
    int num_paths = sizeof(paths) / sizeof(paths[0]);
    
    /* Store some URLs for duplicates */
    char **recent_urls = malloc(100 * sizeof(char *));
    size_t recent_count = 0;
    
    srand(42); /* Fixed seed for reproducibility */
    
    for (size_t i = 0; i < num_urls; i++) {
        char url[MAX_URL_LENGTH];
        
        /* Decide if this should be a duplicate */
        bool is_duplicate = (recent_count > 0) && 
                           ((double)rand() / RAND_MAX < dup_rate);
        
        if (is_duplicate) {
            size_t idx = rand() % recent_count;
            fprintf(file, "%s\n", recent_urls[idx]);
        } else {
            int domain_idx = rand() % num_domains;
            int path_idx = rand() % num_paths;
            int id = rand() % 10000;
            
            snprintf(url, sizeof(url), "https://%s%s/%d",
                     domains[domain_idx], paths[path_idx], id);
            fprintf(file, "%s\n", url);
            
            /* Store for potential future duplicates */
            if (recent_count < 100) {
                recent_urls[recent_count++] = strdup(url);
            } else {
                size_t replace_idx = rand() % 100;
                free(recent_urls[replace_idx]);
                recent_urls[replace_idx] = strdup(url);
            }
        }
    }
    
    /* Clean up */
    for (size_t i = 0; i < recent_count; i++) {
        free(recent_urls[i]);
    }
    free(recent_urls);
    
    fclose(file);
}

/* =============================================================================
 * PART 6: DEDUPLICATION SYSTEM
 * =============================================================================
 */

typedef struct {
    size_t total_urls;
    size_t unique_urls_bloom;
    size_t unique_urls_exact;
    size_t duplicates_bloom;
    size_t duplicates_exact;
    size_t false_positives;
    double bloom_memory_kb;
    double exact_memory_kb;
    double compression_ratio;
} DeduplicationStats;

/**
 * Process URL stream with both Bloom filter and exact hash set
 */
DeduplicationStats process_url_stream(const char *filename,
                                       size_t expected_unique,
                                       double fp_rate) {
    DeduplicationStats stats = {0};
    
    /* Create Bloom filter */
    BloomFilter *bf = bloom_create_optimal(expected_unique, fp_rate);
    if (!bf) {
        fprintf(stderr, "Error: Failed to create Bloom filter\n");
        return stats;
    }
    
    /* Create hash set for exact comparison */
    HashSet *hs = hashset_create(expected_unique * 2);
    if (!hs) {
        bloom_free(bf);
        fprintf(stderr, "Error: Failed to create hash set\n");
        return stats;
    }
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        bloom_free(bf);
        hashset_free(hs);
        fprintf(stderr, "Error: Cannot open URL stream file\n");
        return stats;
    }
    
    char url[MAX_URL_LENGTH];
    
    while (fgets(url, sizeof(url), file)) {
        url[strcspn(url, "\r\n")] = '\0';
        if (strlen(url) == 0) continue;
        
        normalise_url(url);
        stats.total_urls++;
        
        /* Check Bloom filter */
        bool bloom_seen = bloom_query(bf, url, strlen(url));
        
        /* Check exact hash set */
        bool exact_seen = hashset_contains(hs, url);
        
        if (bloom_seen) {
            stats.duplicates_bloom++;
            
            /* Check for false positive */
            if (!exact_seen) {
                stats.false_positives++;
            }
        } else {
            bloom_insert(bf, url, strlen(url));
            stats.unique_urls_bloom++;
        }
        
        if (exact_seen) {
            stats.duplicates_exact++;
        } else {
            hashset_insert(hs, url);
            stats.unique_urls_exact++;
        }
    }
    
    fclose(file);
    
    /* Calculate memory usage */
    stats.bloom_memory_kb = bloom_memory_usage(bf) / 1024.0;
    stats.exact_memory_kb = hashset_memory_usage(hs) / 1024.0;
    stats.compression_ratio = stats.exact_memory_kb / stats.bloom_memory_kb;
    
    bloom_free(bf);
    hashset_free(hs);
    
    return stats;
}

/* =============================================================================
 * PART 7: MAIN PROGRAMME
 * =============================================================================
 */

void print_stats(const DeduplicationStats *stats) {
    printf("\n");
    printf("┌────────────────────────────────────────────────────────────────┐\n");
    printf("│                 URL DEDUPLICATION RESULTS                      │\n");
    printf("├────────────────────────────────────────────────────────────────┤\n");
    printf("│                                                                │\n");
    printf("│  Total URLs processed:        %10zu                       │\n", stats->total_urls);
    printf("│                                                                │\n");
    printf("│  ─── BLOOM FILTER ───                                          │\n");
    printf("│  Unique URLs detected:        %10zu                       │\n", stats->unique_urls_bloom);
    printf("│  Duplicates filtered:         %10zu                       │\n", stats->duplicates_bloom);
    printf("│  Memory usage:                %10.2f KB                   │\n", stats->bloom_memory_kb);
    printf("│                                                                │\n");
    printf("│  ─── EXACT HASH SET ───                                        │\n");
    printf("│  Unique URLs (ground truth):  %10zu                       │\n", stats->unique_urls_exact);
    printf("│  Actual duplicates:           %10zu                       │\n", stats->duplicates_exact);
    printf("│  Memory usage:                %10.2f KB                   │\n", stats->exact_memory_kb);
    printf("│                                                                │\n");
    printf("│  ─── ACCURACY ANALYSIS ───                                     │\n");
    printf("│  False positives:             %10zu                       │\n", stats->false_positives);
    printf("│  False positive rate:         %10.4f%%                     │\n", 
           stats->unique_urls_exact > 0 ? 
           (100.0 * stats->false_positives / stats->unique_urls_exact) : 0);
    printf("│  Memory savings:              %10.2fx                      │\n", stats->compression_ratio);
    printf("│                                                                │\n");
    printf("└────────────────────────────────────────────────────────────────┘\n");
}

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║        HOMEWORK 1: URL DEDUPLICATION SYSTEM                   ║\n");
    printf("║            Using Bloom Filters for Big Data                   ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    char *input_file = NULL;
    bool generate_synthetic = false;
    size_t num_synthetic = 100000;
    double dup_rate = 0.3;
    double fp_rate = 0.01;
    
    if (argc < 2) {
        printf("\nNo input file provided. Generating synthetic URL stream...\n");
        generate_synthetic = true;
        input_file = "/tmp/url_stream.txt";
    } else {
        input_file = argv[1];
        if (argc >= 3) {
            fp_rate = atof(argv[2]);
            if (fp_rate <= 0 || fp_rate >= 1) fp_rate = 0.01;
        }
    }
    
    if (generate_synthetic) {
        printf("Generating %zu URLs with %.0f%% duplicate rate...\n", 
               num_synthetic, dup_rate * 100);
        generate_url_stream(input_file, num_synthetic, dup_rate);
    }
    
    /* Estimate expected unique URLs */
    size_t expected_unique;
    if (generate_synthetic) {
        expected_unique = (size_t)(num_synthetic * (1 - dup_rate) * 1.2);
    } else {
        /* Count lines as estimate */
        FILE *f = fopen(input_file, "r");
        if (!f) {
            fprintf(stderr, "Error: Cannot open '%s'\n", input_file);
            return 1;
        }
        expected_unique = 0;
        int ch;
        while ((ch = fgetc(f)) != EOF) {
            if (ch == '\n') expected_unique++;
        }
        fclose(f);
    }
    
    printf("\nProcessing URL stream from: %s\n", input_file);
    printf("Expected unique URLs: ~%zu\n", expected_unique);
    printf("Target false positive rate: %.2f%%\n", fp_rate * 100);
    
    DeduplicationStats stats = process_url_stream(input_file, expected_unique, fp_rate);
    print_stats(&stats);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    ANALYSIS COMPLETE                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

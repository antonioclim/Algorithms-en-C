/**
 * =============================================================================
 * WEEK 17: PROBABILISTIC DATA STRUCTURES FOR BIG DATA
 * Exercise 1: Spell Checker with Bloom Filter
 * =============================================================================
 *
 * OBJECTIVE:
 * Implement a spell-checking system using a Bloom filter to store a dictionary
 * of valid words. Your implementation must achieve a false positive rate below
 * 1% whilst minimising memory usage.
 *
 * TASKS:
 *   1. Implement the BloomFilter structure and operations
 *   2. Calculate optimal parameters given dictionary size and desired FP rate
 *   3. Load dictionary from file and insert all words
 *   4. Check words from input file and report suspected misspellings
 *   5. Measure and report actual false positive rate
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c -lm
 * Usage: ./exercise1 <dictionary_file> <words_to_check_file>
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 128

/* =============================================================================
 * HASH FUNCTION
 * =============================================================================
 */

/**
 * MurmurHash3-inspired 64-bit hash function.
 * 
 * TODO: This implementation is provided. Study it to understand how it works.
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
 * This generates k independent hashes from just two base hashes.
 */
uint64_t get_hash_i(const void *data, size_t len, int i) {
    uint64_t h1 = murmur_hash64(data, len, 0);
    uint64_t h2 = murmur_hash64(data, len, h1);
    return h1 + i * h2;
}

/* =============================================================================
 * BLOOM FILTER STRUCTURE
 * =============================================================================
 */

typedef struct {
    uint8_t *bits;          /* Bit array */
    size_t num_bits;        /* Size of bit array (m) */
    size_t num_hashes;      /* Number of hash functions (k) */
    size_t num_items;       /* Number of inserted items */
} BloomFilter;

/* =============================================================================
 * TODO: IMPLEMENT THESE FUNCTIONS
 * =============================================================================
 */

/**
 * TODO 1: Create a new Bloom filter with specified parameters.
 * 
 * @param num_bits Size of bit array (m)
 * @param num_hashes Number of hash functions (k)
 * @return Pointer to new BloomFilter, or NULL on failure
 *
 * Hint: 
 *   - Allocate the BloomFilter structure
 *   - Calculate bytes needed: (num_bits + 7) / 8
 *   - Use calloc to zero-initialise the bit array
 */
BloomFilter *bloom_create(size_t num_bits, size_t num_hashes) {
    /* TODO: Implement this function */
    (void)num_bits;
    (void)num_hashes;
    return NULL;
}

/**
 * TODO 2: Create optimal Bloom filter given expected items and FP rate.
 * 
 * @param expected_items Number of items expected to insert (n)
 * @param fp_rate Desired false positive probability (e.g., 0.01 for 1%)
 * @return Pointer to new BloomFilter with optimal parameters
 *
 * Formulas:
 *   m = -n * ln(p) / (ln(2))²   (optimal number of bits)
 *   k = (m/n) * ln(2)           (optimal number of hash functions)
 */
BloomFilter *bloom_create_optimal(size_t expected_items, double fp_rate) {
    /* TODO: Implement this function */
    (void)expected_items;
    (void)fp_rate;
    return NULL;
}

/**
 * TODO 3: Free all memory associated with a Bloom filter.
 */
void bloom_free(BloomFilter *bf) {
    /* TODO: Implement this function */
    (void)bf;
}

/**
 * Helper: Set a bit in the bit array.
 * 
 * Hint: bits[idx / 8] |= (1 << (idx % 8));
 */
static inline void set_bit(uint8_t *bits, size_t idx) {
    bits[idx / 8] |= (1 << (idx % 8));
}

/**
 * Helper: Get a bit from the bit array.
 * 
 * Hint: return (bits[idx / 8] >> (idx % 8)) & 1;
 */
static inline bool get_bit(const uint8_t *bits, size_t idx) {
    return (bits[idx / 8] >> (idx % 8)) & 1;
}

/**
 * TODO 4: Insert an element into the Bloom filter.
 * 
 * @param bf Pointer to BloomFilter
 * @param data Pointer to data to insert
 * @param len Length of data in bytes
 *
 * Algorithm:
 *   For each hash function i from 0 to k-1:
 *     1. Compute hash: h = get_hash_i(data, len, i)
 *     2. Compute index: idx = h % num_bits
 *     3. Set the bit at idx
 *   Increment num_items
 */
void bloom_insert(BloomFilter *bf, const void *data, size_t len) {
    /* TODO: Implement this function */
    (void)bf;
    (void)data;
    (void)len;
}

/**
 * TODO 5: Query if an element might be in the set.
 * 
 * @param bf Pointer to BloomFilter
 * @param data Pointer to data to query
 * @param len Length of data in bytes
 * @return false = definitely NOT in set, true = POSSIBLY in set
 *
 * Algorithm:
 *   For each hash function i from 0 to k-1:
 *     1. Compute hash: h = get_hash_i(data, len, i)
 *     2. Compute index: idx = h % num_bits
 *     3. If bit at idx is 0, return false (definitely not in set)
 *   Return true (all bits set, possibly in set)
 */
bool bloom_query(const BloomFilter *bf, const void *data, size_t len) {
    /* TODO: Implement this function */
    (void)bf;
    (void)data;
    (void)len;
    return false;
}

/**
 * TODO 6: Calculate theoretical false positive rate.
 * 
 * Formula: P(FP) = (1 - e^(-kn/m))^k
 * 
 * where:
 *   k = num_hashes
 *   n = num_items
 *   m = num_bits
 */
double bloom_fp_rate(const BloomFilter *bf) {
    /* TODO: Implement this function */
    (void)bf;
    return 0.0;
}

/* =============================================================================
 * UTILITY FUNCTIONS (PROVIDED)
 * =============================================================================
 */

/**
 * Convert word to lowercase for consistent matching.
 */
void to_lowercase(char *word) {
    for (int i = 0; word[i]; i++) {
        word[i] = tolower((unsigned char)word[i]);
    }
}

/**
 * Strip punctuation from word.
 */
void strip_punctuation(char *word) {
    char *src = word, *dst = word;
    while (*src) {
        if (isalpha((unsigned char)*src) || *src == '\'') {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}

/**
 * Count lines in file (to estimate dictionary size).
 */
size_t count_lines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;
    
    size_t count = 0;
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') count++;
    }
    
    fclose(file);
    return count;
}

/**
 * Load dictionary into Bloom filter.
 */
size_t load_dictionary(BloomFilter *bf, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open dictionary file '%s'\n", filename);
        return 0;
    }
    
    char word[MAX_WORD_LENGTH];
    size_t loaded = 0;
    
    while (fgets(word, sizeof(word), file)) {
        /* Remove newline */
        word[strcspn(word, "\r\n")] = '\0';
        
        if (strlen(word) > 0) {
            to_lowercase(word);
            bloom_insert(bf, word, strlen(word));
            loaded++;
        }
    }
    
    fclose(file);
    return loaded;
}

/**
 * Check words from file and report misspellings.
 */
void check_words(BloomFilter *bf, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open words file '%s'\n", filename);
        return;
    }
    
    printf("\nChecking words from '%s':\n", filename);
    printf("────────────────────────────────────────────────────────────────\n");
    
    char word[MAX_WORD_LENGTH];
    size_t total_words = 0;
    size_t possibly_misspelled = 0;
    
    while (fgets(word, sizeof(word), file)) {
        word[strcspn(word, "\r\n")] = '\0';
        
        if (strlen(word) == 0) continue;
        
        strip_punctuation(word);
        if (strlen(word) == 0) continue;
        
        char lower_word[MAX_WORD_LENGTH];
        strcpy(lower_word, word);
        to_lowercase(lower_word);
        
        total_words++;
        
        if (!bloom_query(bf, lower_word, strlen(lower_word))) {
            printf("  ✗ '%s' — possibly misspelled\n", word);
            possibly_misspelled++;
        }
    }
    
    printf("────────────────────────────────────────────────────────────────\n");
    printf("Total words checked: %zu\n", total_words);
    printf("Possibly misspelled: %zu\n", possibly_misspelled);
    
    fclose(file);
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <dictionary_file> <words_to_check_file>\n", argv[0]);
        printf("\nThis spell checker uses a Bloom filter to efficiently test\n");
        printf("whether words are likely to be in the dictionary.\n");
        return 1;
    }
    
    const char *dict_file = argv[1];
    const char *check_file = argv[2];
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 1: BLOOM FILTER SPELL CHECKER                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Count dictionary size */
    printf("Analysing dictionary...\n");
    size_t dict_size = count_lines(dict_file);
    if (dict_size == 0) {
        fprintf(stderr, "Error: Empty or inaccessible dictionary\n");
        return 1;
    }
    printf("  Dictionary entries: %zu\n", dict_size);
    
    /* Create optimal Bloom filter for 1% FP rate */
    printf("\nCreating Bloom filter...\n");
    BloomFilter *bf = bloom_create_optimal(dict_size, 0.01);
    if (!bf) {
        fprintf(stderr, "Error: Failed to create Bloom filter\n");
        return 1;
    }
    
    printf("  Bits: %zu\n", bf->num_bits);
    printf("  Hash functions: %zu\n", bf->num_hashes);
    printf("  Memory: %zu bytes (%.2f KB)\n", 
           (bf->num_bits + 7) / 8, 
           (bf->num_bits + 7) / 8.0 / 1024);
    
    /* Load dictionary */
    printf("\nLoading dictionary...\n");
    size_t loaded = load_dictionary(bf, dict_file);
    printf("  Loaded %zu words\n", loaded);
    printf("  Theoretical FP rate: %.4f%%\n", bloom_fp_rate(bf) * 100);
    
    /* Check words */
    check_words(bf, check_file);
    
    /* Cleanup */
    bloom_free(bf);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    EXERCISE COMPLETE                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

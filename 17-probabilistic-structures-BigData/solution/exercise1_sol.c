/**
 * =============================================================================
 * WEEK 17: PROBABILISTIC DATA STRUCTURES FOR BIG DATA
 * Exercise 1 SOLUTION: Spell Checker with Bloom Filter
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

/* Hash function */
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

/* Bloom Filter */
typedef struct {
    uint8_t *bits;
    size_t num_bits;
    size_t num_hashes;
    size_t num_items;
} BloomFilter;

/* SOLUTION: Create Bloom filter */
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

/* SOLUTION: Create optimal Bloom filter */
BloomFilter *bloom_create_optimal(size_t expected_items, double fp_rate) {
    /* m = -n * ln(p) / (ln(2))² */
    double m = -((double)expected_items * log(fp_rate)) / (log(2) * log(2));
    size_t num_bits = (size_t)ceil(m);
    
    /* k = (m/n) * ln(2) */
    double k = ((double)num_bits / expected_items) * log(2);
    size_t num_hashes = (size_t)ceil(k);
    if (num_hashes < 1) num_hashes = 1;
    
    return bloom_create(num_bits, num_hashes);
}

/* SOLUTION: Free Bloom filter */
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

/* SOLUTION: Insert element */
void bloom_insert(BloomFilter *bf, const void *data, size_t len) {
    for (size_t i = 0; i < bf->num_hashes; i++) {
        uint64_t hash = get_hash_i(data, len, i);
        size_t idx = hash % bf->num_bits;
        set_bit(bf->bits, idx);
    }
    bf->num_items++;
}

/* SOLUTION: Query element */
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

/* SOLUTION: Calculate FP rate */
double bloom_fp_rate(const BloomFilter *bf) {
    double exp_val = exp(-(double)(bf->num_hashes * bf->num_items) / bf->num_bits);
    return pow(1 - exp_val, bf->num_hashes);
}

/* Utility functions */
void to_lowercase(char *word) {
    for (int i = 0; word[i]; i++) {
        word[i] = tolower((unsigned char)word[i]);
    }
}

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

size_t load_dictionary(BloomFilter *bf, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;
    
    char word[MAX_WORD_LENGTH];
    size_t loaded = 0;
    
    while (fgets(word, sizeof(word), file)) {
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

void check_words(BloomFilter *bf, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open '%s'\n", filename);
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

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <dictionary_file> <words_to_check_file>\n", argv[0]);
        return 1;
    }
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 1 SOLUTION: BLOOM FILTER SPELL CHECKER           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    size_t dict_size = count_lines(argv[1]);
    if (dict_size == 0) {
        fprintf(stderr, "Error: Empty dictionary\n");
        return 1;
    }
    printf("Dictionary entries: %zu\n", dict_size);
    
    BloomFilter *bf = bloom_create_optimal(dict_size, 0.01);
    if (!bf) {
        fprintf(stderr, "Error: Failed to create Bloom filter\n");
        return 1;
    }
    
    printf("Bloom filter: %zu bits, %zu hashes, %zu bytes\n",
           bf->num_bits, bf->num_hashes, (bf->num_bits + 7) / 8);
    
    size_t loaded = load_dictionary(bf, argv[1]);
    printf("Loaded %zu words\n", loaded);
    printf("Theoretical FP rate: %.4f%%\n", bloom_fp_rate(bf) * 100);
    
    check_words(bf, argv[2]);
    
    bloom_free(bf);
    
    printf("\n✓ Exercise 1 Solution Complete\n\n");
    return 0;
}

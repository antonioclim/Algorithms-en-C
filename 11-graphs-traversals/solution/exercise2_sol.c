/**
 * =============================================================================
 * EXERCISE 2 SOLUTION: Word Frequency Counter (Open Addressing)
 * =============================================================================
 *
 * INSTRUCTOR SOLUTION - DO NOT DISTRIBUTE TO STUDENTS
 *
 * This solution implements:
 *   - FNV-1a hash function (primary)
 *   - Double hashing for open addressing
 *   - Tombstone deletion
 *   - Automatic rehashing at load factor > 0.7
 *   - Word frequency counting from text file
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c
 * Usage: ./exercise2_sol data/text_sample.txt
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

#define INITIAL_SIZE 127  /* Prime number for good distribution */
#define MAX_WORD_LEN 64
#define MAX_LOAD_FACTOR 0.7

/**
 * Entry state enumeration
 */
typedef enum {
    EMPTY,
    OCCUPIED,
    DELETED  /* Tombstone marker */
} EntryState;

/**
 * Hash table entry structure
 */
typedef struct {
    char *word;
    int count;
    EntryState state;
} Entry;

/**
 * Open addressing hash table structure
 */
typedef struct {
    Entry *entries;
    int size;
    int count;
    int tombstones;
    long total_probes;
    long total_ops;
} HashTable;

/* =============================================================================
 * HASH FUNCTIONS
 * =============================================================================
 */

/**
 * FNV-1a hash function (primary hash)
 * 
 * @param key The string key to hash
 * @return The hash value
 */
unsigned int hash_fnv1a(const char *key) {
    unsigned int hash = 2166136261u;  /* FNV offset basis */
    
    while (*key) {
        hash ^= (unsigned char)*key++;
        hash *= 16777619u;  /* FNV prime */
    }
    
    return hash;
}

/**
 * Secondary hash function for double hashing
 * Returns an odd value to ensure coprimality with table size
 * 
 * @param key The string key to hash
 * @param size The table size
 * @return The step size (always odd)
 */
unsigned int hash_secondary(const char *key, int size) {
    unsigned int hash = 5381;  /* djb2 starting value */
    int c;
    
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    /* Ensure result is odd (coprime to any table size) */
    return (hash % (size - 1)) | 1;
}

/* =============================================================================
 * HASH TABLE OPERATIONS
 * =============================================================================
 */

/**
 * Create a new hash table
 * 
 * @param size The initial number of slots
 * @return Pointer to the new hash table, or NULL on failure
 */
HashTable *ht_create(int size) {
    HashTable *ht = malloc(sizeof(HashTable));
    if (ht == NULL) return NULL;
    
    ht->size = size;
    ht->count = 0;
    ht->tombstones = 0;
    ht->total_probes = 0;
    ht->total_ops = 0;
    
    ht->entries = calloc(size, sizeof(Entry));
    if (ht->entries == NULL) {
        free(ht);
        return NULL;
    }
    
    /* Initialize all entries as empty */
    for (int i = 0; i < size; i++) {
        ht->entries[i].state = EMPTY;
        ht->entries[i].word = NULL;
    }
    
    return ht;
}

/**
 * Calculate the effective load factor
 * 
 * @param ht The hash table
 * @return The load factor including tombstones
 */
float ht_load_factor(HashTable *ht) {
    return (float)(ht->count + ht->tombstones) / ht->size;
}

/* Forward declaration */
void ht_rehash(HashTable *ht);

/**
 * Insert or increment a word's count
 * 
 * @param ht The hash table
 * @param word The word to insert/increment
 * @return The number of probes required
 */
int ht_insert(HashTable *ht, const char *word) {
    /* Check if rehashing is needed */
    if (ht_load_factor(ht) > MAX_LOAD_FACTOR) {
        ht_rehash(ht);
    }
    
    unsigned int index = hash_fnv1a(word) % ht->size;
    unsigned int step = hash_secondary(word, ht->size);
    int probes = 0;
    int first_tombstone = -1;
    
    /* Probe until we find empty slot or the word */
    while (ht->entries[index].state != EMPTY) {
        probes++;
        
        if (ht->entries[index].state == DELETED) {
            /* Remember first tombstone for potential insertion */
            if (first_tombstone == -1) {
                first_tombstone = index;
            }
        } else if (strcmp(ht->entries[index].word, word) == 0) {
            /* Word found - increment count */
            ht->entries[index].count++;
            ht->total_probes += probes;
            ht->total_ops++;
            return probes;
        }
        
        index = (index + step) % ht->size;
    }
    
    /* Word not found - insert it */
    int insert_index = (first_tombstone != -1) ? first_tombstone : index;
    
    if (ht->entries[insert_index].state == DELETED) {
        ht->tombstones--;
    }
    
    ht->entries[insert_index].word = strdup(word);
    ht->entries[insert_index].count = 1;
    ht->entries[insert_index].state = OCCUPIED;
    ht->count++;
    
    ht->total_probes += probes;
    ht->total_ops++;
    
    return probes;
}

/**
 * Search for a word and return its count
 * 
 * @param ht The hash table
 * @param word The word to search for
 * @param probes Output parameter for probe count
 * @return The word count, or 0 if not found
 */
int ht_search(HashTable *ht, const char *word, int *probes) {
    unsigned int index = hash_fnv1a(word) % ht->size;
    unsigned int step = hash_secondary(word, ht->size);
    *probes = 0;
    
    while (ht->entries[index].state != EMPTY) {
        (*probes)++;
        
        if (ht->entries[index].state == OCCUPIED &&
            strcmp(ht->entries[index].word, word) == 0) {
            return ht->entries[index].count;
        }
        
        index = (index + step) % ht->size;
    }
    
    return 0;  /* Not found */
}

/**
 * Delete a word (mark as tombstone)
 * 
 * @param ht The hash table
 * @param word The word to delete
 * @return true if deleted, false if not found
 */
bool ht_delete(HashTable *ht, const char *word) {
    unsigned int index = hash_fnv1a(word) % ht->size;
    unsigned int step = hash_secondary(word, ht->size);
    
    while (ht->entries[index].state != EMPTY) {
        if (ht->entries[index].state == OCCUPIED &&
            strcmp(ht->entries[index].word, word) == 0) {
            /* Mark as tombstone */
            free(ht->entries[index].word);
            ht->entries[index].word = NULL;
            ht->entries[index].state = DELETED;
            ht->count--;
            ht->tombstones++;
            return true;
        }
        
        index = (index + step) % ht->size;
    }
    
    return false;
}

/**
 * Rehash the table (double the size)
 * 
 * @param ht The hash table to rehash
 */
void ht_rehash(HashTable *ht) {
    Entry *old_entries = ht->entries;
    int old_size = ht->size;
    
    /* Create new larger table */
    ht->size = old_size * 2;
    ht->entries = calloc(ht->size, sizeof(Entry));
    ht->count = 0;
    ht->tombstones = 0;
    
    /* Initialize new entries */
    for (int i = 0; i < ht->size; i++) {
        ht->entries[i].state = EMPTY;
        ht->entries[i].word = NULL;
    }
    
    /* Reinsert all occupied entries */
    for (int i = 0; i < old_size; i++) {
        if (old_entries[i].state == OCCUPIED) {
            /* Insert word with its count */
            unsigned int index = hash_fnv1a(old_entries[i].word) % ht->size;
            unsigned int step = hash_secondary(old_entries[i].word, ht->size);
            
            while (ht->entries[index].state == OCCUPIED) {
                index = (index + step) % ht->size;
            }
            
            ht->entries[index].word = old_entries[i].word;  /* Transfer ownership */
            ht->entries[index].count = old_entries[i].count;
            ht->entries[index].state = OCCUPIED;
            ht->count++;
        }
    }
    
    free(old_entries);
}

/**
 * Print hash table statistics
 * 
 * @param ht The hash table
 */
void ht_print_stats(HashTable *ht) {
    printf("--- Hash Table Statistics ---\n");
    printf("  Table size:       %d\n", ht->size);
    printf("  Entries:          %d\n", ht->count);
    printf("  Load factor:      %.2f\n", ht_load_factor(ht));
    printf("  Total probes:     %ld\n", ht->total_probes);
    if (ht->total_ops > 0) {
        printf("  Avg probes/op:    %.2f\n", 
               (float)ht->total_probes / ht->total_ops);
    }
}

/**
 * Destroy the hash table and free all memory
 * 
 * @param ht The hash table to destroy
 */
void ht_destroy(HashTable *ht) {
    for (int i = 0; i < ht->size; i++) {
        if (ht->entries[i].word != NULL) {
            free(ht->entries[i].word);
        }
    }
    free(ht->entries);
    free(ht);
}

/* =============================================================================
 * WORD PROCESSING FUNCTIONS
 * =============================================================================
 */

/**
 * Convert string to lowercase in place
 * 
 * @param str The string to convert
 */
void to_lowercase(char *str) {
    while (*str) {
        *str = tolower(*str);
        str++;
    }
}

/**
 * Remove non-alphabetic characters from string
 * 
 * @param str The string to clean
 */
void clean_word(char *str) {
    char *src = str, *dst = str;
    
    while (*src) {
        if (isalpha(*src)) {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}

/**
 * Structure for sorting word frequencies
 */
typedef struct {
    char *word;
    int count;
} WordFreq;

/**
 * Comparison function for qsort (descending by count)
 */
int compare_freq(const void *a, const void *b) {
    const WordFreq *fa = (const WordFreq *)a;
    const WordFreq *fb = (const WordFreq *)b;
    return fb->count - fa->count;  /* Descending order */
}

/**
 * Print top N most frequent words
 * 
 * @param ht The hash table
 * @param n Number of words to print
 */
void print_top_words(HashTable *ht, int n) {
    /* Collect all words */
    WordFreq *words = malloc(ht->count * sizeof(WordFreq));
    int idx = 0;
    
    for (int i = 0; i < ht->size; i++) {
        if (ht->entries[i].state == OCCUPIED) {
            words[idx].word = ht->entries[i].word;
            words[idx].count = ht->entries[i].count;
            idx++;
        }
    }
    
    /* Sort by frequency */
    qsort(words, ht->count, sizeof(WordFreq), compare_freq);
    
    /* Print top N */
    printf("\n--- Top %d Most Frequent Words ---\n", n);
    printf("  Rank | Word           | Count\n");
    printf("  ─────┼────────────────┼──────\n");
    
    int display = (n < ht->count) ? n : ht->count;
    for (int i = 0; i < display; i++) {
        printf("  %4d | %-14s | %d\n", i + 1, words[i].word, words[i].count);
    }
    
    free(words);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 2: Word Frequency Counter (Open Addressing)      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Check command line arguments */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    
    /* Open input file */
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", argv[1]);
        return 1;
    }
    
    printf("Processing file: %s\n\n", argv[1]);
    
    /* Create hash table */
    HashTable *ht = ht_create(INITIAL_SIZE);
    if (ht == NULL) {
        fprintf(stderr, "Error: Failed to create hash table\n");
        fclose(file);
        return 1;
    }
    
    /* Read and process words */
    char word[MAX_WORD_LEN];
    int total_words = 0;
    
    while (fscanf(file, "%63s", word) == 1) {
        /* Clean and normalize word */
        to_lowercase(word);
        clean_word(word);
        
        /* Skip empty words */
        if (strlen(word) > 0) {
            ht_insert(ht, word);
            total_words++;
        }
    }
    
    fclose(file);
    
    /* Print statistics */
    ht_print_stats(ht);
    
    /* Print top 10 words */
    print_top_words(ht, 10);
    
    /* Summary */
    printf("\nTotal unique words: %d\n", ht->count);
    printf("Total word count: %d\n", total_words);
    
    /* Clean up */
    ht_destroy(ht);
    
    printf("\nProgram completed successfully.\n");
    printf("Memory freed.\n");
    
    return 0;
}

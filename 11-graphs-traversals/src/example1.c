/**
 * =============================================================================
 * WEEK 11: HASH TABLES
 * Complete Working Example
 * =============================================================================
 */

#define _POSIX_C_SOURCE 200809L  /* Enable strdup declaration */

/*
 *
 * This example demonstrates:
 *   1. Hash function implementations (djb2, FNV-1a, division method)
 *   2. Chained hash table with full CRUD operations
 *   3. Open addressing with double hashing
 *   4. Load factor analysis and rehashing
 *   5. Performance measurement and statistics
 *   6. Memory management best practices
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Usage: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

/* =============================================================================
 * CONSTANTS AND CONFIGURATION
 * =============================================================================
 */

#define INITIAL_SIZE 7          /* Starting table size (prime) */
#define LOAD_FACTOR_THRESHOLD 0.75
#define FNV_OFFSET_BASIS 2166136261u
#define FNV_PRIME 16777619u

/* =============================================================================
 * PART 1: HASH FUNCTIONS
 * =============================================================================
 */

/**
 * djb2 hash function by Daniel J. Bernstein
 * Excellent empirical performance for strings
 */
unsigned int hash_djb2(const char *key) {
    unsigned int hash = 5381;
    int c;
    
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;  /* hash * 33 + c */
    }
    
    return hash;
}

/**
 * FNV-1a (Fowler-Noll-Vo) hash function
 * Fast and well-distributed
 */
unsigned int hash_fnv1a(const char *key) {
    unsigned int hash = FNV_OFFSET_BASIS;
    
    while (*key) {
        hash ^= (unsigned char)*key++;
        hash *= FNV_PRIME;
    }
    
    return hash;
}

/**
 * Simple division method with polynomial accumulation
 */
unsigned int hash_division(const char *key, int table_size) {
    unsigned int hash = 0;
    
    while (*key) {
        hash = hash * 31 + *key++;
    }
    
    return hash % table_size;
}

/**
 * Secondary hash for double hashing (must return odd value for power-of-2 sizes)
 */
unsigned int hash_secondary(const char *key, int table_size) {
    unsigned int hash = hash_fnv1a(key);
    /* Ensure result is odd and less than table_size */
    return (hash % (table_size - 1)) | 1;
}

void demo_hash_functions(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: HASH FUNCTIONS                                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    const char *test_keys[] = {"alice", "bob", "carol", "dave", "eve"};
    int num_keys = 5;
    int table_size = 11;
    
    printf("Testing hash functions with table_size = %d\n\n", table_size);
    printf("%-12s  %-12s  %-12s  %-12s\n", "Key", "djb2", "FNV-1a", "Division");
    printf("%-12s  %-12s  %-12s  %-12s\n", "---", "----", "------", "--------");
    
    for (int i = 0; i < num_keys; i++) {
        unsigned int h1 = hash_djb2(test_keys[i]) % table_size;
        unsigned int h2 = hash_fnv1a(test_keys[i]) % table_size;
        unsigned int h3 = hash_division(test_keys[i], table_size);
        
        printf("%-12s  %-12u  %-12u  %-12u\n", test_keys[i], h1, h2, h3);
    }
    
    printf("\n[!] Notice how different hash functions may produce different\n");
    printf("    distributions. Good hash functions minimise collisions.\n");
}

/* =============================================================================
 * PART 2: CHAINED HASH TABLE - DATA STRUCTURES
 * =============================================================================
 */

/**
 * Node in a hash chain (separate chaining)
 */
typedef struct ChainNode {
    char *key;
    int value;
    struct ChainNode *next;
} ChainNode;

/**
 * Chained hash table structure
 */
typedef struct {
    ChainNode **buckets;    /* Array of chain heads */
    int size;               /* Number of buckets */
    int count;              /* Number of entries */
} ChainedHashTable;

/* =============================================================================
 * PART 2: CHAINED HASH TABLE - OPERATIONS
 * =============================================================================
 */

/**
 * Create a new chained hash table
 */
ChainedHashTable *cht_create(int size) {
    ChainedHashTable *ht = malloc(sizeof(ChainedHashTable));
    if (ht == NULL) {
        fprintf(stderr, "Error: Failed to allocate hash table\n");
        return NULL;
    }
    
    ht->size = size;
    ht->count = 0;
    ht->buckets = calloc(size, sizeof(ChainNode *));
    
    if (ht->buckets == NULL) {
        fprintf(stderr, "Error: Failed to allocate buckets\n");
        free(ht);
        return NULL;
    }
    
    return ht;
}

/**
 * Calculate load factor
 */
float cht_load_factor(ChainedHashTable *ht) {
    return (float)ht->count / ht->size;
}

/**
 * Insert or update a key-value pair
 */
void cht_insert(ChainedHashTable *ht, const char *key, int value) {
    unsigned int index = hash_djb2(key) % ht->size;
    
    /* Check if key already exists */
    ChainNode *current = ht->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;  /* Update existing */
            return;
        }
        current = current->next;
    }
    
    /* Create new node and insert at head of chain */
    ChainNode *new_node = malloc(sizeof(ChainNode));
    new_node->key = strdup(key);
    new_node->value = value;
    new_node->next = ht->buckets[index];
    ht->buckets[index] = new_node;
    ht->count++;
}

/**
 * Search for a key, return pointer to value or NULL
 */
int *cht_search(ChainedHashTable *ht, const char *key) {
    unsigned int index = hash_djb2(key) % ht->size;
    
    ChainNode *current = ht->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return &current->value;
        }
        current = current->next;
    }
    
    return NULL;  /* Not found */
}

/**
 * Delete a key from the hash table
 */
bool cht_delete(ChainedHashTable *ht, const char *key) {
    unsigned int index = hash_djb2(key) % ht->size;
    
    ChainNode *current = ht->buckets[index];
    ChainNode *prev = NULL;
    
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            /* Unlink node */
            if (prev == NULL) {
                ht->buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }
            
            /* Free memory */
            free(current->key);
            free(current);
            ht->count--;
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    return false;  /* Key not found */
}

/**
 * Destroy the hash table and free all memory
 */
void cht_destroy(ChainedHashTable *ht) {
    for (int i = 0; i < ht->size; i++) {
        ChainNode *current = ht->buckets[i];
        while (current != NULL) {
            ChainNode *temp = current;
            current = current->next;
            free(temp->key);
            free(temp);
        }
    }
    free(ht->buckets);
    free(ht);
}

/**
 * Print hash table statistics
 */
void cht_print_stats(ChainedHashTable *ht) {
    int max_chain = 0;
    int empty_buckets = 0;
    int total_chain_length = 0;
    
    for (int i = 0; i < ht->size; i++) {
        int chain_length = 0;
        ChainNode *current = ht->buckets[i];
        
        while (current != NULL) {
            chain_length++;
            current = current->next;
        }
        
        if (chain_length == 0) empty_buckets++;
        if (chain_length > max_chain) max_chain = chain_length;
        total_chain_length += chain_length;
    }
    
    printf("  Table size:      %d buckets\n", ht->size);
    printf("  Entries:         %d\n", ht->count);
    printf("  Load factor:     %.2f\n", cht_load_factor(ht));
    printf("  Empty buckets:   %d (%.1f%%)\n", empty_buckets, 
           100.0 * empty_buckets / ht->size);
    printf("  Longest chain:   %d\n", max_chain);
    printf("  Avg chain len:   %.2f\n", 
           ht->count > 0 ? (float)total_chain_length / (ht->size - empty_buckets) : 0);
}

/**
 * Visualise the hash table structure
 */
void cht_visualise(ChainedHashTable *ht) {
    printf("\n  Hash Table Visualisation:\n");
    printf("  ┌─────┬──────────────────────────────────────────────┐\n");
    
    for (int i = 0; i < ht->size; i++) {
        printf("  │ %3d │", i);
        
        ChainNode *current = ht->buckets[i];
        if (current == NULL) {
            printf(" (empty)                                    │\n");
        } else {
            int first = 1;
            while (current != NULL) {
                if (!first) printf(" → ");
                printf(" [%s:%d]", current->key, current->value);
                first = 0;
                current = current->next;
            }
            printf("\n");
        }
    }
    
    printf("  └─────┴──────────────────────────────────────────────┘\n");
}

void demo_chained_hash_table(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: CHAINED HASH TABLE                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    ChainedHashTable *ht = cht_create(7);
    
    printf("Creating hash table with 7 buckets...\n\n");
    
    /* Insert entries */
    printf("Inserting entries:\n");
    const char *names[] = {"alice", "bob", "carol", "dave", "eve", "frank", "grace"};
    int scores[] = {95, 87, 92, 78, 88, 91, 85};
    
    for (int i = 0; i < 7; i++) {
        cht_insert(ht, names[i], scores[i]);
        printf("  Inserted: %s → %d (bucket %d)\n", 
               names[i], scores[i], hash_djb2(names[i]) % ht->size);
    }
    
    /* Display table */
    cht_visualise(ht);
    
    printf("\nStatistics:\n");
    cht_print_stats(ht);
    
    /* Search demonstration */
    printf("\nSearching for keys:\n");
    const char *search_keys[] = {"bob", "eve", "zoe"};
    for (int i = 0; i < 3; i++) {
        int *result = cht_search(ht, search_keys[i]);
        if (result != NULL) {
            printf("  Found: %s → %d\n", search_keys[i], *result);
        } else {
            printf("  Not found: %s\n", search_keys[i]);
        }
    }
    
    /* Update demonstration */
    printf("\nUpdating bob's score from 87 to 99:\n");
    cht_insert(ht, "bob", 99);
    int *bob_score = cht_search(ht, "bob");
    printf("  bob's new score: %d\n", *bob_score);
    
    /* Delete demonstration */
    printf("\nDeleting 'carol' from table:\n");
    bool deleted = cht_delete(ht, "carol");
    printf("  Deletion %s\n", deleted ? "successful" : "failed");
    
    cht_visualise(ht);
    
    /* Cleanup */
    cht_destroy(ht);
    printf("\n[✓] Hash table destroyed, all memory freed.\n");
}

/* =============================================================================
 * PART 3: OPEN ADDRESSING HASH TABLE - DATA STRUCTURES
 * =============================================================================
 */

/**
 * Entry states for open addressing
 */
typedef enum {
    ENTRY_EMPTY,
    ENTRY_OCCUPIED,
    ENTRY_DELETED  /* Tombstone */
} EntryState;

/**
 * Entry in open addressing table
 */
typedef struct {
    char *key;
    int value;
    EntryState state;
} OpenEntry;

/**
 * Open addressing hash table
 */
typedef struct {
    OpenEntry *entries;
    int size;
    int count;
    int tombstones;
    unsigned long total_probes;
    unsigned long total_operations;
} OpenHashTable;

/* =============================================================================
 * PART 3: OPEN ADDRESSING HASH TABLE - OPERATIONS
 * =============================================================================
 */

/**
 * Create open addressing hash table
 */
OpenHashTable *oht_create(int size) {
    OpenHashTable *ht = malloc(sizeof(OpenHashTable));
    ht->size = size;
    ht->count = 0;
    ht->tombstones = 0;
    ht->total_probes = 0;
    ht->total_operations = 0;
    ht->entries = calloc(size, sizeof(OpenEntry));
    
    for (int i = 0; i < size; i++) {
        ht->entries[i].state = ENTRY_EMPTY;
        ht->entries[i].key = NULL;
    }
    
    return ht;
}

/**
 * Calculate effective load factor (including tombstones)
 */
float oht_load_factor(OpenHashTable *ht) {
    return (float)(ht->count + ht->tombstones) / ht->size;
}

/**
 * Rehash to larger table
 */
void oht_rehash(OpenHashTable *ht) {
    int old_size = ht->size;
    OpenEntry *old_entries = ht->entries;
    
    /* Create new larger table */
    ht->size = old_size * 2;
    ht->entries = calloc(ht->size, sizeof(OpenEntry));
    ht->count = 0;
    ht->tombstones = 0;
    
    for (int i = 0; i < ht->size; i++) {
        ht->entries[i].state = ENTRY_EMPTY;
        ht->entries[i].key = NULL;
    }
    
    /* Reinsert all entries */
    for (int i = 0; i < old_size; i++) {
        if (old_entries[i].state == ENTRY_OCCUPIED) {
            /* Insert without triggering another rehash */
            unsigned int index = hash_djb2(old_entries[i].key) % ht->size;
            unsigned int step = hash_secondary(old_entries[i].key, ht->size);
            
            while (ht->entries[index].state == ENTRY_OCCUPIED) {
                index = (index + step) % ht->size;
            }
            
            ht->entries[index].key = old_entries[i].key;
            ht->entries[index].value = old_entries[i].value;
            ht->entries[index].state = ENTRY_OCCUPIED;
            ht->count++;
        }
    }
    
    free(old_entries);
    printf("  [Rehashed: %d → %d buckets]\n", old_size, ht->size);
}

/**
 * Insert with double hashing
 */
int oht_insert(OpenHashTable *ht, const char *key, int value) {
    /* Check load factor and rehash if needed */
    if (oht_load_factor(ht) > LOAD_FACTOR_THRESHOLD) {
        oht_rehash(ht);
    }
    
    unsigned int index = hash_djb2(key) % ht->size;
    unsigned int step = hash_secondary(key, ht->size);
    int probes = 0;
    int first_tombstone = -1;
    
    ht->total_operations++;
    
    while (ht->entries[index].state != ENTRY_EMPTY) {
        probes++;
        ht->total_probes++;
        
        /* Remember first tombstone for potential insertion */
        if (ht->entries[index].state == ENTRY_DELETED && first_tombstone == -1) {
            first_tombstone = index;
        }
        
        /* Check if key already exists */
        if (ht->entries[index].state == ENTRY_OCCUPIED &&
            strcmp(ht->entries[index].key, key) == 0) {
            ht->entries[index].value = value;
            return probes;
        }
        
        index = (index + step) % ht->size;
    }
    
    /* Use tombstone slot if found, otherwise use empty slot */
    if (first_tombstone != -1) {
        index = first_tombstone;
        ht->tombstones--;
    }
    
    /* Insert new entry */
    ht->entries[index].key = strdup(key);
    ht->entries[index].value = value;
    ht->entries[index].state = ENTRY_OCCUPIED;
    ht->count++;
    
    return probes;
}

/**
 * Search with double hashing
 */
int *oht_search(OpenHashTable *ht, const char *key, int *probes) {
    unsigned int index = hash_djb2(key) % ht->size;
    unsigned int step = hash_secondary(key, ht->size);
    *probes = 0;
    
    while (ht->entries[index].state != ENTRY_EMPTY) {
        (*probes)++;
        
        if (ht->entries[index].state == ENTRY_OCCUPIED &&
            strcmp(ht->entries[index].key, key) == 0) {
            return &ht->entries[index].value;
        }
        
        index = (index + step) % ht->size;
    }
    
    return NULL;
}

/**
 * Delete using tombstone
 */
bool oht_delete(OpenHashTable *ht, const char *key) {
    unsigned int index = hash_djb2(key) % ht->size;
    unsigned int step = hash_secondary(key, ht->size);
    
    while (ht->entries[index].state != ENTRY_EMPTY) {
        if (ht->entries[index].state == ENTRY_OCCUPIED &&
            strcmp(ht->entries[index].key, key) == 0) {
            free(ht->entries[index].key);
            ht->entries[index].key = NULL;
            ht->entries[index].state = ENTRY_DELETED;
            ht->count--;
            ht->tombstones++;
            return true;
        }
        
        index = (index + step) % ht->size;
    }
    
    return false;
}

/**
 * Destroy open addressing hash table
 */
void oht_destroy(OpenHashTable *ht) {
    for (int i = 0; i < ht->size; i++) {
        if (ht->entries[i].state == ENTRY_OCCUPIED) {
            free(ht->entries[i].key);
        }
    }
    free(ht->entries);
    free(ht);
}

/**
 * Print statistics
 */
void oht_print_stats(OpenHashTable *ht) {
    printf("  Table size:      %d slots\n", ht->size);
    printf("  Entries:         %d\n", ht->count);
    printf("  Tombstones:      %d\n", ht->tombstones);
    printf("  Load factor:     %.2f (effective: %.2f)\n", 
           (float)ht->count / ht->size, oht_load_factor(ht));
    printf("  Total probes:    %lu\n", ht->total_probes);
    printf("  Avg probes/op:   %.2f\n", 
           ht->total_operations > 0 ? 
           (float)ht->total_probes / ht->total_operations : 0);
}

/**
 * Visualise open addressing table
 */
void oht_visualise(OpenHashTable *ht) {
    printf("\n  Open Addressing Table:\n");
    printf("  ┌─────┬────────────────────────┐\n");
    
    for (int i = 0; i < ht->size; i++) {
        printf("  │ %3d │", i);
        
        switch (ht->entries[i].state) {
            case ENTRY_EMPTY:
                printf(" (empty)               │\n");
                break;
            case ENTRY_DELETED:
                printf(" [TOMBSTONE]           │\n");
                break;
            case ENTRY_OCCUPIED:
                printf(" %-10s → %4d     │\n", 
                       ht->entries[i].key, ht->entries[i].value);
                break;
        }
    }
    
    printf("  └─────┴────────────────────────┘\n");
}

void demo_open_addressing(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: OPEN ADDRESSING WITH DOUBLE HASHING              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    OpenHashTable *ht = oht_create(11);
    
    printf("Creating open addressing table with 11 slots...\n\n");
    
    /* Insert entries */
    printf("Inserting entries:\n");
    const char *names[] = {"alice", "bob", "carol", "dave", "eve"};
    int values[] = {100, 200, 300, 400, 500};
    
    for (int i = 0; i < 5; i++) {
        int probes = oht_insert(ht, names[i], values[i]);
        printf("  Inserted: %s → %d (%d probes)\n", names[i], values[i], probes);
    }
    
    oht_visualise(ht);
    
    printf("\nStatistics after insertions:\n");
    oht_print_stats(ht);
    
    /* Search demonstration */
    printf("\nSearching:\n");
    const char *search_keys[] = {"carol", "zoe"};
    for (int i = 0; i < 2; i++) {
        int probes;
        int *result = oht_search(ht, search_keys[i], &probes);
        if (result != NULL) {
            printf("  Found: %s → %d (%d probes)\n", search_keys[i], *result, probes);
        } else {
            printf("  Not found: %s (%d probes)\n", search_keys[i], probes);
        }
    }
    
    /* Delete demonstration (tombstones) */
    printf("\nDeleting 'bob' (creates tombstone):\n");
    oht_delete(ht, "bob");
    oht_visualise(ht);
    
    printf("\nInserting 'hank' (may reuse tombstone):\n");
    int probes = oht_insert(ht, "hank", 600);
    printf("  Inserted: hank → 600 (%d probes)\n", probes);
    oht_visualise(ht);
    
    /* Trigger rehash */
    printf("\nTriggering rehash by inserting more entries:\n");
    oht_insert(ht, "ivan", 700);
    oht_insert(ht, "judy", 800);
    oht_insert(ht, "karl", 900);
    
    printf("\nFinal statistics:\n");
    oht_print_stats(ht);
    
    oht_destroy(ht);
    printf("\n[✓] Hash table destroyed, all memory freed.\n");
}

/* =============================================================================
 * PART 4: LOAD FACTOR ANALYSIS
 * =============================================================================
 */

void demo_load_factor_analysis(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: LOAD FACTOR ANALYSIS                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Demonstrating impact of load factor on probe count...\n\n");
    
    int table_size = 100;
    OpenHashTable *ht = oht_create(table_size);
    
    printf("  %-15s  %-15s  %-20s\n", "Load Factor", "Entries", "Avg Probes/Insert");
    printf("  %-15s  %-15s  %-20s\n", "-----------", "-------", "-----------------");
    
    char key_buffer[32];
    unsigned long prev_probes = 0;
    unsigned long prev_ops = 0;
    
    for (int i = 0; i < 95; i++) {
        sprintf(key_buffer, "key_%04d", i);
        oht_insert(ht, key_buffer, i * 10);
        
        /* Report at certain intervals */
        if ((i + 1) % 10 == 0 || i >= 89) {
            float load = (float)ht->count / table_size;
            unsigned long interval_probes = ht->total_probes - prev_probes;
            unsigned long interval_ops = ht->total_operations - prev_ops;
            float avg_probes = interval_ops > 0 ? 
                              (float)interval_probes / interval_ops : 0;
            
            printf("  %-15.2f  %-15d  %-20.2f\n", load, ht->count, avg_probes);
            
            prev_probes = ht->total_probes;
            prev_ops = ht->total_operations;
        }
    }
    
    printf("\n[!] Notice how probe count increases dramatically as load factor\n");
    printf("    approaches 1.0. This is why we rehash at α ≈ 0.7!\n");
    
    oht_destroy(ht);
}

/* =============================================================================
 * PART 5: WORD FREQUENCY COUNTER (PRACTICAL APPLICATION)
 * =============================================================================
 */

void demo_word_frequency(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: WORD FREQUENCY COUNTER                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    const char *sample_text = 
        "Hash tables are fundamental data structures in computer science. "
        "A hash table uses a hash function to compute an index into an array "
        "of buckets from which the desired value can be found. Hash tables "
        "provide average constant time complexity for search insert and delete "
        "operations. The efficiency of a hash table depends on the quality of "
        "the hash function and the collision resolution strategy used. Hash "
        "tables are used in compilers databases caches and many other applications.";
    
    printf("Sample text:\n  \"%s\"\n\n", sample_text);
    
    ChainedHashTable *freq = cht_create(31);
    
    /* Parse and count words */
    char *text_copy = strdup(sample_text);
    char *word = strtok(text_copy, " .,");
    
    while (word != NULL) {
        /* Convert to lowercase */
        for (char *p = word; *p; p++) {
            *p = tolower(*p);
        }
        
        int *count = cht_search(freq, word);
        if (count == NULL) {
            cht_insert(freq, word, 1);
        } else {
            (*count)++;
        }
        
        word = strtok(NULL, " .,");
    }
    
    free(text_copy);
    
    printf("Word frequency results:\n");
    printf("  %-15s  %s\n", "Word", "Count");
    printf("  %-15s  %s\n", "----", "-----");
    
    /* Print all words and frequencies */
    for (int i = 0; i < freq->size; i++) {
        ChainNode *current = freq->buckets[i];
        while (current != NULL) {
            if (current->value > 1) {  /* Only show words appearing more than once */
                printf("  %-15s  %d\n", current->key, current->value);
            }
            current = current->next;
        }
    }
    
    printf("\nTable statistics:\n");
    cht_print_stats(freq);
    
    cht_destroy(freq);
}

/* =============================================================================
 * PART 6: PERFORMANCE COMPARISON
 * =============================================================================
 */

void demo_performance_comparison(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: CHAINING VS OPEN ADDRESSING                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int num_operations = 1000;
    int table_size = 500;
    
    printf("Inserting %d random entries into each table type...\n\n", num_operations);
    
    ChainedHashTable *chain_ht = cht_create(table_size);
    OpenHashTable *open_ht = oht_create(table_size);
    
    char key_buffer[32];
    clock_t start, end;
    
    /* Chaining insertions */
    start = clock();
    for (int i = 0; i < num_operations; i++) {
        sprintf(key_buffer, "item_%06d", rand() % 10000);
        cht_insert(chain_ht, key_buffer, i);
    }
    end = clock();
    double chain_time = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    
    /* Open addressing insertions */
    start = clock();
    for (int i = 0; i < num_operations; i++) {
        sprintf(key_buffer, "item_%06d", rand() % 10000);
        oht_insert(open_ht, key_buffer, i);
    }
    end = clock();
    double open_time = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    
    printf("Results:\n\n");
    
    printf("  CHAINING:\n");
    printf("  ---------\n");
    cht_print_stats(chain_ht);
    printf("  Time: %.3f ms\n\n", chain_time);
    
    printf("  OPEN ADDRESSING:\n");
    printf("  ----------------\n");
    oht_print_stats(open_ht);
    printf("  Time: %.3f ms\n", open_time);
    
    cht_destroy(chain_ht);
    oht_destroy(open_ht);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     WEEK 11: HASH TABLES - Complete Example                   ║\n");
    printf("║     Algorithms and Programming Techniques                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    srand(time(NULL));
    
    demo_hash_functions();
    demo_chained_hash_table();
    demo_open_addressing();
    demo_load_factor_analysis();
    demo_word_frequency();
    demo_performance_comparison();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     DEMONSTRATION COMPLETE                                    ║\n");
    printf("║     All memory has been properly freed.                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

/* =============================================================================
 * END OF FILE
 * =============================================================================
 */

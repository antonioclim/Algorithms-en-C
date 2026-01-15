/**
 * =============================================================================
 * EXERCISE 2: Open Addressing Hash Table - Word Frequency Counter
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement a hash table using open addressing with double hashing to
 *   count word frequencies in a text file. The system must handle collisions
 *   using double hashing, track probe counts and automatically rehash when
 *   the load factor exceeds 0.7.
 *
 * REQUIREMENTS:
 *   1. Implement the FNV-1a hash function (primary hash)
 *   2. Implement a secondary hash function for double hashing
 *   3. Create an open addressing table with tombstone-based deletion
 *   4. Track probe counts for performance analysis
 *   5. Implement automatic rehashing at load factor > 0.7
 *   6. Process a text file and count word occurrences
 *   7. Display top N most frequent words
 *   8. Show detailed statistics about table performance
 *
 * EXAMPLE INPUT (text_sample.txt):
 *   The quick brown fox jumps over the lazy dog.
 *   The dog barks at the fox.
 *
 * EXPECTED OUTPUT:
 *   Hash Table Statistics:
 *     Table size: 16
 *     Entries: 10
 *     Load factor: 0.63
 *     Total probes: 15
 *     Average probes per insert: 1.50
 *
 *   Top 5 Words:
 *     1. the      (3 occurrences)
 *     2. dog      (2 occurrences)
 *     3. fox      (2 occurrences)
 *     ...
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

#define INITIAL_SIZE 11          /* Initial table size (prime number) */
#define LOAD_FACTOR_MAX 0.7      /* Rehash threshold */
#define MAX_WORD_LEN 64          /* Maximum word length */

#define FNV_OFFSET_BASIS 2166136261u
#define FNV_PRIME 16777619u

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * Entry states for open addressing
 */
typedef enum {
    SLOT_EMPTY,     /* Never used */
    SLOT_OCCUPIED,  /* Currently holds data */
    SLOT_DELETED    /* Tombstone - was deleted */
} SlotState;

/**
 * TODO 1: Define the Entry structure for word counting
 *
 * Each entry should contain:
 *   - word: the word string (use char pointer, allocate with strdup)
 *   - count: number of occurrences (integer)
 *   - state: the slot state (use SlotState enum)
 *
 * Hint: The state field is crucial for open addressing deletion
 */
typedef struct {
    /* YOUR CODE HERE */
    char *word;
    int count;
    SlotState state;
} Entry;

/**
 * TODO 2: Define the OpenHashTable structure
 *
 * The hash table should contain:
 *   - entries: array of Entry structures
 *   - size: current table size
 *   - count: number of occupied entries (excluding tombstones)
 *   - tombstones: number of deleted entries
 *   - total_probes: cumulative probe count (for statistics)
 *   - total_inserts: number of insert operations (for statistics)
 *
 * Hint: Track tombstones separately for accurate load factor calculation
 */
typedef struct {
    /* YOUR CODE HERE */
    Entry *entries;
    int size;
    int count;
    int tombstones;
    unsigned long total_probes;
    unsigned long total_inserts;
} OpenHashTable;

/* =============================================================================
 * HASH FUNCTIONS
 * =============================================================================
 */

/**
 * TODO 3: Implement the FNV-1a hash function (primary hash)
 *
 * The FNV-1a algorithm:
 *   1. Start with hash = FNV_OFFSET_BASIS (2166136261)
 *   2. For each byte in the key:
 *      a. XOR the hash with the byte: hash = hash ^ byte
 *      b. Multiply by FNV_PRIME: hash = hash * 16777619
 *   3. Return the final hash value
 *
 * @param key The string to hash
 * @return The hash value (unsigned int)
 *
 * Hint: Process each character as an unsigned char
 */
unsigned int hash_fnv1a(const char *key) {
    /* YOUR CODE HERE */
    unsigned int hash = FNV_OFFSET_BASIS;
    
    /* Implement FNV-1a algorithm here */
    
    return hash;  /* Replace with correct implementation */
}

/**
 * TODO 4: Implement the secondary hash function for double hashing
 *
 * Requirements for secondary hash in double hashing:
 *   - Must NEVER return 0 (would cause infinite loop)
 *   - Should return a value coprime to table_size
 *   - For prime table sizes, any value 1 to size-1 works
 *   - For power-of-2 sizes, return an ODD value
 *
 * Suggested approach:
 *   1. Compute a hash using a different method (e.g., djb2)
 *   2. Take modulo (table_size - 1)
 *   3. Ensure the result is at least 1
 *
 * @param key The string to hash
 * @param table_size Current size of the table
 * @return Step size for probing (must be >= 1)
 *
 * Hint: result = (hash % (table_size - 1)) + 1 ensures range [1, table_size-1]
 */
unsigned int hash_secondary(const char *key, int table_size) {
    /* YOUR CODE HERE */
    
    return 1;  /* Replace with correct implementation */
}

/* =============================================================================
 * HASH TABLE OPERATIONS
 * =============================================================================
 */

/**
 * TODO 5: Implement hash table creation
 *
 * Steps:
 *   1. Allocate memory for the OpenHashTable structure
 *   2. Allocate memory for the entries array (size Entry structures)
 *   3. Initialise all entries:
 *      - word = NULL
 *      - count = 0
 *      - state = SLOT_EMPTY
 *   4. Set size, count, tombstones, total_probes, total_inserts to 0/initial
 *   5. Return the created table
 *
 * @param size The initial number of slots
 * @return Pointer to created table, or NULL on failure
 */
OpenHashTable *oht_create(int size) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace with correct implementation */
}

/**
 * Calculate effective load factor (including tombstones)
 */
float oht_load_factor(OpenHashTable *ht) {
    if (ht == NULL || ht->size == 0) return 0.0f;
    return (float)(ht->count + ht->tombstones) / ht->size;
}

/**
 * TODO 6: Implement the rehash operation
 *
 * Steps:
 *   1. Save the old entries array and old size
 *   2. Create a new entries array with double the size
 *   3. Reset count and tombstones to 0
 *   4. For each OCCUPIED entry in the old array:
 *      - Reinsert into the new array (recalculate index)
 *   5. Free the old entries array (but not the word strings!)
 *
 * @param ht Pointer to the hash table
 *
 * Hint: Don't free the word strings - they're reused in new positions
 */
void oht_rehash(OpenHashTable *ht) {
    if (ht == NULL) return;
    
    printf("  [Rehashing: %d -> %d slots]\n", ht->size, ht->size * 2);
    
    /* YOUR CODE HERE */
}

/**
 * TODO 7: Implement the insert/update operation with double hashing
 *
 * Steps:
 *   1. Check load factor; if > LOAD_FACTOR_MAX, call oht_rehash()
 *   2. Calculate primary index: hash_fnv1a(word) % size
 *   3. Calculate step size: hash_secondary(word, size)
 *   4. Probe until finding:
 *      a. An EMPTY slot (word not in table - insert new)
 *      b. An OCCUPIED slot with matching word (increment count)
 *      c. A DELETED slot - remember position but keep probing
 *   5. If inserting new:
 *      - Use first DELETED slot found, or EMPTY slot
 *      - Set word = strdup(word), count = 1, state = OCCUPIED
 *   6. Update statistics (total_probes, total_inserts)
 *   7. Return the number of probes used
 *
 * @param ht Pointer to the hash table
 * @param word The word to insert/update
 * @return Number of probes used for this operation
 *
 * Hint: Remember first tombstone position but don't stop there -
 *       the word might exist later in the probe sequence!
 */
int oht_insert(OpenHashTable *ht, const char *word) {
    if (ht == NULL || word == NULL) return 0;
    
    /* Check load factor and rehash if needed */
    if (oht_load_factor(ht) > LOAD_FACTOR_MAX) {
        oht_rehash(ht);
    }
    
    /* YOUR CODE HERE */
    
    return 0;  /* Replace with probe count */
}

/**
 * TODO 8: Implement the search operation
 *
 * Steps:
 *   1. Calculate primary index and step size
 *   2. Probe until finding:
 *      a. An EMPTY slot -> word not found, return NULL
 *      b. An OCCUPIED slot with matching word -> return pointer to Entry
 *      c. A DELETED slot -> continue probing
 *   3. Track probe count in the probes output parameter
 *
 * @param ht Pointer to the hash table
 * @param word The word to search for
 * @param probes Output parameter for number of probes used
 * @return Pointer to Entry if found, NULL otherwise
 */
Entry *oht_search(OpenHashTable *ht, const char *word, int *probes) {
    if (ht == NULL || word == NULL) {
        if (probes) *probes = 0;
        return NULL;
    }
    
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace with correct implementation */
}

/**
 * TODO 9: Implement the delete operation using tombstones
 *
 * Steps:
 *   1. Calculate primary index and step size
 *   2. Probe until finding:
 *      a. An EMPTY slot -> word not found, return false
 *      b. An OCCUPIED slot with matching word:
 *         - Free the word string
 *         - Set state to SLOT_DELETED (tombstone)
 *         - Decrement count, increment tombstones
 *         - Return true
 *      c. A DELETED slot -> continue probing
 *
 * @param ht Pointer to the hash table
 * @param word The word to delete
 * @return true if deleted, false if not found
 *
 * Hint: We mark as DELETED rather than EMPTY to preserve probe chains
 */
bool oht_delete(OpenHashTable *ht, const char *word) {
    if (ht == NULL || word == NULL) return false;
    
    /* YOUR CODE HERE */
    
    return false;  /* Replace with correct implementation */
}

/**
 * Destroy the hash table and free all memory
 */
void oht_destroy(OpenHashTable *ht) {
    if (ht == NULL) return;
    
    for (int i = 0; i < ht->size; i++) {
        if (ht->entries[i].state == SLOT_OCCUPIED) {
            free(ht->entries[i].word);
        }
    }
    
    free(ht->entries);
    free(ht);
}

/* =============================================================================
 * STATISTICS AND DISPLAY
 * =============================================================================
 */

/**
 * Print hash table statistics
 */
void oht_print_stats(OpenHashTable *ht) {
    if (ht == NULL) return;
    
    printf("\nHash Table Statistics:\n");
    printf("  Table size:       %d slots\n", ht->size);
    printf("  Entries:          %d\n", ht->count);
    printf("  Tombstones:       %d\n", ht->tombstones);
    printf("  Load factor:      %.2f\n", (float)ht->count / ht->size);
    printf("  Effective LF:     %.2f (including tombstones)\n", 
           oht_load_factor(ht));
    printf("  Total probes:     %lu\n", ht->total_probes);
    printf("  Total inserts:    %lu\n", ht->total_inserts);
    if (ht->total_inserts > 0) {
        printf("  Avg probes/op:    %.2f\n", 
               (float)ht->total_probes / ht->total_inserts);
    }
}

/**
 * TODO 10: Find and display top N most frequent words
 *
 * Steps:
 *   1. Create an array of pointers to all OCCUPIED entries
 *   2. Sort this array by count (descending order)
 *   3. Print the top N entries
 *
 * @param ht Pointer to the hash table
 * @param n Number of top words to display
 *
 * Hint: Use qsort() with a comparison function, or implement simple
 *       selection sort for the top N elements
 */
void oht_top_n_words(OpenHashTable *ht, int n) {
    if (ht == NULL || n <= 0) return;
    
    printf("\nTop %d Words:\n", n);
    
    /* YOUR CODE HERE */
    
    /* Simple approach: for each position 1..n, find the max */
    /* More elegant: collect all entries and qsort */
}

/**
 * Visualise the hash table (for small tables)
 */
void oht_visualise(OpenHashTable *ht) {
    if (ht == NULL) return;
    
    printf("\nHash Table Contents:\n");
    printf("┌─────┬────────────────────────────────────┐\n");
    
    for (int i = 0; i < ht->size && i < 20; i++) {
        printf("│ %3d │", i);
        
        switch (ht->entries[i].state) {
            case SLOT_EMPTY:
                printf(" (empty)                           │\n");
                break;
            case SLOT_DELETED:
                printf(" [TOMBSTONE]                       │\n");
                break;
            case SLOT_OCCUPIED:
                printf(" %-20s  count: %4d  │\n",
                       ht->entries[i].word, ht->entries[i].count);
                break;
        }
    }
    
    if (ht->size > 20) {
        printf("│ ... │ (%d more entries)                  │\n", 
               ht->size - 20);
    }
    
    printf("└─────┴────────────────────────────────────┘\n");
}

/* =============================================================================
 * TEXT PROCESSING
 * =============================================================================
 */

/**
 * TODO 11: Process a text file and count word frequencies
 *
 * Steps:
 *   1. Open the file for reading
 *   2. Read the file character by character
 *   3. Build words from alphabetic characters (convert to lowercase)
 *   4. When a non-alphabetic character is encountered:
 *      - If we have a word, insert it into the hash table
 *      - Reset the word buffer
 *   5. Close the file
 *   6. Return the total number of words processed
 *
 * @param ht Pointer to the hash table
 * @param filename Path to the text file
 * @return Number of words processed, or -1 on error
 *
 * Hint: Use isalpha() and tolower() from <ctype.h>
 */
int process_text_file(OpenHashTable *ht, const char *filename) {
    if (ht == NULL || filename == NULL) return -1;
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return -1;
    }
    
    /* YOUR CODE HERE */
    
    fclose(file);
    return 0;  /* Replace with word count */
}

/**
 * Process a string and count word frequencies
 */
int process_text_string(OpenHashTable *ht, const char *text) {
    if (ht == NULL || text == NULL) return 0;
    
    char word_buffer[MAX_WORD_LEN];
    int word_len = 0;
    int total_words = 0;
    
    for (const char *p = text; ; p++) {
        if (isalpha((unsigned char)*p)) {
            /* Add to current word (lowercase) */
            if (word_len < MAX_WORD_LEN - 1) {
                word_buffer[word_len++] = tolower((unsigned char)*p);
            }
        } else {
            /* End of word */
            if (word_len > 0) {
                word_buffer[word_len] = '\0';
                oht_insert(ht, word_buffer);
                total_words++;
                word_len = 0;
            }
            
            if (*p == '\0') break;
        }
    }
    
    return total_words;
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 2: Open Addressing Word Frequency Counter        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Create hash table */
    OpenHashTable *ht = oht_create(INITIAL_SIZE);
    if (ht == NULL) {
        fprintf(stderr, "Error: Failed to create hash table\n");
        return 1;
    }
    
    int words_processed = 0;
    
    /* Process file or use sample text */
    if (argc > 1) {
        printf("\nProcessing file: %s\n", argv[1]);
        words_processed = process_text_file(ht, argv[1]);
    } else {
        /* Try default file location */
        words_processed = process_text_file(ht, "data/text_sample.txt");
    }
    
    if (words_processed < 0) {
        printf("\nUsing sample text instead...\n");
        
        const char *sample_text = 
            "Hash tables are fundamental data structures in computer science. "
            "A hash table uses a hash function to compute an index into an array "
            "of buckets from which the desired value can be found. Hash tables "
            "provide average constant time complexity for search insert and delete "
            "operations. The efficiency of a hash table depends on the quality of "
            "the hash function and the collision resolution strategy. Hash tables "
            "are widely used in compilers databases caches and many applications. "
            "The load factor of a hash table is the ratio of entries to buckets. "
            "When the load factor becomes too high the table should be resized. "
            "This process is called rehashing and involves creating a larger table "
            "and reinserting all existing entries.";
        
        words_processed = process_text_string(ht, sample_text);
    }
    
    printf("\nProcessed %d words.\n", words_processed);
    
    /* Display statistics */
    oht_print_stats(ht);
    
    /* Display top words */
    oht_top_n_words(ht, 10);
    
    /* Search demonstration */
    printf("\n--- Search Demonstration ---\n");
    const char *search_words[] = {"hash", "table", "function", "xyz"};
    int num_searches = 4;
    
    for (int i = 0; i < num_searches; i++) {
        int probes;
        Entry *result = oht_search(ht, search_words[i], &probes);
        if (result != NULL) {
            printf("  '%s': %d occurrences (%d probes)\n",
                   search_words[i], result->count, probes);
        } else {
            printf("  '%s': not found (%d probes)\n",
                   search_words[i], probes);
        }
    }
    
    /* Delete demonstration */
    printf("\n--- Delete Demonstration ---\n");
    printf("Deleting 'the'...\n");
    bool deleted = oht_delete(ht, "the");
    printf("  Deletion %s\n", deleted ? "successful" : "failed");
    
    printf("\nSearching for 'the' after deletion:\n");
    int probes;
    Entry *result = oht_search(ht, "the", &probes);
    printf("  'the': %s (%d probes)\n",
           result ? "still found (error!)" : "correctly not found", probes);
    
    /* Visualise (if table is small) */
    if (ht->size <= 30) {
        oht_visualise(ht);
    }
    
    /* Cleanup */
    oht_destroy(ht);
    printf("\n[✓] Hash table destroyed. All memory freed.\n\n");
    
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Implement Robin Hood hashing:
 *    - When inserting, if the new key has probed further than the existing
 *      key at that slot, swap them and continue inserting the displaced key.
 *    - This reduces variance in probe lengths.
 *
 * 2. Track and display probe length distribution:
 *    - How many words required 0 probes? 1 probe? 2 probes? etc.
 *    - Visualise as a histogram.
 *
 * 3. Compare FNV-1a with djb2:
 *    - Run both hash functions on the same input
 *    - Compare collision rates and probe counts
 *
 * 4. Implement lazy deletion cleanup:
 *    - When too many tombstones accumulate, perform a cleanup pass
 *    - Rehash all entries to eliminate tombstones
 *
 * 5. Add support for phrase counting (n-grams):
 *    - Count bigrams (two-word sequences) or trigrams
 *    - Example: "hash table" as a single key
 *
 * =============================================================================
 */

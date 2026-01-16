/**
 * =============================================================================
 * EXERCISE 2: Word Frequency Counter (Open Addressing)
 * =============================================================================
 *
 * OVERVIEW
 *   This programme implements a hash table using open addressing with double
 *   hashing. It consumes a text file, tokenises it into alphabetic words,
 *   normalises case and counts word frequencies.
 *
 *   The implementation emphasises:
 *     - FNV-1a as the primary hash
 *     - A secondary hash (djb2-derived) to generate probe steps
 *     - Tombstones to support deletion without breaking probe chains
 *     - A rehash mechanism triggered by an effective load factor threshold
 *     - Probe-count instrumentation to enable empirical performance analysis
 *
 * NOTATION
 *   Table size: m
 *   Occupied entries: n
 *   Tombstones: d
 *   Effective load factor: (n + d) / m
 *
 * COMPLEXITY
 *   Under standard simple-uniform-hashing assumptions, expected cost for
 *   successful search and insertion is O(1) for moderate load factors. In the
 *   worst case (adversarial or degenerate probing), costs degrade to O(m).
 *
 * BUILD
 *   gcc -Wall -Wextra -std=c11 -O2 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/* =============================================================================
 * CONFIGURATION
 * =============================================================================
 */

#define INITIAL_SIZE 127         /* Prime: good distribution for modular hashing */
#define LOAD_FACTOR_MAX 0.70f
#define MAX_WORD_LEN 64

#define FNV_OFFSET_BASIS 2166136261u
#define FNV_PRIME 16777619u

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef enum {
    SLOT_EMPTY,
    SLOT_OCCUPIED,
    SLOT_DELETED
} SlotState;

typedef struct {
    char *word;
    int count;
    SlotState state;
} Entry;

typedef struct {
    Entry *entries;
    int size;
    int count;
    int tombstones;

    /* Instrumentation */
    unsigned long total_probes;   /* total slot inspections across all operations */
    unsigned long total_ops;      /* number of insert/update operations */
} OpenHashTable;

typedef struct {
    char *word;
    int count;
} WordFreq;

/* =============================================================================
 * SMALL UTILITIES
 * =============================================================================
 */

static char *xstrdup(const char *s) {
    if (s == NULL) return NULL;
    size_t n = strlen(s) + 1u;
    char *p = (char *)malloc(n);
    if (p == NULL) return NULL;
    memcpy(p, s, n);
    return p;
}

static int is_prime_int(int x) {
    if (x <= 1) return 0;
    if (x <= 3) return 1;
    if (x % 2 == 0 || x % 3 == 0) return 0;
    for (int i = 5; (long long)i * i <= x; i += 6) {
        if (x % i == 0 || x % (i + 2) == 0) return 0;
    }
    return 1;
}

static int next_prime_int(int x) {
    if (x <= 2) return 2;
    if (x % 2 == 0) x++;
    while (!is_prime_int(x)) {
        x += 2;
    }
    return x;
}

/* =============================================================================
 * HASH FUNCTIONS
 * =============================================================================
 */

static unsigned int hash_fnv1a(const char *key) {
    unsigned int hash = FNV_OFFSET_BASIS;

    while (*key) {
        hash ^= (unsigned char)*key++;
        hash *= FNV_PRIME;
    }

    return hash;
}

static unsigned int hash_secondary(const char *key, int table_size) {
    /* djb2-derived step generator */
    unsigned int hash = 5381u;
    int c;

    while ((c = (unsigned char)*key++) != 0) {
        hash = ((hash << 5) + hash) + (unsigned int)c;
    }

    /* Step must be in [1, table_size - 1]. */
    if (table_size <= 1) return 1u;
    return (hash % (unsigned int)(table_size - 1)) + 1u;
}

/* =============================================================================
 * HASH TABLE CORE
 * =============================================================================
 */

static OpenHashTable *oht_create(int size) {
    if (size <= 0) return NULL;

    OpenHashTable *ht = (OpenHashTable *)malloc(sizeof(*ht));
    if (ht == NULL) return NULL;

    ht->size = size;
    ht->count = 0;
    ht->tombstones = 0;
    ht->total_probes = 0;
    ht->total_ops = 0;

    ht->entries = (Entry *)calloc((size_t)size, sizeof(Entry));
    if (ht->entries == NULL) {
        free(ht);
        return NULL;
    }

    for (int i = 0; i < size; i++) {
        ht->entries[i].word = NULL;
        ht->entries[i].count = 0;
        ht->entries[i].state = SLOT_EMPTY;
    }

    return ht;
}

static float oht_effective_load_factor(const OpenHashTable *ht) {
    if (ht == NULL || ht->size <= 0) return 0.0f;
    return (float)(ht->count + ht->tombstones) / (float)ht->size;
}

static void oht_rehash(OpenHashTable *ht) {
    if (ht == NULL) return;

    Entry *old_entries = ht->entries;
    int old_size = ht->size;

    /* Use a prime size to keep the modular arithmetic well behaved. */
    int new_size = next_prime_int(old_size * 2);
    Entry *new_entries = (Entry *)calloc((size_t)new_size, sizeof(Entry));
    if (new_entries == NULL) {
        /* If rehash fails, keep operating with the old table. */
        return;
    }

    for (int i = 0; i < new_size; i++) {
        new_entries[i].word = NULL;
        new_entries[i].count = 0;
        new_entries[i].state = SLOT_EMPTY;
    }

    ht->entries = new_entries;
    ht->size = new_size;
    ht->count = 0;
    ht->tombstones = 0;

    /* Transfer occupied entries without allocating new strings. */
    for (int i = 0; i < old_size; i++) {
        if (old_entries[i].state != SLOT_OCCUPIED) continue;

        unsigned int index = hash_fnv1a(old_entries[i].word) % (unsigned int)ht->size;
        unsigned int step = hash_secondary(old_entries[i].word, ht->size);

        while (ht->entries[index].state == SLOT_OCCUPIED) {
            index = (index + step) % (unsigned int)ht->size;
        }

        ht->entries[index].word = old_entries[i].word;
        ht->entries[index].count = old_entries[i].count;
        ht->entries[index].state = SLOT_OCCUPIED;
        ht->count++;
    }

    free(old_entries);
}

static int oht_insert(OpenHashTable *ht, const char *word) {
    if (ht == NULL || word == NULL || *word == '\0') return 0;

    if (oht_effective_load_factor(ht) > LOAD_FACTOR_MAX) {
        oht_rehash(ht);
    }

    unsigned int index = hash_fnv1a(word) % (unsigned int)ht->size;
    unsigned int step = hash_secondary(word, ht->size);

    int first_tombstone = -1;
    int probes = 0;

    for (;;) {
        probes++;
        Entry *slot = &ht->entries[index];

        if (slot->state == SLOT_EMPTY) {
            unsigned int insert_index = (first_tombstone >= 0) ? (unsigned int)first_tombstone : index;
            Entry *ins = &ht->entries[insert_index];

            if (ins->state == SLOT_DELETED) {
                ht->tombstones--;
            }

            ins->word = xstrdup(word);
            if (ins->word == NULL) {
                /* Allocation failure: return probes so callers can still record cost. */
                ht->total_probes += (unsigned long)probes;
                ht->total_ops++;
                return probes;
            }

            ins->count = 1;
            ins->state = SLOT_OCCUPIED;
            ht->count++;

            ht->total_probes += (unsigned long)probes;
            ht->total_ops++;
            return probes;
        }

        if (slot->state == SLOT_DELETED) {
            if (first_tombstone < 0) {
                first_tombstone = (int)index;
            }
        } else {
            /* OCCUPIED */
            if (strcmp(slot->word, word) == 0) {
                slot->count++;
                ht->total_probes += (unsigned long)probes;
                ht->total_ops++;
                return probes;
            }
        }

        index = (index + step) % (unsigned int)ht->size;
    }
}

static Entry *oht_search(OpenHashTable *ht, const char *word, int *probes) {
    if (probes) *probes = 0;
    if (ht == NULL || word == NULL || *word == '\0') return NULL;

    unsigned int index = hash_fnv1a(word) % (unsigned int)ht->size;
    unsigned int step = hash_secondary(word, ht->size);
    int p = 0;

    while (ht->entries[index].state != SLOT_EMPTY) {
        p++;
        if (ht->entries[index].state == SLOT_OCCUPIED &&
            strcmp(ht->entries[index].word, word) == 0) {
            if (probes) *probes = p;
            return &ht->entries[index];
        }
        index = (index + step) % (unsigned int)ht->size;
    }

    if (probes) *probes = p + 1; /* final EMPTY probe */
    return NULL;
}

static bool oht_delete(OpenHashTable *ht, const char *word) {
    if (ht == NULL || word == NULL || *word == '\0') return false;

    unsigned int index = hash_fnv1a(word) % (unsigned int)ht->size;
    unsigned int step = hash_secondary(word, ht->size);

    while (ht->entries[index].state != SLOT_EMPTY) {
        if (ht->entries[index].state == SLOT_OCCUPIED &&
            strcmp(ht->entries[index].word, word) == 0) {
            free(ht->entries[index].word);
            ht->entries[index].word = NULL;
            ht->entries[index].count = 0;
            ht->entries[index].state = SLOT_DELETED;
            ht->count--;
            ht->tombstones++;
            return true;
        }

        index = (index + step) % (unsigned int)ht->size;
    }

    return false;
}

static void oht_destroy(OpenHashTable *ht) {
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
 * REPORTING
 * =============================================================================
 */

static void oht_print_stats(const OpenHashTable *ht) {
    if (ht == NULL) return;

    printf("--- Hash Table Statistics ---\n");
    printf("  Table size:       %d\n", ht->size);
    printf("  Entries:          %d\n", ht->count);
    printf("  Load factor:      %.2f\n", (float)ht->count / (float)ht->size);
    printf("  Total probes:     %lu\n", ht->total_probes);
    if (ht->total_ops > 0) {
        printf("  Avg probes/op:    %.2f\n", (float)ht->total_probes / (float)ht->total_ops);
    }
}

static int compare_wordfreq_desc(const void *a, const void *b) {
    const WordFreq *wa = (const WordFreq *)a;
    const WordFreq *wb = (const WordFreq *)b;

    if (wa->count != wb->count) {
        return (wb->count - wa->count);
    }
    return strcmp(wa->word, wb->word);
}

static void oht_print_top_n(const OpenHashTable *ht, int n) {
    if (ht == NULL || n <= 0) return;

    WordFreq *arr = (WordFreq *)malloc((size_t)ht->count * sizeof(WordFreq));
    if (arr == NULL) return;

    int k = 0;
    for (int i = 0; i < ht->size; i++) {
        if (ht->entries[i].state == SLOT_OCCUPIED) {
            arr[k].word = ht->entries[i].word;
            arr[k].count = ht->entries[i].count;
            k++;
        }
    }

    qsort(arr, (size_t)k, sizeof(WordFreq), compare_wordfreq_desc);

    int display = (n < k) ? n : k;

    printf("\n--- Top %d Most Frequent Words ---\n", n);
    printf("  Rank | Word           | Count\n");
    printf("  ─────┼────────────────┼──────\n");

    for (int i = 0; i < display; i++) {
        printf("  %4d | %-14s | %d\n", i + 1, arr[i].word, arr[i].count);
    }

    free(arr);
}

/* =============================================================================
 * TEXT PROCESSING
 * =============================================================================
 */

static int process_text_file(OpenHashTable *ht, const char *filename) {
    if (ht == NULL || filename == NULL) return -1;

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return -1;
    }

    char word[MAX_WORD_LEN];
    int len = 0;
    int total_words = 0;

    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (isalpha((unsigned char)ch)) {
            if (len < MAX_WORD_LEN - 1) {
                word[len++] = (char)tolower((unsigned char)ch);
            } else {
                /* Word too long: consume but truncate. */
            }
        } else {
            if (len > 0) {
                word[len] = '\0';
                oht_insert(ht, word);
                total_words++;
                len = 0;
            }
        }
    }

    if (len > 0) {
        word[len] = '\0';
        oht_insert(ht, word);
        total_words++;
    }

    fclose(file);
    return total_words;
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 2: Word Frequency Counter (Open Addressing)      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    printf("Processing file: %s\n\n", argv[1]);

    OpenHashTable *ht = oht_create(INITIAL_SIZE);
    if (ht == NULL) {
        fprintf(stderr, "Error: Failed to create hash table\n");
        return 1;
    }

    int total_words = process_text_file(ht, argv[1]);
    if (total_words < 0) {
        oht_destroy(ht);
        return 1;
    }

    oht_print_stats(ht);
    oht_print_top_n(ht, 10);

    printf("\nTotal unique words: %d\n", ht->count);
    printf("Total word count: %d\n", total_words);

    oht_destroy(ht);

    printf("\nProgram completed successfully.\n");
    printf("Memory freed.\n");

    return 0;
}

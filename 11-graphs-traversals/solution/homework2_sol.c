/**
 * =============================================================================
 * HOMEWORK 2 SOLUTION: Anagram Grouper
 * =============================================================================
 *
 * INSTRUCTOR SOLUTION - DO NOT DISTRIBUTE TO STUDENTS
 *
 * This solution implements:
 *   - Hash table with sorted letters as keys
 *   - Linked lists of words sharing the same sorted key
 *   - Efficient anagram detection and grouping
 *   - File processing for large word lists
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 * Usage: ./homework2_sol [wordlist.txt]
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

#define MAX_WORD_LEN 64
#define INITIAL_SIZE 101  /* Prime number */

/**
 * Word node in anagram group
 */
typedef struct WordNode {
    char *word;
    struct WordNode *next;
} WordNode;

/**
 * Anagram group (bucket entry)
 */
typedef struct AnagramGroup {
    char *sorted_key;         /* Sorted letters (the hash key) */
    WordNode *words;          /* List of words in this group */
    int count;                /* Number of words in group */
    struct AnagramGroup *next;
} AnagramGroup;

/**
 * Anagram hash table
 */
typedef struct {
    AnagramGroup **buckets;
    int size;
    int num_groups;
    int total_words;
} AnagramTable;

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Comparison function for qsort (sorting characters)
 */
int compare_chars(const void *a, const void *b) {
    return *(const char *)a - *(const char *)b;
}

/**
 * Get sorted key from word (lowercase, alphabetic only)
 * Returns newly allocated string
 */
char *get_sorted_key(const char *word) {
    /* Count alphabetic characters */
    int len = 0;
    for (const char *p = word; *p; p++) {
        if (isalpha(*p)) len++;
    }
    
    if (len == 0) return NULL;
    
    /* Extract and convert to lowercase */
    char *key = malloc(len + 1);
    int idx = 0;
    for (const char *p = word; *p; p++) {
        if (isalpha(*p)) {
            key[idx++] = tolower(*p);
        }
    }
    key[len] = '\0';
    
    /* Sort characters */
    qsort(key, len, sizeof(char), compare_chars);
    
    return key;
}

/**
 * djb2 hash function
 */
unsigned int hash_djb2(const char *key) {
    unsigned int hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

/* =============================================================================
 * ANAGRAM TABLE OPERATIONS
 * =============================================================================
 */

/**
 * Create a new anagram table
 */
AnagramTable *at_create(void) {
    AnagramTable *at = malloc(sizeof(AnagramTable));
    if (!at) return NULL;
    
    at->size = INITIAL_SIZE;
    at->num_groups = 0;
    at->total_words = 0;
    at->buckets = calloc(INITIAL_SIZE, sizeof(AnagramGroup *));
    
    if (!at->buckets) {
        free(at);
        return NULL;
    }
    
    return at;
}

/**
 * Add a word to the anagram table
 */
bool at_add_word(AnagramTable *at, const char *word) {
    /* Get sorted key */
    char *key = get_sorted_key(word);
    if (!key) return false;
    
    /* Find bucket */
    unsigned int index = hash_djb2(key) % at->size;
    
    /* Search for existing group with this key */
    AnagramGroup *group = at->buckets[index];
    while (group) {
        if (strcmp(group->sorted_key, key) == 0) {
            /* Found existing group - add word */
            WordNode *new_word = malloc(sizeof(WordNode));
            new_word->word = strdup(word);
            new_word->next = group->words;
            group->words = new_word;
            group->count++;
            at->total_words++;
            free(key);
            return true;
        }
        group = group->next;
    }
    
    /* Create new group */
    AnagramGroup *new_group = malloc(sizeof(AnagramGroup));
    new_group->sorted_key = key;  /* Transfer ownership */
    new_group->count = 1;
    new_group->next = at->buckets[index];
    at->buckets[index] = new_group;
    
    /* Add first word */
    WordNode *new_word = malloc(sizeof(WordNode));
    new_word->word = strdup(word);
    new_word->next = NULL;
    new_group->words = new_word;
    
    at->num_groups++;
    at->total_words++;
    
    return true;
}

/**
 * Find all anagrams of a word
 * Returns NULL if no anagrams found
 */
WordNode *at_find_anagrams(AnagramTable *at, const char *word) {
    char *key = get_sorted_key(word);
    if (!key) return NULL;
    
    unsigned int index = hash_djb2(key) % at->size;
    
    AnagramGroup *group = at->buckets[index];
    while (group) {
        if (strcmp(group->sorted_key, key) == 0) {
            free(key);
            return group->words;
        }
        group = group->next;
    }
    
    free(key);
    return NULL;
}

/**
 * Structure for sorting groups
 */
typedef struct {
    AnagramGroup *group;
} GroupPtr;

/**
 * Comparison function for sorting groups by size
 */
int compare_groups(const void *a, const void *b) {
    const GroupPtr *ga = (const GroupPtr *)a;
    const GroupPtr *gb = (const GroupPtr *)b;
    return gb->group->count - ga->group->count;  /* Descending */
}

/**
 * Print all anagram groups with 2+ words
 */
void at_print_groups(AnagramTable *at) {
    /* Collect all groups */
    GroupPtr *groups = malloc(at->num_groups * sizeof(GroupPtr));
    int idx = 0;
    
    for (int i = 0; i < at->size; i++) {
        AnagramGroup *group = at->buckets[i];
        while (group) {
            groups[idx++].group = group;
            group = group->next;
        }
    }
    
    /* Sort by group size */
    qsort(groups, at->num_groups, sizeof(GroupPtr), compare_groups);
    
    /* Print groups with 2+ words */
    printf("\n--- Anagram Groups (2+ words) ---\n\n");
    int printed = 0;
    
    for (int i = 0; i < at->num_groups; i++) {
        AnagramGroup *g = groups[i].group;
        if (g->count >= 2) {
            printed++;
            printf("Group %d (%d words): ", printed, g->count);
            
            WordNode *w = g->words;
            while (w) {
                printf("%s", w->word);
                if (w->next) printf(", ");
                w = w->next;
            }
            printf("\n");
        }
    }
    
    if (printed == 0) {
        printf("No anagram groups found.\n");
    }
    
    printf("\n");
    free(groups);
}

/**
 * Print statistics
 */
void at_print_stats(AnagramTable *at) {
    int groups_with_2plus = 0;
    int max_group_size = 0;
    
    for (int i = 0; i < at->size; i++) {
        AnagramGroup *group = at->buckets[i];
        while (group) {
            if (group->count >= 2) groups_with_2plus++;
            if (group->count > max_group_size) max_group_size = group->count;
            group = group->next;
        }
    }
    
    printf("\n--- Statistics ---\n");
    printf("  Total words:        %d\n", at->total_words);
    printf("  Unique sorted keys: %d\n", at->num_groups);
    printf("  Anagram groups:     %d (2+ words)\n", groups_with_2plus);
    printf("  Largest group:      %d words\n", max_group_size);
    printf("\n");
}

/**
 * Destroy the anagram table
 */
void at_destroy(AnagramTable *at) {
    for (int i = 0; i < at->size; i++) {
        AnagramGroup *group = at->buckets[i];
        while (group) {
            AnagramGroup *next_group = group->next;
            
            /* Free words */
            WordNode *word = group->words;
            while (word) {
                WordNode *next_word = word->next;
                free(word->word);
                free(word);
                word = next_word;
            }
            
            free(group->sorted_key);
            free(group);
            group = next_group;
        }
    }
    free(at->buckets);
    free(at);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     HOMEWORK 2: Anagram Grouper                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    AnagramTable *at = at_create();
    if (!at) {
        fprintf(stderr, "Error: Failed to create anagram table\n");
        return 1;
    }
    
    if (argc >= 2) {
        /* Read from file */
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "Error: Cannot open file '%s'\n", argv[1]);
            at_destroy(at);
            return 1;
        }
        
        printf("Reading words from: %s\n", argv[1]);
        
        char word[MAX_WORD_LEN];
        while (fscanf(file, "%63s", word) == 1) {
            if (strlen(word) >= 2) {
                at_add_word(at, word);
            }
        }
        
        fclose(file);
    } else {
        /* Interactive mode / demo */
        printf("No file specified. Using demo words.\n\n");
        
        const char *demo_words[] = {
            "listen", "silent", "enlist", "tinsel", "inlets",
            "hello", "world", "cat", "act", "tac",
            "earth", "heart", "hater", "rathe",
            "state", "taste", "teats",
            "stop", "tops", "spot", "pots", "opts",
            "evil", "vile", "live", "veil",
            "stressed", "desserts",
            NULL
        };
        
        for (int i = 0; demo_words[i] != NULL; i++) {
            at_add_word(at, demo_words[i]);
        }
    }
    
    /* Print statistics */
    at_print_stats(at);
    
    /* Print anagram groups */
    at_print_groups(at);
    
    /* Demo: find anagrams for a specific word */
    printf("--- Anagram Search ---\n");
    const char *test_word = "listen";
    printf("Anagrams of '%s': ", test_word);
    
    WordNode *anagrams = at_find_anagrams(at, test_word);
    if (anagrams) {
        while (anagrams) {
            printf("%s", anagrams->word);
            if (anagrams->next) printf(", ");
            anagrams = anagrams->next;
        }
        printf("\n");
    } else {
        printf("(none found)\n");
    }
    
    /* Clean up */
    at_destroy(at);
    printf("\nMemory freed. Program completed successfully.\n");
    
    return 0;
}

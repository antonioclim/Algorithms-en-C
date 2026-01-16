/**
 * =============================================================================
 * EXERCISE 1: Chained Hash Table for Student Records
 * =============================================================================
 *
 * OVERVIEW
 *   This programme implements a hash table using separate chaining (linked
 *   lists) to store and query student records.
 *
 *   The public interface is intentionally minimal:
 *     - Insert (with in-place update on duplicate key)
 *     - Search
 *     - Delete
 *     - Diagnostics: load factor and non-empty bucket count
 *
 *   The main routine is transcript-driven: it reads records from standard input
 *   and prints a deterministic report used by the automated test harness.
 *
 * DATA MODEL
 *   Key:   student ID (string)
 *   Value: {name, grade}
 *
 * COLLISION RESOLUTION
 *   Buckets are singly linked lists. Each insertion places the node at the head
 *   of the chain. Search and deletion are linear in the chain length.
 *
 * COMPLEXITY (n entries, m buckets, load factor alpha = n/m)
 *   - Expected search:    O(1 + alpha) under uniform hashing assumptions
 *   - Worst-case search:  O(n) if all keys collide
 *   - Insert:             same as search plus O(1) pointer updates
 *   - Delete:             same as search plus O(1) pointer updates
 *
 * BUILD
 *   gcc -Wall -Wextra -std=c11 -O2 -o exercise1 exercise1.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =============================================================================
 * CONFIGURATION
 * =============================================================================
 */

#define MAX_ID_LEN 20
#define MAX_NAME_LEN 50
#define TABLE_SIZE 16

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct {
    char id[MAX_ID_LEN];
    char name[MAX_NAME_LEN];
    float grade;
} Student;

typedef struct HashNode {
    Student student;
    struct HashNode *next;
} HashNode;

typedef struct {
    HashNode **buckets;
    int size;
    int count;
} HashTable;

/* =============================================================================
 * HASH FUNCTION
 * =============================================================================
 */

/**
 * djb2 string hash attributed to Daniel J. Bernstein.
 *
 * This is a multiplicative-additive polynomial hash with multiplier 33.
 * It is not cryptographic. Its value in teaching is that it is compact and
 * empirically well behaved on many practical key distributions.
 */
static unsigned int hash_djb2(const char *key) {
    unsigned int hash = 5381u;
    int c;

    while ((c = (unsigned char)*key++) != 0) {
        hash = ((hash << 5) + hash) + (unsigned int)c; /* hash * 33 + c */
    }

    return hash;
}

/* =============================================================================
 * HASH TABLE OPERATIONS
 * =============================================================================
 */

static HashTable *ht_create(int size) {
    if (size <= 0) return NULL;

    HashTable *ht = (HashTable *)malloc(sizeof(*ht));
    if (ht == NULL) return NULL;

    ht->size = size;
    ht->count = 0;
    ht->buckets = (HashNode **)calloc((size_t)size, sizeof(HashNode *));

    if (ht->buckets == NULL) {
        free(ht);
        return NULL;
    }

    return ht;
}

static float ht_load_factor(const HashTable *ht) {
    if (ht == NULL || ht->size == 0) return 0.0f;
    return (float)ht->count / (float)ht->size;
}

/**
 * Insert or update.
 *
 * Returns the bucket index on success, or -1 on allocation failure.
 */
static int ht_insert(HashTable *ht, const Student *student) {
    if (ht == NULL || student == NULL) return -1;

    unsigned int index = hash_djb2(student->id) % (unsigned int)ht->size;

    /* Update existing record if the key is present. */
    for (HashNode *cur = ht->buckets[index]; cur != NULL; cur = cur->next) {
        if (strcmp(cur->student.id, student->id) == 0) {
            strncpy(cur->student.name, student->name, MAX_NAME_LEN - 1);
            cur->student.name[MAX_NAME_LEN - 1] = '\0';
            cur->student.grade = student->grade;
            return (int)index;
        }
    }

    /* Allocate a new node and insert at the head of the chain. */
    HashNode *node = (HashNode *)malloc(sizeof(*node));
    if (node == NULL) return -1;

    strncpy(node->student.id, student->id, MAX_ID_LEN - 1);
    node->student.id[MAX_ID_LEN - 1] = '\0';

    strncpy(node->student.name, student->name, MAX_NAME_LEN - 1);
    node->student.name[MAX_NAME_LEN - 1] = '\0';

    node->student.grade = student->grade;

    node->next = ht->buckets[index];
    ht->buckets[index] = node;
    ht->count++;

    return (int)index;
}

static Student *ht_search(HashTable *ht, const char *id) {
    if (ht == NULL || id == NULL) return NULL;

    unsigned int index = hash_djb2(id) % (unsigned int)ht->size;

    for (HashNode *cur = ht->buckets[index]; cur != NULL; cur = cur->next) {
        if (strcmp(cur->student.id, id) == 0) {
            return &cur->student;
        }
    }

    return NULL;
}

static bool ht_delete(HashTable *ht, const char *id) {
    if (ht == NULL || id == NULL) return false;

    unsigned int index = hash_djb2(id) % (unsigned int)ht->size;

    HashNode *cur = ht->buckets[index];
    HashNode *prev = NULL;

    while (cur != NULL) {
        if (strcmp(cur->student.id, id) == 0) {
            if (prev == NULL) {
                ht->buckets[index] = cur->next;
            } else {
                prev->next = cur->next;
            }

            free(cur);
            ht->count--;
            return true;
        }

        prev = cur;
        cur = cur->next;
    }

    return false;
}

static int ht_non_empty_buckets(const HashTable *ht) {
    if (ht == NULL) return 0;

    int non_empty = 0;
    for (int i = 0; i < ht->size; i++) {
        if (ht->buckets[i] != NULL) non_empty++;
    }
    return non_empty;
}

static void ht_destroy(HashTable *ht) {
    if (ht == NULL) return;

    for (int i = 0; i < ht->size; i++) {
        HashNode *cur = ht->buckets[i];
        while (cur != NULL) {
            HashNode *tmp = cur;
            cur = cur->next;
            free(tmp);
        }
    }

    free(ht->buckets);
    free(ht);
}

/* =============================================================================
 * PRESENTATION UTILITIES
 * =============================================================================
 */

static int cmp_student_ptr_by_id(const void *a, const void *b) {
    const Student *sa = *(const Student * const *)a;
    const Student *sb = *(const Student * const *)b;
    return strcmp(sa->id, sb->id);
}

static void ht_print_stats(const HashTable *ht) {
    printf("--- Hash Table Statistics ---\n");
    printf("  Table size:        %d\n", ht->size);
    printf("  Entries:           %d\n", ht->count);
    printf("  Load factor:       %.2f\n", ht_load_factor(ht));
    printf("  Non-empty buckets: %d\n", ht_non_empty_buckets(ht));
}

static void ht_print_all_sorted(const HashTable *ht) {
    printf("\n--- All Student Records ---\n");

    if (ht == NULL || ht->count == 0) return;

    Student **items = (Student **)malloc((size_t)ht->count * sizeof(*items));
    if (items == NULL) {
        /* Deterministic fallback: print unsorted by bucket. */
        for (int i = 0; i < ht->size; i++) {
            for (HashNode *cur = ht->buckets[i]; cur != NULL; cur = cur->next) {
                printf("  %s: %s, Grade: %.2f\n",
                       cur->student.id,
                       cur->student.name,
                       cur->student.grade);
            }
        }
        return;
    }

    int idx = 0;
    for (int i = 0; i < ht->size; i++) {
        for (HashNode *cur = ht->buckets[i]; cur != NULL; cur = cur->next) {
            items[idx++] = &cur->student;
        }
    }

    qsort(items, (size_t)idx, sizeof(*items), cmp_student_ptr_by_id);

    for (int i = 0; i < idx; i++) {
        printf("  %s: %s, Grade: %.2f\n",
               items[i]->id,
               items[i]->name,
               items[i]->grade);
    }

    free(items);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 1: Chained Hash Table for Student Records        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    HashTable *ht = ht_create(TABLE_SIZE);
    if (ht == NULL) {
        fprintf(stderr, "Error: Failed to create hash table\n");
        return 1;
    }

    printf("Loading student records from file...\n\n");

    Student s;
    while (scanf("%19s %49s %f", s.id, s.name, &s.grade) == 3) {
        int bucket = ht_insert(ht, &s);
        if (bucket < 0) {
            fprintf(stderr, "Error: Insert failed (out of memory)\n");
            ht_destroy(ht);
            return 1;
        }
        printf("Inserted: %s (%s) at bucket %d\n", s.id, s.name, bucket);
    }

    printf("\n");
    ht_print_stats(ht);

    ht_print_all_sorted(ht);

    printf("\n--- Search Test ---\n");
    const char *search_ids[] = {"S003", "S007", "S999"};
    const int num_searches = (int)(sizeof(search_ids) / sizeof(search_ids[0]));

    for (int i = 0; i < num_searches; i++) {
        Student *found = ht_search(ht, search_ids[i]);
        if (found != NULL) {
            printf("  Search %s: Found - %s, Grade: %.2f\n",
                   search_ids[i], found->name, found->grade);
        } else {
            printf("  Search %s: Not found\n", search_ids[i]);
        }
    }

    printf("\n--- Delete Test ---\n");
    printf("  Deleting S002...\n");

    if (ht_delete(ht, "S002")) {
        printf("  Delete successful.\n");
        printf("  Entries after deletion: %d\n", ht->count);
        printf("  Load factor: %.2f\n", ht_load_factor(ht));
    } else {
        printf("  Delete failed: S002 not found.\n");
    }

    ht_destroy(ht);

    printf("\nProgram completed successfully.\n");
    printf("Memory freed.\n");

    return 0;
}

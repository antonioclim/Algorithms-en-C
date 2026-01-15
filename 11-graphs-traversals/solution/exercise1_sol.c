/**
 * =============================================================================
 * EXERCISE 1 SOLUTION: Chained Hash Table for Student Records
 * =============================================================================
 *
 * INSTRUCTOR SOLUTION - DO NOT DISTRIBUTE TO STUDENTS
 *
 * This solution implements:
 *   - djb2 hash function
 *   - Chained hash table with separate chaining
 *   - Insert, search, delete operations
 *   - Load factor tracking
 *   - File I/O for student data
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
 * Usage: ./exercise1_sol < data/students.txt
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

#define MAX_ID_LEN 20
#define MAX_NAME_LEN 50
#define INITIAL_TABLE_SIZE 16

/**
 * Student record structure
 */
typedef struct {
    char id[MAX_ID_LEN];
    char name[MAX_NAME_LEN];
    float grade;
} Student;

/**
 * Hash table node for chaining
 */
typedef struct HashNode {
    Student student;
    struct HashNode *next;
} HashNode;

/**
 * Hash table structure
 */
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
 * djb2 hash function by Daniel J. Bernstein
 * 
 * @param key The string key to hash
 * @return The hash value
 */
unsigned int hash_djb2(const char *key) {
    unsigned int hash = 5381;
    int c;
    
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;  /* hash * 33 + c */
    }
    
    return hash;
}

/* =============================================================================
 * HASH TABLE OPERATIONS
 * =============================================================================
 */

/**
 * Create a new hash table
 * 
 * @param size The number of buckets
 * @return Pointer to the new hash table, or NULL on failure
 */
HashTable *ht_create(int size) {
    HashTable *ht = malloc(sizeof(HashTable));
    if (ht == NULL) {
        fprintf(stderr, "Error: Failed to allocate hash table\n");
        return NULL;
    }
    
    ht->size = size;
    ht->count = 0;
    ht->buckets = calloc(size, sizeof(HashNode *));
    
    if (ht->buckets == NULL) {
        fprintf(stderr, "Error: Failed to allocate buckets\n");
        free(ht);
        return NULL;
    }
    
    return ht;
}

/**
 * Calculate the current load factor
 * 
 * @param ht The hash table
 * @return The load factor (count / size)
 */
float ht_load_factor(HashTable *ht) {
    return (float)ht->count / ht->size;
}

/**
 * Insert a student into the hash table
 * Updates existing entry if ID already exists
 * 
 * @param ht The hash table
 * @param student The student to insert
 * @return The bucket index where inserted
 */
int ht_insert(HashTable *ht, Student *student) {
    unsigned int index = hash_djb2(student->id) % ht->size;
    
    /* Check if student ID already exists */
    HashNode *current = ht->buckets[index];
    while (current != NULL) {
        if (strcmp(current->student.id, student->id) == 0) {
            /* Update existing record */
            strcpy(current->student.name, student->name);
            current->student.grade = student->grade;
            return index;
        }
        current = current->next;
    }
    
    /* Create new node */
    HashNode *new_node = malloc(sizeof(HashNode));
    if (new_node == NULL) {
        fprintf(stderr, "Error: Failed to allocate node\n");
        return -1;
    }
    
    /* Copy student data */
    strcpy(new_node->student.id, student->id);
    strcpy(new_node->student.name, student->name);
    new_node->student.grade = student->grade;
    
    /* Insert at head of chain */
    new_node->next = ht->buckets[index];
    ht->buckets[index] = new_node;
    ht->count++;
    
    return index;
}

/**
 * Search for a student by ID
 * 
 * @param ht The hash table
 * @param id The student ID to search for
 * @return Pointer to the student, or NULL if not found
 */
Student *ht_search(HashTable *ht, const char *id) {
    unsigned int index = hash_djb2(id) % ht->size;
    
    HashNode *current = ht->buckets[index];
    while (current != NULL) {
        if (strcmp(current->student.id, id) == 0) {
            return &current->student;
        }
        current = current->next;
    }
    
    return NULL;
}

/**
 * Delete a student by ID
 * 
 * @param ht The hash table
 * @param id The student ID to delete
 * @return true if deleted, false if not found
 */
bool ht_delete(HashTable *ht, const char *id) {
    unsigned int index = hash_djb2(id) % ht->size;
    
    HashNode *current = ht->buckets[index];
    HashNode *prev = NULL;
    
    while (current != NULL) {
        if (strcmp(current->student.id, id) == 0) {
            /* Found - remove from chain */
            if (prev == NULL) {
                /* First node in chain */
                ht->buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }
            
            free(current);
            ht->count--;
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    return false;
}

/**
 * Print hash table statistics
 * 
 * @param ht The hash table
 */
void ht_print_stats(HashTable *ht) {
    int non_empty = 0;
    int max_chain = 0;
    
    for (int i = 0; i < ht->size; i++) {
        int chain_len = 0;
        HashNode *current = ht->buckets[i];
        
        while (current != NULL) {
            chain_len++;
            current = current->next;
        }
        
        if (chain_len > 0) non_empty++;
        if (chain_len > max_chain) max_chain = chain_len;
    }
    
    printf("--- Hash Table Statistics ---\n");
    printf("  Table size:        %d\n", ht->size);
    printf("  Entries:           %d\n", ht->count);
    printf("  Load factor:       %.2f\n", ht_load_factor(ht));
    printf("  Non-empty buckets: %d\n", non_empty);
}

/**
 * Print all students in the hash table
 * 
 * @param ht The hash table
 */
void ht_print_all(HashTable *ht) {
    printf("\n--- All Student Records ---\n");
    
    for (int i = 0; i < ht->size; i++) {
        HashNode *current = ht->buckets[i];
        
        while (current != NULL) {
            printf("  %s: %s, Grade: %.2f\n",
                   current->student.id,
                   current->student.name,
                   current->student.grade);
            current = current->next;
        }
    }
}

/**
 * Destroy the hash table and free all memory
 * 
 * @param ht The hash table to destroy
 */
void ht_destroy(HashTable *ht) {
    for (int i = 0; i < ht->size; i++) {
        HashNode *current = ht->buckets[i];
        
        while (current != NULL) {
            HashNode *temp = current;
            current = current->next;
            free(temp);
        }
    }
    
    free(ht->buckets);
    free(ht);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 1: Chained Hash Table for Student Records        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Create hash table */
    HashTable *ht = ht_create(INITIAL_TABLE_SIZE);
    if (ht == NULL) {
        return 1;
    }
    
    printf("Loading student records from file...\n\n");
    
    /* Read student records */
    char id[MAX_ID_LEN];
    char name[MAX_NAME_LEN];
    float grade;
    
    while (scanf("%s %s %f", id, name, &grade) == 3) {
        Student student;
        strcpy(student.id, id);
        strcpy(student.name, name);
        student.grade = grade;
        
        int bucket = ht_insert(ht, &student);
        printf("Inserted: %s (%s) at bucket %d\n", id, name, bucket);
    }
    
    /* Print statistics */
    printf("\n");
    ht_print_stats(ht);
    
    /* Print all records */
    ht_print_all(ht);
    
    /* Demonstrate search */
    printf("\n--- Search Test ---\n");
    
    const char *search_ids[] = {"S003", "S007", "S999"};
    int num_searches = sizeof(search_ids) / sizeof(search_ids[0]);
    
    for (int i = 0; i < num_searches; i++) {
        Student *found = ht_search(ht, search_ids[i]);
        if (found != NULL) {
            printf("  Search %s: Found - %s, Grade: %.2f\n",
                   search_ids[i], found->name, found->grade);
        } else {
            printf("  Search %s: Not found\n", search_ids[i]);
        }
    }
    
    /* Demonstrate delete */
    printf("\n--- Delete Test ---\n");
    printf("  Deleting S002...\n");
    
    if (ht_delete(ht, "S002")) {
        printf("  Delete successful.\n");
        printf("  Entries after deletion: %d\n", ht->count);
        printf("  Load factor: %.2f\n", ht_load_factor(ht));
    } else {
        printf("  Delete failed: S002 not found.\n");
    }
    
    /* Clean up */
    ht_destroy(ht);
    
    printf("\nProgram completed successfully.\n");
    printf("Memory freed.\n");
    
    return 0;
}

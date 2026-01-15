/**
 * =============================================================================
 * EXERCISE 1: Chained Hash Table for Student Records
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement a hash table using separate chaining to store and manage
 *   student records. The system must support insert, search, delete and
 *   display operations with proper memory management.
 *
 * REQUIREMENTS:
 *   1. Implement the djb2 hash function
 *   2. Create a chained hash table with dynamic bucket allocation
 *   3. Support insertion with duplicate key updates
 *   4. Implement search returning a pointer to the value
 *   5. Implement deletion with proper memory cleanup
 *   6. Calculate and display load factor
 *   7. Read student data from input file
 *   8. Free all allocated memory before exit
 *
 * EXAMPLE INPUT (students.txt):
 *   S001 Alice 9.5
 *   S002 Bob 7.8
 *   S003 Carol 8.2
 *
 * EXPECTED OUTPUT:
 *   Loaded 3 students into hash table.
 *   Load factor: 0.43
 *   
 *   Search results:
 *   S002 -> Bob (grade: 7.80)
 *   S099 -> Not found
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

#define TABLE_SIZE 7
#define MAX_ID_LEN 16
#define MAX_NAME_LEN 64

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * TODO 1: Define the Student structure
 *
 * The Student structure should contain:
 *   - id: a string for the student ID (e.g., "S001")
 *   - name: a string for the student's name
 *   - grade: a float for the student's grade
 *
 * Hint: Use char arrays or char pointers for strings.
 *       If using pointers, remember to allocate memory with strdup().
 */
typedef struct {
    /* YOUR CODE HERE */
    char id[MAX_ID_LEN];
    char name[MAX_NAME_LEN];
    float grade;
} Student;

/**
 * TODO 2: Define the HashNode structure for chaining
 *
 * Each node in the chain should contain:
 *   - data: a Student structure (or pointer to one)
 *   - next: pointer to the next node in the chain
 *
 * Hint: This is similar to a linked list node.
 */
typedef struct HashNode {
    /* YOUR CODE HERE */
    Student data;
    struct HashNode *next;
} HashNode;

/**
 * TODO 3: Define the HashTable structure
 *
 * The hash table should contain:
 *   - buckets: an array of pointers to HashNode (the chains)
 *   - size: the number of buckets
 *   - count: the number of entries currently stored
 *
 * Hint: buckets should be HashNode** (pointer to array of pointers)
 */
typedef struct {
    /* YOUR CODE HERE */
    HashNode **buckets;
    int size;
    int count;
} HashTable;

/* =============================================================================
 * HASH FUNCTION
 * =============================================================================
 */

/**
 * TODO 4: Implement the djb2 hash function
 *
 * The djb2 algorithm:
 *   1. Start with hash = 5381
 *   2. For each character c in the key:
 *      hash = hash * 33 + c
 *      (which can be written as: hash = ((hash << 5) + hash) + c)
 *   3. Return the final hash value
 *
 * @param key The string to hash
 * @return The hash value (unsigned int)
 *
 * Hint: Use a while loop that processes characters until '\0'
 */
unsigned int hash_djb2(const char *key) {
    /* YOUR CODE HERE */
    unsigned int hash = 5381;
    
    /* Implement the djb2 algorithm here */
    
    return hash;  /* Replace with correct implementation */
}

/* =============================================================================
 * HASH TABLE OPERATIONS
 * =============================================================================
 */

/**
 * TODO 5: Implement hash table creation
 *
 * Steps:
 *   1. Allocate memory for the HashTable structure
 *   2. Set size to TABLE_SIZE
 *   3. Set count to 0
 *   4. Allocate memory for the buckets array (array of HashNode pointers)
 *   5. Initialise all bucket pointers to NULL
 *   6. Return the created hash table
 *
 * @return Pointer to the newly created hash table, or NULL on failure
 *
 * Hint: Use malloc() for the table and calloc() for buckets (auto-zeroes)
 */
HashTable *ht_create(void) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace with correct implementation */
}

/**
 * TODO 6: Implement the insert operation
 *
 * Steps:
 *   1. Calculate the bucket index using: hash_djb2(student.id) % ht->size
 *   2. Check if a student with the same ID already exists in that bucket
 *      - If yes, update the existing record and return
 *   3. If not found, create a new HashNode
 *   4. Copy the student data into the new node
 *   5. Insert the new node at the HEAD of the bucket's chain
 *   6. Increment the count
 *
 * @param ht Pointer to the hash table
 * @param student The student record to insert
 *
 * Hint: Inserting at head is: new_node->next = bucket; bucket = new_node;
 */
void ht_insert(HashTable *ht, Student student) {
    if (ht == NULL) return;
    
    /* YOUR CODE HERE */
}

/**
 * TODO 7: Implement the search operation
 *
 * Steps:
 *   1. Calculate the bucket index using: hash_djb2(id) % ht->size
 *   2. Traverse the chain at that bucket
 *   3. For each node, compare the student ID with the search key
 *   4. If found, return a pointer to the Student structure
 *   5. If not found (reached end of chain), return NULL
 *
 * @param ht Pointer to the hash table
 * @param id The student ID to search for
 * @return Pointer to the Student if found, NULL otherwise
 *
 * Hint: Use strcmp() to compare strings (returns 0 if equal)
 */
Student *ht_search(HashTable *ht, const char *id) {
    if (ht == NULL || id == NULL) return NULL;
    
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace with correct implementation */
}

/**
 * TODO 8: Implement the delete operation
 *
 * Steps:
 *   1. Calculate the bucket index
 *   2. Traverse the chain, keeping track of the previous node
 *   3. When found:
 *      - If it's the first node: bucket = node->next
 *      - Otherwise: prev->next = node->next
 *   4. Free the deleted node
 *   5. Decrement the count
 *   6. Return true if deleted, false if not found
 *
 * @param ht Pointer to the hash table
 * @param id The student ID to delete
 * @return true if deleted, false if not found
 *
 * Hint: Handle the special case where the node to delete is the first one
 */
bool ht_delete(HashTable *ht, const char *id) {
    if (ht == NULL || id == NULL) return false;
    
    /* YOUR CODE HERE */
    
    return false;  /* Replace with correct implementation */
}

/**
 * Calculate the load factor
 *
 * Load factor = number of entries / number of buckets
 */
float ht_load_factor(HashTable *ht) {
    if (ht == NULL || ht->size == 0) return 0.0f;
    return (float)ht->count / ht->size;
}

/**
 * Display all entries in the hash table
 */
void ht_display(HashTable *ht) {
    if (ht == NULL) return;
    
    printf("\nHash Table Contents:\n");
    printf("┌─────┬────────────────────────────────────────────────┐\n");
    
    for (int i = 0; i < ht->size; i++) {
        printf("│ %3d │", i);
        
        HashNode *current = ht->buckets[i];
        if (current == NULL) {
            printf(" (empty)                                       │\n");
        } else {
            int first = 1;
            while (current != NULL) {
                if (!first) printf("\n│     │");
                printf(" [%s: %s, %.2f]", 
                       current->data.id, 
                       current->data.name, 
                       current->data.grade);
                first = 0;
                current = current->next;
            }
            printf("\n");
        }
    }
    
    printf("└─────┴────────────────────────────────────────────────┘\n");
    printf("\nStatistics:\n");
    printf("  Entries: %d\n", ht->count);
    printf("  Buckets: %d\n", ht->size);
    printf("  Load factor: %.2f\n", ht_load_factor(ht));
}

/**
 * Destroy the hash table and free all memory
 *
 * This function is provided complete as an example of proper cleanup.
 */
void ht_destroy(HashTable *ht) {
    if (ht == NULL) return;
    
    /* Free all nodes in each bucket */
    for (int i = 0; i < ht->size; i++) {
        HashNode *current = ht->buckets[i];
        while (current != NULL) {
            HashNode *temp = current;
            current = current->next;
            free(temp);
        }
    }
    
    /* Free the buckets array and the table itself */
    free(ht->buckets);
    free(ht);
}

/* =============================================================================
 * FILE LOADING
 * =============================================================================
 */

/**
 * Load students from a file into the hash table
 *
 * File format: ID Name Grade (one per line)
 * Example: S001 Alice 9.5
 */
int load_students_from_file(HashTable *ht, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return -1;
    }
    
    Student student;
    int loaded = 0;
    
    while (fscanf(file, "%15s %63s %f", 
                  student.id, student.name, &student.grade) == 3) {
        ht_insert(ht, student);
        loaded++;
    }
    
    fclose(file);
    return loaded;
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 1: Chained Hash Table for Student Records        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Create the hash table */
    HashTable *ht = ht_create();
    if (ht == NULL) {
        fprintf(stderr, "Error: Failed to create hash table\n");
        return 1;
    }
    
    /* Try to load from file, or use sample data */
    int loaded = load_students_from_file(ht, "data/students.txt");
    
    if (loaded < 0) {
        printf("\nUsing sample data instead...\n");
        
        /* Sample data */
        Student students[] = {
            {"S001", "Alice", 9.5},
            {"S002", "Bob", 7.8},
            {"S003", "Carol", 8.2},
            {"S004", "Dave", 6.9},
            {"S005", "Eve", 9.1},
            {"S006", "Frank", 7.5},
            {"S007", "Grace", 8.8}
        };
        
        int num_students = sizeof(students) / sizeof(students[0]);
        
        for (int i = 0; i < num_students; i++) {
            ht_insert(ht, students[i]);
            printf("Inserted: %s (%s) -> bucket %d\n", 
                   students[i].id, students[i].name,
                   hash_djb2(students[i].id) % ht->size);
        }
        
        loaded = num_students;
    }
    
    printf("\nLoaded %d students.\n", loaded);
    
    /* Display the hash table */
    ht_display(ht);
    
    /* Search demonstration */
    printf("\n--- Search Demonstration ---\n");
    const char *search_ids[] = {"S002", "S005", "S099"};
    int num_searches = 3;
    
    for (int i = 0; i < num_searches; i++) {
        Student *result = ht_search(ht, search_ids[i]);
        if (result != NULL) {
            printf("Found %s: %s (grade: %.2f)\n", 
                   search_ids[i], result->name, result->grade);
        } else {
            printf("Not found: %s\n", search_ids[i]);
        }
    }
    
    /* Update demonstration */
    printf("\n--- Update Demonstration ---\n");
    Student updated = {"S002", "Bob", 9.9};
    printf("Updating S002's grade to 9.9...\n");
    ht_insert(ht, updated);
    
    Student *bob = ht_search(ht, "S002");
    if (bob != NULL) {
        printf("S002's new grade: %.2f\n", bob->grade);
    }
    
    /* Delete demonstration */
    printf("\n--- Delete Demonstration ---\n");
    printf("Deleting S003 (Carol)...\n");
    bool deleted = ht_delete(ht, "S003");
    printf("Deletion %s\n", deleted ? "successful" : "failed");
    
    /* Final display */
    ht_display(ht);
    
    /* Cleanup */
    ht_destroy(ht);
    printf("\n[✓] Hash table destroyed. All memory freed.\n\n");
    
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Implement automatic rehashing when load factor exceeds 0.75
 *
 * 2. Add a function to find all students with grades above a threshold
 *
 * 3. Implement an iterator to traverse all entries in the hash table
 *
 * 4. Add support for case-insensitive ID comparisons
 *
 * 5. Track and display collision statistics (chain lengths)
 *
 * =============================================================================
 */

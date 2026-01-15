/**
 * =============================================================================
 * HOMEWORK 1 SOLUTION: Phone Directory
 * =============================================================================
 *
 * INSTRUCTOR SOLUTION - DO NOT DISTRIBUTE TO STUDENTS
 *
 * This solution implements:
 *   - Chained hash table for contact storage
 *   - Case-insensitive name lookup
 *   - Dynamic resizing when load factor > 0.75
 *   - Alphabetically sorted listing
 *   - Command-line interface
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 * Usage: ./homework1_sol
 *
 * Commands: ADD name phone | FIND name | DELETE name | LIST | STATS | EXIT
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

#define MAX_NAME_LEN 50
#define MAX_PHONE_LEN 16
#define INITIAL_SIZE 53
#define MAX_LOAD_FACTOR 0.75

/**
 * Contact structure
 */
typedef struct {
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
} Contact;

/**
 * Hash table node
 */
typedef struct ContactNode {
    Contact contact;
    struct ContactNode *next;
} ContactNode;

/**
 * Phone directory (hash table)
 */
typedef struct {
    ContactNode **buckets;
    int size;
    int count;
} PhoneDirectory;

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Convert string to lowercase (returns new allocated string)
 */
char *str_to_lower(const char *str) {
    char *lower = strdup(str);
    for (char *p = lower; *p; p++) {
        *p = tolower(*p);
    }
    return lower;
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
 * PHONE DIRECTORY OPERATIONS
 * =============================================================================
 */

/**
 * Create a new phone directory
 */
PhoneDirectory *dir_create(void) {
    PhoneDirectory *dir = malloc(sizeof(PhoneDirectory));
    if (!dir) return NULL;
    
    dir->size = INITIAL_SIZE;
    dir->count = 0;
    dir->buckets = calloc(INITIAL_SIZE, sizeof(ContactNode *));
    
    if (!dir->buckets) {
        free(dir);
        return NULL;
    }
    
    return dir;
}

/**
 * Calculate load factor
 */
float dir_load_factor(PhoneDirectory *dir) {
    return (float)dir->count / dir->size;
}

/* Forward declaration */
void dir_rehash(PhoneDirectory *dir);

/**
 * Add or update a contact
 */
bool dir_add(PhoneDirectory *dir, const char *name, const char *phone) {
    /* Rehash if needed */
    if (dir_load_factor(dir) > MAX_LOAD_FACTOR) {
        dir_rehash(dir);
    }
    
    /* Create lowercase key for hashing */
    char *lower_name = str_to_lower(name);
    unsigned int index = hash_djb2(lower_name) % dir->size;
    
    /* Check if contact exists */
    ContactNode *current = dir->buckets[index];
    while (current) {
        char *existing_lower = str_to_lower(current->contact.name);
        int cmp = strcmp(existing_lower, lower_name);
        free(existing_lower);
        
        if (cmp == 0) {
            /* Update existing contact */
            strncpy(current->contact.phone, phone, MAX_PHONE_LEN - 1);
            free(lower_name);
            return true;
        }
        current = current->next;
    }
    
    /* Create new contact */
    ContactNode *new_node = malloc(sizeof(ContactNode));
    if (!new_node) {
        free(lower_name);
        return false;
    }
    
    strncpy(new_node->contact.name, name, MAX_NAME_LEN - 1);
    strncpy(new_node->contact.phone, phone, MAX_PHONE_LEN - 1);
    new_node->next = dir->buckets[index];
    dir->buckets[index] = new_node;
    dir->count++;
    
    free(lower_name);
    return true;
}

/**
 * Find a contact by name (case-insensitive)
 */
const char *dir_find(PhoneDirectory *dir, const char *name) {
    char *lower_name = str_to_lower(name);
    unsigned int index = hash_djb2(lower_name) % dir->size;
    
    ContactNode *current = dir->buckets[index];
    while (current) {
        char *existing_lower = str_to_lower(current->contact.name);
        int cmp = strcmp(existing_lower, lower_name);
        free(existing_lower);
        
        if (cmp == 0) {
            free(lower_name);
            return current->contact.phone;
        }
        current = current->next;
    }
    
    free(lower_name);
    return NULL;
}

/**
 * Delete a contact
 */
bool dir_delete(PhoneDirectory *dir, const char *name) {
    char *lower_name = str_to_lower(name);
    unsigned int index = hash_djb2(lower_name) % dir->size;
    
    ContactNode *current = dir->buckets[index];
    ContactNode *prev = NULL;
    
    while (current) {
        char *existing_lower = str_to_lower(current->contact.name);
        int cmp = strcmp(existing_lower, lower_name);
        free(existing_lower);
        
        if (cmp == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                dir->buckets[index] = current->next;
            }
            free(current);
            dir->count--;
            free(lower_name);
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    free(lower_name);
    return false;
}

/**
 * Comparison function for sorting contacts
 */
int compare_contacts(const void *a, const void *b) {
    const Contact *ca = (const Contact *)a;
    const Contact *cb = (const Contact *)b;
    return strcasecmp(ca->name, cb->name);
}

/**
 * List all contacts in alphabetical order
 */
void dir_list(PhoneDirectory *dir) {
    if (dir->count == 0) {
        printf("Directory is empty.\n");
        return;
    }
    
    /* Collect all contacts */
    Contact *contacts = malloc(dir->count * sizeof(Contact));
    int idx = 0;
    
    for (int i = 0; i < dir->size; i++) {
        ContactNode *current = dir->buckets[i];
        while (current) {
            contacts[idx++] = current->contact;
            current = current->next;
        }
    }
    
    /* Sort alphabetically */
    qsort(contacts, dir->count, sizeof(Contact), compare_contacts);
    
    /* Print sorted list */
    printf("\n--- Directory Listing (%d contacts) ---\n", dir->count);
    for (int i = 0; i < dir->count; i++) {
        printf("  %s: %s\n", contacts[i].name, contacts[i].phone);
    }
    printf("\n");
    
    free(contacts);
}

/**
 * Print statistics
 */
void dir_stats(PhoneDirectory *dir) {
    int max_chain = 0;
    int non_empty = 0;
    
    for (int i = 0; i < dir->size; i++) {
        int chain_len = 0;
        ContactNode *current = dir->buckets[i];
        while (current) {
            chain_len++;
            current = current->next;
        }
        if (chain_len > 0) non_empty++;
        if (chain_len > max_chain) max_chain = chain_len;
    }
    
    printf("\n--- Directory Statistics ---\n");
    printf("  Total contacts:    %d\n", dir->count);
    printf("  Table size:        %d\n", dir->size);
    printf("  Load factor:       %.2f\n", dir_load_factor(dir));
    printf("  Non-empty buckets: %d\n", non_empty);
    printf("  Longest chain:     %d\n", max_chain);
    printf("\n");
}

/**
 * Rehash the directory
 */
void dir_rehash(PhoneDirectory *dir) {
    ContactNode **old_buckets = dir->buckets;
    int old_size = dir->size;
    
    /* Find next prime (approximately double) */
    int new_size = old_size * 2 + 1;
    
    dir->size = new_size;
    dir->buckets = calloc(new_size, sizeof(ContactNode *));
    dir->count = 0;
    
    /* Reinsert all contacts */
    for (int i = 0; i < old_size; i++) {
        ContactNode *current = old_buckets[i];
        while (current) {
            ContactNode *next = current->next;
            dir_add(dir, current->contact.name, current->contact.phone);
            free(current);
            current = next;
        }
    }
    
    free(old_buckets);
    printf("[Rehashed: %d → %d buckets]\n", old_size, new_size);
}

/**
 * Destroy the directory
 */
void dir_destroy(PhoneDirectory *dir) {
    for (int i = 0; i < dir->size; i++) {
        ContactNode *current = dir->buckets[i];
        while (current) {
            ContactNode *next = current->next;
            free(current);
            current = next;
        }
    }
    free(dir->buckets);
    free(dir);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     HOMEWORK 1: Phone Directory                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Commands: ADD <name> <phone> | FIND <name> | DELETE <name>\n");
    printf("          LIST | STATS | EXIT\n\n");
    
    PhoneDirectory *dir = dir_create();
    if (!dir) {
        fprintf(stderr, "Error: Failed to create directory\n");
        return 1;
    }
    
    char command[20];
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    
    while (1) {
        printf("> ");
        if (scanf("%19s", command) != 1) break;
        
        if (strcasecmp(command, "ADD") == 0) {
            if (scanf("%49s %15s", name, phone) == 2) {
                if (dir_add(dir, name, phone)) {
                    printf("Added: %s → %s\n", name, phone);
                } else {
                    printf("Error: Failed to add contact\n");
                }
            }
        }
        else if (strcasecmp(command, "FIND") == 0) {
            if (scanf("%49s", name) == 1) {
                const char *result = dir_find(dir, name);
                if (result) {
                    printf("Found: %s → %s\n", name, result);
                } else {
                    printf("Not found: %s\n", name);
                }
            }
        }
        else if (strcasecmp(command, "DELETE") == 0) {
            if (scanf("%49s", name) == 1) {
                if (dir_delete(dir, name)) {
                    printf("Deleted: %s\n", name);
                } else {
                    printf("Not found: %s\n", name);
                }
            }
        }
        else if (strcasecmp(command, "LIST") == 0) {
            dir_list(dir);
        }
        else if (strcasecmp(command, "STATS") == 0) {
            dir_stats(dir);
        }
        else if (strcasecmp(command, "EXIT") == 0) {
            break;
        }
        else {
            printf("Unknown command: %s\n", command);
        }
    }
    
    dir_destroy(dir);
    printf("\nDirectory destroyed. Goodbye!\n");
    
    return 0;
}

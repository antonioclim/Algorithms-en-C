/**
 * =============================================================================
 * Week 08 Homework 1: Contact Directory - SOLUTION
 * =============================================================================
 * 
 * INSTRUCTOR ONLY - Do not distribute to students
 *
 * This implements a contact directory using a BST ordered alphabetically
 * by contact name.
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

#define MAX_NAME_LEN 50
#define MAX_PHONE_LEN 15

typedef struct Contact {
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
} Contact;

typedef struct ContactNode {
    Contact contact;
    struct ContactNode *left;
    struct ContactNode *right;
} ContactNode;

/* =============================================================================
 * NODE CREATION
 * =============================================================================
 */

ContactNode *create_contact_node(const char *name, const char *phone) {
    ContactNode *node = (ContactNode *)malloc(sizeof(ContactNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    
    strncpy(node->contact.name, name, MAX_NAME_LEN - 1);
    node->contact.name[MAX_NAME_LEN - 1] = '\0';
    
    strncpy(node->contact.phone, phone, MAX_PHONE_LEN - 1);
    node->contact.phone[MAX_PHONE_LEN - 1] = '\0';
    
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

/* =============================================================================
 * INSERTION
 * =============================================================================
 */

ContactNode *insert_contact(ContactNode *root, const char *name, const char *phone) {
    if (root == NULL) {
        return create_contact_node(name, phone);
    }
    
    int cmp = strcmp(name, root->contact.name);
    
    if (cmp < 0) {
        root->left = insert_contact(root->left, name, phone);
    } else if (cmp > 0) {
        root->right = insert_contact(root->right, name, phone);
    } else {
        /* Duplicate name: update phone number */
        strncpy(root->contact.phone, phone, MAX_PHONE_LEN - 1);
        root->contact.phone[MAX_PHONE_LEN - 1] = '\0';
    }
    
    return root;
}

/* =============================================================================
 * SEARCH
 * =============================================================================
 */

ContactNode *search_contact(ContactNode *root, const char *name) {
    if (root == NULL) {
        return NULL;
    }
    
    int cmp = strcmp(name, root->contact.name);
    
    if (cmp == 0) {
        return root;
    } else if (cmp < 0) {
        return search_contact(root->left, name);
    } else {
        return search_contact(root->right, name);
    }
}

/* =============================================================================
 * FIND MINIMUM (for deletion)
 * =============================================================================
 */

ContactNode *find_min_contact(ContactNode *root) {
    if (root == NULL) {
        return NULL;
    }
    
    while (root->left != NULL) {
        root = root->left;
    }
    
    return root;
}

/* =============================================================================
 * DELETION
 * =============================================================================
 */

ContactNode *delete_contact(ContactNode *root, const char *name) {
    if (root == NULL) {
        return NULL;
    }
    
    int cmp = strcmp(name, root->contact.name);
    
    if (cmp < 0) {
        root->left = delete_contact(root->left, name);
    } else if (cmp > 0) {
        root->right = delete_contact(root->right, name);
    } else {
        /* Found node to delete */
        
        /* Case 1 & 2a: No left child */
        if (root->left == NULL) {
            ContactNode *temp = root->right;
            free(root);
            return temp;
        }
        
        /* Case 2b: No right child */
        if (root->right == NULL) {
            ContactNode *temp = root->left;
            free(root);
            return temp;
        }
        
        /* Case 3: Two children */
        ContactNode *successor = find_min_contact(root->right);
        
        /* Copy successor's data */
        strcpy(root->contact.name, successor->contact.name);
        strcpy(root->contact.phone, successor->contact.phone);
        
        /* Delete successor */
        root->right = delete_contact(root->right, successor->contact.name);
    }
    
    return root;
}

/* =============================================================================
 * DISPLAY OPERATIONS
 * =============================================================================
 */

void display_all_contacts(ContactNode *root) {
    if (root == NULL) {
        return;
    }
    
    /* In-order traversal for alphabetical order */
    display_all_contacts(root->left);
    printf("  %s: %s\n", root->contact.name, root->contact.phone);
    display_all_contacts(root->right);
}

void display_contacts_starting_with(ContactNode *root, char letter) {
    if (root == NULL) {
        return;
    }
    
    letter = toupper(letter);
    char first = toupper(root->contact.name[0]);
    
    /* Prune left if first letter > target letter */
    if (first >= letter) {
        display_contacts_starting_with(root->left, letter);
    }
    
    /* Print if matches */
    if (first == letter) {
        printf("  %s: %s\n", root->contact.name, root->contact.phone);
    }
    
    /* Prune right if first letter < target letter */
    if (first <= letter) {
        display_contacts_starting_with(root->right, letter);
    }
}

/* =============================================================================
 * MEMORY CLEANUP
 * =============================================================================
 */

void free_directory(ContactNode *root) {
    if (root == NULL) {
        return;
    }
    
    free_directory(root->left);
    free_directory(root->right);
    free(root);
}

/* =============================================================================
 * COUNT CONTACTS
 * =============================================================================
 */

int count_contacts(ContactNode *root) {
    if (root == NULL) {
        return 0;
    }
    return 1 + count_contacts(root->left) + count_contacts(root->right);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    ContactNode *directory = NULL;
    char command[20];
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    int n;
    
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "Error reading number of operations\n");
        return 1;
    }
    
    for (int i = 0; i < n; i++) {
        if (scanf("%s", command) != 1) {
            break;
        }
        
        if (strcmp(command, "ADD") == 0) {
            if (scanf("%s %s", name, phone) == 2) {
                directory = insert_contact(directory, name, phone);
                printf("Added: %s (%s)\n", name, phone);
            }
        }
        else if (strcmp(command, "SEARCH") == 0) {
            if (scanf("%s", name) == 1) {
                ContactNode *found = search_contact(directory, name);
                if (found) {
                    printf("Found: %s - %s\n", found->contact.name, found->contact.phone);
                } else {
                    printf("Not found: %s\n", name);
                }
            }
        }
        else if (strcmp(command, "DELETE") == 0) {
            if (scanf("%s", name) == 1) {
                ContactNode *exists = search_contact(directory, name);
                if (exists) {
                    directory = delete_contact(directory, name);
                    printf("Deleted: %s\n", name);
                } else {
                    printf("Not found: %s\n", name);
                }
            }
        }
        else if (strcmp(command, "LIST") == 0) {
            printf("Contacts:\n");
            if (directory == NULL) {
                printf("  (empty)\n");
            } else {
                display_all_contacts(directory);
            }
        }
        else if (strcmp(command, "STARTING") == 0) {
            char letter;
            if (scanf(" %c", &letter) == 1) {
                printf("Contacts starting with '%c':\n", toupper(letter));
                display_contacts_starting_with(directory, letter);
            }
        }
        else if (strcmp(command, "COUNT") == 0) {
            printf("Total contacts: %d\n", count_contacts(directory));
        }
        else {
            printf("Unknown command: %s\n", command);
        }
    }
    
    free_directory(directory);
    
    return 0;
}

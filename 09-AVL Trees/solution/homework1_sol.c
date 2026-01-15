/**
 * =============================================================================
 * HOMEWORK 1: AVL Tree Dictionary - SOLUTION
 * =============================================================================
 *
 * A dictionary (key-value store) implementation using AVL trees.
 * Keys are strings, values are integers.
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 *
 * =============================================================================
 */

#define _GNU_SOURCE  /* For strdup */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct DictNode {
    char *key;
    int value;
    int height;
    struct DictNode *left;
    struct DictNode *right;
} DictNode;

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

int height(DictNode *node) {
    if (node == NULL) return -1;
    return node->height;
}

int balance_factor(DictNode *node) {
    if (node == NULL) return 0;
    return height(node->left) - height(node->right);
}

void update_height(DictNode *node) {
    if (node == NULL) return;
    int left_h = height(node->left);
    int right_h = height(node->right);
    node->height = 1 + (left_h > right_h ? left_h : right_h);
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

/**
 * Create a new dictionary node
 */
DictNode *create_node(const char *key, int value) {
    DictNode *node = (DictNode *)malloc(sizeof(DictNode));
    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed for node\n");
        exit(EXIT_FAILURE);
    }
    
    /* Allocate and copy the key string */
    node->key = strdup(key);
    if (node->key == NULL) {
        fprintf(stderr, "Memory allocation failed for key\n");
        free(node);
        exit(EXIT_FAILURE);
    }
    
    node->value = value;
    node->height = 0;
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

/* =============================================================================
 * ROTATIONS
 * =============================================================================
 */

DictNode *rotate_right(DictNode *y) {
    DictNode *x = y->left;
    DictNode *B = x->right;
    
    x->right = y;
    y->left = B;
    
    update_height(y);
    update_height(x);
    
    return x;
}

DictNode *rotate_left(DictNode *x) {
    DictNode *y = x->right;
    DictNode *B = y->left;
    
    y->left = x;
    x->right = B;
    
    update_height(x);
    update_height(y);
    
    return y;
}

DictNode *rebalance(DictNode *node) {
    if (node == NULL) return NULL;
    
    update_height(node);
    int bf = balance_factor(node);
    
    /* Left-heavy */
    if (bf > 1) {
        if (balance_factor(node->left) < 0) {
            node->left = rotate_left(node->left);
        }
        return rotate_right(node);
    }
    
    /* Right-heavy */
    if (bf < -1) {
        if (balance_factor(node->right) > 0) {
            node->right = rotate_right(node->right);
        }
        return rotate_left(node);
    }
    
    return node;
}

/* =============================================================================
 * DICTIONARY OPERATIONS
 * =============================================================================
 */

/**
 * Insert or update a key-value pair
 */
DictNode *dict_insert(DictNode *node, const char *key, int value) {
    if (node == NULL) {
        return create_node(key, value);
    }
    
    int cmp = strcmp(key, node->key);
    
    if (cmp < 0) {
        node->left = dict_insert(node->left, key, value);
    } else if (cmp > 0) {
        node->right = dict_insert(node->right, key, value);
    } else {
        /* Key exists - update value */
        node->value = value;
        return node;
    }
    
    return rebalance(node);
}

/**
 * Get the value for a key
 * Returns -1 if not found
 */
int dict_get(DictNode *node, const char *key) {
    if (node == NULL) return -1;
    
    int cmp = strcmp(key, node->key);
    
    if (cmp == 0) {
        return node->value;
    } else if (cmp < 0) {
        return dict_get(node->left, key);
    } else {
        return dict_get(node->right, key);
    }
}

/**
 * Check if a key exists
 */
bool dict_contains(DictNode *node, const char *key) {
    if (node == NULL) return false;
    
    int cmp = strcmp(key, node->key);
    
    if (cmp == 0) return true;
    if (cmp < 0) return dict_contains(node->left, key);
    return dict_contains(node->right, key);
}

/**
 * Find minimum key node
 */
DictNode *dict_find_min(DictNode *node) {
    if (node == NULL) return NULL;
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

/**
 * Delete a key from the dictionary
 */
DictNode *dict_delete(DictNode *node, const char *key) {
    if (node == NULL) return NULL;
    
    int cmp = strcmp(key, node->key);
    
    if (cmp < 0) {
        node->left = dict_delete(node->left, key);
    } else if (cmp > 0) {
        node->right = dict_delete(node->right, key);
    } else {
        /* Found the node to delete */
        
        if (node->left == NULL || node->right == NULL) {
            DictNode *temp = node->left ? node->left : node->right;
            
            /* Free the key string */
            free(node->key);
            free(node);
            
            return temp;
        }
        
        /* Two children - find inorder successor */
        DictNode *successor = dict_find_min(node->right);
        
        /* Free old key and copy successor's key */
        free(node->key);
        node->key = strdup(successor->key);
        node->value = successor->value;
        
        /* Delete successor */
        node->right = dict_delete(node->right, successor->key);
    }
    
    return rebalance(node);
}

/**
 * Print dictionary in sorted order (inorder traversal)
 */
void dict_print(DictNode *node) {
    if (node == NULL) return;
    dict_print(node->left);
    printf("  \"%s\": %d\n", node->key, node->value);
    dict_print(node->right);
}

/**
 * Get dictionary size
 */
int dict_size(DictNode *node) {
    if (node == NULL) return 0;
    return 1 + dict_size(node->left) + dict_size(node->right);
}

/**
 * Free all memory
 */
void dict_destroy(DictNode *node) {
    if (node == NULL) return;
    dict_destroy(node->left);
    dict_destroy(node->right);
    free(node->key);
    free(node);
}

/**
 * Validate AVL properties
 */
bool dict_validate(DictNode *node) {
    if (node == NULL) return true;
    
    int bf = balance_factor(node);
    if (bf < -1 || bf > 1) {
        printf("AVL violation at \"%s\": bf = %d\n", node->key, bf);
        return false;
    }
    
    if (node->left && strcmp(node->left->key, node->key) >= 0) {
        printf("BST violation at \"%s\"\n", node->key);
        return false;
    }
    
    if (node->right && strcmp(node->right->key, node->key) <= 0) {
        printf("BST violation at \"%s\"\n", node->key);
        return false;
    }
    
    return dict_validate(node->left) && dict_validate(node->right);
}

/* =============================================================================
 * MAIN PROGRAM - DEMONSTRATION
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║          HOMEWORK 1: AVL Dictionary - SOLUTION                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    DictNode *dict = NULL;
    
    /* Insert some fruits with quantities */
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("Building fruit inventory dictionary...\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    
    dict = dict_insert(dict, "apple", 50);
    dict = dict_insert(dict, "banana", 30);
    dict = dict_insert(dict, "cherry", 25);
    dict = dict_insert(dict, "date", 15);
    dict = dict_insert(dict, "elderberry", 10);
    dict = dict_insert(dict, "fig", 20);
    dict = dict_insert(dict, "grape", 100);
    dict = dict_insert(dict, "honeydew", 5);
    
    printf("Dictionary contents (sorted):\n");
    dict_print(dict);
    printf("\n");
    
    printf("Size: %d items\n", dict_size(dict));
    printf("Valid AVL: %s\n", dict_validate(dict) ? "YES ✓" : "NO ✗");
    
    /* Test lookups */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("Testing lookups...\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    
    printf("dict_get(\"apple\") = %d\n", dict_get(dict, "apple"));
    printf("dict_get(\"cherry\") = %d\n", dict_get(dict, "cherry"));
    printf("dict_get(\"mango\") = %d (not found)\n", dict_get(dict, "mango"));
    printf("dict_contains(\"grape\") = %s\n", dict_contains(dict, "grape") ? "true" : "false");
    printf("dict_contains(\"kiwi\") = %s\n", dict_contains(dict, "kiwi") ? "true" : "false");
    
    /* Test update */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("Updating apple quantity to 75...\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    
    dict = dict_insert(dict, "apple", 75);
    printf("dict_get(\"apple\") = %d\n", dict_get(dict, "apple"));
    
    /* Test deletion */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("Deleting cherry and fig...\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    
    dict = dict_delete(dict, "cherry");
    dict = dict_delete(dict, "fig");
    
    printf("Dictionary contents after deletion:\n");
    dict_print(dict);
    printf("\n");
    
    printf("Size: %d items\n", dict_size(dict));
    printf("Valid AVL: %s\n", dict_validate(dict) ? "YES ✓" : "NO ✗");
    printf("dict_contains(\"cherry\") = %s\n", dict_contains(dict, "cherry") ? "true" : "false");
    
    /* Word frequency example */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("Word frequency counter example...\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    
    /* Clean up and start fresh */
    dict_destroy(dict);
    dict = NULL;
    
    const char *words[] = {"the", "quick", "brown", "fox", "jumps", "over", 
                           "the", "lazy", "dog", "the", "fox", "the"};
    int word_count = sizeof(words) / sizeof(words[0]);
    
    for (int i = 0; i < word_count; i++) {
        int current = dict_get(dict, words[i]);
        if (current == -1) current = 0;
        dict = dict_insert(dict, words[i], current + 1);
    }
    
    printf("Word frequencies:\n");
    dict_print(dict);
    printf("\n");
    
    printf("\"the\" appears %d times\n", dict_get(dict, "the"));
    printf("\"fox\" appears %d times\n", dict_get(dict, "fox"));
    
    /* Cleanup */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("Cleaning up...\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    dict_destroy(dict);
    printf("Memory freed successfully. ✓\n");
    
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    Homework 1 Complete! ✓                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    return 0;
}

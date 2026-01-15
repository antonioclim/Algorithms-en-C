/**
 * =============================================================================
 * EXERCISE 1: Basic BST Operations - SOLUTION
 * =============================================================================
 * 
 * INSTRUCTOR ONLY - Do not distribute to students
 *
 * This file contains the complete solution for Exercise 1.
 * All TODOs have been implemented with detailed comments.
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

/* =============================================================================
 * TODO 1: BST Node Structure - SOLUTION
 * =============================================================================
 */

typedef struct BSTNode {
    int key;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

/* =============================================================================
 * TODO 2: Node Creation - SOLUTION
 * =============================================================================
 */

BSTNode *bst_create_node(int key) {
    BSTNode *node = (BSTNode *)malloc(sizeof(BSTNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

/* =============================================================================
 * TODO 3: Iterative Search - SOLUTION
 * =============================================================================
 */

BSTNode *bst_search(BSTNode *root, int key) {
    BSTNode *current = root;
    
    while (current != NULL) {
        if (key == current->key) {
            return current;  /* Found */
        }
        if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    
    return NULL;  /* Not found */
}

/* =============================================================================
 * TODO 4: Recursive Insertion - SOLUTION
 * =============================================================================
 */

BSTNode *bst_insert(BSTNode *root, int key) {
    /* Base case: found empty position */
    if (root == NULL) {
        return bst_create_node(key);
    }
    
    /* Recursive case: navigate to correct position */
    if (key < root->key) {
        root->left = bst_insert(root->left, key);
    } else if (key > root->key) {
        root->right = bst_insert(root->right, key);
    }
    /* Duplicate keys are ignored */
    
    return root;
}

/* =============================================================================
 * TODO 5: In-order Traversal - SOLUTION
 * =============================================================================
 */

void bst_inorder(BSTNode *root) {
    if (root == NULL) {
        return;
    }
    
    bst_inorder(root->left);      /* L */
    printf("%d ", root->key);     /* N */
    bst_inorder(root->right);     /* R */
}

/* =============================================================================
 * TODO 6: Pre-order and Post-order Traversals - SOLUTION
 * =============================================================================
 */

void bst_preorder(BSTNode *root) {
    if (root == NULL) {
        return;
    }
    
    printf("%d ", root->key);     /* N */
    bst_preorder(root->left);     /* L */
    bst_preorder(root->right);    /* R */
}

void bst_postorder(BSTNode *root) {
    if (root == NULL) {
        return;
    }
    
    bst_postorder(root->left);    /* L */
    bst_postorder(root->right);   /* R */
    printf("%d ", root->key);     /* N */
}

/* =============================================================================
 * TODO 7: Find Minimum and Maximum - SOLUTION
 * =============================================================================
 */

BSTNode *bst_find_min(BSTNode *root) {
    if (root == NULL) {
        return NULL;
    }
    
    /* Minimum is in leftmost node */
    while (root->left != NULL) {
        root = root->left;
    }
    
    return root;
}

BSTNode *bst_find_max(BSTNode *root) {
    if (root == NULL) {
        return NULL;
    }
    
    /* Maximum is in rightmost node */
    while (root->right != NULL) {
        root = root->right;
    }
    
    return root;
}

/* =============================================================================
 * TODO 8: Tree Deallocation - SOLUTION
 * =============================================================================
 */

void bst_free(BSTNode *root) {
    if (root == NULL) {
        return;
    }
    
    /* Post-order: free children before parent */
    bst_free(root->left);
    bst_free(root->right);
    free(root);
}

/* =============================================================================
 * TODO 9 (BONUS): BST Validation - SOLUTION
 * =============================================================================
 */

bool bst_is_valid_helper(BSTNode *node, int min, int max) {
    if (node == NULL) {
        return true;
    }
    
    /* Check if current node violates range */
    if (node->key <= min || node->key >= max) {
        return false;
    }
    
    /* Recursively check subtrees with updated ranges */
    return bst_is_valid_helper(node->left, min, node->key) &&
           bst_is_valid_helper(node->right, node->key, max);
}

bool bst_is_valid(BSTNode *root) {
    return bst_is_valid_helper(root, INT_MIN, INT_MAX);
}

/* =============================================================================
 * BONUS: Additional Utility Functions
 * =============================================================================
 */

int bst_count_nodes(BSTNode *root) {
    if (root == NULL) {
        return 0;
    }
    return 1 + bst_count_nodes(root->left) + bst_count_nodes(root->right);
}

int bst_count_leaves(BSTNode *root) {
    if (root == NULL) {
        return 0;
    }
    if (root->left == NULL && root->right == NULL) {
        return 1;
    }
    return bst_count_leaves(root->left) + bst_count_leaves(root->right);
}

int bst_height(BSTNode *root) {
    if (root == NULL) {
        return -1;
    }
    
    int left_h = bst_height(root->left);
    int right_h = bst_height(root->right);
    
    return 1 + (left_h > right_h ? left_h : right_h);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    BSTNode *root = NULL;
    char command[20];
    int key, n;
    
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "Error reading number of operations\n");
        return 1;
    }
    
    for (int i = 0; i < n; i++) {
        if (scanf("%s", command) != 1) {
            break;
        }
        
        if (strcmp(command, "INSERT") == 0) {
            if (scanf("%d", &key) == 1) {
                root = bst_insert(root, key);
                printf("Inserted: %d\n", key);
            }
        }
        else if (strcmp(command, "SEARCH") == 0) {
            if (scanf("%d", &key) == 1) {
                BSTNode *result = bst_search(root, key);
                if (result != NULL) {
                    printf("Found: %d\n", result->key);
                } else {
                    printf("Not found: %d\n", key);
                }
            }
        }
        else if (strcmp(command, "INORDER") == 0) {
            printf("In-order: ");
            bst_inorder(root);
            printf("\n");
        }
        else if (strcmp(command, "PREORDER") == 0) {
            printf("Pre-order: ");
            bst_preorder(root);
            printf("\n");
        }
        else if (strcmp(command, "POSTORDER") == 0) {
            printf("Post-order: ");
            bst_postorder(root);
            printf("\n");
        }
        else if (strcmp(command, "MIN") == 0) {
            BSTNode *min_node = bst_find_min(root);
            if (min_node != NULL) {
                printf("Minimum: %d\n", min_node->key);
            } else {
                printf("Tree is empty\n");
            }
        }
        else if (strcmp(command, "MAX") == 0) {
            BSTNode *max_node = bst_find_max(root);
            if (max_node != NULL) {
                printf("Maximum: %d\n", max_node->key);
            } else {
                printf("Tree is empty\n");
            }
        }
        else if (strcmp(command, "VALIDATE") == 0) {
            printf("Is valid BST: %s\n", bst_is_valid(root) ? "YES" : "NO");
        }
        else if (strcmp(command, "COUNT") == 0) {
            printf("Node count: %d\n", bst_count_nodes(root));
        }
        else if (strcmp(command, "HEIGHT") == 0) {
            printf("Height: %d\n", bst_height(root));
        }
        else {
            printf("Unknown command: %s\n", command);
        }
    }
    
    bst_free(root);
    
    return 0;
}

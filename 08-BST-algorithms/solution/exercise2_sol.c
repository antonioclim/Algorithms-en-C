/**
 * =============================================================================
 * EXERCISE 2: BST Deletion and Advanced Operations - SOLUTION
 * =============================================================================
 * 
 * INSTRUCTOR ONLY - Do not distribute to students
 *
 * This file contains the complete solution for Exercise 2.
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
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct BSTNode {
    int key;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

/* =============================================================================
 * BASIC OPERATIONS (PROVIDED)
 * =============================================================================
 */

BSTNode *bst_create_node(int key) {
    BSTNode *node = (BSTNode *)malloc(sizeof(BSTNode));
    if (node == NULL) {
        return NULL;
    }
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    return node;
}

BSTNode *bst_insert(BSTNode *root, int key) {
    if (root == NULL) {
        return bst_create_node(key);
    }
    if (key < root->key) {
        root->left = bst_insert(root->left, key);
    } else if (key > root->key) {
        root->right = bst_insert(root->right, key);
    }
    return root;
}

void bst_inorder(BSTNode *root) {
    if (root == NULL) return;
    bst_inorder(root->left);
    printf("%d ", root->key);
    bst_inorder(root->right);
}

void bst_free(BSTNode *root) {
    if (root == NULL) return;
    bst_free(root->left);
    bst_free(root->right);
    free(root);
}

/* =============================================================================
 * TODO 1: Find Minimum - SOLUTION
 * =============================================================================
 */

BSTNode *bst_find_min(BSTNode *root) {
    if (root == NULL) {
        return NULL;
    }
    
    while (root->left != NULL) {
        root = root->left;
    }
    
    return root;
}

/* =============================================================================
 * TODO 2: BST Deletion - SOLUTION
 * =============================================================================
 */

BSTNode *bst_delete(BSTNode *root, int key) {
    /* Base case: key not found */
    if (root == NULL) {
        return NULL;
    }
    
    /* Navigate to the node to delete */
    if (key < root->key) {
        root->left = bst_delete(root->left, key);
    } else if (key > root->key) {
        root->right = bst_delete(root->right, key);
    } else {
        /* Found the node to delete */
        
        /* Case 1: Leaf node (no children) */
        /* Case 2a: Only right child */
        if (root->left == NULL) {
            BSTNode *temp = root->right;
            free(root);
            return temp;
        }
        
        /* Case 2b: Only left child */
        if (root->right == NULL) {
            BSTNode *temp = root->left;
            free(root);
            return temp;
        }
        
        /* Case 3: Two children */
        /* Find in-order successor (smallest in right subtree) */
        BSTNode *successor = bst_find_min(root->right);
        
        /* Copy successor's key to current node */
        root->key = successor->key;
        
        /* Delete the successor from right subtree */
        root->right = bst_delete(root->right, successor->key);
    }
    
    return root;
}

/* =============================================================================
 * TODO 3: Tree Height - SOLUTION
 * =============================================================================
 */

int bst_height(BSTNode *root) {
    if (root == NULL) {
        return -1;  /* Empty tree has height -1 */
    }
    
    int left_height = bst_height(root->left);
    int right_height = bst_height(root->right);
    
    /* Return 1 + max(left, right) */
    if (left_height > right_height) {
        return 1 + left_height;
    } else {
        return 1 + right_height;
    }
}

/* =============================================================================
 * TODO 4: Node Counting - SOLUTION
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
    
    /* Leaf: no children */
    if (root->left == NULL && root->right == NULL) {
        return 1;
    }
    
    return bst_count_leaves(root->left) + bst_count_leaves(root->right);
}

/* =============================================================================
 * TODO 5: Range Search - SOLUTION
 * =============================================================================
 */

void bst_range_search(BSTNode *root, int low, int high) {
    if (root == NULL) {
        return;
    }
    
    /* Only search left subtree if there could be elements >= low */
    if (root->key > low) {
        bst_range_search(root->left, low, high);
    }
    
    /* Print current node if in range */
    if (root->key >= low && root->key <= high) {
        printf("%d ", root->key);
    }
    
    /* Only search right subtree if there could be elements <= high */
    if (root->key < high) {
        bst_range_search(root->right, low, high);
    }
}

/* =============================================================================
 * TODO 6: Kth Smallest Element - SOLUTION
 * =============================================================================
 */

void bst_kth_helper(BSTNode *root, int k, int *count, int *result) {
    if (root == NULL || *result != -1) {
        return;  /* Already found or empty */
    }
    
    /* In-order: left first */
    bst_kth_helper(root->left, k, count, result);
    
    /* Process current node */
    (*count)++;
    if (*count == k) {
        *result = root->key;
        return;
    }
    
    /* Then right */
    bst_kth_helper(root->right, k, count, result);
}

int bst_kth_smallest(BSTNode *root, int k) {
    int count = 0;
    int result = -1;
    
    if (k <= 0) {
        return -1;  /* Invalid k */
    }
    
    bst_kth_helper(root, k, &count, &result);
    
    return result;
}

/* =============================================================================
 * TODO 7: Lowest Common Ancestor - SOLUTION
 * =============================================================================
 */

BSTNode *bst_lca(BSTNode *root, int key1, int key2) {
    if (root == NULL) {
        return NULL;
    }
    
    /* Ensure key1 <= key2 for consistent logic */
    if (key1 > key2) {
        int temp = key1;
        key1 = key2;
        key2 = temp;
    }
    
    /* Both keys smaller: LCA is in left subtree */
    if (key2 < root->key) {
        return bst_lca(root->left, key1, key2);
    }
    
    /* Both keys larger: LCA is in right subtree */
    if (key1 > root->key) {
        return bst_lca(root->right, key1, key2);
    }
    
    /* Keys diverge here (or one equals root): this is the LCA */
    return root;
}

/* =============================================================================
 * TODO 8: Visual Tree Printing - SOLUTION
 * =============================================================================
 */

void bst_print_helper(BSTNode *root, int space, int indent) {
    if (root == NULL) {
        return;
    }
    
    /* Increase distance from left margin */
    space += indent;
    
    /* Print right subtree first (will appear on top) */
    bst_print_helper(root->right, space, indent);
    
    /* Print current node with indentation */
    printf("\n");
    for (int i = indent; i < space; i++) {
        printf(" ");
    }
    printf("%d", root->key);
    
    /* Print left subtree */
    bst_print_helper(root->left, space, indent);
}

void bst_print_tree(BSTNode *root) {
    if (root == NULL) {
        printf("  (empty tree)\n");
        return;
    }
    
    bst_print_helper(root, 0, 5);
    printf("\n");
}

/* =============================================================================
 * TODO 9: BST Validation - SOLUTION
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
    
    /* Recursively check subtrees */
    return bst_is_valid_helper(node->left, min, node->key) &&
           bst_is_valid_helper(node->right, node->key, max);
}

bool bst_is_valid(BSTNode *root) {
    return bst_is_valid_helper(root, INT_MIN, INT_MAX);
}

/* =============================================================================
 * TODO 10 (BONUS): Tree Mirroring - SOLUTION
 * =============================================================================
 */

void bst_mirror(BSTNode *root) {
    if (root == NULL) {
        return;
    }
    
    /* Swap left and right */
    BSTNode *temp = root->left;
    root->left = root->right;
    root->right = temp;
    
    /* Recursively mirror subtrees */
    bst_mirror(root->left);
    bst_mirror(root->right);
}

/* =============================================================================
 * TODO 11 (BONUS): Path Sum - SOLUTION
 * =============================================================================
 */

bool bst_has_path_sum(BSTNode *root, int target) {
    if (root == NULL) {
        return false;
    }
    
    /* Subtract current key from target */
    target -= root->key;
    
    /* Check if leaf and target is zero */
    if (root->left == NULL && root->right == NULL) {
        return target == 0;
    }
    
    /* Check either subtree */
    return bst_has_path_sum(root->left, target) ||
           bst_has_path_sum(root->right, target);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    BSTNode *root = NULL;
    char line[256];
    char command[20];
    int key, key2, n;
    
    /* Read initial tree values */
    if (fgets(line, sizeof(line), stdin) != NULL) {
        char *token = strtok(line, " \t\n");
        while (token != NULL) {
            key = atoi(token);
            root = bst_insert(root, key);
            token = strtok(NULL, " \t\n");
        }
    }
    
    /* Read number of operations */
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "Error reading number of operations\n");
        bst_free(root);
        return 1;
    }
    
    /* Process each operation */
    for (int i = 0; i < n; i++) {
        if (scanf("%s", command) != 1) {
            break;
        }
        
        if (strcmp(command, "DELETE") == 0) {
            if (scanf("%d", &key) == 1) {
                root = bst_delete(root, key);
                printf("Deleted: %d\n", key);
            }
        }
        else if (strcmp(command, "HEIGHT") == 0) {
            printf("Height: %d\n", bst_height(root));
        }
        else if (strcmp(command, "COUNT") == 0) {
            printf("Node count: %d\n", bst_count_nodes(root));
        }
        else if (strcmp(command, "LEAVES") == 0) {
            printf("Leaf count: %d\n", bst_count_leaves(root));
        }
        else if (strcmp(command, "INORDER") == 0) {
            printf("In-order: ");
            bst_inorder(root);
            printf("\n");
        }
        else if (strcmp(command, "RANGE") == 0) {
            if (scanf("%d %d", &key, &key2) == 2) {
                printf("Range [%d, %d]: ", key, key2);
                bst_range_search(root, key, key2);
                printf("\n");
            }
        }
        else if (strcmp(command, "KTH") == 0) {
            if (scanf("%d", &key) == 1) {
                int result = bst_kth_smallest(root, key);
                if (result != -1) {
                    printf("%d%s smallest: %d\n", 
                           key, 
                           (key == 1) ? "st" : (key == 2) ? "nd" : (key == 3) ? "rd" : "th",
                           result);
                } else {
                    printf("Invalid k=%d\n", key);
                }
            }
        }
        else if (strcmp(command, "LCA") == 0) {
            if (scanf("%d %d", &key, &key2) == 2) {
                BSTNode *lca = bst_lca(root, key, key2);
                if (lca != NULL) {
                    printf("LCA of %d and %d: %d\n", key, key2, lca->key);
                } else {
                    printf("LCA not found\n");
                }
            }
        }
        else if (strcmp(command, "PRINT") == 0) {
            printf("Tree structure:\n");
            bst_print_tree(root);
        }
        else if (strcmp(command, "VALID") == 0) {
            printf("Is valid BST: %s\n", bst_is_valid(root) ? "YES" : "NO");
        }
        else if (strcmp(command, "MIRROR") == 0) {
            bst_mirror(root);
            printf("Tree mirrored\n");
        }
        else if (strcmp(command, "PATHSUM") == 0) {
            if (scanf("%d", &key) == 1) {
                printf("Has path sum %d: %s\n", key, 
                       bst_has_path_sum(root, key) ? "YES" : "NO");
            }
        }
        else {
            printf("Unknown command: %s\n", command);
        }
    }
    
    bst_free(root);
    
    return 0;
}

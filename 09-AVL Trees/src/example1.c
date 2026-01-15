/**
 * =============================================================================
 * WEEK 09: AVL TREES
 * Complete Working Example
 * =============================================================================
 *
 * This example demonstrates:
 *   1. AVL node structure and creation
 *   2. Height and balance factor calculations
 *   3. All four rotation types (LL, RR, LR, RL)
 *   4. Insertion with automatic rebalancing
 *   5. Deletion with automatic rebalancing
 *   6. Search operations (search, min, max)
 *   7. Tree traversals (inorder, preorder, postorder)
 *   8. Tree visualisation and validation
 *   9. Memory management
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Usage: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* =============================================================================
 * PART 1: DATA STRUCTURES AND CONSTANTS
 * =============================================================================
 */

/**
 * AVL Tree Node Structure
 * 
 * Each node stores:
 * - key: The integer value stored in this node
 * - height: The height of the subtree rooted at this node
 * - left: Pointer to left child (values < key)
 * - right: Pointer to right child (values > key)
 */
typedef struct AVLNode {
    int key;
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

/* Statistics for demonstration */
static int rotation_count = 0;
static int comparison_count = 0;

/* =============================================================================
 * PART 2: UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Get the height of a node (handles NULL safely)
 * 
 * Convention: height(NULL) = -1, height(leaf) = 0
 */
int height(AVLNode *node) {
    if (node == NULL) return -1;
    return node->height;
}

/**
 * Calculate the balance factor of a node
 * 
 * balance_factor = height(left) - height(right)
 * Valid values in AVL tree: {-1, 0, +1}
 */
int balance_factor(AVLNode *node) {
    if (node == NULL) return 0;
    return height(node->left) - height(node->right);
}

/**
 * Update the height of a node based on its children
 */
void update_height(AVLNode *node) {
    int left_h = height(node->left);
    int right_h = height(node->right);
    node->height = 1 + (left_h > right_h ? left_h : right_h);
}

/**
 * Return the maximum of two integers
 */
int max(int a, int b) {
    return (a > b) ? a : b;
}

/**
 * Create a new AVL node with the given key
 */
AVLNode *avl_create_node(int key) {
    AVLNode *node = (AVLNode *)malloc(sizeof(AVLNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for node %d\n", key);
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->height = 0;  /* New node is a leaf */
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* =============================================================================
 * PART 3: ROTATIONS
 * =============================================================================
 */

/**
 * Right Rotation (LL Case)
 * 
 * Applied when a node becomes left-heavy (balance factor = +2)
 * and its left child is left-heavy or balanced (bf >= 0)
 * 
 *       y                x
 *      / \              / \
 *     x   C    --->    A   y
 *    / \                  / \
 *   A   B                B   C
 */
AVLNode *rotate_right(AVLNode *y) {
    printf("    -> Performing RIGHT rotation on node %d\n", y->key);
    rotation_count++;
    
    AVLNode *x = y->left;       /* x becomes the new root */
    AVLNode *B = x->right;      /* B will be reassigned */
    
    /* Perform rotation */
    x->right = y;
    y->left = B;
    
    /* Update heights (order matters: y first, then x) */
    update_height(y);
    update_height(x);
    
    return x;  /* Return new root of subtree */
}

/**
 * Left Rotation (RR Case)
 * 
 * Applied when a node becomes right-heavy (balance factor = -2)
 * and its right child is right-heavy or balanced (bf <= 0)
 * 
 *     x                    y
 *    / \                  / \
 *   A   y      --->      x   C
 *      / \              / \
 *     B   C            A   B
 */
AVLNode *rotate_left(AVLNode *x) {
    printf("    -> Performing LEFT rotation on node %d\n", x->key);
    rotation_count++;
    
    AVLNode *y = x->right;      /* y becomes the new root */
    AVLNode *B = y->left;       /* B will be reassigned */
    
    /* Perform rotation */
    y->left = x;
    x->right = B;
    
    /* Update heights (order matters: x first, then y) */
    update_height(x);
    update_height(y);
    
    return y;  /* Return new root of subtree */
}

/**
 * Rebalance a node if necessary
 * 
 * Checks the balance factor and performs the appropriate rotation(s)
 */
AVLNode *rebalance(AVLNode *node) {
    if (node == NULL) return NULL;
    
    update_height(node);
    int bf = balance_factor(node);
    
    /* Left-heavy (bf = +2) */
    if (bf > 1) {
        /* Check if we need double rotation (LR case) */
        if (balance_factor(node->left) < 0) {
            printf("    -> LR Case detected at node %d\n", node->key);
            node->left = rotate_left(node->left);
        } else {
            printf("    -> LL Case detected at node %d\n", node->key);
        }
        return rotate_right(node);
    }
    
    /* Right-heavy (bf = -2) */
    if (bf < -1) {
        /* Check if we need double rotation (RL case) */
        if (balance_factor(node->right) > 0) {
            printf("    -> RL Case detected at node %d\n", node->key);
            node->right = rotate_right(node->right);
        } else {
            printf("    -> RR Case detected at node %d\n", node->key);
        }
        return rotate_left(node);
    }
    
    /* Already balanced */
    return node;
}

/* =============================================================================
 * PART 4: INSERTION
 * =============================================================================
 */

/**
 * Insert a key into the AVL tree
 * 
 * Uses standard BST insertion followed by rebalancing
 */
AVLNode *avl_insert(AVLNode *node, int key) {
    /* Base case: create new node */
    if (node == NULL) {
        printf("  Creating node with key %d\n", key);
        return avl_create_node(key);
    }
    
    comparison_count++;
    
    /* Recurse into appropriate subtree */
    if (key < node->key) {
        node->left = avl_insert(node->left, key);
    } else if (key > node->key) {
        node->right = avl_insert(node->right, key);
    } else {
        /* Duplicate key - no insertion */
        printf("  Key %d already exists, skipping\n", key);
        return node;
    }
    
    /* Rebalance on the way back up */
    return rebalance(node);
}

/* =============================================================================
 * PART 5: SEARCH OPERATIONS
 * =============================================================================
 */

/**
 * Search for a key in the AVL tree
 * 
 * Returns pointer to node if found, NULL otherwise
 */
AVLNode *avl_search(AVLNode *node, int key) {
    if (node == NULL) return NULL;
    
    comparison_count++;
    
    if (key == node->key) {
        return node;
    } else if (key < node->key) {
        return avl_search(node->left, key);
    } else {
        return avl_search(node->right, key);
    }
}

/**
 * Find the node with minimum key in the subtree
 */
AVLNode *avl_find_min(AVLNode *node) {
    if (node == NULL) return NULL;
    
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

/**
 * Find the node with maximum key in the subtree
 */
AVLNode *avl_find_max(AVLNode *node) {
    if (node == NULL) return NULL;
    
    while (node->right != NULL) {
        node = node->right;
    }
    return node;
}

/* =============================================================================
 * PART 6: DELETION
 * =============================================================================
 */

/**
 * Delete a key from the AVL tree
 * 
 * Handles three cases:
 * 1. Leaf node (no children)
 * 2. One child
 * 3. Two children (replace with inorder successor)
 */
AVLNode *avl_delete(AVLNode *node, int key) {
    if (node == NULL) {
        printf("  Key %d not found\n", key);
        return NULL;
    }
    
    comparison_count++;
    
    /* Search for the node to delete */
    if (key < node->key) {
        node->left = avl_delete(node->left, key);
    } else if (key > node->key) {
        node->right = avl_delete(node->right, key);
    } else {
        /* Found the node to delete */
        printf("  Found node %d to delete\n", key);
        
        /* Case 1 & 2: Node with 0 or 1 child */
        if (node->left == NULL || node->right == NULL) {
            AVLNode *temp = node->left ? node->left : node->right;
            
            if (temp == NULL) {
                /* No children - just remove */
                printf("    Deleting leaf node\n");
                free(node);
                return NULL;
            } else {
                /* One child - replace with child */
                printf("    Replacing with single child\n");
                free(node);
                return temp;
            }
        }
        
        /* Case 3: Node with two children */
        printf("    Node has two children, finding successor\n");
        AVLNode *successor = avl_find_min(node->right);
        printf("    Successor is %d\n", successor->key);
        
        /* Copy successor's key to this node */
        node->key = successor->key;
        
        /* Delete the successor */
        node->right = avl_delete(node->right, successor->key);
    }
    
    /* Rebalance on the way back up */
    return rebalance(node);
}

/* =============================================================================
 * PART 7: TRAVERSALS
 * =============================================================================
 */

/**
 * Inorder traversal: Left, Root, Right
 * Produces sorted output
 */
void avl_inorder(AVLNode *node) {
    if (node == NULL) return;
    avl_inorder(node->left);
    printf("%d ", node->key);
    avl_inorder(node->right);
}

/**
 * Preorder traversal: Root, Left, Right
 * Useful for copying the tree
 */
void avl_preorder(AVLNode *node) {
    if (node == NULL) return;
    printf("%d ", node->key);
    avl_preorder(node->left);
    avl_preorder(node->right);
}

/**
 * Postorder traversal: Left, Right, Root
 * Useful for deleting the tree
 */
void avl_postorder(AVLNode *node) {
    if (node == NULL) return;
    avl_postorder(node->left);
    avl_postorder(node->right);
    printf("%d ", node->key);
}

/* =============================================================================
 * PART 8: TREE STATISTICS AND VISUALISATION
 * =============================================================================
 */

/**
 * Count the number of nodes in the tree
 */
int avl_size(AVLNode *node) {
    if (node == NULL) return 0;
    return 1 + avl_size(node->left) + avl_size(node->right);
}

/**
 * Get the height of the entire tree
 */
int avl_height(AVLNode *node) {
    return height(node);
}

/**
 * Pretty print the tree structure
 */
void avl_print_helper(AVLNode *node, int depth, char prefix) {
    if (node == NULL) return;
    
    /* Print right subtree first (will appear on top) */
    avl_print_helper(node->right, depth + 1, '/');
    
    /* Print current node */
    for (int i = 0; i < depth; i++) {
        printf("    ");
    }
    printf("%c--[%d](h=%d,bf=%d)\n", 
           prefix, node->key, node->height, balance_factor(node));
    
    /* Print left subtree */
    avl_print_helper(node->left, depth + 1, '\\');
}

void avl_print(AVLNode *root) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                      AVL Tree Structure                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    if (root == NULL) {
        printf("    (empty tree)\n");
    } else {
        avl_print_helper(root, 0, '-');
    }
    printf("\n");
}

/* =============================================================================
 * PART 9: VALIDATION
 * =============================================================================
 */

/**
 * Validate the AVL tree properties
 * 
 * Checks:
 * 1. BST property (left < node < right)
 * 2. AVL balance property (|bf| <= 1)
 * 3. Correct height values
 * 
 * Returns: height if valid, -2 if invalid
 */
int avl_validate(AVLNode *node) {
    if (node == NULL) return -1;
    
    int left_h = avl_validate(node->left);
    int right_h = avl_validate(node->right);
    
    /* Check for invalid subtrees */
    if (left_h == -2 || right_h == -2) return -2;
    
    /* Check BST property */
    if (node->left && node->left->key >= node->key) {
        printf("BST violation: left child %d >= node %d\n", 
               node->left->key, node->key);
        return -2;
    }
    if (node->right && node->right->key <= node->key) {
        printf("BST violation: right child %d <= node %d\n", 
               node->right->key, node->key);
        return -2;
    }
    
    /* Check AVL balance property */
    int bf = left_h - right_h;
    if (bf < -1 || bf > 1) {
        printf("AVL violation: node %d has balance factor %d\n", 
               node->key, bf);
        return -2;
    }
    
    /* Check stored height */
    int expected_h = 1 + (left_h > right_h ? left_h : right_h);
    if (node->height != expected_h) {
        printf("Height mismatch: node %d has stored height %d, expected %d\n",
               node->key, node->height, expected_h);
        return -2;
    }
    
    return expected_h;
}

bool avl_is_valid(AVLNode *root) {
    return avl_validate(root) != -2;
}

/* =============================================================================
 * PART 10: MEMORY MANAGEMENT
 * =============================================================================
 */

/**
 * Free all nodes in the tree (postorder traversal)
 */
void avl_destroy(AVLNode *node) {
    if (node == NULL) return;
    avl_destroy(node->left);
    avl_destroy(node->right);
    free(node);
}

/* =============================================================================
 * DEMONSTRATION FUNCTIONS
 * =============================================================================
 */

void demo_part1_basic_operations(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: Basic AVL Tree Operations                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    AVLNode *root = NULL;
    rotation_count = 0;
    comparison_count = 0;
    
    printf("Inserting values: 50, 30, 70, 20, 40, 60, 80\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    int values[] = {50, 30, 70, 20, 40, 60, 80};
    int n = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < n; i++) {
        printf("\nInserting %d:\n", values[i]);
        root = avl_insert(root, values[i]);
    }
    
    avl_print(root);
    
    printf("Tree Statistics:\n");
    printf("  Size: %d nodes\n", avl_size(root));
    printf("  Height: %d\n", avl_height(root));
    printf("  Rotations performed: %d\n", rotation_count);
    printf("  Valid AVL tree: %s\n", avl_is_valid(root) ? "Yes" : "No");
    
    printf("\nTraversals:\n");
    printf("  Inorder (sorted):  ");
    avl_inorder(root);
    printf("\n  Preorder:          ");
    avl_preorder(root);
    printf("\n  Postorder:         ");
    avl_postorder(root);
    printf("\n");
    
    avl_destroy(root);
}

void demo_part2_ll_rotation(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: LL Case (Right Rotation)                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    AVLNode *root = NULL;
    rotation_count = 0;
    
    printf("Inserting sorted sequence: 30, 20, 10\n");
    printf("This triggers LL case (left-left imbalance)\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    printf("\nInsert 30:\n");
    root = avl_insert(root, 30);
    avl_print(root);
    
    printf("Insert 20:\n");
    root = avl_insert(root, 20);
    avl_print(root);
    
    printf("Insert 10 (triggers rebalancing):\n");
    root = avl_insert(root, 10);
    avl_print(root);
    
    printf("Result: Single right rotation restored balance\n");
    printf("Total rotations: %d\n", rotation_count);
    
    avl_destroy(root);
}

void demo_part3_rr_rotation(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: RR Case (Left Rotation)                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    AVLNode *root = NULL;
    rotation_count = 0;
    
    printf("Inserting sorted sequence: 10, 20, 30\n");
    printf("This triggers RR case (right-right imbalance)\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    printf("\nInsert 10:\n");
    root = avl_insert(root, 10);
    avl_print(root);
    
    printf("Insert 20:\n");
    root = avl_insert(root, 20);
    avl_print(root);
    
    printf("Insert 30 (triggers rebalancing):\n");
    root = avl_insert(root, 30);
    avl_print(root);
    
    printf("Result: Single left rotation restored balance\n");
    printf("Total rotations: %d\n", rotation_count);
    
    avl_destroy(root);
}

void demo_part4_lr_rotation(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: LR Case (Left-Right Double Rotation)             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    AVLNode *root = NULL;
    rotation_count = 0;
    
    printf("Inserting sequence: 30, 10, 20\n");
    printf("This triggers LR case (left child is right-heavy)\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    printf("\nInsert 30:\n");
    root = avl_insert(root, 30);
    avl_print(root);
    
    printf("Insert 10:\n");
    root = avl_insert(root, 10);
    avl_print(root);
    
    printf("Insert 20 (triggers rebalancing):\n");
    root = avl_insert(root, 20);
    avl_print(root);
    
    printf("Result: Double rotation (left on 10, right on 30)\n");
    printf("Total rotations: %d\n", rotation_count);
    
    avl_destroy(root);
}

void demo_part5_rl_rotation(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: RL Case (Right-Left Double Rotation)             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    AVLNode *root = NULL;
    rotation_count = 0;
    
    printf("Inserting sequence: 10, 30, 20\n");
    printf("This triggers RL case (right child is left-heavy)\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    printf("\nInsert 10:\n");
    root = avl_insert(root, 10);
    avl_print(root);
    
    printf("Insert 30:\n");
    root = avl_insert(root, 30);
    avl_print(root);
    
    printf("Insert 20 (triggers rebalancing):\n");
    root = avl_insert(root, 20);
    avl_print(root);
    
    printf("Result: Double rotation (right on 30, left on 10)\n");
    printf("Total rotations: %d\n", rotation_count);
    
    avl_destroy(root);
}

void demo_part6_deletion(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: AVL Deletion with Rebalancing                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    AVLNode *root = NULL;
    rotation_count = 0;
    
    /* Build a larger tree */
    int values[] = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
    int n = sizeof(values) / sizeof(values[0]);
    
    printf("Building tree with: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", values[i]);
        root = avl_insert(root, values[i]);
    }
    printf("\n");
    
    avl_print(root);
    printf("Initial rotations: %d\n\n", rotation_count);
    
    /* Delete several nodes */
    rotation_count = 0;
    
    printf("Deleting 70:\n");
    printf("━━━━━━━━━━━━━\n");
    root = avl_delete(root, 70);
    avl_print(root);
    
    printf("Deleting 60:\n");
    printf("━━━━━━━━━━━━━\n");
    root = avl_delete(root, 60);
    avl_print(root);
    
    printf("Deleting 50 (root with two children):\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    root = avl_delete(root, 50);
    avl_print(root);
    
    printf("Deletions complete. Rotations during deletion: %d\n", rotation_count);
    printf("Tree is still valid: %s\n", avl_is_valid(root) ? "Yes" : "No");
    
    avl_destroy(root);
}

void demo_part7_worst_case(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 7: Worst Case Comparison (Sorted Input)             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    AVLNode *root = NULL;
    rotation_count = 0;
    comparison_count = 0;
    
    printf("Inserting sorted sequence 1-15:\n");
    printf("(This would create a degenerate BST without balancing)\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    
    for (int i = 1; i <= 15; i++) {
        root = avl_insert(root, i);
    }
    
    avl_print(root);
    
    printf("Statistics:\n");
    printf("  Nodes: %d\n", avl_size(root));
    printf("  Height: %d (vs %d for unbalanced BST)\n", 
           avl_height(root), 14);
    printf("  Rotations performed: %d\n", rotation_count);
    printf("  Height bound: 1.44 × log₂(15) ≈ 5.6\n");
    
    /* Search performance */
    comparison_count = 0;
    printf("\nSearching for key 8:\n");
    AVLNode *found = avl_search(root, 8);
    printf("  Found: %s\n", found ? "Yes" : "No");
    printf("  Comparisons: %d (vs up to 15 in unbalanced BST)\n", comparison_count);
    
    avl_destroy(root);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     WEEK 09: AVL TREES - Complete Demonstration               ║\n");
    printf("║     Algorithms and Programming Techniques                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    demo_part1_basic_operations();
    demo_part2_ll_rotation();
    demo_part3_rr_rotation();
    demo_part4_lr_rotation();
    demo_part5_rl_rotation();
    demo_part6_deletion();
    demo_part7_worst_case();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    Demonstration Complete                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

/* =============================================================================
 * END OF FILE
 * =============================================================================
 */

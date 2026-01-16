/**
 * =============================================================================
 * EXERCISE 1: Basic BST Operations
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement a complete Binary Search Tree supporting fundamental operations
 *   including insertion, search, traversals and basic tree management.
 *
 * REQUIREMENTS:
 *   1. Define a BST node structure with key, left and right pointers
 *   2. Implement node creation with proper memory allocation
 *   3. Implement iterative search that returns the node or NULL
 *   4. Implement recursive insertion maintaining BST property
 *   5. Implement all three traversals: in-order, pre-order, post-order
 *   6. Implement find minimum and find maximum functions
 *   7. Implement tree deallocation to prevent memory leaks
 *   8. Create a validation function to verify the BST property
 *
 * EXAMPLE INPUT:
 *   8
 *   INSERT 50
 *   INSERT 30
 *   INSERT 70
 *   INSERT 20
 *   INSERT 40
 *   SEARCH 40
 *   INORDER
 *   MIN
 *
 * EXPECTED OUTPUT:
 *   Inserted: 50
 *   Inserted: 30
 *   Inserted: 70
 *   Inserted: 20
 *   Inserted: 40
 *   Found: 40
 *   In-order: 20 30 40 50 70
 *   Minimum: 20
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
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

/**
 * TODO 1: Define the BST node structure
 *
 * Create a structure named BSTNode containing:
 *   - An integer field called 'key' to store the node's value
 *   - A pointer to struct BSTNode called 'left' for the left child
 *   - A pointer to struct BSTNode called 'right' for the right child
 *
 * Use typedef to create an alias 'BSTNode' for the structure.
 *
 * Hint: Remember to use 'struct BSTNode' for the pointer types since
 *       the typedef isn't complete yet within the structure definition.
 */

typedef struct BSTNode {
    int key;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;



/* =============================================================================
 * NODE CREATION
 * =============================================================================
 */

/**
 * TODO 2: Implement node creation function
 *
 * Create a function: BSTNode *bst_create_node(int key)
 *
 * This function should:
 *   1. Allocate memory for a new BSTNode using malloc
 *   2. Check if allocation succeeded (return NULL if it failed)
 *   3. Initialise the key field with the given key parameter
 *   4. Set both left and right pointers to NULL
 *   5. Return the pointer to the new node
 *
 * @param key The value to store in the new node
 * @return Pointer to the new node, or NULL if allocation fails
 */

BSTNode *bst_create_node(int key) {
    BSTNode *node = (BSTNode *)malloc(sizeof(BSTNode));
    if (node == NULL) {
        /* Allocation failure is unrecoverable for this exercise. */
        return NULL;
    }
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    return node;
}



/* =============================================================================
 * SEARCH OPERATION
 * =============================================================================
 */

/**
 * TODO 3: Implement iterative search
 *
 * Create a function: BSTNode *bst_search(BSTNode *root, int key)
 *
 * This function should:
 *   1. Start at the root node
 *   2. While the current node is not NULL:
 *      a. If key equals current node's key, return current node
 *      b. If key is less than current node's key, go left
 *      c. Otherwise, go right
 *   3. If loop ends, return NULL (key not found)
 *
 * Use iterative approach (while loop), not recursion.
 *
 * @param root The root of the BST
 * @param key The key to search for
 * @return Pointer to the node if found, NULL otherwise
 */

BSTNode *bst_search(BSTNode *root, int key) {
    BSTNode *current = root;

    while (current != NULL) {
        if (key == current->key) {
            return current;
        }
        if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    return NULL;
}



/* =============================================================================
 * INSERTION
 * =============================================================================
 */

/**
 * TODO 4: Implement recursive insertion
 *
 * Create a function: BSTNode *bst_insert(BSTNode *root, int key)
 *
 * This function should:
 *   1. Base case: if root is NULL, create and return a new node
 *   2. If key is less than root's key, recursively insert into left subtree
 *   3. If key is greater than root's key, recursively insert into right subtree
 *   4. If key equals root's key, do nothing (ignore duplicates)
 *   5. Return the root pointer
 *
 * Important: Update left/right pointers with the result of recursive calls!
 *   Example: root->left = bst_insert(root->left, key);
 *
 * @param root The root of the BST (may be NULL for empty tree)
 * @param key The key to insert
 * @return The root of the tree (may be new if tree was empty)
 */

BSTNode *bst_insert(BSTNode *root, int key) {
    if (root == NULL) {
        return bst_create_node(key);
    }

    if (key < root->key) {
        root->left = bst_insert(root->left, key);
    } else if (key > root->key) {
        root->right = bst_insert(root->right, key);
    }

    /* Duplicate keys are ignored. */
    return root;
}



/* =============================================================================
 * TRAVERSALS
 * =============================================================================
 */

/**
 * TODO 5: Implement in-order traversal
 *
 * Create a function: void bst_inorder(BSTNode *root)
 *
 * In-order traversal visits nodes in the order: Left, Node, Right
 * For a BST, this produces keys in sorted (ascending) order.
 *
 * Steps:
 *   1. Base case: if root is NULL, return immediately
 *   2. Recursively traverse the left subtree
 *   3. Print the current node's key followed by a space
 *   4. Recursively traverse the right subtree
 *
 * @param root The root of the BST
 */

void bst_inorder(BSTNode *root) {
    if (root == NULL) {
        return;
    }

    bst_inorder(root->left);
    printf("%d ", root->key);
    bst_inorder(root->right);
}



/**
 * TODO 6: Implement pre-order and post-order traversals
 *
 * Create two functions:
 *   void bst_preorder(BSTNode *root)  - visits: Node, Left, Right
 *   void bst_postorder(BSTNode *root) - visits: Left, Right, Node
 *
 * Follow the same pattern as in-order, but change the order of operations.
 *
 * Pre-order: print first, then recurse left, then recurse right
 * Post-order: recurse left, recurse right, then print
 */

void bst_preorder(BSTNode *root) {
    if (root == NULL) {
        return;
    }

    printf("%d ", root->key);
    bst_preorder(root->left);
    bst_preorder(root->right);
}

void bst_postorder(BSTNode *root) {
    if (root == NULL) {
        return;
    }

    bst_postorder(root->left);
    bst_postorder(root->right);
    printf("%d ", root->key);
}



/* =============================================================================
 * FIND MINIMUM AND MAXIMUM
 * =============================================================================
 */

/**
 * TODO 7: Implement find minimum and find maximum
 *
 * Create two functions:
 *   BSTNode *bst_find_min(BSTNode *root)
 *   BSTNode *bst_find_max(BSTNode *root)
 *
 * For bst_find_min:
 *   - The minimum value is always in the leftmost node
 *   - Keep following left pointers until you reach NULL
 *   - Return the last non-NULL node encountered
 *
 * For bst_find_max:
 *   - The maximum value is always in the rightmost node
 *   - Keep following right pointers until you reach NULL
 *   - Return the last non-NULL node encountered
 *
 * Both functions should return NULL if the tree is empty.
 *
 * @param root The root of the BST
 * @return Pointer to the node with min/max key, or NULL if empty
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

BSTNode *bst_find_max(BSTNode *root) {
    if (root == NULL) {
        return NULL;
    }

    while (root->right != NULL) {
        root = root->right;
    }

    return root;
}



/* =============================================================================
 * MEMORY CLEANUP
 * =============================================================================
 */

/**
 * TODO 8: Implement tree deallocation
 *
 * Create a function: void bst_free(BSTNode *root)
 *
 * This function must use POST-ORDER traversal to free all nodes:
 *   1. Base case: if root is NULL, return
 *   2. Recursively free the left subtree
 *   3. Recursively free the right subtree
 *   4. Free the current node
 *
 * IMPORTANT: Must be post-order! If you free the parent before children,
 * you lose the pointers to the children and cause memory leaks.
 *
 * @param root The root of the BST to deallocate
 */

void bst_free(BSTNode *root) {
    if (root == NULL) {
        return;
    }

    bst_free(root->left);
    bst_free(root->right);
    free(root);
}



/* =============================================================================
 * BST VALIDATION (BONUS)
 * =============================================================================
 */

/**
 * TODO 9 (BONUS): Implement BST validation
 *
 * Create a helper function and main function:
 *   bool bst_is_valid_helper(BSTNode *node, int min, int max)
 *   bool bst_is_valid(BSTNode *root)
 *
 * The helper function checks if all nodes fall within valid range:
 *   1. If node is NULL, return true (empty tree is valid)
 *   2. If node's key is <= min or >= max, return false
 *   3. Recursively check left subtree with range [min, node->key]
 *   4. Recursively check right subtree with range [node->key, max]
 *   5. Return true only if both subtrees are valid
 *
 * The main function calls helper with initial range [INT_MIN, INT_MAX]
 *
 * Hint: #include <limits.h> for INT_MIN and INT_MAX
 */

static bool bst_is_valid_helper(BSTNode *node, int min, int max) {
    if (node == NULL) {
        return true;
    }

    if (node->key <= min || node->key >= max) {
        return false;
    }

    return bst_is_valid_helper(node->left, min, node->key) &&
           bst_is_valid_helper(node->right, node->key, max);
}

bool bst_is_valid(BSTNode *root) {
    return bst_is_valid_helper(root, INT_MIN, INT_MAX);
}



/* =============================================================================
 * MAIN PROGRAM - DO NOT MODIFY
 * =============================================================================
 */

int main(void) {
    BSTNode *root = NULL;
    char command[20];
    int key, n;
    
    /* Read number of operations */
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "Error reading number of operations\n");
        return 1;
    }
    
    /* Process each operation */
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
        else {
            printf("Unknown command: %s\n", command);
        }
    }
    
    /* Clean up */
    bst_free(root);
    
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * After completing the required TODOs, try these additional challenges:
 *
 * 1. Implement bst_count_nodes(BSTNode *root) to count total nodes
 *
 * 2. Implement bst_count_leaves(BSTNode *root) to count leaf nodes only
 *
 * 3. Implement bst_height(BSTNode *root) to calculate tree height
 *
 * 4. Implement bst_search_recursive(BSTNode *root, int key) using recursion
 *
 * 5. Add support for a VALIDATE command that checks if the tree is a valid BST
 *
 * =============================================================================
 */

/**
 * =============================================================================
 * EXERCISE 2: BST Deletion and Advanced Operations
 * =============================================================================
 *
 * OBJECTIVE:
 *   Extend BST functionality with deletion and advanced operations including
 *   range queries, kth smallest element, lowest common ancestor and more.
 *
 * REQUIREMENTS:
 *   1. Implement deletion handling all three cases (leaf, one child, two children)
 *   2. Implement tree height calculation
 *   3. Implement node counting (total and leaves)
 *   4. Implement range search to find all keys in [low, high]
 *   5. Implement kth smallest element finder
 *   6. Implement lowest common ancestor
 *   7. Implement visual tree printing
 *   8. Ensure all operations maintain proper memory management
 *
 * EXAMPLE INPUT:
 *   50 30 70 20 40 60 80
 *   8
 *   HEIGHT
 *   COUNT
 *   DELETE 30
 *   INORDER
 *   RANGE 25 65
 *   KTH 3
 *   LCA 20 40
 *   PRINT
 *
 * EXPECTED OUTPUT:
 *   Height: 2
 *   Node count: 7
 *   Deleted: 30
 *   In-order: 20 40 50 60 70 80
 *   Range [25, 65]: 40 50 60
 *   3rd smallest: 50
 *   LCA of 20 and 40: 40
 *   [visual tree output]
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

/* =============================================================================
 * TYPE DEFINITIONS (PROVIDED)
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
 * FIND MINIMUM (NEEDED FOR DELETION)
 * =============================================================================
 */

/**
 * TODO 1: Implement find minimum
 *
 * Create a function: BSTNode *bst_find_min(BSTNode *root)
 *
 * The minimum value in a BST is always in the leftmost node.
 * Keep following left pointers until you reach a node with no left child.
 *
 * This function is essential for deletion (Case 3: two children).
 *
 * @param root The root of the BST
 * @return Pointer to the node with minimum key, or NULL if empty
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
 * DELETION
 * =============================================================================
 */

/**
 * TODO 2: Implement BST deletion
 *
 * Create a function: BSTNode *bst_delete(BSTNode *root, int key)
 *
 * Deletion has THREE cases to handle:
 *
 * CASE 1: Node is a LEAF (no children)
 *   - Simply free the node and return NULL
 *
 * CASE 2: Node has ONE child
 *   - Save pointer to the child
 *   - Free the node
 *   - Return the child pointer (it takes the deleted node's place)
 *
 * CASE 3: Node has TWO children
 *   - Find the in-order successor (smallest in right subtree)
 *   - Copy successor's key to current node
 *   - Recursively delete the successor from right subtree
 *
 * Algorithm:
 *   1. If root is NULL, return NULL (key not found)
 *   2. If key < root->key, delete from left subtree
 *   3. If key > root->key, delete from right subtree
 *   4. If key == root->key, handle the three cases above
 *   5. Return the (possibly updated) root
 *
 * IMPORTANT: Don't forget to free() memory in cases 1 and 2!
 *
 * @param root The root of the BST
 * @param key The key to delete
 * @return The root of the tree (may change if root was deleted)
 */

BSTNode *bst_delete(BSTNode *root, int key) {
    if (root == NULL) {
        return NULL;
    }

    if (key < root->key) {
        root->left = bst_delete(root->left, key);
        return root;
    }

    if (key > root->key) {
        root->right = bst_delete(root->right, key);
        return root;
    }

    /* Found the node to delete. */

    /* Case 1 and Case 2a: no left child (includes leaf). */
    if (root->left == NULL) {
        BSTNode *temp = root->right;
        free(root);
        return temp;
    }

    /* Case 2b: no right child. */
    if (root->right == NULL) {
        BSTNode *temp = root->left;
        free(root);
        return temp;
    }

    /* Case 3: two children. Replace with in-order successor. */
    BSTNode *successor = bst_find_min(root->right);
    root->key = successor->key;
    root->right = bst_delete(root->right, successor->key);

    return root;
}


/* =============================================================================
 * TREE HEIGHT
 * =============================================================================
 */

/**
 * TODO 3: Implement tree height calculation
 *
 * Create a function: int bst_height(BSTNode *root)
 *
 * Height is the length of the longest path from root to any leaf.
 * Convention: empty tree has height -1, single node has height 0.
 *
 * Algorithm:
 *   1. If root is NULL, return -1
 *   2. Recursively get height of left subtree
 *   3. Recursively get height of right subtree
 *   4. Return 1 + maximum of left and right heights
 *
 * @param root The root of the BST
 * @return Height of the tree (-1 for empty tree)
 */

int bst_height(BSTNode *root) {
    if (root == NULL) {
        return -1;
    }

    int left_h = bst_height(root->left);
    int right_h = bst_height(root->right);

    return 1 + (left_h > right_h ? left_h : right_h);
}


/* =============================================================================
 * NODE COUNTING
 * =============================================================================
 */

/**
 * TODO 4: Implement node counting functions
 *
 * Create two functions:
 *   int bst_count_nodes(BSTNode *root)  - counts ALL nodes
 *   int bst_count_leaves(BSTNode *root) - counts only LEAF nodes
 *
 * For bst_count_nodes:
 *   - If root is NULL, return 0
 *   - Return 1 + count(left) + count(right)
 *
 * For bst_count_leaves:
 *   - If root is NULL, return 0
 *   - If root has no children (leaf), return 1
 *   - Otherwise, return count_leaves(left) + count_leaves(right)
 *
 * @param root The root of the BST
 * @return Count of nodes/leaves
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


/* =============================================================================
 * RANGE SEARCH
 * =============================================================================
 */

/**
 * TODO 5: Implement range search
 *
 * Create a function: void bst_range_search(BSTNode *root, int low, int high)
 *
 * Find and print all keys k where low <= k <= high.
 * Use the BST property to prune unnecessary branches:
 *   - Only search left subtree if root->key > low
 *   - Only search right subtree if root->key < high
 *
 * Algorithm (in-order style for sorted output):
 *   1. If root is NULL, return
 *   2. If root->key > low, search left subtree
 *   3. If low <= root->key <= high, print root->key
 *   4. If root->key < high, search right subtree
 *
 * @param root The root of the BST
 * @param low Lower bound of range (inclusive)
 * @param high Upper bound of range (inclusive)
 */

void bst_range_search(BSTNode *root, int low, int high) {
    if (root == NULL) {
        return;
    }

    if (root->key > low) {
        bst_range_search(root->left, low, high);
    }

    if (root->key >= low && root->key <= high) {
        printf("%d ", root->key);
    }

    if (root->key < high) {
        bst_range_search(root->right, low, high);
    }
}


/* =============================================================================
 * KTH SMALLEST ELEMENT
 * =============================================================================
 */

/**
 * TODO 6: Implement kth smallest element finder
 *
 * Create a helper and main function:
 *   void bst_kth_helper(BSTNode *root, int k, int *count, int *result)
 *   int bst_kth_smallest(BSTNode *root, int k)
 *
 * Use in-order traversal (visits nodes in sorted order).
 * Keep a counter that increments each time you visit a node.
 * When counter equals k, you've found the kth smallest.
 *
 * Helper function approach:
 *   1. Traverse left subtree (if result not yet found)
 *   2. Increment count
 *   3. If count == k, set result to current node's key
 *   4. Traverse right subtree (if result not yet found)
 *
 * The main function initialises count=0, result=-1, calls helper, returns result.
 *
 * @param root The root of the BST
 * @param k Which smallest element to find (1 = smallest, 2 = second smallest, etc.)
 * @return The kth smallest key, or -1 if k is invalid
 */

static void bst_kth_helper(BSTNode *root, int k, int *count, int *result) {
    if (root == NULL || *result != -1) {
        return;
    }

    bst_kth_helper(root->left, k, count, result);

    (*count)++;
    if (*count == k) {
        *result = root->key;
        return;
    }

    bst_kth_helper(root->right, k, count, result);
}

int bst_kth_smallest(BSTNode *root, int k) {
    if (k <= 0) {
        return -1;
    }

    int count = 0;
    int result = -1;
    bst_kth_helper(root, k, &count, &result);
    return result;
}


/* =============================================================================
 * LOWEST COMMON ANCESTOR
 * =============================================================================
 */

/**
 * TODO 7: Implement lowest common ancestor (LCA)
 *
 * Create a function: BSTNode *bst_lca(BSTNode *root, int key1, int key2)
 *
 * The LCA is the deepest node that has both key1 and key2 as descendants.
 * For BST, this is where the search paths for key1 and key2 diverge.
 *
 * Algorithm:
 *   1. If root is NULL, return NULL
 *   2. If both keys are smaller than root, LCA is in left subtree
 *   3. If both keys are larger than root, LCA is in right subtree
 *   4. Otherwise, current root is the LCA (paths diverge here)
 *
 * Note: This assumes both keys exist in the tree.
 *
 * @param root The root of the BST
 * @param key1 First key
 * @param key2 Second key
 * @return Pointer to the LCA node, or NULL if tree is empty
 */

BSTNode *bst_lca(BSTNode *root, int key1, int key2) {
    if (root == NULL) {
        return NULL;
    }

    /* Normalise ordering so key1 <= key2. */
    if (key1 > key2) {
        int tmp = key1;
        key1 = key2;
        key2 = tmp;
    }

    if (key2 < root->key) {
        return bst_lca(root->left, key1, key2);
    }

    if (key1 > root->key) {
        return bst_lca(root->right, key1, key2);
    }

    return root;
}


/* =============================================================================
 * VISUAL TREE PRINTING
 * =============================================================================
 */

/**
 * TODO 8: Implement visual tree printing
 *
 * Create helper and main functions:
 *   void bst_print_helper(BSTNode *root, int space, int indent)
 *   void bst_print_tree(BSTNode *root)
 *
 * Print the tree rotated 90 degrees (right subtree on top).
 * Use indentation to show tree structure.
 *
 * Helper algorithm:
 *   1. If root is NULL, return
 *   2. Increase space by indent amount
 *   3. Recursively print right subtree first
 *   4. Print newline
 *   5. Print 'space' number of spaces
 *   6. Print root's key
 *   7. Recursively print left subtree
 *
 * Main function:
 *   - If root is NULL, print "(empty tree)"
 *   - Otherwise call helper with space=0, indent=5
 *   - Print final newline
 *
 * Example output for tree with root 50:
 *          80
 *     70
 *          60
 * 50
 *          40
 *     30
 *          20
 *
 * @param root The root of the BST
 */

static void bst_print_helper(BSTNode *root, int space, int indent) {
    if (root == NULL) {
        return;
    }

    space += indent;

    bst_print_helper(root->right, space, indent);

    printf("\n");
    for (int i = indent; i < space; i++) {
        printf(" ");
    }
    printf("%d", root->key);

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
 * BST VALIDATION
 * =============================================================================
 */

/**
 * TODO 9: Implement BST validation
 *
 * Create functions:
 *   bool bst_is_valid_helper(BSTNode *node, int min, int max)
 *   bool bst_is_valid(BSTNode *root)
 *
 * A valid BST has all left descendants < node < all right descendants.
 * Use range checking: each node must fall within [min, max].
 *
 * Helper algorithm:
 *   1. If node is NULL, return true
 *   2. If node->key <= min OR node->key >= max, return false
 *   3. Recursively check left subtree with range [min, node->key]
 *   4. Recursively check right subtree with range [node->key, max]
 *   5. Return true only if both subtrees are valid
 *
 * Main function calls helper with [INT_MIN, INT_MAX].
 *
 * @param root The root of the BST
 * @return true if valid BST, false otherwise
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
 * TREE MIRRORING (BONUS)
 * =============================================================================
 */

/**
 * TODO 10 (BONUS): Implement tree mirroring
 *
 * Create a function: void bst_mirror(BSTNode *root)
 *
 * Mirror the tree by swapping left and right children at every node.
 * Note: After mirroring, the tree is NO LONGER a valid BST!
 *
 * Algorithm:
 *   1. If root is NULL, return
 *   2. Swap root->left and root->right
 *   3. Recursively mirror left subtree
 *   4. Recursively mirror right subtree
 *
 * @param root The root of the tree to mirror
 */

void bst_mirror(BSTNode *root) {
    if (root == NULL) {
        return;
    }

    BSTNode *temp = root->left;
    root->left = root->right;
    root->right = temp;

    bst_mirror(root->left);
    bst_mirror(root->right);
}


/* =============================================================================
 * PATH SUM (BONUS)
 * =============================================================================
 */

/**
 * TODO 11 (BONUS): Implement root-to-leaf path sum check
 *
 * Create a function: bool bst_has_path_sum(BSTNode *root, int target)
 *
 * Return true if there exists a root-to-leaf path where the sum
 * of all keys equals the target value.
 *
 * Algorithm:
 *   1. If root is NULL, return false
 *   2. Subtract root->key from target
 *   3. If root is a leaf AND target is now 0, return true
 *   4. Recursively check left OR right subtree with new target
 *
 * @param root The root of the BST
 * @param target The target sum to find
 * @return true if such a path exists, false otherwise
 */

bool bst_has_path_sum(BSTNode *root, int target) {
    if (root == NULL) {
        return false;
    }

    target -= root->key;

    /* If this is a leaf, the path sum condition reduces to target == 0. */
    if (root->left == NULL && root->right == NULL) {
        return target == 0;
    }

    return bst_has_path_sum(root->left, target) || bst_has_path_sum(root->right, target);
}


/* =============================================================================
 * SERIALISATION (BONUS)
 * =============================================================================
 */

/**
 * TODO 12 (BONUS): Implement tree serialisation
 *
 * Create functions:
 *   void bst_serialize(BSTNode *root, FILE *fp)
 *   BSTNode *bst_deserialize(FILE *fp)
 *
 * Serialise: Write tree to file using pre-order traversal.
 *   - Use a special marker (e.g., INT_MIN) for NULL nodes.
 *
 * Deserialise: Read from file and reconstruct the tree.
 *   - Read values in the same order they were written.
 *
 * This allows saving and loading BST structures.
 */

void bst_serialize(BSTNode *root, FILE *fp) {
    if (fp == NULL) {
        return;
    }

    /*
     * Serialise using pre-order traversal.
     *
     * We write INT_MIN as a null marker. This is convenient for teaching but it
     * assumes that INT_MIN is not used as a legitimate key in the tree.
     */
    if (root == NULL) {
        fprintf(fp, "%d ", INT_MIN);
        return;
    }

    fprintf(fp, "%d ", root->key);
    bst_serialize(root->left, fp);
    bst_serialize(root->right, fp);
}

BSTNode *bst_deserialize(FILE *fp) {
    if (fp == NULL) {
        return NULL;
    }

    int value;
    if (fscanf(fp, "%d", &value) != 1) {
        return NULL;
    }

    if (value == INT_MIN) {
        return NULL;
    }

    BSTNode *node = bst_create_node(value);
    if (node == NULL) {
        return NULL;
    }

    node->left = bst_deserialize(fp);
    node->right = bst_deserialize(fp);

    return node;
}


/* =============================================================================
 * MAIN PROGRAM - DO NOT MODIFY
 * =============================================================================
 */

int main(void) {
    BSTNode *root = NULL;
    char line[256];
    char command[20];
    int key, key2, n;
    
    /* Read initial tree values (space-separated on first line) */
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
        else {
            printf("Unknown command: %s\n", command);
        }
    }
    
    /* Clean up */
    bst_free(root);
    
    return 0;
}

/* =============================================================================
 * ADDITIONAL CHALLENGES
 * =============================================================================
 *
 * For extra practice, implement these advanced operations:
 *
 * 1. bst_floor(root, key) - Largest key <= given key
 * 2. bst_ceiling(root, key) - Smallest key >= given key
 * 3. bst_rank(root, key) - Number of keys less than given key
 * 4. bst_delete_range(root, low, high) - Delete all keys in range
 * 5. bst_build_balanced(arr, n) - Build balanced BST from sorted array
 *
 * =============================================================================
 */

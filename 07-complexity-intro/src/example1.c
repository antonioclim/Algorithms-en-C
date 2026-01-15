/**
 * =============================================================================
 * WEEK 07: BINARY TREES
 * Complete Working Example
 * =============================================================================
 *
 * This comprehensive example demonstrates:
 *   1. Binary tree node structure and creation
 *   2. All three traversal algorithms (preorder, inorder, postorder)
 *   3. Level-order (BFS) traversal using a queue
 *   4. Tree statistics (height, node count, leaf count, sum)
 *   5. Tree visualisation (rotated printing)
 *   6. Tree mirroring/inversion
 *   7. Proper memory deallocation
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Usage: ./example1
 *
 * =============================================================================
 * Algorithms and Programming Techniques
 * ASE-CSIE, Bucharest
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTS AND MACROS
 * =============================================================================
 */

#define MAX_QUEUE_SIZE 100
#define INDENT_SPACES 4

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * Binary Tree Node Structure
 * 
 * Each node contains:
 *   - data: the integer value stored in the node
 *   - left: pointer to the left child (NULL if none)
 *   - right: pointer to the right child (NULL if none)
 */
typedef struct TreeNode {
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

/**
 * Simple Queue for Level-Order Traversal
 * 
 * Implemented as a circular buffer for efficiency.
 */
typedef struct Queue {
    TreeNode *items[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int count;
} Queue;

/* =============================================================================
 * FORWARD DECLARATIONS
 * =============================================================================
 */

void free_tree(TreeNode *node);

/* =============================================================================
 * QUEUE OPERATIONS (for Level-Order Traversal)
 * =============================================================================
 */

/**
 * Create and initialise an empty queue.
 */
Queue* queue_create(void) {
    Queue *q = malloc(sizeof(Queue));
    if (q == NULL) {
        fprintf(stderr, "Error: Failed to allocate queue\n");
        exit(EXIT_FAILURE);
    }
    q->front = 0;
    q->rear = -1;
    q->count = 0;
    return q;
}

/**
 * Check if queue is empty.
 */
bool queue_is_empty(Queue *q) {
    return q->count == 0;
}

/**
 * Check if queue is full.
 */
bool queue_is_full(Queue *q) {
    return q->count == MAX_QUEUE_SIZE;
}

/**
 * Add a node to the rear of the queue.
 */
void queue_enqueue(Queue *q, TreeNode *node) {
    if (queue_is_full(q)) {
        fprintf(stderr, "Error: Queue overflow\n");
        return;
    }
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->items[q->rear] = node;
    q->count++;
}

/**
 * Remove and return the node at the front of the queue.
 */
TreeNode* queue_dequeue(Queue *q) {
    if (queue_is_empty(q)) {
        return NULL;
    }
    TreeNode *node = q->items[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->count--;
    return node;
}

/**
 * Free the queue structure.
 */
void queue_destroy(Queue *q) {
    free(q);
}

/* =============================================================================
 * PART 1: NODE CREATION AND BASIC OPERATIONS
 * =============================================================================
 */

/**
 * Create a new tree node with the given value.
 * 
 * @param value The integer value to store in the node
 * @return Pointer to the newly created node
 * 
 * Memory is allocated dynamically. The caller is responsible for
 * eventually freeing this memory (typically via free_tree).
 */
TreeNode* create_node(int value) {
    TreeNode *node = malloc(sizeof(TreeNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for node\n");
        exit(EXIT_FAILURE);
    }
    node->data = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/**
 * Build a sample binary tree for demonstration.
 * 
 * Tree structure:
 *              10
 *            /    \
 *           5      15
 *          / \    /  \
 *         3   7  12   20
 *        /     \
 *       1       8
 * 
 * @return Pointer to the root of the constructed tree
 */
TreeNode* build_sample_tree(void) {
    /* Create all nodes */
    TreeNode *root = create_node(10);
    
    /* Level 1 */
    root->left = create_node(5);
    root->right = create_node(15);
    
    /* Level 2 */
    root->left->left = create_node(3);
    root->left->right = create_node(7);
    root->right->left = create_node(12);
    root->right->right = create_node(20);
    
    /* Level 3 */
    root->left->left->left = create_node(1);
    root->left->right->right = create_node(8);
    
    return root;
}

void demo_part1(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: Node Creation and Tree Construction              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Creating a sample binary tree:\n\n");
    printf("              10\n");
    printf("            /    \\\n");
    printf("           5      15\n");
    printf("          / \\    /  \\\n");
    printf("         3   7  12   20\n");
    printf("        /     \\\n");
    printf("       1       8\n\n");
    
    printf("Tree created successfully using create_node() function.\n");
    printf("Each node contains: data (int), left pointer, right pointer.\n");
    printf("Size of TreeNode: %zu bytes\n", sizeof(TreeNode));
}

/* =============================================================================
 * PART 2: RECURSIVE TRAVERSALS
 * =============================================================================
 */

/**
 * Preorder traversal: Node -> Left -> Right
 * 
 * Process the current node BEFORE its children.
 * Used for: creating a copy of the tree, prefix expression notation.
 * 
 * @param node The current node being visited
 */
void preorder(TreeNode *node) {
    if (node == NULL) {
        return;  /* Base case: empty tree */
    }
    
    printf("%d ", node->data);  /* Process node FIRST */
    preorder(node->left);        /* Then left subtree */
    preorder(node->right);       /* Finally right subtree */
}

/**
 * Inorder traversal: Left -> Node -> Right
 * 
 * Process the current node BETWEEN its children.
 * For BST: produces elements in sorted order.
 * 
 * @param node The current node being visited
 */
void inorder(TreeNode *node) {
    if (node == NULL) {
        return;  /* Base case: empty tree */
    }
    
    inorder(node->left);         /* First left subtree */
    printf("%d ", node->data);   /* Process node IN MIDDLE */
    inorder(node->right);        /* Finally right subtree */
}

/**
 * Postorder traversal: Left -> Right -> Node
 * 
 * Process the current node AFTER its children.
 * Used for: deleting/freeing tree, postfix expression evaluation.
 * 
 * @param node The current node being visited
 */
void postorder(TreeNode *node) {
    if (node == NULL) {
        return;  /* Base case: empty tree */
    }
    
    postorder(node->left);       /* First left subtree */
    postorder(node->right);      /* Then right subtree */
    printf("%d ", node->data);   /* Process node LAST */
}

void demo_part2(TreeNode *root) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: Recursive Traversals                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Three fundamental traversal algorithms:\n\n");
    
    printf("1. PREORDER (Node -> Left -> Right):\n");
    printf("   ");
    preorder(root);
    printf("\n");
    printf("   Use: Copy tree, serialisation, prefix notation\n\n");
    
    printf("2. INORDER (Left -> Node -> Right):\n");
    printf("   ");
    inorder(root);
    printf("\n");
    printf("   Use: BST sorted output, infix notation\n\n");
    
    printf("3. POSTORDER (Left -> Right -> Node):\n");
    printf("   ");
    postorder(root);
    printf("\n");
    printf("   Use: Delete tree, expression evaluation, postfix notation\n");
}

/* =============================================================================
 * PART 3: LEVEL-ORDER (BFS) TRAVERSAL
 * =============================================================================
 */

/**
 * Level-order traversal using BFS (Breadth-First Search).
 * 
 * Visits nodes level by level, from left to right.
 * Requires a queue data structure.
 * 
 * @param root The root of the tree to traverse
 */
void level_order(TreeNode *root) {
    if (root == NULL) {
        return;
    }
    
    Queue *q = queue_create();
    queue_enqueue(q, root);
    
    while (!queue_is_empty(q)) {
        TreeNode *current = queue_dequeue(q);
        printf("%d ", current->data);
        
        /* Enqueue children for later processing */
        if (current->left != NULL) {
            queue_enqueue(q, current->left);
        }
        if (current->right != NULL) {
            queue_enqueue(q, current->right);
        }
    }
    
    queue_destroy(q);
}

/**
 * Level-order traversal with level separators.
 * 
 * Shows which nodes belong to which level.
 * 
 * @param root The root of the tree to traverse
 */
void level_order_by_level(TreeNode *root) {
    if (root == NULL) {
        return;
    }
    
    Queue *q = queue_create();
    queue_enqueue(q, root);
    int level = 0;
    
    while (!queue_is_empty(q)) {
        int level_size = q->count;
        printf("   Level %d: ", level);
        
        for (int i = 0; i < level_size; i++) {
            TreeNode *current = queue_dequeue(q);
            printf("%d ", current->data);
            
            if (current->left != NULL) {
                queue_enqueue(q, current->left);
            }
            if (current->right != NULL) {
                queue_enqueue(q, current->right);
            }
        }
        printf("\n");
        level++;
    }
    
    queue_destroy(q);
}

void demo_part3(TreeNode *root) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: Level-Order (BFS) Traversal                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Level-order traversal visits nodes level by level:\n\n");
    
    printf("1. Simple level-order:\n");
    printf("   ");
    level_order(root);
    printf("\n\n");
    
    printf("2. Level-order with level indicators:\n");
    level_order_by_level(root);
    
    printf("\n");
    printf("Note: Level-order requires a QUEUE (FIFO) data structure,\n");
    printf("      unlike recursive traversals which use the call stack.\n");
}

/* =============================================================================
 * PART 4: TREE STATISTICS
 * =============================================================================
 */

/**
 * Calculate the height of the tree.
 * 
 * Height = longest path from root to any leaf.
 * Empty tree has height -1 (convention).
 * 
 * @param node The root of the tree/subtree
 * @return Height of the tree (-1 if empty)
 */
int tree_height(TreeNode *node) {
    if (node == NULL) {
        return -1;  /* Empty tree */
    }
    
    int left_height = tree_height(node->left);
    int right_height = tree_height(node->right);
    
    /* Return 1 + maximum of subtree heights */
    if (left_height > right_height) {
        return 1 + left_height;
    } else {
        return 1 + right_height;
    }
}

/**
 * Count total number of nodes in the tree.
 * 
 * @param node The root of the tree/subtree
 * @return Total node count
 */
int count_nodes(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }
    return 1 + count_nodes(node->left) + count_nodes(node->right);
}

/**
 * Count leaf nodes (nodes with no children).
 * 
 * @param node The root of the tree/subtree
 * @return Number of leaf nodes
 */
int count_leaves(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }
    if (node->left == NULL && node->right == NULL) {
        return 1;  /* This is a leaf */
    }
    return count_leaves(node->left) + count_leaves(node->right);
}

/**
 * Count internal nodes (nodes with at least one child).
 * 
 * @param node The root of the tree/subtree
 * @return Number of internal nodes
 */
int count_internal(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }
    if (node->left == NULL && node->right == NULL) {
        return 0;  /* Leaves are not internal */
    }
    return 1 + count_internal(node->left) + count_internal(node->right);
}

/**
 * Calculate sum of all node values.
 * 
 * @param node The root of the tree/subtree
 * @return Sum of all values
 */
int tree_sum(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }
    return node->data + tree_sum(node->left) + tree_sum(node->right);
}

/**
 * Find the minimum value in the tree.
 * 
 * @param node The root of the tree/subtree
 * @return Minimum value (or INT_MAX if empty)
 */
int tree_min(TreeNode *node) {
    if (node == NULL) {
        return __INT_MAX__;
    }
    
    int min_val = node->data;
    int left_min = tree_min(node->left);
    int right_min = tree_min(node->right);
    
    if (left_min < min_val) min_val = left_min;
    if (right_min < min_val) min_val = right_min;
    
    return min_val;
}

/**
 * Find the maximum value in the tree.
 * 
 * @param node The root of the tree/subtree
 * @return Maximum value (or INT_MIN if empty)
 */
int tree_max(TreeNode *node) {
    if (node == NULL) {
        return -__INT_MAX__ - 1;  /* INT_MIN */
    }
    
    int max_val = node->data;
    int left_max = tree_max(node->left);
    int right_max = tree_max(node->right);
    
    if (left_max > max_val) max_val = left_max;
    if (right_max > max_val) max_val = right_max;
    
    return max_val;
}

void demo_part4(TreeNode *root) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: Tree Statistics                                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Computing various tree statistics:\n\n");
    
    printf("┌─────────────────────────┬─────────────┐\n");
    printf("│ Statistic               │ Value       │\n");
    printf("├─────────────────────────┼─────────────┤\n");
    printf("│ Tree Height             │ %d           │\n", tree_height(root));
    printf("│ Total Nodes             │ %d           │\n", count_nodes(root));
    printf("│ Leaf Nodes              │ %d           │\n", count_leaves(root));
    printf("│ Internal Nodes          │ %d           │\n", count_internal(root));
    printf("│ Sum of All Values       │ %d          │\n", tree_sum(root));
    printf("│ Minimum Value           │ %d           │\n", tree_min(root));
    printf("│ Maximum Value           │ %d          │\n", tree_max(root));
    printf("└─────────────────────────┴─────────────┘\n");
    
    printf("\nAll statistics computed using recursive algorithms.\n");
    printf("Time complexity: O(n) for each operation.\n");
    printf("Space complexity: O(h) where h is tree height.\n");
}

/* =============================================================================
 * PART 5: TREE VISUALISATION
 * =============================================================================
 */

/**
 * Print tree rotated 90 degrees (right subtree on top).
 * 
 * This produces an intuitive visual representation of the tree structure.
 * 
 * @param node The current node
 * @param level Current indentation level
 */
void print_tree_rotated(TreeNode *node, int level) {
    if (node == NULL) {
        return;
    }
    
    /* Print right subtree first (appears on top) */
    print_tree_rotated(node->right, level + 1);
    
    /* Print current node with indentation */
    for (int i = 0; i < level; i++) {
        printf("    ");  /* 4 spaces per level */
    }
    printf("%d\n", node->data);
    
    /* Print left subtree (appears on bottom) */
    print_tree_rotated(node->left, level + 1);
}

/**
 * Print tree with connecting lines (enhanced visualisation).
 * 
 * @param node The current node
 * @param prefix String prefix for indentation
 * @param is_left Whether this is a left child
 */
void print_tree_fancy(TreeNode *node, char *prefix, bool is_left) {
    if (node == NULL) {
        return;
    }
    
    printf("%s", prefix);
    printf("%s", is_left ? "├── " : "└── ");
    printf("%d\n", node->data);
    
    /* Prepare prefix for children */
    char new_prefix[256];
    snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, 
             is_left ? "│   " : "    ");
    
    /* Print children (left first, then right) */
    if (node->left != NULL || node->right != NULL) {
        if (node->left != NULL) {
            print_tree_fancy(node->left, new_prefix, true);
        }
        if (node->right != NULL) {
            print_tree_fancy(node->right, new_prefix, false);
        }
    }
}

void demo_part5(TreeNode *root) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: Tree Visualisation                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("1. Rotated view (right subtree on top):\n\n");
    print_tree_rotated(root, 0);
    
    printf("\n2. Tree structure with connectors:\n\n");
    printf("%d (root)\n", root->data);
    
    char prefix[256] = "";
    if (root->left != NULL) {
        print_tree_fancy(root->left, prefix, true);
    }
    if (root->right != NULL) {
        print_tree_fancy(root->right, prefix, false);
    }
}

/* =============================================================================
 * PART 6: TREE OPERATIONS
 * =============================================================================
 */

/**
 * Mirror (invert) a binary tree.
 * 
 * Swaps left and right children at every node.
 * 
 * @param node The root of the tree/subtree to mirror
 */
void mirror_tree(TreeNode *node) {
    if (node == NULL) {
        return;
    }
    
    /* Swap left and right children */
    TreeNode *temp = node->left;
    node->left = node->right;
    node->right = temp;
    
    /* Recursively mirror subtrees */
    mirror_tree(node->left);
    mirror_tree(node->right);
}

/**
 * Search for a value in the tree.
 * 
 * @param node The root of the tree/subtree
 * @param target The value to search for
 * @return Pointer to the node containing the value, or NULL if not found
 */
TreeNode* tree_search(TreeNode *node, int target) {
    if (node == NULL) {
        return NULL;  /* Not found */
    }
    
    if (node->data == target) {
        return node;  /* Found! */
    }
    
    /* Search left subtree */
    TreeNode *result = tree_search(node->left, target);
    if (result != NULL) {
        return result;
    }
    
    /* Search right subtree */
    return tree_search(node->right, target);
}

/**
 * Check if the tree contains a specific value.
 * 
 * @param node The root of the tree/subtree
 * @param target The value to search for
 * @return true if found, false otherwise
 */
bool tree_contains(TreeNode *node, int target) {
    return tree_search(node, target) != NULL;
}

/**
 * Create a deep copy of a binary tree.
 * 
 * @param node The root of the tree to copy
 * @return Pointer to the root of the new tree
 */
TreeNode* copy_tree(TreeNode *node) {
    if (node == NULL) {
        return NULL;
    }
    
    TreeNode *new_node = create_node(node->data);
    new_node->left = copy_tree(node->left);
    new_node->right = copy_tree(node->right);
    
    return new_node;
}

/**
 * Check if two trees are identical.
 * 
 * @param tree1 Root of first tree
 * @param tree2 Root of second tree
 * @return true if identical, false otherwise
 */
bool trees_identical(TreeNode *tree1, TreeNode *tree2) {
    /* Both empty */
    if (tree1 == NULL && tree2 == NULL) {
        return true;
    }
    
    /* One empty, one not */
    if (tree1 == NULL || tree2 == NULL) {
        return false;
    }
    
    /* Both non-empty: check data and subtrees */
    return (tree1->data == tree2->data) &&
           trees_identical(tree1->left, tree2->left) &&
           trees_identical(tree1->right, tree2->right);
}

void demo_part6(TreeNode *root) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: Tree Operations                                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Search demonstration */
    printf("1. Search Operations:\n");
    printf("   Searching for 7: %s\n", tree_contains(root, 7) ? "FOUND" : "NOT FOUND");
    printf("   Searching for 99: %s\n", tree_contains(root, 99) ? "FOUND" : "NOT FOUND");
    printf("   Searching for 12: %s\n", tree_contains(root, 12) ? "FOUND" : "NOT FOUND");
    
    /* Copy demonstration */
    printf("\n2. Tree Copy:\n");
    TreeNode *copy = copy_tree(root);
    printf("   Created deep copy of tree.\n");
    printf("   Original and copy identical: %s\n", 
           trees_identical(root, copy) ? "YES" : "NO");
    
    /* Mirror demonstration */
    printf("\n3. Tree Mirroring:\n");
    printf("   Original tree inorder:  ");
    inorder(root);
    printf("\n");
    
    mirror_tree(copy);  /* Mirror the copy, not original */
    printf("   Mirrored tree inorder:  ");
    inorder(copy);
    printf("\n");
    
    printf("\n   Mirrored tree structure:\n");
    print_tree_rotated(copy, 0);
    
    /* Clean up copy */
    free_tree(copy);
    printf("\n   Mirrored copy freed successfully.\n");
}

/* =============================================================================
 * PART 7: MEMORY MANAGEMENT
 * =============================================================================
 */

/**
 * Free all nodes in the tree (postorder traversal).
 * 
 * MUST use postorder: children must be freed before parent,
 * otherwise we lose access to children and leak memory.
 * 
 * @param node The root of the tree to free
 */
void free_tree(TreeNode *node) {
    if (node == NULL) {
        return;
    }
    
    /* POSTORDER: free children FIRST */
    free_tree(node->left);
    free_tree(node->right);
    
    /* Then free current node */
    free(node);
}

void demo_part7(TreeNode **root_ptr) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 7: Memory Management                                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Proper memory deallocation is CRITICAL in C.\n\n");
    
    printf("Rules for freeing a binary tree:\n");
    printf("   1. Use POSTORDER traversal (children before parent)\n");
    printf("   2. Check for NULL before freeing\n");
    printf("   3. Set pointer to NULL after freeing\n");
    printf("   4. Use Valgrind to verify no leaks\n\n");
    
    printf("Freeing the tree...\n");
    int node_count = count_nodes(*root_ptr);
    free_tree(*root_ptr);
    *root_ptr = NULL;  /* Prevent dangling pointer */
    
    printf("   Freed %d nodes.\n", node_count);
    printf("   Root pointer set to NULL.\n");
    printf("\n");
    printf("Memory deallocation complete. Run with Valgrind to verify:\n");
    printf("   valgrind --leak-check=full ./example1\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 07: BINARY TREES - Complete Example                  ║\n");
    printf("║                                                               ║\n");
    printf("║     Algorithms and Programming Techniques                     ║\n");
    printf("║     ASE-CSIE, Bucharest                                       ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Part 1: Build the tree */
    demo_part1();
    TreeNode *root = build_sample_tree();
    
    /* Part 2: Recursive traversals */
    demo_part2(root);
    
    /* Part 3: Level-order traversal */
    demo_part3(root);
    
    /* Part 4: Tree statistics */
    demo_part4(root);
    
    /* Part 5: Tree visualisation */
    demo_part5(root);
    
    /* Part 6: Tree operations */
    demo_part6(root);
    
    /* Part 7: Memory management (frees the tree) */
    demo_part7(&root);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                   Example Complete!                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

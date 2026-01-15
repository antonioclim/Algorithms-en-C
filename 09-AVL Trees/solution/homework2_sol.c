/**
 * =============================================================================
 * HOMEWORK 2: AVL Tree Visualiser - SOLUTION
 * =============================================================================
 *
 * Reads operations from a file and visualises the AVL tree after each operation.
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 * USAGE: ./homework2_sol operations.txt
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

typedef struct AVLNode {
    int key;
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

/* Statistics tracking */
typedef struct Stats {
    int total_operations;
    int insertions;
    int deletions;
    int searches;
    int rotations;
    int comparisons;
} Stats;

static Stats stats = {0, 0, 0, 0, 0, 0};

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

int height(AVLNode *node) {
    if (node == NULL) return -1;
    return node->height;
}

int balance_factor(AVLNode *node) {
    if (node == NULL) return 0;
    return height(node->left) - height(node->right);
}

void update_height(AVLNode *node) {
    if (node == NULL) return;
    int left_h = height(node->left);
    int right_h = height(node->right);
    node->height = 1 + (left_h > right_h ? left_h : right_h);
}

AVLNode *create_node(int key) {
    AVLNode *node = (AVLNode *)malloc(sizeof(AVLNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->height = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* =============================================================================
 * ROTATIONS (with statistics tracking)
 * =============================================================================
 */

AVLNode *rotate_right(AVLNode *y) {
    printf("    → Performing RIGHT rotation on %d\n", y->key);
    stats.rotations++;
    
    AVLNode *x = y->left;
    AVLNode *B = x->right;
    
    x->right = y;
    y->left = B;
    
    update_height(y);
    update_height(x);
    
    return x;
}

AVLNode *rotate_left(AVLNode *x) {
    printf("    → Performing LEFT rotation on %d\n", x->key);
    stats.rotations++;
    
    AVLNode *y = x->right;
    AVLNode *B = y->left;
    
    y->left = x;
    x->right = B;
    
    update_height(x);
    update_height(y);
    
    return y;
}

AVLNode *rebalance(AVLNode *node) {
    if (node == NULL) return NULL;
    
    update_height(node);
    int bf = balance_factor(node);
    
    if (bf > 1) {
        if (balance_factor(node->left) < 0) {
            printf("    LR Case detected at node %d\n", node->key);
            node->left = rotate_left(node->left);
        } else {
            printf("    LL Case detected at node %d\n", node->key);
        }
        return rotate_right(node);
    }
    
    if (bf < -1) {
        if (balance_factor(node->right) > 0) {
            printf("    RL Case detected at node %d\n", node->key);
            node->right = rotate_right(node->right);
        } else {
            printf("    RR Case detected at node %d\n", node->key);
        }
        return rotate_left(node);
    }
    
    return node;
}

/* =============================================================================
 * AVL OPERATIONS
 * =============================================================================
 */

AVLNode *avl_insert(AVLNode *node, int key) {
    if (node == NULL) {
        printf("    Created node %d\n", key);
        return create_node(key);
    }
    
    stats.comparisons++;
    
    if (key < node->key) {
        node->left = avl_insert(node->left, key);
    } else if (key > node->key) {
        node->right = avl_insert(node->right, key);
    } else {
        printf("    Key %d already exists\n", key);
        return node;
    }
    
    return rebalance(node);
}

AVLNode *avl_find_min(AVLNode *node) {
    if (node == NULL) return NULL;
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

AVLNode *avl_delete(AVLNode *node, int key) {
    if (node == NULL) {
        printf("    Key %d not found\n", key);
        return NULL;
    }
    
    stats.comparisons++;
    
    if (key < node->key) {
        node->left = avl_delete(node->left, key);
    } else if (key > node->key) {
        node->right = avl_delete(node->right, key);
    } else {
        printf("    Deleting node %d\n", key);
        
        if (node->left == NULL || node->right == NULL) {
            AVLNode *temp = node->left ? node->left : node->right;
            free(node);
            return temp;
        }
        
        AVLNode *successor = avl_find_min(node->right);
        printf("    Replacing with successor %d\n", successor->key);
        node->key = successor->key;
        node->right = avl_delete(node->right, successor->key);
    }
    
    return rebalance(node);
}

AVLNode *avl_search(AVLNode *node, int key) {
    while (node != NULL) {
        stats.comparisons++;
        if (key == node->key) {
            return node;
        } else if (key < node->key) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return NULL;
}

void avl_destroy(AVLNode *node) {
    if (node == NULL) return;
    avl_destroy(node->left);
    avl_destroy(node->right);
    free(node);
}

int avl_count(AVLNode *node) {
    if (node == NULL) return 0;
    return 1 + avl_count(node->left) + avl_count(node->right);
}

/* =============================================================================
 * VISUALISATION
 * =============================================================================
 */

void print_tree_helper(AVLNode *node, int depth, char prefix) {
    if (node == NULL) return;
    
    print_tree_helper(node->right, depth + 1, '/');
    
    for (int i = 0; i < depth; i++) {
        printf("    ");
    }
    printf("%c──[%d](h=%d,bf=%d)\n", 
           prefix, node->key, node->height, balance_factor(node));
    
    print_tree_helper(node->left, depth + 1, '\\');
}

void print_tree(AVLNode *root) {
    printf("\n  Tree:\n");
    if (root == NULL) {
        printf("    (empty)\n");
    } else {
        print_tree_helper(root, 1, '-');
    }
    printf("\n");
}

void avl_inorder(AVLNode *node) {
    if (node == NULL) return;
    avl_inorder(node->left);
    printf("%d ", node->key);
    avl_inorder(node->right);
}

/* =============================================================================
 * FILE PROCESSING
 * =============================================================================
 */

char *trim(char *str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    
    return str;
}

void process_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    
    AVLNode *root = NULL;
    char line[256];
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║             AVL Tree Visualiser - Processing                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    while (fgets(line, sizeof(line), file)) {
        char *trimmed = trim(line);
        if (strlen(trimmed) == 0 || trimmed[0] == '#') {
            continue;  /* Skip empty lines and comments */
        }
        
        stats.total_operations++;
        
        char command[32];
        int value;
        
        if (sscanf(trimmed, "%31s %d", command, &value) >= 1) {
            printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
            printf("> %s", trimmed);
            if (strchr(trimmed, '\n') == NULL) printf("\n");
            printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
            
            if (strcmp(command, "INSERT") == 0) {
                stats.insertions++;
                root = avl_insert(root, value);
                print_tree(root);
                
            } else if (strcmp(command, "DELETE") == 0) {
                stats.deletions++;
                root = avl_delete(root, value);
                print_tree(root);
                
            } else if (strcmp(command, "SEARCH") == 0) {
                stats.searches++;
                AVLNode *found = avl_search(root, value);
                if (found) {
                    printf("    Found: %d ✓\n\n", found->key);
                } else {
                    printf("    Not found: %d ✗\n\n", value);
                }
                
            } else if (strcmp(command, "PRINT") == 0) {
                print_tree(root);
                printf("  Inorder: ");
                avl_inorder(root);
                printf("\n  Nodes: %d\n\n", avl_count(root));
                
            } else if (strcmp(command, "CLEAR") == 0) {
                avl_destroy(root);
                root = NULL;
                printf("    Tree cleared.\n\n");
                
            } else {
                printf("    Unknown command: %s\n\n", command);
            }
        }
    }
    
    fclose(file);
    
    /* Print statistics */
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                       Statistics                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("  Total operations: %d\n", stats.total_operations);
    printf("  Insertions:       %d\n", stats.insertions);
    printf("  Deletions:        %d\n", stats.deletions);
    printf("  Searches:         %d\n", stats.searches);
    printf("  Rotations:        %d\n", stats.rotations);
    printf("  Comparisons:      %d\n", stats.comparisons);
    
    if (root != NULL) {
        printf("\n  Final tree state:\n");
        printf("    Nodes:  %d\n", avl_count(root));
        printf("    Height: %d\n", height(root));
    }
    
    printf("\n");
    
    avl_destroy(root);
}

/* =============================================================================
 * DEMO MODE (when no file provided)
 * =============================================================================
 */

void demo_mode(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║          HOMEWORK 2: AVL Visualiser - DEMO MODE               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    AVLNode *root = NULL;
    
    /* Demo insertions */
    int values[] = {50, 30, 70, 20, 40, 60, 80, 10};
    int n = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < n; i++) {
        printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
        printf("> INSERT %d\n", values[i]);
        printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
        stats.insertions++;
        stats.total_operations++;
        root = avl_insert(root, values[i]);
        print_tree(root);
    }
    
    /* Demo search */
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("> SEARCH 40\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    stats.searches++;
    stats.total_operations++;
    AVLNode *found = avl_search(root, 40);
    printf("    %s\n\n", found ? "Found: 40 ✓" : "Not found ✗");
    
    /* Demo deletion */
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("> DELETE 30\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    stats.deletions++;
    stats.total_operations++;
    root = avl_delete(root, 30);
    print_tree(root);
    
    /* Print statistics */
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                       Statistics                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("  Total operations: %d\n", stats.total_operations);
    printf("  Insertions:       %d\n", stats.insertions);
    printf("  Deletions:        %d\n", stats.deletions);
    printf("  Searches:         %d\n", stats.searches);
    printf("  Rotations:        %d\n", stats.rotations);
    printf("  Comparisons:      %d\n", stats.comparisons);
    printf("  Final height:     %d\n", height(root));
    printf("\n");
    
    avl_destroy(root);
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    Homework 2 Complete! ✓                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Usage: ./homework2_sol <operations_file>\n");
    printf("File format:\n");
    printf("  INSERT <value>\n");
    printf("  DELETE <value>\n");
    printf("  SEARCH <value>\n");
    printf("  PRINT\n");
    printf("  CLEAR\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    if (argc < 2) {
        demo_mode();
    } else {
        process_file(argv[1]);
    }
    
    return 0;
}

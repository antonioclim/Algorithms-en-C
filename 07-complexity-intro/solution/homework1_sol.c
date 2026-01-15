/**
 * =============================================================================
 * HOMEWORK 1 - SOLUTION: Binary Tree Serialisation
 * =============================================================================
 *
 * INSTRUCTOR COPY - Complete working solution
 *
 * Implements:
 *   - serialise(): Convert tree to string
 *   - deserialise(): Reconstruct tree from string
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_BUFFER 10000

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct TreeNode {
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

/* =============================================================================
 * NODE CREATION
 * =============================================================================
 */

TreeNode* create_node(int value) {
    TreeNode *node = malloc(sizeof(TreeNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->data = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* =============================================================================
 * TREE OPERATIONS
 * =============================================================================
 */

void free_tree(TreeNode *node) {
    if (node == NULL) return;
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

bool trees_identical(TreeNode *t1, TreeNode *t2) {
    if (t1 == NULL && t2 == NULL) return true;
    if (t1 == NULL || t2 == NULL) return false;
    return (t1->data == t2->data) &&
           trees_identical(t1->left, t2->left) &&
           trees_identical(t1->right, t2->right);
}

void print_preorder(TreeNode *node) {
    if (node == NULL) {
        printf("# ");
        return;
    }
    printf("%d ", node->data);
    print_preorder(node->left);
    print_preorder(node->right);
}

/* =============================================================================
 * SERIALISATION
 * =============================================================================
 */

/**
 * Helper function for serialisation.
 * Appends node data to buffer using preorder traversal.
 */
void serialise_helper(TreeNode *node, char *buffer) {
    if (node == NULL) {
        strcat(buffer, "#,");
        return;
    }
    
    char num[20];
    sprintf(num, "%d,", node->data);
    strcat(buffer, num);
    
    serialise_helper(node->left, buffer);
    serialise_helper(node->right, buffer);
}

/**
 * Serialise a binary tree to a string.
 * Uses preorder traversal with '#' for NULL nodes.
 *
 * @param root Root of the tree to serialise
 * @return Dynamically allocated string (caller must free)
 */
char* serialise(TreeNode *root) {
    char *buffer = malloc(MAX_BUFFER);
    if (buffer == NULL) {
        fprintf(stderr, "Error: Buffer allocation failed\n");
        exit(EXIT_FAILURE);
    }
    buffer[0] = '\0';
    
    serialise_helper(root, buffer);
    
    /* Remove trailing comma */
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == ',') {
        buffer[len - 1] = '\0';
    }
    
    return buffer;
}

/* =============================================================================
 * DESERIALISATION
 * =============================================================================
 */

/**
 * Helper function for deserialisation.
 * Uses an index pointer to track position in token array.
 */
TreeNode* deserialise_helper(char **tokens, int *index, int count) {
    if (*index >= count) {
        return NULL;
    }
    
    char *token = tokens[*index];
    (*index)++;
    
    if (strcmp(token, "#") == 0) {
        return NULL;
    }
    
    int value = atoi(token);
    TreeNode *node = create_node(value);
    
    node->left = deserialise_helper(tokens, index, count);
    node->right = deserialise_helper(tokens, index, count);
    
    return node;
}

/**
 * Deserialise a string back to a binary tree.
 *
 * @param data The serialised string
 * @return Root of the reconstructed tree
 */
TreeNode* deserialise(char *data) {
    if (data == NULL || strlen(data) == 0) {
        return NULL;
    }
    
    /* Make a copy since strtok modifies the string */
    char *copy = malloc(strlen(data) + 1);
    if (copy == NULL) {
        fprintf(stderr, "Error: Copy allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(copy, data);
    
    /* Count tokens */
    int count = 0;
    char *temp = malloc(strlen(data) + 1);
    strcpy(temp, data);
    char *token = strtok(temp, ",");
    while (token != NULL) {
        count++;
        token = strtok(NULL, ",");
    }
    free(temp);
    
    /* Allocate token array */
    char **tokens = malloc(count * sizeof(char*));
    if (tokens == NULL) {
        fprintf(stderr, "Error: Token array allocation failed\n");
        free(copy);
        exit(EXIT_FAILURE);
    }
    
    /* Fill token array */
    int i = 0;
    token = strtok(copy, ",");
    while (token != NULL && i < count) {
        tokens[i] = token;
        i++;
        token = strtok(NULL, ",");
    }
    
    /* Deserialise */
    int index = 0;
    TreeNode *root = deserialise_helper(tokens, &index, count);
    
    /* Clean up */
    free(tokens);
    free(copy);
    
    return root;
}

/* =============================================================================
 * TEST CASES
 * =============================================================================
 */

void test_case_1(void) {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Test 1: Standard tree\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    /*
     *       1
     *      / \
     *     2   3
     *        / \
     *       4   5
     */
    TreeNode *root = create_node(1);
    root->left = create_node(2);
    root->right = create_node(3);
    root->right->left = create_node(4);
    root->right->right = create_node(5);
    
    printf("Original tree (preorder): ");
    print_preorder(root);
    printf("\n\n");
    
    char *data = serialise(root);
    printf("Serialised: %s\n\n", data);
    
    TreeNode *reconstructed = deserialise(data);
    printf("Reconstructed tree (preorder): ");
    print_preorder(reconstructed);
    printf("\n\n");
    
    printf("Trees identical: %s\n\n", 
           trees_identical(root, reconstructed) ? "YES ✓" : "NO ✗");
    
    free(data);
    free_tree(root);
    free_tree(reconstructed);
}

void test_case_2(void) {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Test 2: Empty tree\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    TreeNode *root = NULL;
    
    char *data = serialise(root);
    printf("Serialised: %s\n\n", data);
    
    TreeNode *reconstructed = deserialise(data);
    printf("Reconstructed: %s\n\n", reconstructed == NULL ? "NULL ✓" : "NOT NULL ✗");
    
    free(data);
}

void test_case_3(void) {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Test 3: Single node\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    TreeNode *root = create_node(42);
    
    printf("Original: %d\n\n", root->data);
    
    char *data = serialise(root);
    printf("Serialised: %s\n\n", data);
    
    TreeNode *reconstructed = deserialise(data);
    printf("Reconstructed: %d\n\n", reconstructed->data);
    
    printf("Trees identical: %s\n\n", 
           trees_identical(root, reconstructed) ? "YES ✓" : "NO ✗");
    
    free(data);
    free_tree(root);
    free_tree(reconstructed);
}

void test_case_4(void) {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Test 4: Left-skewed tree\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    /*
     *     1
     *    /
     *   2
     *  /
     * 3
     */
    TreeNode *root = create_node(1);
    root->left = create_node(2);
    root->left->left = create_node(3);
    
    printf("Original tree (preorder): ");
    print_preorder(root);
    printf("\n\n");
    
    char *data = serialise(root);
    printf("Serialised: %s\n\n", data);
    
    TreeNode *reconstructed = deserialise(data);
    printf("Reconstructed tree (preorder): ");
    print_preorder(reconstructed);
    printf("\n\n");
    
    printf("Trees identical: %s\n\n", 
           trees_identical(root, reconstructed) ? "YES ✓" : "NO ✗");
    
    free(data);
    free_tree(root);
    free_tree(reconstructed);
}

void test_case_5(void) {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Test 5: Negative numbers\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    TreeNode *root = create_node(-1);
    root->left = create_node(-2);
    root->right = create_node(3);
    
    printf("Original tree (preorder): ");
    print_preorder(root);
    printf("\n\n");
    
    char *data = serialise(root);
    printf("Serialised: %s\n\n", data);
    
    TreeNode *reconstructed = deserialise(data);
    printf("Reconstructed tree (preorder): ");
    print_preorder(reconstructed);
    printf("\n\n");
    
    printf("Trees identical: %s\n\n", 
           trees_identical(root, reconstructed) ? "YES ✓" : "NO ✗");
    
    free(data);
    free_tree(root);
    free_tree(reconstructed);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     HOMEWORK 1 SOLUTION: Binary Tree Serialisation            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    test_case_1();
    test_case_2();
    test_case_3();
    test_case_4();
    test_case_5();
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("All tests completed.\n");
    printf("Run with Valgrind to verify no memory leaks.\n\n");
    
    return 0;
}

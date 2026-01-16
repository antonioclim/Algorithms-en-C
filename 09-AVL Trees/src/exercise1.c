/**
 * =============================================================================
 * EXERCISE 1: AVL Tree Foundation
 * =============================================================================
 *
 * This programme is intentionally minimalist in its public interface and
 * intentionally rigorous in its internal invariants.
 *
 * The implementation provides:
 *   - AVL insertion with single and double rotations (LL, RR, LR, RL)
 *   - structural validation (BST ordering, AVL balance and stored heights)
 *   - an in-order traversal printer used as an observational oracle
 *
 * The repository's automated tests supply the input on standard input.
 * The required output format is a deterministic transcript that is compared
 * byte-for-byte against a golden file.
 *
 * Input format (per test case):
 *   <LABEL>\n
 *   <SPACE-SEPARATED-INTEGERS>\n
 *
 * Example:
 *   LL
 *   30 20 10
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
 *
 * =============================================================================
 */

#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct AVLNode {
    int key;
    int height; /* height of subtree rooted at this node; height(NULL) = -1 */
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

/* Optional instrumentation. Kept silent by default because the transcript-based
 * test harness treats any extra output as a failure.
 */
static bool g_verbose = false;
static unsigned long long g_rotation_count = 0ULL;

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

static int avl_height(const AVLNode *node) {
    return (node == NULL) ? -1 : node->height;
}

static int avl_balance_factor(const AVLNode *node) {
    if (node == NULL) return 0;
    return avl_height(node->left) - avl_height(node->right);
}

static void avl_update_height(AVLNode *node) {
    if (node == NULL) return;
    int lh = avl_height(node->left);
    int rh = avl_height(node->right);
    node->height = 1 + (lh > rh ? lh : rh);
}

static AVLNode *avl_create_node(int key) {
    AVLNode *node = (AVLNode *)malloc(sizeof(*node));
    if (node == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->height = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* =============================================================================
 * ROTATIONS
 * =============================================================================
 */

static AVLNode *avl_rotate_right(AVLNode *y) {
    if (g_verbose) {
        printf("  Performing RIGHT rotation on %d\n", y->key);
    }
    g_rotation_count++;

    AVLNode *x = y->left;
    AVLNode *B = x->right;

    x->right = y;
    y->left = B;

    /* Heights must be updated bottom-up: first the demoted node then the new root. */
    avl_update_height(y);
    avl_update_height(x);

    return x;
}

static AVLNode *avl_rotate_left(AVLNode *x) {
    if (g_verbose) {
        printf("  Performing LEFT rotation on %d\n", x->key);
    }
    g_rotation_count++;

    AVLNode *y = x->right;
    AVLNode *B = y->left;

    y->left = x;
    x->right = B;

    avl_update_height(x);
    avl_update_height(y);

    return y;
}

static AVLNode *avl_rebalance(AVLNode *node) {
    if (node == NULL) return NULL;

    avl_update_height(node);
    int bf = avl_balance_factor(node);

    if (bf > 1) {
        /* Left-heavy: LL or LR. */
        if (avl_balance_factor(node->left) < 0) {
            /* LR: rotate left at child then right at node. */
            node->left = avl_rotate_left(node->left);
        }
        return avl_rotate_right(node);
    }

    if (bf < -1) {
        /* Right-heavy: RR or RL. */
        if (avl_balance_factor(node->right) > 0) {
            /* RL: rotate right at child then left at node. */
            node->right = avl_rotate_right(node->right);
        }
        return avl_rotate_left(node);
    }

    return node;
}

/* =============================================================================
 * INSERTION
 * =============================================================================
 */

static AVLNode *avl_insert(AVLNode *node, int key) {
    if (node == NULL) {
        return avl_create_node(key);
    }

    if (key < node->key) {
        node->left = avl_insert(node->left, key);
    } else if (key > node->key) {
        node->right = avl_insert(node->right, key);
    } else {
        /* Duplicate policy: ignore insertion to preserve strict BST ordering. */
        return node;
    }

    return avl_rebalance(node);
}

/* =============================================================================
 * OUTPUT HELPERS
 * =============================================================================
 */

static void avl_inorder_compact(const AVLNode *node, bool *first) {
    if (node == NULL) return;
    avl_inorder_compact(node->left, first);
    if (*first) {
        printf("%d", node->key);
        *first = false;
    } else {
        printf(" %d", node->key);
    }
    avl_inorder_compact(node->right, first);
}

static void avl_print_inorder_line(const AVLNode *root) {
    printf("Inorder: ");
    bool first = true;
    avl_inorder_compact(root, &first);
    printf("\n");
}

/* =============================================================================
 * MEMORY MANAGEMENT
 * =============================================================================
 */

static void avl_destroy(AVLNode *node) {
    if (node == NULL) return;
    avl_destroy(node->left);
    avl_destroy(node->right);
    free(node);
}

/* =============================================================================
 * VALIDATION
 * =============================================================================
 */

static int avl_validate_rec(const AVLNode *node, long long min_key, long long max_key, bool *ok) {
    if (node == NULL) return -1;

    if (!(*ok)) return -1; /* fast-fail once invalid */

    if ((long long)node->key <= min_key || (long long)node->key >= max_key) {
        *ok = false;
        return -1;
    }

    int lh = avl_validate_rec(node->left, min_key, node->key, ok);
    int rh = avl_validate_rec(node->right, node->key, max_key, ok);

    int bf = lh - rh;
    if (bf < -1 || bf > 1) {
        *ok = false;
        return -1;
    }

    int expected_h = 1 + (lh > rh ? lh : rh);
    if (node->height != expected_h) {
        *ok = false;
        return -1;
    }

    return expected_h;
}

static bool avl_is_valid(const AVLNode *root) {
    bool ok = true;
    (void)avl_validate_rec(root, LLONG_MIN, LLONG_MAX, &ok);
    return ok;
}

/* =============================================================================
 * PARSING HELPERS
 * =============================================================================
 */

static void trim_in_place(char *s) {
    if (s == NULL) return;

    /* Left trim */
    size_t i = 0;
    while (s[i] != '\0' && isspace((unsigned char)s[i])) i++;
    if (i > 0) {
        memmove(s, s + i, strlen(s + i) + 1);
    }

    /* Right trim */
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1])) {
        s[n - 1] = '\0';
        n--;
    }
}

static size_t parse_ints_from_line(const char *line, int *out, size_t cap) {
    if (line == NULL || out == NULL || cap == 0) return 0;

    char buf[1024];
    snprintf(buf, sizeof(buf), "%s", line);

    size_t count = 0;
    for (char *tok = strtok(buf, " \t"); tok != NULL; tok = strtok(NULL, " \t")) {
        if (count >= cap) break;
        char *endptr = NULL;
        long v = strtol(tok, &endptr, 10);
        if (endptr == tok || *endptr != '\0') continue;
        out[count++] = (int)v;
    }

    return count;
}

static void print_values_commas(const int *values, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (i > 0) printf(", ");
        printf("%d", values[i]);
    }
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char **argv) {
    /* Optional flags:
     *   --verbose enables rotation logging.
     */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--verbose") == 0) {
            g_verbose = true;
        }
    }

    char label[128];
    char numbers_line[1024];

    int test_index = 1;

    while (fgets(label, sizeof(label), stdin) != NULL) {
        trim_in_place(label);
        if (label[0] == '\0') continue;

        if (fgets(numbers_line, sizeof(numbers_line), stdin) == NULL) {
            break;
        }
        trim_in_place(numbers_line);

        int values[512];
        size_t n = parse_ints_from_line(numbers_line, values, sizeof(values) / sizeof(values[0]));

        AVLNode *root = NULL;
        g_rotation_count = 0ULL;

        for (size_t i = 0; i < n; i++) {
            root = avl_insert(root, values[i]);
        }

        if (test_index >= 1 && test_index <= 4) {
            printf("TEST %d: %s Case - Insert ", test_index, label);
            print_values_commas(values, n);
            printf("\n");

            avl_print_inorder_line(root);
            printf("Valid AVL: %s\n", avl_is_valid(root) ? "YES" : "NO");
            printf("Root: %d, Height: %d\n", root ? root->key : 0, avl_height(root));
            printf("\n");
        } else if (test_index == 5) {
            printf("TEST 5: Larger tree\n");
            avl_print_inorder_line(root);
            printf("Valid AVL: %s\n", avl_is_valid(root) ? "YES" : "NO");
            printf("Height: %d\n", avl_height(root));
        } else {
            /* Generic fallback in case additional test cases are appended. */
            printf("TEST %d: %s\n", test_index, label);
            avl_print_inorder_line(root);
            printf("Valid AVL: %s\n", avl_is_valid(root) ? "YES" : "NO");
            printf("Height: %d\n", avl_height(root));
        }

        avl_destroy(root);
        test_index++;
    }

    return 0;
}

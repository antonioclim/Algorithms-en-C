/**
 * =============================================================================
 * EXERCISE 2: AVL Tree Advanced Operations
 * =============================================================================
 *
 * This programme extends the Week 09 AVL implementation with deletion and
 * order-aware queries (successor, predecessor and range enumeration).
 *
 * The implementation is designed to be assessed through deterministic
 * transcript testing: a command script is supplied on standard input and the
 * programme prints an exact textual contract.
 *
 * Supported commands (as used by tests/test2_input.txt):
 *   BUILD\n<space-separated keys>\n
 *   SEARCH\n<key>\n
 *   MIN\n
 *   MAX\n
 *   SUCCESSOR\n<key>\n
 *   PREDECESSOR\n<key>\n
 *   RANGE\n<low high>\n
 *   DELETE\n<key>\n
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
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
 * DATA STRUCTURE
 * =============================================================================
 */

typedef struct AVLNode {
    int key;
    int height; /* height of subtree rooted at this node, where height(NULL) = -1 */
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

/* =============================================================================
 * CORE UTILITIES
 * =============================================================================
 */

static int avl_height(const AVLNode *node) {
    return (node == NULL) ? -1 : node->height;
}

static int avl_balance_factor(const AVLNode *node) {
    return (node == NULL) ? 0 : (avl_height(node->left) - avl_height(node->right));
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
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->height = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* =============================================================================
 * ROTATIONS AND REBALANCING
 * =============================================================================
 */

static AVLNode *avl_rotate_right(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *B = x->right;

    x->right = y;
    y->left = B;

    avl_update_height(y);
    avl_update_height(x);

    return x;
}

static AVLNode *avl_rotate_left(AVLNode *x) {
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
        if (avl_balance_factor(node->left) < 0) {
            node->left = avl_rotate_left(node->left);
        }
        return avl_rotate_right(node);
    }

    if (bf < -1) {
        if (avl_balance_factor(node->right) > 0) {
            node->right = avl_rotate_right(node->right);
        }
        return avl_rotate_left(node);
    }

    return node;
}

static AVLNode *avl_insert(AVLNode *node, int key) {
    if (node == NULL) return avl_create_node(key);

    if (key < node->key) {
        node->left = avl_insert(node->left, key);
    } else if (key > node->key) {
        node->right = avl_insert(node->right, key);
    } else {
        return node; /* duplicate policy: ignore */
    }

    return avl_rebalance(node);
}

/* =============================================================================
 * SEARCH OPERATIONS
 * =============================================================================
 */

static AVLNode *avl_find_min(AVLNode *node) {
    if (node == NULL) return NULL;
    while (node->left != NULL) node = node->left;
    return node;
}

static AVLNode *avl_find_max(AVLNode *node) {
    if (node == NULL) return NULL;
    while (node->right != NULL) node = node->right;
    return node;
}

static AVLNode *avl_search(AVLNode *node, int key) {
    while (node != NULL) {
        if (key == node->key) return node;
        node = (key < node->key) ? node->left : node->right;
    }
    return NULL;
}

static bool avl_contains(AVLNode *node, int key) {
    return avl_search(node, key) != NULL;
}

/* =============================================================================
 * DELETION
 * =============================================================================
 */

static AVLNode *avl_delete(AVLNode *node, int key) {
    if (node == NULL) return NULL;

    if (key < node->key) {
        node->left = avl_delete(node->left, key);
        return avl_rebalance(node);
    }

    if (key > node->key) {
        node->right = avl_delete(node->right, key);
        return avl_rebalance(node);
    }

    /* key == node->key: delete this node */
    if (node->left == NULL || node->right == NULL) {
        AVLNode *child = (node->left != NULL) ? node->left : node->right;
        free(node);
        return child;
    }

    /* Two children: replace by inorder successor */
    AVLNode *succ = avl_find_min(node->right);
    node->key = succ->key;
    node->right = avl_delete(node->right, succ->key);
    return avl_rebalance(node);
}

/* =============================================================================
 * SUCCESSOR AND PREDECESSOR
 * =============================================================================
 */

static AVLNode *avl_successor(AVLNode *root, int key) {
    AVLNode *succ = NULL;
    AVLNode *cur = root;

    while (cur != NULL) {
        if (key < cur->key) {
            succ = cur;
            cur = cur->left;
        } else if (key > cur->key) {
            cur = cur->right;
        } else {
            if (cur->right != NULL) {
                return avl_find_min(cur->right);
            }
            break;
        }
    }

    return succ;
}

static AVLNode *avl_predecessor(AVLNode *root, int key) {
    AVLNode *pred = NULL;
    AVLNode *cur = root;

    while (cur != NULL) {
        if (key > cur->key) {
            pred = cur;
            cur = cur->right;
        } else if (key < cur->key) {
            cur = cur->left;
        } else {
            if (cur->left != NULL) {
                return avl_find_max(cur->left);
            }
            break;
        }
    }

    return pred;
}

/* =============================================================================
 * RANGE QUERY AND ORDERED PRINTING
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

static void avl_range_compact(const AVLNode *node, int low, int high, bool *first) {
    if (node == NULL) return;

    if (node->key > low) {
        avl_range_compact(node->left, low, high, first);
    }

    if (node->key >= low && node->key <= high) {
        if (*first) {
            printf("%d", node->key);
            *first = false;
        } else {
            printf(" %d", node->key);
        }
    }

    if (node->key < high) {
        avl_range_compact(node->right, low, high, first);
    }
}

static void avl_print_range_line(const AVLNode *root, int low, int high) {
    printf("RANGE [%d, %d]: ", low, high);
    bool first = true;
    avl_range_compact(root, low, high, &first);
    printf("\n");
}

/* =============================================================================
 * STATISTICS AND MEMORY MANAGEMENT
 * =============================================================================
 */

static int avl_count_nodes(const AVLNode *node) {
    if (node == NULL) return 0;
    return 1 + avl_count_nodes(node->left) + avl_count_nodes(node->right);
}

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
    if (!(*ok)) return -1;

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
 * SCRIPT PARSING
 * =============================================================================
 */

typedef enum {
    GROUP_NONE = 0,
    GROUP_BUILD,
    GROUP_SEARCH,
    GROUP_MINMAX,
    GROUP_SUCCPRED,
    GROUP_RANGE,
    GROUP_DELETE
} OutputGroup;

static void trim_in_place(char *s) {
    if (s == NULL) return;

    /* left trim */
    size_t i = 0;
    while (s[i] != '\0' && isspace((unsigned char)s[i])) i++;
    if (i > 0) memmove(s, s + i, strlen(s + i) + 1);

    /* right trim */
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1])) {
        s[n - 1] = '\0';
        n--;
    }
}

static bool read_next_nonempty_line(char *buf, size_t cap) {
    while (fgets(buf, (int)cap, stdin) != NULL) {
        trim_in_place(buf);
        if (buf[0] != '\0') return true;
    }
    return false;
}

static OutputGroup classify_group(const char *cmd) {
    if (strcmp(cmd, "BUILD") == 0) return GROUP_BUILD;
    if (strcmp(cmd, "SEARCH") == 0) return GROUP_SEARCH;
    if (strcmp(cmd, "MIN") == 0 || strcmp(cmd, "MAX") == 0) return GROUP_MINMAX;
    if (strcmp(cmd, "SUCCESSOR") == 0 || strcmp(cmd, "PREDECESSOR") == 0) return GROUP_SUCCPRED;
    if (strcmp(cmd, "RANGE") == 0) return GROUP_RANGE;
    if (strcmp(cmd, "DELETE") == 0) return GROUP_DELETE;
    return GROUP_NONE;
}

static size_t parse_ints_from_line(const char *line, int *out, size_t cap) {
    if (line == NULL) return 0;

    char tmp[2048];
    strncpy(tmp, line, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    size_t n = 0;
    for (char *tok = strtok(tmp, " \t"); tok != NULL; tok = strtok(NULL, " \t")) {
        if (n >= cap) break;
        int v;
        if (sscanf(tok, "%d", &v) == 1) {
            out[n++] = v;
        }
    }

    return n;
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(void) {
    AVLNode *root = NULL;

    OutputGroup last_group = GROUP_NONE;

    char cmd[128];
    while (read_next_nonempty_line(cmd, sizeof(cmd))) {
        OutputGroup group = classify_group(cmd);

        if (group == GROUP_NONE) {
            /* Unknown line: ignore to remain robust to comments or stray text. */
            continue;
        }

        /* Insert a blank line between logical output sections.
           DELETE blocks print their own trailing blank line, hence the guard. */
        if (group != last_group && last_group != GROUP_NONE && last_group != GROUP_DELETE) {
            printf("\n");
        }
        last_group = group;

        if (strcmp(cmd, "BUILD") == 0) {
            char keys_line[2048];
            if (!read_next_nonempty_line(keys_line, sizeof(keys_line))) break;

            printf("BUILD: %s\n", keys_line);

            int values[2048];
            size_t n = parse_ints_from_line(keys_line, values, sizeof(values) / sizeof(values[0]));

            for (size_t i = 0; i < n; i++) {
                root = avl_insert(root, values[i]);
            }

            avl_print_inorder_line(root);
            printf("Valid AVL: %s\n", avl_is_valid(root) ? "YES" : "NO");
            continue;
        }

        if (strcmp(cmd, "SEARCH") == 0) {
            char key_line[128];
            if (!read_next_nonempty_line(key_line, sizeof(key_line))) break;
            int key;
            if (sscanf(key_line, "%d", &key) != 1) continue;
            printf("SEARCH %d: %s\n", key, avl_contains(root, key) ? "FOUND" : "NOT FOUND");
            continue;
        }

        if (strcmp(cmd, "MIN") == 0) {
            AVLNode *mn = avl_find_min(root);
            printf("MIN: %d\n", mn ? mn->key : 0);
            continue;
        }

        if (strcmp(cmd, "MAX") == 0) {
            AVLNode *mx = avl_find_max(root);
            printf("MAX: %d\n", mx ? mx->key : 0);
            continue;
        }

        if (strcmp(cmd, "SUCCESSOR") == 0) {
            char key_line[128];
            if (!read_next_nonempty_line(key_line, sizeof(key_line))) break;
            int key;
            if (sscanf(key_line, "%d", &key) != 1) continue;
            AVLNode *succ = avl_successor(root, key);
            printf("SUCCESSOR of %d: %d\n", key, succ ? succ->key : 0);
            continue;
        }

        if (strcmp(cmd, "PREDECESSOR") == 0) {
            char key_line[128];
            if (!read_next_nonempty_line(key_line, sizeof(key_line))) break;
            int key;
            if (sscanf(key_line, "%d", &key) != 1) continue;
            AVLNode *pred = avl_predecessor(root, key);
            printf("PREDECESSOR of %d: %d\n", key, pred ? pred->key : 0);
            continue;
        }

        if (strcmp(cmd, "RANGE") == 0) {
            char bounds_line[128];
            if (!read_next_nonempty_line(bounds_line, sizeof(bounds_line))) break;
            int low, high;
            if (sscanf(bounds_line, "%d %d", &low, &high) != 2) continue;
            avl_print_range_line(root, low, high);
            continue;
        }

        if (strcmp(cmd, "DELETE") == 0) {
            char key_line[128];
            if (!read_next_nonempty_line(key_line, sizeof(key_line))) break;
            int key;
            if (sscanf(key_line, "%d", &key) != 1) continue;

            printf("DELETE %d:\n", key);
            root = avl_delete(root, key);
            printf("Valid AVL: %s\n", avl_is_valid(root) ? "YES" : "NO");
            avl_print_inorder_line(root);
            printf("\n");
            continue;
        }
    }

    printf("Final node count: %d\n", avl_count_nodes(root));
    printf("Final height: %d\n", avl_height(root));

    avl_destroy(root);
    return 0;
}

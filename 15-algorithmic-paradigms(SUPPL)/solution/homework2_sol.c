/**
 * =============================================================================
 * WEEK 15: ALGORITHMIC PARADIGMS
 * Homework 2 Solution - Algorithm Design Problems
 * =============================================================================
 *
 * This solution implements:
 *   1. Weighted Activity Selection (DP)
 *   2. Huffman Encoding/Decoding
 *   3. 0/1 Knapsack with Item Reconstruction
 *   4. Edit Distance with Alignment
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c -lm
 * Usage: ./homework2_sol
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

/* =============================================================================
 * PROBLEM 1: WEIGHTED ACTIVITY SELECTION (DP)
 * =============================================================================
 *
 * Unlike unweighted activity selection where greedy works, weighted activities
 * require dynamic programming because selecting a high-weight activity might
 * prevent selecting multiple compatible lower-weight activities.
 *
 * Time Complexity: O(n log n) with binary search
 * Space Complexity: O(n)
 */

typedef struct {
    int start;
    int finish;
    int weight;
    int original_idx;  /* Track original index for reconstruction */
} Activity;

int compare_activities_by_finish(const void *a, const void *b) {
    return ((Activity *)a)->finish - ((Activity *)b)->finish;
}

/* Binary search to find latest compatible activity */
int binary_search_compatible(Activity *activities, int i) {
    int lo = 0, hi = i - 1;
    
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (activities[mid].finish <= activities[i].start) {
            if (activities[mid + 1].finish <= activities[i].start) {
                lo = mid + 1;
            } else {
                return mid;
            }
        } else {
            hi = mid - 1;
        }
    }
    return -1;
}

int weighted_activity_selection(Activity *activities, int n, int *selected, int *num_selected) {
    /* Sort by finish time */
    qsort(activities, n, sizeof(Activity), compare_activities_by_finish);
    
    /* DP arrays */
    int *dp = malloc(n * sizeof(int));
    int *choice = malloc(n * sizeof(int));  /* 1 = include, 0 = exclude */
    
    if (!dp || !choice) {
        free(dp);
        free(choice);
        return -1;
    }
    
    /* Base case */
    dp[0] = activities[0].weight;
    choice[0] = 1;
    
    /* Fill DP table */
    for (int i = 1; i < n; i++) {
        /* Option 1: Exclude activity i */
        int exclude = dp[i - 1];
        
        /* Option 2: Include activity i */
        int include = activities[i].weight;
        int p = binary_search_compatible(activities, i);
        if (p != -1) {
            include += dp[p];
        }
        
        if (include > exclude) {
            dp[i] = include;
            choice[i] = 1;
        } else {
            dp[i] = exclude;
            choice[i] = 0;
        }
    }
    
    int max_weight = dp[n - 1];
    
    /* Reconstruct solution */
    *num_selected = 0;
    int i = n - 1;
    while (i >= 0) {
        if (choice[i] == 1) {
            selected[(*num_selected)++] = activities[i].original_idx;
            int p = binary_search_compatible(activities, i);
            i = p;
        } else {
            i--;
        }
    }
    
    /* Reverse selected array to get chronological order */
    for (int j = 0; j < *num_selected / 2; j++) {
        int temp = selected[j];
        selected[j] = selected[*num_selected - 1 - j];
        selected[*num_selected - 1 - j] = temp;
    }
    
    free(dp);
    free(choice);
    
    return max_weight;
}

void demo_weighted_activity_selection(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║    PROBLEM 1: WEIGHTED ACTIVITY SELECTION                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Activity activities[] = {
        {1, 4, 5, 0},
        {3, 5, 1, 1},
        {0, 6, 8, 2},
        {4, 7, 4, 3},
        {3, 8, 6, 4},
        {5, 9, 3, 5},
        {6, 10, 2, 6},
        {8, 11, 4, 7}
    };
    int n = 8;
    
    printf("Activities (start, finish, weight):\n");
    printf("┌─────┬───────┬────────┬────────┐\n");
    printf("│ ID  │ Start │ Finish │ Weight │\n");
    printf("├─────┼───────┼────────┼────────┤\n");
    for (int i = 0; i < n; i++) {
        printf("│  %d  │   %d   │   %2d   │   %2d   │\n",
               activities[i].original_idx,
               activities[i].start,
               activities[i].finish,
               activities[i].weight);
    }
    printf("└─────┴───────┴────────┴────────┘\n\n");
    
    int selected[8];
    int num_selected;
    int max_weight = weighted_activity_selection(activities, n, selected, &num_selected);
    
    printf("After sorting by finish time:\n");
    for (int i = 0; i < n; i++) {
        printf("  Activity %d: [%d, %d) weight=%d\n",
               activities[i].original_idx,
               activities[i].start,
               activities[i].finish,
               activities[i].weight);
    }
    printf("\n");
    
    printf("═════════════════════════════════════════════════════\n");
    printf("Maximum weight: %d\n", max_weight);
    printf("Activities selected: ");
    for (int i = 0; i < num_selected; i++) {
        printf("%d ", selected[i]);
    }
    printf("\n═════════════════════════════════════════════════════\n\n");
}

/* =============================================================================
 * PROBLEM 2: HUFFMAN ENCODING/DECODING
 * =============================================================================
 *
 * Huffman coding is an optimal prefix-free coding algorithm.
 * The key insight is that more frequent characters get shorter codes.
 *
 * Time Complexity: O(n log n) using heap
 * Space Complexity: O(n)
 */

#define MAX_TREE_SIZE 256

typedef struct HuffmanNode {
    char ch;
    int freq;
    struct HuffmanNode *left, *right;
} HuffmanNode;

typedef struct {
    HuffmanNode *nodes[MAX_TREE_SIZE];
    int size;
} MinHeap;

HuffmanNode *create_huffman_node(char ch, int freq) {
    HuffmanNode *node = malloc(sizeof(HuffmanNode));
    if (node) {
        node->ch = ch;
        node->freq = freq;
        node->left = node->right = NULL;
    }
    return node;
}

void swap_nodes(HuffmanNode **a, HuffmanNode **b) {
    HuffmanNode *t = *a;
    *a = *b;
    *b = t;
}

void min_heapify(MinHeap *heap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    
    if (left < heap->size && heap->nodes[left]->freq < heap->nodes[smallest]->freq)
        smallest = left;
    if (right < heap->size && heap->nodes[right]->freq < heap->nodes[smallest]->freq)
        smallest = right;
    
    if (smallest != idx) {
        swap_nodes(&heap->nodes[smallest], &heap->nodes[idx]);
        min_heapify(heap, smallest);
    }
}

HuffmanNode *extract_min(MinHeap *heap) {
    HuffmanNode *min = heap->nodes[0];
    heap->nodes[0] = heap->nodes[heap->size - 1];
    heap->size--;
    min_heapify(heap, 0);
    return min;
}

void insert_node(MinHeap *heap, HuffmanNode *node) {
    heap->size++;
    int i = heap->size - 1;
    
    while (i && node->freq < heap->nodes[(i - 1) / 2]->freq) {
        heap->nodes[i] = heap->nodes[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    heap->nodes[i] = node;
}

void build_heap(MinHeap *heap) {
    int n = heap->size;
    for (int i = (n - 1) / 2; i >= 0; i--) {
        min_heapify(heap, i);
    }
}

HuffmanNode *build_huffman_tree(char *chars, int *freqs, int n) {
    MinHeap heap = {.size = 0};
    
    /* Create leaf nodes */
    for (int i = 0; i < n; i++) {
        heap.nodes[heap.size++] = create_huffman_node(chars[i], freqs[i]);
    }
    build_heap(&heap);
    
    /* Build tree */
    while (heap.size > 1) {
        HuffmanNode *left = extract_min(&heap);
        HuffmanNode *right = extract_min(&heap);
        
        HuffmanNode *parent = create_huffman_node('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        
        insert_node(&heap, parent);
    }
    
    return extract_min(&heap);
}

void generate_codes(HuffmanNode *root, char *code, int depth, char codes[][32], char *chars, int n) {
    if (!root) return;
    
    /* Leaf node */
    if (!root->left && !root->right) {
        code[depth] = '\0';
        for (int i = 0; i < n; i++) {
            if (chars[i] == root->ch) {
                strcpy(codes[i], code);
                break;
            }
        }
        return;
    }
    
    /* Recurse left (0) and right (1) */
    code[depth] = '0';
    generate_codes(root->left, code, depth + 1, codes, chars, n);
    
    code[depth] = '1';
    generate_codes(root->right, code, depth + 1, codes, chars, n);
}

void free_huffman_tree(HuffmanNode *root) {
    if (root) {
        free_huffman_tree(root->left);
        free_huffman_tree(root->right);
        free(root);
    }
}

void demo_huffman(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║    PROBLEM 2: HUFFMAN ENCODING/DECODING                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    const char *text = "abracadabra";
    printf("Input text: \"%s\"\n", text);
    printf("Original size: %zu characters = %zu bits\n\n", strlen(text), strlen(text) * 8);
    
    /* Calculate frequencies */
    int freq[256] = {0};
    for (size_t i = 0; i < strlen(text); i++) {
        freq[(unsigned char)text[i]]++;
    }
    
    /* Extract unique characters */
    char chars[256];
    int freqs[256];
    int n = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            chars[n] = (char)i;
            freqs[n] = freq[i];
            n++;
        }
    }
    
    printf("Character frequencies:\n");
    printf("┌──────┬───────────┐\n");
    printf("│ Char │ Frequency │\n");
    printf("├──────┼───────────┤\n");
    for (int i = 0; i < n; i++) {
        printf("│  %c   │     %d     │\n", chars[i], freqs[i]);
    }
    printf("└──────┴───────────┘\n\n");
    
    /* Build Huffman tree */
    HuffmanNode *root = build_huffman_tree(chars, freqs, n);
    
    /* Generate codes */
    char codes[256][32];
    char code_buffer[32];
    generate_codes(root, code_buffer, 0, codes, chars, n);
    
    printf("Huffman codes:\n");
    printf("┌──────┬──────────┐\n");
    printf("│ Char │   Code   │\n");
    printf("├──────┼──────────┤\n");
    for (int i = 0; i < n; i++) {
        printf("│  %c   │ %-8s │\n", chars[i], codes[i]);
    }
    printf("└──────┴──────────┘\n\n");
    
    /* Encode */
    printf("Encoded: ");
    int encoded_bits = 0;
    for (size_t i = 0; i < strlen(text); i++) {
        for (int j = 0; j < n; j++) {
            if (chars[j] == text[i]) {
                printf("%s", codes[j]);
                encoded_bits += strlen(codes[j]);
                break;
            }
        }
    }
    printf("\n\n");
    
    printf("═════════════════════════════════════════════════════\n");
    printf("Encoded size: %d bits\n", encoded_bits);
    printf("Compression ratio: %.1f%%\n", 100.0 * (1.0 - (double)encoded_bits / (strlen(text) * 8)));
    printf("═════════════════════════════════════════════════════\n\n");
    
    free_huffman_tree(root);
}

/* =============================================================================
 * PROBLEM 3: 0/1 KNAPSACK WITH ITEM RECONSTRUCTION
 * =============================================================================
 *
 * Classic DP problem. The key is tracking which items were selected,
 * not just the maximum value.
 *
 * Time Complexity: O(n × W)
 * Space Complexity: O(n × W)
 */

typedef struct {
    int weight;
    int value;
} Item;

int knapsack_01(Item *items, int n, int capacity, int *selected, int *num_selected) {
    /* DP table */
    int **dp = malloc((n + 1) * sizeof(int *));
    for (int i = 0; i <= n; i++) {
        dp[i] = calloc(capacity + 1, sizeof(int));
    }
    
    /* Fill DP table */
    for (int i = 1; i <= n; i++) {
        for (int w = 0; w <= capacity; w++) {
            if (items[i - 1].weight <= w) {
                int include = items[i - 1].value + dp[i - 1][w - items[i - 1].weight];
                int exclude = dp[i - 1][w];
                dp[i][w] = (include > exclude) ? include : exclude;
            } else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }
    
    int max_value = dp[n][capacity];
    
    /* Backtrack to find selected items */
    *num_selected = 0;
    int w = capacity;
    for (int i = n; i > 0 && w > 0; i--) {
        if (dp[i][w] != dp[i - 1][w]) {
            selected[(*num_selected)++] = i - 1;  /* 0-indexed */
            w -= items[i - 1].weight;
        }
    }
    
    /* Free DP table */
    for (int i = 0; i <= n; i++) {
        free(dp[i]);
    }
    free(dp);
    
    return max_value;
}

void demo_knapsack(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║    PROBLEM 3: 0/1 KNAPSACK WITH RECONSTRUCTION                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Item items[] = {{1, 1}, {3, 4}, {4, 5}, {5, 7}};
    int n = 4;
    int capacity = 7;
    
    printf("Items (weight, value):\n");
    printf("┌──────┬────────┬───────┐\n");
    printf("│ Item │ Weight │ Value │\n");
    printf("├──────┼────────┼───────┤\n");
    for (int i = 0; i < n; i++) {
        printf("│  %d   │   %d    │   %d   │\n", i, items[i].weight, items[i].value);
    }
    printf("└──────┴────────┴───────┘\n\n");
    printf("Knapsack capacity: %d\n\n", capacity);
    
    /* Show DP table */
    printf("DP Table dp[i][w]:\n");
    printf("       w=0  w=1  w=2  w=3  w=4  w=5  w=6  w=7\n");
    
    int selected[4];
    int num_selected;
    int max_value = knapsack_01(items, n, capacity, selected, &num_selected);
    
    printf("\n═════════════════════════════════════════════════════\n");
    printf("Maximum value: %d\n", max_value);
    printf("Selected items (0-indexed): ");
    int total_weight = 0;
    for (int i = num_selected - 1; i >= 0; i--) {
        printf("%d ", selected[i]);
        total_weight += items[selected[i]].weight;
    }
    printf("\nTotal weight: %d\n", total_weight);
    printf("═════════════════════════════════════════════════════\n\n");
}

/* =============================================================================
 * PROBLEM 4: EDIT DISTANCE WITH ALIGNMENT
 * =============================================================================
 *
 * Levenshtein distance: minimum edit operations (insert, delete, replace)
 * to transform one string into another.
 *
 * Time Complexity: O(m × n)
 * Space Complexity: O(m × n) for full solution, O(min(m,n)) for just distance
 */

int edit_distance(const char *s1, const char *s2, char *alignment1, char *alignment2, char *operations) {
    int m = strlen(s1);
    int n = strlen(s2);
    
    /* DP table */
    int **dp = malloc((m + 1) * sizeof(int *));
    for (int i = 0; i <= m; i++) {
        dp[i] = malloc((n + 1) * sizeof(int));
    }
    
    /* Base cases */
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;
    
    /* Fill table */
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                int insert = dp[i][j - 1] + 1;
                int delete = dp[i - 1][j] + 1;
                int replace = dp[i - 1][j - 1] + 1;
                
                dp[i][j] = insert;
                if (delete < dp[i][j]) dp[i][j] = delete;
                if (replace < dp[i][j]) dp[i][j] = replace;
            }
        }
    }
    
    int distance = dp[m][n];
    
    /* Backtrack for alignment */
    int i = m, j = n;
    int pos = 0;
    char temp1[256], temp2[256], tempop[256];
    
    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && s1[i - 1] == s2[j - 1]) {
            temp1[pos] = s1[i - 1];
            temp2[pos] = s2[j - 1];
            tempop[pos] = '=';
            i--; j--;
        } else if (j > 0 && (i == 0 || dp[i][j - 1] <= dp[i - 1][j - 1] && dp[i][j - 1] <= dp[i - 1][j])) {
            temp1[pos] = '-';
            temp2[pos] = s2[j - 1];
            tempop[pos] = 'I';
            j--;
        } else if (i > 0 && (j == 0 || dp[i - 1][j] <= dp[i - 1][j - 1])) {
            temp1[pos] = s1[i - 1];
            temp2[pos] = '-';
            tempop[pos] = 'D';
            i--;
        } else {
            temp1[pos] = s1[i - 1];
            temp2[pos] = s2[j - 1];
            tempop[pos] = 'R';
            i--; j--;
        }
        pos++;
    }
    
    /* Reverse strings */
    for (int k = 0; k < pos; k++) {
        alignment1[k] = temp1[pos - 1 - k];
        alignment2[k] = temp2[pos - 1 - k];
        operations[k] = tempop[pos - 1 - k];
    }
    alignment1[pos] = '\0';
    alignment2[pos] = '\0';
    operations[pos] = '\0';
    
    /* Free DP table */
    for (int k = 0; k <= m; k++) {
        free(dp[k]);
    }
    free(dp);
    
    return distance;
}

void demo_edit_distance(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║    PROBLEM 4: EDIT DISTANCE WITH ALIGNMENT                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    const char *s1 = "kitten";
    const char *s2 = "sitting";
    
    printf("String 1: \"%s\"\n", s1);
    printf("String 2: \"%s\"\n\n", s2);
    
    char align1[256], align2[256], ops[256];
    int dist = edit_distance(s1, s2, align1, align2, ops);
    
    printf("Alignment:\n");
    printf("  %s\n", align1);
    printf("  ");
    for (size_t i = 0; i < strlen(align1); i++) {
        printf("%c", (align1[i] == align2[i] && align1[i] != '-') ? '|' : ' ');
    }
    printf("\n");
    printf("  %s\n", align2);
    printf("  %s\n\n", ops);
    
    printf("Legend: = match, R replace, I insert, D delete\n\n");
    
    printf("═════════════════════════════════════════════════════\n");
    printf("Edit distance: %d\n", dist);
    printf("Operations:\n");
    for (size_t i = 0; i < strlen(ops); i++) {
        if (ops[i] == 'R') {
            printf("  - Replace '%c' with '%c'\n", align1[i], align2[i]);
        } else if (ops[i] == 'I') {
            printf("  - Insert '%c'\n", align2[i]);
        } else if (ops[i] == 'D') {
            printf("  - Delete '%c'\n", align1[i]);
        }
    }
    printf("═════════════════════════════════════════════════════\n\n");
}

/* =============================================================================
 * MAIN FUNCTION
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 15: ALGORITHMIC PARADIGMS                            ║\n");
    printf("║     Homework 2 Solution: Algorithm Design Problems            ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    demo_weighted_activity_selection();
    demo_huffman();
    demo_knapsack();
    demo_edit_distance();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    ALL PROBLEMS COMPLETE                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

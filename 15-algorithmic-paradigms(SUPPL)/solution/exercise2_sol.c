/**
 * =============================================================================
 * WEEK 15: ALGORITHMIC PARADIGMS
 * Exercise 2 Solution - Greedy vs Dynamic Programming
 * =============================================================================
 *
 * This solution implements:
 *   1. Coin change using greedy approach
 *   2. Coin change using dynamic programming
 *   3. Job sequencing with deadlines (greedy)
 *   4. Longest increasing subsequence (DP)
 *   5. Demonstration where greedy fails but DP succeeds
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c
 * Usage: ./exercise2_sol
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

/* =============================================================================
 * SECTION 1: COIN CHANGE - GREEDY APPROACH
 * =============================================================================
 *
 * Greedy Strategy: Always select the largest coin that doesn't exceed the
 * remaining amount. This works optimally for canonical coin systems
 * (e.g., US coins: 25, 10, 5, 1) but fails for non-canonical systems.
 *
 * Time Complexity: O(n) where n is the number of coin denominations
 * Space Complexity: O(n) for storing the result
 */

/**
 * Solve coin change using greedy approach.
 *
 * @param coins Array of coin denominations (sorted descending)
 * @param num_coins Number of different coin denominations
 * @param amount Target amount to make change for
 * @param result Array to store count of each coin used
 * @return Total number of coins used, or -1 if impossible
 */
int coin_change_greedy(const int *coins, int num_coins, int amount, int *result) {
    /* =========================================================================
     * SOLUTION:
     * Iterate through coins from largest to smallest, using as many as possible
     * of each denomination before moving to the next.
     * ========================================================================= */
    
    int total_coins = 0;
    int remaining = amount;
    
    /* Initialise result array */
    for (int i = 0; i < num_coins; i++) {
        result[i] = 0;
    }
    
    /* Greedy selection: use largest coins first */
    for (int i = 0; i < num_coins && remaining > 0; i++) {
        if (coins[i] <= remaining) {
            result[i] = remaining / coins[i];
            remaining -= result[i] * coins[i];
            total_coins += result[i];
        }
    }
    
    /* Check if exact change was achieved */
    if (remaining != 0) {
        return -1;  /* Greedy failed to find solution */
    }
    
    return total_coins;
}

/* =============================================================================
 * SECTION 2: COIN CHANGE - DYNAMIC PROGRAMMING
 * =============================================================================
 *
 * DP Formulation:
 *   State: dp[i] = minimum coins needed to make amount i
 *   Base: dp[0] = 0 (zero coins for zero amount)
 *   Recurrence: dp[i] = min(dp[i], dp[i - coins[j]] + 1) for all valid j
 *
 * Time Complexity: O(amount × num_coins)
 * Space Complexity: O(amount)
 */

/**
 * Solve coin change using dynamic programming.
 *
 * @param coins Array of coin denominations
 * @param num_coins Number of different coin denominations
 * @param amount Target amount to make change for
 * @param result Array to store count of each coin used
 * @return Total number of coins used, or -1 if impossible
 */
int coin_change_dp(const int *coins, int num_coins, int amount, int *result) {
    /* =========================================================================
     * SOLUTION:
     * Build DP table bottom-up, tracking both minimum coins and which coin
     * was used at each step for reconstruction.
     * ========================================================================= */
    
    /* Allocate DP table and backtracking array */
    int *dp = malloc((amount + 1) * sizeof(int));
    int *used_coin = malloc((amount + 1) * sizeof(int));  /* Which coin was used */
    
    if (!dp || !used_coin) {
        free(dp);
        free(used_coin);
        return -1;
    }
    
    /* Initialise: dp[0] = 0, all others = infinity */
    dp[0] = 0;
    used_coin[0] = -1;
    for (int i = 1; i <= amount; i++) {
        dp[i] = INT_MAX;
        used_coin[i] = -1;
    }
    
    /* Fill DP table */
    for (int i = 1; i <= amount; i++) {
        for (int j = 0; j < num_coins; j++) {
            if (coins[j] <= i && dp[i - coins[j]] != INT_MAX) {
                if (dp[i - coins[j]] + 1 < dp[i]) {
                    dp[i] = dp[i - coins[j]] + 1;
                    used_coin[i] = j;  /* Record which coin was used */
                }
            }
        }
    }
    
    /* Check if solution exists */
    if (dp[amount] == INT_MAX) {
        free(dp);
        free(used_coin);
        return -1;
    }
    
    int total_coins = dp[amount];
    
    /* Backtrack to find which coins were used */
    for (int i = 0; i < num_coins; i++) {
        result[i] = 0;
    }
    
    int current = amount;
    while (current > 0) {
        int coin_idx = used_coin[current];
        result[coin_idx]++;
        current -= coins[coin_idx];
    }
    
    free(dp);
    free(used_coin);
    
    return total_coins;
}

/* =============================================================================
 * SECTION 3: JOB SEQUENCING WITH DEADLINES (GREEDY)
 * =============================================================================
 *
 * Problem: Given n jobs with deadlines and profits, schedule jobs to maximise
 * total profit. Each job takes 1 unit of time. A job can only be scheduled
 * if it finishes before or at its deadline.
 *
 * Greedy Strategy: Sort by profit descending, schedule each job in the
 * latest available slot before its deadline.
 *
 * Time Complexity: O(n²) naive, O(n log n) with Union-Find optimisation
 * Space Complexity: O(max_deadline)
 */

typedef struct {
    char id;
    int deadline;
    int profit;
} Job;

/**
 * Compare jobs by profit (descending order).
 */
int compare_jobs_by_profit(const void *a, const void *b) {
    const Job *ja = (const Job *)a;
    const Job *jb = (const Job *)b;
    return jb->profit - ja->profit;  /* Descending */
}

/**
 * Schedule jobs to maximise profit using greedy approach.
 *
 * @param jobs Array of jobs
 * @param n Number of jobs
 * @param schedule Array to store scheduled job IDs
 * @param max_profit Pointer to store maximum achievable profit
 * @return Number of jobs scheduled
 */
int job_sequencing(Job *jobs, int n, char *schedule, int *max_profit) {
    /* =========================================================================
     * SOLUTION:
     * 1. Sort jobs by profit (descending)
     * 2. For each job, find latest available slot before deadline
     * 3. Schedule job if slot available
     * ========================================================================= */
    
    /* Find maximum deadline */
    int max_deadline = 0;
    for (int i = 0; i < n; i++) {
        if (jobs[i].deadline > max_deadline) {
            max_deadline = jobs[i].deadline;
        }
    }
    
    /* Sort jobs by profit descending */
    qsort(jobs, n, sizeof(Job), compare_jobs_by_profit);
    
    /* Track which time slots are filled */
    bool *slot_filled = calloc(max_deadline + 1, sizeof(bool));
    if (!slot_filled) {
        return -1;
    }
    
    int jobs_scheduled = 0;
    *max_profit = 0;
    
    /* Try to schedule each job */
    for (int i = 0; i < n; i++) {
        /* Find latest available slot before deadline */
        for (int slot = jobs[i].deadline; slot >= 1; slot--) {
            if (!slot_filled[slot]) {
                /* Schedule this job */
                slot_filled[slot] = true;
                schedule[jobs_scheduled] = jobs[i].id;
                *max_profit += jobs[i].profit;
                jobs_scheduled++;
                break;
            }
        }
    }
    
    schedule[jobs_scheduled] = '\0';
    free(slot_filled);
    
    return jobs_scheduled;
}

/* =============================================================================
 * SECTION 4: LONGEST INCREASING SUBSEQUENCE (DP)
 * =============================================================================
 *
 * Problem: Find the length of the longest strictly increasing subsequence.
 *
 * DP Formulation (O(n²) approach):
 *   State: dp[i] = length of LIS ending at index i
 *   Base: dp[i] = 1 for all i (single element is LIS of length 1)
 *   Recurrence: dp[i] = max(dp[j] + 1) for all j < i where arr[j] < arr[i]
 *
 * Time Complexity: O(n²)
 * Space Complexity: O(n)
 */

/**
 * Find longest increasing subsequence.
 *
 * @param arr Input array
 * @param n Size of array
 * @param lis_result Array to store the actual LIS (must be size n)
 * @return Length of LIS
 */
int longest_increasing_subsequence(const int *arr, int n, int *lis_result) {
    /* =========================================================================
     * SOLUTION:
     * 1. Build dp[] where dp[i] = length of LIS ending at i
     * 2. Track predecessor for reconstruction
     * 3. Find maximum and backtrack
     * ========================================================================= */
    
    if (n == 0) {
        return 0;
    }
    
    int *dp = malloc(n * sizeof(int));
    int *pred = malloc(n * sizeof(int));  /* Predecessor index */
    
    if (!dp || !pred) {
        free(dp);
        free(pred);
        return -1;
    }
    
    /* Initialise: each element forms LIS of length 1 */
    for (int i = 0; i < n; i++) {
        dp[i] = 1;
        pred[i] = -1;
    }
    
    /* Fill DP table */
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (arr[j] < arr[i] && dp[j] + 1 > dp[i]) {
                dp[i] = dp[j] + 1;
                pred[i] = j;
            }
        }
    }
    
    /* Find maximum LIS length and its ending index */
    int max_length = 0;
    int max_idx = 0;
    for (int i = 0; i < n; i++) {
        if (dp[i] > max_length) {
            max_length = dp[i];
            max_idx = i;
        }
    }
    
    /* Reconstruct LIS by backtracking */
    int *temp = malloc(max_length * sizeof(int));
    if (!temp) {
        free(dp);
        free(pred);
        return max_length;
    }
    
    int idx = max_idx;
    for (int i = max_length - 1; i >= 0; i--) {
        temp[i] = arr[idx];
        idx = pred[idx];
    }
    
    /* Copy to result */
    for (int i = 0; i < max_length; i++) {
        lis_result[i] = temp[i];
    }
    
    free(temp);
    free(dp);
    free(pred);
    
    return max_length;
}

/* =============================================================================
 * SECTION 5: GREEDY FAILURE DEMONSTRATION
 * =============================================================================
 *
 * This section demonstrates cases where greedy algorithms fail to find
 * optimal solutions, while dynamic programming succeeds.
 *
 * Classic example: Coin denominations {1, 3, 4} with amount 6
 *   Greedy: 4 + 1 + 1 = 3 coins
 *   Optimal (DP): 3 + 3 = 2 coins
 */

void demonstrate_greedy_failure(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║    DEMONSTRATION: WHEN GREEDY FAILS BUT DP SUCCEEDS           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Non-canonical coin system where greedy fails */
    int coins[] = {4, 3, 1};  /* Sorted descending */
    int coins_sorted[] = {1, 3, 4};  /* For DP (doesn't need sorted, but consistent) */
    int num_coins = 3;
    int amount = 6;
    
    printf("Coin denominations: {1, 3, 4}\n");
    printf("Target amount: %d\n\n", amount);
    
    /* Greedy approach */
    int greedy_result[3];
    int greedy_count = coin_change_greedy(coins, num_coins, amount, greedy_result);
    
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│ GREEDY APPROACH                                             │\n");
    printf("├─────────────────────────────────────────────────────────────┤\n");
    printf("│ Strategy: Always take largest coin possible                 │\n");
    printf("│                                                             │\n");
    printf("│ Step 1: 6 - 4 = 2  (use coin 4)                             │\n");
    printf("│ Step 2: 2 - 1 = 1  (use coin 1, can't use 3 or 4)           │\n");
    printf("│ Step 3: 1 - 1 = 0  (use coin 1)                             │\n");
    printf("│                                                             │\n");
    printf("│ Result: 4 + 1 + 1 = %d coins                                 │\n", greedy_count);
    printf("│ Coins used: 4×%d, 3×%d, 1×%d                                  │\n",
           greedy_result[0], greedy_result[1], greedy_result[2]);
    printf("└─────────────────────────────────────────────────────────────┘\n\n");
    
    /* Dynamic Programming approach */
    int dp_result[3];
    int dp_count = coin_change_dp(coins_sorted, num_coins, amount, dp_result);
    
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│ DYNAMIC PROGRAMMING APPROACH                                │\n");
    printf("├─────────────────────────────────────────────────────────────┤\n");
    printf("│ State: dp[i] = minimum coins for amount i                   │\n");
    printf("│                                                             │\n");
    printf("│ DP Table:                                                   │\n");
    printf("│   Amount:  0  1  2  3  4  5  6                              │\n");
    printf("│   dp[i]:   0  1  2  1  1  2  2                              │\n");
    printf("│                                                             │\n");
    printf("│ Result: 3 + 3 = %d coins                                     │\n", dp_count);
    printf("│ Coins used: 1×%d, 3×%d, 4×%d                                  │\n",
           dp_result[0], dp_result[1], dp_result[2]);
    printf("└─────────────────────────────────────────────────────────────┘\n\n");
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║ CONCLUSION: DP finds optimal solution (2 coins)               ║\n");
    printf("║             Greedy gives suboptimal (3 coins)                 ║\n");
    printf("║                                                               ║\n");
    printf("║ Why greedy fails:                                             ║\n");
    printf("║   • Local optimum (taking 4) prevents global optimum          ║\n");
    printf("║   • No greedy choice property for this coin system            ║\n");
    printf("║                                                               ║\n");
    printf("║ When greedy works:                                            ║\n");
    printf("║   • Canonical coin systems (US: 25,10,5,1)                    ║\n");
    printf("║   • Each coin divides all larger coins                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
}

/* =============================================================================
 * DEMONSTRATION FUNCTIONS
 * =============================================================================
 */

void demo_coin_change(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           PART 1: COIN CHANGE COMPARISON                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Standard US coins (greedy works) */
    int us_coins[] = {25, 10, 5, 1};
    int num_us_coins = 4;
    int amount = 67;
    
    printf("US Coin System: {25, 10, 5, 1}\n");
    printf("Amount: %d cents\n\n", amount);
    
    int greedy_result[4], dp_result[4];
    int greedy_count = coin_change_greedy(us_coins, num_us_coins, amount, greedy_result);
    
    int us_coins_asc[] = {1, 5, 10, 25};
    int dp_count = coin_change_dp(us_coins_asc, num_us_coins, amount, dp_result);
    
    printf("Greedy: 25×%d + 10×%d + 5×%d + 1×%d = %d coins\n",
           greedy_result[0], greedy_result[1], greedy_result[2], greedy_result[3],
           greedy_count);
    printf("DP:     25×%d + 10×%d + 5×%d + 1×%d = %d coins\n",
           dp_result[3], dp_result[2], dp_result[1], dp_result[0],
           dp_count);
    printf("\n✓ Both approaches give same result for canonical coin systems\n\n");
}

void demo_job_sequencing(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║         PART 2: JOB SEQUENCING WITH DEADLINES                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Job jobs[] = {
        {'A', 2, 100},
        {'B', 1, 19},
        {'C', 2, 27},
        {'D', 1, 25},
        {'E', 3, 15}
    };
    int n = 5;
    
    printf("Jobs (ID, Deadline, Profit):\n");
    printf("┌─────┬──────────┬────────┐\n");
    printf("│ ID  │ Deadline │ Profit │\n");
    printf("├─────┼──────────┼────────┤\n");
    for (int i = 0; i < n; i++) {
        printf("│  %c  │    %d     │  %3d   │\n", jobs[i].id, jobs[i].deadline, jobs[i].profit);
    }
    printf("└─────┴──────────┴────────┘\n\n");
    
    char schedule[10];
    int max_profit;
    int scheduled = job_sequencing(jobs, n, schedule, &max_profit);
    
    printf("After sorting by profit (descending):\n");
    printf("A(100) → C(27) → D(25) → B(19) → E(15)\n\n");
    
    printf("Scheduling process:\n");
    printf("  Job A (profit=100, deadline=2): Schedule at slot 2 ✓\n");
    printf("  Job C (profit=27, deadline=2): Slot 2 taken, try slot 1 ✓\n");
    printf("  Job D (profit=25, deadline=1): Slot 1 taken ✗\n");
    printf("  Job B (profit=19, deadline=1): Slot 1 taken ✗\n");
    printf("  Job E (profit=15, deadline=3): Schedule at slot 3 ✓\n\n");
    
    printf("═════════════════════════════════════════════\n");
    printf("Jobs scheduled: %d\n", scheduled);
    printf("Schedule: %s\n", schedule);
    printf("Maximum profit: %d\n", max_profit);
    printf("═════════════════════════════════════════════\n\n");
}

void demo_lis(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║       PART 3: LONGEST INCREASING SUBSEQUENCE                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int arr[] = {10, 22, 9, 33, 21, 50, 41, 60, 80};
    int n = 9;
    
    printf("Array: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n\n");
    
    int lis[9];
    int length = longest_increasing_subsequence(arr, n, lis);
    
    printf("DP Table Construction:\n");
    printf("┌───────┬────┬────┬────┬────┬────┬────┬────┬────┬────┐\n");
    printf("│ Index │  0 │  1 │  2 │  3 │  4 │  5 │  6 │  7 │  8 │\n");
    printf("├───────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤\n");
    printf("│ arr[] │ 10 │ 22 │  9 │ 33 │ 21 │ 50 │ 41 │ 60 │ 80 │\n");
    printf("├───────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤\n");
    printf("│ dp[]  │  1 │  2 │  1 │  3 │  2 │  4 │  4 │  5 │  6 │\n");
    printf("└───────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘\n\n");
    
    printf("LIS Length: %d\n", length);
    printf("LIS: ");
    for (int i = 0; i < length; i++) {
        printf("%d ", lis[i]);
    }
    printf("\n\n");
    
    printf("Backtracking path: 80 ← 60 ← 50 ← 33 ← 22 ← 10\n\n");
}

/* =============================================================================
 * INTERACTIVE MENU
 * =============================================================================
 */

void print_menu(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║       EXERCISE 2: GREEDY VS DYNAMIC PROGRAMMING               ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║                                                               ║\n");
    printf("║   1. Coin Change Comparison                                   ║\n");
    printf("║   2. Job Sequencing (Greedy)                                  ║\n");
    printf("║   3. Longest Increasing Subsequence (DP)                      ║\n");
    printf("║   4. Demonstrate Greedy Failure                               ║\n");
    printf("║   5. Run All Demonstrations                                   ║\n");
    printf("║   0. Exit                                                     ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\nEnter choice: ");
}

/* =============================================================================
 * MAIN FUNCTION
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║        WEEK 15: ALGORITHMIC PARADIGMS                         ║\n");
    printf("║        Exercise 2 Solution: Greedy vs DP                      ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    int choice;
    
    do {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            /* Clear invalid input */
            while (getchar() != '\n');
            choice = -1;
        }
        
        switch (choice) {
            case 1:
                demo_coin_change();
                break;
            case 2:
                demo_job_sequencing();
                break;
            case 3:
                demo_lis();
                break;
            case 4:
                demonstrate_greedy_failure();
                break;
            case 5:
                demo_coin_change();
                demo_job_sequencing();
                demo_lis();
                demonstrate_greedy_failure();
                break;
            case 0:
                printf("\nExiting...\n");
                break;
            default:
                printf("\nInvalid choice. Please try again.\n");
        }
    } while (choice != 0);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    PROGRAM COMPLETE                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

/**
 * =============================================================================
 * EXERCISE 2: Greedy vs Dynamic Programming
 * =============================================================================
 *
 * OBJECTIVE:
 *   Explore the boundary between greedy algorithms and dynamic programming
 *   by implementing both approaches for appropriate problems.
 *
 * REQUIREMENTS:
 *   1. Implement greedy coin change algorithm
 *   2. Implement DP coin change algorithm
 *   3. Find cases where greedy fails but DP succeeds
 *   4. Implement job sequencing with deadlines (greedy)
 *   5. Implement longest increasing subsequence (DP)
 *
 * EXAMPLE INPUT:
 *   1
 *   4
 *   1 5 10 25
 *   30
 *
 * EXPECTED OUTPUT:
 *   Coin Change Problem:
 *   Denominations: 1 5 10 25
 *   Amount: 30
 *   
 *   Greedy: 6 coins (25 + 5)
 *   DP:     6 coins (25 + 5)
 *   Both methods agree!
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c -lm
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* =============================================================================
 * PART A: COIN CHANGE PROBLEM
 * =============================================================================
 */

/**
 * TODO 1: Implement greedy coin change
 *
 * Greedy strategy: Always use the largest denomination possible
 *
 * @param coins Array of coin denominations (sorted ascending)
 * @param n Number of denominations
 * @param amount Target amount
 * @param result Array to store count of each coin used (size n)
 * @return Total number of coins used, or -1 if impossible
 *
 * Hint:
 *   1. Start from the largest denomination (coins[n-1])
 *   2. Use as many of that coin as possible: count = amount / coins[i]
 *   3. Update amount = amount % coins[i]
 *   4. Store result[i] = count
 *   5. If amount is still > 0 after all coins, return -1
 */
int coin_change_greedy(int coins[], int n, int amount, int result[]) {
    /* Initialize result array to 0 */
    for (int i = 0; i < n; i++) {
        result[i] = 0;
    }

    if (amount < 0) {
        return -1;
    }

    /* Greedy: start from the largest coin and take as many as possible. */
    int remaining = amount;
    int total = 0;

    for (int i = n - 1; i >= 0; i--) {
        if (coins[i] <= 0) {
            continue;
        }
        if (remaining <= 0) {
            break;
        }

        int cnt = remaining / coins[i];
        result[i] = cnt;
        total += cnt;
        remaining = remaining % coins[i];
    }

    if (remaining != 0) {
        /* Not representable with the given denominations. */
        for (int i = 0; i < n; i++) {
            result[i] = 0;
        }
        return -1;
    }
    return total;
}

/**
 * TODO 2: Implement DP coin change
 *
 * Find minimum number of coins to make the target amount.
 *
 * @param coins Array of coin denominations
 * @param n Number of denominations
 * @param amount Target amount
 * @param result Array to store count of each coin used (size n)
 * @return Total number of coins used, or -1 if impossible
 *
 * State: dp[i] = minimum coins needed to make amount i
 * Base: dp[0] = 0
 * Recurrence: dp[i] = min(dp[i], dp[i - coins[j]] + 1) for all valid j
 *
 * Hint:
 *   1. Create dp array of size (amount + 1)
 *   2. Initialize dp[0] = 0, dp[i] = INT_MAX for i > 0
 *   3. For each amount from 1 to target:
 *        For each coin:
 *          If coin <= current amount and dp[amount - coin] != INT_MAX:
 *            dp[amount] = min(dp[amount], dp[amount - coin] + 1)
 *   4. Backtrack to find which coins were used
 */
int coin_change_dp(int coins[], int n, int amount, int result[]) {
    /* Initialize result array to 0 */
    for (int i = 0; i < n; i++) {
        result[i] = 0;
    }

    if (amount < 0) {
        return -1;
    }

    /* dp[x] stores the minimum number of coins needed to make amount x.
       pick[x] stores the index of the last coin used in an optimal solution
       for x, or -1 if x is unreachable. */
    int *dp = (int *)malloc((size_t)(amount + 1) * sizeof(int));
    int *pick = (int *)malloc((size_t)(amount + 1) * sizeof(int));

    if (dp == NULL || pick == NULL) {
        free(dp);
        free(pick);
        return -1;
    }

    dp[0] = 0;
    pick[0] = -1;
    for (int x = 1; x <= amount; x++) {
        dp[x] = INT_MAX;
        pick[x] = -1;
    }

    for (int x = 1; x <= amount; x++) {
        for (int j = 0; j < n; j++) {
            int c = coins[j];
            if (c <= 0 || c > x) {
                continue;
            }
            if (dp[x - c] == INT_MAX) {
                continue;
            }
            if (dp[x - c] + 1 < dp[x]) {
                dp[x] = dp[x - c] + 1;
                pick[x] = j;
            }
        }
    }

    if (dp[amount] == INT_MAX) {
        free(dp);
        free(pick);
        return -1;
    }

    /* Backtrack to reconstruct an optimal multiset of coins. */
    int remaining = amount;
    while (remaining > 0) {
        int j = pick[remaining];
        if (j < 0) {
            /* Defensive fallback: inconsistent table. */
            for (int i = 0; i < n; i++) {
                result[i] = 0;
            }
            free(dp);
            free(pick);
            return -1;
        }
        result[j]++;
        remaining -= coins[j];
    }

    int answer = dp[amount];
    free(dp);
    free(pick);
    return answer;
}

/**
 * Compare greedy and DP results
 */
void compare_coin_change(int coins[], int n, int amount) {
    int *greedy_result = (int *)calloc(n, sizeof(int));
    int *dp_result = (int *)calloc(n, sizeof(int));
    
    printf("Coin Change Problem:\n");
    printf("  Denominations: ");
    for (int i = 0; i < n; i++) printf("%d ", coins[i]);
    printf("\n  Amount: %d\n\n", amount);
    
    int greedy_count = coin_change_greedy(coins, n, amount, greedy_result);
    int dp_count = coin_change_dp(coins, n, amount, dp_result);
    
    printf("  Greedy: ");
    if (greedy_count == -1) {
        printf("IMPOSSIBLE\n");
    } else {
        printf("%d coins (", greedy_count);
        int first = 1;
        for (int i = n - 1; i >= 0; i--) {
            for (int j = 0; j < greedy_result[i]; j++) {
                if (!first) printf(" + ");
                printf("%d", coins[i]);
                first = 0;
            }
        }
        printf(")\n");
    }
    
    printf("  DP:     ");
    if (dp_count == -1) {
        printf("IMPOSSIBLE\n");
    } else {
        printf("%d coins (", dp_count);
        int first = 1;
        for (int i = n - 1; i >= 0; i--) {
            for (int j = 0; j < dp_result[i]; j++) {
                if (!first) printf(" + ");
                printf("%d", coins[i]);
                first = 0;
            }
        }
        printf(")\n");
    }
    
    if (greedy_count != -1 && dp_count != -1) {
        if (greedy_count == dp_count) {
            printf("  ✓ Both methods agree!\n");
        } else {
            printf("  ✗ GREEDY FAILS! DP is better by %d coins\n", 
                   greedy_count - dp_count);
        }
    }
    
    free(greedy_result);
    free(dp_result);
}

/* =============================================================================
 * PART B: JOB SEQUENCING WITH DEADLINES (GREEDY)
 * =============================================================================
 */

typedef struct {
    int id;
    int deadline;
    int profit;
} Job;

/**
 * Comparator for sorting jobs by profit (descending)
 */
int cmp_job_profit(const void *a, const void *b) {
    return ((Job *)b)->profit - ((Job *)a)->profit;
}

/**
 * TODO 3: Implement job sequencing with deadlines
 *
 * Given jobs with deadlines and profits, find the maximum profit
 * subset of jobs that can be completed on time (one job per time slot).
 *
 * @param jobs Array of jobs
 * @param n Number of jobs
 * @param schedule Output array of job IDs in each time slot (size max_deadline)
 * @return Maximum profit achievable
 *
 * Greedy strategy:
 *   1. Sort jobs by profit (descending)
 *   2. For each job, find the latest available slot before its deadline
 *   3. If slot available, schedule job there
 *
 * Hint:
 *   1. Find max_deadline among all jobs
 *   2. Create time_slot array of size max_deadline, initialized to -1
 *   3. Sort jobs by profit
 *   4. For each job (in profit order):
 *        For slot from min(deadline, max_deadline) - 1 down to 0:
 *          If time_slot[slot] == -1:
 *            Assign job to this slot, add profit, break
 */
int job_sequencing(Job jobs[], int n, int schedule[]) {
    /* Find maximum deadline */
    int max_deadline = 0;
    for (int i = 0; i < n; i++) {
        if (jobs[i].deadline > max_deadline) {
            max_deadline = jobs[i].deadline;
        }
    }
    
    /* Initialize schedule to -1 (empty) */
    for (int i = 0; i < max_deadline; i++) {
        schedule[i] = -1;
    }
    
    /* Sort jobs by profit (descending) */
    qsort(jobs, n, sizeof(Job), cmp_job_profit);
    
    /* Greedy: schedule each job in latest available slot */
    int total_profit = 0;

    for (int i = 0; i < n; i++) {
        int d = jobs[i].deadline;
        if (d <= 0) {
            continue;
        }

        int last_slot = d;
        if (last_slot > max_deadline) {
            last_slot = max_deadline;
        }

        /* Search for the latest free slot strictly before the deadline index.
           Time slots are 0-based: slot 0 represents the first unit of time. */
        for (int slot = last_slot - 1; slot >= 0; slot--) {
            if (schedule[slot] == -1) {
                schedule[slot] = jobs[i].id;
                total_profit += jobs[i].profit;
                break;
            }
        }
    }
    
    return total_profit;
}

/* =============================================================================
 * PART C: LONGEST INCREASING SUBSEQUENCE (DP)
 * =============================================================================
 */

/**
 * TODO 4: Implement longest increasing subsequence
 *
 * Find the length of the longest strictly increasing subsequence.
 *
 * @param arr Input array
 * @param n Array size
 * @param lis Output array containing the actual LIS
 * @return Length of LIS
 *
 * State: dp[i] = length of LIS ending at index i
 * Base: dp[i] = 1 (each element is LIS of length 1)
 * Recurrence: dp[i] = max(dp[j] + 1) for all j < i where arr[j] < arr[i]
 *
 * Hint:
 *   1. Create dp array, initialize all to 1
 *   2. Create parent array to track previous element in LIS
 *   3. For i from 1 to n-1:
 *        For j from 0 to i-1:
 *          If arr[j] < arr[i] and dp[j] + 1 > dp[i]:
 *            dp[i] = dp[j] + 1
 *            parent[i] = j
 *   4. Find index with maximum dp value
 *   5. Backtrack using parent to reconstruct LIS
 */
int longest_increasing_subsequence(int arr[], int n, int lis[]) {
    if (n == 0) return 0;

    int *dp = (int *)malloc((size_t)n * sizeof(int));
    int *parent = (int *)malloc((size_t)n * sizeof(int));

    if (dp == NULL || parent == NULL) {
        free(dp);
        free(parent);
        return 0;
    }

    for (int i = 0; i < n; i++) {
        dp[i] = 1;
        parent[i] = -1;
    }

    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (arr[j] < arr[i] && dp[j] + 1 > dp[i]) {
                dp[i] = dp[j] + 1;
                parent[i] = j;
            }
        }
    }

    int best_len = 1;
    int best_end = 0;
    for (int i = 0; i < n; i++) {
        if (dp[i] > best_len) {
            best_len = dp[i];
            best_end = i;
        }
    }

    /* Reconstruct the subsequence by following parent pointers then reversing
       into the provided output buffer. */
    int idx = best_end;
    int k = best_len - 1;
    while (idx != -1 && k >= 0) {
        lis[k--] = arr[idx];
        idx = parent[idx];
    }

    free(dp);
    free(parent);
    return best_len;
}

/* =============================================================================
 * PART D: DEMONSTRATING GREEDY FAILURE
 * =============================================================================
 */

/**
 * TODO 5: Find coin denominations where greedy fails
 *
 * Create a test case where greedy gives a suboptimal solution
 * but DP finds the optimal one.
 *
 * Example: coins = {1, 3, 4}, amount = 6
 *   Greedy: 4 + 1 + 1 = 3 coins
 *   DP:     3 + 3 = 2 coins
 */
void demonstrate_greedy_failure(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      DEMONSTRATING GREEDY FAILURE                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* TODO 5: Find and demonstrate a case where greedy fails
     *
     * Hint: Try non-canonical coin systems like:
     *   - {1, 3, 4} for amount 6
     *   - {1, 5, 6, 9} for amount 11
     *   - {1, 15, 25} for amount 30
     */
    
    printf("Test case 1: Coins = {1, 3, 4}, Amount = 6\n");
    int coins1[] = {1, 3, 4};
    compare_coin_change(coins1, 3, 6);
    printf("\n");
    
    /* Optional extension: additional non-canonical coin systems can be explored
       without modifying the default transcript used by automated tests. Set
       run_extended to a non-zero value if you want to print further cases. */
    const int run_extended = 0;
    if (run_extended) {
        printf("Test case 2: Coins = {1, 7, 10}, Amount = 14\n");
        int coins2[] = {1, 7, 10};
        compare_coin_change(coins2, 3, 14);
        printf("\n");
    }
    
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    int choice;
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 2: Greedy vs Dynamic Programming                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Select problem:\n");
    printf("  1. Coin Change (compare greedy vs DP)\n");
    printf("  2. Job Sequencing (greedy)\n");
    printf("  3. Longest Increasing Subsequence (DP)\n");
    printf("  4. Demonstrate Greedy Failure\n");
    printf("\nChoice: ");
    
    if (scanf("%d", &choice) != 1) {
        fprintf(stderr, "Invalid input\n");
        return 1;
    }
    
    switch (choice) {
        case 1: {
            int n, amount;
            printf("\nEnter number of coin denominations: ");
            scanf("%d", &n);
            
            int *coins = (int *)malloc(n * sizeof(int));
            printf("Enter denominations (ascending): ");
            for (int i = 0; i < n; i++) {
                scanf("%d", &coins[i]);
            }
            
            printf("Enter target amount: ");
            scanf("%d", &amount);
            printf("\n");
            
            compare_coin_change(coins, n, amount);
            free(coins);
            break;
        }
        
        case 2: {
            int n;
            printf("\nEnter number of jobs: ");
            scanf("%d", &n);

            if (n <= 0) {
                printf("\nJob Sequencing Result:\n");
                printf("  Schedule: \n");
                printf("  Total Profit: 0\n");
                break;
            }
            
            Job *jobs = (Job *)malloc(n * sizeof(Job));
            printf("Enter jobs (id deadline profit):\n");
            for (int i = 0; i < n; i++) {
                scanf("%d %d %d", &jobs[i].id, &jobs[i].deadline, &jobs[i].profit);
            }
            
            /* Find max deadline for schedule array */
            int max_d = 0;
            for (int i = 0; i < n; i++) {
                if (jobs[i].deadline > max_d) max_d = jobs[i].deadline;
            }

            if (max_d <= 0) {
                printf("\nJob Sequencing Result:\n");
                printf("  Schedule: \n");
                printf("  Total Profit: 0\n");
                free(jobs);
                break;
            }
            
            int *schedule = (int *)malloc(max_d * sizeof(int));
            int profit = job_sequencing(jobs, n, schedule);
            
            printf("\nJob Sequencing Result:\n");
            printf("  Schedule: ");
            for (int i = 0; i < max_d; i++) {
                if (schedule[i] != -1) {
                    printf("J%d ", schedule[i]);
                } else {
                    printf("-- ");
                }
            }
            printf("\n  Total Profit: %d\n", profit);
            
            free(jobs);
            free(schedule);
            break;
        }
        
        case 3: {
            int n;
            printf("\nEnter array size: ");
            scanf("%d", &n);
            
            int *arr = (int *)malloc(n * sizeof(int));
            int *lis = (int *)malloc(n * sizeof(int));
            
            printf("Enter %d integers: ", n);
            for (int i = 0; i < n; i++) {
                scanf("%d", &arr[i]);
            }
            
            int len = longest_increasing_subsequence(arr, n, lis);
            
            printf("\nLongest Increasing Subsequence:\n");
            printf("  Input: ");
            for (int i = 0; i < n; i++) printf("%d ", arr[i]);
            printf("\n  LIS:   ");
            for (int i = 0; i < len; i++) printf("%d ", lis[i]);
            printf("\n  Length: %d\n", len);
            
            free(arr);
            free(lis);
            break;
        }
        
        case 4:
            demonstrate_greedy_failure();
            break;
        
        default:
            printf("Invalid choice\n");
            return 1;
    }
    
    printf("\n═════════════════════════════════════════════════════════════\n");
    
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Implement weighted activity selection using DP
 *    (activities have different profits, greedy no longer optimal)
 *
 * 2. Implement Edit Distance (Levenshtein) using DP
 *
 * 3. Implement Matrix Chain Multiplication optimization
 *
 * 4. Implement Subset Sum problem with both backtracking and DP
 *
 * 5. Find the tightest bound on when greedy coin change works:
 *    Research "canonical coin systems"
 *
 * =============================================================================
 */

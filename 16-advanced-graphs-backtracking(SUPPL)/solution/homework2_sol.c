/**
 * =============================================================================
 * WEEK 16: ADVANCED GRAPH ALGORITHMS & BACKTRACKING
 * Homework 2 Solution - Backtracking Applications
 * =============================================================================
 *
 * This solution implements:
 *   Part A: Knight's Tour (with Warnsdorff's heuristic)
 *   Part B: Cryptarithmetic Solver (SEND + MORE = MONEY)
 *   Part C: Optimal Job Scheduling (with deadlines and profits)
 *
 * Usage:
 *   ./homework2 knight <size> [closed]
 *   ./homework2 crypto <puzzle_file>
 *   ./homework2 schedule <jobs_file>
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>

/* =============================================================================
 * PART A: KNIGHT'S TOUR
 * =============================================================================
 * The Knight's Tour problem: find a sequence of moves such that the knight
 * visits every square on an N×N chessboard exactly once.
 *
 * Open tour: Ends anywhere
 * Closed tour: Returns to starting square (Hamiltonian cycle)
 *
 * Warnsdorff's heuristic (1823): At each step, move the knight to the square
 * with the fewest onward moves. This dramatically improves performance.
 */

/* Knight movement offsets (8 possible L-shaped moves) */
static const int dx[] = {2, 1, -1, -2, -2, -1, 1, 2};
static const int dy[] = {1, 2, 2, 1, -1, -2, -2, -1};

typedef struct {
    int **board;
    int size;
    int moves;
    bool found;
} KnightTour;

KnightTour* kt_create(int n) {
    KnightTour *kt = (KnightTour *)malloc(sizeof(KnightTour));
    kt->size = n;
    kt->moves = 0;
    kt->found = false;
    
    kt->board = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        kt->board[i] = (int *)calloc(n, sizeof(int));
    }
    return kt;
}

void kt_destroy(KnightTour *kt) {
    for (int i = 0; i < kt->size; i++) {
        free(kt->board[i]);
    }
    free(kt->board);
    free(kt);
}

bool kt_is_valid(KnightTour *kt, int x, int y) {
    return x >= 0 && x < kt->size && y >= 0 && y < kt->size &&
           kt->board[x][y] == 0;
}

/* Count available moves from position (x, y) */
int kt_count_moves(KnightTour *kt, int x, int y) {
    int count = 0;
    for (int i = 0; i < 8; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (kt_is_valid(kt, nx, ny)) {
            count++;
        }
    }
    return count;
}

void kt_print_board(KnightTour *kt) {
    printf("\n   ");
    for (int j = 0; j < kt->size; j++) {
        printf(" %2c ", 'a' + j);
    }
    printf("\n");
    
    printf("   ┌");
    for (int j = 0; j < kt->size; j++) {
        printf("───");
        printf("%c", (j < kt->size - 1) ? '┬' : '┐');
    }
    printf("\n");
    
    for (int i = 0; i < kt->size; i++) {
        printf(" %2d│", i + 1);
        for (int j = 0; j < kt->size; j++) {
            if (kt->board[i][j] == 0) {
                printf("   │");
            } else {
                printf("%2d │", kt->board[i][j]);
            }
        }
        printf("\n");
        
        if (i < kt->size - 1) {
            printf("   ├");
            for (int j = 0; j < kt->size; j++) {
                printf("───");
                printf("%c", (j < kt->size - 1) ? '┼' : '┤');
            }
            printf("\n");
        }
    }
    
    printf("   └");
    for (int j = 0; j < kt->size; j++) {
        printf("───");
        printf("%c", (j < kt->size - 1) ? '┴' : '┘');
    }
    printf("\n");
}

/* Basic backtracking solver (no heuristic) */
bool kt_solve_basic(KnightTour *kt, int x, int y, int move, bool closed, int start_x, int start_y) {
    kt->board[x][y] = move;
    kt->moves++;
    
    if (move == kt->size * kt->size) {
        if (closed) {
            /* Check if we can return to start */
            for (int i = 0; i < 8; i++) {
                int nx = x + dx[i];
                int ny = y + dy[i];
                if (nx == start_x && ny == start_y) {
                    kt->found = true;
                    return true;
                }
            }
            kt->board[x][y] = 0;
            return false;
        }
        kt->found = true;
        return true;
    }
    
    for (int i = 0; i < 8; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        
        if (kt_is_valid(kt, nx, ny)) {
            if (kt_solve_basic(kt, nx, ny, move + 1, closed, start_x, start_y)) {
                return true;
            }
        }
    }
    
    kt->board[x][y] = 0;
    return false;
}

/* Warnsdorff's heuristic solver */
bool kt_solve_warnsdorff(KnightTour *kt, int x, int y, int move, bool closed, int start_x, int start_y) {
    kt->board[x][y] = move;
    kt->moves++;
    
    if (move == kt->size * kt->size) {
        if (closed) {
            for (int i = 0; i < 8; i++) {
                int nx = x + dx[i];
                int ny = y + dy[i];
                if (nx == start_x && ny == start_y) {
                    kt->found = true;
                    return true;
                }
            }
            kt->board[x][y] = 0;
            return false;
        }
        kt->found = true;
        return true;
    }
    
    /* Order moves by Warnsdorff's heuristic */
    typedef struct { int x, y, score; } Move;
    Move moves[8];
    int num_moves = 0;
    
    for (int i = 0; i < 8; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        
        if (kt_is_valid(kt, nx, ny)) {
            moves[num_moves].x = nx;
            moves[num_moves].y = ny;
            moves[num_moves].score = kt_count_moves(kt, nx, ny);
            num_moves++;
        }
    }
    
    /* Sort moves by score (ascending) - prefer squares with fewer exits */
    for (int i = 0; i < num_moves - 1; i++) {
        for (int j = i + 1; j < num_moves; j++) {
            if (moves[j].score < moves[i].score) {
                Move tmp = moves[i];
                moves[i] = moves[j];
                moves[j] = tmp;
            }
        }
    }
    
    for (int i = 0; i < num_moves; i++) {
        if (kt_solve_warnsdorff(kt, moves[i].x, moves[i].y, move + 1, 
                                closed, start_x, start_y)) {
            return true;
        }
    }
    
    kt->board[x][y] = 0;
    return false;
}

void solve_knight(int size, bool closed) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     PART A: KNIGHT'S TOUR                                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("  Board size: %d×%d\n", size, size);
    printf("  Tour type:  %s\n", closed ? "Closed (Hamiltonian cycle)" : "Open");
    
    /* Method 1: Basic backtracking (only for small boards) */
    if (size <= 6) {
        printf("\n  Method 1: Basic Backtracking\n");
        printf("  ════════════════════════════\n");
        
        KnightTour *kt = kt_create(size);
        clock_t start = clock();
        bool found = kt_solve_basic(kt, 0, 0, 1, closed, 0, 0);
        clock_t end = clock();
        
        if (found) {
            printf("  Solution found!\n");
            kt_print_board(kt);
        } else {
            printf("  No solution found.\n");
        }
        printf("  Moves explored: %d\n", kt->moves);
        printf("  Time: %.3f ms\n", (double)(end - start) * 1000.0 / CLOCKS_PER_SEC);
        
        kt_destroy(kt);
    } else {
        printf("\n  (Basic backtracking skipped for large boards - too slow)\n");
    }
    
    /* Method 2: Warnsdorff's heuristic */
    printf("\n  Method 2: Warnsdorff's Heuristic\n");
    printf("  ═════════════════════════════════\n");
    
    KnightTour *kt = kt_create(size);
    clock_t start = clock();
    bool found = kt_solve_warnsdorff(kt, 0, 0, 1, closed, 0, 0);
    clock_t end = clock();
    
    if (found) {
        printf("  Solution found!\n");
        if (size <= 12) {
            kt_print_board(kt);
        } else {
            printf("  (Board too large to display)\n");
        }
    } else {
        printf("  No solution found.\n");
    }
    printf("  Moves explored: %d\n", kt->moves);
    printf("  Time: %.3f ms\n", (double)(end - start) * 1000.0 / CLOCKS_PER_SEC);
    
    kt_destroy(kt);
}

/* =============================================================================
 * PART B: CRYPTARITHMETIC SOLVER
 * =============================================================================
 * Solve puzzles like SEND + MORE = MONEY where each letter represents a
 * unique digit (0-9). Leading digits cannot be zero.
 */

#define MAX_WORD_LEN 20
#define MAX_WORDS 10

typedef struct {
    char words[MAX_WORDS][MAX_WORD_LEN];
    int num_addends;  /* Number of words being added (result is last) */
    char letters[26]; /* Unique letters in puzzle */
    int num_letters;
    int digit[26];    /* digit[i] = digit assigned to letter i */
    bool used[10];    /* used[d] = true if digit d is assigned */
    bool leading[26]; /* leading[i] = true if letter i starts a word */
    int solutions;
    int nodes;
} CryptoPuzzle;

void cp_init(CryptoPuzzle *cp) {
    memset(cp->digit, -1, sizeof(cp->digit));
    memset(cp->used, false, sizeof(cp->used));
    memset(cp->leading, false, sizeof(cp->leading));
    cp->num_letters = 0;
    cp->solutions = 0;
    cp->nodes = 0;
}

void cp_add_letter(CryptoPuzzle *cp, char c) {
    int idx = toupper(c) - 'A';
    for (int i = 0; i < cp->num_letters; i++) {
        if (cp->letters[i] == idx) return;
    }
    cp->letters[cp->num_letters++] = idx;
}

long cp_word_to_number(CryptoPuzzle *cp, const char *word) {
    long num = 0;
    for (int i = 0; word[i]; i++) {
        int idx = toupper(word[i]) - 'A';
        num = num * 10 + cp->digit[idx];
    }
    return num;
}

bool cp_check_solution(CryptoPuzzle *cp) {
    long sum = 0;
    for (int i = 0; i < cp->num_addends; i++) {
        sum += cp_word_to_number(cp, cp->words[i]);
    }
    return sum == cp_word_to_number(cp, cp->words[cp->num_addends]);
}

void cp_print_solution(CryptoPuzzle *cp) {
    printf("\n   Solution found:\n");
    printf("   ────────────────────────────────────\n");
    
    /* Print letter assignments */
    printf("   ");
    for (int i = 0; i < cp->num_letters; i++) {
        printf(" %c", 'A' + cp->letters[i]);
    }
    printf("\n   ");
    for (int i = 0; i < cp->num_letters; i++) {
        printf(" %d", cp->digit[cp->letters[i]]);
    }
    printf("\n\n");
    
    /* Print equation */
    long sum = 0;
    for (int i = 0; i < cp->num_addends; i++) {
        long val = cp_word_to_number(cp, cp->words[i]);
        sum += val;
        printf("   %s%s = %ld\n", (i == 0) ? "  " : "+ ", cp->words[i], val);
    }
    printf("   ────────────────────────────────────\n");
    printf("   %s = %ld\n", cp->words[cp->num_addends], 
           cp_word_to_number(cp, cp->words[cp->num_addends]));
}

bool cp_solve(CryptoPuzzle *cp, int idx) {
    cp->nodes++;
    
    if (idx == cp->num_letters) {
        if (cp_check_solution(cp)) {
            cp->solutions++;
            cp_print_solution(cp);
            return false;  /* Continue searching for more solutions */
        }
        return false;
    }
    
    int letter = cp->letters[idx];
    
    for (int d = 0; d <= 9; d++) {
        /* Leading letters cannot be zero */
        if (d == 0 && cp->leading[letter]) continue;
        
        if (!cp->used[d]) {
            cp->digit[letter] = d;
            cp->used[d] = true;
            
            cp_solve(cp, idx + 1);
            
            cp->digit[letter] = -1;
            cp->used[d] = false;
        }
    }
    
    return false;
}

void solve_crypto(const char *puzzle) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     PART B: CRYPTARITHMETIC SOLVER                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    CryptoPuzzle cp;
    cp_init(&cp);
    
    /* Parse puzzle: WORD1 + WORD2 = RESULT or WORD1 WORD2 RESULT */
    char temp[256];
    strncpy(temp, puzzle, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    
    /* Replace + and = with spaces */
    for (char *p = temp; *p; p++) {
        if (*p == '+' || *p == '=') *p = ' ';
    }
    
    /* Extract words */
    char *token = strtok(temp, " ");
    int word_count = 0;
    while (token && word_count < MAX_WORDS) {
        strcpy(cp.words[word_count], token);
        
        /* Mark leading letter */
        cp.leading[toupper(token[0]) - 'A'] = true;
        
        /* Collect unique letters */
        for (int i = 0; token[i]; i++) {
            cp_add_letter(&cp, token[i]);
        }
        
        word_count++;
        token = strtok(NULL, " ");
    }
    
    cp.num_addends = word_count - 1;
    
    printf("  Puzzle: ");
    for (int i = 0; i < cp.num_addends; i++) {
        printf("%s%s", (i > 0) ? " + " : "", cp.words[i]);
    }
    printf(" = %s\n", cp.words[cp.num_addends]);
    
    printf("  Unique letters: %d (", cp.num_letters);
    for (int i = 0; i < cp.num_letters; i++) {
        printf("%c", 'A' + cp.letters[i]);
    }
    printf(")\n");
    
    if (cp.num_letters > 10) {
        printf("\n  ⚠ Error: More than 10 unique letters - no solution possible.\n\n");
        return;
    }
    
    printf("\n  Searching for solutions...\n");
    
    clock_t start = clock();
    cp_solve(&cp, 0);
    clock_t end = clock();
    
    printf("\n  ═══════════════════════════════════════════════════════════════\n");
    printf("  RESULTS\n");
    printf("  ═══════════════════════════════════════════════════════════════\n\n");
    printf("  Solutions found: %d\n", cp.solutions);
    printf("  Nodes explored:  %d\n", cp.nodes);
    printf("  Time: %.3f ms\n\n", (double)(end - start) * 1000.0 / CLOCKS_PER_SEC);
}

/* =============================================================================
 * PART C: OPTIMAL JOB SCHEDULING
 * =============================================================================
 * Given jobs with deadlines and profits, find the optimal schedule.
 * Each job takes unit time. Maximise total profit.
 *
 * Approaches:
 *   1. Greedy: Sort by profit, schedule if deadline slot available
 *   2. Backtracking: Explore all valid schedules
 */

#define MAX_JOBS 50

typedef struct {
    char name[32];
    int deadline;
    int profit;
} Job;

typedef struct {
    Job jobs[MAX_JOBS];
    int num_jobs;
    int max_deadline;
} JobSchedule;

/* Comparison for sorting by profit (descending) */
int compare_jobs_profit(const void *a, const void *b) {
    return ((Job *)b)->profit - ((Job *)a)->profit;
}

void js_greedy(JobSchedule *js) {
    printf("\n  Method 1: Greedy Algorithm\n");
    printf("  ═══════════════════════════\n");
    
    /* Sort jobs by profit (descending) */
    Job sorted[MAX_JOBS];
    memcpy(sorted, js->jobs, js->num_jobs * sizeof(Job));
    qsort(sorted, js->num_jobs, sizeof(Job), compare_jobs_profit);
    
    /* Schedule array (-1 = free) */
    int schedule[MAX_JOBS];
    for (int i = 0; i < js->max_deadline; i++) {
        schedule[i] = -1;
    }
    
    int total_profit = 0;
    int jobs_scheduled = 0;
    
    for (int i = 0; i < js->num_jobs; i++) {
        /* Find latest free slot before deadline */
        for (int t = sorted[i].deadline - 1; t >= 0; t--) {
            if (schedule[t] == -1) {
                schedule[t] = i;
                total_profit += sorted[i].profit;
                jobs_scheduled++;
                break;
            }
        }
    }
    
    printf("  Schedule (greedy by profit):\n\n");
    printf("   Time:  ");
    for (int t = 0; t < js->max_deadline; t++) {
        printf(" %2d ", t + 1);
    }
    printf("\n   Job:   ");
    for (int t = 0; t < js->max_deadline; t++) {
        if (schedule[t] == -1) {
            printf(" -- ");
        } else {
            printf(" %s ", sorted[schedule[t]].name);
        }
    }
    printf("\n   Profit:");
    for (int t = 0; t < js->max_deadline; t++) {
        if (schedule[t] == -1) {
            printf("  0 ");
        } else {
            printf(" %2d ", sorted[schedule[t]].profit);
        }
    }
    printf("\n\n");
    printf("  Jobs scheduled: %d\n", jobs_scheduled);
    printf("  Total profit:   %d\n", total_profit);
}

/* Backtracking for optimal schedule */
static int best_profit;
static int best_schedule[MAX_JOBS];
static int current_schedule[MAX_JOBS];

void js_backtrack(JobSchedule *js, int job_idx, int *schedule, int current_profit) {
    if (current_profit + (js->num_jobs - job_idx) * 100 <= best_profit) {
        return;  /* Pruning: can't beat best even with max remaining profit */
    }
    
    if (job_idx == js->num_jobs) {
        if (current_profit > best_profit) {
            best_profit = current_profit;
            memcpy(best_schedule, schedule, js->max_deadline * sizeof(int));
        }
        return;
    }
    
    Job *job = &js->jobs[job_idx];
    
    /* Try scheduling this job */
    for (int t = job->deadline - 1; t >= 0; t--) {
        if (schedule[t] == -1) {
            schedule[t] = job_idx;
            js_backtrack(js, job_idx + 1, schedule, current_profit + job->profit);
            schedule[t] = -1;
            break;  /* Only try one slot per job (optimization) */
        }
    }
    
    /* Try not scheduling this job */
    js_backtrack(js, job_idx + 1, schedule, current_profit);
}

void js_optimal(JobSchedule *js) {
    printf("\n  Method 2: Backtracking (Optimal)\n");
    printf("  ══════════════════════════════════\n");
    
    int schedule[MAX_JOBS];
    for (int i = 0; i < js->max_deadline; i++) {
        schedule[i] = -1;
        best_schedule[i] = -1;
    }
    best_profit = 0;
    
    clock_t start = clock();
    js_backtrack(js, 0, schedule, 0);
    clock_t end = clock();
    
    printf("  Optimal schedule:\n\n");
    printf("   Time:  ");
    for (int t = 0; t < js->max_deadline; t++) {
        printf(" %2d ", t + 1);
    }
    printf("\n   Job:   ");
    int jobs_scheduled = 0;
    for (int t = 0; t < js->max_deadline; t++) {
        if (best_schedule[t] == -1) {
            printf(" -- ");
        } else {
            printf(" %s ", js->jobs[best_schedule[t]].name);
            jobs_scheduled++;
        }
    }
    printf("\n   Profit:");
    for (int t = 0; t < js->max_deadline; t++) {
        if (best_schedule[t] == -1) {
            printf("  0 ");
        } else {
            printf(" %2d ", js->jobs[best_schedule[t]].profit);
        }
    }
    printf("\n\n");
    printf("  Jobs scheduled: %d\n", jobs_scheduled);
    printf("  Total profit:   %d\n", best_profit);
    printf("  Time: %.3f ms\n", (double)(end - start) * 1000.0 / CLOCKS_PER_SEC);
}

void solve_schedule(const char *filename) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     PART C: OPTIMAL JOB SCHEDULING                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    JobSchedule js;
    js.num_jobs = 0;
    js.max_deadline = 0;
    
    /* Read jobs from file or use demo data */
    FILE *f = fopen(filename, "r");
    if (f) {
        while (fscanf(f, "%s %d %d", js.jobs[js.num_jobs].name,
                      &js.jobs[js.num_jobs].deadline,
                      &js.jobs[js.num_jobs].profit) == 3) {
            if (js.jobs[js.num_jobs].deadline > js.max_deadline) {
                js.max_deadline = js.jobs[js.num_jobs].deadline;
            }
            js.num_jobs++;
        }
        fclose(f);
    } else {
        /* Demo data */
        strcpy(js.jobs[0].name, "J1"); js.jobs[0].deadline = 2; js.jobs[0].profit = 100;
        strcpy(js.jobs[1].name, "J2"); js.jobs[1].deadline = 1; js.jobs[1].profit = 19;
        strcpy(js.jobs[2].name, "J3"); js.jobs[2].deadline = 2; js.jobs[2].profit = 27;
        strcpy(js.jobs[3].name, "J4"); js.jobs[3].deadline = 1; js.jobs[3].profit = 25;
        strcpy(js.jobs[4].name, "J5"); js.jobs[4].deadline = 3; js.jobs[4].profit = 15;
        js.num_jobs = 5;
        js.max_deadline = 3;
    }
    
    printf("  Input Jobs:\n");
    printf("  ┌────────┬──────────┬─────────┐\n");
    printf("  │  Job   │ Deadline │ Profit  │\n");
    printf("  ├────────┼──────────┼─────────┤\n");
    for (int i = 0; i < js.num_jobs; i++) {
        printf("  │  %s   │    %2d    │   %3d   │\n",
               js.jobs[i].name, js.jobs[i].deadline, js.jobs[i].profit);
    }
    printf("  └────────┴──────────┴─────────┘\n");
    printf("\n  Max deadline: %d time units\n", js.max_deadline);
    
    js_greedy(&js);
    js_optimal(&js);
    
    printf("\n");
}

/* =============================================================================
 * MAIN FUNCTION
 * =============================================================================
 */

void print_usage(const char *prog) {
    printf("\nUsage: %s <problem> [options]\n\n", prog);
    printf("Problems:\n");
    printf("  knight <size> [closed]  - Knight's Tour\n");
    printf("  crypto <puzzle>         - Cryptarithmetic solver\n");
    printf("  schedule <file>         - Optimal job scheduling\n");
    printf("\nExamples:\n");
    printf("  %s knight 8\n", prog);
    printf("  %s knight 5 closed\n", prog);
    printf("  %s crypto \"SEND + MORE = MONEY\"\n", prog);
    printf("  %s schedule jobs.txt\n", prog);
    printf("\n");
}

int main(int argc, char *argv[]) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 16: BACKTRACKING APPLICATIONS                        ║\n");
    printf("║              Homework 2 Solution                              ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    if (argc < 2) {
        print_usage(argv[0]);
        
        printf("  Running demonstration with default parameters...\n");
        
        /* Demo all three problems */
        solve_knight(5, false);
        solve_crypto("SEND + MORE = MONEY");
        solve_schedule("nonexistent.txt");  /* Uses demo data */
        
        return 0;
    }
    
    const char *problem = argv[1];
    
    if (strcmp(problem, "knight") == 0) {
        int size = (argc > 2) ? atoi(argv[2]) : 5;
        bool closed = (argc > 3) && strcmp(argv[3], "closed") == 0;
        solve_knight(size, closed);
        
    } else if (strcmp(problem, "crypto") == 0) {
        const char *puzzle = (argc > 2) ? argv[2] : "SEND + MORE = MONEY";
        solve_crypto(puzzle);
        
    } else if (strcmp(problem, "schedule") == 0) {
        const char *filename = (argc > 2) ? argv[2] : "jobs.txt";
        solve_schedule(filename);
        
    } else {
        fprintf(stderr, "Error: Unknown problem '%s'\n", problem);
        print_usage(argv[0]);
        return 1;
    }
    
    return 0;
}

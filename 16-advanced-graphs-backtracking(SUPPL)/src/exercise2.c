/**
 * =============================================================================
 * WEEK 16: ADVANCED GRAPH ALGORITHMS & BACKTRACKING
 * Exercise 2: Backtracking Problems
 * =============================================================================
 *
 * In this exercise, you will implement:
 *   1. N-Queens solver that finds all solutions
 *   2. Count all solutions for N = 8 (should be 92)
 *   3. Sudoku solver with constraint checking
 *   4. Hamiltonian path detection
 *   5. Solution counting and backtrack statistics
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c -lm
 * Testing: make test-ex2
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

#define MAX_N         15
#define BOARD_SIZE    9
#define MAX_VERTICES  20

/* Global statistics */
static int g_solutions = 0;
static int g_backtracks = 0;
static int g_nodes_explored = 0;

/* =============================================================================
 * PART 1: N-QUEENS PROBLEM
 * =============================================================================
 *
 * The N-Queens problem asks: place N queens on an N×N chessboard such that
 * no two queens threaten each other.
 *
 * Queens can attack along:
 *   - Rows (horizontal)
 *   - Columns (vertical)
 *   - Diagonals (both directions)
 *
 * Representation: board[i] = column position of queen in row i
 * This automatically ensures no two queens share a row.
 *
 * Checking safety: For row `row` and column `col`:
 *   - Check all previous rows (0 to row-1)
 *   - Column conflict: board[i] == col
 *   - Diagonal conflict: |board[i] - col| == |i - row|
 */

/**
 * Check if placing a queen at (row, col) is safe
 *
 * @param board Array where board[i] = column of queen in row i
 * @param row   Row to place queen
 * @param col   Column to place queen
 * @param n     Board size
 * @return      true if placement is safe
 *
 * TODO 1: Implement safety check for N-Queens
 *         Check all queens in rows 0 to row-1:
 *         - Same column: board[i] == col
 *         - Same diagonal: |board[i] - col| == |i - row|
 *
 * HINT: Use abs() for absolute value
 */
bool is_safe_queens(int board[], int row, int col, int n) {
    /* TODO: Implement safety check */
    (void)n;  /* Unused in this implementation */
    
    /* HINT:
     * for (int i = 0; i < row; i++) {
     *     // Check column conflict
     *     if (board[i] == col) return false;
     *     
     *     // Check diagonal conflict
     *     if (abs(board[i] - col) == abs(i - row)) return false;
     * }
     * return true;
     */
    
    (void)board;
    (void)col;
    return true;  /* Placeholder - always returns true */
}

/**
 * Print the N-Queens board
 */
void print_queens_board(int board[], int n) {
    printf("  ");
    for (int j = 0; j < n; j++) printf(" %d", j);
    printf("\n  ┌");
    for (int j = 0; j < n; j++) printf("──");
    printf("─┐\n");
    
    for (int i = 0; i < n; i++) {
        printf("%d │", i);
        for (int j = 0; j < n; j++) {
            if (board[i] == j) {
                printf(" Q");
            } else {
                printf(" .");
            }
        }
        printf(" │\n");
    }
    
    printf("  └");
    for (int j = 0; j < n; j++) printf("──");
    printf("─┘\n");
}

/**
 * Solve N-Queens using backtracking
 *
 * @param board     Array to store queen positions
 * @param row       Current row being processed
 * @param n         Board size
 * @param count_only If true, only count solutions (don't print)
 *
 * TODO 2: Implement N-Queens backtracking solver
 *         Base case: row == n means all queens placed (found solution)
 *         Recursive case: try each column in current row
 *         - If safe, place queen and recurse
 *         - After recursion, remove queen (backtrack)
 *
 * HINT: Use g_solutions to count solutions
 *       Use g_backtracks to count backtrack operations
 */
void solve_nqueens(int board[], int row, int n, bool count_only) {
    g_nodes_explored++;
    
    /* TODO: Implement backtracking solver */
    
    /* Base case: all queens placed */
    if (row == n) {
        g_solutions++;
        if (!count_only && g_solutions <= 3) {
            printf("\nSolution #%d:\n", g_solutions);
            print_queens_board(board, n);
        }
        return;
    }
    
    /* Try each column in this row */
    /* HINT:
     * for (int col = 0; col < n; col++) {
     *     if (is_safe_queens(board, row, col, n)) {
     *         board[row] = col;            // Place queen
     *         solve_nqueens(board, row + 1, n, count_only);
     *         board[row] = -1;             // Remove queen (backtrack)
     *         g_backtracks++;
     *     }
     * }
     */
    
    /* Placeholder - no actual solving */
    (void)board;
    (void)count_only;
}

/* =============================================================================
 * PART 2: SUDOKU SOLVER
 * =============================================================================
 *
 * Sudoku rules:
 *   - 9×9 grid divided into 3×3 boxes
 *   - Each row must contain digits 1-9 exactly once
 *   - Each column must contain digits 1-9 exactly once
 *   - Each 3×3 box must contain digits 1-9 exactly once
 *
 * Backtracking approach:
 *   1. Find an empty cell
 *   2. Try digits 1-9
 *   3. If valid, place digit and recurse
 *   4. If recursion fails, remove digit (backtrack)
 */

/**
 * Check if placing digit at (row, col) is valid
 *
 * @param grid  9×9 Sudoku grid (0 = empty)
 * @param row   Row position
 * @param col   Column position
 * @param digit Digit to place (1-9)
 * @return      true if placement is valid
 *
 * TODO 3: Implement Sudoku validity check
 *         Check three constraints:
 *         1. Row: no duplicate in row
 *         2. Column: no duplicate in column
 *         3. Box: no duplicate in 3×3 box
 *
 * HINT: For box, calculate top-left corner: (row/3)*3, (col/3)*3
 */
bool is_valid_sudoku(int grid[BOARD_SIZE][BOARD_SIZE], int row, int col, int digit) {
    /* TODO: Implement validity check */
    
    /* Check row */
    /* HINT:
     * for (int j = 0; j < BOARD_SIZE; j++) {
     *     if (grid[row][j] == digit) return false;
     * }
     */
    
    /* Check column */
    /* HINT:
     * for (int i = 0; i < BOARD_SIZE; i++) {
     *     if (grid[i][col] == digit) return false;
     * }
     */
    
    /* Check 3x3 box */
    /* HINT:
     * int box_row = (row / 3) * 3;
     * int box_col = (col / 3) * 3;
     * for (int i = 0; i < 3; i++) {
     *     for (int j = 0; j < 3; j++) {
     *         if (grid[box_row + i][box_col + j] == digit) return false;
     *     }
     * }
     */
    
    (void)grid;
    (void)row;
    (void)col;
    (void)digit;
    
    return true;  /* Placeholder - always returns true */
}

/**
 * Find next empty cell in grid
 *
 * @param grid 9×9 Sudoku grid
 * @param row  Pointer to store row of empty cell
 * @param col  Pointer to store column of empty cell
 * @return     true if empty cell found, false if grid is full
 */
bool find_empty(int grid[BOARD_SIZE][BOARD_SIZE], int *row, int *col) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (grid[i][j] == 0) {
                *row = i;
                *col = j;
                return true;
            }
        }
    }
    return false;
}

/**
 * Print Sudoku grid
 */
void print_sudoku(int grid[BOARD_SIZE][BOARD_SIZE]) {
    printf("  ┌───────┬───────┬───────┐\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (i > 0 && i % 3 == 0) {
            printf("  ├───────┼───────┼───────┤\n");
        }
        printf("  │");
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (j > 0 && j % 3 == 0) printf(" │");
            if (grid[i][j] == 0) {
                printf(" .");
            } else {
                printf(" %d", grid[i][j]);
            }
        }
        printf(" │\n");
    }
    printf("  └───────┴───────┴───────┘\n");
}

/**
 * Solve Sudoku using backtracking
 *
 * @param grid 9×9 Sudoku grid (0 = empty)
 * @return     true if solved, false if no solution
 *
 * TODO 4: Implement Sudoku solver
 *         1. Find empty cell (if none, puzzle solved)
 *         2. Try digits 1-9
 *         3. If valid, place and recurse
 *         4. If recursion fails, backtrack
 *
 * HINT: Return true immediately when solution found
 */
bool solve_sudoku(int grid[BOARD_SIZE][BOARD_SIZE]) {
    g_nodes_explored++;
    
    /* TODO: Implement Sudoku solver */
    
    int row, col;
    
    /* Find empty cell */
    if (!find_empty(grid, &row, &col)) {
        return true;  /* No empty cells - puzzle solved! */
    }
    
    /* Try digits 1-9 */
    /* HINT:
     * for (int digit = 1; digit <= 9; digit++) {
     *     if (is_valid_sudoku(grid, row, col, digit)) {
     *         grid[row][col] = digit;      // Place digit
     *         
     *         if (solve_sudoku(grid)) {
     *             return true;              // Found solution
     *         }
     *         
     *         grid[row][col] = 0;           // Backtrack
     *         g_backtracks++;
     *     }
     * }
     */
    
    return false;  /* Placeholder - no solving */
}

/* =============================================================================
 * PART 3: HAMILTONIAN PATH
 * =============================================================================
 *
 * A Hamiltonian path visits every vertex exactly once.
 * Finding a Hamiltonian path is NP-complete.
 *
 * Backtracking approach:
 *   1. Start from a vertex
 *   2. Try to extend path to unvisited adjacent vertices
 *   3. If path has V vertices, success
 *   4. If stuck, backtrack
 */

/**
 * Check if vertex v can be added to path
 */
bool is_safe_hamiltonian(int adj[MAX_VERTICES][MAX_VERTICES], int path[], int pos, int v, int V) {
    /* Check if v is adjacent to previous vertex */
    if (adj[path[pos - 1]][v] == 0) return false;
    
    /* Check if v is already in path */
    for (int i = 0; i < pos; i++) {
        if (path[i] == v) return false;
    }
    
    (void)V;  /* Unused */
    return true;
}

/**
 * Solve Hamiltonian Path using backtracking
 *
 * @param adj  Adjacency matrix
 * @param path Array to store path
 * @param pos  Current position in path
 * @param V    Number of vertices
 * @return     true if path found
 *
 * TODO 5: Implement Hamiltonian path solver
 *         Base case: pos == V (visited all vertices)
 *         Recursive case: try each unvisited adjacent vertex
 *
 * HINT: Use is_safe_hamiltonian() to check if vertex can be added
 */
bool hamiltonian_util(int adj[MAX_VERTICES][MAX_VERTICES], int path[], int pos, int V) {
    g_nodes_explored++;
    
    /* TODO: Implement backtracking solver */
    
    /* Base case: all vertices included */
    if (pos == V) {
        return true;
    }
    
    /* Try all vertices as next candidate */
    /* HINT:
     * for (int v = 0; v < V; v++) {
     *     if (is_safe_hamiltonian(adj, path, pos, v, V)) {
     *         path[pos] = v;
     *         
     *         if (hamiltonian_util(adj, path, pos + 1, V)) {
     *             return true;
     *         }
     *         
     *         path[pos] = -1;  // Backtrack
     *         g_backtracks++;
     *     }
     * }
     */
    
    (void)adj;
    (void)path;
    
    return false;
}

/**
 * Find Hamiltonian path starting from vertex 0
 */
bool find_hamiltonian_path(int adj[MAX_VERTICES][MAX_VERTICES], int V, int path[]) {
    /* Initialise path */
    for (int i = 0; i < V; i++) {
        path[i] = -1;
    }
    
    /* Start from vertex 0 */
    path[0] = 0;
    
    return hamiltonian_util(adj, path, 1, V);
}

/* =============================================================================
 * TESTING AND DEMONSTRATION
 * =============================================================================
 */

void test_nqueens(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      Testing N-Queens Solver                                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Test 4-Queens */
    printf("4-Queens (all solutions):\n");
    int board4[MAX_N];
    for (int i = 0; i < 4; i++) board4[i] = -1;
    
    g_solutions = 0;
    g_backtracks = 0;
    g_nodes_explored = 0;
    solve_nqueens(board4, 0, 4, false);
    
    printf("\nStatistics for 4-Queens:\n");
    printf("  Solutions found: %d (expected: 2)\n", g_solutions);
    printf("  Nodes explored: %d\n", g_nodes_explored);
    printf("  Backtracks: %d\n", g_backtracks);
    
    /* Test 8-Queens (count only) */
    printf("\n8-Queens (first 3 solutions shown):\n");
    int board8[MAX_N];
    for (int i = 0; i < 8; i++) board8[i] = -1;
    
    g_solutions = 0;
    g_backtracks = 0;
    g_nodes_explored = 0;
    solve_nqueens(board8, 0, 8, false);
    
    printf("\nStatistics for 8-Queens:\n");
    printf("  Solutions found: %d (expected: 92)\n", g_solutions);
    printf("  Nodes explored: %d\n", g_nodes_explored);
    printf("  Backtracks: %d\n", g_backtracks);
}

void test_sudoku(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      Testing Sudoku Solver                                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int puzzle[BOARD_SIZE][BOARD_SIZE] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };
    
    printf("Original puzzle:\n");
    print_sudoku(puzzle);
    
    g_nodes_explored = 0;
    g_backtracks = 0;
    
    printf("\nSolving...\n");
    if (solve_sudoku(puzzle)) {
        printf("\nSolution:\n");
        print_sudoku(puzzle);
    } else {
        printf("\nNo solution found (check your implementation)\n");
    }
    
    printf("\nStatistics:\n");
    printf("  Nodes explored: %d\n", g_nodes_explored);
    printf("  Backtracks: %d\n", g_backtracks);
}

void test_hamiltonian(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      Testing Hamiltonian Path                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int V = 5;
    int adj[MAX_VERTICES][MAX_VERTICES] = {
        {0, 1, 0, 1, 0},
        {1, 0, 1, 1, 1},
        {0, 1, 0, 0, 1},
        {1, 1, 0, 0, 1},
        {0, 1, 1, 1, 0}
    };
    
    printf("Graph (adjacency matrix):\n");
    printf("    0 1 2 3 4\n");
    for (int i = 0; i < V; i++) {
        printf("  %d:", i);
        for (int j = 0; j < V; j++) {
            printf(" %d", adj[i][j]);
        }
        printf("\n");
    }
    
    int path[MAX_VERTICES];
    g_nodes_explored = 0;
    g_backtracks = 0;
    
    printf("\nSearching for Hamiltonian path starting from vertex 0...\n");
    if (find_hamiltonian_path(adj, V, path)) {
        printf("Path found: ");
        for (int i = 0; i < V; i++) {
            printf("%d", path[i]);
            if (i < V - 1) printf(" -> ");
        }
        printf("\n");
    } else {
        printf("No Hamiltonian path found (check your implementation)\n");
    }
    
    printf("\nStatistics:\n");
    printf("  Nodes explored: %d\n", g_nodes_explored);
    printf("  Backtracks: %d\n", g_backtracks);
}

/* =============================================================================
 * MAIN FUNCTION
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 16: EXERCISE 2                                       ║\n");
    printf("║     Backtracking Problems                                     ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    test_nqueens();
    test_sudoku();
    test_hamiltonian();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    EXERCISE COMPLETE                          ║\n");
    printf("║                                                               ║\n");
    printf("║  Implement the TODOs:                                         ║\n");
    printf("║    TODO 1: N-Queens safety check                              ║\n");
    printf("║    TODO 2: N-Queens backtracking solver                       ║\n");
    printf("║    TODO 3: Sudoku validity check                              ║\n");
    printf("║    TODO 4: Sudoku backtracking solver                         ║\n");
    printf("║    TODO 5: Hamiltonian path solver                            ║\n");
    printf("║                                                               ║\n");
    printf("║  Run 'make test-ex2' to verify your implementation            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

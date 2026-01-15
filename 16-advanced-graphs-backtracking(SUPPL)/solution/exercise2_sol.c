/**
 * =============================================================================
 * WEEK 16: ADVANCED GRAPH ALGORITHMS & BACKTRACKING
 * Exercise 2 Solution - Complete Backtracking Implementations
 * =============================================================================
 *
 * This solution provides complete implementations for:
 *   1. N-Queens problem with all solutions enumeration
 *   2. Sudoku solver with constraint propagation
 *   3. Hamiltonian path detection in graphs
 *
 * Each implementation includes:
 *   - Full working code with optimisations
 *   - Statistics tracking (nodes explored, backtracks)
 *   - Visualisation of solutions
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c
 * Usage: ./exercise2_sol
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

/* =============================================================================
 * GLOBAL STATISTICS
 * =============================================================================
 */

static int g_solutions = 0;
static int g_backtracks = 0;
static int g_nodes_explored = 0;

/* =============================================================================
 * PART 1: N-QUEENS PROBLEM - COMPLETE SOLUTION
 * =============================================================================
 *
 * The N-Queens problem asks: place N queens on an N×N chessboard such that
 * no two queens threaten each other. Queens attack along rows, columns,
 * and both diagonals.
 *
 * Key insight: Since we place exactly one queen per row, we can represent
 * a configuration as an array col[row] = column where queen in row is placed.
 *
 * Complexity:
 *   - Time: O(N!) in worst case, but pruning significantly reduces this
 *   - Space: O(N) for the configuration array
 *
 * Historical note: The 8-Queens problem was first posed by chess composer
 * Max Bezzel in 1848. Carl Friedrich Gauss worked on it but didn't find
 * all 92 solutions.
 */

/**
 * Check if placing a queen at (row, col) is safe.
 * 
 * We only need to check rows 0 to row-1 since we place queens row by row.
 *
 * @param col Configuration array (col[i] = column of queen in row i)
 * @param row Current row where we want to place
 * @param c   Column where we want to place
 * @return    true if placement is safe, false otherwise
 *
 * Diagonal check explanation:
 *   - Same diagonal (↘): row - col is constant
 *   - Same anti-diagonal (↙): row + col is constant
 *   
 *   Equivalently: |row1 - row2| == |col1 - col2| means same diagonal
 */
bool is_safe_nqueens(int *col, int row, int c) {
    for (int i = 0; i < row; i++) {
        /* Check same column */
        if (col[i] == c) {
            return false;
        }
        
        /* Check diagonals: if |row_diff| == |col_diff|, same diagonal */
        int row_diff = row - i;
        int col_diff = (c > col[i]) ? (c - col[i]) : (col[i] - c);
        
        if (row_diff == col_diff) {
            return false;
        }
    }
    return true;
}

/**
 * Print a chessboard configuration with Unicode art.
 *
 * @param col Configuration array
 * @param n   Board size
 */
void print_board(int *col, int n) {
    printf("    ");
    for (int c = 0; c < n; c++) {
        printf(" %c  ", 'a' + c);
    }
    printf("\n");
    
    printf("   ┌");
    for (int c = 0; c < n; c++) {
        printf("───");
        printf("%c", (c < n - 1) ? '┬' : '┐');
    }
    printf("\n");
    
    for (int r = 0; r < n; r++) {
        printf(" %d │", r + 1);
        for (int c = 0; c < n; c++) {
            if (col[r] == c) {
                printf(" ♛ │");
            } else {
                /* Checkerboard pattern */
                if ((r + c) % 2 == 0) {
                    printf("   │");
                } else {
                    printf(" · │");
                }
            }
        }
        printf("\n");
        
        if (r < n - 1) {
            printf("   ├");
            for (int c = 0; c < n; c++) {
                printf("───");
                printf("%c", (c < n - 1) ? '┼' : '┤');
            }
            printf("\n");
        }
    }
    
    printf("   └");
    for (int c = 0; c < n; c++) {
        printf("───");
        printf("%c", (c < n - 1) ? '┴' : '┘');
    }
    printf("\n");
}

/**
 * Recursive backtracking solver for N-Queens.
 *
 * @param col       Configuration array
 * @param row       Current row to fill
 * @param n         Board size
 * @param print_all Whether to print all solutions
 */
void solve_nqueens(int *col, int row, int n, bool print_all) {
    g_nodes_explored++;
    
    /* Base case: all queens placed successfully */
    if (row == n) {
        g_solutions++;
        if (print_all && g_solutions <= 10) {  /* Print first 10 solutions */
            printf("\n  Solution #%d:\n", g_solutions);
            print_board(col, n);
        }
        return;
    }
    
    /* Try placing queen in each column of current row */
    for (int c = 0; c < n; c++) {
        if (is_safe_nqueens(col, row, c)) {
            col[row] = c;
            solve_nqueens(col, row + 1, n, print_all);
            /* Backtrack: column will be overwritten, no explicit undo needed */
        } else {
            g_backtracks++;
        }
    }
}

/**
 * Entry point for N-Queens solver.
 *
 * @param n         Board size
 * @param print_all Whether to print all solutions
 * @return          Number of solutions found
 */
int nqueens(int n, bool print_all) {
    int *col = (int *)malloc(n * sizeof(int));
    if (!col) {
        fprintf(stderr, "Memory allocation failed\n");
        return 0;
    }
    
    g_solutions = 0;
    g_backtracks = 0;
    g_nodes_explored = 0;
    
    solve_nqueens(col, 0, n, print_all);
    
    free(col);
    return g_solutions;
}

/* =============================================================================
 * PART 2: SUDOKU SOLVER - COMPLETE SOLUTION
 * =============================================================================
 *
 * Sudoku is a constraint satisfaction problem:
 *   - 9×9 grid divided into 3×3 boxes
 *   - Each row, column, and box must contain digits 1-9 exactly once
 *
 * Our approach:
 *   1. Find empty cell (value 0)
 *   2. Try digits 1-9
 *   3. Check if placement is valid
 *   4. Recursively solve
 *   5. Backtrack if stuck
 *
 * Optimisation: We use simple constraint checking. More advanced solvers
 * use constraint propagation (naked singles, hidden singles) and
 * sophisticated cell selection heuristics (MRV - Most Constrained Variable).
 *
 * Complexity:
 *   - Worst case: O(9^(81)) for empty board
 *   - Practical: Much better due to constraint pruning
 */

/**
 * Check if digit can be placed at (row, col) in Sudoku grid.
 *
 * @param grid Sudoku grid (9×9)
 * @param row  Row index
 * @param col  Column index
 * @param num  Digit to check (1-9)
 * @return     true if valid placement, false otherwise
 */
bool is_valid_sudoku(int grid[9][9], int row, int col, int num) {
    /* Check row: no duplicate in same row */
    for (int c = 0; c < 9; c++) {
        if (grid[row][c] == num) {
            return false;
        }
    }
    
    /* Check column: no duplicate in same column */
    for (int r = 0; r < 9; r++) {
        if (grid[r][col] == num) {
            return false;
        }
    }
    
    /* Check 3×3 box */
    int box_row = (row / 3) * 3;
    int box_col = (col / 3) * 3;
    
    for (int r = box_row; r < box_row + 3; r++) {
        for (int c = box_col; c < box_col + 3; c++) {
            if (grid[r][c] == num) {
                return false;
            }
        }
    }
    
    return true;
}

/**
 * Find the next empty cell in the Sudoku grid.
 *
 * Simple strategy: scan left-to-right, top-to-bottom.
 * Advanced solvers use MRV (Minimum Remaining Values) heuristic.
 *
 * @param grid Sudoku grid
 * @param row  Output: row of empty cell
 * @param col  Output: column of empty cell
 * @return     true if empty cell found, false if grid is complete
 */
bool find_empty_cell(int grid[9][9], int *row, int *col) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (grid[r][c] == 0) {
                *row = r;
                *col = c;
                return true;
            }
        }
    }
    return false;
}

/**
 * Print Sudoku grid with box separators.
 *
 * @param grid Sudoku grid
 */
void print_sudoku(int grid[9][9]) {
    printf("   ╔═══════╤═══════╤═══════╗\n");
    
    for (int r = 0; r < 9; r++) {
        printf("   ║");
        for (int c = 0; c < 9; c++) {
            if (grid[r][c] == 0) {
                printf(" · ");
            } else {
                printf(" %d ", grid[r][c]);
            }
            
            if (c == 2 || c == 5) {
                printf("│");
            } else if (c == 8) {
                printf("║");
            }
        }
        printf("\n");
        
        if (r == 2 || r == 5) {
            printf("   ╟───────┼───────┼───────╢\n");
        }
    }
    
    printf("   ╚═══════╧═══════╧═══════╝\n");
}

/**
 * Recursive backtracking Sudoku solver.
 *
 * @param grid Sudoku grid (modified in place)
 * @return     true if solution found, false otherwise
 */
bool solve_sudoku(int grid[9][9]) {
    g_nodes_explored++;
    
    int row, col;
    
    /* Find empty cell; if none, puzzle is solved */
    if (!find_empty_cell(grid, &row, &col)) {
        g_solutions++;
        return true;
    }
    
    /* Try digits 1-9 */
    for (int num = 1; num <= 9; num++) {
        if (is_valid_sudoku(grid, row, col, num)) {
            grid[row][col] = num;
            
            if (solve_sudoku(grid)) {
                return true;
            }
            
            /* Backtrack */
            grid[row][col] = 0;
        } else {
            g_backtracks++;
        }
    }
    
    return false;
}

/* =============================================================================
 * PART 3: HAMILTONIAN PATH - COMPLETE SOLUTION
 * =============================================================================
 *
 * A Hamiltonian path visits each vertex exactly once.
 * A Hamiltonian cycle is a path that returns to the starting vertex.
 *
 * This is an NP-complete problem - no known polynomial algorithm exists.
 * Backtracking explores all possibilities with pruning.
 *
 * Applications:
 *   - Travelling Salesman Problem (TSP)
 *   - Knight's Tour
 *   - Circuit board manufacturing (visiting all test points)
 *
 * Complexity: O(N!) worst case
 */

/**
 * Check if vertex v can be added to current Hamiltonian path.
 *
 * @param v       Vertex to check
 * @param graph   Adjacency matrix
 * @param path    Current path
 * @param pos     Current position in path
 * @param n       Number of vertices
 * @return        true if v can be added, false otherwise
 */
bool is_safe_hamiltonian(int v, int **graph, int *path, int pos, int n) {
    /* Check if there's an edge from last vertex in path to v */
    if (graph[path[pos - 1]][v] == 0) {
        return false;
    }
    
    /* Check if v is already in path */
    for (int i = 0; i < pos; i++) {
        if (path[i] == v) {
            return false;
        }
    }
    
    return true;
}

/**
 * Recursive Hamiltonian path solver.
 *
 * @param graph      Adjacency matrix
 * @param path       Current path
 * @param pos        Current position
 * @param n          Number of vertices
 * @param find_cycle Whether to find cycle (return to start)
 * @return           true if path/cycle found
 */
bool solve_hamiltonian_util(int **graph, int *path, int pos, int n, bool find_cycle) {
    g_nodes_explored++;
    
    /* Base case: all vertices included */
    if (pos == n) {
        if (find_cycle) {
            /* Check if there's edge from last to first vertex */
            if (graph[path[pos - 1]][path[0]] == 1) {
                g_solutions++;
                return true;
            }
            return false;
        }
        g_solutions++;
        return true;
    }
    
    /* Try adding vertices 1 to n-1 (vertex 0 is starting point) */
    for (int v = 1; v < n; v++) {
        if (is_safe_hamiltonian(v, graph, path, pos, n)) {
            path[pos] = v;
            
            if (solve_hamiltonian_util(graph, path, pos + 1, n, find_cycle)) {
                return true;
            }
            
            /* Backtrack */
            path[pos] = -1;
        }
        g_backtracks++;
    }
    
    return false;
}

/**
 * Print Hamiltonian path with arrows.
 *
 * @param path Path array
 * @param n    Number of vertices
 * @param cycle Whether this is a cycle
 */
void print_hamiltonian_path(int *path, int n, bool cycle) {
    printf("   ");
    for (int i = 0; i < n; i++) {
        printf("V%d", path[i]);
        if (i < n - 1) {
            printf(" → ");
        }
    }
    if (cycle) {
        printf(" → V%d", path[0]);
    }
    printf("\n");
}

/**
 * Find Hamiltonian path starting from vertex 0.
 *
 * @param graph      Adjacency matrix
 * @param n          Number of vertices
 * @param find_cycle Whether to find cycle
 * @return           true if path/cycle exists
 */
bool hamiltonian_path(int **graph, int n, bool find_cycle) {
    int *path = (int *)malloc(n * sizeof(int));
    if (!path) {
        fprintf(stderr, "Memory allocation failed\n");
        return false;
    }
    
    /* Initialise path */
    for (int i = 0; i < n; i++) {
        path[i] = -1;
    }
    
    /* Start from vertex 0 */
    path[0] = 0;
    
    g_solutions = 0;
    g_backtracks = 0;
    g_nodes_explored = 0;
    
    bool found = solve_hamiltonian_util(graph, path, 1, n, find_cycle);
    
    if (found) {
        printf("\n   %s found:\n", find_cycle ? "Hamiltonian cycle" : "Hamiltonian path");
        print_hamiltonian_path(path, n, find_cycle);
    }
    
    free(path);
    return found;
}

/* =============================================================================
 * TEST FUNCTIONS
 * =============================================================================
 */

void test_nqueens(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║            TEST 1: N-QUEENS PROBLEM                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Test 4-Queens (small, visual) */
    printf("\n  Testing 4-Queens (showing all solutions):\n");
    int count4 = nqueens(4, true);
    printf("\n  Results for 4-Queens:\n");
    printf("   ├─ Solutions found: %d\n", count4);
    printf("   ├─ Nodes explored:  %d\n", g_nodes_explored);
    printf("   └─ Backtracks:      %d\n", g_backtracks);
    
    /* Test 8-Queens (classic problem) */
    printf("\n  Testing 8-Queens (first 10 solutions shown):\n");
    int count8 = nqueens(8, true);
    printf("\n  Results for 8-Queens:\n");
    printf("   ├─ Solutions found: %d (expected: 92)\n", count8);
    printf("   ├─ Nodes explored:  %d\n", g_nodes_explored);
    printf("   └─ Backtracks:      %d\n", g_backtracks);
    
    /* Test larger boards */
    printf("\n  Solution counts for various N:\n");
    printf("   ┌────────┬────────────┬──────────────┐\n");
    printf("   │    N   │  Solutions │ Nodes Expl.  │\n");
    printf("   ├────────┼────────────┼──────────────┤\n");
    
    for (int n = 4; n <= 12; n++) {
        clock_t start = clock();
        int count = nqueens(n, false);
        clock_t end = clock();
        double time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
        
        printf("   │   %2d   │ %10d │ %12d │ %.2f ms\n", 
               n, count, g_nodes_explored, time_ms);
    }
    printf("   └────────┴────────────┴──────────────┘\n");
}

void test_sudoku(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║            TEST 2: SUDOKU SOLVER                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Easy puzzle */
    int easy[9][9] = {
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
    
    printf("\n  Easy puzzle - Input:\n");
    print_sudoku(easy);
    
    g_solutions = 0;
    g_backtracks = 0;
    g_nodes_explored = 0;
    
    clock_t start = clock();
    bool solved = solve_sudoku(easy);
    clock_t end = clock();
    
    if (solved) {
        printf("\n  Easy puzzle - Solution:\n");
        print_sudoku(easy);
        printf("\n  Statistics:\n");
        printf("   ├─ Nodes explored: %d\n", g_nodes_explored);
        printf("   ├─ Backtracks:     %d\n", g_backtracks);
        printf("   └─ Time:           %.3f ms\n", 
               (double)(end - start) * 1000.0 / CLOCKS_PER_SEC);
    }
    
    /* Hard puzzle (requires more backtracking) */
    int hard[9][9] = {
        {0, 0, 0, 0, 0, 0, 0, 1, 2},
        {0, 0, 0, 0, 3, 5, 0, 0, 0},
        {0, 0, 0, 6, 0, 0, 0, 7, 0},
        {7, 0, 0, 0, 0, 0, 3, 0, 0},
        {0, 0, 0, 4, 0, 0, 8, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 2, 0, 0, 0, 0},
        {0, 8, 0, 0, 0, 0, 0, 4, 0},
        {0, 5, 0, 0, 0, 0, 6, 0, 0}
    };
    
    printf("\n  Hard puzzle - Input:\n");
    print_sudoku(hard);
    
    g_solutions = 0;
    g_backtracks = 0;
    g_nodes_explored = 0;
    
    start = clock();
    solved = solve_sudoku(hard);
    end = clock();
    
    if (solved) {
        printf("\n  Hard puzzle - Solution:\n");
        print_sudoku(hard);
        printf("\n  Statistics:\n");
        printf("   ├─ Nodes explored: %d\n", g_nodes_explored);
        printf("   ├─ Backtracks:     %d\n", g_backtracks);
        printf("   └─ Time:           %.3f ms\n", 
               (double)(end - start) * 1000.0 / CLOCKS_PER_SEC);
    }
}

void test_hamiltonian(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║            TEST 3: HAMILTONIAN PATH                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    int n = 5;
    
    /* Allocate adjacency matrix */
    int **graph = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        graph[i] = (int *)calloc(n, sizeof(int));
    }
    
    /* Create a graph with Hamiltonian path:
     *    0 --- 1
     *    |     |
     *    3 --- 2
     *     \   /
     *       4
     */
    graph[0][1] = graph[1][0] = 1;
    graph[0][3] = graph[3][0] = 1;
    graph[1][2] = graph[2][1] = 1;
    graph[2][3] = graph[3][2] = 1;
    graph[2][4] = graph[4][2] = 1;
    graph[3][4] = graph[4][3] = 1;
    
    printf("\n  Graph (adjacency matrix):\n");
    printf("       ");
    for (int i = 0; i < n; i++) printf("V%d ", i);
    printf("\n");
    for (int i = 0; i < n; i++) {
        printf("    V%d ", i);
        for (int j = 0; j < n; j++) {
            printf(" %d ", graph[i][j]);
        }
        printf("\n");
    }
    
    printf("\n  Graph structure:\n");
    printf("         0 ─── 1\n");
    printf("         │     │\n");
    printf("         3 ─── 2\n");
    printf("          \\   /\n");
    printf("            4\n");
    
    /* Find Hamiltonian path */
    printf("\n  Finding Hamiltonian path:\n");
    bool found_path = hamiltonian_path(graph, n, false);
    printf("   ├─ Path exists:    %s\n", found_path ? "Yes" : "No");
    printf("   ├─ Nodes explored: %d\n", g_nodes_explored);
    printf("   └─ Backtracks:     %d\n", g_backtracks);
    
    /* Find Hamiltonian cycle */
    printf("\n  Finding Hamiltonian cycle:\n");
    bool found_cycle = hamiltonian_path(graph, n, true);
    printf("   ├─ Cycle exists:   %s\n", found_cycle ? "Yes" : "No");
    printf("   ├─ Nodes explored: %d\n", g_nodes_explored);
    printf("   └─ Backtracks:     %d\n", g_backtracks);
    
    /* Test graph without Hamiltonian path */
    printf("\n  Testing disconnected graph:\n");
    
    /* Clear graph */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            graph[i][j] = 0;
        }
    }
    
    /* Create a disconnected graph */
    graph[0][1] = graph[1][0] = 1;
    graph[2][3] = graph[3][2] = 1;
    
    found_path = hamiltonian_path(graph, n, false);
    printf("   └─ Hamiltonian path exists: %s (expected: No)\n", 
           found_path ? "Yes" : "No");
    
    /* Free memory */
    for (int i = 0; i < n; i++) {
        free(graph[i]);
    }
    free(graph);
}

/* =============================================================================
 * MAIN FUNCTION
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 16: ADVANCED GRAPHS & BACKTRACKING                   ║\n");
    printf("║              Exercise 2 - Complete Solutions                  ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    test_nqueens();
    test_sudoku();
    test_hamiltonian();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    ALL TESTS COMPLETE                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("  Key Takeaways:\n");
    printf("  ═══════════════\n");
    printf("  • Backtracking systematically explores solution space\n");
    printf("  • Pruning (constraint checking) dramatically reduces search\n");
    printf("  • Statistics help analyse algorithm efficiency\n");
    printf("  • Same template works for many constraint satisfaction problems\n\n");
    
    return 0;
}

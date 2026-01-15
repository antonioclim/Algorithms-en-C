/**
 * =============================================================================
 * HOMEWORK 2 SOLUTION: Sparse Matrix Operations
 * =============================================================================
 *
 * INSTRUCTOR COPY - Complete working solution
 *
 * Implements a sparse matrix using linked lists for efficient storage
 * of matrices with many zero elements.
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -lm -o homework2_sol homework2_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define EPSILON 1e-9

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct MatrixNode {
    int row;
    int col;
    double value;
    struct MatrixNode *next_in_row;
    struct MatrixNode *next_in_col;
} MatrixNode;

typedef struct SparseMatrix {
    int rows;
    int cols;
    MatrixNode **row_heads;  /* Array of row head pointers */
    MatrixNode **col_heads;  /* Array of column head pointers */
    int nonzero_count;
} SparseMatrix;

/* =============================================================================
 * FUNCTION IMPLEMENTATIONS
 * =============================================================================
 */

/**
 * Creates a new matrix node.
 */
MatrixNode* create_matrix_node(int row, int col, double value) {
    MatrixNode *node = (MatrixNode*)malloc(sizeof(MatrixNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    
    node->row = row;
    node->col = col;
    node->value = value;
    node->next_in_row = NULL;
    node->next_in_col = NULL;
    
    return node;
}

/**
 * Creates an empty sparse matrix.
 */
SparseMatrix* create_sparse_matrix(int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
        fprintf(stderr, "Error: Invalid matrix dimensions\n");
        return NULL;
    }
    
    SparseMatrix *matrix = (SparseMatrix*)malloc(sizeof(SparseMatrix));
    if (matrix == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->nonzero_count = 0;
    
    matrix->row_heads = (MatrixNode**)calloc(rows, sizeof(MatrixNode*));
    matrix->col_heads = (MatrixNode**)calloc(cols, sizeof(MatrixNode*));
    
    if (matrix->row_heads == NULL || matrix->col_heads == NULL) {
        free(matrix->row_heads);
        free(matrix->col_heads);
        free(matrix);
        return NULL;
    }
    
    return matrix;
}

/**
 * Sets an element in the sparse matrix.
 * If value is approximately zero, removes the element if it exists.
 */
void set_element(SparseMatrix *matrix, int row, int col, double value) {
    if (matrix == NULL || row < 0 || row >= matrix->rows || 
        col < 0 || col >= matrix->cols) {
        return;
    }
    
    /* Find existing node or position to insert */
    MatrixNode *prev_row = NULL;
    MatrixNode *curr_row = matrix->row_heads[row];
    
    while (curr_row != NULL && curr_row->col < col) {
        prev_row = curr_row;
        curr_row = curr_row->next_in_row;
    }
    
    /* Check if element exists */
    if (curr_row != NULL && curr_row->col == col) {
        if (fabs(value) < EPSILON) {
            /* Remove the element */
            /* Update row links */
            if (prev_row == NULL) {
                matrix->row_heads[row] = curr_row->next_in_row;
            } else {
                prev_row->next_in_row = curr_row->next_in_row;
            }
            
            /* Update column links */
            MatrixNode *prev_col = NULL;
            MatrixNode *curr_col = matrix->col_heads[col];
            while (curr_col != curr_row) {
                prev_col = curr_col;
                curr_col = curr_col->next_in_col;
            }
            
            if (prev_col == NULL) {
                matrix->col_heads[col] = curr_row->next_in_col;
            } else {
                prev_col->next_in_col = curr_row->next_in_col;
            }
            
            free(curr_row);
            matrix->nonzero_count--;
        } else {
            /* Update existing element */
            curr_row->value = value;
        }
        return;
    }
    
    /* If value is zero, don't add new element */
    if (fabs(value) < EPSILON) {
        return;
    }
    
    /* Create new node */
    MatrixNode *new_node = create_matrix_node(row, col, value);
    if (new_node == NULL) return;
    
    /* Insert in row list */
    new_node->next_in_row = curr_row;
    if (prev_row == NULL) {
        matrix->row_heads[row] = new_node;
    } else {
        prev_row->next_in_row = new_node;
    }
    
    /* Insert in column list */
    MatrixNode *prev_col = NULL;
    MatrixNode *curr_col = matrix->col_heads[col];
    
    while (curr_col != NULL && curr_col->row < row) {
        prev_col = curr_col;
        curr_col = curr_col->next_in_col;
    }
    
    new_node->next_in_col = curr_col;
    if (prev_col == NULL) {
        matrix->col_heads[col] = new_node;
    } else {
        prev_col->next_in_col = new_node;
    }
    
    matrix->nonzero_count++;
}

/**
 * Gets an element from the sparse matrix.
 */
double get_element(SparseMatrix *matrix, int row, int col) {
    if (matrix == NULL || row < 0 || row >= matrix->rows ||
        col < 0 || col >= matrix->cols) {
        return 0.0;
    }
    
    MatrixNode *curr = matrix->row_heads[row];
    while (curr != NULL && curr->col < col) {
        curr = curr->next_in_row;
    }
    
    if (curr != NULL && curr->col == col) {
        return curr->value;
    }
    
    return 0.0;
}

/**
 * Adds two sparse matrices.
 */
SparseMatrix* add_matrices(SparseMatrix *A, SparseMatrix *B) {
    if (A == NULL || B == NULL) return NULL;
    
    if (A->rows != B->rows || A->cols != B->cols) {
        fprintf(stderr, "Error: Matrix dimensions must match for addition\n");
        return NULL;
    }
    
    SparseMatrix *result = create_sparse_matrix(A->rows, A->cols);
    if (result == NULL) return NULL;
    
    /* Add elements from A */
    for (int i = 0; i < A->rows; i++) {
        MatrixNode *curr = A->row_heads[i];
        while (curr != NULL) {
            set_element(result, curr->row, curr->col, curr->value);
            curr = curr->next_in_row;
        }
    }
    
    /* Add elements from B */
    for (int i = 0; i < B->rows; i++) {
        MatrixNode *curr = B->row_heads[i];
        while (curr != NULL) {
            double current_val = get_element(result, curr->row, curr->col);
            set_element(result, curr->row, curr->col, current_val + curr->value);
            curr = curr->next_in_row;
        }
    }
    
    return result;
}

/**
 * Multiplies two sparse matrices.
 */
SparseMatrix* multiply_matrices(SparseMatrix *A, SparseMatrix *B) {
    if (A == NULL || B == NULL) return NULL;
    
    if (A->cols != B->rows) {
        fprintf(stderr, "Error: Invalid dimensions for multiplication\n");
        return NULL;
    }
    
    SparseMatrix *result = create_sparse_matrix(A->rows, B->cols);
    if (result == NULL) return NULL;
    
    /* For each element (i, k) in A */
    for (int i = 0; i < A->rows; i++) {
        MatrixNode *a_node = A->row_heads[i];
        
        while (a_node != NULL) {
            int k = a_node->col;
            double a_val = a_node->value;
            
            /* Multiply by elements (k, j) in B */
            MatrixNode *b_node = B->row_heads[k];
            
            while (b_node != NULL) {
                int j = b_node->col;
                double b_val = b_node->value;
                
                double current = get_element(result, i, j);
                set_element(result, i, j, current + a_val * b_val);
                
                b_node = b_node->next_in_row;
            }
            
            a_node = a_node->next_in_row;
        }
    }
    
    return result;
}

/**
 * Transposes a sparse matrix.
 */
SparseMatrix* transpose(SparseMatrix *matrix) {
    if (matrix == NULL) return NULL;
    
    SparseMatrix *result = create_sparse_matrix(matrix->cols, matrix->rows);
    if (result == NULL) return NULL;
    
    for (int i = 0; i < matrix->rows; i++) {
        MatrixNode *curr = matrix->row_heads[i];
        while (curr != NULL) {
            set_element(result, curr->col, curr->row, curr->value);
            curr = curr->next_in_row;
        }
    }
    
    return result;
}

/**
 * Multiplies all elements by a scalar.
 */
void scalar_multiply(SparseMatrix *matrix, double scalar) {
    if (matrix == NULL) return;
    
    if (fabs(scalar) < EPSILON) {
        /* Multiplying by zero - clear the matrix */
        for (int i = 0; i < matrix->rows; i++) {
            MatrixNode *curr = matrix->row_heads[i];
            while (curr != NULL) {
                MatrixNode *next = curr->next_in_row;
                free(curr);
                curr = next;
            }
            matrix->row_heads[i] = NULL;
        }
        for (int j = 0; j < matrix->cols; j++) {
            matrix->col_heads[j] = NULL;
        }
        matrix->nonzero_count = 0;
        return;
    }
    
    for (int i = 0; i < matrix->rows; i++) {
        MatrixNode *curr = matrix->row_heads[i];
        while (curr != NULL) {
            curr->value *= scalar;
            curr = curr->next_in_row;
        }
    }
}

/**
 * Displays sparse matrix as list of non-zero elements.
 */
void display_sparse(SparseMatrix *matrix) {
    if (matrix == NULL) {
        printf("NULL matrix\n");
        return;
    }
    
    printf("\nSparse Matrix (%dx%d), %d non-zero elements:\n",
           matrix->rows, matrix->cols, matrix->nonzero_count);
    
    if (matrix->nonzero_count == 0) {
        printf("  (all zeros)\n");
        return;
    }
    
    for (int i = 0; i < matrix->rows; i++) {
        MatrixNode *curr = matrix->row_heads[i];
        while (curr != NULL) {
            printf("  (%d, %d) = %.2f\n", curr->row, curr->col, curr->value);
            curr = curr->next_in_row;
        }
    }
}

/**
 * Displays the full matrix format.
 */
void display_dense(SparseMatrix *matrix) {
    if (matrix == NULL) {
        printf("NULL matrix\n");
        return;
    }
    
    printf("\nDense format (%dx%d):\n", matrix->rows, matrix->cols);
    
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            printf("%7.2f ", get_element(matrix, i, j));
        }
        printf("\n");
    }
}

/**
 * Counts non-zero elements.
 */
int count_nonzero(SparseMatrix *matrix) {
    return matrix ? matrix->nonzero_count : 0;
}

/**
 * Calculates sparsity ratio (percentage of zero elements).
 */
double sparsity_ratio(SparseMatrix *matrix) {
    if (matrix == NULL || matrix->rows == 0 || matrix->cols == 0) {
        return 0.0;
    }
    
    int total = matrix->rows * matrix->cols;
    int zeros = total - matrix->nonzero_count;
    
    return (100.0 * zeros) / total;
}

/**
 * Frees a sparse matrix.
 */
void free_sparse_matrix(SparseMatrix *matrix) {
    if (matrix == NULL) return;
    
    /* Free all nodes via row lists */
    for (int i = 0; i < matrix->rows; i++) {
        MatrixNode *curr = matrix->row_heads[i];
        while (curr != NULL) {
            MatrixNode *next = curr->next_in_row;
            free(curr);
            curr = next;
        }
    }
    
    free(matrix->row_heads);
    free(matrix->col_heads);
    free(matrix);
}

/**
 * Loads a sparse matrix from a file.
 */
SparseMatrix* load_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return NULL;
    }
    
    int rows, cols;
    if (fscanf(file, "%d %d", &rows, &cols) != 2) {
        fclose(file);
        return NULL;
    }
    
    SparseMatrix *matrix = create_sparse_matrix(rows, cols);
    if (matrix == NULL) {
        fclose(file);
        return NULL;
    }
    
    int row, col;
    double value;
    while (fscanf(file, "%d %d %lf", &row, &col, &value) == 3) {
        set_element(matrix, row, col, value);
    }
    
    fclose(file);
    return matrix;
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     Sparse Matrix Operations - SOLUTION                       ║\n");
    printf("║     Homework 2 - Linked Lists                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Create matrix A */
    printf("\n--- Creating Matrix A ---\n");
    SparseMatrix *A = create_sparse_matrix(4, 4);
    
    set_element(A, 0, 0, 5.0);
    set_element(A, 0, 2, 3.0);
    set_element(A, 1, 1, 8.0);
    set_element(A, 2, 3, 6.0);
    set_element(A, 3, 0, 2.0);
    
    display_sparse(A);
    display_dense(A);
    printf("\nSparsity: %.1f%%\n", sparsity_ratio(A));
    
    /* Create matrix B */
    printf("\n--- Creating Matrix B ---\n");
    SparseMatrix *B = create_sparse_matrix(4, 4);
    
    set_element(B, 0, 0, 1.0);
    set_element(B, 1, 1, 2.0);
    set_element(B, 2, 2, 3.0);
    set_element(B, 3, 3, 4.0);
    
    display_sparse(B);
    display_dense(B);
    
    /* Addition */
    printf("\n--- Matrix Addition (A + B) ---\n");
    SparseMatrix *sum = add_matrices(A, B);
    display_sparse(sum);
    display_dense(sum);
    
    /* Multiplication */
    printf("\n--- Matrix Multiplication (A * B) ---\n");
    SparseMatrix *product = multiply_matrices(A, B);
    display_sparse(product);
    display_dense(product);
    
    /* Transpose */
    printf("\n--- Transpose of A ---\n");
    SparseMatrix *A_T = transpose(A);
    display_sparse(A_T);
    display_dense(A_T);
    
    /* Scalar multiplication */
    printf("\n--- Scalar Multiplication (A * 2) ---\n");
    SparseMatrix *A_copy = create_sparse_matrix(4, 4);
    for (int i = 0; i < A->rows; i++) {
        MatrixNode *curr = A->row_heads[i];
        while (curr != NULL) {
            set_element(A_copy, curr->row, curr->col, curr->value);
            curr = curr->next_in_row;
        }
    }
    scalar_multiply(A_copy, 2.0);
    display_dense(A_copy);
    
    /* Cleanup */
    free_sparse_matrix(A);
    free_sparse_matrix(B);
    free_sparse_matrix(sum);
    free_sparse_matrix(product);
    free_sparse_matrix(A_T);
    free_sparse_matrix(A_copy);
    
    printf("\n--- Program finished (no memory leaks) ---\n\n");
    
    return 0;
}

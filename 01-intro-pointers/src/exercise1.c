/**
 * =============================================================================
 * EXERCISE 1: Calculator with Dispatch Table
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement a calculator that uses function pointers and a dispatch table
 *   instead of switch statements.
 *
 * REQUIREMENTS:
 *   1. Implement the 5 arithmetic operations (+, -, *, /, %)
 *   2. Handle division by zero gracefully
 *   3. Initialize a dispatch table with function pointers
 *   4. Read operations from stdin in format: "operand1 operator operand2"
 *   5. Process until EOF or invalid input
 *
 * EXAMPLE INPUT:
 *   10 + 5
 *   20 - 8
 *   7 * 3
 *   15 / 4
 *   17 % 5
 *   10 / 0
 *
 * EXPECTED OUTPUT:
 *   10 + 5 = 15
 *   20 - 8 = 12
 *   7 * 3 = 21
 *   15 / 4 = 3
 *   17 % 5 = 2
 *   Error: Division by zero
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * TODO 1: Define a typedef for a function pointer type
 *
 * The function pointer should:
 *   - Take two int parameters
 *   - Return an int
 *   - Be named 'Operation'
 *
 * Hint: typedef return_type (*TypeName)(param_types);
 */

/* YOUR CODE HERE */


/* =============================================================================
 * ARITHMETIC OPERATIONS
 * =============================================================================
 */

/**
 * TODO 2: Implement the addition function
 *
 * @param a First operand
 * @param b Second operand
 * @return Sum of a and b
 */
int op_add(int a, int b) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}

/**
 * TODO 3: Implement the subtraction function
 */
int op_subtract(int a, int b) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}

/**
 * TODO 4: Implement the multiplication function
 */
int op_multiply(int a, int b) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}

/**
 * TODO 5: Implement safe division
 *
 * IMPORTANT: Handle division by zero!
 * If b is 0, print "Error: Division by zero\n" to stderr
 * and return 0 (or use a special error indicator)
 *
 * For this exercise, we'll use a global flag to indicate errors.
 */
static int division_error = 0;  /* Global error flag */

int op_divide(int a, int b) {
    /* YOUR CODE HERE */
    /* Remember to set division_error = 1 if b == 0 */
    return 0;  /* Replace this */
}

/**
 * TODO 6: Implement modulo operation with error handling
 */
int op_modulo(int a, int b) {
    /* YOUR CODE HERE */
    /* Remember to handle division by zero like op_divide */
    return 0;  /* Replace this */
}


/* =============================================================================
 * DISPATCH TABLE
 * =============================================================================
 */

/**
 * The dispatch table: an array of function pointers indexed by ASCII value
 * of the operator character.
 *
 * We use size 256 to cover all possible char values.
 * Most entries will be NULL (unused operators).
 */

/* TODO 7: Declare the dispatch table
 *
 * Hint: Operation dispatch[256] = {NULL};
 * or:   Operation dispatch[256] = {0};
 */

/* YOUR CODE HERE */


/**
 * TODO 8: Initialize the dispatch table
 *
 * Map each operator character to its corresponding function:
 *   '+' -> op_add
 *   '-' -> op_subtract
 *   '*' -> op_multiply
 *   '/' -> op_divide
 *   '%' -> op_modulo
 *
 * Hint: dispatch['+'] = op_add;
 */
void init_dispatch_table(void) {
    /* YOUR CODE HERE */

}


/**
 * TODO 9: Implement the calculate function
 *
 * Use the dispatch table to perform the operation.
 *
 * @param a First operand
 * @param op Operator character
 * @param b Second operand
 * @param error Pointer to store error status (1 if error, 0 otherwise)
 * @return Result of the operation, or 0 if operator unknown
 *
 * Steps:
 *   1. Reset the global division_error flag
 *   2. Check if dispatch[(unsigned char)op] is not NULL
 *   3. If valid, call the function and return result
 *   4. If NULL, set *error = 1 and print "Error: Unknown operator"
 *   5. After calling, check division_error and set *error accordingly
 */
int calculate(int a, char op, int b, int *error) {
    *error = 0;
    division_error = 0;

    /* YOUR CODE HERE */

    return 0;  /* Replace this */
}


/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    int a, b;
    char op;
    int error;

    /* Initialize the dispatch table */
    init_dispatch_table();

    printf("Calculator with Dispatch Table\n");
    printf("Enter operations (e.g., '10 + 5'), Ctrl+D to exit:\n");
    printf("─────────────────────────────────────────────────\n");

    /* Read and process operations until EOF */
    while (scanf("%d %c %d", &a, &op, &b) == 3) {

        /* TODO 10: Call calculate() and handle the result
         *
         * Steps:
         *   1. Call calculate(a, op, b, &error)
         *   2. If no error, print: "a op b = result\n"
         *   3. If error, the error message was already printed
         *
         * Use printf("%d %c %d = %d\n", a, op, b, result);
         */

        /* YOUR CODE HERE */

    }

    printf("─────────────────────────────────────────────────\n");
    printf("Calculator terminated.\n");

    return 0;
}


/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Add power operation ('^') using a loop or recursion
 *    Note: Cannot use pow() without -lm flag
 *
 * 2. Add bitwise operations: & (AND), | (OR), ~ (XOR)
 *
 * 3. Implement a history feature that stores last N calculations
 *
 * 4. Add parentheses support using a simple expression parser
 *
 * =============================================================================
 */

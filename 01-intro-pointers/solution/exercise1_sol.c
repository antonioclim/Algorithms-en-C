/**
 * =============================================================================
 * SOLUTION: Exercise 1 - Calculator with Dispatch Table
 * =============================================================================
 * INSTRUCTOR USE ONLY - Do not distribute to students
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

/* Type definition for operation function pointer */
typedef int (*Operation)(int, int);

/* Global error flag for division operations */
static int division_error = 0;

/* Arithmetic operations */
int op_add(int a, int b) {
    return a + b;
}

int op_subtract(int a, int b) {
    return a - b;
}

int op_multiply(int a, int b) {
    return a * b;
}

int op_divide(int a, int b) {
    if (b == 0) {
        fprintf(stderr, "Error: Division by zero\n");
        division_error = 1;
        return 0;
    }
    division_error = 0;
    return a / b;
}

int op_modulo(int a, int b) {
    if (b == 0) {
        fprintf(stderr, "Error: Division by zero\n");
        division_error = 1;
        return 0;
    }
    division_error = 0;
    return a % b;
}

/* Dispatch table */
static Operation dispatch[256] = {NULL};

void init_dispatch_table(void) {
    dispatch['+'] = op_add;
    dispatch['-'] = op_subtract;
    dispatch['*'] = op_multiply;
    dispatch['/'] = op_divide;
    dispatch['%'] = op_modulo;
}

int calculate(int a, char op, int b, int *error) {
    *error = 0;
    division_error = 0;

    if (dispatch[(unsigned char)op] == NULL) {
        fprintf(stderr, "Error: Unknown operator '%c'\n", op);
        *error = 1;
        return 0;
    }

    int result = dispatch[(unsigned char)op](a, b);

    if (division_error) {
        *error = 1;
    }

    return result;
}

int main(void) {
    int a, b;
    char op;
    int error;

    init_dispatch_table();

    printf("Calculator with Dispatch Table\n");
    printf("Enter operations (e.g., '10 + 5'), Ctrl+D to exit:\n");
    printf("─────────────────────────────────────────────────\n");

    while (scanf("%d %c %d", &a, &op, &b) == 3) {
        int result = calculate(a, op, b, &error);

        if (!error) {
            printf("%d %c %d = %d\n", a, op, b, result);
        }
    }

    printf("─────────────────────────────────────────────────\n");
    printf("Calculator terminated.\n");

    return 0;
}

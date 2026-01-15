/**
 * =============================================================================
 * SOLUTION: Tema 2 - Expression Parser
 * =============================================================================
 * INSTRUCTOR USE ONLY - Do not distribute to students
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct {
    int operand1;
    char operator;
    int operand2;
} Expression;

typedef int (*Operation)(int, int, int *error);

/* =============================================================================
 * ARITHMETIC OPERATIONS
 * =============================================================================
 */

int op_add(int a, int b, int *error) {
    *error = 0;
    /* Check for overflow */
    if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b)) {
        fprintf(stderr, "Eroare: Overflow la adunare\n");
        *error = 1;
        return 0;
    }
    return a + b;
}

int op_subtract(int a, int b, int *error) {
    *error = 0;
    /* Check for overflow */
    if ((b < 0 && a > INT_MAX + b) || (b > 0 && a < INT_MIN + b)) {
        fprintf(stderr, "Eroare: Overflow la scădere\n");
        *error = 1;
        return 0;
    }
    return a - b;
}

int op_multiply(int a, int b, int *error) {
    *error = 0;
    /* Check for overflow */
    if (a > 0 && b > 0 && a > INT_MAX / b) {
        fprintf(stderr, "Eroare: Overflow la înmulțire\n");
        *error = 1;
        return 0;
    }
    if (a < 0 && b < 0 && a < INT_MAX / b) {
        fprintf(stderr, "Eroare: Overflow la înmulțire\n");
        *error = 1;
        return 0;
    }
    if (a > 0 && b < 0 && b < INT_MIN / a) {
        fprintf(stderr, "Eroare: Overflow la înmulțire\n");
        *error = 1;
        return 0;
    }
    if (a < 0 && b > 0 && a < INT_MIN / b) {
        fprintf(stderr, "Eroare: Overflow la înmulțire\n");
        *error = 1;
        return 0;
    }
    return a * b;
}

int op_divide(int a, int b, int *error) {
    *error = 0;
    if (b == 0) {
        fprintf(stderr, "Eroare: Împărțire la zero\n");
        *error = 1;
        return 0;
    }
    /* Check for overflow: INT_MIN / -1 */
    if (a == INT_MIN && b == -1) {
        fprintf(stderr, "Eroare: Overflow la împărțire\n");
        *error = 1;
        return 0;
    }
    return a / b;
}

int op_modulo(int a, int b, int *error) {
    *error = 0;
    if (b == 0) {
        fprintf(stderr, "Eroare: Modulo cu zero\n");
        *error = 1;
        return 0;
    }
    return a % b;
}

/**
 * Power function (iterative to avoid stack overflow)
 */
int op_power(int base, int exp, int *error) {
    *error = 0;

    if (exp < 0) {
        fprintf(stderr, "Eroare: Exponent negativ nu este suportat\n");
        *error = 1;
        return 0;
    }

    if (exp == 0) {
        return 1;
    }

    long long result = 1;
    long long b = base;

    while (exp > 0) {
        if (exp % 2 == 1) {
            result *= b;
            if (result > INT_MAX || result < INT_MIN) {
                fprintf(stderr, "Eroare: Overflow la putere\n");
                *error = 1;
                return 0;
            }
        }
        exp /= 2;
        if (exp > 0) {
            b *= b;
            if (b > INT_MAX || b < INT_MIN) {
                fprintf(stderr, "Eroare: Overflow la putere\n");
                *error = 1;
                return 0;
            }
        }
    }

    return (int)result;
}

/* =============================================================================
 * DISPATCH TABLE
 * =============================================================================
 */

static Operation dispatch[256] = {NULL};

void init_dispatch_table(void) {
    dispatch['+'] = op_add;
    dispatch['-'] = op_subtract;
    dispatch['*'] = op_multiply;
    dispatch['/'] = op_divide;
    dispatch['%'] = op_modulo;
    dispatch['^'] = op_power;
}

/* =============================================================================
 * EXPRESSION PARSER
 * =============================================================================
 */

/**
 * Skip whitespace in a string
 */
const char* skip_whitespace(const char *str) {
    while (*str && isspace((unsigned char)*str)) {
        str++;
    }
    return str;
}

/**
 * Parse an integer from string
 * Returns pointer to character after the number, or NULL on error
 */
const char* parse_int(const char *str, int *value) {
    str = skip_whitespace(str);

    if (*str == '\0') {
        return NULL;
    }

    char *endptr;
    long val = strtol(str, &endptr, 10);

    if (endptr == str) {
        return NULL;  /* No digits found */
    }

    if (val > INT_MAX || val < INT_MIN) {
        fprintf(stderr, "Eroare: Număr prea mare\n");
        return NULL;
    }

    *value = (int)val;
    return endptr;
}

/**
 * Parse an expression from string
 * Format: "operand1 operator operand2"
 *
 * @param str Input string
 * @param expr Output expression structure
 * @return 1 on success, 0 on error
 */
int parse_expression(const char *str, Expression *expr) {
    const char *ptr = str;

    /* Parse first operand */
    ptr = parse_int(ptr, &expr->operand1);
    if (ptr == NULL) {
        fprintf(stderr, "Eroare: Nu pot parsa primul operand\n");
        return 0;
    }

    /* Skip whitespace and get operator */
    ptr = skip_whitespace(ptr);
    if (*ptr == '\0') {
        fprintf(stderr, "Eroare: Lipsește operatorul\n");
        return 0;
    }

    expr->operator = *ptr;
    ptr++;

    /* Parse second operand */
    ptr = parse_int(ptr, &expr->operand2);
    if (ptr == NULL) {
        fprintf(stderr, "Eroare: Nu pot parsa al doilea operand\n");
        return 0;
    }

    /* Check for extra characters */
    ptr = skip_whitespace(ptr);
    if (*ptr != '\0') {
        fprintf(stderr, "Eroare: Caractere extra după expresie: '%s'\n", ptr);
        return 0;
    }

    return 1;
}

/* =============================================================================
 * EXPRESSION EVALUATOR
 * =============================================================================
 */

/**
 * Evaluate an expression
 *
 * @param expr Expression to evaluate
 * @param result Pointer to store result
 * @return 1 on success, 0 on error
 */
int evaluate(const Expression *expr, int *result) {
    Operation op = dispatch[(unsigned char)expr->operator];

    if (op == NULL) {
        fprintf(stderr, "Eroare: Operator necunoscut '%c'\n", expr->operator);
        return 0;
    }

    int error;
    *result = op(expr->operand1, expr->operand2, &error);

    return !error;
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    char line[256];
    Expression expr;
    int result;

    init_dispatch_table();

    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║              CALCULATOR DE EXPRESII - SOLUȚIE                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    printf("Operatori disponibili: + - * / %% ^\n");
    printf("Introduceți expresii (ex: '5 + 3'), 'exit' pentru a ieși.\n");
    printf("─────────────────────────────────────────────────────────────────\n");

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break;  /* EOF */
        }

        /* Remove newline */
        line[strcspn(line, "\n")] = '\0';

        /* Check for exit command */
        if (strcmp(line, "exit") == 0 || strcmp(line, "quit") == 0) {
            break;
        }

        /* Skip empty lines */
        if (strlen(line) == 0) {
            continue;
        }

        /* Parse and evaluate */
        if (parse_expression(line, &expr)) {
            if (evaluate(&expr, &result)) {
                printf("%d %c %d = %d\n", expr.operand1, expr.operator,
                       expr.operand2, result);
            }
        }
    }

    printf("─────────────────────────────────────────────────────────────────\n");
    printf("La revedere!\n");

    return 0;
}

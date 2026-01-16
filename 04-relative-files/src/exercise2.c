/**
 * =============================================================================
 * EXERCISE 2: Polynomial Calculator
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement a polynomial representation using linked lists and support
 *   arithmetic operations (addition, multiplication) and evaluation.
 *
 * REQUIREMENTS:
 *   1. Represent polynomials as linked lists of terms (coefficient, exponent)
 *   2. Store terms in descending order by exponent
 *   3. Automatically combine like terms
 *   4. Implement polynomial addition
 *   5. Implement polynomial multiplication
 *   6. Implement polynomial evaluation at a given x value
 *   7. Display polynomials in mathematical notation
 *
 * POLYNOMIAL REPRESENTATION:
 *   3x^4 + 2x^2 - 5x + 7 is stored as:
 *   [3,4] -> [2,2] -> [-5,1] -> [7,0] -> NULL
 *
 * EXAMPLE INPUT (from data/polynomials.txt):
 *   POLY A: 3 4 2 2 -5 1 7 0
 *   POLY B: 1 3 -2 2 4 0
 *   ADD A B
 *   MULTIPLY A B
 *   EVAL A 2
 *
 * EXPECTED OUTPUT:
 *   P(x) = 3x^4 + 2x^2 - 5x + 7
 *   Q(x) = x^3 - 2x^2 + 4
 *   P(x) + Q(x) = 3x^4 + x^3 + 0x^2 - 5x + 11
 *   P(2) = 61
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -lm -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define MAX_LINE_LENGTH 512
#define EPSILON 1e-9  /* For floating-point comparison */

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * TODO 1: Define the Term structure
 *
 * A polynomial term consists of:
 *   - coefficient: a double representing the numerical coefficient
 *   - exponent: an integer representing the power of x (must be >= 0)
 *   - next: a pointer to the next term
 *
 * Example: The term 3x^4 has coefficient=3.0 and exponent=4
 */
typedef struct Term {
    double coefficient;
    int exponent;
    struct Term *next;
} Term;


/**
 * TODO 2: Define the Polynomial structure
 *
 * A polynomial is represented by:
 *   - head: pointer to the first term (highest exponent)
 *   - name: a character for identification (e.g., 'A', 'B', 'P')
 *
 * This wrapper structure makes it easier to manage polynomials.
 */

/* YOUR CODE HERE */
typedef struct Polynomial {
    Term *head;
    char name;
} Polynomial;


/* =============================================================================
 * FUNCTION PROTOTYPES
 * =============================================================================
 */

Term* create_term(double coeff, int exp);
Polynomial* create_polynomial(char name);
void add_term(Polynomial *poly, double coeff, int exp);
Polynomial* add_polynomials(const Polynomial *p1, const Polynomial *p2, char result_name);
Polynomial* multiply_polynomials(const Polynomial *p1, const Polynomial *p2, char result_name);
double evaluate_polynomial(const Polynomial *poly, double x);
void display_polynomial(const Polynomial *poly);
int get_degree(const Polynomial *poly);
void free_polynomial(Polynomial *poly);
Polynomial* derivative(const Polynomial *poly, char result_name);
bool is_zero_polynomial(const Polynomial *poly);

/* Provided helpers, declared here so they can be used by the file-mode parser */
Polynomial* parse_polynomial(const char *str, char name);
bool polynomials_equal(const Polynomial *p1, const Polynomial *p2);

/* =============================================================================
 * FUNCTIONS
 * =============================================================================
 */

/**
 * TODO 3: Implement create_term
 *
 * Creates a new term with the given coefficient and exponent.
 *
 * @param coeff The coefficient value
 * @param exp The exponent value (must be >= 0)
 * @return Pointer to newly created term, or NULL on error
 *
 * Steps:
 *   1. Validate that exponent is non-negative
 *   2. Allocate memory for a new Term
 *   3. Check if allocation succeeded
 *   4. Initialise the fields
 *   5. Return the pointer
 */
Term* create_term(double coeff, int exp) {
    if (exp < 0) {
        fprintf(stderr, "Error: Negative exponent not allowed\n");
        return NULL;
    }

    Term *new_term = (Term*)malloc(sizeof(Term));
    if (new_term == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }

    new_term->coefficient = coeff;
    new_term->exponent = exp;
    new_term->next = NULL;
    return new_term;
}

/**
 * TODO 4: Implement create_polynomial
 *
 * Creates an empty polynomial with the given name.
 *
 * @param name A character identifier for the polynomial
 * @return Pointer to newly created polynomial, or NULL on error
 */
Polynomial* create_polynomial(char name) {
    Polynomial *poly = (Polynomial*)malloc(sizeof(Polynomial));
    if (poly == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }

    poly->head = NULL;
    poly->name = name;
    return poly;
}

/**
 * TODO 5: Implement add_term
 *
 * Adds a term to the polynomial, maintaining descending order by exponent.
 * If a term with the same exponent exists, combine them (add coefficients).
 * If the resulting coefficient is zero (or very close), remove the term.
 *
 * @param poly Pointer to the polynomial
 * @param coeff Coefficient of the new term
 * @param exp Exponent of the new term
 *
 * Steps:
 *   1. If coefficient is essentially zero, skip adding
 *   2. Check if a term with this exponent already exists
 *   3. If yes, add the coefficients together
 *      - If result is zero, remove the term
 *   4. If no, insert the new term in the correct position
 *      - Maintain descending order by exponent
 *
 * Hint: Use fabs(value) < EPSILON to check if a value is "zero"
 */
void add_term(Polynomial *poly, double coeff, int exp) {
    if (poly == NULL || fabs(coeff) < EPSILON) {
        return;
    }

    /* Case 1: empty polynomial or insertion before head */
    if (poly->head == NULL || poly->head->exponent < exp) {
        Term *new_term = create_term(coeff, exp);
        if (new_term == NULL) {
            return;
        }
        new_term->next = poly->head;
        poly->head = new_term;
        return;
    }

    /* Case 2: combine with head */
    if (poly->head->exponent == exp) {
        poly->head->coefficient += coeff;
        if (fabs(poly->head->coefficient) < EPSILON) {
            Term *temp = poly->head;
            poly->head = poly->head->next;
            free(temp);
        }
        return;
    }

    /* Case 3: find insertion point */
    Term *current = poly->head;
    while (current->next != NULL && current->next->exponent > exp) {
        current = current->next;
    }

    if (current->next != NULL && current->next->exponent == exp) {
        current->next->coefficient += coeff;
        if (fabs(current->next->coefficient) < EPSILON) {
            Term *temp = current->next;
            current->next = temp->next;
            free(temp);
        }
    } else {
        Term *new_term = create_term(coeff, exp);
        if (new_term == NULL) {
            return;
        }
        new_term->next = current->next;
        current->next = new_term;
    }
}

/**
 * TODO 6: Implement add_polynomials
 *
 * Adds two polynomials and returns a new polynomial with the result.
 *
 * @param p1 First polynomial
 * @param p2 Second polynomial
 * @param result_name Name for the result polynomial
 * @return New polynomial representing p1 + p2
 *
 * Algorithm:
 *   1. Create a new polynomial for the result
 *   2. Traverse p1 and add all its terms to the result
 *   3. Traverse p2 and add all its terms to the result
 *      (add_term will handle combining like terms)
 *   4. Return the result polynomial
 *
 * Note: The add_term function handles combining like terms automatically!
 */
Polynomial* add_polynomials(const Polynomial *p1, const Polynomial *p2, char result_name) {
    Polynomial *result = create_polynomial(result_name);
    if (result == NULL) {
        return NULL;
    }

    const Term *t1 = (p1 != NULL) ? p1->head : NULL;
    while (t1 != NULL) {
        add_term(result, t1->coefficient, t1->exponent);
        t1 = t1->next;
    }

    const Term *t2 = (p2 != NULL) ? p2->head : NULL;
    while (t2 != NULL) {
        add_term(result, t2->coefficient, t2->exponent);
        t2 = t2->next;
    }

    return result;
}

/**
 * TODO 7: Implement multiply_polynomials
 *
 * Multiplies two polynomials and returns a new polynomial with the result.
 *
 * @param p1 First polynomial
 * @param p2 Second polynomial
 * @param result_name Name for the result polynomial
 * @return New polynomial representing p1 * p2
 *
 * Algorithm:
 *   For each term (c1, e1) in p1:
 *     For each term (c2, e2) in p2:
 *       Add term (c1*c2, e1+e2) to result
 *
 * Example: (2x + 3) * (x - 1)
 *   2x * x = 2x^2
 *   2x * (-1) = -2x
 *   3 * x = 3x
 *   3 * (-1) = -3
 *   Result: 2x^2 + (-2x + 3x) + (-3) = 2x^2 + x - 3
 */
Polynomial* multiply_polynomials(const Polynomial *p1, const Polynomial *p2, char result_name) {
    Polynomial *result = create_polynomial(result_name);
    if (result == NULL) {
        return NULL;
    }

    if (p1 == NULL || p2 == NULL || p1->head == NULL || p2->head == NULL) {
        return result;
    }

    const Term *t1 = p1->head;
    while (t1 != NULL) {
        const Term *t2 = p2->head;
        while (t2 != NULL) {
            add_term(result, t1->coefficient * t2->coefficient, t1->exponent + t2->exponent);
            t2 = t2->next;
        }
        t1 = t1->next;
    }

    return result;
}

/**
 * TODO 8: Implement evaluate_polynomial
 *
 * Evaluates the polynomial at a given value of x.
 *
 * @param poly Pointer to the polynomial
 * @param x The value at which to evaluate
 * @return The computed value P(x)
 *
 * Method 1 (Direct):
 *   Sum of (coefficient * x^exponent) for each term
 *
 * Method 2 (Horner's method - more efficient, optional):
 *   Rearrange polynomial for nested evaluation
 *
 * Hint: Use pow(x, exp) from <math.h> for exponentiation
 */
double evaluate_polynomial(const Polynomial *poly, double x) {
    if (poly == NULL || poly->head == NULL) {
        return 0.0;
    }

    double result = 0.0;
    const Term *current = poly->head;
    while (current != NULL) {
        result += current->coefficient * pow(x, current->exponent);
        current = current->next;
    }

    return result;
}

/**
 * TODO 9: Implement display_polynomial
 *
 * Displays the polynomial in mathematical notation.
 *
 * @param poly Pointer to the polynomial
 *
 * Output format examples:
 *   P(x) = 3x^4 + 2x^2 - 5x + 7
 *   Q(x) = -2x^3 + x - 4
 *   R(x) = 5 (constant polynomial)
 *   Z(x) = 0 (zero polynomial)
 *
 * Rules:
 *   - Don't print coefficient if it's 1 or -1 (except for constant term)
 *   - Print x instead of x^1
 *   - Print just the number for x^0 (constant term)
 *   - Handle negative coefficients properly (- instead of + -)
 *   - Print 0 if the polynomial is empty
 *
 * Hint: This requires careful handling of many special cases!
 */
void display_polynomial(const Polynomial *poly) {
    if (poly == NULL) {
        printf("NULL polynomial\n");
        return;
    }
    
    printf("%c(x) = ", poly->name);
    
    if (poly->head == NULL) {
        printf("0");
        return;
    }

    const Term *current = poly->head;
    bool first = true;

    while (current != NULL) {
        double coeff = current->coefficient;
        int exp = current->exponent;

        if (!first) {
            if (coeff >= 0) {
                printf(" + ");
            } else {
                printf(" - ");
                coeff = -coeff;
            }
        } else if (coeff < 0) {
            printf("-");
            coeff = -coeff;
        }

        if (exp == 0) {
            printf("%.2g", coeff);
        } else if (exp == 1) {
            if (fabs(coeff - 1.0) < EPSILON) {
                printf("x");
            } else {
                printf("%.2gx", coeff);
            }
        } else {
            if (fabs(coeff - 1.0) < EPSILON) {
                printf("x^%d", exp);
            } else {
                printf("%.2gx^%d", coeff, exp);
            }
        }

        first = false;
        current = current->next;
    }
    
    printf("\n");
}

/**
 * TODO 10: Implement get_degree
 *
 * Returns the degree of the polynomial (highest exponent).
 *
 * @param poly Pointer to the polynomial
 * @return Degree of the polynomial, or -1 for zero polynomial
 */
int get_degree(const Polynomial *poly) {
    if (poly == NULL || poly->head == NULL) {
        return -1;
    }
    return poly->head->exponent;
}

/**
 * TODO 11: Implement free_polynomial
 *
 * Frees all memory associated with a polynomial.
 *
 * @param poly Pointer to the polynomial to free
 *
 * Steps:
 *   1. Free each term in the linked list
 *   2. Free the polynomial structure itself
 */
void free_polynomial(Polynomial *poly) {
    if (poly == NULL) {
        return;
    }

    Term *current = poly->head;
    while (current != NULL) {
        Term *next = current->next;
        free(current);
        current = next;
    }

    free(poly);
}

/**
 * TODO 12: Implement derivative
 *
 * Computes the derivative of a polynomial.
 *
 * @param poly Pointer to the original polynomial
 * @param result_name Name for the result polynomial
 * @return New polynomial representing d/dx P(x)
 *
 * Derivative rules:
 *   d/dx (c * x^n) = c * n * x^(n-1)
 *   d/dx (constant) = 0
 *
 * Example: d/dx (3x^4 + 2x^2 - 5x + 7) = 12x^3 + 4x - 5
 */
Polynomial* derivative(const Polynomial *poly, char result_name) {
    Polynomial *result = create_polynomial(result_name);
    if (result == NULL) {
        return NULL;
    }

    if (poly == NULL) {
        return result;
    }

    const Term *current = poly->head;
    while (current != NULL) {
        if (current->exponent > 0) {
            add_term(result, current->coefficient * current->exponent, current->exponent - 1);
        }
        current = current->next;
    }

    return result;
}

/**
 * TODO 13: Implement is_zero_polynomial
 *
 * Checks if a polynomial is the zero polynomial (no terms or all zero).
 *
 * @param poly Pointer to the polynomial
 * @return true if zero polynomial, false otherwise
 */
bool is_zero_polynomial(const Polynomial *poly) {
    return (poly == NULL || poly->head == NULL);
}

/* =============================================================================
 * FILE PROCESSING HELPERS
 * =============================================================================
 */

static void display_polynomial_expression(const Polynomial *poly) {
    if (poly == NULL || poly->head == NULL) {
        printf("0");
        return;
    }

    const Term *current = poly->head;
    bool first = true;

    while (current != NULL) {
        double coeff = current->coefficient;
        int exp = current->exponent;

        if (!first) {
            if (coeff >= 0) {
                printf(" + ");
            } else {
                printf(" - ");
                coeff = -coeff;
            }
        } else if (coeff < 0) {
            printf("-");
            coeff = -coeff;
        }

        if (exp == 0) {
            printf("%.2g", coeff);
        } else if (exp == 1) {
            if (fabs(coeff - 1.0) < EPSILON) {
                printf("x");
            } else {
                printf("%.2gx", coeff);
            }
        } else {
            if (fabs(coeff - 1.0) < EPSILON) {
                printf("x^%d", exp);
            } else {
                printf("%.2gx^%d", coeff, exp);
            }
        }

        first = false;
        current = current->next;
    }
}

static int polynomial_slot(char name) {
    if (name >= 'a' && name <= 'z') {
        name = (char)(name - 'a' + 'A');
    }
    if (name < 'A' || name > 'Z') {
        return -1;
    }
    return (int)(name - 'A');
}

static void process_polynomial_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return;
    }

    Polynomial *polys[26] = {0};

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        /* POLY X: <pairs> */
        if (strncmp(line, "POLY", 4) == 0) {
            char name = 0;
            char rest[MAX_LINE_LENGTH] = {0};
            if (sscanf(line, "POLY %c: %511[^\n]", &name, rest) == 2) {
                int idx = polynomial_slot(name);
                if (idx >= 0) {
                    if (polys[idx] != NULL) {
                        free_polynomial(polys[idx]);
                    }
                    polys[idx] = parse_polynomial(rest, (idx + 'A'));
                }
            }
            continue;
        }

        /* DISPLAY X */
        if (strncmp(line, "DISPLAY", 7) == 0) {
            char name = 0;
            if (sscanf(line, "DISPLAY %c", &name) == 1) {
                int idx = polynomial_slot(name);
                if (idx >= 0 && polys[idx] != NULL) {
                    display_polynomial(polys[idx]);
                }
            }
            continue;
        }

        /* ADD A B */
        if (strncmp(line, "ADD", 3) == 0) {
            char a = 0;
            char b = 0;
            if (sscanf(line, "ADD %c %c", &a, &b) == 2) {
                int ia = polynomial_slot(a);
                int ib = polynomial_slot(b);
                if (ia >= 0 && ib >= 0 && polys[ia] != NULL && polys[ib] != NULL) {
                    Polynomial *sum = add_polynomials(polys[ia], polys[ib], 'R');
                    printf("%c(x) + %c(x) = ", polys[ia]->name, polys[ib]->name);
                    display_polynomial_expression(sum);
                    printf("\n");
                    free_polynomial(sum);
                }
            }
            continue;
        }

        /* MULTIPLY A B */
        if (strncmp(line, "MULTIPLY", 8) == 0) {
            char a = 0;
            char b = 0;
            if (sscanf(line, "MULTIPLY %c %c", &a, &b) == 2) {
                int ia = polynomial_slot(a);
                int ib = polynomial_slot(b);
                if (ia >= 0 && ib >= 0 && polys[ia] != NULL && polys[ib] != NULL) {
                    Polynomial *prod = multiply_polynomials(polys[ia], polys[ib], 'R');
                    printf("%c(x) * %c(x) = ", polys[ia]->name, polys[ib]->name);
                    display_polynomial_expression(prod);
                    printf("\n");
                    free_polynomial(prod);
                }
            }
            continue;
        }

        /* EVAL A x */
        if (strncmp(line, "EVAL", 4) == 0) {
            char a = 0;
            double x = 0.0;
            if (sscanf(line, "EVAL %c %lf", &a, &x) == 2) {
                int ia = polynomial_slot(a);
                if (ia >= 0 && polys[ia] != NULL) {
                    double val = evaluate_polynomial(polys[ia], x);
                    if (fabs(x - round(x)) < EPSILON) {
                        printf("%c(%.0f) = %.2f\n", polys[ia]->name, x, val);
                    } else {
                        printf("%c(%.2f) = %.2f\n", polys[ia]->name, x, val);
                    }
                }
            }
            continue;
        }
    }

    fclose(file);

    for (int i = 0; i < 26; i++) {
        free_polynomial(polys[i]);
    }
}

/* =============================================================================
 * HELPER FUNCTIONS (PROVIDED)
 * =============================================================================
 */

/**
 * Parses a polynomial from a string of coefficient-exponent pairs.
 * Format: "c1 e1 c2 e2 c3 e3 ..."
 * Example: "3 4 2 2 -5 1 7 0" represents 3x^4 + 2x^2 - 5x + 7
 */
Polynomial* parse_polynomial(const char *str, char name) {
    Polynomial *poly = create_polynomial(name);
    if (poly == NULL) return NULL;
    
    const char *ptr = str;
    double coeff;
    int exp;
    
    while (sscanf(ptr, "%lf %d", &coeff, &exp) == 2) {
        add_term(poly, coeff, exp);
        /* Skip past the two numbers we just read */
        while (*ptr && (*ptr == ' ' || *ptr == '\t')) ptr++;
        while (*ptr && *ptr != ' ' && *ptr != '\t') ptr++;
        while (*ptr && (*ptr == ' ' || *ptr == '\t')) ptr++;
        while (*ptr && *ptr != ' ' && *ptr != '\t') ptr++;
    }
    
    return poly;
}

/**
 * Compares two polynomials for equality.
 */
bool polynomials_equal(const Polynomial *p1, const Polynomial *p2) {
    if (p1 == NULL || p2 == NULL) {
        return p1 == p2;
    }
    
    const Term *t1 = p1->head;
    const Term *t2 = p2->head;
    
    while (t1 != NULL && t2 != NULL) {
        if (t1->exponent != t2->exponent) return false;
        if (fabs(t1->coefficient - t2->coefficient) > EPSILON) return false;
        t1 = t1->next;
        t2 = t2->next;
    }
    
    return t1 == NULL && t2 == NULL;
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     Polynomial Calculator                                     ║\n");
    printf("║     Exercise 2 - Linked Lists                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* TODO 14: Add file processing mode
     *
     * If a filename is provided as argument, read polynomials from file
     * Otherwise, run the demo mode below
     */
    
    if (argc > 1) {
        /*
         * In file mode we do not run the demo. Instead we parse the command
         * file and emit results deterministically which makes automated
         * regression testing feasible.
         */
        printf("\n");
        process_polynomial_file(argv[1]);
        printf("\n--- Program finished ---\n\n");
        return 0;
    }
    
    /* Demo mode */
    printf("\n--- Demo Mode ---\n\n");
    
    /* Create polynomial P(x) = 3x^4 + 2x^2 - 5x + 7 */
    printf("Creating P(x)...\n");
    Polynomial *P = create_polynomial('P');
    add_term(P, 3.0, 4);
    add_term(P, 2.0, 2);
    add_term(P, -5.0, 1);
    add_term(P, 7.0, 0);
    display_polynomial(P);
    printf("Degree: %d\n", get_degree(P));
    
    /* Create polynomial Q(x) = x^3 - 2x^2 + 4 */
    printf("\nCreating Q(x)...\n");
    Polynomial *Q = create_polynomial('Q');
    add_term(Q, 1.0, 3);
    add_term(Q, -2.0, 2);
    add_term(Q, 4.0, 0);
    display_polynomial(Q);
    printf("Degree: %d\n", get_degree(Q));
    
    /* Addition */
    printf("\n--- Addition ---\n");
    Polynomial *sum = add_polynomials(P, Q, 'S');
    if (sum) {
        display_polynomial(sum);
    }
    
    /* Multiplication */
    printf("\n--- Multiplication ---\n");
    Polynomial *product = multiply_polynomials(P, Q, 'M');
    if (product) {
        display_polynomial(product);
    }
    
    /* Evaluation */
    printf("\n--- Evaluation ---\n");
    double x_values[] = {0, 1, 2, -1};
    for (int i = 0; i < 4; i++) {
        double x = x_values[i];
        printf("P(%.1f) = %.2f\n", x, evaluate_polynomial(P, x));
    }
    
    /* Derivative */
    printf("\n--- Derivative ---\n");
    Polynomial *dP = derivative(P, 'D');
    if (dP) {
        printf("P'(x) = ");
        display_polynomial(dP);
    }
    
    /* TODO 15: Free all allocated memory
     *
     * Don't forget to free all polynomials created!
     */
    
    free_polynomial(P);
    free_polynomial(Q);
    free_polynomial(sum);
    free_polynomial(product);
    free_polynomial(dP);
    
    printf("\n--- Program finished ---\n\n");
    
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Implement polynomial subtraction: subtract_polynomials(p1, p2)
 *
 * 2. Implement polynomial division with remainder (long division)
 *
 * 3. Find roots of quadratic polynomials using the quadratic formula
 *
 * 4. Implement Horner's method for more efficient evaluation
 *
 * 5. Add support for reading polynomials in standard notation:
 *    "3x^4 + 2x^2 - 5x + 7" instead of "3 4 2 2 -5 1 7 0"
 *
 * =============================================================================
 */

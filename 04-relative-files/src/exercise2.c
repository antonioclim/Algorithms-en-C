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

/* YOUR CODE HERE */
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
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace this */
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
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace this */
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
    
    /* YOUR CODE HERE */
    
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
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace this */
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
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace this */
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
    /* YOUR CODE HERE */
    
    return 0.0;  /* Replace this */
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
    
    /* YOUR CODE HERE */
    
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
    /* YOUR CODE HERE */
    
    return -1;  /* Replace this */
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
    /* YOUR CODE HERE */
    
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
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace this */
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
    /* YOUR CODE HERE */
    
    return true;  /* Replace this */
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
        printf("\nFile processing mode not yet implemented.\n");
        printf("Usage: %s <polynomial_file>\n", argv[0]);
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
    
    /* YOUR CODE HERE */
    
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

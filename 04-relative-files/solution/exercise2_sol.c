/**
 * =============================================================================
 * EXERCISE 2 SOLUTION: Polynomial Calculator
 * =============================================================================
 *
 * INSTRUCTOR COPY - Complete working solution
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -lm -o exercise2_sol exercise2_sol.c
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

typedef struct Term {
    double coefficient;
    int exponent;
    struct Term *next;
} Term;

typedef struct Polynomial {
    Term *head;
    char name;
} Polynomial;

/* =============================================================================
 * FUNCTION IMPLEMENTATIONS
 * =============================================================================
 */

/**
 * Creates a new term.
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
 * Creates an empty polynomial.
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
 * Adds a term to the polynomial in descending order by exponent.
 * Combines like terms automatically.
 */
void add_term(Polynomial *poly, double coeff, int exp) {
    if (poly == NULL || fabs(coeff) < EPSILON) {
        return;
    }
    
    /* Case 1: Empty polynomial or insert at beginning */
    if (poly->head == NULL || poly->head->exponent < exp) {
        Term *new_term = create_term(coeff, exp);
        if (new_term == NULL) return;
        new_term->next = poly->head;
        poly->head = new_term;
        return;
    }
    
    /* Case 2: Same exponent as head - combine */
    if (poly->head->exponent == exp) {
        poly->head->coefficient += coeff;
        /* Remove if result is zero */
        if (fabs(poly->head->coefficient) < EPSILON) {
            Term *temp = poly->head;
            poly->head = poly->head->next;
            free(temp);
        }
        return;
    }
    
    /* Case 3: Find correct position */
    Term *current = poly->head;
    while (current->next != NULL && current->next->exponent > exp) {
        current = current->next;
    }
    
    /* Check if we found a term with same exponent */
    if (current->next != NULL && current->next->exponent == exp) {
        current->next->coefficient += coeff;
        /* Remove if result is zero */
        if (fabs(current->next->coefficient) < EPSILON) {
            Term *temp = current->next;
            current->next = temp->next;
            free(temp);
        }
    } else {
        /* Insert new term */
        Term *new_term = create_term(coeff, exp);
        if (new_term == NULL) return;
        new_term->next = current->next;
        current->next = new_term;
    }
}

/**
 * Adds two polynomials.
 */
Polynomial* add_polynomials(const Polynomial *p1, const Polynomial *p2, char result_name) {
    Polynomial *result = create_polynomial(result_name);
    if (result == NULL) return NULL;
    
    /* Add all terms from p1 */
    const Term *t1 = p1 ? p1->head : NULL;
    while (t1 != NULL) {
        add_term(result, t1->coefficient, t1->exponent);
        t1 = t1->next;
    }
    
    /* Add all terms from p2 */
    const Term *t2 = p2 ? p2->head : NULL;
    while (t2 != NULL) {
        add_term(result, t2->coefficient, t2->exponent);
        t2 = t2->next;
    }
    
    return result;
}

/**
 * Multiplies two polynomials.
 */
Polynomial* multiply_polynomials(const Polynomial *p1, const Polynomial *p2, char result_name) {
    Polynomial *result = create_polynomial(result_name);
    if (result == NULL) return NULL;
    
    if (p1 == NULL || p2 == NULL || p1->head == NULL || p2->head == NULL) {
        return result;
    }
    
    /* For each term in p1, multiply by each term in p2 */
    const Term *t1 = p1->head;
    while (t1 != NULL) {
        const Term *t2 = p2->head;
        while (t2 != NULL) {
            double new_coeff = t1->coefficient * t2->coefficient;
            int new_exp = t1->exponent + t2->exponent;
            add_term(result, new_coeff, new_exp);
            t2 = t2->next;
        }
        t1 = t1->next;
    }
    
    return result;
}

/**
 * Evaluates the polynomial at x.
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
 * Displays the polynomial in mathematical notation.
 */
void display_polynomial(const Polynomial *poly) {
    if (poly == NULL) {
        printf("NULL polynomial\n");
        return;
    }
    
    printf("%c(x) = ", poly->name);
    
    if (poly->head == NULL) {
        printf("0\n");
        return;
    }
    
    const Term *current = poly->head;
    bool first = true;
    
    while (current != NULL) {
        double coeff = current->coefficient;
        int exp = current->exponent;
        
        /* Handle sign */
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
        
        /* Print term */
        if (exp == 0) {
            /* Constant term */
            printf("%.2g", coeff);
        } else if (exp == 1) {
            /* x^1 term */
            if (fabs(coeff - 1.0) < EPSILON) {
                printf("x");
            } else {
                printf("%.2gx", coeff);
            }
        } else {
            /* x^n term */
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
 * Gets the degree of the polynomial.
 */
int get_degree(const Polynomial *poly) {
    if (poly == NULL || poly->head == NULL) {
        return -1;
    }
    return poly->head->exponent;
}

/**
 * Computes the derivative.
 */
Polynomial* derivative(const Polynomial *poly, char result_name) {
    Polynomial *result = create_polynomial(result_name);
    if (result == NULL || poly == NULL) return result;
    
    const Term *current = poly->head;
    while (current != NULL) {
        if (current->exponent > 0) {
            double new_coeff = current->coefficient * current->exponent;
            int new_exp = current->exponent - 1;
            add_term(result, new_coeff, new_exp);
        }
        current = current->next;
    }
    
    return result;
}

/**
 * Checks if polynomial is zero.
 */
bool is_zero_polynomial(const Polynomial *poly) {
    return (poly == NULL || poly->head == NULL);
}

/**
 * Frees a polynomial.
 */
void free_polynomial(Polynomial *poly) {
    if (poly == NULL) return;
    
    Term *current = poly->head;
    Term *next;
    
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    
    free(poly);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     Polynomial Calculator - SOLUTION                          ║\n");
    printf("║     Exercise 2 - Linked Lists                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    printf("\n--- Demo Mode ---\n\n");
    
    /* Create P(x) = 3x^4 + 2x^2 - 5x + 7 */
    printf("Creating P(x)...\n");
    Polynomial *P = create_polynomial('P');
    add_term(P, 3.0, 4);
    add_term(P, 2.0, 2);
    add_term(P, -5.0, 1);
    add_term(P, 7.0, 0);
    display_polynomial(P);
    printf("Degree: %d\n", get_degree(P));
    
    /* Create Q(x) = x^3 - 2x^2 + 4 */
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
    display_polynomial(sum);
    
    /* Multiplication */
    printf("\n--- Multiplication ---\n");
    Polynomial *product = multiply_polynomials(P, Q, 'M');
    display_polynomial(product);
    
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
    printf("P'(x) = ");
    display_polynomial(dP);
    
    /* Free all memory */
    free_polynomial(P);
    free_polynomial(Q);
    free_polynomial(sum);
    free_polynomial(product);
    free_polynomial(dP);
    
    printf("\n--- Program finished ---\n\n");
    
    return 0;
}

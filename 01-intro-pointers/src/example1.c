/**
 * =============================================================================
 * WEEK 01: POINTERS TO FUNCTIONS AND CALLBACKS IN C
 * Complete Working Example
 * =============================================================================
 *
 * This example demonstrates:
 *   1. Function pointer declaration and usage
 *   2. Callbacks for generic operations
 *   3. qsort() with multiple comparator functions
 *   4. bsearch() for efficient searching
 *   5. Dispatch tables using function pointer arrays
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Usage: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =============================================================================
 * PART 1: BASIC FUNCTION POINTERS
 * =============================================================================
 */

/**
 * Simple arithmetic operations
 * These functions will be used through function pointers
 */
int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide_safe(int a, int b) { return (b != 0) ? (a / b) : 0; }
int modulo(int a, int b) { return (b != 0) ? (a % b) : 0; }

/**
 * Demonstrates basic function pointer usage
 */
void demo_basic_function_pointers(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: BASIC FUNCTION POINTERS                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    /* Declare a function pointer */
    int (*operation)(int, int);

    int a = 20, b = 6;

    /* Method 1: Direct assignment (function name decays to pointer) */
    operation = add;
    printf("Direct assignment:\n");
    printf("  operation = add;     → %d + %d = %d\n", a, b, operation(a, b));

    /* Method 2: Using address-of operator (explicit) */
    operation = &subtract;
    printf("  operation = &subtract; → %d - %d = %d\n", a, b, operation(a, b));

    /* Method 3: Explicit dereference when calling (older style) */
    operation = multiply;
    printf("  (*operation)(a, b)     → %d * %d = %d\n", a, b, (*operation)(a, b));

    printf("\n");
}

/* =============================================================================
 * PART 2: CALLBACKS FOR GENERIC OPERATIONS
 * =============================================================================
 */

/**
 * Type definition for cleaner code
 */
typedef int (*BinaryOperation)(int, int);

/**
 * Generic function that applies any binary operation to two numbers
 * This is the callback pattern in action!
 *
 * @param x First operand
 * @param y Second operand
 * @param op Function pointer to the operation
 * @return Result of applying op to x and y
 */
int apply_operation(int x, int y, BinaryOperation op) {
    if (op == NULL) {
        fprintf(stderr, "Error: NULL function pointer\n");
        return 0;
    }
    return op(x, y);
}

/**
 * Apply operation to entire array
 */
void apply_to_array(int *arr, int n, int operand, BinaryOperation op) {
    for (int i = 0; i < n; i++) {
        arr[i] = op(arr[i], operand);
    }
}

void demo_callbacks(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: CALLBACKS FOR GENERIC OPERATIONS                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    int a = 15, b = 4;

    printf("Using apply_operation() with different callbacks:\n");
    printf("  apply_operation(%d, %d, add)      = %d\n", a, b, apply_operation(a, b, add));
    printf("  apply_operation(%d, %d, subtract) = %d\n", a, b, apply_operation(a, b, subtract));
    printf("  apply_operation(%d, %d, multiply) = %d\n", a, b, apply_operation(a, b, multiply));
    printf("  apply_operation(%d, %d, divide)   = %d\n", a, b, apply_operation(a, b, divide_safe));

    /* Apply to array */
    printf("\nApply operation to array:\n");
    int arr[] = {1, 2, 3, 4, 5};
    int n = 5;

    printf("  Original: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");

    apply_to_array(arr, n, 10, multiply);
    printf("  After multiply by 10: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n\n");
}

/* =============================================================================
 * PART 3: QSORT() WITH STRUCTURES
 * =============================================================================
 */

/**
 * Student structure for demonstration
 */
typedef struct {
    int id;
    char name[50];
    float grade;
} Student;

/* Comparator functions for qsort() */

/**
 * Compare students by ID (ascending)
 */
int cmp_by_id(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;
    return s1->id - s2->id;
}

/**
 * Compare students by name (alphabetical)
 */
int cmp_by_name(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;
    return strcmp(s1->name, s2->name);
}

/**
 * Compare students by grade (descending - highest first)
 * Note: For floats, don't use subtraction! Use conditionals.
 */
int cmp_by_grade_desc(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;

    if (s2->grade > s1->grade) return 1;
    if (s2->grade < s1->grade) return -1;
    return 0;
}

/**
 * Compare students by grade (ascending)
 */
int cmp_by_grade_asc(const void *a, const void *b) {
    return -cmp_by_grade_desc(a, b);
}

/**
 * Print student list in a nice table format
 */
void print_students(Student *students, int n, const char *title) {
    printf("\n%s:\n", title);
    printf("  ┌─────┬────────────────────────┬────────┐\n");
    printf("  │ ID  │ Name                   │ Grade  │\n");
    printf("  ├─────┼────────────────────────┼────────┤\n");
    for (int i = 0; i < n; i++) {
        printf("  │ %3d │ %-22s │ %6.2f │\n",
               students[i].id, students[i].name, students[i].grade);
    }
    printf("  └─────┴────────────────────────┴────────┘\n");
}

void demo_qsort(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: QSORT() WITH STRUCTURES                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    Student students[] = {
        {105, "Popescu Alexandru", 8.50},
        {101, "Ionescu Maria", 9.25},
        {103, "Georgescu Ana", 7.80},
        {102, "Vasilescu Dan", 9.50},
        {104, "Marinescu Elena", 8.90},
        {106, "Dumitrescu Mihai", 6.75}
    };
    int n = sizeof(students) / sizeof(students[0]);

    print_students(students, n, "Original list");

    /* Sort by ID */
    qsort(students, n, sizeof(Student), cmp_by_id);
    print_students(students, n, "Sorted by ID (ascending)");

    /* Sort by name */
    qsort(students, n, sizeof(Student), cmp_by_name);
    print_students(students, n, "Sorted by Name (alphabetical)");

    /* Sort by grade (descending) */
    qsort(students, n, sizeof(Student), cmp_by_grade_desc);
    print_students(students, n, "Sorted by Grade (descending)");

    /* Display top 3 */
    printf("\n🏆 Top 3 Students:\n");
    for (int i = 0; i < 3 && i < n; i++) {
        printf("   %d. %s - %.2f\n", i + 1, students[i].name, students[i].grade);
    }
    printf("\n");
}

/* =============================================================================
 * PART 4: BSEARCH()
 * =============================================================================
 */

/**
 * Comparator for bsearch: key is a string (name to find)
 * Note: In bsearch, first parameter is always the KEY!
 */
int cmp_search_by_name(const void *key, const void *element) {
    const char *search_name = (const char *)key;
    const Student *student = (const Student *)element;
    return strcmp(search_name, student->name);
}

/**
 * Comparator for bsearch: key is an int (ID to find)
 */
int cmp_search_by_id(const void *key, const void *element) {
    int search_id = *(const int *)key;
    const Student *student = (const Student *)element;
    return search_id - student->id;
}

void demo_bsearch(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: BSEARCH()                                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    Student students[] = {
        {101, "Georgescu Ana", 7.80},
        {102, "Ionescu Maria", 9.25},
        {103, "Marinescu Elena", 8.90},
        {104, "Popescu Alexandru", 8.50},
        {105, "Vasilescu Dan", 9.50}
    };
    int n = sizeof(students) / sizeof(students[0]);

    /* Array MUST be sorted by the search key! */
    qsort(students, n, sizeof(Student), cmp_by_name);
    print_students(students, n, "Sorted by name (required for bsearch)");

    /* Search for students */
    printf("\nSearching by name:\n");
    const char *names_to_find[] = {"Ionescu Maria", "Popescu Alexandru", "Unknown Person"};

    for (int i = 0; i < 3; i++) {
        Student *found = (Student *)bsearch(
            names_to_find[i],
            students,
            n,
            sizeof(Student),
            cmp_search_by_name
        );

        if (found) {
            printf("  ✓ Found '%s': ID=%d, Grade=%.2f\n",
                   names_to_find[i], found->id, found->grade);
        } else {
            printf("  ✗ '%s' not found\n", names_to_find[i]);
        }
    }

    /* Search by ID (need to re-sort!) */
    printf("\nSearching by ID:\n");
    qsort(students, n, sizeof(Student), cmp_by_id);

    int ids_to_find[] = {103, 101, 999};
    for (int i = 0; i < 3; i++) {
        Student *found = (Student *)bsearch(
            &ids_to_find[i],
            students,
            n,
            sizeof(Student),
            cmp_search_by_id
        );

        if (found) {
            printf("  ✓ Found ID %d: %s, Grade=%.2f\n",
                   ids_to_find[i], found->name, found->grade);
        } else {
            printf("  ✗ ID %d not found\n", ids_to_find[i]);
        }
    }
    printf("\n");
}

/* =============================================================================
 * PART 5: DISPATCH TABLE
 * =============================================================================
 */

/**
 * Dispatch table: array of function pointers indexed by operator character
 */
static BinaryOperation dispatch_table[256] = {NULL};

/**
 * Initialize the dispatch table
 */
void init_dispatch_table(void) {
    dispatch_table['+'] = add;
    dispatch_table['-'] = subtract;
    dispatch_table['*'] = multiply;
    dispatch_table['/'] = divide_safe;
    dispatch_table['%'] = modulo;
}

/**
 * Calculate using dispatch table (no switch statement!)
 */
int calculate(int a, char op, int b) {
    if (dispatch_table[(unsigned char)op] != NULL) {
        return dispatch_table[(unsigned char)op](a, b);
    }
    fprintf(stderr, "Unknown operator: '%c'\n", op);
    return 0;
}

void demo_dispatch_table(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: DISPATCH TABLE                                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    init_dispatch_table();

    printf("Calculator using dispatch table:\n");
    printf("  15 + 4 = %d\n", calculate(15, '+', 4));
    printf("  15 - 4 = %d\n", calculate(15, '-', 4));
    printf("  15 * 4 = %d\n", calculate(15, '*', 4));
    printf("  15 / 4 = %d\n", calculate(15, '/', 4));
    printf("  15 %% 4 = %d\n", calculate(15, '%', 4));
    printf("  15 ^ 4 = %d (unknown operator)\n", calculate(15, '^', 4));

    printf("\nAdvantages of dispatch tables:\n");
    printf("  • O(1) lookup (direct array access)\n");
    printf("  • Easy to add/remove operations\n");
    printf("  • Cleaner than long switch statements\n");
    printf("  • Operations can be modified at runtime\n");
    printf("\n");
}

/* =============================================================================
 * PART 6: SORTING INTEGERS WITH VARIOUS CRITERIA
 * =============================================================================
 */

/* Safe comparator pattern for integers */
int cmp_int_asc(const void *a, const void *b) {
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    return (ia > ib) - (ia < ib);  /* Safe: no overflow */
}

int cmp_int_desc(const void *a, const void *b) {
    return -cmp_int_asc(a, b);
}

int cmp_int_abs(const void *a, const void *b) {
    int ia = abs(*(const int *)a);
    int ib = abs(*(const int *)b);
    return (ia > ib) - (ia < ib);
}

/* Evens first, then odds */
int cmp_even_odd(const void *a, const void *b) {
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    int even_a = (ia % 2 == 0);
    int even_b = (ib % 2 == 0);

    if (even_a != even_b) {
        return even_b - even_a;  /* Evens first */
    }
    return ia - ib;  /* Same parity: sort by value */
}

void print_int_array(int *arr, int n, const char *title) {
    printf("  %s: ", title);
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void demo_integer_sorting(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: SORTING INTEGERS WITH VARIOUS CRITERIA          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    int original[] = {-7, 3, -2, 8, 1, -5, 6, 4, -9, 2};
    int n = sizeof(original) / sizeof(original[0]);
    int arr[10];

    print_int_array(original, n, "Original       ");
    printf("\n");

    memcpy(arr, original, sizeof(original));
    qsort(arr, n, sizeof(int), cmp_int_asc);
    print_int_array(arr, n, "Ascending      ");

    memcpy(arr, original, sizeof(original));
    qsort(arr, n, sizeof(int), cmp_int_desc);
    print_int_array(arr, n, "Descending     ");

    memcpy(arr, original, sizeof(original));
    qsort(arr, n, sizeof(int), cmp_int_abs);
    print_int_array(arr, n, "By |value|     ");

    memcpy(arr, original, sizeof(original));
    qsort(arr, n, sizeof(int), cmp_even_odd);
    print_int_array(arr, n, "Evens first    ");

    printf("\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 01: POINTERS TO FUNCTIONS AND CALLBACKS IN C        ║\n");
    printf("║                    Complete Example                          ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    demo_basic_function_pointers();
    demo_callbacks();
    demo_qsort();
    demo_bsearch();
    demo_dispatch_table();
    demo_integer_sorting();

    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    DEMONSTRATION COMPLETE                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    return 0;
}

/**
 * =============================================================================
 * EXERCISE 2: Student Database with Sorting and Searching
 * =============================================================================
 *
 * OBJECTIVE:
 *   Create a student management system using qsort() for sorting and
 *   bsearch() for efficient searching.
 *
 * REQUIREMENTS:
 *   1. Define a Student structure (id, name, grade)
 *   2. Implement comparators for sorting by each field
 *   3. Load students from a file
 *   4. Sort and display students by different criteria
 *   5. Search for students by name using bsearch()
 *   6. Display top N students by grade
 *
 * INPUT FILE FORMAT (data/students.txt):
 *   id,name,grade
 *   101,Popescu Ion,8.50
 *   102,Ionescu Maria,9.25
 *   ...
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =============================================================================
 * CONSTANTS AND TYPE DEFINITIONS
 * =============================================================================
 */

#define MAX_STUDENTS 100
#define MAX_NAME_LEN 50
#define MAX_LINE_LEN 128

/**
 * TODO 1: Define the Student structure
 *
 * Fields:
 *   - int id          (student ID number)
 *   - char name[50]   (student full name)
 *   - float grade     (grade from 0.0 to 10.0)
 */
typedef struct {
    /* YOUR CODE HERE */
    int id;          /* Remove this placeholder */
    char name[50];   /* Remove this placeholder */
    float grade;     /* Remove this placeholder */
} Student;


/* =============================================================================
 * COMPARATOR FUNCTIONS FOR QSORT
 * =============================================================================
 */

/**
 * TODO 2: Compare students by ID (ascending)
 *
 * @param a Pointer to first Student (as const void*)
 * @param b Pointer to second Student (as const void*)
 * @return Negative if a < b, 0 if equal, positive if a > b
 *
 * Steps:
 *   1. Cast void pointers to const Student*
 *   2. Compare the id fields
 *   3. Return the difference (or use safe comparison)
 */
int cmp_by_id(const void *a, const void *b) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}

/**
 * TODO 3: Compare students by name (alphabetical, case-sensitive)
 *
 * Hint: Use strcmp() from <string.h>
 */
int cmp_by_name(const void *a, const void *b) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}

/**
 * TODO 4: Compare students by grade (descending - highest first)
 *
 * IMPORTANT: For float comparison, don't use subtraction!
 * Use conditional comparison to avoid floating-point issues.
 *
 * if (grade2 > grade1) return 1;
 * if (grade2 < grade1) return -1;
 * return 0;
 */
int cmp_by_grade_desc(const void *a, const void *b) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}

/**
 * Compare students by grade (ascending - lowest first)
 */
int cmp_by_grade_asc(const void *a, const void *b) {
    return -cmp_by_grade_desc(a, b);
}


/* =============================================================================
 * COMPARATOR FOR BSEARCH
 * =============================================================================
 */

/**
 * TODO 5: Comparator for searching by name
 *
 * IMPORTANT: In bsearch(), the FIRST parameter is the KEY (what we search for),
 * and the SECOND parameter is the array element.
 *
 * @param key The name to search for (const char*)
 * @param element The Student to compare with (const Student*)
 *
 * Hint:
 *   const char *search_name = (const char*)key;
 *   const Student *student = (const Student*)element;
 *   return strcmp(search_name, student->name);
 */
int cmp_search_name(const void *key, const void *element) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}


/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Print a single student's information
 */
void print_student(const Student *s) {
    printf("  │ %4d │ %-30s │ %6.2f │\n", s->id, s->name, s->grade);
}

/**
 * Print all students in a formatted table
 */
void print_students(Student *students, int n, const char *title) {
    printf("\n%s (%d students):\n", title, n);
    printf("  ┌──────┬────────────────────────────────┬────────┐\n");
    printf("  │  ID  │ Name                           │ Grade  │\n");
    printf("  ├──────┼────────────────────────────────┼────────┤\n");

    for (int i = 0; i < n; i++) {
        print_student(&students[i]);
    }

    printf("  └──────┴────────────────────────────────┴────────┘\n");
}

/**
 * TODO 6: Load students from a CSV file
 *
 * File format: id,name,grade (one student per line)
 * First line may be a header (skip if starts with "id")
 *
 * @param filename Path to the CSV file
 * @param students Array to store loaded students
 * @param max_students Maximum capacity of the array
 * @return Number of students loaded, or -1 on error
 *
 * Steps:
 *   1. Open the file with fopen()
 *   2. Read lines with fgets()
 *   3. Parse each line with sscanf() or strtok()
 *   4. Store in students array
 *   5. Close file and return count
 *
 * Hint for parsing:
 *   sscanf(line, "%d,%49[^,],%f", &id, name, &grade)
 *   The %49[^,] reads up to 49 chars until a comma
 */
int load_students(const char *filename, Student *students, int max_students) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    char line[MAX_LINE_LEN];
    int count = 0;

    /* TODO: Read and parse each line */
    while (fgets(line, sizeof(line), file) != NULL && count < max_students) {
        /* Skip header line if present */
        if (count == 0 && strncmp(line, "id", 2) == 0) {
            continue;
        }

        /* YOUR CODE HERE */
        /* Parse the line and store in students[count] */
        /* Increment count if successful */

    }

    fclose(file);
    return count;
}

/**
 * TODO 7: Find and display top N students by grade
 *
 * @param students Array of students (will be sorted in place)
 * @param n Number of students in array
 * @param top_n Number of top students to display
 *
 * Steps:
 *   1. Sort students by grade (descending)
 *   2. Print the top N students
 */
void show_top_students(Student *students, int n, int top_n) {
    /* YOUR CODE HERE */
    /* Sort by grade descending */
    /* Print top N */

    printf("\n🏆 Top %d Students:\n", top_n);
    printf("   (Not implemented yet)\n");
}

/**
 * TODO 8: Search for a student by name
 *
 * @param students Array of students (MUST be sorted by name!)
 * @param n Number of students
 * @param name Name to search for
 * @return Pointer to found student, or NULL if not found
 *
 * Steps:
 *   1. Use bsearch() with cmp_search_name comparator
 *   2. Return the result (pointer or NULL)
 *
 * IMPORTANT: Array must be sorted by name before calling bsearch!
 */
Student* find_student_by_name(Student *students, int n, const char *name) {
    /* YOUR CODE HERE */
    return NULL;  /* Replace this */
}


/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    Student students[MAX_STUDENTS];
    int count;

    /* Determine input file */
    const char *filename = (argc > 1) ? argv[1] : "data/students.txt";

    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           STUDENT DATABASE MANAGEMENT SYSTEM                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    /* Load students from file */
    printf("\nLoading students from: %s\n", filename);
    count = load_students(filename, students, MAX_STUDENTS);

    if (count <= 0) {
        printf("No students loaded. Using sample data.\n");

        /* Sample data if file not found */
        students[0] = (Student){105, "Popescu Alexandru", 8.50};
        students[1] = (Student){101, "Ionescu Maria", 9.25};
        students[2] = (Student){103, "Georgescu Ana", 7.80};
        students[3] = (Student){102, "Vasilescu Dan", 9.50};
        students[4] = (Student){104, "Marinescu Elena", 8.90};
        students[5] = (Student){106, "Dumitrescu Mihai", 6.75};
        count = 6;
    }

    printf("Loaded %d students.\n", count);

    /* Display original list */
    print_students(students, count, "Original List");

    /* TODO 9: Sort by ID and display */
    printf("\n─── Sorting by ID ───\n");
    /* YOUR CODE HERE: qsort by id, then print_students */


    /* TODO 10: Sort by name and display */
    printf("\n─── Sorting by Name ───\n");
    /* YOUR CODE HERE: qsort by name, then print_students */


    /* TODO 11: Show top 3 students */
    printf("\n─── Top Students ───\n");
    show_top_students(students, count, 3);


    /* TODO 12: Search for students by name */
    printf("\n─── Search Results ───\n");
    /* First, sort by name for bsearch to work! */
    qsort(students, count, sizeof(Student), cmp_by_name);

    const char *names_to_find[] = {"Ionescu Maria", "Vasilescu Dan", "Unknown Person"};
    for (int i = 0; i < 3; i++) {
        printf("Searching for '%s': ", names_to_find[i]);

        Student *found = find_student_by_name(students, count, names_to_find[i]);

        if (found) {
            printf("Found! ID=%d, Grade=%.2f\n", found->id, found->grade);
        } else {
            printf("Not found.\n");
        }
    }

    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    PROGRAM COMPLETE                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    return 0;
}


/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Implement case-insensitive name comparison using strcasecmp()
 *
 * 2. Add a function to find all students with grade above a threshold
 *
 * 3. Implement multi-field sorting (e.g., by grade then by name)
 *
 * 4. Add a menu-driven interface for interactive use
 *
 * 5. Implement save_students() to write back to CSV
 *
 * =============================================================================
 */

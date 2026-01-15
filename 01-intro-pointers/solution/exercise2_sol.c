/**
 * =============================================================================
 * SOLUTION: Exercise 2 - Student Database with Sorting and Searching
 * =============================================================================
 * INSTRUCTOR USE ONLY - Do not distribute to students
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 100
#define MAX_NAME_LEN 50
#define MAX_LINE_LEN 128

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    float grade;
} Student;

/* Comparators for qsort */
int cmp_by_id(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;
    return s1->id - s2->id;
}

int cmp_by_name(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;
    return strcmp(s1->name, s2->name);
}

int cmp_by_grade_desc(const void *a, const void *b) {
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;

    if (s2->grade > s1->grade) return 1;
    if (s2->grade < s1->grade) return -1;
    return 0;
}

int cmp_by_grade_asc(const void *a, const void *b) {
    return -cmp_by_grade_desc(a, b);
}

/* Comparator for bsearch (key is name string) */
int cmp_search_name(const void *key, const void *element) {
    const char *search_name = (const char *)key;
    const Student *student = (const Student *)element;
    return strcmp(search_name, student->name);
}

/* Utility functions */
void print_student(const Student *s) {
    printf("  │ %4d │ %-30s │ %6.2f │\n", s->id, s->name, s->grade);
}

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

int load_students(const char *filename, Student *students, int max_students) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    char line[MAX_LINE_LEN];
    int count = 0;
    int line_num = 0;

    while (fgets(line, sizeof(line), file) != NULL && count < max_students) {
        line_num++;

        /* Remove trailing newline */
        line[strcspn(line, "\n")] = '\0';

        /* Skip header line */
        if (line_num == 1 && strncmp(line, "id", 2) == 0) {
            continue;
        }

        /* Skip empty lines */
        if (strlen(line) == 0) {
            continue;
        }

        /* Parse: id,name,grade */
        int id;
        char name[MAX_NAME_LEN];
        float grade;

        if (sscanf(line, "%d,%49[^,],%f", &id, name, &grade) == 3) {
            students[count].id = id;
            strncpy(students[count].name, name, MAX_NAME_LEN - 1);
            students[count].name[MAX_NAME_LEN - 1] = '\0';
            students[count].grade = grade;
            count++;
        } else {
            fprintf(stderr, "Warning: Could not parse line %d: %s\n", line_num, line);
        }
    }

    fclose(file);
    return count;
}

void show_top_students(Student *students, int n, int top_n) {
    /* Sort by grade descending */
    qsort(students, n, sizeof(Student), cmp_by_grade_desc);

    printf("\n🏆 Top %d Students:\n", top_n);
    for (int i = 0; i < top_n && i < n; i++) {
        printf("   %d. %s - %.2f\n", i + 1, students[i].name, students[i].grade);
    }
}

Student* find_student_by_name(Student *students, int n, const char *name) {
    return (Student *)bsearch(name, students, n, sizeof(Student), cmp_search_name);
}

int main(int argc, char *argv[]) {
    Student students[MAX_STUDENTS];
    int count;

    const char *filename = (argc > 1) ? argv[1] : "data/students.txt";

    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           STUDENT DATABASE MANAGEMENT SYSTEM                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    printf("\nLoading students from: %s\n", filename);
    count = load_students(filename, students, MAX_STUDENTS);

    if (count <= 0) {
        printf("No students loaded. Using sample data.\n");

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

    /* Sort by ID */
    printf("\n─── Sorting by ID ───\n");
    qsort(students, count, sizeof(Student), cmp_by_id);
    print_students(students, count, "Sorted by ID");

    /* Sort by name */
    printf("\n─── Sorting by Name ───\n");
    qsort(students, count, sizeof(Student), cmp_by_name);
    print_students(students, count, "Sorted by Name");

    /* Show top 3 students */
    printf("\n─── Top Students ───\n");
    show_top_students(students, count, 3);

    /* Search for students (re-sort by name first!) */
    printf("\n─── Search Results ───\n");
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

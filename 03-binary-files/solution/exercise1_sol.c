/**
 * =============================================================================
 * EXERCISE 1: Student Records Database - SOLUTION
 * =============================================================================
 *
 * INSTRUCTOR COPY - Contains complete implementation
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
 * NOTE ON RECORD LAYOUT
 *
 * The laboratory exercises deliberately write and read fixed-size records via
 * fwrite and fread. Default struct alignment typically introduces padding for
 * MAX_NAME_LENGTH = 50 which would change the on-disk size and break
 * deterministic regression output.
 *
 * For that reason the reference solution also forces a packed layout for the
 * Student record. In production you would prefer explicit serialisation.
 */

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

#define MAX_NAME_LENGTH     50
#define DATABASE_FILE       "students.bin"

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * Student structure - fixed size for binary storage
 */
#if defined(__GNUC__) || defined(__clang__)
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif

#if defined(_MSC_VER)
#pragma pack(push, 1)
#endif

typedef struct PACKED {
    int32_t id;
    char    name[MAX_NAME_LENGTH];
    float   gpa;
    int32_t year;
} Student;

#if defined(_MSC_VER)
#pragma pack(pop)
#endif

/* =============================================================================
 * FUNCTION PROTOTYPES
 * =============================================================================
 */

int save_student(const char *filename, const Student *student);
int load_students(const char *filename, Student **students, size_t *count);
Student *find_student_by_id(Student *students, size_t count, int32_t id);
int update_student(const char *filename, size_t index, const Student *student);
size_t count_students(const char *filename);
void print_student_table(const Student *students, size_t count);
void free_students(Student *students);

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

static void init_student(Student *s, int32_t id, const char *name, 
                         float gpa, int32_t year) {
    memset(s, 0, sizeof(Student));
    s->id = id;
    strncpy(s->name, name, MAX_NAME_LENGTH - 1);
    s->name[MAX_NAME_LENGTH - 1] = '\0';
    s->gpa = gpa;
    s->year = year;
}

static void replace_underscores(char *str) {
    for (char *p = str; *p; p++) {
        if (*p == '_') *p = ' ';
    }
}

/* =============================================================================
 * SOLUTION IMPLEMENTATIONS
 * =============================================================================
 */

/**
 * SOLUTION: save_student()
 * 
 * Appends a single student record to the binary file.
 */
int save_student(const char *filename, const Student *student) {
    /* Open file in append binary mode */
    FILE *fp = fopen(filename, "ab");
    if (fp == NULL) {
        perror("Error opening file for writing");
        return -1;
    }
    
    /* Write the student structure */
    size_t written = fwrite(student, sizeof(Student), 1, fp);
    
    /* Close the file */
    fclose(fp);
    
    /* Check if write was successful */
    if (written != 1) {
        fprintf(stderr, "Error: failed to write student record\n");
        return -1;
    }
    
    return 0;
}

/**
 * SOLUTION: load_students()
 * 
 * Reads all student records from the binary file.
 */
int load_students(const char *filename, Student **students, size_t *count) {
    /* Initialise output parameters */
    *students = NULL;
    *count = 0;
    
    /* Open file in read binary mode */
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        /* File doesn't exist - not an error, just empty database */
        return 0;
    }
    
    /* Calculate file size */
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    /* Handle empty file */
    if (file_size == 0) {
        fclose(fp);
        return 0;
    }
    
    /* Calculate number of records */
    *count = file_size / sizeof(Student);
    
    /* Allocate memory for all records */
    *students = malloc(*count * sizeof(Student));
    if (*students == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        fclose(fp);
        *count = 0;
        return -1;
    }
    
    /* Read all records */
    size_t read = fread(*students, sizeof(Student), *count, fp);
    
    /* Close file */
    fclose(fp);
    
    /* Verify all records were read */
    if (read != *count) {
        fprintf(stderr, "Warning: only read %zu of %zu records\n", read, *count);
        *count = read;
    }
    
    return 0;
}

/**
 * SOLUTION: find_student_by_id()
 * 
 * Sequential search for a student by ID.
 */
Student *find_student_by_id(Student *students, size_t count, int32_t id) {
    if (students == NULL || count == 0) {
        return NULL;
    }
    
    for (size_t i = 0; i < count; i++) {
        if (students[i].id == id) {
            return &students[i];
        }
    }
    
    return NULL;
}

/**
 * SOLUTION: update_student()
 * 
 * Updates a student record at a specific index.
 */
int update_student(const char *filename, size_t index, const Student *student) {
    /* Open file in read+write binary mode */
    FILE *fp = fopen(filename, "r+b");
    if (fp == NULL) {
        perror("Error opening file for update");
        return -1;
    }
    
    /* Calculate byte offset */
    long offset = (long)(index * sizeof(Student));
    
    /* Seek to the record position */
    if (fseek(fp, offset, SEEK_SET) != 0) {
        perror("Error seeking to record");
        fclose(fp);
        return -1;
    }
    
    /* Write the updated record */
    size_t written = fwrite(student, sizeof(Student), 1, fp);
    
    /* Flush to ensure write is committed */
    fflush(fp);
    
    /* Close file */
    fclose(fp);
    
    if (written != 1) {
        fprintf(stderr, "Error: failed to write updated record\n");
        return -1;
    }
    
    return 0;
}

/**
 * SOLUTION: count_students()
 * 
 * Counts records without loading them.
 */
size_t count_students(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        return 0;
    }
    
    /* Seek to end to get file size */
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fclose(fp);
    
    if (file_size <= 0) {
        return 0;
    }
    
    return (size_t)file_size / sizeof(Student);
}

/**
 * SOLUTION: print_student_table()
 * 
 * Displays students in a formatted table.
 */
void print_student_table(const Student *students, size_t count) {
    if (students == NULL || count == 0) {
        printf("[INFO] No students to display\n");
        return;
    }
    
    /* Print table header (verbatim to match the published reference output) */
    printf("┌────────┬──────────────────────┬───────┬──────┐\n");
    printf("│   ID   │        Name          │  GPA  │ Year │\n");
    printf("├────────┼──────────────────────┼───────┼──────┤\n");
    
    /* Print each student */
    for (size_t i = 0; i < count; i++) {
        printf("│ %6d │ %-20s │ %5.2f │ %4d │\n",
               students[i].id,
               students[i].name,
               students[i].gpa,
               students[i].year);
    }
    
    /* Print table footer */
    printf("└────────┴──────────────────────┴───────┴──────┘\n");
}

/**
 * SOLUTION: free_students()
 * 
 * Frees dynamically allocated students array.
 */
void free_students(Student *students) {
    if (students != NULL) {
        free(students);
    }
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                    STUDENT RECORDS DATABASE                    ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
    
    /* Remove any existing database file for clean start */
    remove(DATABASE_FILE);
    
    /* Read number of students to add */
    int n;
    printf("Enter number of students: ");
    if (scanf("%d", &n) != 1 || n < 0) {
        printf("[ERROR] Invalid input\n");
        return 1;
    }
    
    /* Read and save each student */
    printf("\n[INFO] Saving %d students to database...\n", n);
    
    for (int i = 0; i < n; i++) {
        int32_t id, year;
        char name[MAX_NAME_LENGTH];
        float gpa;
        
        if (scanf("%d %49s %f %d", &id, name, &gpa, &year) != 4) {
            printf("[ERROR] Invalid input for student %d\n", i + 1);
            continue;
        }
        
        replace_underscores(name);
        
        Student s;
        init_student(&s, id, name, gpa, year);
        
        if (save_student(DATABASE_FILE, &s) == 0) {
            printf("[OK] Student %d saved successfully\n", id);
        } else {
            printf("[ERROR] Failed to save student %d\n", id);
        }
    }
    
    /* Load and display all students */
    printf("\n[INFO] Loading all students from database...\n");
    
    Student *students = NULL;
    size_t count = 0;
    
    if (load_students(DATABASE_FILE, &students, &count) == 0) {
        print_student_table(students, count);
    } else {
        printf("[ERROR] Failed to load students\n");
    }
    
    /* Display file statistics */
    printf("\nTotal records: %zu\n", count);
    printf("File size: %zu bytes\n", count * sizeof(Student));
    
    /* Test search functionality */
    if (count > 0) {
        printf("\n[INFO] Testing search functionality...\n");
        
        /* Search for first student */
        int32_t search_id = students[0].id;
        Student *found = find_student_by_id(students, count, search_id);
        
        if (found != NULL) {
            printf("[OK] Found student %d: %s\n", found->id, found->name);
        } else {
            printf("[ERROR] Student %d not found\n", search_id);
        }
        
        /* Search for non-existent student */
        found = find_student_by_id(students, count, 99999);
        if (found == NULL) {
            printf("[OK] Correctly returned NULL for non-existent ID\n");
        }
    }
    
    /* Clean up */
    free_students(students);
    
    printf("\n[INFO] Database operations complete\n\n");
    
    return 0;
}

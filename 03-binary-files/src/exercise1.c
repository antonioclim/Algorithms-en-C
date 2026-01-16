/**
 * =============================================================================
 * EXERCISE 1: Student Records Database
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement a binary file-based student records database with support for
 *   creating, reading, searching and updating student records.
 *
 * REQUIREMENTS:
 *   1. Define a Student structure with id, name, gpa and year fields
 *   2. Implement save_student() to append records to a binary file
 *   3. Implement load_students() to read all records from the file
 *   4. Implement find_student_by_id() using sequential search
 *   5. Implement update_student() to modify a record in place
 *   6. Implement count_students() using file size calculation
 *   7. Display formatted output with proper table alignment
 *   8. Handle all file operation errors gracefully
 *
 * EXAMPLE INPUT:
 *   3
 *   1001 Alice_Johnson 3.75 2024
 *   1002 Bob_Smith 3.42 2023
 *   1003 Carol_Williams 3.91 2024
 *
 * EXPECTED OUTPUT:
 *   (See tests/test1_expected.txt)
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* =============================================================================
 * PORTABILITY NOTE: STRUCT LAYOUT AND PACKING
 * =============================================================================
 *
 * This exercise persists records by writing the in-memory struct layout
 * directly to disk via fwrite and reading them back via fread.
 *
 * In standard C the compiler may insert padding bytes between fields and at
 * the end of a struct to satisfy alignment constraints. That padding is
 * implementation-defined and may vary across compilers, optimisation levels
 * and architectures.
 *
 * For teaching purposes and to keep the on-disk format deterministic in the
 * provided tests we force a packed layout for Student. In production code you
 * would typically define an explicit serialisation format instead of relying
 * on packed structs.
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
 * TODO 1: Define the Student structure
 *
 * The structure should contain:
 *   - id:   a 32-bit integer for the student ID
 *   - name: a character array of MAX_NAME_LENGTH for the student's name
 *   - gpa:  a float for the grade point average
 *   - year: a 32-bit integer for the enrollment year
 *
 * Hint: Use int32_t from <stdint.h> for portable integer sizes
 */

/* GCC and Clang support __attribute__((packed)). For MSVC we also provide a
 * pragma-based fallback. The tests for this laboratory assume a packed record
 * size of 62 bytes (4 + 50 + 4 + 4) rather than the padded size typically
 * produced by default alignment rules.
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
 * UTILITY FUNCTIONS (PROVIDED)
 * =============================================================================
 */

/**
 * Initialise a Student structure safely
 * Ensures proper null-termination and clears padding bytes.
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

/**
 * Replace underscores with spaces in a string
 */
static void replace_underscores(char *str) {
    for (char *p = str; *p; p++) {
        if (*p == '_') *p = ' ';
    }
}

/* =============================================================================
 * FUNCTIONS TO IMPLEMENT
 * =============================================================================
 */

/**
 * TODO 2: Implement save_student()
 *
 * Appends a single student record to the binary file.
 *
 * @param filename  Path to the database file
 * @param student   Pointer to the student record to save
 * @return          0 on success, -1 on failure
 *
 * Steps:
 *   1. Open the file in append binary mode ("ab")
 *   2. Check if the file was opened successfully
 *   3. Write the student structure using fwrite()
 *   4. Verify that exactly 1 record was written
 *   5. Close the file
 *   6. Return 0 on success, -1 on any failure
 *
 * Hint: fwrite returns the number of elements successfully written
 */
int save_student(const char *filename, const Student *student) {
    if (filename == NULL || student == NULL) {
        return -1;
    }

    FILE *fp = fopen(filename, "ab");
    if (fp == NULL) {
        perror("Error opening file for writing");
        return -1;
    }

    size_t written = fwrite(student, sizeof(Student), 1, fp);
    fclose(fp);

    if (written != 1) {
        fprintf(stderr, "Error: failed to write student record\n");
        return -1;
    }

    return 0;
}

/**
 * TODO 3: Implement load_students()
 *
 * Reads all student records from the binary file into a dynamically
 * allocated array.
 *
 * @param filename  Path to the database file
 * @param students  Output: pointer to array of students (caller must free)
 * @param count     Output: number of students loaded
 * @return          0 on success, -1 on failure
 *
 * Steps:
 *   1. Open the file in read binary mode ("rb")
 *   2. If file doesn't exist, set *count = 0, *students = NULL, return 0
 *   3. Calculate file size using fseek(SEEK_END) and ftell()
 *   4. Calculate number of records: file_size / sizeof(Student)
 *   5. Allocate memory for all records using malloc()
 *   6. Seek back to beginning with fseek(SEEK_SET)
 *   7. Read all records using fread()
 *   8. Close the file
 *   9. Set output parameters and return 0
 *
 * Hint: Remember to handle the case where the file is empty
 */
int load_students(const char *filename, Student **students, size_t *count) {
    if (students == NULL || count == NULL) {
        return -1;
    }

    *students = NULL;
    *count = 0;

    if (filename == NULL) {
        return -1;
    }

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        /* Missing file means empty database rather than an error. */
        return 0;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return -1;
    }
    long file_size = ftell(fp);
    if (file_size < 0) {
        fclose(fp);
        return -1;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return -1;
    }

    if (file_size == 0) {
        fclose(fp);
        return 0;
    }

    size_t records = (size_t)file_size / sizeof(Student);
    if (records == 0) {
        fclose(fp);
        return 0;
    }

    Student *buffer = malloc(records * sizeof(Student));
    if (buffer == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        fclose(fp);
        return -1;
    }

    size_t read = fread(buffer, sizeof(Student), records, fp);
    fclose(fp);

    /* If the read is partial, we still return the successfully read prefix. */
    if (read == 0) {
        free(buffer);
        return 0;
    }

    *students = buffer;
    *count = read;
    return 0;
}

/**
 * TODO 4: Implement find_student_by_id()
 *
 * Searches for a student with the given ID using sequential search.
 *
 * @param students  Array of student records
 * @param count     Number of students in the array
 * @param id        Student ID to search for
 * @return          Pointer to the found student, or NULL if not found
 *
 * Steps:
 *   1. Iterate through all students in the array
 *   2. Compare each student's ID with the target ID
 *   3. Return pointer to the student if found
 *   4. Return NULL if not found after checking all records
 *
 * Hint: This is a simple linear search - O(n) complexity
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
 * TODO 5: Implement update_student()
 *
 * Updates a student record at a specific index in the file.
 *
 * @param filename  Path to the database file
 * @param index     Zero-based index of the record to update
 * @param student   Pointer to the new student data
 * @return          0 on success, -1 on failure
 *
 * Steps:
 *   1. Open the file in read+write binary mode ("r+b")
 *   2. Calculate the byte offset: index * sizeof(Student)
 *   3. Seek to the calculated position using fseek(SEEK_SET)
 *   4. Write the new student data using fwrite()
 *   5. Flush the buffer using fflush()
 *   6. Close the file
 *   7. Return 0 on success, -1 on any failure
 *
 * Hint: "r+b" opens for both reading and writing without truncating
 */
int update_student(const char *filename, size_t index, const Student *student) {
    if (filename == NULL || student == NULL) {
        return -1;
    }

    FILE *fp = fopen(filename, "r+b");
    if (fp == NULL) {
        perror("Error opening file for update");
        return -1;
    }

    long offset = (long)(index * sizeof(Student));
    if (fseek(fp, offset, SEEK_SET) != 0) {
        perror("Error seeking to record");
        fclose(fp);
        return -1;
    }

    size_t written = fwrite(student, sizeof(Student), 1, fp);
    fflush(fp);
    fclose(fp);

    if (written != 1) {
        fprintf(stderr, "Error: failed to write updated record\n");
        return -1;
    }

    return 0;
}

/**
 * TODO 6: Implement count_students()
 *
 * Counts the number of student records in the file without loading them.
 *
 * @param filename  Path to the database file
 * @return          Number of student records, or 0 if file doesn't exist
 *
 * Steps:
 *   1. Open the file in read binary mode
 *   2. If file doesn't exist, return 0
 *   3. Seek to end of file
 *   4. Get file size using ftell()
 *   5. Calculate record count: file_size / sizeof(Student)
 *   6. Close the file
 *   7. Return the count
 *
 * Hint: This should NOT load the file contents into memory
 */
size_t count_students(const char *filename) {
    if (filename == NULL) {
        return 0;
    }

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        return 0;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return 0;
    }
    long file_size = ftell(fp);
    fclose(fp);

    if (file_size <= 0) {
        return 0;
    }

    return (size_t)file_size / sizeof(Student);
}

/**
 * TODO 7: Implement print_student_table()
 *
 * Displays students in a formatted table with borders.
 *
 * @param students  Array of student records
 * @param count     Number of students to display
 *
 * Expected output format:
 * ┌────────┬──────────────────────┬───────┬──────┐
 * │   ID   │        Name          │  GPA  │ Year │
 * ├────────┼──────────────────────┼───────┼──────┤
 * │   1001 │ Alice Johnson        │  3.75 │ 2024 │
 * │   1002 │ Bob Smith            │  3.42 │ 2023 │
 * └────────┴──────────────────────┴───────┴──────┘
 *
 * Hint: Use %-20s for left-aligned strings, %6.2f for floats
 */
void print_student_table(const Student *students, size_t count) {
    if (count == 0) {
        printf("[INFO] No students to display\n");
        return;
    }

    /* The header line is printed verbatim to match the provided reference
     * output used by the automated tests.
     */
    printf("┌────────┬──────────────────────┬───────┬──────┐\n");
    printf("│   ID   │        Name          │  GPA  │ Year │\n");
    printf("├────────┼──────────────────────┼───────┼──────┤\n");

    for (size_t i = 0; i < count; i++) {
        printf("│ %6d │ %-20s │ %5.2f │ %4d │\n",
               students[i].id,
               students[i].name,
               students[i].gpa,
               students[i].year);
    }

    printf("└────────┴──────────────────────┴───────┴──────┘\n");
}

/**
 * TODO 8: Implement free_students()
 *
 * Frees the dynamically allocated students array.
 *
 * @param students  Pointer to the students array (may be NULL)
 *
 * Hint: Always check for NULL before calling free()
 */
void free_students(Student *students) {
    if (students != NULL) {
        free(students);
    }
}

/* =============================================================================
 * MAIN PROGRAM (PROVIDED - DO NOT MODIFY)
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

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Implement delete_student() that marks records as deleted without
 *    physically removing them (tombstone approach).
 *
 * 2. Implement compact_database() that removes all tombstone records
 *    and compacts the file.
 *
 * 3. Add a simple command-line interface with menu options for
 *    add, search, update, delete, list, and exit.
 *
 * 4. Implement binary search by keeping the file sorted by ID.
 *
 * 5. Add data validation (GPA must be 0.0-4.0, year must be reasonable).
 *
 * =============================================================================
 */

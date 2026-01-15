/**
 * =============================================================================
 * EXERCISE 1: Student Grade Processor
 * =============================================================================
 *
 * OBJECTIVE:
 *   Read student records from a text file, calculate statistics and generate
 *   a formatted report. This exercise practises file I/O, parsing with sscanf,
 *   and formatted output with fprintf.
 *
 * INPUT FILE FORMAT (data/studgrades.txt):
 *   Each line contains: ID Name Year Programme Grade1 Grade2 ... GradeN
 *   Example: 1122334 Popescu 2022 1050 5 6 7 8 9 10 7 6 6 5
 *
 * REQUIREMENTS:
 *   1. Read all student records from the input file
 *   2. Calculate each student's average grade
 *   3. Find the student with the highest average
 *   4. Count students per programme
 *   5. Calculate overall class statistics
 *   6. Write a formatted report to output file
 *
 * EXPECTED OUTPUT (to stdout):
 *   - Number of students successfully parsed
 *   - Name and average of top student
 *   - Confirmation that report was written
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
 * USAGE: ./exercise1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

#define MAX_LINE_LENGTH 256
#define MAX_NAME_LENGTH 50
#define MAX_STUDENTS 100
#define MAX_GRADES 20
#define INPUT_FILE "data/studgrades.txt"
#define OUTPUT_FILE "output/report.txt"

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

/**
 * TODO 1: Complete the Student structure
 *
 * The structure should contain:
 *   - id: integer for student ID
 *   - name: character array for student name (use MAX_NAME_LENGTH)
 *   - year: integer for enrolment year
 *   - programme: integer for programme code
 *   - grades: array of floats for grades (use MAX_GRADES)
 *   - grade_count: integer for number of grades
 *   - average: float for calculated average
 *
 * Some fields are provided; complete the rest.
 */
typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    /* TODO 1: Add the remaining fields below */
    int year;
    int programme;
    float grades[MAX_GRADES];
    int grade_count;
    float average;
} Student;


/* =============================================================================
 * FUNCTION DECLARATIONS
 * =============================================================================
 */

float calculate_average(const float grades[], int count);
int parse_student_line(const char *line, Student *s);
int read_students_from_file(const char *filename, Student students[], int max_students);
int find_top_student(const Student students[], int count);
void count_by_programme(const Student students[], int count, int *prog1050, int *prog1051);
int write_report(const char *filename, const Student students[], int count);

/* =============================================================================
 * FUNCTION IMPLEMENTATIONS
 * =============================================================================
 */

/**
 * TODO 2: Implement calculate_average
 *
 * Calculate the arithmetic mean of an array of grades.
 *
 * @param grades Array of grade values
 * @param count Number of grades in the array
 * @return The average grade, or 0.0 if count is 0
 *
 * Steps:
 *   1. Check if count is 0 (return 0.0 to avoid division by zero)
 *   2. Sum all grades using a loop
 *   3. Return sum divided by count
 */
float calculate_average(const float grades[], int count) {
    /* YOUR CODE HERE */
    return 0.0f;  /* Replace this */
}

/**
 * TODO 3: Implement parse_student_line
 *
 * Parse a single line of text into a Student structure.
 * Format: ID Name Year Programme Grade1 Grade2 ... GradeN
 *
 * @param line The input line to parse
 * @param s Pointer to Student structure to populate
 * @return 1 on success, 0 on parse error
 *
 * Steps:
 *   1. Use sscanf to parse the fixed fields (id, name, year, programme)
 *      Use format: "%d %49s %d %d%n" (the %n stores position)
 *   2. Check that sscanf returned 4 (all fixed fields matched)
 *   3. Parse remaining grades from the rest of the line
 *      Hint: Use a pointer to line + offset, then loop with sscanf
 *   4. Calculate and store the average
 *   5. Return 1 for success
 *
 * Hint: The %n specifier stores the number of characters consumed
 */
int parse_student_line(const char *line, Student *s) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}

/**
 * TODO 4: Implement read_students_from_file
 *
 * Read all student records from a text file.
 *
 * @param filename Path to the input file
 * @param students Array to store parsed students
 * @param max_students Maximum number of students to read
 * @return Number of students successfully read, or -1 on file error
 *
 * Steps:
 *   1. Open the file for reading ("r" mode)
 *   2. Check if fopen succeeded (return -1 if NULL)
 *   3. Read lines using fgets in a while loop
 *   4. For each line, call parse_student_line
 *   5. If parsing succeeds, increment the count
 *   6. Close the file
 *   7. Return the count
 *
 * Hint: Remember to check count < max_students in loop condition
 */
int read_students_from_file(const char *filename, Student students[], int max_students) {
    /* YOUR CODE HERE */
    return -1;  /* Replace this */
}

/**
 * TODO 5: Implement find_top_student
 *
 * Find the index of the student with the highest average.
 *
 * @param students Array of Student structures
 * @param count Number of students in the array
 * @return Index of the top student, or -1 if array is empty
 *
 * Steps:
 *   1. Check if count is 0 (return -1)
 *   2. Assume first student is top (index 0)
 *   3. Loop through remaining students
 *   4. If current student's average > top's average, update top index
 *   5. Return the index of the top student
 */
int find_top_student(const Student students[], int count) {
    /* YOUR CODE HERE */
    return -1;  /* Replace this */
}

/**
 * TODO 6: Implement count_by_programme
 *
 * Count the number of students in each programme.
 *
 * @param students Array of Student structures
 * @param count Number of students in the array
 * @param prog1050 Pointer to store count of programme 1050 students
 * @param prog1051 Pointer to store count of programme 1051 students
 *
 * Steps:
 *   1. Initialise both counts to 0
 *   2. Loop through all students
 *   3. Check programme field and increment appropriate counter
 */
void count_by_programme(const Student students[], int count, int *prog1050, int *prog1051) {
    /* YOUR CODE HERE */
}

/**
 * TODO 7: Implement write_report
 *
 * Write a formatted report to a file.
 *
 * @param filename Path to the output file
 * @param students Array of Student structures
 * @param count Number of students in the array
 * @return 0 on success, -1 on error
 *
 * Steps:
 *   1. Open the file for writing ("w" mode)
 *   2. Check if fopen succeeded
 *   3. Write a header section with title and date
 *   4. Write a table header (ID, Name, Year, Programme, Average)
 *   5. Write each student's data in a formatted row
 *   6. Write summary statistics:
 *      - Total number of students
 *      - Students per programme
 *      - Top performer's name and average
 *      - Overall class average
 *   7. Close the file
 *   8. Return 0 for success
 *
 * Hint: Use fprintf with width specifiers like %-10d, %-20s for alignment
 */
int write_report(const char *filename, const Student students[], int count) {
    /* YOUR CODE HERE */
    return -1;  /* Replace this */
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║          EXERCISE 1: Student Grade Processor                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Student students[MAX_STUDENTS];
    
    /**
     * TODO 8: Complete the main programme
     *
     * Steps:
     *   1. Call read_students_from_file to load student data
     *   2. Check if reading was successful (count > 0)
     *   3. Print the number of students loaded
     *   4. Find and print the top student's name and average
     *   5. Call write_report to generate the output file
     *   6. Print confirmation message
     *
     * Expected output format:
     *   "Loaded X students from file."
     *   "Top student: [Name] with average [X.XX]"
     *   "Report written to [filename]"
     */
    
    /* YOUR CODE HERE */
    
    printf("\nExercise completed.\n");
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Add error handling for malformed lines (print warnings with line numbers)
 *
 * 2. Implement sorting students by average (descending order)
 *    Hint: Use qsort with a custom comparison function
 *
 * 3. Add command-line arguments for input/output filenames
 *    Usage: ./exercise1 input.txt output.txt
 *
 * 4. Calculate additional statistics:
 *    - Standard deviation of averages
 *    - Minimum and maximum average
 *    - Median average
 *
 * 5. Add support for filtering by year or programme
 *
 * =============================================================================
 */

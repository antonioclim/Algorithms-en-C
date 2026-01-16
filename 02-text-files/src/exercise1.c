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
 * COMMAND-LINE INTERFACE
 * =============================================================================
 *
 * The laboratory specification uses a fixed input file and a fixed output
 * report path. For regression testing and for controlled experimentation, this
 * implementation also accepts optional arguments:
 *
 *   --input  <path>   Override the input file path
 *   --output <path>   Override the report output path
 *   --test            Suppress decorative banners and emit a minimal stdout
 *
 * Test mode is intentionally narrow: it is a formatting discipline, not a
 * behavioural change.
 */

static void print_usage(const char *argv0) {
    fprintf(stderr,
            "Usage: %s [--input <file>] [--output <file>] [--test]\n",
            argv0);
}

static int ensure_output_directory_exists(void) {
    /* Sufficiently portable for an educational setting. */
    #ifdef _WIN32
    return system("mkdir output 2>nul");
    #else
    return system("mkdir -p output 2>/dev/null");
    #endif
}


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
    if (count <= 0) {
        return 0.0f;
    }

    float sum = 0.0f;
    for (int i = 0; i < count; i++) {
        sum += grades[i];
    }

    return sum / (float)count;
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
    int offset = 0;

    /* Parse fixed fields: ID, Name, Year and Programme. */
    int matched = sscanf(line, "%d %49s %d %d%n",
                         &s->id, s->name, &s->year, &s->programme, &offset);
    if (matched != 4) {
        return 0;
    }

    /* Parse the variable-length grade sequence. */
    s->grade_count = 0;
    const char *ptr = line + offset;
    float grade = 0.0f;
    int consumed = 0;

    while (s->grade_count < MAX_GRADES && sscanf(ptr, "%f%n", &grade, &consumed) == 1) {
        s->grades[s->grade_count++] = grade;
        ptr += consumed;
    }

    s->average = calculate_average(s->grades, s->grade_count);
    return 1;
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
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening input file");
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    int count = 0;
    int line_num = 0;

    while (fgets(line, (int)sizeof(line), fp) != NULL && count < max_students) {
        line_num++;

        /* Skip empty lines. */
        if (line[0] == '\n' || line[0] == '\0') {
            continue;
        }

        if (parse_student_line(line, &students[count])) {
            count++;
        } else {
            fprintf(stderr, "Warning: Failed to parse line %d\n", line_num);
        }
    }

    fclose(fp);
    return count;
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
    if (count <= 0) {
        return -1;
    }

    int top_idx = 0;
    for (int i = 1; i < count; i++) {
        if (students[i].average > students[top_idx].average) {
            top_idx = i;
        }
    }

    return top_idx;
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
    if (prog1050 == NULL || prog1051 == NULL) {
        return;
    }

    *prog1050 = 0;
    *prog1051 = 0;

    for (int i = 0; i < count; i++) {
        if (students[i].programme == 1050) {
            (*prog1050)++;
        } else if (students[i].programme == 1051) {
            (*prog1051)++;
        }
    }
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
    ensure_output_directory_exists();

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error creating report file");
        return -1;
    }

    /* Header */
    fprintf(fp, "═══════════════════════════════════════════════════════════════\n");
    fprintf(fp, "                    STUDENT GRADE REPORT                        \n");
    fprintf(fp, "═══════════════════════════════════════════════════════════════\n\n");

    /* Table header */
    fprintf(fp, "%-10s %-20s %-6s %-10s %-8s %-8s\n",
            "ID", "Name", "Year", "Programme", "Grades", "Average");
    fprintf(fp, "%-10s %-20s %-6s %-10s %-8s %-8s\n",
            "----------", "--------------------", "------",
            "----------", "--------", "--------");

    float total_average = 0.0f;
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%-10d %-20s %-6d %-10d %-8d %-8.2f\n",
                students[i].id,
                students[i].name,
                students[i].year,
                students[i].programme,
                students[i].grade_count,
                students[i].average);
        total_average += students[i].average;
    }

    /* Summary section */
    fprintf(fp, "\n───────────────────────────────────────────────────────────────\n");
    fprintf(fp, "SUMMARY\n");
    fprintf(fp, "───────────────────────────────────────────────────────────────\n");

    fprintf(fp, "Total students:      %d\n", count);

    int prog1050 = 0;
    int prog1051 = 0;
    count_by_programme(students, count, &prog1050, &prog1051);
    fprintf(fp, "Programme 1050:      %d students\n", prog1050);
    fprintf(fp, "Programme 1051:      %d students\n", prog1051);

    int top_idx = find_top_student(students, count);
    if (top_idx >= 0) {
        fprintf(fp, "Top performer:       %s (%.2f)\n",
                students[top_idx].name, students[top_idx].average);
    }

    if (count > 0) {
        fprintf(fp, "Class average:       %.2f\n", total_average / (float)count);
    }

    fprintf(fp, "═══════════════════════════════════════════════════════════════\n");

    fclose(fp);
    return 0;
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(int argc, char **argv) {
    const char *input_path = INPUT_FILE;
    const char *output_path = OUTPUT_FILE;
    int test_mode = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            input_path = argv[++i];
        } else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            output_path = argv[++i];
        } else if (strcmp(argv[i], "--test") == 0) {
            test_mode = 1;
        } else {
            print_usage(argv[0]);
            return 2;
        }
    }

    if (!test_mode) {
        printf("╔═══════════════════════════════════════════════════════════════╗\n");
        printf("║          EXERCISE 1: Student Grade Processor                  ║\n");
        printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    }

    Student students[MAX_STUDENTS];

    int count = read_students_from_file(input_path, students, MAX_STUDENTS);
    if (count < 0) {
        fprintf(stderr, "Failed to read student data.\n");
        return 1;
    }
    if (count == 0) {
        fprintf(stderr, "No students found in file.\n");
        return 1;
    }

    printf("Loaded %d students from file.\n", count);

    int top_idx = find_top_student(students, count);
    if (top_idx >= 0) {
        printf("Top student: %s with average %.2f\n",
               students[top_idx].name, students[top_idx].average);
    }

    if (write_report(output_path, students, count) == 0) {
        printf("Report written to %s\n", output_path);
    } else {
        fprintf(stderr, "Failed to write report.\n");
        return 1;
    }

    if (!test_mode) {
        printf("\nExercise completed successfully.\n");
    }

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

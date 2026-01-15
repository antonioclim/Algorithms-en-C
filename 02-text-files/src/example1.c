/**
 * =============================================================================
 * WEEK 02: TEXT FILE PROCESSING IN C
 * Complete Working Example
 * =============================================================================
 *
 * This example demonstrates:
 *   1. Opening and closing files safely
 *   2. Reading files line-by-line with fgets()
 *   3. Parsing structured data with sscanf()
 *   4. Writing formatted output with fprintf()
 *   5. Buffering control with setvbuf() and fflush()
 *   6. Error handling with perror() and errno
 *   7. CSV parsing with strtok()
 *   8. Character-by-character processing with fgetc()
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Usage: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* =============================================================================
 * CONSTANTS AND CONFIGURATION
 * =============================================================================
 */

#define MAX_LINE_LENGTH 256
#define MAX_NAME_LENGTH 50
#define MAX_STUDENTS 100
#define MAX_GRADES 20

/* ANSI colour codes for terminal output */
#define COLOUR_RESET   "\033[0m"
#define COLOUR_RED     "\033[31m"
#define COLOUR_GREEN   "\033[32m"
#define COLOUR_YELLOW  "\033[33m"
#define COLOUR_BLUE    "\033[34m"
#define COLOUR_CYAN    "\033[36m"

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

/**
 * Student record structure
 * Stores a student's identification and academic performance data.
 */
typedef struct {
    int id;                         /* Unique student identifier */
    char name[MAX_NAME_LENGTH];     /* Student name */
    int year;                       /* Year of enrolment */
    int programme;                  /* Programme code */
    float grades[MAX_GRADES];       /* Array of grades */
    int grade_count;                /* Number of grades recorded */
    float average;                  /* Calculated average grade */
} Student;

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Print a decorative section header
 *
 * @param title The title to display in the header
 */
void print_section_header(const char *title) {
    printf("\n");
    printf(COLOUR_CYAN "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  %-61s ║\n", title);
    printf("╚═══════════════════════════════════════════════════════════════╝" COLOUR_RESET "\n\n");
}

/**
 * Print a subsection header
 *
 * @param title The title to display
 */
void print_subsection(const char *title) {
    printf(COLOUR_YELLOW "\n--- %s ---\n\n" COLOUR_RESET, title);
}

/**
 * Remove trailing newline from a string
 *
 * @param str The string to modify
 */
void remove_newline(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

/* =============================================================================
 * PART 1: BASIC FILE OPERATIONS
 * =============================================================================
 */

/**
 * Demonstrate basic file opening, reading and closing.
 * Shows proper error handling patterns.
 */
void demo_basic_file_operations(void) {
    print_section_header("PART 1: Basic File Operations");
    
    const char *test_filename = "test_basic.txt";
    
    /* --- Writing to a file --- */
    print_subsection("1.1 Writing to a file");
    
    FILE *fp_write = fopen(test_filename, "w");
    if (fp_write == NULL) {
        perror(COLOUR_RED "Error creating file" COLOUR_RESET);
        return;
    }
    
    printf("Created file: %s\n", test_filename);
    
    /* Write some test data */
    fprintf(fp_write, "This is the first line.\n");
    fprintf(fp_write, "Second line with a number: %d\n", 42);
    fprintf(fp_write, "Third line with a float: %.2f\n", 3.14159);
    fprintf(fp_write, "Final line.\n");
    
    printf("Wrote 4 lines to file.\n");
    
    /* Close the file - this flushes any buffered data */
    if (fclose(fp_write) == EOF) {
        perror(COLOUR_RED "Error closing file" COLOUR_RESET);
        return;
    }
    printf("File closed successfully.\n");
    
    /* --- Reading from the file --- */
    print_subsection("1.2 Reading from a file");
    
    FILE *fp_read = fopen(test_filename, "r");
    if (fp_read == NULL) {
        perror(COLOUR_RED "Error opening file for reading" COLOUR_RESET);
        return;
    }
    
    printf("Opened file for reading.\n");
    printf("Contents:\n" COLOUR_GREEN);
    
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    
    /* Read file line by line using fgets */
    while (fgets(line, sizeof(line), fp_read) != NULL) {
        line_number++;
        printf("  [%d] %s", line_number, line);
    }
    printf(COLOUR_RESET);
    
    /* Check how the loop ended */
    if (ferror(fp_read)) {
        perror(COLOUR_RED "\nRead error occurred" COLOUR_RESET);
    } else if (feof(fp_read)) {
        printf("\nReached end of file after %d lines.\n", line_number);
    }
    
    fclose(fp_read);
    
    /* Clean up test file */
    remove(test_filename);
    printf("Cleaned up test file.\n");
}

/* =============================================================================
 * PART 2: PARSING STRUCTURED DATA
 * =============================================================================
 */

/**
 * Calculate the average of a student's grades
 *
 * @param s Pointer to Student structure
 * @return The calculated average, or 0 if no grades
 */
float calculate_average(const Student *s) {
    if (s->grade_count == 0) return 0.0f;
    
    float sum = 0.0f;
    for (int i = 0; i < s->grade_count; i++) {
        sum += s->grades[i];
    }
    return sum / s->grade_count;
}

/**
 * Parse a student record from a line of text.
 * Format: ID Name Year Programme Grade1 Grade2 ... GradeN
 *
 * @param line The input line to parse
 * @param s Pointer to Student structure to populate
 * @return 1 on success, 0 on parse error
 */
int parse_student_line(const char *line, Student *s) {
    /* First, parse the fixed fields */
    int offset;
    int matched = sscanf(line, "%d %49s %d %d%n",
                         &s->id, s->name, &s->year, &s->programme, &offset);
    
    if (matched != 4) {
        return 0;  /* Parse failed */
    }
    
    /* Now parse variable number of grades from remaining string */
    s->grade_count = 0;
    const char *ptr = line + offset;
    float grade;
    
    while (s->grade_count < MAX_GRADES && 
           sscanf(ptr, "%f%n", &grade, &offset) == 1) {
        s->grades[s->grade_count++] = grade;
        ptr += offset;
    }
    
    /* Calculate average */
    s->average = calculate_average(s);
    
    return 1;
}

/**
 * Demonstrate parsing structured data from a file.
 */
void demo_parsing_structured_data(void) {
    print_section_header("PART 2: Parsing Structured Data");
    
    const char *filename = "students_demo.txt";
    
    /* Create sample data file */
    print_subsection("2.1 Creating sample data file");
    
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Cannot create sample file");
        return;
    }
    
    fprintf(fp, "1122334 Popescu 2022 1050 5 6 7 8 9 10 7 6 6 5\n");
    fprintf(fp, "222222 Ionescu 2022 1051 9 10 10 10 10 9 6 10 10 9\n");
    fprintf(fp, "333333 Vasilescu 2023 1050 4 5 6 7 8 9 10 9 8 7\n");
    fprintf(fp, "444444 Georgescu 2022 1051 9 8 9 7 9 8 6 9 7 8\n");
    fclose(fp);
    
    printf("Created file with 4 student records.\n");
    
    /* Read and parse the file */
    print_subsection("2.2 Parsing student records");
    
    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Cannot open file for reading");
        return;
    }
    
    Student students[MAX_STUDENTS];
    int count = 0;
    char line[MAX_LINE_LENGTH];
    int line_num = 0;
    
    while (fgets(line, sizeof(line), fp) != NULL && count < MAX_STUDENTS) {
        line_num++;
        
        if (parse_student_line(line, &students[count])) {
            printf(COLOUR_GREEN "✓" COLOUR_RESET " Line %d: Parsed student %s (ID: %d)\n",
                   line_num, students[count].name, students[count].id);
            count++;
        } else {
            printf(COLOUR_RED "✗" COLOUR_RESET " Line %d: Parse error\n", line_num);
        }
    }
    
    fclose(fp);
    
    /* Display parsed data */
    print_subsection("2.3 Parsed student data");
    
    printf("%-10s %-15s %-6s %-10s %-8s %-8s\n",
           "ID", "Name", "Year", "Programme", "Grades", "Average");
    printf("%-10s %-15s %-6s %-10s %-8s %-8s\n",
           "----------", "---------------", "------", "----------", "--------", "--------");
    
    for (int i = 0; i < count; i++) {
        printf("%-10d %-15s %-6d %-10d %-8d %-8.2f\n",
               students[i].id,
               students[i].name,
               students[i].year,
               students[i].programme,
               students[i].grade_count,
               students[i].average);
    }
    
    /* Find top student */
    print_subsection("2.4 Statistics");
    
    int top_idx = 0;
    for (int i = 1; i < count; i++) {
        if (students[i].average > students[top_idx].average) {
            top_idx = i;
        }
    }
    
    printf("Top performer: " COLOUR_GREEN "%s" COLOUR_RESET " with average " 
           COLOUR_GREEN "%.2f" COLOUR_RESET "\n",
           students[top_idx].name, students[top_idx].average);
    
    /* Clean up */
    remove(filename);
}

/* =============================================================================
 * PART 3: CSV PARSING WITH STRTOK
 * =============================================================================
 */

/**
 * Demonstrate CSV parsing using strtok().
 * Note: strtok modifies the original string and is not thread-safe.
 */
void demo_csv_parsing(void) {
    print_section_header("PART 3: CSV Parsing with strtok()");
    
    const char *filename = "products_demo.csv";
    
    /* Create sample CSV file */
    print_subsection("3.1 Creating sample CSV file");
    
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Cannot create CSV file");
        return;
    }
    
    fprintf(fp, "ID,Name,Category,Price,Stock\n");
    fprintf(fp, "001,Laptop,Electronics,2499.99,15\n");
    fprintf(fp, "002,Chair,Furniture,149.50,42\n");
    fprintf(fp, "003,Coffee,Beverages,12.99,100\n");
    fprintf(fp, "004,Monitor,Electronics,399.00,28\n");
    fclose(fp);
    
    printf("Created CSV file with header and 4 product records.\n");
    
    /* Parse CSV file */
    print_subsection("3.2 Parsing CSV data");
    
    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Cannot open CSV file");
        return;
    }
    
    char line[MAX_LINE_LENGTH];
    int line_num = 0;
    int is_header = 1;
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        line_num++;
        remove_newline(line);
        
        /* Skip empty lines */
        if (strlen(line) == 0) continue;
        
        /* Make a copy since strtok modifies the string */
        char line_copy[MAX_LINE_LENGTH];
        strncpy(line_copy, line, sizeof(line_copy) - 1);
        line_copy[sizeof(line_copy) - 1] = '\0';
        
        /* Parse fields */
        char *fields[10];
        int field_count = 0;
        
        char *token = strtok(line_copy, ",");
        while (token != NULL && field_count < 10) {
            fields[field_count++] = token;
            token = strtok(NULL, ",");
        }
        
        if (is_header) {
            printf(COLOUR_CYAN "Header: ");
            for (int i = 0; i < field_count; i++) {
                printf("[%s] ", fields[i]);
            }
            printf(COLOUR_RESET "\n\n");
            is_header = 0;
        } else {
            printf("Row %d: ", line_num);
            for (int i = 0; i < field_count; i++) {
                printf("%s", fields[i]);
                if (i < field_count - 1) printf(" | ");
            }
            printf("\n");
        }
    }
    
    fclose(fp);
    
    /* Demonstrate strtok warning */
    print_subsection("3.3 Important: strtok() modifies the string!");
    
    char original[] = "one,two,three";
    printf("Before strtok: \"%s\"\n", original);
    
    char *tok = strtok(original, ",");
    printf("After first strtok(): \"%s\"\n", original);
    printf("  Notice: The comma is replaced with '\\0'!\n");
    printf("  Original string is now modified.\n");
    
    printf("\n" COLOUR_YELLOW "Tip:" COLOUR_RESET " For thread-safe tokenising, use strtok_r() on POSIX systems.\n");
    
    /* Clean up */
    remove(filename);
}

/* =============================================================================
 * PART 4: CHARACTER-BY-CHARACTER PROCESSING
 * =============================================================================
 */

/**
 * Count words, lines and characters in a file.
 * Demonstrates fgetc() usage.
 *
 * @param filename The file to analyse
 * @param words Pointer to store word count
 * @param lines Pointer to store line count
 * @param chars Pointer to store character count
 * @return 0 on success, -1 on error
 */
int count_file_stats(const char *filename, int *words, int *lines, int *chars) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return -1;
    }
    
    *words = 0;
    *lines = 0;
    *chars = 0;
    
    int ch;
    int in_word = 0;
    
    while ((ch = fgetc(fp)) != EOF) {
        (*chars)++;
        
        if (ch == '\n') {
            (*lines)++;
        }
        
        if (isspace(ch)) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            (*words)++;
        }
    }
    
    /* Count last line if file doesn't end with newline */
    if (*chars > 0 && ch != '\n') {
        (*lines)++;
    }
    
    fclose(fp);
    return 0;
}

/**
 * Demonstrate character-by-character file processing.
 */
void demo_character_processing(void) {
    print_section_header("PART 4: Character-by-Character Processing");
    
    const char *filename = "sample_text.txt";
    
    /* Create sample file */
    print_subsection("4.1 Creating sample text file");
    
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Cannot create file");
        return;
    }
    
    fprintf(fp, "The quick brown fox jumps over the lazy dog.\n");
    fprintf(fp, "Pack my box with five dozen liquor jugs.\n");
    fprintf(fp, "How vexingly quick daft zebras jump!\n");
    fclose(fp);
    
    printf("Created sample text file.\n");
    
    /* Count statistics */
    print_subsection("4.2 File statistics (wc clone)");
    
    int words, lines, chars;
    if (count_file_stats(filename, &words, &lines, &chars) == 0) {
        printf("  Lines:      %d\n", lines);
        printf("  Words:      %d\n", words);
        printf("  Characters: %d\n", chars);
    }
    
    /* Convert to uppercase */
    print_subsection("4.3 Character conversion (to uppercase)");
    
    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Cannot open file");
        return;
    }
    
    printf("Uppercase conversion:\n" COLOUR_GREEN);
    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        putchar(toupper(ch));
    }
    printf(COLOUR_RESET);
    
    fclose(fp);
    
    /* Clean up */
    remove(filename);
}

/* =============================================================================
 * PART 5: BUFFERING CONTROL
 * =============================================================================
 */

/**
 * Demonstrate buffering modes and their effects.
 */
void demo_buffering(void) {
    print_section_header("PART 5: Buffering Control");
    
    print_subsection("5.1 Buffering modes explanation");
    
    printf("Three buffering modes available:\n\n");
    printf("  " COLOUR_GREEN "_IOFBF" COLOUR_RESET " (Full buffering)\n");
    printf("    - Data written when buffer fills or fclose()/fflush() called\n");
    printf("    - Most efficient for file I/O\n");
    printf("    - Default for regular files\n\n");
    
    printf("  " COLOUR_YELLOW "_IOLBF" COLOUR_RESET " (Line buffering)\n");
    printf("    - Data written when newline encountered\n");
    printf("    - Good for interactive output and log files\n");
    printf("    - Default for stdout when connected to terminal\n\n");
    
    printf("  " COLOUR_RED "_IONBF" COLOUR_RESET " (No buffering)\n");
    printf("    - Data written immediately\n");
    printf("    - Slowest but most responsive\n");
    printf("    - Default for stderr\n\n");
    
    /* Demonstrate fflush usage */
    print_subsection("5.2 Using fflush()");
    
    printf("Common fflush() use case - ensuring prompt appears:\n\n");
    printf("  " COLOUR_CYAN "printf(\"Enter name: \");" COLOUR_RESET "\n");
    printf("  " COLOUR_CYAN "fflush(stdout);" COLOUR_RESET "  /* Ensure prompt is visible */\n");
    printf("  " COLOUR_CYAN "fgets(name, sizeof(name), stdin);" COLOUR_RESET "\n\n");
    
    printf(COLOUR_YELLOW "Warning:" COLOUR_RESET " fflush(stdin) is undefined behaviour in standard C!\n");
    printf("  It may work on some systems but is not portable.\n");
    
    /* Show setvbuf usage */
    print_subsection("5.3 Setting custom buffer with setvbuf()");
    
    printf("Example: Setting line buffering for a log file\n\n");
    printf("  " COLOUR_CYAN "FILE *log = fopen(\"app.log\", \"w\");" COLOUR_RESET "\n");
    printf("  " COLOUR_CYAN "setvbuf(log, NULL, _IOLBF, 0);" COLOUR_RESET "  /* Line buffered */\n\n");
    
    printf("Example: Setting custom buffer size\n\n");
    printf("  " COLOUR_CYAN "char buffer[65536];" COLOUR_RESET "  /* 64KB buffer */\n");
    printf("  " COLOUR_CYAN "setvbuf(fp, buffer, _IOFBF, sizeof(buffer));" COLOUR_RESET "\n");
}

/* =============================================================================
 * PART 6: ERROR HANDLING
 * =============================================================================
 */

/**
 * Demonstrate comprehensive error handling for file operations.
 */
void demo_error_handling(void) {
    print_section_header("PART 6: Error Handling");
    
    print_subsection("6.1 Handling fopen() errors");
    
    /* Try to open a non-existent file */
    FILE *fp = fopen("nonexistent_file_xyz123.txt", "r");
    if (fp == NULL) {
        printf("Attempted to open non-existent file:\n");
        printf("  errno = %d\n", errno);
        printf("  perror output: ");
        perror("fopen");
        
        /* Show specific error handling */
        switch (errno) {
            case ENOENT:
                printf("  → File not found (ENOENT)\n");
                break;
            case EACCES:
                printf("  → Permission denied (EACCES)\n");
                break;
            default:
                printf("  → Other error\n");
        }
    }
    
    /* Try to create file in non-existent directory */
    print_subsection("6.2 Handling directory errors");
    
    fp = fopen("/nonexistent_dir/file.txt", "w");
    if (fp == NULL) {
        printf("Attempted to create file in non-existent directory:\n");
        printf("  perror output: ");
        perror("fopen");
    }
    
    /* Demonstrate proper cleanup pattern */
    print_subsection("6.3 Defensive programming pattern");
    
    printf("Recommended error handling pattern:\n\n");
    printf(COLOUR_CYAN
"int process_file(const char *filename) {\n"
"    int result = -1;  /* Assume failure */\n"
"    FILE *fp = NULL;\n"
"\n"
"    fp = fopen(filename, \"r\");\n"
"    if (fp == NULL) {\n"
"        perror(\"Cannot open file\");\n"
"        goto cleanup;\n"
"    }\n"
"\n"
"    /* Process file... */\n"
"    result = 0;  /* Success */\n"
"\n"
"cleanup:\n"
"    if (fp) fclose(fp);\n"
"    return result;\n"
"}\n" COLOUR_RESET);
    
    printf("\nThis pattern ensures resources are always cleaned up,\n");
    printf("regardless of where errors occur.\n");
}

/* =============================================================================
 * PART 7: WRITING REPORTS
 * =============================================================================
 */

/**
 * Generate a formatted report to a file.
 */
void demo_report_generation(void) {
    print_section_header("PART 7: Generating Formatted Reports");
    
    const char *report_file = "student_report.txt";
    
    print_subsection("7.1 Creating formatted report");
    
    /* Sample student data */
    Student students[] = {
        {1001, "Popescu Ana", 2022, 1050, {8, 9, 7, 10, 9}, 5, 8.6f},
        {1002, "Ionescu Mihai", 2022, 1050, {10, 10, 9, 10, 10}, 5, 9.8f},
        {1003, "Vasilescu Ion", 2023, 1051, {6, 7, 6, 8, 7}, 5, 6.8f},
        {1004, "Georgescu Elena", 2022, 1051, {9, 8, 9, 9, 8}, 5, 8.6f}
    };
    int count = 4;
    
    FILE *fp = fopen(report_file, "w");
    if (fp == NULL) {
        perror("Cannot create report file");
        return;
    }
    
    /* Write report header */
    fprintf(fp, "═══════════════════════════════════════════════════════════════\n");
    fprintf(fp, "                    STUDENT GRADE REPORT                        \n");
    fprintf(fp, "                    Academic Year 2022-2023                     \n");
    fprintf(fp, "═══════════════════════════════════════════════════════════════\n\n");
    
    /* Write table header */
    fprintf(fp, "%-8s %-20s %-6s %-10s %-8s\n",
            "ID", "Name", "Year", "Programme", "Average");
    fprintf(fp, "%-8s %-20s %-6s %-10s %-8s\n",
            "--------", "--------------------", "------", "----------", "--------");
    
    /* Write student data */
    float total_avg = 0;
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%-8d %-20s %-6d %-10d %-8.2f\n",
                students[i].id,
                students[i].name,
                students[i].year,
                students[i].programme,
                students[i].average);
        total_avg += students[i].average;
    }
    
    /* Write summary */
    fprintf(fp, "\n───────────────────────────────────────────────────────────────\n");
    fprintf(fp, "SUMMARY\n");
    fprintf(fp, "───────────────────────────────────────────────────────────────\n");
    fprintf(fp, "Total students:   %d\n", count);
    fprintf(fp, "Overall average:  %.2f\n", total_avg / count);
    fprintf(fp, "═══════════════════════════════════════════════════════════════\n");
    
    fclose(fp);
    
    printf("Report written to: %s\n\n", report_file);
    
    /* Display the generated report */
    print_subsection("7.2 Generated report contents");
    
    fp = fopen(report_file, "r");
    if (fp != NULL) {
        char line[MAX_LINE_LENGTH];
        printf(COLOUR_GREEN);
        while (fgets(line, sizeof(line), fp) != NULL) {
            printf("%s", line);
        }
        printf(COLOUR_RESET);
        fclose(fp);
    }
    
    /* Clean up */
    remove(report_file);
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf(COLOUR_CYAN "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║      WEEK 02: TEXT FILE PROCESSING - Complete Example         ║\n");
    printf("║                                                               ║\n");
    printf("║      Algorithms and Programming Techniques (ATP)              ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝" COLOUR_RESET "\n");

    demo_basic_file_operations();
    demo_parsing_structured_data();
    demo_csv_parsing();
    demo_character_processing();
    demo_buffering();
    demo_error_handling();
    demo_report_generation();
    
    printf("\n");
    printf(COLOUR_GREEN "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    Example completed successfully!            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝" COLOUR_RESET "\n\n");
    
    return 0;
}

/* =============================================================================
 * END OF FILE
 * =============================================================================
 */

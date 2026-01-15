/**
 * =============================================================================
 * EXERCISE 1 SOLUTION: Student Grade Processor
 * =============================================================================
 *
 * INSTRUCTOR NOTES:
 *   This is the reference solution for Exercise 1. Students should arrive at
 *   a similar implementation by completing the TODO markers in exercise1.c.
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
 * USAGE: ./exercise1_sol
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
 * Student record structure
 */
typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    int year;
    int programme;
    float grades[MAX_GRADES];
    int grade_count;
    float average;
} Student;

/* =============================================================================
 * FUNCTION IMPLEMENTATIONS
 * =============================================================================
 */

/**
 * Calculate the average of an array of grades.
 */
float calculate_average(const float grades[], int count) {
    if (count == 0) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (int i = 0; i < count; i++) {
        sum += grades[i];
    }
    
    return sum / count;
}

/**
 * Parse a student record from a line of text.
 * Returns 1 on success, 0 on failure.
 */
int parse_student_line(const char *line, Student *s) {
    int offset;
    
    /* Parse fixed fields: ID, Name, Year, Programme */
    int matched = sscanf(line, "%d %49s %d %d%n",
                         &s->id, s->name, &s->year, &s->programme, &offset);
    
    if (matched != 4) {
        return 0;  /* Failed to parse required fields */
    }
    
    /* Parse variable number of grades */
    s->grade_count = 0;
    const char *ptr = line + offset;
    float grade;
    
    while (s->grade_count < MAX_GRADES && 
           sscanf(ptr, "%f%n", &grade, &offset) == 1) {
        s->grades[s->grade_count++] = grade;
        ptr += offset;
    }
    
    /* Calculate average */
    s->average = calculate_average(s->grades, s->grade_count);
    
    return 1;
}

/**
 * Read students from file.
 * Returns count of students read, or -1 on file error.
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
    
    while (fgets(line, sizeof(line), fp) != NULL && count < max_students) {
        line_num++;
        
        /* Skip empty lines */
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
 * Find the index of the student with the highest average.
 */
int find_top_student(const Student students[], int count) {
    if (count == 0) {
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
 * Count students by programme.
 */
void count_by_programme(const Student students[], int count, int *prog1050, int *prog1051) {
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
 * Write formatted report to file.
 */
int write_report(const char *filename, const Student students[], int count) {
    /* Create output directory if needed */
    #ifdef _WIN32
    system("mkdir output 2>nul");
    #else
    system("mkdir -p output 2>/dev/null");
    #endif
    
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
    
    /* Student data */
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
    
    int prog1050, prog1051;
    count_by_programme(students, count, &prog1050, &prog1051);
    fprintf(fp, "Programme 1050:      %d students\n", prog1050);
    fprintf(fp, "Programme 1051:      %d students\n", prog1051);
    
    int top_idx = find_top_student(students, count);
    if (top_idx >= 0) {
        fprintf(fp, "Top performer:       %s (%.2f)\n",
                students[top_idx].name, students[top_idx].average);
    }
    
    if (count > 0) {
        fprintf(fp, "Class average:       %.2f\n", total_average / count);
    }
    
    fprintf(fp, "═══════════════════════════════════════════════════════════════\n");
    
    fclose(fp);
    return 0;
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
    
    /* Read students from file */
    int count = read_students_from_file(INPUT_FILE, students, MAX_STUDENTS);
    
    if (count < 0) {
        fprintf(stderr, "Failed to read student data.\n");
        return 1;
    }
    
    if (count == 0) {
        fprintf(stderr, "No students found in file.\n");
        return 1;
    }
    
    printf("Loaded %d students from file.\n", count);
    
    /* Find and display top student */
    int top_idx = find_top_student(students, count);
    if (top_idx >= 0) {
        printf("Top student: %s with average %.2f\n",
               students[top_idx].name, students[top_idx].average);
    }
    
    /* Generate report */
    if (write_report(OUTPUT_FILE, students, count) == 0) {
        printf("Report written to %s\n", OUTPUT_FILE);
    } else {
        fprintf(stderr, "Failed to write report.\n");
        return 1;
    }
    
    printf("\nExercise completed successfully.\n");
    return 0;
}

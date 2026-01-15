/**
 * =============================================================================
 * EXERCISE 1 SOLUTION: Student Records Management System
 * =============================================================================
 *
 * INSTRUCTOR COPY - Complete working solution
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NAME_LENGTH 50
#define MAX_LINE_LENGTH 256

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct StudentNode {
    int id;
    char name[MAX_NAME_LENGTH];
    float gpa;
    struct StudentNode *next;
} StudentNode;

/* =============================================================================
 * FUNCTION IMPLEMENTATIONS
 * =============================================================================
 */

/**
 * Creates a new student node with the given information.
 */
StudentNode* create_student(int id, const char *name, float gpa) {
    StudentNode *new_node = (StudentNode*)malloc(sizeof(StudentNode));
    
    if (new_node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    
    new_node->id = id;
    new_node->gpa = gpa;
    
    /* Safe string copy with null termination */
    strncpy(new_node->name, name, MAX_NAME_LENGTH - 1);
    new_node->name[MAX_NAME_LENGTH - 1] = '\0';
    
    new_node->next = NULL;
    
    return new_node;
}

/**
 * Inserts a new student in ascending order by ID.
 */
bool insert_sorted(StudentNode **head_ref, int id, const char *name, float gpa) {
    StudentNode *new_node = create_student(id, name, gpa);
    if (new_node == NULL) {
        return false;
    }
    
    /* Case 1: Empty list or insert at beginning */
    if (*head_ref == NULL || (*head_ref)->id > id) {
        new_node->next = *head_ref;
        *head_ref = new_node;
        return true;
    }
    
    /* Case 2: Check for duplicate at head */
    if ((*head_ref)->id == id) {
        free(new_node);
        return false;
    }
    
    /* Case 3: Find correct position */
    StudentNode *current = *head_ref;
    while (current->next != NULL && current->next->id < id) {
        current = current->next;
    }
    
    /* Check for duplicate */
    if (current->next != NULL && current->next->id == id) {
        free(new_node);
        return false;
    }
    
    /* Insert after current */
    new_node->next = current->next;
    current->next = new_node;
    
    return true;
}

/**
 * Searches for a student by ID.
 */
StudentNode* find_student(StudentNode *head, int id) {
    StudentNode *current = head;
    
    while (current != NULL) {
        if (current->id == id) {
            return current;
        }
        /* Optimisation: stop early since list is sorted */
        if (current->id > id) {
            return NULL;
        }
        current = current->next;
    }
    
    return NULL;
}

/**
 * Deletes a student by ID.
 */
bool delete_student(StudentNode **head_ref, int id) {
    if (*head_ref == NULL) {
        return false;
    }
    
    /* Case 1: Target is at head */
    if ((*head_ref)->id == id) {
        StudentNode *temp = *head_ref;
        *head_ref = (*head_ref)->next;
        free(temp);
        return true;
    }
    
    /* Case 2: Find node before target */
    StudentNode *current = *head_ref;
    while (current->next != NULL && current->next->id < id) {
        current = current->next;
    }
    
    /* Check if found */
    if (current->next != NULL && current->next->id == id) {
        StudentNode *temp = current->next;
        current->next = temp->next;
        free(temp);
        return true;
    }
    
    return false;
}

/**
 * Displays all students in a formatted table.
 */
void display_all(const StudentNode *head) {
    printf("+------+------------------+------+\n");
    printf("|  ID  |       Name       | GPA  |\n");
    printf("+------+------------------+------+\n");
    
    if (head == NULL) {
        printf("|         (empty list)         |\n");
    } else {
        const StudentNode *current = head;
        while (current != NULL) {
            printf("| %4d | %-16s | %.2f |\n", 
                   current->id, current->name, current->gpa);
            current = current->next;
        }
    }
    
    printf("+------+------------------+------+\n");
}

/**
 * Counts the number of students.
 */
int count_students(const StudentNode *head) {
    int count = 0;
    const StudentNode *current = head;
    
    while (current != NULL) {
        count++;
        current = current->next;
    }
    
    return count;
}

/**
 * Frees all nodes.
 */
void free_all_students(StudentNode **head_ref) {
    StudentNode *current = *head_ref;
    StudentNode *next;
    
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    
    *head_ref = NULL;
}

/**
 * Prints a single student.
 */
void print_student(const StudentNode *student) {
    if (student == NULL) {
        printf("Student not found\n");
        return;
    }
    printf("ID: %d | Name: %s | GPA: %.2f\n", 
           student->id, student->name, student->gpa);
}

/**
 * Processes commands from a file.
 */
void process_commands(StudentNode **head_ref, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return;
    }
    
    char line[MAX_LINE_LENGTH];
    char command[20];
    int id;
    char name[MAX_NAME_LENGTH];
    float gpa;
    
    printf("\n=== Processing commands from '%s' ===\n\n", filename);
    
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }
        
        if (sscanf(line, "%s", command) != 1) {
            continue;
        }
        
        if (strcmp(command, "ADD") == 0) {
            if (sscanf(line, "%*s %d %s %f", &id, name, &gpa) == 3) {
                if (insert_sorted(head_ref, id, name, gpa)) {
                    printf("Added: %s (ID: %d, GPA: %.2f)\n", name, id, gpa);
                } else {
                    printf("Failed to add: ID %d already exists\n", id);
                }
            }
        } else if (strcmp(command, "FIND") == 0) {
            if (sscanf(line, "%*s %d", &id) == 1) {
                StudentNode *found = find_student(*head_ref, id);
                if (found) {
                    printf("Found: %s (ID: %d, GPA: %.2f)\n", 
                           found->name, found->id, found->gpa);
                } else {
                    printf("Not found: ID %d\n", id);
                }
            }
        } else if (strcmp(command, "DELETE") == 0) {
            if (sscanf(line, "%*s %d", &id) == 1) {
                StudentNode *student = find_student(*head_ref, id);
                if (student) {
                    char deleted_name[MAX_NAME_LENGTH];
                    strcpy(deleted_name, student->name);
                    if (delete_student(head_ref, id)) {
                        printf("Deleted: %s (ID: %d)\n", deleted_name, id);
                    }
                } else {
                    printf("Cannot delete: ID %d not found\n", id);
                }
            }
        } else if (strcmp(command, "DISPLAY") == 0) {
            display_all(*head_ref);
        } else if (strcmp(command, "COUNT") == 0) {
            printf("Total students: %d\n", count_students(*head_ref));
        }
    }
    
    fclose(file);
    printf("\n=== Command processing complete ===\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     Student Records Management System - SOLUTION              ║\n");
    printf("║     Exercise 1 - Linked Lists                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    StudentNode *head = NULL;
    
    if (argc > 1) {
        process_commands(&head, argv[1]);
    } else {
        printf("\n--- Demo Mode ---\n\n");
        
        printf("Adding students...\n");
        insert_sorted(&head, 1003, "Charlie", 3.50);
        insert_sorted(&head, 1001, "Alice", 3.75);
        insert_sorted(&head, 1004, "Diana", 3.90);
        insert_sorted(&head, 1002, "Bob", 3.85);
        
        printf("\nAll students (sorted by ID):\n");
        display_all(head);
        
        printf("\nSearching for ID 1002:\n");
        StudentNode *found = find_student(head, 1002);
        print_student(found);
        
        printf("\nDeleting ID 1001...\n");
        if (delete_student(&head, 1001)) {
            printf("Deleted successfully\n");
        }
        
        printf("\nUpdated list:\n");
        display_all(head);
        
        printf("\nTotal students: %d\n", count_students(head));
    }
    
    free_all_students(&head);
    
    printf("\n--- Program finished ---\n\n");
    
    return 0;
}

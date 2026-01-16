/**
 * =============================================================================
 * EXERCISE 1: Student Records Management System
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement a singly linked list to manage student records, maintaining
 *   them in sorted order by student ID.
 *
 * REQUIREMENTS:
 *   1. Define a Student structure with id, name and gpa fields
 *   2. Implement node creation with proper memory allocation
 *   3. Insert students in ascending order by ID
 *   4. Search for students by ID
 *   5. Delete students by ID
 *   6. Display all students in a formatted table
 *   7. Free all memory before program termination
 *
 * EXAMPLE INPUT (from data/students.txt):
 *   ADD 1001 Alice 3.75
 *   ADD 1003 Charlie 3.50
 *   ADD 1002 Bob 3.85
 *   FIND 1002
 *   DELETE 1001
 *   DISPLAY
 *
 * EXPECTED OUTPUT:
 *   Added: Alice (ID: 1001, GPA: 3.75)
 *   Added: Charlie (ID: 1003, GPA: 3.50)
 *   Added: Bob (ID: 1002, GPA: 3.85)
 *   Found: Bob (ID: 1002, GPA: 3.85)
 *   Deleted: Alice (ID: 1001)
 *   +------+----------+------+
 *   |  ID  |   Name   | GPA  |
 *   +------+----------+------+
 *   | 1002 | Bob      | 3.85 |
 *   | 1003 | Charlie  | 3.50 |
 *   +------+----------+------+
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
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

/**
 * TODO 1: Define the Student structure
 *
 * The structure should contain:
 *   - id: an integer representing the student's unique identifier
 *   - name: a character array of MAX_NAME_LENGTH to store the student's name
 *   - gpa: a float representing the grade point average (0.0 to 4.0)
 *   - next: a pointer to the next StudentNode (for linking)
 *
 * Hint: Use typedef to create an alias for easier use
 */

/* YOUR CODE HERE */
typedef struct StudentNode {
    int id;
    char name[MAX_NAME_LENGTH];
    float gpa;
    struct StudentNode *next;
} StudentNode;


/* =============================================================================
 * FUNCTION PROTOTYPES
 * =============================================================================
 */

StudentNode* create_student(int id, const char *name, float gpa);
bool insert_sorted(StudentNode **head_ref, int id, const char *name, float gpa);
StudentNode* find_student(StudentNode *head, int id);
bool delete_student(StudentNode **head_ref, int id);
void display_all(const StudentNode *head);
int count_students(const StudentNode *head);
void free_all_students(StudentNode **head_ref);
void process_commands(StudentNode **head_ref, const char *filename);

/* =============================================================================
 * FUNCTIONS
 * =============================================================================
 */

/**
 * TODO 2: Implement create_student
 *
 * Creates a new student node with the given information.
 *
 * @param id Student's unique identifier
 * @param name Student's name (will be copied)
 * @param gpa Student's grade point average
 * @return Pointer to newly created node, or NULL if allocation fails
 *
 * Steps:
 *   1. Allocate memory for a new StudentNode using malloc
 *   2. Check if allocation succeeded (return NULL if not)
 *   3. Copy the id and gpa values directly
 *   4. Copy the name using strncpy (ensure null termination!)
 *   5. Set the next pointer to NULL
 *   6. Return the pointer to the new node
 *
 * Hint: strncpy(dest, src, n) copies at most n characters
 *       Always ensure the string is null-terminated
 */
StudentNode* create_student(int id, const char *name, float gpa) {
    StudentNode *new_node = (StudentNode*)malloc(sizeof(StudentNode));

    if (new_node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }

    new_node->id = id;
    new_node->gpa = gpa;

    strncpy(new_node->name, name, MAX_NAME_LENGTH - 1);
    new_node->name[MAX_NAME_LENGTH - 1] = '\0';

    new_node->next = NULL;
    return new_node;
}

/**
 * TODO 3: Implement insert_sorted
 *
 * Inserts a new student in ascending order by ID.
 * If a student with the same ID already exists, do not insert.
 *
 * @param head_ref Pointer to the head pointer
 * @param id Student's unique identifier
 * @param name Student's name
 * @param gpa Student's GPA
 * @return true if inserted successfully, false if ID already exists or error
 *
 * Steps:
 *   1. Create a new student node
 *   2. Handle empty list case (insert as head)
 *   3. Handle insertion at beginning (new ID < head's ID)
 *   4. Find correct position by traversing until we find a node with larger ID
 *   5. Check for duplicate ID during traversal
 *   6. Insert the new node at the found position
 *
 * Hint: You need to keep track of the previous node during traversal
 */
bool insert_sorted(StudentNode **head_ref, int id, const char *name, float gpa) {
    StudentNode *new_node = create_student(id, name, gpa);
    if (new_node == NULL) {
        return false;
    }

    /* Case 1: empty list or insertion before head */
    if (*head_ref == NULL || (*head_ref)->id > id) {
        new_node->next = *head_ref;
        *head_ref = new_node;
        return true;
    }

    /* Duplicate at head */
    if ((*head_ref)->id == id) {
        free(new_node);
        return false;
    }

    /* Find insertion point */
    StudentNode *current = *head_ref;
    while (current->next != NULL && current->next->id < id) {
        current = current->next;
    }

    /* Duplicate at insertion point */
    if (current->next != NULL && current->next->id == id) {
        free(new_node);
        return false;
    }

    new_node->next = current->next;
    current->next = new_node;
    return true;
}

/**
 * TODO 4: Implement find_student
 *
 * Searches for a student by ID.
 *
 * @param head Pointer to the first node
 * @param id ID to search for
 * @return Pointer to the student node if found, NULL otherwise
 *
 * Steps:
 *   1. Traverse the list from head
 *   2. Compare each node's ID with the target
 *   3. Return immediately when found
 *   4. Since list is sorted, stop early if current ID > target
 *   5. Return NULL if not found
 */
StudentNode* find_student(StudentNode *head, int id) {
    StudentNode *current = head;

    while (current != NULL) {
        if (current->id == id) {
            return current;
        }
        /* The list is maintained in ascending order so we can stop early */
        if (current->id > id) {
            return NULL;
        }
        current = current->next;
    }

    return NULL;
}

/**
 * TODO 5: Implement delete_student
 *
 * Deletes a student by ID and frees the memory.
 *
 * @param head_ref Pointer to the head pointer
 * @param id ID of student to delete
 * @return true if deleted, false if not found
 *
 * Steps:
 *   1. Handle empty list case
 *   2. Handle deletion of head node (special case)
 *   3. Traverse to find the node before the target
 *   4. Since list is sorted, stop early if we pass the target ID
 *   5. Unlink the target node and free its memory
 *
 * Hint: You need the previous node to unlink the target
 */
bool delete_student(StudentNode **head_ref, int id) {
    if (head_ref == NULL || *head_ref == NULL) {
        return false;
    }

    /* Case 1: delete head */
    if ((*head_ref)->id == id) {
        StudentNode *temp = *head_ref;
        *head_ref = (*head_ref)->next;
        free(temp);
        return true;
    }

    /* Case 2: locate node before the one to delete */
    StudentNode *current = *head_ref;
    while (current->next != NULL && current->next->id < id) {
        current = current->next;
    }

    if (current->next != NULL && current->next->id == id) {
        StudentNode *temp = current->next;
        current->next = temp->next;
        free(temp);
        return true;
    }

    return false;
}

/**
 * TODO 6: Implement display_all
 *
 * Displays all students in a formatted table.
 *
 * @param head Pointer to the first node
 *
 * Output format:
 *   +------+------------------+------+
 *   |  ID  |       Name       | GPA  |
 *   +------+------------------+------+
 *   | 1001 | Alice            | 3.75 |
 *   | 1002 | Bob              | 3.85 |
 *   +------+------------------+------+
 *
 * Steps:
 *   1. Print the header row with borders
 *   2. Traverse the list and print each student
 *   3. Use printf formatting for alignment: %-6d, %-16s, %.2f
 *   4. Print the footer border
 *
 * Hint: Use printf format specifiers for column alignment
 *       %6d = right-aligned, width 6
 *       %-16s = left-aligned, width 16
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
 * TODO 7: Implement count_students
 *
 * Counts the number of students in the list.
 *
 * @param head Pointer to the first node
 * @return Number of students
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
 * TODO 8: Implement free_all_students
 *
 * Frees all nodes in the list and sets head to NULL.
 *
 * @param head_ref Pointer to the head pointer
 *
 * Steps:
 *   1. Traverse the list
 *   2. For each node, save the next pointer BEFORE freeing
 *   3. Free the current node
 *   4. Move to the next node
 *   5. Set *head_ref to NULL after all nodes are freed
 *
 * WARNING: Do NOT access current->next after free(current)!
 */
void free_all_students(StudentNode **head_ref) {
    if (head_ref == NULL) {
        return;
    }

    StudentNode *current = *head_ref;
    while (current != NULL) {
        StudentNode *next = current->next;
        free(current);
        current = next;
    }

    *head_ref = NULL;
}

/**
 * TODO 9: Implement process_commands
 *
 * Reads commands from a file and executes them.
 *
 * @param head_ref Pointer to the head pointer
 * @param filename Name of the command file
 *
 * Commands:
 *   ADD <id> <name> <gpa>   - Add a new student
 *   FIND <id>               - Find and print a student
 *   DELETE <id>             - Delete a student
 *   DISPLAY                 - Display all students
 *   COUNT                   - Print the number of students
 *
 * Steps:
 *   1. Open the file for reading
 *   2. Read each line using fgets
 *   3. Parse the command using sscanf or strtok
 *   4. Execute the appropriate function
 *   5. Print appropriate messages for each operation
 *   6. Close the file
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
        /* Remove newline character */
        line[strcspn(line, "\n")] = '\0';
        
        /* Skip empty lines and comments */
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }
        
        /* TODO 10: Parse and execute the command
         *
         * Use sscanf to parse the command and its arguments.
         * Example: sscanf(line, "%s %d %s %f", command, &id, name, &gpa);
         *
         * Then use strcmp to check which command was given:
         *   if (strcmp(command, "ADD") == 0) { ... }
         *
         * Print appropriate messages for success/failure of each operation.
         */
        
        if (sscanf(line, "%19s", command) != 1) {
            continue;
        }

        if (strcmp(command, "ADD") == 0) {
            if (sscanf(line, "%*s %d %49s %f", &id, name, &gpa) == 3) {
                if (insert_sorted(head_ref, id, name, gpa)) {
                    printf("Added: %s (ID: %d, GPA: %.2f)\n", name, id, gpa);
                } else {
                    printf("Failed to add: ID %d already exists\n", id);
                }
            }
        } else if (strcmp(command, "FIND") == 0) {
            if (sscanf(line, "%*s %d", &id) == 1) {
                StudentNode *found = find_student(*head_ref, id);
                if (found != NULL) {
                    printf("Found: %s (ID: %d, GPA: %.2f)\n",
                           found->name, found->id, found->gpa);
                } else {
                    printf("Not found: ID %d\n", id);
                }
            }
        } else if (strcmp(command, "DELETE") == 0) {
            if (sscanf(line, "%*s %d", &id) == 1) {
                StudentNode *student = find_student(*head_ref, id);
                if (student != NULL) {
                    char deleted_name[MAX_NAME_LENGTH];
                    strncpy(deleted_name, student->name, MAX_NAME_LENGTH - 1);
                    deleted_name[MAX_NAME_LENGTH - 1] = '\0';

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
 * HELPER FUNCTIONS (PROVIDED)
 * =============================================================================
 */

/**
 * Prints a single student's information.
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
 * Validates GPA range.
 */
bool is_valid_gpa(float gpa) {
    return gpa >= 0.0f && gpa <= 4.0f;
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     Student Records Management System                         ║\n");
    printf("║     Exercise 1 - Linked Lists                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    StudentNode *head = NULL;
    
    /* TODO 11: Implement interactive mode or file processing
     *
     * If a filename is provided as command-line argument:
     *   - Process commands from that file
     * Otherwise:
     *   - Run the demo with sample data
     *
     * Hint: argc contains the number of arguments
     *       argv[1] would be the first argument (filename)
     */
    
    if (argc > 1) {
        /* Process commands from file */
        process_commands(&head, argv[1]);
    } else {
        /* Demo mode with sample data */
        printf("\n--- Demo Mode ---\n\n");
        
        /* Add some sample students */
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
    
    /* TODO 12: Free all memory before exiting
     *
     * Call free_all_students to release all allocated memory.
     * This is CRITICAL to prevent memory leaks!
     */
    
    free_all_students(&head);
    
    printf("\n--- Program finished ---\n\n");
    
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Add a SORT_BY_GPA command that creates a new list sorted by GPA
 *    (descending order for ranking)
 *
 * 2. Add an AVERAGE command that calculates and prints the average GPA
 *
 * 3. Implement a SEARCH_BY_NAME command for partial name matching
 *
 * 4. Add support for UPDATE command to modify an existing student's GPA
 *
 * 5. Implement a REVERSE command that reverses the list order
 *
 * =============================================================================
 */

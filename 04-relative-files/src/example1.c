/**
 * =============================================================================
 * WEEK 04: LINKED LISTS
 * Complete Working Example
 * =============================================================================
 *
 * This example demonstrates:
 *   1. Node structure definition and creation
 *   2. Singly linked list operations (insert, delete, search, traverse)
 *   3. Doubly linked list implementation
 *   4. List reversal and other transformations
 *   5. Memory management and debugging techniques
 *   6. Common patterns and best practices
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -g -o example1 example1.c
 * Usage: ./example1
 *
 * Memory check: valgrind --leak-check=full ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =============================================================================
 * PART 1: SINGLY LINKED LIST - BASIC STRUCTURE
 * =============================================================================
 */

/**
 * Node structure for singly linked list.
 * 
 * Memory layout (64-bit system):
 *   - data: 4 bytes (int)
 *   - padding: 4 bytes (alignment)
 *   - next: 8 bytes (pointer)
 *   - Total: 16 bytes per node
 */
typedef struct Node {
    int data;           /* Data payload */
    struct Node *next;  /* Pointer to next node */
} Node;

/**
 * Creates a new node with the given value.
 * 
 * @param value The integer value to store in the node
 * @return Pointer to the newly created node, or NULL if allocation fails
 * 
 * Time Complexity: O(1)
 * Space Complexity: O(1) - one node allocated
 */
Node* create_node(int value) {
    Node *new_node = (Node*)malloc(sizeof(Node));
    
    if (new_node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for node\n");
        return NULL;
    }
    
    new_node->data = value;
    new_node->next = NULL;
    
    return new_node;
}

/**
 * Prints all elements in the list.
 * 
 * @param head Pointer to the first node
 * 
 * Time Complexity: O(n)
 */
void print_list(const Node *head) {
    const Node *current = head;
    
    printf("List: ");
    while (current != NULL) {
        printf("%d", current->data);
        if (current->next != NULL) {
            printf(" -> ");
        }
        current = current->next;
    }
    printf(" -> NULL\n");
}

/**
 * Counts the number of nodes in the list.
 * 
 * @param head Pointer to the first node
 * @return Number of nodes in the list
 * 
 * Time Complexity: O(n)
 */
int count_nodes(const Node *head) {
    int count = 0;
    const Node *current = head;
    
    while (current != NULL) {
        count++;
        current = current->next;
    }
    
    return count;
}

/**
 * Frees all nodes in the list.
 * 
 * @param head_ref Pointer to the head pointer (will be set to NULL)
 * 
 * Time Complexity: O(n)
 */
void free_list(Node **head_ref) {
    Node *current = *head_ref;
    Node *next;
    
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    
    *head_ref = NULL;
}

void demo_part1(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: Basic Node Operations                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Create individual nodes */
    Node *node1 = create_node(10);
    Node *node2 = create_node(20);
    Node *node3 = create_node(30);
    
    /* Manually link them */
    node1->next = node2;
    node2->next = node3;
    
    printf("Created three nodes and linked manually:\n");
    print_list(node1);
    printf("Node count: %d\n", count_nodes(node1));
    
    /* Demonstrate sizeof */
    printf("\nMemory information:\n");
    printf("  sizeof(int):   %zu bytes\n", sizeof(int));
    printf("  sizeof(Node*): %zu bytes\n", sizeof(Node*));
    printf("  sizeof(Node):  %zu bytes\n", sizeof(Node));
    
    /* Clean up */
    free_list(&node1);
    printf("\nList freed. Head is now: %p\n", (void*)node1);
}

/* =============================================================================
 * PART 2: INSERTION OPERATIONS
 * =============================================================================
 */

/**
 * Inserts a new node at the beginning of the list.
 * 
 * Why double pointer? We need to modify where 'head' points.
 * With single pointer, changes would be local to this function.
 * 
 * @param head_ref Pointer to the head pointer
 * @param value Value for the new node
 * 
 * Time Complexity: O(1)
 */
void insert_at_beginning(Node **head_ref, int value) {
    Node *new_node = create_node(value);
    if (new_node == NULL) return;
    
    new_node->next = *head_ref;
    *head_ref = new_node;
}

/**
 * Inserts a new node at the end of the list.
 * 
 * @param head_ref Pointer to the head pointer
 * @param value Value for the new node
 * 
 * Time Complexity: O(n) - must traverse to find the end
 */
void insert_at_end(Node **head_ref, int value) {
    Node *new_node = create_node(value);
    if (new_node == NULL) return;
    
    /* Special case: empty list */
    if (*head_ref == NULL) {
        *head_ref = new_node;
        return;
    }
    
    /* Traverse to the last node */
    Node *last = *head_ref;
    while (last->next != NULL) {
        last = last->next;
    }
    
    last->next = new_node;
}

/**
 * Inserts a new node at a specific position (0-indexed).
 * 
 * @param head_ref Pointer to the head pointer
 * @param value Value for the new node
 * @param position Position to insert at (0 = beginning)
 * 
 * Time Complexity: O(n)
 */
void insert_at_position(Node **head_ref, int value, int position) {
    if (position < 0) {
        fprintf(stderr, "Error: Invalid position %d\n", position);
        return;
    }
    
    if (position == 0) {
        insert_at_beginning(head_ref, value);
        return;
    }
    
    Node *new_node = create_node(value);
    if (new_node == NULL) return;
    
    /* Find node at position-1 */
    Node *current = *head_ref;
    for (int i = 0; i < position - 1 && current != NULL; i++) {
        current = current->next;
    }
    
    if (current == NULL) {
        fprintf(stderr, "Error: Position %d out of bounds\n", position);
        free(new_node);
        return;
    }
    
    /* Insert after current */
    new_node->next = current->next;
    current->next = new_node;
}

/**
 * Inserts a value in sorted order (ascending).
 * 
 * @param head_ref Pointer to the head pointer
 * @param value Value for the new node
 * 
 * Time Complexity: O(n)
 */
void insert_sorted(Node **head_ref, int value) {
    Node *new_node = create_node(value);
    if (new_node == NULL) return;
    
    /* Special case: insert at beginning */
    if (*head_ref == NULL || (*head_ref)->data >= value) {
        new_node->next = *head_ref;
        *head_ref = new_node;
        return;
    }
    
    /* Find correct position */
    Node *current = *head_ref;
    while (current->next != NULL && current->next->data < value) {
        current = current->next;
    }
    
    new_node->next = current->next;
    current->next = new_node;
}

void demo_part2(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: Insertion Operations                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Node *head = NULL;
    
    /* Demonstrate insert at beginning */
    printf("Inserting at beginning: 30, 20, 10\n");
    insert_at_beginning(&head, 30);
    insert_at_beginning(&head, 20);
    insert_at_beginning(&head, 10);
    print_list(head);
    
    /* Demonstrate insert at end */
    printf("\nInserting 40, 50 at end:\n");
    insert_at_end(&head, 40);
    insert_at_end(&head, 50);
    print_list(head);
    
    /* Demonstrate insert at position */
    printf("\nInserting 25 at position 3:\n");
    insert_at_position(&head, 25, 3);
    print_list(head);
    
    free_list(&head);
    
    /* Demonstrate sorted insertion */
    printf("\n--- Sorted Insertion Demo ---\n");
    printf("Inserting in random order: 30, 10, 50, 20, 40\n");
    insert_sorted(&head, 30);
    insert_sorted(&head, 10);
    insert_sorted(&head, 50);
    insert_sorted(&head, 20);
    insert_sorted(&head, 40);
    printf("Result: ");
    print_list(head);
    
    free_list(&head);
}

/* =============================================================================
 * PART 3: DELETION OPERATIONS
 * =============================================================================
 */

/**
 * Deletes the first node in the list.
 * 
 * @param head_ref Pointer to the head pointer
 * 
 * Time Complexity: O(1)
 */
void delete_first(Node **head_ref) {
    if (*head_ref == NULL) {
        printf("Warning: Cannot delete from empty list\n");
        return;
    }
    
    Node *temp = *head_ref;
    *head_ref = (*head_ref)->next;
    free(temp);
}

/**
 * Deletes the last node in the list.
 * 
 * @param head_ref Pointer to the head pointer
 * 
 * Time Complexity: O(n)
 */
void delete_last(Node **head_ref) {
    if (*head_ref == NULL) {
        printf("Warning: Cannot delete from empty list\n");
        return;
    }
    
    /* Special case: only one node */
    if ((*head_ref)->next == NULL) {
        free(*head_ref);
        *head_ref = NULL;
        return;
    }
    
    /* Find second-to-last node */
    Node *current = *head_ref;
    while (current->next->next != NULL) {
        current = current->next;
    }
    
    free(current->next);
    current->next = NULL;
}

/**
 * Deletes the first node containing the specified value.
 * 
 * @param head_ref Pointer to the head pointer
 * @param target Value to delete
 * @return true if deleted, false if not found
 * 
 * Time Complexity: O(n)
 */
bool delete_by_value(Node **head_ref, int target) {
    if (*head_ref == NULL) {
        return false;
    }
    
    /* Special case: target is in head */
    if ((*head_ref)->data == target) {
        Node *temp = *head_ref;
        *head_ref = (*head_ref)->next;
        free(temp);
        return true;
    }
    
    /* Search for node preceding target */
    Node *current = *head_ref;
    while (current->next != NULL && current->next->data != target) {
        current = current->next;
    }
    
    /* Target found? */
    if (current->next != NULL) {
        Node *temp = current->next;
        current->next = temp->next;
        free(temp);
        return true;
    }
    
    return false;
}

/**
 * Deletes all nodes containing the specified value.
 * 
 * @param head_ref Pointer to the head pointer
 * @param target Value to delete
 * @return Number of nodes deleted
 * 
 * Time Complexity: O(n)
 */
int delete_all_occurrences(Node **head_ref, int target) {
    int count = 0;
    
    /* Handle head nodes with target value */
    while (*head_ref != NULL && (*head_ref)->data == target) {
        Node *temp = *head_ref;
        *head_ref = (*head_ref)->next;
        free(temp);
        count++;
    }
    
    if (*head_ref == NULL) {
        return count;
    }
    
    /* Handle remaining nodes */
    Node *current = *head_ref;
    while (current->next != NULL) {
        if (current->next->data == target) {
            Node *temp = current->next;
            current->next = temp->next;
            free(temp);
            count++;
        } else {
            current = current->next;
        }
    }
    
    return count;
}

void demo_part3(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: Deletion Operations                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Node *head = NULL;
    for (int i = 1; i <= 5; i++) {
        insert_at_end(&head, i * 10);
    }
    
    printf("Initial list: ");
    print_list(head);
    
    printf("\nDeleting first node:\n");
    delete_first(&head);
    print_list(head);
    
    printf("\nDeleting last node:\n");
    delete_last(&head);
    print_list(head);
    
    printf("\nDeleting node with value 30:\n");
    if (delete_by_value(&head, 30)) {
        printf("Deleted successfully: ");
        print_list(head);
    }
    
    free_list(&head);
    
    /* Delete all occurrences demo */
    printf("\n--- Delete All Occurrences Demo ---\n");
    int values[] = {5, 3, 5, 7, 5, 9, 5};
    for (int i = 0; i < 7; i++) {
        insert_at_end(&head, values[i]);
    }
    printf("List with duplicates: ");
    print_list(head);
    
    int deleted = delete_all_occurrences(&head, 5);
    printf("Deleted %d occurrences of 5: ", deleted);
    print_list(head);
    
    free_list(&head);
}

/* =============================================================================
 * PART 4: SEARCH AND ACCESS OPERATIONS
 * =============================================================================
 */

/**
 * Searches for a value in the list.
 * 
 * @param head Pointer to the first node
 * @param target Value to search for
 * @return Pointer to the node containing target, or NULL if not found
 * 
 * Time Complexity: O(n)
 */
Node* search(Node *head, int target) {
    Node *current = head;
    
    while (current != NULL) {
        if (current->data == target) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

/**
 * Returns the position (0-indexed) of a value in the list.
 * 
 * @param head Pointer to the first node
 * @param target Value to search for
 * @return Position of target (0-indexed), or -1 if not found
 * 
 * Time Complexity: O(n)
 */
int get_position(const Node *head, int target) {
    const Node *current = head;
    int position = 0;
    
    while (current != NULL) {
        if (current->data == target) {
            return position;
        }
        current = current->next;
        position++;
    }
    
    return -1;
}

/**
 * Gets the value at a specific position.
 * 
 * @param head Pointer to the first node
 * @param position Position to access (0-indexed)
 * @param value Pointer to store the result
 * @return true if position exists, false otherwise
 * 
 * Time Complexity: O(n)
 */
bool get_at_position(const Node *head, int position, int *value) {
    if (position < 0) {
        return false;
    }
    
    const Node *current = head;
    for (int i = 0; i < position && current != NULL; i++) {
        current = current->next;
    }
    
    if (current == NULL) {
        return false;
    }
    
    *value = current->data;
    return true;
}

/**
 * Gets the nth node from the end of the list.
 * Uses two-pointer technique (fast/slow).
 * 
 * @param head Pointer to the first node
 * @param n Position from end (1 = last node)
 * @return Pointer to the nth node from end, or NULL if invalid
 * 
 * Time Complexity: O(n)
 */
Node* get_nth_from_end(Node *head, int n) {
    if (head == NULL || n <= 0) {
        return NULL;
    }
    
    Node *fast = head;
    Node *slow = head;
    
    /* Move fast pointer n nodes ahead */
    for (int i = 0; i < n; i++) {
        if (fast == NULL) {
            return NULL;  /* List too short */
        }
        fast = fast->next;
    }
    
    /* Move both until fast reaches end */
    while (fast != NULL) {
        fast = fast->next;
        slow = slow->next;
    }
    
    return slow;
}

void demo_part4(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: Search and Access Operations                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Node *head = NULL;
    int values[] = {15, 25, 35, 45, 55};
    for (int i = 0; i < 5; i++) {
        insert_at_end(&head, values[i]);
    }
    
    printf("List: ");
    print_list(head);
    
    /* Search demo */
    int target = 35;
    Node *found = search(head, target);
    printf("\nSearch for %d: %s\n", target, found ? "Found" : "Not found");
    
    target = 100;
    found = search(head, target);
    printf("Search for %d: %s\n", target, found ? "Found" : "Not found");
    
    /* Position demo */
    printf("\nPosition of 45: %d\n", get_position(head, 45));
    printf("Position of 100: %d\n", get_position(head, 100));
    
    /* Get at position demo */
    int value;
    if (get_at_position(head, 2, &value)) {
        printf("\nValue at position 2: %d\n", value);
    }
    
    /* Nth from end demo */
    printf("\n--- Nth from End Demo ---\n");
    for (int n = 1; n <= 5; n++) {
        Node *nth = get_nth_from_end(head, n);
        if (nth) {
            printf("  %d%s from end: %d\n", n, 
                   n == 1 ? "st" : n == 2 ? "nd" : n == 3 ? "rd" : "th",
                   nth->data);
        }
    }
    
    free_list(&head);
}

/* =============================================================================
 * PART 5: TRANSFORMATIONS
 * =============================================================================
 */

/**
 * Reverses the list in place.
 * 
 * @param head_ref Pointer to the head pointer
 * 
 * Time Complexity: O(n)
 * Space Complexity: O(1) - only uses three pointers
 */
void reverse_list(Node **head_ref) {
    Node *prev = NULL;
    Node *current = *head_ref;
    Node *next = NULL;
    
    while (current != NULL) {
        next = current->next;    /* Save next */
        current->next = prev;    /* Reverse link */
        prev = current;          /* Move prev forward */
        current = next;          /* Move current forward */
    }
    
    *head_ref = prev;
}

/**
 * Finds the middle node using the fast/slow pointer technique.
 * 
 * @param head Pointer to the first node
 * @return Pointer to the middle node
 * 
 * Time Complexity: O(n)
 */
Node* find_middle(Node *head) {
    if (head == NULL) {
        return NULL;
    }
    
    Node *slow = head;
    Node *fast = head;
    
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    return slow;
}

/**
 * Removes duplicate values from a sorted list.
 * 
 * @param head Pointer to the first node
 * 
 * Time Complexity: O(n)
 */
void remove_duplicates_sorted(Node *head) {
    if (head == NULL) {
        return;
    }
    
    Node *current = head;
    
    while (current->next != NULL) {
        if (current->data == current->next->data) {
            Node *temp = current->next;
            current->next = temp->next;
            free(temp);
        } else {
            current = current->next;
        }
    }
}

/**
 * Creates a copy of the list.
 * 
 * @param head Pointer to the first node of the original list
 * @return Pointer to the first node of the copied list
 * 
 * Time Complexity: O(n)
 */
Node* copy_list(const Node *head) {
    if (head == NULL) {
        return NULL;
    }
    
    /* Create head of new list */
    Node *new_head = create_node(head->data);
    if (new_head == NULL) {
        return NULL;
    }
    
    /* Copy remaining nodes */
    const Node *original = head->next;
    Node *copy = new_head;
    
    while (original != NULL) {
        copy->next = create_node(original->data);
        if (copy->next == NULL) {
            free_list(&new_head);
            return NULL;
        }
        copy = copy->next;
        original = original->next;
    }
    
    return new_head;
}

void demo_part5(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: List Transformations                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Node *head = NULL;
    for (int i = 1; i <= 5; i++) {
        insert_at_end(&head, i * 10);
    }
    
    printf("Original list: ");
    print_list(head);
    
    /* Reverse demo */
    reverse_list(&head);
    printf("Reversed list: ");
    print_list(head);
    
    reverse_list(&head);  /* Restore */
    
    /* Find middle demo */
    Node *middle = find_middle(head);
    printf("\nMiddle element: %d\n", middle ? middle->data : 0);
    
    /* Copy demo */
    Node *copy = copy_list(head);
    printf("\nOriginal: ");
    print_list(head);
    printf("Copy:     ");
    print_list(copy);
    
    free_list(&copy);
    free_list(&head);
    
    /* Remove duplicates demo */
    printf("\n--- Remove Duplicates Demo ---\n");
    int sorted_vals[] = {1, 1, 2, 3, 3, 3, 4, 5, 5};
    for (int i = 0; i < 9; i++) {
        insert_at_end(&head, sorted_vals[i]);
    }
    printf("With duplicates: ");
    print_list(head);
    
    remove_duplicates_sorted(head);
    printf("Without duplicates: ");
    print_list(head);
    
    free_list(&head);
}

/* =============================================================================
 * PART 6: DOUBLY LINKED LIST
 * =============================================================================
 */

/**
 * Node structure for doubly linked list.
 */
typedef struct DNode {
    int data;
    struct DNode *prev;
    struct DNode *next;
} DNode;

/**
 * Creates a new doubly linked list node.
 */
DNode* create_dnode(int value) {
    DNode *new_node = (DNode*)malloc(sizeof(DNode));
    
    if (new_node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    
    new_node->data = value;
    new_node->prev = NULL;
    new_node->next = NULL;
    
    return new_node;
}

/**
 * Inserts at the beginning of a doubly linked list.
 */
void dll_insert_beginning(DNode **head_ref, int value) {
    DNode *new_node = create_dnode(value);
    if (new_node == NULL) return;
    
    new_node->next = *head_ref;
    
    if (*head_ref != NULL) {
        (*head_ref)->prev = new_node;
    }
    
    *head_ref = new_node;
}

/**
 * Inserts after a given node in a doubly linked list.
 */
void dll_insert_after(DNode *node, int value) {
    if (node == NULL) return;
    
    DNode *new_node = create_dnode(value);
    if (new_node == NULL) return;
    
    new_node->next = node->next;
    new_node->prev = node;
    
    if (node->next != NULL) {
        node->next->prev = new_node;
    }
    
    node->next = new_node;
}

/**
 * Deletes a node from a doubly linked list.
 */
void dll_delete(DNode **head_ref, DNode *target) {
    if (*head_ref == NULL || target == NULL) return;
    
    /* If target is the head */
    if (*head_ref == target) {
        *head_ref = target->next;
    }
    
    /* Update next's prev pointer */
    if (target->next != NULL) {
        target->next->prev = target->prev;
    }
    
    /* Update prev's next pointer */
    if (target->prev != NULL) {
        target->prev->next = target->next;
    }
    
    free(target);
}

/**
 * Prints a doubly linked list forward and backward.
 */
void print_dll(const DNode *head) {
    const DNode *current = head;
    const DNode *last = NULL;
    
    printf("Forward:  NULL <-> ");
    while (current != NULL) {
        printf("%d <-> ", current->data);
        last = current;
        current = current->next;
    }
    printf("NULL\n");
    
    printf("Backward: NULL <-> ");
    current = last;
    while (current != NULL) {
        printf("%d <-> ", current->data);
        current = current->prev;
    }
    printf("NULL\n");
}

/**
 * Frees a doubly linked list.
 */
void free_dll(DNode **head_ref) {
    DNode *current = *head_ref;
    DNode *next;
    
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    
    *head_ref = NULL;
}

void demo_part6(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: Doubly Linked List                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    DNode *head = NULL;
    
    printf("Inserting 10, 20, 30 at beginning:\n");
    dll_insert_beginning(&head, 30);
    dll_insert_beginning(&head, 20);
    dll_insert_beginning(&head, 10);
    print_dll(head);
    
    printf("\nInserting 25 after node with value 20:\n");
    DNode *node20 = head->next;  /* Second node */
    dll_insert_after(node20, 25);
    print_dll(head);
    
    printf("\nDeleting node with value 25:\n");
    DNode *node25 = head->next->next;
    dll_delete(&head, node25);
    print_dll(head);
    
    printf("\nMemory information:\n");
    printf("  sizeof(DNode): %zu bytes\n", sizeof(DNode));
    printf("  Overhead per node: %zu bytes (two pointers)\n", 2 * sizeof(DNode*));
    
    free_dll(&head);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     WEEK 04: LINKED LISTS - Complete Example                  ║\n");
    printf("║     Algorithms and Programming Techniques                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    demo_part1();
    demo_part2();
    demo_part3();
    demo_part4();
    demo_part5();
    demo_part6();

    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     All demonstrations completed successfully!                ║\n");
    printf("║     Run 'valgrind ./example1' to verify no memory leaks       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    return 0;
}

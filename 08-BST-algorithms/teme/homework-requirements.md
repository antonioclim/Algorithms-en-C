# Week 08 Homework: Binary Search Trees

## 📋 General Information

- **Deadline:** End of Week 09
- **Points:** 100 (10% of final grade)
- **Language:** C (C11 standard)
- **Compiler:** GCC with `-Wall -Wextra -std=c11`
- **Submission:** Single `.c` file per homework via university platform

---

## 📝 Homework 1: Contact Directory (50 points)

### Description

Implement a contact directory application using a Binary Search Tree. Each contact has a unique name (string) and an associated phone number. The BST should be ordered alphabetically by contact name.

### Requirements

1. **Contact Structure** (5 points)
   - Define a `Contact` structure with:
     - `name` - character array (max 50 characters)
     - `phone` - character array (max 15 characters)
   - Define a `ContactNode` structure with:
     - `contact` - the Contact data
     - `left` and `right` - pointers to children

2. **Insertion** (10 points)
   - Implement `ContactNode *insert_contact(ContactNode *root, const char *name, const char *phone)`
   - Use `strcmp()` for alphabetical ordering
   - Ignore duplicate names (or update phone number)

3. **Search** (10 points)
   - Implement `ContactNode *search_contact(ContactNode *root, const char *name)`
   - Return the node if found, NULL otherwise

4. **Deletion** (10 points)
   - Implement `ContactNode *delete_contact(ContactNode *root, const char *name)`
   - Handle all three deletion cases correctly

5. **Display Operations** (10 points)
   - Implement `void display_all_contacts(ContactNode *root)` - alphabetical listing
   - Implement `void display_contacts_starting_with(ContactNode *root, char letter)` - contacts starting with given letter

6. **Memory Management** (5 points)
   - Implement `void free_directory(ContactNode *root)`
   - No memory leaks (verified with Valgrind)

### Example Usage

```c
int main(void) {
    ContactNode *directory = NULL;
    
    directory = insert_contact(directory, "Alice", "555-0101");
    directory = insert_contact(directory, "Bob", "555-0102");
    directory = insert_contact(directory, "Charlie", "555-0103");
    
    printf("All contacts:\n");
    display_all_contacts(directory);
    
    ContactNode *found = search_contact(directory, "Bob");
    if (found) {
        printf("Found: %s - %s\n", found->contact.name, found->contact.phone);
    }
    
    directory = delete_contact(directory, "Alice");
    
    free_directory(directory);
    return 0;
}
```

### Input/Output Format

```
Input:
5
ADD Alice 555-0101
ADD Bob 555-0102
SEARCH Charlie
DELETE Alice
LIST

Output:
Added: Alice (555-0101)
Added: Bob (555-0102)
Not found: Charlie
Deleted: Alice
Contacts:
  Bob: 555-0102
```

### File: `homework1_contacts.c`

---

## 📝 Homework 2: Expression Tree Evaluator (50 points)

### Description

Implement an expression tree that represents arithmetic expressions. Each leaf node contains an integer operand, and each internal node contains an operator (+, -, *, /). Build the tree from postfix notation and evaluate it.

### Requirements

1. **Node Structure** (5 points)
   - Define an `ExprNode` structure supporting both:
     - Operand nodes (integers)
     - Operator nodes (characters: +, -, *, /)
   - Use a union or separate fields for value and operator
   - Include a type indicator (is_operator flag or enum)

2. **Tree Building from Postfix** (15 points)
   - Implement `ExprNode *build_expression_tree(const char *postfix)`
   - Parse postfix expression (e.g., "3 4 + 2 *" = (3+4)*2)
   - Use a stack to build the tree:
     - For operand: create leaf node, push to stack
     - For operator: pop two nodes, create operator node with them as children, push result

3. **Tree Evaluation** (15 points)
   - Implement `int evaluate(ExprNode *root)`
   - Recursively evaluate:
     - Leaf: return the operand value
     - Internal: evaluate left, evaluate right, apply operator
   - Handle division by zero (return 0 or error)

4. **Tree Display** (10 points)
   - Implement `void display_infix(ExprNode *root)` - print with parentheses
   - Implement `void display_prefix(ExprNode *root)` - prefix notation
   - Implement `void display_postfix(ExprNode *root)` - postfix notation

5. **Memory Management** (5 points)
   - Implement `void free_expression_tree(ExprNode *root)`
   - No memory leaks

### Example Usage

```c
int main(void) {
    /* Expression: (3 + 4) * 2 = 14 */
    /* Postfix: 3 4 + 2 * */
    
    ExprNode *expr = build_expression_tree("3 4 + 2 *");
    
    printf("Infix: ");
    display_infix(expr);  /* ((3 + 4) * 2) */
    printf("\n");
    
    printf("Prefix: ");
    display_prefix(expr);  /* * + 3 4 2 */
    printf("\n");
    
    printf("Result: %d\n", evaluate(expr));  /* 14 */
    
    free_expression_tree(expr);
    return 0;
}
```

### Input/Output Format

```
Input:
3 4 + 2 *

Output:
Infix: ((3 + 4) * 2)
Prefix: * + 3 4 2
Postfix: 3 4 + 2 *
Result: 14
```

### File: `homework2_expression.c`

---

## 📊 Evaluation Criteria

| Criterion | Homework 1 | Homework 2 |
|-----------|------------|------------|
| Functional correctness | 20p | 20p |
| Proper BST/tree operations | 12p | 12p |
| Edge case handling | 8p | 8p |
| Code quality & style | 5p | 5p |
| No compiler warnings | 3p | 3p |
| Memory management (Valgrind) | 2p | 2p |
| **Total** | **50p** | **50p** |

### Penalties

| Issue | Penalty |
|-------|---------|
| Compiler warnings | -10% of homework score |
| Memory leaks (Valgrind errors) | -20% of homework score |
| Crashes on valid input | -30% of homework score |
| Incorrect output format | -10% of homework score |
| Late submission (per day) | -10% of homework score |
| Plagiarism | -100% (0 points) + disciplinary action |

---

## 📤 Submission Instructions

1. **Naming Convention:**
   - `homework1_contacts.c`
   - `homework2_expression.c`

2. **File Header:** Each file must begin with:
   ```c
   /**
    * Week 08 Homework [1/2]: [Title]
    * Student: [Your Name]
    * Group: [Your Group]
    * Date: [Submission Date]
    */
   ```

3. **Verification Before Submission:**
   ```bash
   # Compile without warnings
   gcc -Wall -Wextra -std=c11 -o homework1 homework1_contacts.c
   
   # Check for memory leaks
   valgrind --leak-check=full ./homework1
   ```

4. **Submit via:** University e-learning platform

---

## 💡 Tips for Success

1. **Start with the structure definitions** — get the data types right first

2. **Test incrementally** — verify each function works before moving to the next

3. **Draw the trees** — sketch what the tree should look like for test cases

4. **Handle edge cases:**
   - Empty tree operations
   - Single-node tree
   - Duplicate keys (Homework 1)
   - Division by zero (Homework 2)

5. **Use Valgrind early** — don't wait until the end to check for memory issues

6. **Read the requirements carefully** — points are allocated to specific features

---

## 📚 Reference Materials

- Lecture slides: Week 08 - Binary Search Trees
- Example code: `example1.c` in the starter kit
- Textbook: CLRS Chapter 12
- Online: [VisuAlgo BST](https://visualgo.net/en/bst)

---

*Good luck! Remember: a well-tested, working solution is better than a complex, buggy one.*

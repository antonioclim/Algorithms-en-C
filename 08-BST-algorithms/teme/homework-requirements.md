# Week 08 Homework: Binary Search Trees and Expression Trees

## 1. Administrative information

- **Deadline**: end of Week 09 (as communicated via the course platform)
- **Total score**: 100 points (10% of the final grade)
- **Language and standard**: ISO C (C11)
- **Compilation policy**: GCC with `-Wall -Wextra -std=c11` and no warnings
- **Submission format**: one `.c` file per homework task uploaded via the university platform

You are expected to submit code that is correct, robust and readable. Correctness is assessed by automatic tests and by manual inspection. Manual inspection focuses on invariants, memory ownership and clarity of algorithmic intent.

## 2. General technical requirements

### 2.1 Determinism and I/O discipline

Your programmes must produce deterministic output for the same input. Avoid any prompts such as `Enter option:` because they make automated marking brittle. Print exactly what the specification requests and nothing else.

### 2.2 Memory ownership and deallocation

All dynamically allocated nodes must be freed before programme termination. In a tree, correct deallocation is **post-order**:

```text
FREE(node):
  if node == NULL: return
  FREE(node.left)
  FREE(node.right)
  free(node)
```

If you free the parent before the children you lose access to the children and you leak memory.

### 2.3 Error handling policy

Where an operation can fail (for instance `malloc` returning `NULL` or division by zero in Homework 2) you must adopt and document a consistent policy:

- either terminate with an informative error message on standard error
- or return a neutral value and continue, provided the behaviour is specified

A silent crash will be penalised even if the core algorithm is correct.

## 3. Homework 1: Contact directory implemented as a BST (50 points)

### 3.1 Problem statement

Implement a contact directory that stores pairs `(name, phone)` where `name` is a unique identifier. The directory must support insertion, lookup, deletion and ordered listing. The underlying data structure must be a **Binary Search Tree ordered lexicographically by name**.

The assignment is designed to test whether you can apply the BST invariant to a non-numeric key domain and whether you can implement deletion without losing subtrees.

### 3.2 Data model

You must define the following two structures.

```c
#define MAX_NAME_LEN 50
#define MAX_PHONE_LEN 15

typedef struct Contact {
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
} Contact;

typedef struct ContactNode {
    Contact contact;
    struct ContactNode *left;
    struct ContactNode *right;
} ContactNode;
```

**Invariant (BST by name).** For every node `x`:

- every contact name in `x.left` is strictly smaller than `x.contact.name` under `strcmp`
- every contact name in `x.right` is strictly larger than `x.contact.name` under `strcmp`

You must state in comments whether your implementation is case-sensitive. If you choose to be case-insensitive you must normalise consistently at insertion and lookup.

### 3.3 Required operations and contracts

#### 3.3.1 Insertion

```c
ContactNode *insert_contact(ContactNode *root, const char *name, const char *phone);
```

- If `name` does not exist: insert a new node.
- If `name` already exists: either ignore the operation or update the phone number. State your policy clearly.
- Time complexity: `O(h)` where `h` is tree height.

**Algorithmic skeleton (recursive).**

```text
INSERT(root, name, phone):
  if root == NULL: return new_node(name, phone)
  cmp = strcmp(name, root.name)
  if cmp < 0: root.left = INSERT(root.left, name, phone)
  else if cmp > 0: root.right = INSERT(root.right, name, phone)
  else: handle duplicate policy
  return root
```

#### 3.3.2 Search

```c
ContactNode *search_contact(ContactNode *root, const char *name);
```

An iterative implementation is recommended because it uses constant auxiliary space.

#### 3.3.3 Deletion

```c
ContactNode *delete_contact(ContactNode *root, const char *name);
```

You must correctly handle all three deletion cases:

1. leaf node
2. one child
3. two children, using successor or predecessor replacement

If you use successor replacement, the successor is the minimum element of the right subtree.

#### 3.3.4 Display and queries

```c
void display_all_contacts(ContactNode *root);
void display_contacts_starting_with(ContactNode *root, char letter);
```

- `display_all_contacts` must output contacts in lexicographic order, which corresponds to in-order traversal.
- `display_contacts_starting_with` must output only names that start with the given letter. You may implement this as an in-order traversal with a predicate filter. If you want asymptotic pruning you must justify your pruning condition.

#### 3.3.5 Cleanup

```c
void free_directory(ContactNode *root);
```

### 3.4 Input language and output contract

Your programme should read an integer `n` followed by `n` commands.

Commands:

- `ADD <Name> <Phone>`
- `SEARCH <Name>`
- `DELETE <Name>`
- `LIST`
- `LISTPREFIX <Letter>`

Example:

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

You may extend the language with additional commands if you wish, but the required commands must behave exactly as specified.

### 3.5 Marking rubric (50 points)

- Data structures and invariant definition: 5
- Insertion correctness and duplicate policy: 10
- Search correctness and robustness: 10
- Deletion correctness (all three cases): 10
- Ordered printing and filtered printing: 10
- Memory management and absence of leaks: 5

## 4. Homework 2: Expression tree evaluator (50 points)

### 4.1 Problem statement

Implement an expression tree for arithmetic expressions. Leaves contain integer operands. Internal nodes contain binary operators from the set `{+, -, *, /}`. The tree is built from a postfix string and then used to:

- evaluate the expression
- print infix notation (fully parenthesised)
- print prefix notation
- print postfix notation

This task complements the BST work by focusing on tree construction from a linear representation and on post-order evaluation.

### 4.2 Data model

You must define a node type that can represent both operands and operators. A union is recommended:

```c
typedef struct ExprNode {
    bool is_operator;
    union {
        int operand;
        char operator;
    } value;
    struct ExprNode *left;
    struct ExprNode *right;
} ExprNode;
```

### 4.3 Building from postfix

```c
ExprNode *build_expression_tree(const char *postfix);
```

You must use a stack of node pointers. Each token is processed as follows.

- operand token: create a leaf and push it
- operator token: pop right operand, pop left operand, create operator node and push it

Pseudocode:

```text
BUILD(postfix_tokens):
  S = empty stack
  for token in postfix_tokens:
    if token is integer:
      push(S, operand_node(token))
    else if token is operator:
      R = pop(S)
      L = pop(S)
      push(S, operator_node(token, L, R))
  return pop(S)
```

The ordering constraint is essential: the first pop is the right operand.

### 4.4 Evaluation

```c
int evaluate(ExprNode *root);
```

Pseudocode:

```text
EVAL(node):
  if node is operand: return node.operand
  a = EVAL(node.left)
  b = EVAL(node.right)
  return APPLY(node.operator, a, b)
```

Division by zero must be handled. If you choose to return 0, you must document that this is your defined semantics.

### 4.5 Display functions

```c
void display_infix(ExprNode *root);
void display_prefix(ExprNode *root);
void display_postfix(ExprNode *root);
```

- infix must be fully parenthesised so that evaluation order is unambiguous
- prefix corresponds to pre-order traversal
- postfix corresponds to post-order traversal

### 4.6 Memory management

```c
void free_expression_tree(ExprNode *root);
```

Use post-order deallocation.

### 4.7 Marking rubric (50 points)

- Node type design and correctness of representation: 5
- Correct postfix parsing and tree construction: 15
- Correct evaluation including edge cases: 15
- Correct rendering of notations: 10
- Memory management and robustness: 5

## 5. Global evaluation criteria and penalties

### 5.1 Criteria applied to both homeworks

- functional correctness under automated tests
- correct application of BST and tree invariants
- correct handling of edge cases
- absence of compiler warnings
- absence of memory leaks

### 5.2 Penalties

- compiler warnings: up to 10% of the homework score
- memory leaks: up to 20% of the homework score
- crashes on valid input: up to 30% of the homework score
- incorrect output format: up to 10% of the homework score
- late submission: as specified by the course policy

### 5.3 Academic integrity

You must write your own implementation. If you consult external material, cite it in comments. Submissions that are substantially identical will be treated as plagiarism according to university regulations.

# Week 05 Homework: Stacks

## 📋 General Information

| Item | Details |
|------|---------|
| **Deadline** | End of Week 06 |
| **Total Points** | 100 (10% of final grade) |
| **Language** | C (C11 standard) |
| **Compiler** | GCC with `-Wall -Wextra -std=c11` |
| **Submission** | Upload source files to course platform |

---

## 📝 Homework 1: Postfix Expression Calculator (50 points)

### Description

Implement a complete postfix expression calculator that reads expressions from a file or standard input and evaluates them using a stack. The calculator must support multi-digit numbers, negative numbers and all four basic arithmetic operations.

### Requirements

1. **Input Parsing** (10 points)
   - Read expressions from standard input or a file specified as command-line argument
   - Each expression is on a separate line
   - Tokens (numbers and operators) are separated by spaces
   - Support multi-digit integers (e.g., `123`)
   - Support negative numbers (e.g., `-5`)

2. **Stack Implementation** (15 points)
   - Use an array-based stack with dynamic resizing
   - Implement proper memory management (no leaks)
   - Handle stack underflow gracefully with error messages

3. **Expression Evaluation** (15 points)
   - Support operators: `+`, `-`, `*`, `/`
   - Integer division (truncate towards zero)
   - Detect and report division by zero
   - Detect and report malformed expressions

4. **Output Format** (10 points)
   - For each valid expression, print: `Expression: [expr] = [result]`
   - For errors, print: `Error: [description]`
   - Exit cleanly after processing all input

### Example Usage

```bash
$ ./homework1 < expressions.txt

Expression: 3 4 + = 7
Expression: 10 5 2 * - = 0
Expression: 100 25 / = 4
Error: Division by zero in expression "5 0 /"
Error: Malformed expression "3 + 4" (insufficient operands)
```

### Example Input File

```
3 4 +
10 5 2 * -
100 25 /
5 0 /
3 + 4
15 7 1 1 + - / 3 * 2 1 1 + + -
```

### Hints

- Use `strtol()` for robust number parsing
- Check for stack underflow before every pop
- Consider edge cases: empty input, single number, all operators

### File: `homework1_postfix.c`

---

## 📝 Homework 2: Infix to Postfix Converter (50 points)

### Description

Implement the Shunting-Yard algorithm to convert infix mathematical expressions to postfix notation. This classic algorithm, invented by Edsger Dijkstra, uses two stacks: one for operators and one for the output.

### Requirements

1. **Operator Stack** (10 points)
   - Implement using a linked-list stack
   - Store operators with their precedence
   - Handle stack operations correctly

2. **Shunting-Yard Algorithm** (20 points)
   - Correctly handle operator precedence: `*` `/` > `+` `-`
   - Correctly handle left-to-right associativity
   - Support parentheses `(` and `)`
   - Detect mismatched parentheses

3. **Input/Output** (10 points)
   - Read infix expressions from standard input
   - Output equivalent postfix expression
   - Handle whitespace in input gracefully

4. **Error Handling** (10 points)
   - Detect and report mismatched parentheses
   - Detect and report invalid characters
   - Clean up all memory on error

### Operator Precedence Table

| Operator | Precedence | Associativity |
|----------|------------|---------------|
| `*`, `/` | 2 | Left-to-right |
| `+`, `-` | 1 | Left-to-right |

### Example Usage

```bash
$ ./homework2

Enter infix expression: 3 + 4 * 2
Postfix: 3 4 2 * +

Enter infix expression: ( 3 + 4 ) * 2
Postfix: 3 4 + 2 *

Enter infix expression: 10 / ( 5 - 3 )
Postfix: 10 5 3 - /

Enter infix expression: ( ( 1 + 2 ) * 3
Error: Mismatched parentheses - unclosed '('
```

### Algorithm Outline

```
FOR each token in expression:
    IF token is number:
        add to output
    ELSE IF token is operator:
        WHILE (operator stack not empty AND 
               top has greater/equal precedence):
            pop from operator stack to output
        push token to operator stack
    ELSE IF token is '(':
        push to operator stack
    ELSE IF token is ')':
        WHILE top is not '(':
            pop to output
        pop '(' from stack (discard)

WHILE operator stack not empty:
    pop to output
```

### Hints

- Store operator precedence in a function or lookup table
- Remember: parentheses are pushed but never go to output
- Test with nested parentheses: `((1 + 2) * (3 + 4))`

### File: `homework2_shunting.c`

---

## 📊 Evaluation Criteria

| Criterion | Points |
|-----------|--------|
| Functional correctness | 40 |
| Proper stack usage | 25 |
| Memory management (no leaks) | 15 |
| Error handling | 10 |
| Code quality and comments | 10 |

### Penalties

| Issue | Penalty |
|-------|---------|
| Compiler warnings | -10 points |
| Memory leaks (Valgrind) | -20 points |
| Crashes on valid input | -30 points |
| Plagiarism | -50 points (minimum) |

---

## 📤 Submission Instructions

1. Create a directory named `week05_[your_name]`
2. Place your source files inside:
   - `homework1_postfix.c`
   - `homework2_shunting.c`
3. Include a `README.txt` with:
   - Your name and student ID
   - Compilation instructions
   - Any known issues or limitations
4. Compress as `.zip` and upload to course platform

**Naming Convention:** `week05_popescu_ion.zip`

---

## 💡 Tips for Success

1. **Start with the stack implementation** — test it thoroughly before building the calculator
2. **Test incrementally** — verify each component works before combining them
3. **Use Valgrind early and often** — memory bugs are easier to fix when caught early
4. **Handle edge cases** — empty input, single elements, maximum values
5. **Comment your code** — explain the algorithm, not obvious syntax
6. **Draw diagrams** — visualise stack operations for complex expressions

---

## 📚 Recommended Resources

- Sedgewick & Wayne, *Algorithms* — Section 1.3 on Stacks
- [Shunting-Yard Algorithm on Wikipedia](https://en.wikipedia.org/wiki/Shunting-yard_algorithm)
- [Visualgo Stack Visualisation](https://visualgo.net/en/stack)

---

*Good luck! Remember: understanding the algorithm is more important than writing code.*

# Week 05 Homework: Stack-Centred Expression Processing in C (C11)

## Administrative specification

**Deadline:** end of Week 06.

**Weighting:** 100 points (10% of the final grade).

**Implementation language and toolchain:** ISO C11 compiled with GCC using `-Wall -Wextra -std=c11`.

**Submission format:** a single archive containing the required source files and a brief `README.txt` describing compilation commands and known limitations.

**Academic integrity note:** the expected solution is small enough that similarity detection is highly effective. If you reuse external sources, cite them in your `README.txt` and ensure that your submitted code remains your own work.

---

## Deliverable overview

You must submit the following two programs:

1. **Homework 1** `homework1_postfix.c`: a postfix (Reverse Polish) evaluator.
2. **Homework 2** `homework2_shunting.c`: an infix-to-postfix converter based on the Shunting-Yard method.

Each file must compile independently. Your implementation must perform explicit memory management and must exit cleanly on both success and error paths.

---

## Homework 1: Postfix expression calculator (50 points)

### Problem statement

Implement a complete postfix evaluator that reads one expression per line either from standard input or from a file supplied as a command line argument. Each expression consists of space-separated tokens. Tokens are either signed integers or one of the binary operators `+`, `-`, `*` and `/`.

The evaluator must compute the integer result and report errors precisely.

### Functional requirements

#### 1. Input acquisition and tokenisation (10 points)

- If the programme is executed with no arguments, read from `stdin`.
- If the programme is executed with exactly one argument, interpret it as a path and read from that file.
- Each input line is one independent expression.
- Tokens are separated by one or more ASCII whitespace characters.
- Integers may be multi-digit and may have a leading minus sign.

Recommended primitive: `strtol()` with careful end-pointer checking.

#### 2. Stack implementation and memory safety (15 points)

- Implement an **array-based stack** of integers with geometric growth.
- Detect stack underflow and treat it as a malformed expression.
- Free all allocated memory before returning from `main` including in error cases.

#### 3. Expression evaluation semantics (15 points)

- Support the operators `+`, `-`, `*` and `/`.
- Use C integer division semantics which truncate towards zero.
- Detect division by zero.
- Detect malformed expressions such as:
  - insufficient operands for an operator
  - surplus operands at the end of evaluation (more than one value remains)
  - invalid tokens

#### 4. Output contract (10 points)

For each input line:

- On success print:

  `Expression: <original line> = <result>`

- On error print:

  `Error: <diagnostic>`

The diagnostic text must be meaningful and must identify the cause. If you quote the original expression in the diagnostic, keep the quoting consistent.

### Canonical algorithmic model

Postfix evaluation is a deterministic left-to-right reduction strategy.

#### Informal invariant

After processing the first *k* tokens, the stack contains exactly the partial results of the subexpressions that have been fully determined from the prefix. In particular, the stack height equals the number of operands that have been read but not yet consumed by operators.

#### Pseudocode

```text
procedure evaluate_postfix(tokens):
    S <- empty stack

    for each t in tokens:
        if t is integer:
            push(S, parse_int(t))
        else if t is operator op in {+, -, *, /}:
            if size(S) < 2:
                error "insufficient operands"
            b <- pop(S)
            a <- pop(S)
            if op = / and b = 0:
                error "division by zero"
            push(S, apply(op, a, b))
        else:
            error "invalid token"

    if size(S) != 1:
        error "malformed expression"

    return pop(S)
```

#### Reference implementation sketch in C

```c
long b = stack_pop(&st);
long a = stack_pop(&st);
long r;

switch (op) {
    case '+': r = a + b; break;
    case '-': r = a - b; break;
    case '*': r = a * b; break;
    case '/':
        if (b == 0) { /* report error */ }
        r = a / b;
        break;
}

stack_push(&st, r);
```

### Example session

```bash
$ ./homework1 < expressions.txt
Expression: 3 4 + = 7
Expression: 10 5 2 * - = 0
Expression: 100 25 / = 4
Error: Division by zero in expression "5 0 /"
Error: Malformed expression "3 + 4" (operator without sufficient operands)
```

### Marking notes

A correct evaluator is not merely a calculator. It is a *validator* for the structural well-formedness of the postfix notation. Most marks are awarded for disciplined error detection, deterministic output and memory safety.

---

## Homework 2: Infix to postfix converter using a Shunting-Yard variant (50 points)

### Problem statement

Convert an infix expression over integers and the binary operators `+`, `-`, `*` and `/` into an equivalent postfix expression. Parentheses `(` and `)` may be used for grouping.

You must implement a Shunting-Yard style algorithm using a stack for operators.

### Functional requirements

#### 1. Operator stack (10 points)

- Implement the operator stack as a linked stack.
- Each node stores at least the operator character.
- The stack must support `push`, `pop` and `peek` in constant time.

#### 2. Conversion algorithm (20 points)

- Enforce precedence: `*` and `/` bind more strongly than `+` and `-`.
- Enforce left-to-right associativity for all operators.
- Support parentheses.
- Detect mismatched parentheses and report a precise error.

#### 3. Input and output (10 points)

- Read a line of infix input from standard input.
- Produce a space-separated postfix expression.
- Whitespace in the input must be tolerated.

#### 4. Error handling and clean-up (10 points)

- Detect invalid characters.
- Detect misplaced operators.
- Free all allocated memory on all exit paths.

### Operator precedence model

| Operator | Precedence | Associativity |
|---|---:|---|
| `*` `/` | 2 | left-to-right |
| `+` `-` | 1 | left-to-right |

### Canonical algorithmic model

The conversion maintains two streams: an output list and a stack of operators. Operators are delayed until it is safe to emit them without violating precedence and associativity.

#### Pseudocode

```text
procedure infix_to_postfix(tokens):
    out <- empty list
    ops <- empty stack

    for each token t in tokens:
        if t is number:
            append(out, t)
        else if t is operator op:
            while ops not empty and top(ops) is operator and
                  precedence(top(ops)) >= precedence(op):
                append(out, pop(ops))
            push(ops, op)
        else if t = '(':
            push(ops, t)
        else if t = ')':
            while ops not empty and top(ops) != '(':
                append(out, pop(ops))
            if ops empty:
                error "mismatched parentheses"
            pop(ops)  // discard '('
        else:
            error "invalid token"

    while ops not empty:
        if top(ops) is '(':
            error "mismatched parentheses"
        append(out, pop(ops))

    return join(out, " ")
```

### Worked example

Input: `3 + 4 * 2`

- read `3` → output `3`
- read `+` → ops `[+]`
- read `4` → output `3 4`
- read `*` → ops `[+, *]` (no pop because `*` has higher precedence)
- read `2` → output `3 4 2`
- end → pop ops → output `3 4 2 * +`

### Extension notes for advanced students

If you wish to support unary minus, you must disambiguate it during tokenisation which typically requires keeping track of whether the previous token can terminate an operand. This extension is not required unless explicitly stated by the instructor.

---

## Evaluation rubric

| Criterion | Points |
|---|---:|
| Functional correctness | 40 |
| Appropriate stack usage | 25 |
| Memory management and clean-up | 15 |
| Error handling quality | 10 |
| Code clarity and maintainability | 10 |

### Common penalties

| Issue | Penalty |
|---|---:|
| Compiler warnings | −10 |
| Memory leaks or use-after-free | −20 |
| Crash on valid input | −30 |

---

## Submission checklist

- Both files compile with the required flags.
- No warnings are emitted under `-Wall -Wextra`.
- Your output conforms to the stated contract.
- You tested with both valid and invalid inputs.
- Your `README.txt` includes build commands.

# Extended Challenges — Week 05: Stacks

## 🚀 Advanced Challenges (Optional)

These challenges extend beyond the standard homework and are designed for students seeking additional practice or extra credit. Each correctly solved challenge earns **+10 bonus points**.

---

## ⭐ Challenge 1: Min Stack (Difficulty: Medium)

### Description

Design a stack that supports push, pop, peek and **getMin** operations, all in O(1) time complexity. The getMin operation returns the minimum element currently in the stack.

### Requirements

- Implement `MinStack` structure
- `min_stack_push(stack, value)` — O(1)
- `min_stack_pop(stack)` — O(1)
- `min_stack_peek(stack)` — O(1)
- `min_stack_get_min(stack)` — O(1) ← The challenge!
- Memory usage should be O(n) where n is number of elements

### Hint

Use an auxiliary stack to track minimum values. When pushing, also push to the min stack if the value is less than or equal to the current minimum.

### Example

```c
MinStack *s = min_stack_create();
min_stack_push(s, 5);  // Stack: [5], Min: 5
min_stack_push(s, 3);  // Stack: [5,3], Min: 3
min_stack_push(s, 7);  // Stack: [5,3,7], Min: 3
min_stack_get_min(s);  // Returns 3
min_stack_pop(s);      // Stack: [5,3], Min: 3
min_stack_pop(s);      // Stack: [5], Min: 5
min_stack_get_min(s);  // Returns 5
```

### Bonus Points: +10

---

## ⭐ Challenge 2: Queue Using Two Stacks (Difficulty: Medium)

### Description

Implement a queue (FIFO) data structure using only two stacks. The queue must support standard operations: enqueue (add to back) and dequeue (remove from front).

### Requirements

- Use only stack push and pop operations internally
- `queue_enqueue(queue, value)` — amortised O(1)
- `queue_dequeue(queue)` — amortised O(1)
- `queue_peek(queue)` — O(1) after first dequeue
- No additional data structures allowed

### Approach

- Use one stack (`inbox`) for enqueue operations
- Use another stack (`outbox`) for dequeue operations
- Transfer elements from inbox to outbox only when outbox is empty

### Example

```c
QueueFromStacks *q = queue_create();
queue_enqueue(q, 1);  // inbox: [1]
queue_enqueue(q, 2);  // inbox: [1, 2]
queue_enqueue(q, 3);  // inbox: [1, 2, 3]
queue_dequeue(q);     // Returns 1 (transfer to outbox: [3, 2, 1], pop)
queue_dequeue(q);     // Returns 2
queue_enqueue(q, 4);  // inbox: [4], outbox: [3]
queue_dequeue(q);     // Returns 3
queue_dequeue(q);     // Returns 4
```

### Bonus Points: +10

---

## ⭐ Challenge 3: Valid Parentheses with Wildcards (Difficulty: Hard)

### Description

Given a string containing only `(`, `)` and `*`, determine if the string is valid. The `*` character can represent either an opening parenthesis `(`, a closing parenthesis `)`, or an empty string.

### Requirements

- Implement `is_valid_with_wildcards(const char *s)`
- Return `true` if the string can be valid with some assignment of wildcards
- Time complexity should be O(n)

### Examples

```
"()"     → true
"(*)"   → true (∗ = empty)
"(*))"  → true (∗ = ()
"((*))" → true (∗ = ))
")*("   → false (no valid assignment)
"((("   → false
"((**)" → true (first ∗ = ), second ∗ = empty)
```

### Hint

Use two passes: one left-to-right tracking potential unmatched `(`, and one right-to-left tracking potential unmatched `)`. Alternatively, track a range of possible open parenthesis counts.

### Bonus Points: +10

---

## ⭐ Challenge 4: Simplify Unix Path (Difficulty: Medium)

### Description

Given an absolute Unix file path, simplify it to its canonical form. The canonical path should:
- Start with a single `/`
- Not end with `/` (unless it's the root)
- Not contain `.` or `..` as path components
- Not contain multiple consecutive `/`

### Requirements

- Implement `char* simplify_path(const char *path)`
- Handle `.` (current directory)
- Handle `..` (parent directory)
- Handle multiple slashes
- Return dynamically allocated result (caller frees)

### Examples

```
"/home/"           → "/home"
"/home//foo/"      → "/home/foo"
"/a/./b/../../c/"  → "/c"
"/../"             → "/"
"/home/../usr/./local" → "/usr/local"
```

### Hint

Split the path by `/`, push directory names onto a stack, pop on `..`, ignore `.` and empty strings.

### Bonus Points: +10

---

## ⭐ Challenge 5: Decode String (Difficulty: Hard)

### Description

Given an encoded string in the format `k[encoded_string]`, decode it by repeating `encoded_string` exactly `k` times. The encoding may be nested.

### Requirements

- Implement `char* decode_string(const char *s)`
- Handle nested encodings: `2[a2[b]]` → `abbabb`
- Handle multiple sequential encodings: `2[a]3[b]` → `aabbb`
- `k` is guaranteed to be a positive integer
- Return dynamically allocated result

### Examples

```
"3[a]2[bc]"     → "aaabcbc"
"3[a2[c]]"      → "accaccacc"
"2[abc]3[cd]ef" → "abcabccdcdcdef"
"10[a]"         → "aaaaaaaaaa"
```

### Hint

Use a stack of strings and a stack of multipliers. When you see `[`, push current string and multiplier. When you see `]`, pop and repeat.

### Bonus Points: +10

---

## 📊 Bonus Point System

| Challenges Completed | Total Bonus |
|---------------------|-------------|
| 1 | +10 points |
| 2 | +20 points |
| 3 | +30 points |
| 4 | +40 points |
| All 5 | +50 points + "Stack Master" badge 🏆 |

---

## 📤 Submission

- Name files: `challenge1_minstack.c`, `challenge2_queue.c`, etc.
- Include in the same submission as regular homework
- Each file must compile independently
- Include test cases demonstrating functionality

---

## 📋 Evaluation Notes

- Partial credit available for incomplete solutions with correct approach
- Bonus points cannot exceed 50 total
- Bonus points are added after regular homework score
- Maximum total score: 100 (homework) + 50 (bonus) = 150 points

---

*These challenges are optional but highly recommended for deepening your understanding of stack applications.*

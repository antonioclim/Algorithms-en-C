# Extended challenges for Week 05: Stack applications beyond the core syllabus

## Purpose and scope

The exercises below are optional. They are designed to reinforce the idea that the stack is not merely a container with a restricted interface but an operational model for reasoning about partial computation, deferred commitment and nested structure. Each challenge emphasises a particular pattern that recurs in real software: auxiliary state maintenance, amortised reasoning, constraint propagation, canonicalisation of paths and decoding of nested grammars.

Unless a challenge explicitly requires a different representation, you may reuse either an array-based stack with geometric growth or a linked stack. In all cases, you must demonstrate explicit memory management, deterministic error handling and reproducible testing.

**Assessment convention:** each completed challenge is worth **+10 bonus points** with a maximum of +50.

---

## Challenge 1: Min Stack with constant-time minimum query

### Statement

Design a stack that supports `push`, `pop`, `peek` and `get_min` such that each operation runs in worst-case O(1) time.

### Canonical design

Maintain two stacks:

- `S` stores all values in the ordinary LIFO manner.
- `M` stores a non-increasing sequence of minima. Each element of `M` equals the minimum of `S` at the moment it was pushed.

The central invariant is:

- `top(M) = min(S)` whenever `S` is non-empty.

To preserve the invariant, every `push(x)` compares `x` to the current minimum.

### Pseudocode

```text
procedure push(x):
    push(S, x)
    if M empty or x <= top(M):
        push(M, x)

procedure pop():
    if S empty: error
    x <- pop(S)
    if x = top(M):
        pop(M)
    return x

procedure get_min():
    if M empty: error
    return top(M)
```

### Correctness sketch

- When a new global minimum is introduced it is recorded in `M`.
- When the current minimum is removed from `S`, the corresponding entry is removed from `M`.
- Therefore `M` mirrors the historical minima that remain present in `S`.

### Complexity

- Worst-case time: O(1) per operation.
- Space: O(n) in the worst case, achieved when the sequence is strictly decreasing.

### Alternative representation

You may instead store pairs `(value, current_min)` in a single stack. This doubles per-element storage but avoids an auxiliary structure. Provide a clear argument for whichever choice you make.

---

## Challenge 2: Queue implemented using two stacks

### Statement

Implement a FIFO queue using only stack operations. The queue must support `enqueue`, `dequeue` and `peek`.

### Canonical design

Maintain two stacks:

- `inbox`: receives enqueued elements.
- `outbox`: serves dequeues.

When `outbox` is empty and a dequeue is requested, transfer all items from `inbox` to `outbox`. This reverses order once and thereby exposes the oldest element at the top of `outbox`.

### Pseudocode

```text
procedure enqueue(x):
    push(inbox, x)

procedure normalise():
    if outbox empty:
        while inbox not empty:
            push(outbox, pop(inbox))

procedure dequeue():
    normalise()
    if outbox empty: error
    return pop(outbox)

procedure peek():
    normalise()
    if outbox empty: error
    return top(outbox)
```

### Amortised analysis

Although `normalise()` may move O(n) elements in a single call, each element is transferred at most once from `inbox` to `outbox` before it is removed permanently. Across a sequence of m operations, the total number of stack moves is O(m) and therefore the amortised cost of each queue operation is O(1).

A concise potential argument is to define the potential Φ as the number of elements in `inbox`. Each transfer reduces Φ while paying for the pops and pushes.

---

## Challenge 3: Parentheses with wildcards

### Statement

Given a string over the alphabet `{ '(', ')', '*' }`, decide whether the string can be interpreted as a balanced parenthesis expression by replacing each `*` with `'('`, `')'` or the empty string.

### Range-tracking algorithm (recommended)

Track an interval `[lo, hi]` representing the set of possible counts of currently open parentheses after reading each prefix.

- `lo` is the minimum possible number of opens.
- `hi` is the maximum possible number of opens.

Update rules:

- On `'('`: `lo++`, `hi++`.
- On `')'`: `lo--`, `hi--`.
- On `'*'`: `lo--`, `hi++` (because `*` may act as `')'` or `'('` or empty).

Clamp `lo` at 0 because negative open counts are not meaningful.

Reject immediately if `hi` becomes negative.

Accept at the end if `lo == 0`.

### Pseudocode

```text
procedure valid_with_wildcards(s):
    lo <- 0
    hi <- 0

    for each c in s:
        if c = '(':
            lo <- lo + 1
            hi <- hi + 1
        else if c = ')':
            lo <- lo - 1
            hi <- hi - 1
        else if c = '*':
            lo <- lo - 1
            hi <- hi + 1
        else:
            error

        if hi < 0:
            return false
        if lo < 0:
            lo <- 0

    return lo = 0
```

### Rationale

The interval summarises all feasible assignments without enumerating them. The key insight is that `*` widens the feasible set in both directions. The algorithm is linear in the length of the string and uses constant extra space.

---

## Challenge 4: Simplify an absolute Unix path

### Statement

Given an absolute path string, compute its canonical form by eliminating redundant slashes, `.` components and `..` components.

### Stack interpretation

Treat the path as a sequence of components separated by `/`. Maintain a stack of directory names.

- On a normal name, push it.
- On `.`, do nothing.
- On `..`, pop if possible.

At the end, join the stack with `/` and prepend a leading `/`.

### Pseudocode

```text
procedure simplify(path):
    components <- split(path, '/')
    S <- empty stack

    for each comp in components:
        if comp = '' or comp = '.':
            continue
        else if comp = '..':
            if S not empty:
                pop(S)
        else:
            push(S, comp)

    if S empty:
        return '/'
    else:
        return '/' + join(S, '/')
```

### Implementation note in C

You may parse in-place using two indices to avoid allocating intermediate arrays. If you allocate components, ensure you free them all.

---

## Challenge 5: Decode nested repetition `k[encoded_string]`

### Statement

Decode a string that encodes repetition using brackets. Nested encodings are permitted.

### Two-stack design

Maintain:

- `counts`: an integer stack of repetition factors.
- `fragments`: a string stack of the accumulated prefix prior to each `[`.

Parse digits to build `k`. On `[`, push `k` and the current fragment. Reset the current fragment. On `]`, pop `(k, prev)` and set `current = prev + current repeated k times`.

### Pseudocode

```text
procedure decode(s):
    counts <- empty stack
    frags <- empty stack
    current <- ''
    k <- 0

    for each c in s:
        if c is digit:
            k <- 10*k + value(c)
        else if c = '[':
            push(counts, k)
            push(frags, current)
            current <- ''
            k <- 0
        else if c = ']':
            rep <- pop(counts)
            prev <- pop(frags)
            current <- prev + repeat(current, rep)
        else:
            current <- current + c

    return current
```

### Complexity

Let n be the input length and let L be the output length. Parsing is O(n) while string construction is O(L). In C, repeated concatenation must be implemented carefully to avoid quadratic behaviour. A robust strategy is to use dynamic buffers with geometric growth.

---

## Submission guidance and minimal evidence

For each challenge you submit:

- Provide a short `main` function that demonstrates the behaviour on at least three non-trivial test cases.
- Document edge cases you considered.
- Demonstrate that your program emits no warnings under the required compilation flags.

A small but well-designed test suite is part of the solution.

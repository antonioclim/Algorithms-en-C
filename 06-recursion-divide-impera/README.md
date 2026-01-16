# Week 06: Queues in C

## 1. Purpose and scope

This laboratory unit consolidates the queue abstract data type (ADT) as a first-class modelling device for *ordered, single-ended admission* and *single-ended service*. The focus is deliberately twofold:

1. A **circular buffer queue** in which FIFO semantics are realised over a bounded array by modular arithmetic and a carefully maintained representation invariant.
2. A **round-robin scheduler simulation** in which a queue becomes a concrete operational model of a ready list under preemptive time-slicing.

The repository therefore serves both as an algorithms-and-data-structures artefact and as a bridge to systems thinking: queues are not only containers but also *policies expressed in state transitions*.

## 2. Learning outcomes

After completing the material and exercises you should be able to:

- Specify the queue ADT by behaviour and invariants rather than by implementation details.
- Implement a bounded queue as a ring buffer with strict O(1) enqueue and dequeue in C.
- Justify the correctness of circular indexing by maintaining an explicit state invariant.
- Analyse the time and space complexity of queue operations in array-based and pointer-based designs.
- Use a queue as the central structure in a discrete-event simulation of round-robin CPU scheduling.
- Produce deterministic, testable output by separating algorithmic state evolution from presentation.

## 3. Repository structure

```
06-recursion-divide-impera/
├── README.md
├── Makefile
├── src/
│   ├── example1.c        # Fully worked demonstrations
│   ├── exercise1.c       # Circular buffer queue (student implementation)
│   └── exercise2.c       # Round-robin scheduler (student implementation)
├── solution/
│   ├── exercise1_sol.c
│   ├── exercise2_sol.c
│   ├── homework1_sol.c
│   └── homework2_sol.c
├── data/
│   ├── commands.txt      # Interactive command sequence for Exercise 1
│   └── processes.txt     # Process set for Exercise 2
├── tests/
│   ├── test1_input.txt
│   ├── test1_expected.txt
│   ├── test2_input.txt
│   └── test2_expected.txt
├── teme/
│   ├── homework-requirements.md
│   └── homework-extended.md
└── slides/
    ├── presentation-week06.html
    └── presentation-comparativ.html
```

The directory name is historical. The technical content of this week is **queues**.

## 4. The queue ADT as a behavioural contract

### 4.1 Abstract specification

Let a queue contain a finite sequence \(Q = \langle q_0, q_1, \dots, q_{n-1} \rangle\) where \(q_0\) is the element at the **front** and \(q_{n-1}\) is the element at the **rear**.

The core operations may be specified as partial functions:

- **enqueue**: \(\text{enqueue}(Q, x) = \langle q_0, \dots, q_{n-1}, x \rangle\)
- **dequeue**: \(\text{dequeue}(Q) = (q_0, \langle q_1, \dots, q_{n-1} \rangle)\) provided \(n > 0\)
- **peek**: \(\text{peek}(Q) = q_0\) provided \(n > 0\)

A crucial point in robust implementation is that this specification does **not** mention memory layout, indices or pointers. Those are *representation choices* that must be shown to preserve the contract.

### 4.2 Representation invariants

In this week’s bounded implementation, the queue state is represented by a 5-tuple:

\[
(data, front, rear, count, capacity)
\]

with the invariants:

1. \(0 \le front < capacity\)
2. \(0 \le rear < capacity\)
3. \(0 \le count \le capacity\)
4. **Empty condition**: \(count = 0\)
5. **Full condition**: \(count = capacity\)
6. **Index semantics**:
   - `front` indexes the next element to be dequeued
   - `rear` indexes the next slot to be written by enqueue

The use of `count` eliminates the classic ambiguity in circular buffers where `(front == rear)` can mean either empty or full.

## 5. Circular buffers and modular arithmetic

### 5.1 The algorithmic idea

A circular buffer treats an array as a ring. If \(i\) is an index then its successor under wraparound is:

\[
\text{next}(i) = (i + 1) \bmod capacity
\]

This simple recurrence is the entire reason the data structure is attractive: it provides a *constant-time pointer movement* without shifting elements.

### 5.2 Pseudocode

#### Enqueue

```
procedure ENQUEUE(Q, x)
    if Q.count = Q.capacity then
        report OVERFLOW
        return false
    end if

    Q.data[Q.rear] ← x
    Q.rear ← (Q.rear + 1) mod Q.capacity
    Q.count ← Q.count + 1
    return true
end procedure
```

#### Dequeue

```
procedure DEQUEUE(Q)
    if Q.count = 0 then
        report UNDERFLOW
        return (false, ⊥)
    end if

    x ← Q.data[Q.front]
    Q.front ← (Q.front + 1) mod Q.capacity
    Q.count ← Q.count − 1
    return (true, x)
end procedure
```

#### Peek

```
procedure PEEK(Q)
    if Q.count = 0 then
        return (false, ⊥)
    end if
    return (true, Q.data[Q.front])
end procedure
```

### 5.3 Language mapping examples

The algorithm above is language-agnostic. The following snippets show the same invariant and update pattern expressed in several common languages.

#### C (bounded ring buffer)

```c
q->data[q->rear] = item;
q->rear = (q->rear + 1) % q->capacity;
q->count++;
```

#### C++ (same logic with RAII container)

```cpp
buffer[rear] = item;
rear = (rear + 1) % capacity;
++count;
```

#### Java (typical array-backed queue skeleton)

```java
array[rear] = item;
rear = (rear + 1) % capacity;
count++;
```

#### Python (explicit indices, not `collections.deque`)

```python
data[rear] = item
rear = (rear + 1) % capacity
count += 1
```

The essential pedagogical point is that the *proof obligations* remain the same: the invariants must be preserved after each operation, regardless of syntax.

## 6. Exercise 1: `src/exercise1.c` (interactive circular queue)

### 6.1 Functional overview

The program implements a dynamic circular queue of integers and exposes it through a small command language:

- `ENQUEUE n`
- `DEQUEUE`
- `PEEK`
- `SIZE`
- `DISPLAY` (linear view)
- `CIRCULAR` (ring view)
- `STATS`
- `QUIT`

The internal queue operations are strictly O(1). The display procedures are O(capacity) by design because they render the entire ring for conceptual clarity.

### 6.2 Deterministic test behaviour

The program distinguishes **interactive mode** from **scripted mode**:

- In interactive mode the programme prints a banner and a command prompt.
- In scripted mode (stdin is redirected or `--test` is passed) the programme prints only the semantic results of commands.

This separation prevents the frequent failure mode in automated marking where prompts and decorative output pollute golden files.

### 6.3 Correctness argument sketch

A minimal correctness argument can be organised around invariant preservation.

- Base case: at creation `front = rear = 0` and `count = 0` so all invariants hold.
- Enqueue step: if not full then the write at `rear` is within bounds and `rear ← next(rear)` remains within bounds. Incrementing `count` preserves `0 ≤ count ≤ capacity`.
- Dequeue step: if not empty then the read at `front` is within bounds and `front ← next(front)` remains within bounds. Decrementing `count` preserves `0 ≤ count`.

FIFO ordering follows because the only removal point is `front` and `front` advances exactly once per successful dequeue, while enqueued elements are written at successive `rear` positions.

## 7. Exercise 2: `src/exercise2.c` (round-robin scheduler)

### 7.1 Process model

Each process is represented by:

- `pid` and `name` for identification
- `burst_time` and `remaining_time` to model required CPU service
- `arrival_time` to model release into the ready set
- `start_time`, `completion_time`, `turnaround_time`, `waiting_time` as derived metrics

The simulation is discrete in units of milliseconds. The quantum is a positive integer number of milliseconds.

### 7.2 Scheduler as a queue-driven state machine

Round-robin scheduling can be described as repeated application of a transition rule over the global state:

\[
(t, Ready, P) \rightarrow (t', Ready', P')
\]

where `Ready` is a FIFO queue of process indices.

A single scheduling step performs:

1. Admit all processes with `arrival_time ≤ t` that are not yet completed.
2. If the queue is empty but there exists a future arrival then jump time to the next arrival.
3. Dequeue the front process.
4. Run it for `min(quantum, remaining_time)`.
5. Update `t` and `remaining_time`.
6. If unfinished then re-enqueue it, otherwise finalise metrics.

### 7.3 Pseudocode

```
procedure ROUND_ROBIN(processes, quantum)
    sort processes by (arrival_time, pid)

    Ready ← empty queue
    t ← 0
    completed ← 0
    next ← 0

    while completed < N do
        while next < N and processes[next].arrival_time ≤ t do
            Ready.enqueue(next)
            next ← next + 1
        end while

        if Ready.empty() then
            if next < N then
                t ← processes[next].arrival_time
                continue
            else
                break
            end if
        end if

        i ← Ready.dequeue()
        p ← processes[i]

        if p.start_time = −1 then
            p.start_time ← t
        end if

        slice ← min(quantum, p.remaining_time)
        t ← t + slice
        p.remaining_time ← p.remaining_time − slice

        while next < N and processes[next].arrival_time ≤ t do
            Ready.enqueue(next)
            next ← next + 1
        end while

        if p.remaining_time > 0 then
            Ready.enqueue(i)
        else
            p.completion_time ← t
            p.turnaround_time ← t − p.arrival_time
            p.waiting_time ← p.turnaround_time − p.burst_time
            completed ← completed + 1
        end if
    end while
end procedure
```

Two implementation choices in this repository are worth noticing:

- The ready queue stores **indices**, not pointers. This avoids ownership ambiguity and makes it easy to store processes in a fixed array.
- Sorting ties by PID ensures deterministic behaviour when multiple processes arrive at the same time. A stable result matters for reproducible tests.

### 7.4 Test mode

`exercise2` supports a minimal mode:

```bash
./exercise2 tests/test2_input.txt 2 --test
```

In this mode it prints the loaded processes and average statistics only. It still runs the full scheduler internally, therefore correctness is unchanged. Only the presentation layer is reduced.

## 8. Build, run and test

### 8.1 Compilation

```bash
make
```

### 8.2 Interactive execution

```bash
make run-ex1
make run-ex2
```

### 8.3 Automated tests

The `Makefile` implements golden-file tests:

```bash
make test
```

Exercise 1 uses redirected stdin and compares output against `tests/test1_expected.txt`.

Exercise 2 runs in `--test` mode and compares output against `tests/test2_expected.txt`.

### 8.4 Memory checking

If you have `valgrind` installed:

```bash
make valgrind
make valgrind-ex1
make valgrind-ex2
```

## 9. Notes on the slide decks

The HTML slide decks are teaching artefacts rather than executable programmes. They provide:

- A conceptual explanation of FIFO discipline and its contrast with LIFO.
- A comparison between array-backed and linked-list-backed queues.
- A worked round-robin scheduling trace that links the abstract queue to a systems application.

For reproducibility, prefer the code in `src/` as the reference implementation.

## 9.1 Analytical appendix: representation invariants and correctness arguments

### 9.1.1 Algebraic view of the queue ADT

The queue can be described as an abstract sequence Q = ⟨q0, q1, …, q{n-1}⟩ with the following partial operations:

- `enqueue(Q, x)` yields Q′ = ⟨q0, …, q{n-1}, x⟩
- `dequeue(Q)` yields (q0, ⟨q1, …, q{n-1}⟩) when n > 0
- `peek(Q)` yields q0 when n > 0
- `size(Q)` yields n

A correct concrete implementation must establish a simulation relation between (front, rear, count, data[]) and the abstract sequence. In this laboratory unit the relation is most conveniently expressed by a derived indexing function:

- `abs_index(i) = (front + i) mod capacity` for i ∈ [0, count)
- `Q[i] = data[abs_index(i)]`

The *representation invariants* then become the proof obligations that must hold after every operation:

1. `0 ≤ count ≤ capacity`
2. `0 ≤ front < capacity` and `0 ≤ rear < capacity`
3. `rear = (front + count) mod capacity`
4. For all i in [0, count) the element at `data[abs_index(i)]` equals the abstract element Q[i]

The use of an explicit `count` field is not cosmetic. It provides an unambiguous empty state (`count = 0`) and an unambiguous full state (`count = capacity`) without reserving a sentinel slot. Alternative designs exist but they move the ambiguity elsewhere.

### 9.1.2 Inductive proof sketch for circular enqueue and dequeue

A suitable correctness argument for `enqueue` and `dequeue` is an induction over the length of an operation trace. The inductive hypothesis is that the representation invariants hold before an operation. The step case then shows they hold afterwards.

For `enqueue` with item x:

- Precondition: `count < capacity`.
- Store: `data[rear] ← x`.
- Update: `rear ← (rear + 1) mod capacity` and `count ← count + 1`.

Invariant (3) after the update follows from algebra:

- Old: `rear_old = (front + count_old) mod capacity`.
- New: `rear_new = (rear_old + 1) mod capacity = (front + count_old + 1) mod capacity = (front + count_new) mod capacity`.

For `dequeue`:

- Precondition: `count > 0`.
- Read: `x ← data[front]`.
- Update: `front ← (front + 1) mod capacity` and `count ← count - 1`.

Invariant (3) holds because the new rear remains the end of the abstract sequence and the new front points to the former second element.

These sketches are short because the data representation is intentionally simple. In more elaborate designs (for example queues with resizing) the invariants become richer and the proof requires a stronger simulation relation.

### 9.1.3 Alternative ring-buffer designs and when to use them

The present code uses (front, rear, count). Two common alternatives appear frequently in systems code:

1. **One-slot-empty convention**
   - Maintain front and rear only.
   - Declare the buffer full when `(rear + 1) mod capacity == front`.
   - Effective capacity becomes `capacity - 1`.

2. **Monotone indices with masking**
   - Maintain `front` and `rear` as monotonically increasing unsigned integers.
   - Let the physical index be `front & (capacity - 1)` when capacity is a power of two.
   - This avoids the modulo operator and can be beneficial in high-rate producer–consumer code but it imposes a strong constraint on the chosen capacity.

Each alternative is a trade-off between conceptual simplicity, throughput, memory footprint and debuggability. For an introductory course the (front, rear, count) model provides the most direct correspondence with the ADT specification.

## 9.2 Cross-language correspondences

The laboratory exercises are implemented in C to make memory layout, pointer discipline and invariants explicit. Conceptually the same designs exist in higher-level languages though the failure modes change.

### 9.2.1 Python

Python’s `collections.deque` implements a double-ended queue with amortised O(1) insertion and removal at both ends. A FIFO queue is therefore immediate:

```python
from collections import deque

q = deque()
q.append(10)      # enqueue
x = q.popleft()   # dequeue
```

If one insists on an explicit circular buffer for didactic symmetry, the structure mirrors the C version:

```python
class CircularQueue:
    def __init__(self, capacity: int):
        self.data = [None] * capacity
        self.front = 0
        self.rear = 0
        self.count = 0
        self.capacity = capacity

    def enqueue(self, x):
        if self.count == self.capacity:
            raise OverflowError
        self.data[self.rear] = x
        self.rear = (self.rear + 1) % self.capacity
        self.count += 1

    def dequeue(self):
        if self.count == 0:
            raise IndexError
        x = self.data[self.front]
        self.front = (self.front + 1) % self.capacity
        self.count -= 1
        return x
```

### 9.2.2 C++

The standard library provides `std::queue` (typically backed by `std::deque`). The key conceptual point is that the interface enforces FIFO but does not expose the representation:

```cpp
#include <queue>

std::queue<int> q;
q.push(10);
int x = q.front();
q.pop();
```

### 9.2.3 Java

`ArrayDeque<E>` is the canonical non-concurrent FIFO structure in modern Java. It is array-backed and uses an internal circular buffer:

```java
import java.util.ArrayDeque;

ArrayDeque<Integer> q = new ArrayDeque<>();
q.addLast(10);   // enqueue
int x = q.removeFirst();  // dequeue
```

The pedagogical point is not that one should reimplement `ArrayDeque` in production but that the invariants studied here are exactly the invariants that make library implementations reliable.

## 9.3 BFS as a queue-first graph traversal

The complete example includes a BFS demonstration to emphasise that the FIFO discipline is not an arbitrary choice but a correctness condition: the queue realises exploration in non-decreasing distance from the start vertex.

Pseudocode (parent pointers omitted for brevity):

```
BFS(G, s):
    for each vertex v in G:
        visited[v] = false
    Q = empty queue
    visited[s] = true
    enqueue(Q, s)

    while Q not empty:
        v = dequeue(Q)
        for each neighbour w of v:
            if not visited[w]:
                visited[w] = true
                enqueue(Q, w)
```

The complexity is O(|V| + |E|) when adjacency lists are used and each vertex is enqueued at most once. A queue that violates FIFO can break the layer-by-layer invariant and therefore destroy shortest-path guarantees in unweighted graphs.

## 9.4 Test design notes

The `tests/` directory contains *golden files* rather than property tests. This is intentional. For an introductory laboratory the most common failure modes are formatting mistakes, off-by-one errors in wraparound logic and incorrect handling of empty or full states. Golden tests detect these failures early and deterministically.

A robust personal test suite should additionally include:

- **Wraparound stress**: enqueue to capacity, dequeue half, enqueue again and confirm index arithmetic wraps correctly.
- **Underflow and overflow accounting**: issue invalid operations and verify that counters change but the queue state does not.
- **Scheduler tie cases**: processes with identical arrival times, identical burst times and identical PIDs should still yield deterministic output. This is why the implementation sorts by arrival time and then PID.

If you extend the scheduler to accept dynamic arrivals from stdin, introduce a fixed seed for random arrivals during testing and record the seed in the output so that a trace is reproducible.

## 10. References

| APA (7th ed) reference | DOI |
|---|---|
| Dijkstra, E. W. (1968). The structure of the “THE”-multiprogramming system. *Communications of the ACM, 11*(5), 341–346. | https://doi.org/10.1145/363095.363143 citeturn0search2turn0search13 |
| Hoare, C. A. R. (1978). Communicating sequential processes. *Communications of the ACM, 21*(8), 666–677. | https://doi.org/10.1145/359576.359585 citeturn1search0 |
| Lee, C. Y. (1961). An algorithm for path connections and its applications. *IRE Transactions on Electronic Computers, EC-10*(3), 346–365. | https://doi.org/10.1109/TEC.1961.5219222 citeturn1search8 |
| Little, J. D. C. (1961). A proof for the queuing formula: L = λW. *Operations Research, 9*(3), 383–387. | https://doi.org/10.1287/opre.9.3.383 citeturn0search0 |
| Michael, M. M., & Scott, M. L. (1996). Simple, fast and practical non-blocking and blocking concurrent queue algorithms. In *Proceedings of the Fifteenth Annual ACM Symposium on Principles of Distributed Computing* (pp. 267–275). ACM. | https://doi.org/10.1145/248052.248106 citeturn0search1 |


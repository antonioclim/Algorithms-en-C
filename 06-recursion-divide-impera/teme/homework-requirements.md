# Week 06 Homework: Queue-Centred Design Tasks

## 1. Administrative frame

- **Submission horizon:** End of Week 07 (local course calendar)
- **Maximum score:** 100 points (10% of the overall module mark)
- **Implementation language:** ISO C11
- **Toolchain expectation:** GCC or Clang with `-Wall -Wextra -std=c11` and no warnings
- **Deliverable granularity:** One `.c` source file per homework task
- **Integrity requirements:** Original work only; any form of unauthorised code reuse is treated as academic misconduct

The homework set is designed to force an explicit mapping between an *abstract specification* (queue ADT and its invariants) and a *concrete program* that is testable, memory-safe and deterministic under controlled inputs.

---

## 2. Shared technical constraints

### 2.1 Queue implementation constraints

You must implement the queue as an array-backed circular buffer. Linked lists are not permitted for the core queue used for scoring, even if you implement a linked list version as an auxiliary structure for exploratory purposes.

A recommended representation is the familiar five-tuple:

- `data`: dynamically allocated array of elements
- `front`: index of the element that would be returned by `dequeue`
- `rear`: index of the next free slot for `enqueue`
- `count`: number of elements currently stored
- `capacity`: maximum number of elements addressable by the queue

This representation is advantageous because it yields a crisp invariant and a constant-time method to disambiguate empty from full:

- **Empty:** `count == 0`
- **Full:** `count == capacity`

### 2.2 Mandatory robustness properties

Your programme must satisfy all of the following:

- **Total correctness for valid inputs:** For all inputs that satisfy the stated format constraints your programme must terminate and produce the stated outputs.
- **Graceful failure for invalid inputs:** If input files cannot be opened or are malformed the programme must exit with a non-zero status and a short diagnostic on `stderr`.
- **No memory leaks:** Your programme must be clean under Valgrind with `--leak-check=full`.
- **Deterministic behaviour under test mode:** If randomness is required you must provide a deterministic override (see §3.3).

### 2.3 Required documentation inside source files

At the top of each `.c` file include a comment header containing:

- Your name and student identifier
- A concise specification of the implemented algorithm
- Representation invariants for your queue
- A description of worst-case time complexity for the principal operations

---

## 3. Homework 1: Hot Potato elimination simulation (50 points)

### 3.1 Problem statement

Simulate the children’s game “Hot Potato” using a circular queue. Players form a conceptual ring. In each round the potato is passed a number of times. The player who ends up holding it is eliminated. The game continues until one player remains.

The goal is not a graphical interface but a well-specified state machine whose evolution can be audited via printed traces.

### 3.2 Functional requirements

1. **Player model (5 points)**
   - Define a `Player` structure containing an `id` (integer) and a `name` (fixed-length character array, at least 32 bytes).
   - Provide a stable mapping between player identifiers and names.

2. **Queue model (15 points)**
   - The queue stores *player identifiers* or indices into a player array.
   - `enqueue` and `dequeue` must be O(1) in the worst case.
   - Passing the potato is represented by repeated `dequeue` followed by `enqueue`.

3. **Simulation core (15 points)**
   - Read player names from a file or from command-line arguments.
   - For each round choose a pass count `k` in the range `[1, 10]`.
   - Execute exactly `k - 1` passes, then eliminate the next dequeued player.
   - Print a clear trace line per round.

4. **Reporting (10 points)**
   - Print the elimination order.
   - Print the winner.
   - Print at least: number of rounds, total passes.

5. **Error handling and memory safety (5 points)**
   - Invalid empty player sets are rejected.
   - Queue underflow must never occur under valid inputs.

### 3.3 Deterministic testing interface

Randomness is pedagogically useful but makes automated marking fragile. You must therefore support a deterministic override, for example:

- An optional command-line argument `--seed <integer>` that seeds a pseudo-random generator
- An optional command-line argument `--passes <file>` that provides an explicit pass count per round

Any one of these is acceptable provided it allows exact reproduction of a run.

### 3.4 Reference pseudocode

```text
HOT_POTATO(names[0..n-1])
    Q <- new queue of capacity n
    for i in 0..n-1
        enqueue(Q, i)

    round <- 0
    total_passes <- 0
    while size(Q) > 1
        round <- round + 1
        k <- next_pass_count()          // in [1, 10]

        for t in 1..(k-1)
            x <- dequeue(Q)
            enqueue(Q, x)
            total_passes <- total_passes + 1

        eliminated <- dequeue(Q)
        print(round, names[eliminated], k)

    winner <- dequeue(Q)
    print(winner, round, total_passes)
```

### 3.5 Example invocation

```bash
./homework1_hotpotato players.txt --seed 12345
```

---

## 4. Homework 2: Priority-aware print queue manager (50 points)

### 4.1 Problem statement

Implement a print service that receives print jobs over time and processes them according to a strict priority policy. The policy is:

1. Always process the highest priority queue that is non-empty.
2. Within a priority level preserve FIFO order.

This is a minimal model of real print spooling systems where fairness is constrained by service class.

### 4.2 Functional requirements

1. **Job representation (5 points)**
   - Define a `PrintJob` structure containing:
     - `job_id` (unique integer)
     - `filename` (string buffer)
     - `pages` (positive integer)
     - `priority` (enumeration with at least HIGH, MEDIUM and LOW)
     - `arrival_time`, `start_time`, `completion_time`

2. **Multi-queue architecture (15 points)**
   - Maintain three circular queues, one per priority.
   - `add_job` routes to the correct queue.
   - `get_next_job` selects the correct queue under the priority policy.

3. **Discrete-time simulation (15 points)**
   - Input file lines define arriving jobs including arrival times.
   - Printing consumes one page per time unit.
   - Jobs can arrive while another job is printing.
   - The simulation terminates when all jobs have been processed.

4. **Statistics (10 points)**
   - Mean waiting time per priority level.
   - Total completion time.
   - Completion order.
   - At least one queue utilisation statistic such as average queue length.

5. **Interactive interface (5 points)**
   - Support adding jobs during simulation via `stdin`.
   - Support `STATUS` to print queue states.
   - Support `QUIT` to terminate early.

### 4.3 Algorithmic outline

The simulation is an event loop over discrete time. At each time step:

1. Add all jobs whose `arrival_time == current_time` into their priority queue.
2. If no job is currently printing select the next job by priority.
3. Decrement the remaining pages of the current job.
4. If it reaches zero set its completion time and mark the printer as idle.

### 4.4 Reference pseudocode

```text
PRINT_MANAGER(jobs)
    sort jobs by arrival_time then by job_id
    QH, QM, QL <- three circular queues
    t <- 0
    i <- 0                      // next job to arrive
    current <- NONE

    while i < |jobs| or current != NONE or not empty(QH) or not empty(QM) or not empty(QL)
        while i < |jobs| and jobs[i].arrival_time == t
            enqueue(queue_for_priority(jobs[i]), jobs[i])
            i <- i + 1

        if current == NONE
            current <- dequeue_first_non_empty(QH, QM, QL)
            if current != NONE and current.start_time is unset
                current.start_time <- t

        if current != NONE
            current.pages_remaining <- current.pages_remaining - 1
            if current.pages_remaining == 0
                current.completion_time <- t + 1
                record_completion(current)
                current <- NONE

        t <- t + 1
```

### 4.5 Example file format

```text
# JobID Filename Pages Priority ArrivalTime
1 report.pdf 10 HIGH 0
2 image.png 2 LOW 0
3 thesis.pdf 50 MEDIUM 5
4 urgent.doc 5 HIGH 8
```

---

## 5. Evaluation rubric

| Criterion | Points |
|---|---:|
| Functional correctness (conformance to specification) | 40 |
| Queue design quality (correct invariants and O(1) operations) | 25 |
| Edge case handling and robustness | 15 |
| Code clarity and internal documentation | 10 |
| Clean compilation and memory safety | 10 |

Automatic deductions may be applied for compiler warnings, memory leaks, crashes on valid inputs or failure to compile.

---

## 6. References

| APA (7th ed) reference | DOI |
|---|---|
| Little, J. D. C. (1961). A proof for the queuing formula: L = λW. *Operations Research, 9*(3), 383–387. | https://doi.org/10.1287/opre.9.3.383 citeturn0search0 |
| Michael, M. M., & Scott, M. L. (1996). Simple, fast and practical non-blocking and blocking concurrent queue algorithms. In *Proceedings of the Fifteenth Annual ACM Symposium on Principles of Distributed Computing* (pp. 267–275). ACM. | https://doi.org/10.1145/248052.248106 citeturn0search1 |

# Week 06 Homework: Queues

## 📋 General Information

- **Deadline:** End of Week 07
- **Points:** 100 (10% of final grade)
- **Language:** C (C11 standard)
- **Compiler:** GCC with `-Wall -Wextra -std=c11`
- **Submission:** Single `.c` file per homework via university portal

---

## 📝 Homework 1: Hot Potato Game Simulation (50 points)

### Description

Implement a simulation of the "Hot Potato" children's game using a circular queue. In this game, players stand in a circle and pass an object (the "hot potato") around. When the music stops (after a random number of passes), the player holding the potato is eliminated. The game continues until only one player remains.

### Requirements

1. **Player Structure (5p)**
   - Define a `Player` structure with fields: `id` (int), `name` (char[32]) and `is_active` (bool)
   - Support at least 20 players

2. **Queue Implementation (15p)**
   - Implement a circular queue for player IDs
   - Must use circular buffer (array-based) implementation
   - Support `enqueue`, `dequeue`, `peek` and `size` operations
   - Handle the circular nature correctly with modulo arithmetic

3. **Game Logic (15p)**
   - Read player names from a file or command line
   - Simulate passing the potato with a random count (1-10) each round
   - Remove the eliminated player and continue
   - Display each round's result

4. **Output (10p)**
   - Print the elimination order
   - Announce the winner
   - Show statistics: total rounds, total passes

5. **Error Handling (5p)**
   - Handle file errors gracefully
   - Validate input data
   - Prevent memory leaks

### Example Usage

```bash
./homework1_hotpotato players.txt
```

**players.txt:**
```
Alice
Bob
Charlie
Diana
Eve
```

**Expected Output:**
```
=== Hot Potato Game ===
Players: Alice, Bob, Charlie, Diana, Eve

Round 1: Passed 7 times - Diana eliminated!
Round 2: Passed 3 times - Bob eliminated!
Round 3: Passed 9 times - Eve eliminated!
Round 4: Passed 5 times - Alice eliminated!

Winner: Charlie!

Statistics:
  Total rounds: 4
  Total passes: 24
```

### File: `homework1_hotpotato.c`

---

## 📝 Homework 2: Print Queue Manager (50 points)

### Description

Implement a print queue management system that handles print jobs with different priorities. The system should use multiple queues (high, medium, low priority) and process jobs in order of priority whilst maintaining FIFO within each priority level.

### Requirements

1. **PrintJob Structure (5p)**
   - Define a `PrintJob` structure with:
     - `job_id` (int) - unique identifier
     - `filename` (char[64]) - document name
     - `pages` (int) - number of pages
     - `priority` (enum: HIGH, MEDIUM, LOW)
     - `submitted_time` (int) - simulation time unit when submitted
     - `started_time` (int) - when printing began
     - `completed_time` (int) - when printing finished

2. **Multi-Queue System (15p)**
   - Implement three separate queues (one per priority level)
   - Each queue must be a proper circular buffer implementation
   - Implement `add_job()` which routes to appropriate queue
   - Implement `get_next_job()` which returns highest priority available job

3. **Simulation (15p)**
   - Read jobs from file (with arrival times)
   - Simulate printing at 1 page per time unit
   - Process jobs in priority order
   - Handle jobs arriving during printing of another job

4. **Statistics (10p)**
   - Calculate average waiting time per priority level
   - Calculate total simulation time
   - Display job completion order
   - Show queue utilisation statistics

5. **Interactive Mode (5p)**
   - Support adding jobs during simulation via stdin
   - Support `STATUS` command to show current queue states
   - Support `QUIT` command to end simulation

### Example Usage

```bash
./homework2_printqueue jobs.txt
```

**jobs.txt format:**
```
# JobID Filename Pages Priority ArrivalTime
1 report.pdf 10 HIGH 0
2 image.png 2 LOW 0
3 thesis.pdf 50 MEDIUM 5
4 urgent.doc 5 HIGH 8
```

**Expected Output:**
```
=== Print Queue Manager ===

[Time 0] Job 1 (report.pdf) arrived - Priority: HIGH
[Time 0] Job 2 (image.png) arrived - Priority: LOW
[Time 0] Started printing Job 1 (report.pdf)
[Time 5] Job 3 (thesis.pdf) arrived - Priority: MEDIUM
[Time 8] Job 4 (urgent.doc) arrived - Priority: HIGH
[Time 10] Completed Job 1 (report.pdf)
[Time 10] Started printing Job 4 (urgent.doc)
[Time 15] Completed Job 4 (urgent.doc)
[Time 15] Started printing Job 3 (thesis.pdf)
[Time 65] Completed Job 3 (thesis.pdf)
[Time 65] Started printing Job 2 (image.png)
[Time 67] Completed Job 2 (image.png)

=== Statistics ===
Total simulation time: 67 units
Jobs processed: 4

Average wait time by priority:
  HIGH:   2.5 units
  MEDIUM: 10.0 units
  LOW:    65.0 units
```

### File: `homework2_printqueue.c`

---

## 📊 Evaluation Criteria

| Criterion | Points |
|-----------|--------|
| Functional correctness | 40 |
| Proper queue implementation (circular buffer) | 25 |
| Edge case handling | 15 |
| Code quality and documentation | 10 |
| No compiler warnings | 10 |

### Automatic Deductions

| Issue | Penalty |
|-------|---------|
| Compiler warnings | -10p |
| Memory leaks (Valgrind) | -20p |
| Crashes on valid input | -30p |
| Does not compile | -50p |
| Plagiarism | -100p + disciplinary action |

---

## 📤 Submission Guidelines

1. Submit **one** `.c` file per homework
2. Include your name and student ID in a comment at the top
3. Ensure code compiles with: `gcc -Wall -Wextra -std=c11 -o output homework.c`
4. Test with Valgrind before submission: `valgrind --leak-check=full ./output`
5. Submit via university portal before deadline

---

## 💡 Tips for Success

1. **Start with the queue implementation** - get it working perfectly before adding game/simulation logic

2. **Test incrementally** - verify each function works before moving to the next

3. **Use descriptive variable names** - `front_index` is clearer than `f`

4. **Draw diagrams** - sketch out the circular buffer states to understand wraparound

5. **Handle edge cases explicitly:**
   - Empty queue operations
   - Full queue operations
   - Single element scenarios
   - Wraparound scenarios

6. **Comment your code** - explain the circular buffer mathematics

7. **Use `const` appropriately** - mark read-only parameters as `const`

---

## 📚 Reference Materials

- Week 06 lecture slides
- `example1.c` - complete queue implementation examples
- Cormen et al., "Introduction to Algorithms", Chapter 10
- Online: [Visualgo Queue Animation](https://visualgo.net/en/list)

---

*Good luck! Remember: understanding queues thoroughly will help with BFS, scheduling and many interview problems.*

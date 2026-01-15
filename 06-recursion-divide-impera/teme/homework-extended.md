# Extended Challenges - Week 06: Queues

## 🚀 Advanced Challenges (Optional)

Each correctly solved challenge earns **+10 bonus points** (maximum +50 total).

These challenges are designed for students who want to deepen their understanding of queue data structures and explore advanced applications.

---

## ⭐ Challenge 1: Sliding Window Maximum (Difficulty: Medium)

### Description

Given an array of integers and a window size `k`, find the maximum element in each sliding window as it moves from left to right across the array.

**Twist:** You must solve this in O(n) time using a **deque** (double-ended queue).

### Requirements

1. Implement a deque data structure from scratch (not using linked list)
2. Support `push_front`, `push_back`, `pop_front`, `pop_back` operations
3. Use the deque to maintain candidates for the maximum
4. Process an array of up to 100,000 elements efficiently

### Example

```
Input:  arr = [1, 3, -1, -3, 5, 3, 6, 7], k = 3
Output: [3, 3, 5, 5, 6, 7]

Explanation:
Window [1, 3, -1]    -> max = 3
Window [3, -1, -3]   -> max = 3
Window [-1, -3, 5]   -> max = 5
Window [-3, 5, 3]    -> max = 5
Window [5, 3, 6]     -> max = 6
Window [3, 6, 7]     -> max = 7
```

### Hints

- The deque should store indices, not values
- Remove elements from back if they are smaller than current element
- Remove elements from front if they are outside the current window

### Bonus Points: +10

---

## ⭐ Challenge 2: Queue Using Two Stacks (Difficulty: Medium)

### Description

Implement a queue data structure using **only two stacks**. You cannot use any array indexing or direct queue operations.

This is a classic interview question that tests understanding of both stacks and queues.

### Requirements

1. Implement a `Stack` structure with `push`, `pop`, `peek`, `isEmpty`
2. Implement `QueueFromStacks` using exactly two Stack instances
3. Support `enqueue`, `dequeue`, `peek`, `isEmpty` operations
4. Achieve amortised O(1) time for all operations

### Algorithm Hint

- Use one stack for enqueue operations
- Use another stack for dequeue operations
- Transfer elements between stacks when necessary

### Example

```c
QueueFromStacks q;
qfs_init(&q);

qfs_enqueue(&q, 1);
qfs_enqueue(&q, 2);
qfs_enqueue(&q, 3);

printf("%d\n", qfs_dequeue(&q));  // Output: 1
printf("%d\n", qfs_dequeue(&q));  // Output: 2

qfs_enqueue(&q, 4);

printf("%d\n", qfs_dequeue(&q));  // Output: 3
printf("%d\n", qfs_dequeue(&q));  // Output: 4
```

### Bonus Points: +10

---

## ⭐ Challenge 3: Snake Game Simulation (Difficulty: Medium-Hard)

### Description

Implement the classic Snake game using a queue to represent the snake's body. The queue naturally models the snake: new positions are enqueued at the head, and old positions are dequeued from the tail.

### Requirements

1. Create a 20x20 game board
2. Use a queue of (x, y) coordinate pairs for the snake body
3. Implement movement in four directions (W, A, S, D keys)
4. Generate food at random positions
5. Grow the snake when food is eaten (don't dequeue tail)
6. Detect collisions with walls and self
7. Display the game state after each move

### Game Mechanics

- Snake starts with length 3 at position (10, 10)
- Food appears at random empty positions
- Eating food increases score and snake length
- Game ends on collision

### Example Output

```
Score: 5  Length: 8

┌────────────────────┐
│                    │
│     ●              │
│    ████            │
│       █            │
│       █            │
│       █            │
│       █            │
│                    │
│              *     │
│                    │
└────────────────────┘

Move (WASD): 
```

### Bonus Points: +10

---

## ⭐ Challenge 4: Bank Queue Simulation with Multiple Counters (Difficulty: Hard)

### Description

Simulate a bank with multiple service counters. Customers arrive randomly and are served by available counters. Implement queue balancing and statistics tracking.

### Requirements

1. Support N configurable service counters (each with its own queue)
2. Implement customer arrival with Poisson-like distribution
3. Implement service time with exponential-like distribution
4. Customer joins the shortest queue
5. Track comprehensive statistics:
   - Average waiting time
   - Counter utilisation rates
   - Maximum queue lengths
   - Customer throughput

### Configuration

```c
#define NUM_COUNTERS 3
#define SIMULATION_TIME 480  // 8 hours in minutes
#define AVG_ARRIVAL_RATE 2   // customers per minute
#define AVG_SERVICE_TIME 5   // minutes per customer
```

### Expected Output

```
=== Bank Simulation Results ===
Duration: 480 minutes
Customers served: 187

Counter Statistics:
  Counter 1: 65 customers, 89% utilisation, max queue: 7
  Counter 2: 62 customers, 85% utilisation, max queue: 6
  Counter 3: 60 customers, 82% utilisation, max queue: 5

Overall:
  Average waiting time: 4.3 minutes
  Maximum waiting time: 18 minutes
  Customers still waiting at close: 3
```

### Bonus Points: +10

---

## ⭐ Challenge 5: Lock-Free Queue (Difficulty: Expert)

### Description

Implement a thread-safe queue that does not use mutexes. Instead, use atomic operations (compare-and-swap) to ensure thread safety.

**Warning:** This is an advanced systems programming challenge. Only attempt if you are comfortable with concurrent programming concepts.

### Requirements

1. Use `<stdatomic.h>` for atomic operations
2. Implement using the Michael-Scott algorithm or similar
3. Support multiple producers and multiple consumers
4. No data races or undefined behaviour
5. Provide proof of correctness via stress testing

### Key Concepts

- Atomic compare-and-swap (CAS)
- ABA problem and solutions
- Memory ordering (acquire/release semantics)
- Hazard pointers or epoch-based reclamation

### Testing

```c
// Launch 4 producer threads, 4 consumer threads
// Each producer enqueues 100,000 items
// Verify all items are dequeued exactly once
```

### Bonus Points: +10

---

## 📊 Bonus Point System

| Challenges Completed | Total Bonus |
|---------------------|-------------|
| 1 | +10 points |
| 2 | +20 points |
| 3 | +30 points |
| 4 | +40 points |
| All 5 | +50 points + "Queue Master" badge 🏆 |

---

## 📤 Submission Requirements

1. Submit each challenge as a separate `.c` file
2. Name files: `bonus1_sliding.c`, `bonus2_twostack.c`, etc.
3. Include time complexity analysis in comments
4. Provide test cases demonstrating correctness
5. Challenges must compile without warnings

---

## 🎯 Learning Outcomes

Completing these challenges will help you:

- Master circular buffer and deque implementations
- Understand stack-queue relationships
- Apply queues to game development
- Learn discrete event simulation
- Explore lock-free concurrent programming

---

*These challenges go beyond the standard curriculum. Attempting them demonstrates initiative and deeper understanding.*

# Week 06: Queues

## 🎯 Learning Objectives

Upon completion of this laboratory, students will be able to:

1. **Remember** the fundamental characteristics of the queue abstract data type, including FIFO ordering, front and rear pointers and the core operations of enqueue and dequeue
2. **Understand** the behavioural differences between array-based and linked-list implementations, particularly regarding memory allocation strategies and pointer manipulation
3. **Apply** queue data structures to solve practical problems such as task scheduling, breadth-first traversal and buffer management in producer-consumer scenarios
4. **Analyse** the time and space complexity of queue operations across different implementations, recognising the trade-offs between static array bounds and dynamic memory allocation
5. **Evaluate** the suitability of circular buffer implementations versus linear arrays, considering cache locality, memory fragmentation and wraparound overhead
6. **Create** custom queue implementations tailored to specific application requirements, including priority queues, double-ended queues (deques) and bounded blocking queues

---

## 📜 Historical Context

The queue data structure emerged from the theoretical foundations of automata theory and formal language processing during the mid-twentieth century. The concept of ordered, sequential access—where elements depart in the same order they arrive—mirrors countless natural phenomena: customers awaiting service, messages traversing network routers and processes competing for CPU time.

Alan Turing's seminal 1936 paper on computable numbers established the tape-based model of computation that implicitly relied on sequential access patterns. However, the explicit formalisation of queues as abstract data types crystallised during the 1960s alongside the development of ALGOL and early operating systems. Edsger Dijkstra's work on the THE multiprogramming system (1968) demonstrated how queues could coordinate concurrent processes, whilst Tony Hoare's subsequent research on communicating sequential processes (1978) elevated message queues to a fundamental synchronisation primitive.

The circular buffer—a memory-efficient queue variant—gained prominence in embedded systems and real-time signal processing during the 1970s. Its elegant solution to the "false overflow" problem (where a linear array appears exhausted despite containing unused space) became standard practice in audio codecs, network interface cards and kernel-level buffer caches.

### Key Figure: Edsger W. Dijkstra (1930–2002)

Edsger Wybe Dijkstra was a Dutch computer scientist whose contributions shaped nearly every aspect of modern programming. At the Technische Hogeschool Eindhoven, he developed the THE operating system—one of the first to employ layered abstraction and structured synchronisation primitives including semaphores and process queues.

His shortest path algorithm (1956) revolutionised graph theory and network routing, whilst his later advocacy for structured programming and formal verification established rigorous standards for software correctness. Dijkstra received the Turing Award in 1972 for his fundamental contributions to programming as a high, intellectual challenge.

> *"Computer Science is no more about computers than astronomy is about telescopes."*
> — Edsger W. Dijkstra

---

## 📚 Theoretical Foundations

### 1. The Queue Abstract Data Type

A queue is a linear collection that enforces First-In-First-Out (FIFO) ordering: elements are inserted at one end (the **rear**) and removed from the opposite end (the **front**). This discipline ensures that the earliest arrival is always the next departure.

```
                    QUEUE OPERATIONS
    
         enqueue(x)                    dequeue()
              │                             │
              ▼                             ▼
    ┌─────────────────────────────────────────────┐
    │  A  │  B  │  C  │  D  │  E  │     │     │   │
    └─────────────────────────────────────────────┘
           ▲                       ▲
         front                   rear
    
    peek() returns A without removal
    isEmpty() returns false
    size() returns 5
```

**Core Operations:**

| Operation | Description | Time Complexity |
|-----------|-------------|-----------------|
| `enqueue(item)` | Insert item at rear | O(1) |
| `dequeue()` | Remove and return front item | O(1) |
| `peek()` / `front()` | Return front item without removal | O(1) |
| `isEmpty()` | Test whether queue contains no elements | O(1) |
| `size()` | Return count of elements | O(1) |

### 2. Array-Based Implementation

The simplest queue implementation uses a contiguous array with two index variables tracking the front and rear positions. However, naïve implementations suffer from **queue drift**: as elements are dequeued, the front index advances, leaving unusable space at the array's beginning.

```c
typedef struct {
    int *data;
    int front;
    int rear;
    int capacity;
    int count;
} ArrayQueue;

void enqueue(ArrayQueue *q, int item) {
    if (q->count == q->capacity) {
        /* Queue is full - cannot insert */
        return;
    }
    q->data[q->rear] = item;
    q->rear = (q->rear + 1) % q->capacity;  /* Circular wraparound */
    q->count++;
}

int dequeue(ArrayQueue *q) {
    if (q->count == 0) {
        /* Queue is empty - underflow */
        return -1;
    }
    int item = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->count--;
    return item;
}
```

### 3. Circular Buffer (Ring Buffer)

The circular buffer elegantly solves queue drift by treating the underlying array as a logical ring. When indices reach the array boundary, they wrap around to position zero via modular arithmetic.

```
              CIRCULAR BUFFER MECHANICS
    
    Linear View:    [0] [1] [2] [3] [4] [5] [6] [7]
                         ▲               ▲
                       front           rear
    
    Circular View:
    
                         ┌───┐
                    ┌────┤ 2 ├────┐
                    │    └───┘    │
                 ┌──┴─┐        ┌──┴─┐
                 │ 1  │        │ 3  │
                 └──┬─┘        └──┬─┘
                    │            │
    Empty ──►   ┌───┴──┐    ┌───┴──┐
                │  0   │    │  4   │   ◄── rear (next insert)
                └───┬──┘    └───┬──┘
                    │            │
                 ┌──┴─┐        ┌──┴─┐
                 │ 7  │        │ 5  │
                 └──┬─┘        └──┬─┘
                    │    ┌───┐    │
                    └────┤ 6 ├────┘
                         └───┘
    
    front points to oldest element
    rear points to next available slot
    Size = (rear - front + capacity) % capacity
```

**Complexity Analysis:**

| Implementation | Enqueue | Dequeue | Space | Cache Locality |
|----------------|---------|---------|-------|----------------|
| Linear Array | O(1) amortised | O(n)* | O(n) | Excellent |
| Circular Buffer | O(1) | O(1) | O(n) | Excellent |
| Linked List | O(1) | O(1) | O(n) + pointers | Poor |

*Linear dequeue requires shifting all elements

---

## 🏭 Industrial Applications

### 1. Operating System Process Scheduling

Modern operating systems maintain ready queues to schedule processes for CPU execution. The completely fair scheduler (CFS) in Linux employs red-black trees, but simpler round-robin schedulers use circular queues.

```c
/* Simplified round-robin scheduler */
typedef struct {
    pid_t processes[MAX_PROCESSES];
    int front, rear;
    int time_quantum;
} ReadyQueue;

void schedule_next(ReadyQueue *rq) {
    if (!is_empty(rq)) {
        pid_t current = dequeue(rq);
        execute_for(current, rq->time_quantum);
        if (process_remaining(current)) {
            enqueue(rq, current);  /* Re-queue if not finished */
        }
    }
}
```

### 2. Network Packet Buffering

Network interface cards and routers employ queues to buffer incoming packets during traffic bursts. Quality of Service (QoS) mechanisms often use multiple priority queues.

```c
/* Priority queue for network packets */
typedef struct {
    Queue high_priority;    /* VoIP, video conferencing */
    Queue medium_priority;  /* HTTP, general traffic */
    Queue low_priority;     /* Background downloads */
} QoSBuffer;

Packet *get_next_packet(QoSBuffer *buf) {
    if (!is_empty(&buf->high_priority))
        return dequeue(&buf->high_priority);
    if (!is_empty(&buf->medium_priority))
        return dequeue(&buf->medium_priority);
    return dequeue(&buf->low_priority);
}
```

### 3. Print Spooler Systems

Print spoolers queue documents awaiting output to physical printers, enabling asynchronous job submission and fair resource allocation.

```c
/* Print job spooler */
typedef struct {
    char filename[256];
    int pages;
    int copies;
    time_t submitted;
} PrintJob;

typedef struct {
    PrintJob jobs[MAX_JOBS];
    int front, rear, count;
} PrintQueue;
```

### 4. Breadth-First Search (BFS)

The BFS algorithm relies fundamentally on a queue to explore graph vertices level by level, ensuring the shortest path discovery in unweighted graphs.

```c
void bfs(Graph *g, int start) {
    bool visited[g->vertices];
    memset(visited, false, sizeof(visited));
    
    Queue q;
    init_queue(&q);
    
    visited[start] = true;
    enqueue(&q, start);
    
    while (!is_empty(&q)) {
        int current = dequeue(&q);
        printf("Visited: %d\n", current);
        
        for (int i = 0; i < g->adj_count[current]; i++) {
            int neighbour = g->adj_list[current][i];
            if (!visited[neighbour]) {
                visited[neighbour] = true;
                enqueue(&q, neighbour);
            }
        }
    }
}
```

### 5. Message Queues in Distributed Systems

Enterprise systems like Apache Kafka, RabbitMQ and Amazon SQS use persistent message queues for asynchronous communication between microservices.

```c
/* Producer-consumer pattern */
typedef struct {
    Message buffer[BUFFER_SIZE];
    int front, rear, count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} BlockingQueue;
```

---

## 💻 Laboratory Exercises

### Exercise 1: Circular Buffer Implementation

Implement a complete circular buffer queue supporting integers with the following requirements:

1. Create the `CircularQueue` structure with appropriate fields
2. Implement `cq_init()` to initialise a queue with given capacity
3. Implement `cq_enqueue()` with overflow detection
4. Implement `cq_dequeue()` with underflow detection
5. Implement `cq_peek()` to view the front element
6. Implement `cq_is_empty()` and `cq_is_full()` predicates
7. Implement `cq_size()` to return current element count
8. Implement `cq_destroy()` to release allocated memory
9. Create a visualisation function showing queue state

**Input:** Commands from stdin (ENQUEUE n, DEQUEUE, PEEK, SIZE, DISPLAY)  
**Output:** Operation results and queue visualisation

### Exercise 2: Task Scheduler Simulation

Build a round-robin task scheduler using a queue of process structures:

1. Define `Process` structure with pid, name, burst_time and remaining_time
2. Implement queue operations for Process pointers
3. Create `load_processes()` to read from file
4. Implement `run_scheduler()` with configurable time quantum
5. Track and display completion times, turnaround times and waiting times
6. Generate Gantt chart visualisation of execution order
7. Calculate average turnaround and waiting times
8. Support adding new processes during simulation

**Input:** Process file and time quantum  
**Output:** Execution trace, statistics and Gantt chart

---

## 🔧 Compilation and Execution

```bash
# Build all targets
make

# Build individual files
make example1
make exercise1
make exercise2

# Run the complete demonstration
make run

# Execute automated tests
make test

# Check for memory leaks
make valgrind

# Clean build artifacts
make clean

# Display help
make help
```

**Compiler Flags:**
- `-Wall` : Enable all warnings
- `-Wextra` : Enable extra warnings
- `-std=c11` : Use C11 standard
- `-g` : Include debugging symbols

---

## 📁 Directory Structure

```
week-06-queues/
├── README.md                           # This documentation
├── Makefile                            # Build automation
│
├── slides/
│   ├── presentation-week06.html        # Main lecture (40 slides)
│   └── presentation-comparativ.html    # Language comparison (12 slides)
│
├── src/
│   ├── example1.c                      # Complete queue demonstration
│   ├── exercise1.c                     # Circular buffer exercise
│   └── exercise2.c                     # Task scheduler exercise
│
├── data/
│   ├── processes.txt                   # Sample process data
│   └── commands.txt                    # Queue command sequence
│
├── tests/
│   ├── test1_input.txt                 # Test input for exercise 1
│   ├── test1_expected.txt              # Expected output for exercise 1
│   ├── test2_input.txt                 # Test input for exercise 2
│   └── test2_expected.txt              # Expected output for exercise 2
│
├── teme/
│   ├── homework-requirements.md        # Homework assignments
│   └── homework-extended.md            # Bonus challenges
│
└── solution/
    ├── exercise1_sol.c                 # Exercise 1 solution
    ├── exercise2_sol.c                 # Exercise 2 solution
    ├── homework1_sol.c                 # Homework 1 solution
    └── homework2_sol.c                 # Homework 2 solution
```

---

## 📖 Recommended Reading

### Essential

- Cormen, T.H., Leiserson, C.E., Rivest, R.L. & Stein, C. (2022). *Introduction to Algorithms* (4th ed.). MIT Press. Chapter 10: Elementary Data Structures
- Sedgewick, R. & Wayne, K. (2011). *Algorithms* (4th ed.). Addison-Wesley. Section 1.3: Bags, Queues, and Stacks
- Kernighan, B.W. & Ritchie, D.M. (1988). *The C Programming Language* (2nd ed.). Prentice Hall

### Advanced

- Herlihy, M. & Shavit, N. (2012). *The Art of Multiprocessor Programming*. Morgan Kaufmann. Chapter 10: Concurrent Queues
- Tanenbaum, A.S. & Bos, H. (2014). *Modern Operating Systems* (4th ed.). Pearson. Chapter 2: Processes and Threads

### Online Resources

- [GeeksforGeeks: Queue Data Structure](https://www.geeksforgeeks.org/queue-data-structure/)
- [Visualgo: Queue Visualisation](https://visualgo.net/en/list)
- [Linux Kernel Circular Buffer](https://www.kernel.org/doc/html/latest/core-api/circular-buffers.html)

---

## ✅ Self-Assessment Checklist

Before submitting your work, verify that you can:

- [ ] Explain the FIFO principle and contrast it with LIFO (stack) behaviour
- [ ] Implement a queue using both arrays and linked lists
- [ ] Describe the circular buffer technique and its advantages
- [ ] Calculate the number of elements using front and rear indices with modular arithmetic
- [ ] Distinguish between queue full and queue empty conditions in circular implementations
- [ ] Apply queues to solve BFS traversal problems
- [ ] Analyse the time complexity of all queue operations
- [ ] Implement a producer-consumer pattern using queues
- [ ] Debug common queue errors: off-by-one, boundary wraparound and memory leaks
- [ ] Design a priority queue using multiple standard queues

---

## 💼 Interview Preparation

### Common Interview Questions

1. **"Implement a queue using two stacks."**
   
   *Hint:* Use one stack for enqueue operations and another for dequeue. Transfer elements between stacks when necessary.

2. **"How would you implement a circular queue? What are the edge cases?"**
   
   *Key points:* Wraparound with modulo operator, distinguishing empty vs full (use count or waste one slot), handling capacity of 1.

3. **"Design a queue that supports getMin() in O(1) time."**
   
   *Hint:* Maintain an auxiliary data structure (deque) tracking minimum candidates.

4. **"Explain the difference between a blocking queue and a non-blocking queue."**
   
   *Key concepts:* Thread synchronisation, condition variables, lock-free algorithms.

5. **"How would you implement a queue with a maximum size that discards the oldest elements when full?"**
   
   *Approach:* Circular buffer with automatic overwrite when rear catches front.

---

## 🔗 Next Week Preview

**Week 07: Binary Trees**

Next week introduces hierarchical data structures through binary trees. You will learn about tree terminology (root, leaf, height, depth), traversal algorithms (preorder, inorder, postorder and level-order) and recursive tree construction. This builds directly upon queue knowledge, as level-order traversal uses BFS with a queue.

Key topics include:
- Binary tree node structure and memory layout
- Recursive vs iterative traversal implementations
- Tree height and balance calculations
- Expression tree construction and evaluation

---

*Prepared for ATP - Algorithms and Programming Techniques*  
*Academy of Economic Studies, Bucharest*  
*Week 06: Queues*

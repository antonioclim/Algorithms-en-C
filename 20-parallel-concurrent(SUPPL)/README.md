# Week 20: Parallel and Concurrent Programming

## 🎯 Learning Objectives

Upon successful completion of this laboratory, students will be able to:

1. **Remember** the fundamental terminology of concurrent programming including threads, mutexes, semaphores, race conditions and critical sections, recalling their definitions and standard implementations using POSIX threads.

2. **Understand** the mathematical principles underlying parallel execution models, explaining why certain synchronisation mechanisms prevent race conditions whilst others introduce deadlock.

3. **Apply** mutex locks, semaphores and condition variables to implement thread-safe data structures capable of correct concurrent access by multiple threads.

4. **Analyse** the performance characteristics of parallel algorithms using Amdahl's Law, determining speedup limits and optimal thread counts for given workloads.

5. **Evaluate** trade-offs between different synchronisation strategies, selecting appropriate primitives based on contention patterns, fairness requirements and performance constraints.

6. **Create** complete parallel implementations of classical algorithms including parallel merge sort, producer-consumer systems and thread pools suitable for real-world applications.

---

## 📜 Historical Context

The pursuit of parallel computation predates electronic computers themselves. Charles Babbage's Analytical Engine (1837) conceptually supported parallel operations through its "anticipating carriage" mechanism. However, practical concurrent programming emerged only with the development of time-sharing systems in the 1960s, when multiple users first competed for shared computational resources.

The seminal contributions to concurrency theory arrived from Edsger Dijkstra, whose 1965 paper "Cooperating Sequential Processes" introduced semaphores and identified the mutual exclusion problem. His famous "Dining Philosophers" problem (1965) elegantly captured the challenges of resource allocation among competing processes—a thought experiment that continues to illuminate deadlock conditions for students today. Dijkstra's work established that concurrent programming demanded fundamentally different reasoning from sequential code: the interleaving of operations created combinatorial state spaces that defied traditional debugging approaches.

C.A.R. Hoare's concurrent programming contributions complemented Dijkstra's foundational work. His 1974 paper "Monitors: An Operating System Structuring Concept" proposed a higher-level synchronisation abstraction that encapsulated shared data with the operations upon it—a design pattern that influenced Java's synchronised methods and modern concurrent data structures. Hoare's Communicating Sequential Processes (CSP, 1978) offered an alternative model based on message passing rather than shared memory, inspiring languages from occam to Go.

The transformation from single-core to multi-core processors beginning around 2005 fundamentally changed parallel programming's significance. Where previously parallelism was the domain of specialised high-performance computing, multi-core architectures made concurrent execution unavoidable for any software seeking to utilise available hardware. This shift—sometimes called "the end of the free lunch"—demanded that ordinary programmers master concepts once reserved for specialists.

### Key Figure: Edsger W. Dijkstra (1930–2002)

Edsger Wybe Dijkstra ranks among the most influential computer scientists of the twentieth century. Born in Rotterdam, he initially studied physics before discovering his passion for the nascent field of programming. His contributions span algorithms (Dijkstra's shortest path algorithm), programming methodology (structured programming), concurrency (semaphores, mutual exclusion) and distributed systems (self-stabilisation).

Dijkstra's approach to computing emphasised mathematical rigour and clarity. He famously advocated for programs as mathematical objects amenable to formal reasoning, rejecting the prevailing "debugging culture" that treated testing as sufficient verification. His handwritten EWD manuscripts—over 1300 technical notes circulated among colleagues—established a unique form of scientific communication that combined accessibility with depth.

The Dijkstra Prize, awarded annually for outstanding papers on the principles of distributed computing, honours his legacy in concurrent and distributed systems.

> *"Simplicity is prerequisite for reliability."*
> — Edsger W. Dijkstra, EWD498

---

## 📚 Theoretical Foundations

### 1. Processes vs Threads: The Concurrency Spectrum

A **process** represents an independent execution context with its own address space, file descriptors and system resources. Creating a process (via `fork()`) duplicates the parent's memory—an expensive operation measured in milliseconds. Communication between processes requires explicit mechanisms: pipes, shared memory segments or network sockets.

A **thread** represents a lightweight execution context sharing its parent process's address space. Thread creation requires only stack allocation and register context—operations measured in microseconds. Threads communicate through shared variables, making cooperation natural but synchronisation essential.

```
Process Architecture                Thread Architecture
┌─────────────────────┐            ┌─────────────────────────────────────┐
│ Process A           │            │ Process with Multiple Threads       │
│ ┌─────────────────┐ │            │                                     │
│ │ Address Space   │ │            │ ┌───────────────────────────────┐   │
│ │ Code + Data     │ │            │ │     Shared Address Space      │   │
│ └─────────────────┘ │            │ │     Code + Heap + Globals     │   │
│ ┌─────────────────┐ │            │ └───────────────────────────────┘   │
│ │ Stack           │ │            │ ┌───────┐ ┌───────┐ ┌───────┐      │
│ └─────────────────┘ │            │ │Stack 1│ │Stack 2│ │Stack 3│      │
│ ┌─────────────────┐ │            │ │Thread1│ │Thread2│ │Thread3│      │
│ │ File Descriptors│ │            │ └───────┘ └───────┘ └───────┘      │
│ └─────────────────┘ │            │                                     │
└─────────────────────┘            │ ┌───────────────────────────────┐   │
                                   │ │   Shared File Descriptors     │   │
┌─────────────────────┐            │ └───────────────────────────────┘   │
│ Process B           │            └─────────────────────────────────────┘
│ (Independent)       │
└─────────────────────┘
```

### 2. Race Conditions and Critical Sections

A **race condition** occurs when the correctness of a computation depends on the relative timing of concurrent operations. Consider incrementing a shared counter:

```c
/* Thread 1 */              /* Thread 2 */
temp1 = counter;            temp2 = counter;
temp1 = temp1 + 1;          temp2 = temp2 + 1;
counter = temp1;            counter = temp2;
```

If `counter` initially equals 0 and both threads execute concurrently, the final value might be 1 (not 2) if both read the initial value before either writes. This **lost update** exemplifies the fundamental hazard of unsynchronised shared memory access.

A **critical section** is a code region accessing shared resources that must not be executed concurrently by multiple threads. The requirements for correct critical section implementation are:

1. **Mutual Exclusion:** At most one thread executes the critical section at any time
2. **Progress:** If no thread is in the critical section, a waiting thread must eventually enter
3. **Bounded Waiting:** A thread cannot wait indefinitely to enter the critical section

```
Thread Interleaving Problem
───────────────────────────────────────────────────────────────────
Time    Thread 1             Thread 2             counter value
───────────────────────────────────────────────────────────────────
t0                                                0 (initial)
t1      read counter (0)                          0
t2                           read counter (0)     0
t3      add 1 → temp=1                            0
t4                           add 1 → temp=1       0
t5      write temp=1                              1
t6                           write temp=1         1  ← WRONG!
───────────────────────────────────────────────────────────────────
Expected: 2    Actual: 1    (Lost update due to race condition)
```

### 3. POSIX Threads (pthreads) API

The POSIX threads specification (IEEE 1003.1c) provides a portable threading interface for Unix-like systems. Thread management involves four fundamental operations:

```c
#include <pthread.h>

/* Thread creation */
int pthread_create(
    pthread_t *thread,           /* Output: thread identifier */
    const pthread_attr_t *attr,  /* Attributes (NULL for defaults) */
    void *(*start_routine)(void*), /* Function to execute */
    void *arg                    /* Argument to function */
);

/* Thread termination (from within the thread) */
void pthread_exit(void *retval);

/* Wait for thread completion */
int pthread_join(
    pthread_t thread,            /* Thread to wait for */
    void **retval               /* Output: thread's return value */
);

/* Detach thread (resources freed automatically on exit) */
int pthread_detach(pthread_t thread);
```

### 4. Mutual Exclusion with Mutexes

A **mutex** (mutual exclusion lock) provides the simplest synchronisation primitive. A thread must acquire (lock) the mutex before entering a critical section and release (unlock) it upon exit.

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* Critical section pattern */
pthread_mutex_lock(&mutex);
    /* Only one thread can execute here at a time */
    shared_counter++;
pthread_mutex_unlock(&mutex);
```

```
Mutex Operation Timeline
───────────────────────────────────────────────────────────────────
     Thread 1                   Thread 2                Mutex State
───────────────────────────────────────────────────────────────────
     lock() ─────────────────►                          LOCKED (T1)
     critical section          lock() → BLOCKS          LOCKED (T1)
     unlock() ────────────────►  │                      UNLOCKED
                                 └──► acquires lock     LOCKED (T2)
                               critical section         LOCKED (T2)
                               unlock()                 UNLOCKED
───────────────────────────────────────────────────────────────────
```

**Mutex Variants:**

| Type | Behaviour on Relock by Same Thread | Use Case |
|------|-----------------------------------|----------|
| Normal (Default) | Undefined behaviour (may deadlock) | Standard critical sections |
| Recursive | Increments lock count | Nested function calls |
| Error-checking | Returns EDEADLK error | Debugging |

### 5. Semaphores: Generalised Synchronisation

A **semaphore** maintains an integer counter with atomic operations:

- **wait()** (P operation): If counter > 0, decrement and proceed; otherwise block
- **post()** (V operation): Increment counter; if threads are waiting, wake one

```c
#include <semaphore.h>

sem_t sem;
sem_init(&sem, 0, initial_value);  /* 0 = thread-shared */

sem_wait(&sem);   /* Decrement (block if 0) */
/* ... critical section ... */
sem_post(&sem);   /* Increment */

sem_destroy(&sem);
```

**Binary Semaphore** (initial value 1): Functions like a mutex
**Counting Semaphore** (initial value n): Permits n concurrent accessors

### 6. Condition Variables: Waiting for State Changes

Condition variables allow threads to wait efficiently for specific conditions to become true, avoiding busy-waiting loops.

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

/* Waiting thread pattern */
pthread_mutex_lock(&mutex);
while (!condition_is_true) {      /* MUST use while, not if */
    pthread_cond_wait(&cond, &mutex);
}
/* Condition is now true; mutex is held */
pthread_mutex_unlock(&mutex);

/* Signalling thread pattern */
pthread_mutex_lock(&mutex);
/* Modify shared state to make condition true */
condition_is_true = 1;
pthread_cond_signal(&cond);       /* Wake one waiter */
/* pthread_cond_broadcast(&cond); // Wake all waiters */
pthread_mutex_unlock(&mutex);
```

**Critical insight:** The `while` loop handles **spurious wakeups**—situations where a thread is awakened without the condition being true.

### 7. Classic Concurrency Problems

**Producer-Consumer (Bounded Buffer):**

```
┌─────────────────────────────────────────────────────────────────┐
│                       BOUNDED BUFFER                             │
│  ┌────┬────┬────┬────┬────┬────┬────┬────┐                      │
│  │ D  │ D  │ D  │    │    │    │    │    │ Capacity = 8         │
│  └────┴────┴────┴────┴────┴────┴────┴────┘                      │
│    ↑                   ↑                                         │
│   tail               head                                        │
│                                                                  │
│  Producers ──[put]──► Buffer ──[get]──► Consumers               │
│                                                                  │
│  Synchronisation:                                                │
│  • Mutex: protect buffer access                                  │
│  • Semaphore not_full: block producers when buffer full         │
│  • Semaphore not_empty: block consumers when buffer empty       │
└─────────────────────────────────────────────────────────────────┘
```

**Readers-Writers:**
- Multiple readers may access data concurrently
- Writers require exclusive access
- Variants differ in starvation prevention for readers vs writers

**Dining Philosophers:**
- N philosophers around a table with N forks
- Each needs two forks to eat
- Demonstrates deadlock conditions

### 8. Deadlock: Conditions and Prevention

**Coffman Conditions** (all four must hold for deadlock):

1. **Mutual Exclusion:** Resources cannot be shared
2. **Hold and Wait:** Processes hold resources while waiting for others
3. **No Preemption:** Resources cannot be forcibly taken
4. **Circular Wait:** Circular chain of processes waiting for resources

```
Deadlock Example: Dining Philosophers

     P0                      P1
     ├─ holds fork 0        ├─ holds fork 1
     └─ waits for fork 1    └─ waits for fork 0
     
       ↓                      ↓
    ╔═════════════════════════════╗
    ║  DEADLOCK: Circular Wait    ║
    ║  P0 → F1 → P1 → F0 → P0     ║
    ╚═════════════════════════════╝

Prevention: Resource Ordering
  • Number all forks 0..N-1
  • Always acquire lower-numbered fork first
  • P0 acquires F0, then F1
  • P1 acquires F0 (waits if held), then F1
  • No circular wait possible!
```

### 9. Amdahl's Law: Parallelism Limits

Amdahl's Law quantifies the maximum speedup achievable through parallelisation:

```
            1
S(n) = ─────────────
       (1-P) + P/n

Where:
  S(n) = Speedup with n processors
  P    = Fraction of work that is parallelisable
  n    = Number of processors
```

```
Speedup vs Processors (Amdahl's Law)
Speedup
   │
 10├─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─────── P = 0.95
   │                        ╱───────────
  8├─ ─ ─ ─ ─ ─ ─ ─ ─ ╱─────────────────
   │              ╱───╱
  6├─ ─ ─ ─ ─ ╱───╱
   │        ╱──╱
  4├─ ─ ─╱──╱─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─────── P = 0.75
   │   ╱╱
  2├──╱─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─────── P = 0.50
   │╱
  1├─────────────────────────────────────
   └────┬────┬────┬────┬────┬────┬────┬─► Processors
        2    4    8   16   32   64  128

Key insight: Even with 95% parallelisable code,
speedup is limited to 20x regardless of processor count.
```

### 10. C11 Atomics: Lock-Free Programming

C11 introduced `<stdatomic.h>` for atomic operations without explicit locks:

```c
#include <stdatomic.h>

atomic_int counter = 0;

/* Atomic increment (no mutex needed) */
atomic_fetch_add(&counter, 1);

/* Compare-and-swap (CAS) - foundation of lock-free algorithms */
int expected = 5;
int desired = 10;
if (atomic_compare_exchange_strong(&counter, &expected, desired)) {
    /* Success: counter was 5, now is 10 */
} else {
    /* Failure: counter was not 5, expected now holds actual value */
}
```

---

## 🏭 Industrial Applications

### 1. Web Server Thread Pool (nginx-style)

Modern web servers maintain worker thread pools to handle concurrent requests:

```c
/* Simplified thread pool pattern */
#define POOL_SIZE 4

void *worker(void *arg) {
    TaskQueue *queue = (TaskQueue *)arg;
    while (1) {
        Task task = queue_get(queue);  /* Blocks if empty */
        process_request(task);
    }
}

/* Main thread accepts connections, submits to pool */
for (int i = 0; i < POOL_SIZE; i++) {
    pthread_create(&workers[i], NULL, worker, &queue);
}
```

### 2. Database Connection Pooling

Database systems limit concurrent connections; applications pool and reuse them:

```c
typedef struct {
    DBConnection *connections[MAX_CONNECTIONS];
    sem_t available;      /* Counting semaphore */
    pthread_mutex_t lock;
} ConnectionPool;

DBConnection *pool_acquire(ConnectionPool *pool) {
    sem_wait(&pool->available);  /* Block if none available */
    pthread_mutex_lock(&pool->lock);
    DBConnection *conn = find_free_connection(pool);
    pthread_mutex_unlock(&pool->lock);
    return conn;
}
```

### 3. MapReduce: Parallel Data Processing

Distributed computation frameworks partition data across workers:

```
MapReduce Word Count
────────────────────────────────────────────────────────────────
INPUT                    MAP PHASE              REDUCE PHASE
────────────────────────────────────────────────────────────────
"hello world"       →    (hello, 1)         ╲
"hello there"       →    (world, 1)          ╲   (hello, [1,1]) → (hello, 2)
                         (hello, 1)          ╱   (world, [1])   → (world, 1)
                         (there, 1)         ╱    (there, [1])   → (there, 1)
────────────────────────────────────────────────────────────────
```

### 4. Real-Time Systems: Rate Monotonic Scheduling

Embedded systems schedule periodic tasks by frequency:

```c
/* Task with period and execution time */
typedef struct {
    void (*function)(void);
    int period_ms;
    int deadline_ms;
    pthread_t thread;
} PeriodicTask;

/* Worker executes at fixed intervals */
void *periodic_worker(void *arg) {
    PeriodicTask *task = (PeriodicTask *)arg;
    while (running) {
        task->function();
        sleep_until_next_period(task->period_ms);
    }
}
```

---

## 💻 Laboratory Exercises

### Exercise 1: Parallel Log Processor

Implement a multi-threaded log file processor with producer-consumer architecture. Multiple producer threads read log entries from input files whilst multiple consumer threads process and categorise entries.

**Requirements:**

- Bounded buffer with configurable capacity
- N producer threads (one per input file)
- M consumer threads (configurable)
- Thread-safe statistics collection
- Graceful shutdown on completion

**File:** `src/exercise1.c`

**Input:** Multiple log files in `data/` directory

**Expected Output:**
```
Processing started with 2 producers, 4 consumers, buffer size 16
Producer 0: read 1500 entries from access.log
Producer 1: read 2300 entries from error.log
Consumer 0: processed 950 entries (400 INFO, 550 ERROR)
Consumer 1: processed 870 entries (380 INFO, 490 ERROR)
...
Total: 3800 entries processed
Throughput: 12500 entries/second
```

### Exercise 2: Parallel Quick Sort

Implement a parallel version of Quick Sort using task-based parallelism. Large partitions spawn new threads; small partitions execute sequentially.

**Requirements:**

- Parallel partitioning at configurable threshold
- Thread count limiting (prevent explosion)
- Benchmark comparison with sequential version
- Correct handling of edge cases

**File:** `src/exercise2.c`

**Expected Output:**
```
Array size: 10000000
Threshold: 10000

Sequential Quick Sort:
  Time: 1.234 seconds
  
Parallel Quick Sort (4 threads):
  Time: 0.389 seconds
  Speedup: 3.17x
  Efficiency: 79.3%

Parallel Quick Sort (8 threads):
  Time: 0.298 seconds
  Speedup: 4.14x
  Efficiency: 51.8%
```

---

## 🔧 Compilation and Execution

```bash
# Build all targets
make

# Build specific exercise
make exercise1
make exercise2

# Run complete example demonstration
make run

# Execute with test data
make test

# Check for memory leaks and race conditions
make valgrind

# Run with thread sanitiser
make tsan

# Clean build artefacts
make clean

# Display available targets
make help
```

**Manual Compilation:**
```bash
gcc -Wall -Wextra -std=c11 -pthread -g -o example1 src/example1.c
gcc -Wall -Wextra -std=c11 -pthread -g -o exercise1 src/exercise1.c
gcc -Wall -Wextra -std=c11 -pthread -g -o exercise2 src/exercise2.c
```

**Docker Execution:**
```bash
docker build -t week20-parallel .
docker run --rm week20-parallel
```

---

## 📁 Directory Structure

```
20-parallel-concurrent/
├── README.md                           # This documentation
├── Makefile                            # Build automation
├── Dockerfile                          # Container build
├── docker-compose.yml                  # Multi-container setup
│
├── slides/
│   ├── presentation-week20.html        # Main lecture (~40 slides)
│   └── presentation-comparativ.html    # Language comparison (~12 slides)
│
├── src/
│   ├── example1.c                      # Complete concurrency demonstration
│   ├── exercise1.c                     # Producer-consumer exercise
│   └── exercise2.c                     # Parallel sort exercise
│
├── data/
│   ├── access.log                      # Sample log file (producer input)
│   ├── error.log                       # Sample log file (producer input)
│   └── numbers.txt                     # Array data for sorting
│
├── tests/
│   ├── test1_input.txt                 # Test commands for exercise 1
│   ├── test1_expected.txt              # Expected output for exercise 1
│   ├── test2_input.txt                 # Test commands for exercise 2
│   └── test2_expected.txt              # Expected output for exercise 2
│
├── teme/
│   ├── homework-requirements.md        # Homework assignments (100 points)
│   └── homework-extended.md            # Bonus challenges (+50 points)
│
├── python_comparison/
│   ├── threads_demo.py                 # Python threading equivalent
│   └── multiprocessing_demo.py         # Python multiprocessing example
│
└── solution/
    ├── exercise1_sol.c                 # Solution for exercise 1
    ├── exercise2_sol.c                 # Solution for exercise 2
    ├── homework1_sol.c                 # Solution for homework 1
    └── homework2_sol.c                 # Solution for homework 2
```

---

## 📖 Recommended Reading

### Essential References

1. **Butenhof, D. R.** (1997). *Programming with POSIX Threads*. Addison-Wesley. The authoritative guide to pthreads programming.

2. **Herlihy, M., & Shavit, N.** (2012). *The Art of Multiprocessor Programming* (Revised ed.). Morgan Kaufmann. Comprehensive treatment of concurrent data structures.

3. **Cormen, T. H., et al.** (2009). *Introduction to Algorithms* (3rd ed.). MIT Press. Chapter 27: Multithreaded Algorithms.

### Advanced Topics

4. **Dijkstra, E. W.** (1965). Cooperating Sequential Processes. *Technical Report EWD-123*. Eindhoven University. The foundational paper on concurrent programming.

5. **Hoare, C. A. R.** (1974). Monitors: An Operating System Structuring Concept. *Communications of the ACM*, 17(10), 549-557.

6. **Lamport, L.** (1978). Time, Clocks, and the Ordering of Events in a Distributed System. *Communications of the ACM*, 21(7), 558-565.

### Online Resources

7. **POSIX Threads Programming** - https://computing.llnl.gov/tutorials/pthreads/
8. **Thread Sanitizer Documentation** - https://clang.llvm.org/docs/ThreadSanitizer.html
9. **C11 Atomics Tutorial** - https://en.cppreference.com/w/c/atomic

---

## ✅ Self-Assessment Checklist

Before submitting your laboratory work, verify:

- [ ] I can explain the difference between processes and threads
- [ ] I understand why race conditions occur and how to prevent them
- [ ] I can implement correct critical sections using mutexes
- [ ] I understand the producer-consumer pattern with condition variables
- [ ] I can identify the four Coffman conditions for deadlock
- [ ] I can apply Amdahl's Law to predict parallel speedup
- [ ] I understand when to use semaphores vs condition variables
- [ ] I can implement a thread pool with proper shutdown
- [ ] My code compiles without warnings using `-Wall -Wextra -pthread`
- [ ] Thread sanitiser reports no data races

---

## 💼 Interview Preparation

**Question 1:** *What is a race condition, and how do you prevent it?*

A race condition occurs when program correctness depends on the relative timing of thread execution. Prevention strategies include mutex locks for critical sections, atomic operations for simple counters and lock-free algorithms using compare-and-swap. The key insight is identifying shared mutable state and ensuring all access is properly synchronised.

**Question 2:** *Explain the difference between a mutex and a semaphore.*

A mutex provides binary mutual exclusion—exactly one thread may hold it at any time. A semaphore maintains a counter permitting multiple concurrent holders (up to the initial value). Mutexes have ownership (only the locker may unlock); semaphores do not. Use mutexes for critical sections; semaphores for resource pools and signalling.

**Question 3:** *What is deadlock? How do you prevent it?*

Deadlock occurs when threads wait circularly for resources held by each other. Prevention eliminates one of the four Coffman conditions: avoid mutual exclusion (impossible for most resources), avoid hold-and-wait (acquire all resources atomically), allow preemption (difficult) or impose resource ordering (acquire in fixed order, breaking circular wait).

**Question 4:** *What does Amdahl's Law tell us about parallel speedup?*

Amdahl's Law states that speedup is limited by the sequential fraction of work. With parallel fraction P and n processors, maximum speedup is 1/(1-P). Even 95% parallel code achieves at most 20x speedup regardless of processor count. The law emphasises optimising sequential bottlenecks.

**Question 5:** *How does a condition variable differ from busy waiting?*

Busy waiting (spin loop) consumes CPU cycles checking a condition repeatedly. Condition variables allow threads to sleep until signalled, freeing the CPU for useful work. Use condition variables when the expected wait time exceeds the cost of context switching (roughly 1000+ cycles).

---

## 🔗 Course Conclusion

**Congratulations!** This laboratory concludes the ATP (Algorithms and Programming Techniques) course. Throughout Weeks 1-20, you have progressed from basic pointer manipulation through advanced data structures to parallel programming—a comprehensive journey covering the foundational algorithms and techniques essential for professional software development.

Key milestones in your learning journey:
- **Weeks 1-5:** Memory management, files and recursion
- **Weeks 6-10:** Complexity analysis, trees and heaps
- **Weeks 11-14:** Hash tables, graphs and optimisation
- **Weeks 15-16:** Advanced sorting and algorithmic paradigms
- **Weeks 17-19:** Big data structures, ML fundamentals and IoT
- **Week 20:** Parallel and concurrent programming

The skills acquired in this course form the foundation for advanced study in operating systems, distributed systems, databases and high-performance computing. May your algorithms be efficient and your code race-condition free!

---

## 🔌 Real Hardware Extension (Optional)

> **Note:** This section is for students who wish to experiment with physical hardware.
> Arduino kits (ESP32, Arduino Due) are available for borrowing from the faculty library.

Multi-core programming extends naturally to embedded systems with multi-core microcontrollers:

**ESP32 Dual-Core Programming:**
- Core 0: WiFi and system tasks
- Core 1: Application code
- FreeRTOS provides threading primitives

```c
/* ESP32 task creation (FreeRTOS) */
xTaskCreatePinnedToCore(
    sensor_task,     /* Function */
    "Sensor",        /* Name */
    4096,            /* Stack size */
    NULL,            /* Parameters */
    1,               /* Priority */
    &sensorHandle,   /* Handle */
    1                /* Core ID (0 or 1) */
);
```

**Advantages over simulation:**
- True concurrent execution on separate cores
- Hardware interrupts and timing constraints
- Power consumption considerations
- Real peripheral contention

---

*Algorithms and Programming Techniques — Week 20 Laboratory Materials*
*Academy of Economic Studies, Bucharest — Computer Science Faculty*
*Alternative learning kit for non-formal education*

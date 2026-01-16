# Week 20: Extended Challenges — Parallel and Concurrent Programming

## 🏆 Bonus Challenges

These optional challenges extend beyond the core curriculum, offering motivated students
the opportunity to explore advanced synchronisation patterns and lock-free programming
paradigms. Each challenge is worth **+10 bonus points** and requires a thorough
understanding of the foundational material.

---

## Challenge 1: Read-Write Lock Implementation (+10 points)

### Problem Statement

Implement a read-write lock (also known as a shared-exclusive lock) that permits
multiple concurrent readers whilst guaranteeing exclusive access for writers. The
implementation must satisfy the following constraints:

**Invariants:**
- Multiple threads may hold the read lock simultaneously
- Only one thread may hold the write lock at any time
- No read locks may be held whilst a write lock is active
- No write lock may be acquired whilst any read lock is held

**Interface:**
```c
typedef struct rwlock RWLock;

RWLock *rwlock_create(void);
void    rwlock_destroy(RWLock *lock);
void    rwlock_read_lock(RWLock *lock);
void    rwlock_read_unlock(RWLock *lock);
void    rwlock_write_lock(RWLock *lock);
void    rwlock_write_unlock(RWLock *lock);
```

### Requirements

1. **Writer Starvation Prevention**: Implement a fairness policy that prevents writers
   from being indefinitely delayed when readers continuously arrive.

2. **Downgrade Support**: Allow a thread holding a write lock to atomically downgrade
   to a read lock without releasing and reacquiring.

3. **Statistics Tracking**: Maintain counters for total reads, total writes and
   current wait queue depth.

### Evaluation Criteria

| Criterion | Points |
|-----------|--------|
| Basic functionality (concurrent reads, exclusive writes) | 4 |
| Writer starvation prevention | 3 |
| Downgrade operation | 2 |
| Documentation and testing | 1 |

---

## Challenge 2: Barrier Synchronisation Primitive (+10 points)

### Problem Statement

Implement a reusable barrier that blocks threads until all participants have arrived.
The barrier must support multiple phases (reusability) without requiring recreation.

**Interface:**
```c
typedef struct barrier Barrier;

Barrier *barrier_create(unsigned int count);
void     barrier_destroy(Barrier *barrier);
int      barrier_wait(Barrier *barrier);  /* Returns BARRIER_SERIAL_THREAD for one */
```

### Requirements

1. **Reusability**: After all threads pass the barrier, it automatically resets for
   the next phase without explicit reset calls.

2. **Serial Thread Designation**: Exactly one thread receives the return value
   `BARRIER_SERIAL_THREAD` per phase, enabling post-synchronisation work.

3. **Timeout Support**: Implement `barrier_wait_timeout()` that returns an error code
   if the barrier is not reached within the specified duration.

4. **Dynamic Resize**: Support `barrier_set_count()` to adjust participant count
   between phases (only when no threads are waiting).

### Implementation Hints

Consider using a generation counter to distinguish between consecutive barrier phases.
The "sense reversal" technique provides an elegant solution for reusability.

### Evaluation Criteria

| Criterion | Points |
|-----------|--------|
| Basic barrier functionality | 3 |
| Correct reusability across phases | 3 |
| Timeout support | 2 |
| Serial thread designation | 1 |
| Dynamic resize capability | 1 |

---

## Challenge 3: Work-Stealing Scheduler (+10 points)

### Problem Statement

Implement a work-stealing scheduler where worker threads maintain local double-ended
queues (deques). When a worker's queue is empty, it steals tasks from other workers'
queues to maintain load balance.

**Interface:**
```c
typedef struct work_stealing_pool WSPool;
typedef void (*TaskFunction)(void *arg);

WSPool *wspool_create(size_t num_workers);
void    wspool_destroy(WSPool *pool);
void    wspool_submit(WSPool *pool, TaskFunction fn, void *arg);
void    wspool_wait_all(WSPool *pool);

/* Statistics */
size_t  wspool_get_steal_count(WSPool *pool, size_t worker_id);
size_t  wspool_get_local_executions(WSPool *pool, size_t worker_id);
```

### Requirements

1. **Chase-Lev Deque**: Implement the Chase-Lev work-stealing deque where owners
   push/pop from the bottom (LIFO) whilst thieves steal from the top (FIFO).

2. **Randomised Stealing**: When a worker needs to steal, it should select victims
   randomly to avoid contention hot-spots.

3. **Recursive Task Support**: Tasks may spawn child tasks; newly created tasks
   should be pushed onto the spawning worker's local queue.

4. **Termination Detection**: Implement efficient termination detection to know
   when all submitted work has completed.

### Applications

Demonstrate the scheduler with a parallel divide-and-conquer algorithm such as
parallel Fibonacci or parallel merge sort.

### Evaluation Criteria

| Criterion | Points |
|-----------|--------|
| Lock-free deque implementation | 4 |
| Correct stealing protocol | 3 |
| Termination detection | 2 |
| Demonstration application | 1 |

---

## Challenge 4: Lock-Free Queue — Michael & Scott Algorithm (+10 points)

### Problem Statement

Implement the Michael & Scott lock-free queue using compare-and-swap (CAS) operations.
This queue permits concurrent enqueue and dequeue operations without mutual exclusion.

**Interface:**
```c
typedef struct lfqueue LFQueue;

LFQueue *lfqueue_create(void);
void     lfqueue_destroy(LFQueue *queue);
bool     lfqueue_enqueue(LFQueue *queue, void *item);
bool     lfqueue_dequeue(LFQueue *queue, void **item_out);
bool     lfqueue_is_empty(LFQueue *queue);
```

### Requirements

1. **ABA Problem Mitigation**: Use tagged pointers or hazard pointers to prevent
   the ABA problem that can corrupt lock-free data structures.

2. **Memory Reclamation**: Implement safe memory reclamation for dequeued nodes.
   Consider epoch-based reclamation or hazard pointers.

3. **Linearisability**: The queue must be linearisable — operations appear to
   take effect instantaneously at some point between invocation and response.

4. **Progress Guarantees**: Document and demonstrate the lock-free progress
   guarantee (some thread always makes progress).

### Implementation Hints

```c
/* Tagged pointer to solve ABA problem */
typedef struct {
    uintptr_t ptr  : 48;
    uintptr_t tag  : 16;
} TaggedPointer;

/* Atomic operations on tagged pointers */
bool cas_tagged(TaggedPointer *target,
                TaggedPointer expected,
                TaggedPointer desired);
```

### Evaluation Criteria

| Criterion | Points |
|-----------|--------|
| Basic lock-free enqueue/dequeue | 4 |
| ABA problem solution | 3 |
| Memory reclamation | 2 |
| Stress test validation | 1 |

---

## Challenge 5: Mini Actor Model with Message Passing (+10 points)

### Problem Statement

Implement a lightweight actor model framework where actors communicate exclusively
through asynchronous message passing. No shared state exists between actors.

**Interface:**
```c
typedef struct actor Actor;
typedef struct message Message;
typedef void (*ActorBehaviour)(Actor *self, Message *msg);

/* Actor system */
ActorSystem *actor_system_create(size_t num_schedulers);
void         actor_system_shutdown(ActorSystem *system);

/* Actor lifecycle */
Actor   *actor_spawn(ActorSystem *system, ActorBehaviour behaviour, void *state);
void     actor_stop(Actor *actor);

/* Messaging */
void     actor_send(Actor *recipient, Message *msg);
Message *message_create(int type, void *payload, size_t size);
void     message_destroy(Message *msg);
```

### Requirements

1. **Asynchronous Mailboxes**: Each actor maintains a private mailbox (queue) for
   incoming messages. Messages are processed one at a time (sequential consistency).

2. **Location Transparency**: Actors reference each other through handles; the
   physical location (which scheduler thread) is hidden.

3. **Supervision Trees** (Optional): Implement parent-child relationships where
   parent actors can monitor child failures and take recovery actions.

4. **Bounded Mailboxes**: Support configurable mailbox capacity with back-pressure
   mechanisms when full (block sender, drop oldest, drop newest).

### Demonstration

Implement a distributed computation such as:
- Parallel prime number sieve using actor workers
- Ping-pong benchmark between actor pairs
- Simple chat room simulation

### Evaluation Criteria

| Criterion | Points |
|-----------|--------|
| Actor creation and messaging | 3 |
| Thread-safe mailbox implementation | 3 |
| Multiple schedulers distributing actors | 2 |
| Demonstration application | 1 |
| Supervision trees (bonus within bonus) | 1 |

---

## Submission Guidelines

### Directory Structure

```
bonus/
├── challenge1/
│   ├── rwlock.h
│   ├── rwlock.c
│   ├── test_rwlock.c
│   └── README.md
├── challenge2/
│   ├── barrier.h
│   ├── barrier.c
│   ├── test_barrier.c
│   └── README.md
├── challenge3/
│   ├── wspool.h
│   ├── wspool.c
│   ├── demo_fib.c
│   └── README.md
├── challenge4/
│   ├── lfqueue.h
│   ├── lfqueue.c
│   ├── test_lfqueue.c
│   └── README.md
└── challenge5/
    ├── actor.h
    ├── actor.c
    ├── demo_primes.c
    └── README.md
```

### Documentation Requirements

Each challenge submission must include:

1. **Design Document**: Explain algorithmic choices and trade-offs (500-800 words)
2. **Correctness Arguments**: Informal proofs or invariant documentation
3. **Performance Analysis**: Benchmark results with varying thread counts
4. **Known Limitations**: Honest assessment of edge cases or weaknesses

### Testing Requirements

- Stress tests with high contention
- Valgrind/ThreadSanitiser clean execution
- Edge case coverage (empty queue, single thread, maximum threads)

---

## Resources for Further Study

### Academic Papers

1. **Michael, M.M. & Scott, M.L.** (1996). "Simple, Fast, and Practical Non-Blocking
   and Blocking Concurrent Queue Algorithms." *PODC '96*.

2. **Chase, D. & Lev, Y.** (2005). "Dynamic Circular Work-Stealing Deque."
   *SPAA '05*.

3. **Herlihy, M. & Shavit, N.** (2008). *The Art of Multiprocessor Programming*.
   Morgan Kaufmann. (Chapters 10-11 on lock-free structures)

4. **Hewitt, C., Bishop, P. & Steiger, R.** (1973). "A Universal Modular Actor
   Formalism for Artificial Intelligence." *IJCAI '73*.

### Online Resources

- [Preshing on Programming](https://preshing.com) — Lock-free programming tutorials
- [1024cores](https://www.1024cores.net) — Concurrent programming patterns
- [C++ Memory Model Documentation](https://en.cppreference.com/w/cpp/atomic/memory_order)
  (concepts apply to C11 atomics)

---

## Grading Summary

| Challenge | Maximum Bonus |
|-----------|---------------|
| Challenge 1: Read-Write Lock | +10 |
| Challenge 2: Barrier Synchronisation | +10 |
| Challenge 3: Work-Stealing Scheduler | +10 |
| Challenge 4: Lock-Free Queue | +10 |
| Challenge 5: Actor Model | +10 |
| **Total Maximum Bonus** | **+50** |

**Note**: Bonus points cannot exceed 50% of the base homework total. If base homework
scores 80/100, maximum achievable with bonuses is 120/100 (capped at the instructor's
discretion).

---

*Extended challenges for ATP Course — Week 20*
*Academy of Economic Studies - CSIE Bucharest*
*Alternative learning kit for non-formal education*

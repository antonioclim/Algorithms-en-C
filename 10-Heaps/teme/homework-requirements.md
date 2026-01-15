# Week 10 Homework: Heaps and Priority Queues

## 📋 General Information

- **Deadline:** End of week 11
- **Points:** 100 (10% of final grade)
- **Language:** C (C11 standard)
- **Compiler:** GCC with `-Wall -Wextra -std=c11`

---

## 📝 Homework 1: Task Scheduler with Priority Queue (50 points)

### Description

Implement a task scheduler that processes tasks based on their priority using a heap-based priority queue. The scheduler should support both time-based and priority-based scheduling policies.

### Requirements

1. **Task Structure** (5p)
   - Task ID (unique integer)
   - Priority level (1-10, where 10 is highest)
   - Arrival time (timestamp)
   - Execution time (milliseconds)
   - Description (string, max 64 characters)

2. **Priority Queue Operations** (15p)
   - `scheduler_create()` - Create scheduler with configurable capacity
   - `scheduler_add_task()` - Add task to queue
   - `scheduler_get_next()` - Extract highest priority task
   - `scheduler_peek_next()` - View next task without removal
   - `scheduler_destroy()` - Free all resources

3. **Scheduling Policies** (15p)
   - **Priority-First:** Higher priority tasks always execute first
   - **Priority-with-Aging:** Tasks gain priority over time to prevent starvation
   - Implement policy selection via function pointer

4. **Statistics Tracking** (10p)
   - Total tasks processed
   - Average wait time
   - Maximum wait time
   - Throughput (tasks per second)

5. **Memory Management** (5p)
   - No memory leaks (verified with Valgrind)
   - Handle allocation failures gracefully

### Example Usage

```c
Scheduler *s = scheduler_create(100, PRIORITY_FIRST);

Task t1 = {1, 8, time(NULL), 100, "Critical update"};
Task t2 = {2, 3, time(NULL), 50, "Background sync"};
Task t3 = {3, 10, time(NULL), 200, "Emergency fix"};

scheduler_add_task(s, &t1);
scheduler_add_task(s, &t2);
scheduler_add_task(s, &t3);

Task next;
while (scheduler_get_next(s, &next)) {
    printf("Processing: %s (priority %d)\n", next.description, next.priority);
}

scheduler_print_stats(s);
scheduler_destroy(s);
```

### File: `homework1_scheduler.c`

---

## 📝 Homework 2: K-Way Merge Using Min-Heap (50 points)

### Description

Implement an efficient algorithm to merge K sorted arrays into a single sorted array using a min-heap. This is a classic problem used in external sorting and database merge operations.

### Requirements

1. **Heap Node Structure** (5p)
   - Current value
   - Array index (which of the K arrays)
   - Element index within that array

2. **Min-Heap Operations** (15p)
   - Implement min-heap specifically for merge nodes
   - Support for efficient extract-min and insert
   - Handle empty arrays gracefully

3. **Merge Algorithm** (20p)
   - `merge_k_sorted(int **arrays, int *sizes, int k, int *result, int *result_size)`
   - Time complexity: O(N log K) where N is total elements
   - Space complexity: O(K) for the heap

4. **Edge Cases** (5p)
   - Empty arrays in input
   - Single array (no merge needed)
   - Arrays of different sizes
   - K = 0 or K = 1

5. **Testing and Verification** (5p)
   - Verify output is sorted
   - Verify all elements are present
   - Performance testing with large K

### Example Usage

```c
int arr1[] = {1, 4, 7, 10};
int arr2[] = {2, 5, 8};
int arr3[] = {3, 6, 9, 11, 12};

int *arrays[] = {arr1, arr2, arr3};
int sizes[] = {4, 3, 5};
int k = 3;

int result[12];
int result_size;

merge_k_sorted(arrays, sizes, k, result, &result_size);

// result = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}
```

### File: `homework2_kmerge.c`

---

## 📊 Evaluation Criteria

| Criterion | Points |
|-----------|--------|
| Functional correctness | 40 |
| Proper use of heap data structure | 25 |
| Edge case handling | 15 |
| Code quality and comments | 10 |
| No compiler warnings | 10 |

### Penalties

- -10p: Compiler warnings
- -20p: Memory leaks detected by Valgrind
- -30p: Crashes on valid input
- -50p: Plagiarism (automatic failure)

---

## 📤 Submission

1. Submit two separate `.c` files:
   - `homework1_scheduler.c`
   - `homework2_kmerge.c`

2. Each file must be self-contained (no external headers beyond standard library)

3. Include your name and student ID in comments at the top

4. Upload to the course portal before the deadline

---

## 💡 Tips

1. **Start with the data structures** - Design your heap node and task structures carefully before implementing operations.

2. **Test incrementally** - Implement and test each operation before moving to the next.

3. **Draw diagrams** - Visualise the heap operations on paper first.

4. **Use Valgrind early** - Check for memory leaks throughout development, not just at the end.

5. **Handle edge cases** - Empty inputs, single elements and boundary conditions often reveal bugs.

6. **Document your complexity** - Add comments explaining the time complexity of each operation.

---

## 📚 Resources

- Lecture slides: Week 10 - Heaps and Priority Queues
- Example code: `example1.c` in the starter kit
- Textbook: CLRS Chapter 6 (Heapsort)
- Online: [Visualgo Heap Visualisation](https://visualgo.net/en/heap)

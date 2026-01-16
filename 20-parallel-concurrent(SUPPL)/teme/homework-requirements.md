# Week 20 Homework: Parallel and Concurrent Programming

## 📋 General Information

- **Deadline:** End of semester (Sunday, 23:59)
- **Points:** 100 (10% of final grade)
- **Language:** C (C11 standard)
- **Compiler:** GCC with `-Wall -Wextra -std=c11 -pthread`
- **Memory check:** Valgrind must report no leaks
- **Thread check:** No data races (verified with Thread Sanitiser)

---

## 📝 Homework 1: Thread Pool with Futures (50 points)

### Description

Implement a thread pool that supports "futures"—handles that allow the caller to retrieve the result of an asynchronous computation once it completes. This pattern is fundamental to modern concurrent programming and appears in languages from Java to Rust.

### Requirements

1. **Thread Pool Structure** (10 points)
   - Fixed number of worker threads (configurable at creation)
   - Task queue with bounded capacity
   - Graceful shutdown mechanism
   - Worker threads wait when queue is empty

2. **Future Implementation** (15 points)
   - `Future` handle returned when task is submitted
   - `future_get(future)` blocks until result is available
   - `future_is_ready(future)` checks completion status (non-blocking)
   - `future_destroy(future)` releases resources
   - Support for cancellation of pending tasks

3. **Task Interface** (10 points)
   ```c
   typedef void *(*TaskFunction)(void *arg);
   
   ThreadPool *threadpool_create(int num_threads, int queue_capacity);
   Future *threadpool_submit(ThreadPool *pool, TaskFunction func, void *arg);
   void *future_get(Future *future);
   bool future_is_ready(Future *future);
   bool future_cancel(Future *future);  /* Cancel if not started */
   void future_destroy(Future *future);
   void threadpool_shutdown(ThreadPool *pool);
   void threadpool_destroy(ThreadPool *pool);
   ```

4. **Timeout Support** (10 points)
   - `future_get_timeout(future, timeout_ms)` with configurable timeout
   - Returns NULL and sets error flag if timeout occurs
   - Does not cancel the underlying task

5. **Memory and Thread Safety** (5 points)
   - No memory leaks
   - No data races
   - Proper cleanup on shutdown

### Example Usage

```c
ThreadPool *pool = threadpool_create(4, 16);

/* Submit tasks */
Future *f1 = threadpool_submit(pool, expensive_computation, &data1);
Future *f2 = threadpool_submit(pool, expensive_computation, &data2);

/* Do other work while tasks execute... */

/* Wait for results */
void *result1 = future_get(f1);
void *result2 = future_get_timeout(f2, 5000);  /* 5 second timeout */

if (result2 == NULL && future_timed_out(f2)) {
    printf("Task 2 timed out!\n");
}

future_destroy(f1);
future_destroy(f2);

threadpool_shutdown(pool);
threadpool_destroy(pool);
```

### File: `homework1_thread_pool.c`

---

## 📝 Homework 2: MapReduce Word Count (50 points)

### Description

Implement a simplified MapReduce framework for word counting across multiple text files. This exercise demonstrates the fundamental pattern used by distributed computing frameworks like Hadoop and Spark.

### Requirements

1. **MapReduce Architecture** (10 points)
   - Map phase: parallel word extraction from files
   - Shuffle phase: group words by key (hash-based)
   - Reduce phase: aggregate word counts
   - Configurable number of mappers and reducers

2. **Map Phase** (15 points)
   ```c
   /* Each mapper processes one or more files */
   typedef struct {
       char *word;
       int count;  /* Always 1 for map output */
   } KeyValue;
   
   /* Mapper function signature */
   void map(const char *filename, void (*emit)(const char *word));
   ```
   - Split files among mapper threads
   - Emit (word, 1) pairs for each word found
   - Handle punctuation and case normalisation
   - Thread-safe emission to intermediate storage

3. **Shuffle Phase** (10 points)
   - Partition words by hash to reducer buckets
   - Group all values for each key together
   - Efficient concurrent hash table for intermediate storage

4. **Reduce Phase** (10 points)
   ```c
   /* Reducer function signature */
   int reduce(const char *word, int *counts, int num_counts);
   ```
   - Sum all counts for each word
   - Parallel reduction across reducer threads
   - Output final word counts

5. **Output and Statistics** (5 points)
   - Print top N words by frequency
   - Report total unique words
   - Report processing statistics (time, throughput)

### Example Usage

```c
MapReduceConfig config = {
    .num_mappers = 4,
    .num_reducers = 4,
    .input_files = {"file1.txt", "file2.txt", "file3.txt", NULL}
};

MapReduceResult *result = mapreduce_run(&config);

printf("Top 10 words:\n");
for (int i = 0; i < 10 && i < result->num_words; i++) {
    printf("  %s: %d\n", result->words[i].word, result->words[i].count);
}

printf("\nTotal unique words: %d\n", result->num_words);
printf("Processing time: %.2f seconds\n", result->elapsed_time);

mapreduce_free_result(result);
```

### Input File Format

Plain text files, one or more words per line:
```
The quick brown fox jumps over the lazy dog
The dog was not amused by the fox
```

### Expected Output

```
MapReduce Word Count Results
============================

Top 10 Words:
  1. the      (4 occurrences)
  2. fox      (2 occurrences)
  3. dog      (2 occurrences)
  4. quick    (1 occurrence)
  5. brown    (1 occurrence)
  ...

Statistics:
  Total words processed: 18
  Unique words: 12
  Map time: 0.023s
  Shuffle time: 0.005s
  Reduce time: 0.008s
  Total time: 0.036s
```

### File: `homework2_mapreduce.c`

---

## 📊 Evaluation Criteria

| Criterion | Points |
|-----------|--------|
| Functional correctness | 40 |
| Thread safety (no races) | 25 |
| Memory management | 15 |
| Code quality and style | 10 |
| No compiler warnings | 10 |

### Penalties

- **-10 points:** Compiler warnings with `-Wall -Wextra`
- **-20 points:** Memory leaks detected by Valgrind
- **-20 points:** Data races detected by Thread Sanitiser
- **-30 points:** Crashes or hangs on valid input
- **-50 points:** Plagiarism (automatic failure)
- **-5 points per day:** Late submission (max -25 points)

### Bonus Points

- **+5 points:** Implement work-stealing between threads
- **+5 points:** Add progress reporting callback
- **+5 points:** Implement priority-based task scheduling

---

## 📤 Submission

1. Submit two C source files:
   - `homework1_thread_pool.c`
   - `homework2_mapreduce.c`

2. Each file must be self-contained and compile independently

3. Include your name and student ID in a comment at the top

4. Submit via the course platform before the deadline

---

## 💡 Tips

1. **Start with the basics** — Get single-threaded version working first, then add parallelism

2. **Use condition variables correctly** — Always use `while` loops, not `if` statements

3. **Avoid busy waiting** — Use condition variables instead of sleep loops

4. **Test with Thread Sanitiser** — Compile with `-fsanitize=thread` to detect races

5. **Handle shutdown carefully** — Ensure threads can exit cleanly when signalled

6. **Draw diagrams** — Visualise the data flow between threads before coding

7. **Start with fewer threads** — Debug with 2 threads before scaling up

---

## 📚 Reference Materials

- Lecture slides: Week 20 - Parallel and Concurrent Programming
- Example code: `src/example1.c` (especially thread pool section)
- Laboratory exercises: `src/exercise1.c`, `src/exercise2.c`
- Recommended reading: Butenhof's "Programming with POSIX Threads"

---

*Good luck! Mastering concurrent programming is challenging but essential.*
*Take your time, test thoroughly, and don't forget about edge cases.*

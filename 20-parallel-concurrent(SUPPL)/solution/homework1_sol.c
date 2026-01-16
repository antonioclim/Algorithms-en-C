/**
 * =============================================================================
 * HOMEWORK 1 SOLUTION: Thread Pool with Futures
 * Week 20: Parallel and Concurrent Programming
 * =============================================================================
 *
 * Complete implementation of a thread pool with future handles for
 * asynchronous result retrieval.
 *
 * Features:
 *   - Submit tasks and receive future handles
 *   - future_get() blocks until result is available
 *   - future_get_timeout() with configurable timeout
 *   - Task cancellation for pending (not running) tasks
 *   - Graceful shutdown with completion of running tasks
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c11 -pthread -o homework1_sol homework1_sol.c
 *
 * Usage:
 *   ./homework1_sol
 *
 * =============================================================================
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

/* =============================================================================
 * CONFIGURATION
 * =============================================================================
 */

#define DEFAULT_POOL_SIZE     4
#define DEFAULT_QUEUE_SIZE    64
#define TIMEOUT_PRECISION_MS  10

/* =============================================================================
 * FUTURE STATES
 * =============================================================================
 */

typedef enum {
    FUTURE_PENDING,     /* Task queued, not yet started */
    FUTURE_RUNNING,     /* Task currently executing */
    FUTURE_COMPLETED,   /* Task finished successfully */
    FUTURE_CANCELLED,   /* Task was cancelled before execution */
    FUTURE_ERROR        /* Task encountered an error */
} FutureState;

/* =============================================================================
 * FUTURE STRUCTURE
 * =============================================================================
 */

/**
 * Future handle for asynchronous result retrieval.
 *
 * A future represents the result of an asynchronous computation that may
 * not yet be available. It provides thread-safe access to the result once
 * the computation completes.
 */
typedef struct Future {
    FutureState state;              /* Current state of the future */
    void *result;                   /* Result value (set on completion) */
    int error_code;                 /* Error code if state == FUTURE_ERROR */
    
    pthread_mutex_t mutex;          /* Protects state transitions */
    pthread_cond_t completed;       /* Signalled when computation finishes */
    
    atomic_int ref_count;           /* Reference count for memory management */
    
    /* Task information */
    void *(*function)(void *);      /* The function to execute */
    void *arg;                      /* Argument to pass to function */
} Future;

/* =============================================================================
 * TASK STRUCTURE
 * =============================================================================
 */

typedef struct Task {
    Future *future;                 /* Associated future handle */
    struct Task *next;              /* For linked list queue */
} Task;

/* =============================================================================
 * THREAD POOL STRUCTURE
 * =============================================================================
 */

typedef struct ThreadPool {
    pthread_t *threads;             /* Worker thread handles */
    size_t num_threads;             /* Number of worker threads */
    
    Task *queue_head;               /* Head of task queue */
    Task *queue_tail;               /* Tail of task queue */
    size_t queue_size;              /* Current number of queued tasks */
    size_t max_queue_size;          /* Maximum queue capacity */
    
    pthread_mutex_t queue_mutex;    /* Protects queue operations */
    pthread_cond_t queue_not_empty; /* Signalled when task added */
    pthread_cond_t queue_not_full;  /* Signalled when task removed */
    
    atomic_bool shutdown;           /* Shutdown flag */
    atomic_bool immediate_shutdown; /* Immediate shutdown (cancel pending) */
    
    /* Statistics */
    atomic_size_t tasks_submitted;
    atomic_size_t tasks_completed;
    atomic_size_t tasks_cancelled;
} ThreadPool;

/* =============================================================================
 * FUTURE OPERATIONS
 * =============================================================================
 */

/**
 * Create a new future.
 *
 * @param function The function to execute asynchronously
 * @param arg Argument to pass to the function
 * @return New future handle, or NULL on failure
 */
static Future *future_create(void *(*function)(void *), void *arg) {
    Future *future = malloc(sizeof(Future));
    if (!future) {
        return NULL;
    }
    
    future->state = FUTURE_PENDING;
    future->result = NULL;
    future->error_code = 0;
    future->function = function;
    future->arg = arg;
    
    if (pthread_mutex_init(&future->mutex, NULL) != 0) {
        free(future);
        return NULL;
    }
    
    if (pthread_cond_init(&future->completed, NULL) != 0) {
        pthread_mutex_destroy(&future->mutex);
        free(future);
        return NULL;
    }
    
    atomic_init(&future->ref_count, 1);
    
    return future;
}

/**
 * Increment future reference count.
 *
 * @param future The future to reference
 */
static void future_ref(Future *future) {
    if (future) {
        atomic_fetch_add(&future->ref_count, 1);
    }
}

/**
 * Decrement future reference count and free if zero.
 *
 * @param future The future to unreference
 */
static void future_unref(Future *future) {
    if (future) {
        if (atomic_fetch_sub(&future->ref_count, 1) == 1) {
            pthread_mutex_destroy(&future->mutex);
            pthread_cond_destroy(&future->completed);
            free(future);
        }
    }
}

/**
 * Wait for a future to complete and retrieve the result.
 *
 * This function blocks until the future reaches a terminal state
 * (COMPLETED, CANCELLED, or ERROR).
 *
 * @param future The future to wait for
 * @param result Output parameter for the result (may be NULL)
 * @return 0 on success, -1 if cancelled, error_code if error
 */
int future_get(Future *future, void **result) {
    if (!future) {
        return -1;
    }
    
    pthread_mutex_lock(&future->mutex);
    
    while (future->state == FUTURE_PENDING || future->state == FUTURE_RUNNING) {
        pthread_cond_wait(&future->completed, &future->mutex);
    }
    
    int ret = 0;
    
    switch (future->state) {
        case FUTURE_COMPLETED:
            if (result) {
                *result = future->result;
            }
            ret = 0;
            break;
            
        case FUTURE_CANCELLED:
            ret = -1;
            break;
            
        case FUTURE_ERROR:
            ret = future->error_code;
            break;
            
        default:
            ret = -1;
    }
    
    pthread_mutex_unlock(&future->mutex);
    
    return ret;
}

/**
 * Wait for a future with timeout.
 *
 * @param future The future to wait for
 * @param result Output parameter for the result (may be NULL)
 * @param timeout_ms Maximum time to wait in milliseconds
 * @return 0 on success, -1 if cancelled, -2 if timeout, error_code if error
 */
int future_get_timeout(Future *future, void **result, unsigned int timeout_ms) {
    if (!future) {
        return -1;
    }
    
    struct timespec deadline;
    clock_gettime(CLOCK_REALTIME, &deadline);
    
    deadline.tv_sec += timeout_ms / 1000;
    deadline.tv_nsec += (timeout_ms % 1000) * 1000000L;
    if (deadline.tv_nsec >= 1000000000L) {
        deadline.tv_sec++;
        deadline.tv_nsec -= 1000000000L;
    }
    
    pthread_mutex_lock(&future->mutex);
    
    while (future->state == FUTURE_PENDING || future->state == FUTURE_RUNNING) {
        int rc = pthread_cond_timedwait(&future->completed, &future->mutex, &deadline);
        if (rc == ETIMEDOUT) {
            pthread_mutex_unlock(&future->mutex);
            return -2; /* Timeout */
        }
    }
    
    int ret = 0;
    
    switch (future->state) {
        case FUTURE_COMPLETED:
            if (result) {
                *result = future->result;
            }
            ret = 0;
            break;
            
        case FUTURE_CANCELLED:
            ret = -1;
            break;
            
        case FUTURE_ERROR:
            ret = future->error_code;
            break;
            
        default:
            ret = -1;
    }
    
    pthread_mutex_unlock(&future->mutex);
    
    return ret;
}

/**
 * Attempt to cancel a future.
 *
 * Cancellation only succeeds if the task has not yet started executing.
 *
 * @param future The future to cancel
 * @return true if cancelled successfully, false if already running/completed
 */
bool future_cancel(Future *future) {
    if (!future) {
        return false;
    }
    
    pthread_mutex_lock(&future->mutex);
    
    bool cancelled = false;
    
    if (future->state == FUTURE_PENDING) {
        future->state = FUTURE_CANCELLED;
        pthread_cond_broadcast(&future->completed);
        cancelled = true;
    }
    
    pthread_mutex_unlock(&future->mutex);
    
    return cancelled;
}

/**
 * Check if a future is done (completed, cancelled, or error).
 *
 * @param future The future to check
 * @return true if in a terminal state, false if still pending/running
 */
bool future_is_done(Future *future) {
    if (!future) {
        return true;
    }
    
    pthread_mutex_lock(&future->mutex);
    bool done = (future->state != FUTURE_PENDING && future->state != FUTURE_RUNNING);
    pthread_mutex_unlock(&future->mutex);
    
    return done;
}

/**
 * Get the current state of a future.
 *
 * @param future The future to query
 * @return Current state
 */
FutureState future_get_state(Future *future) {
    if (!future) {
        return FUTURE_ERROR;
    }
    
    pthread_mutex_lock(&future->mutex);
    FutureState state = future->state;
    pthread_mutex_unlock(&future->mutex);
    
    return state;
}

/* =============================================================================
 * WORKER THREAD FUNCTION
 * =============================================================================
 */

/**
 * Worker thread main loop.
 *
 * Workers continuously:
 *   1. Wait for a task to become available
 *   2. Execute the task
 *   3. Store the result in the future
 *   4. Signal completion
 */
static void *worker_thread(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;
    
    while (true) {
        pthread_mutex_lock(&pool->queue_mutex);
        
        /* Wait for work or shutdown */
        while (pool->queue_head == NULL && !atomic_load(&pool->shutdown)) {
            pthread_cond_wait(&pool->queue_not_empty, &pool->queue_mutex);
        }
        
        /* Check for shutdown conditions */
        if (atomic_load(&pool->immediate_shutdown) ||
            (atomic_load(&pool->shutdown) && pool->queue_head == NULL)) {
            pthread_mutex_unlock(&pool->queue_mutex);
            break;
        }
        
        /* Dequeue task */
        Task *task = pool->queue_head;
        if (task == NULL) {
            pthread_mutex_unlock(&pool->queue_mutex);
            continue;
        }
        
        pool->queue_head = task->next;
        if (pool->queue_head == NULL) {
            pool->queue_tail = NULL;
        }
        pool->queue_size--;
        
        /* Signal that queue has space */
        pthread_cond_signal(&pool->queue_not_full);
        
        pthread_mutex_unlock(&pool->queue_mutex);
        
        /* Execute the task */
        Future *future = task->future;
        
        pthread_mutex_lock(&future->mutex);
        
        /* Check if cancelled before we could start */
        if (future->state == FUTURE_CANCELLED) {
            pthread_mutex_unlock(&future->mutex);
            atomic_fetch_add(&pool->tasks_cancelled, 1);
            future_unref(future);
            free(task);
            continue;
        }
        
        /* Mark as running */
        future->state = FUTURE_RUNNING;
        pthread_mutex_unlock(&future->mutex);
        
        /* Execute the function */
        void *result = future->function(future->arg);
        
        /* Store result and mark completed */
        pthread_mutex_lock(&future->mutex);
        
        if (future->state == FUTURE_RUNNING) {
            future->result = result;
            future->state = FUTURE_COMPLETED;
            atomic_fetch_add(&pool->tasks_completed, 1);
        }
        
        pthread_cond_broadcast(&future->completed);
        pthread_mutex_unlock(&future->mutex);
        
        /* Cleanup */
        future_unref(future);
        free(task);
    }
    
    return NULL;
}

/* =============================================================================
 * THREAD POOL OPERATIONS
 * =============================================================================
 */

/**
 * Create a new thread pool.
 *
 * @param num_threads Number of worker threads
 * @param max_queue_size Maximum number of pending tasks
 * @return New thread pool, or NULL on failure
 */
ThreadPool *threadpool_create(size_t num_threads, size_t max_queue_size) {
    if (num_threads == 0) {
        num_threads = DEFAULT_POOL_SIZE;
    }
    if (max_queue_size == 0) {
        max_queue_size = DEFAULT_QUEUE_SIZE;
    }
    
    ThreadPool *pool = malloc(sizeof(ThreadPool));
    if (!pool) {
        return NULL;
    }
    
    pool->threads = malloc(num_threads * sizeof(pthread_t));
    if (!pool->threads) {
        free(pool);
        return NULL;
    }
    
    pool->num_threads = num_threads;
    pool->queue_head = NULL;
    pool->queue_tail = NULL;
    pool->queue_size = 0;
    pool->max_queue_size = max_queue_size;
    
    atomic_init(&pool->shutdown, false);
    atomic_init(&pool->immediate_shutdown, false);
    atomic_init(&pool->tasks_submitted, 0);
    atomic_init(&pool->tasks_completed, 0);
    atomic_init(&pool->tasks_cancelled, 0);
    
    if (pthread_mutex_init(&pool->queue_mutex, NULL) != 0) {
        free(pool->threads);
        free(pool);
        return NULL;
    }
    
    if (pthread_cond_init(&pool->queue_not_empty, NULL) != 0) {
        pthread_mutex_destroy(&pool->queue_mutex);
        free(pool->threads);
        free(pool);
        return NULL;
    }
    
    if (pthread_cond_init(&pool->queue_not_full, NULL) != 0) {
        pthread_cond_destroy(&pool->queue_not_empty);
        pthread_mutex_destroy(&pool->queue_mutex);
        free(pool->threads);
        free(pool);
        return NULL;
    }
    
    /* Create worker threads */
    for (size_t i = 0; i < num_threads; i++) {
        if (pthread_create(&pool->threads[i], NULL, worker_thread, pool) != 0) {
            /* Shutdown already created threads */
            atomic_store(&pool->immediate_shutdown, true);
            pthread_cond_broadcast(&pool->queue_not_empty);
            
            for (size_t j = 0; j < i; j++) {
                pthread_join(pool->threads[j], NULL);
            }
            
            pthread_cond_destroy(&pool->queue_not_full);
            pthread_cond_destroy(&pool->queue_not_empty);
            pthread_mutex_destroy(&pool->queue_mutex);
            free(pool->threads);
            free(pool);
            return NULL;
        }
    }
    
    return pool;
}

/**
 * Submit a task to the thread pool.
 *
 * @param pool The thread pool
 * @param function The function to execute
 * @param arg Argument to pass to the function
 * @return Future handle for the result, or NULL on failure
 */
Future *threadpool_submit(ThreadPool *pool, void *(*function)(void *), void *arg) {
    if (!pool || !function || atomic_load(&pool->shutdown)) {
        return NULL;
    }
    
    Future *future = future_create(function, arg);
    if (!future) {
        return NULL;
    }
    
    Task *task = malloc(sizeof(Task));
    if (!task) {
        future_unref(future);
        return NULL;
    }
    
    /* Add reference for the task queue */
    future_ref(future);
    task->future = future;
    task->next = NULL;
    
    pthread_mutex_lock(&pool->queue_mutex);
    
    /* Wait if queue is full */
    while (pool->queue_size >= pool->max_queue_size && !atomic_load(&pool->shutdown)) {
        pthread_cond_wait(&pool->queue_not_full, &pool->queue_mutex);
    }
    
    if (atomic_load(&pool->shutdown)) {
        pthread_mutex_unlock(&pool->queue_mutex);
        future_unref(future); /* Remove task queue reference */
        future_unref(future); /* Remove user reference */
        free(task);
        return NULL;
    }
    
    /* Enqueue task */
    if (pool->queue_tail == NULL) {
        pool->queue_head = task;
        pool->queue_tail = task;
    } else {
        pool->queue_tail->next = task;
        pool->queue_tail = task;
    }
    pool->queue_size++;
    
    atomic_fetch_add(&pool->tasks_submitted, 1);
    
    /* Signal worker */
    pthread_cond_signal(&pool->queue_not_empty);
    
    pthread_mutex_unlock(&pool->queue_mutex);
    
    return future;
}

/**
 * Shutdown the thread pool gracefully.
 *
 * Waits for all currently executing and queued tasks to complete.
 *
 * @param pool The thread pool to shutdown
 */
void threadpool_shutdown(ThreadPool *pool) {
    if (!pool) {
        return;
    }
    
    atomic_store(&pool->shutdown, true);
    
    /* Wake all workers */
    pthread_mutex_lock(&pool->queue_mutex);
    pthread_cond_broadcast(&pool->queue_not_empty);
    pthread_cond_broadcast(&pool->queue_not_full);
    pthread_mutex_unlock(&pool->queue_mutex);
    
    /* Wait for workers to finish */
    for (size_t i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }
}

/**
 * Shutdown the thread pool immediately.
 *
 * Cancels all pending tasks and waits for running tasks to complete.
 *
 * @param pool The thread pool to shutdown
 */
void threadpool_shutdown_now(ThreadPool *pool) {
    if (!pool) {
        return;
    }
    
    atomic_store(&pool->shutdown, true);
    atomic_store(&pool->immediate_shutdown, true);
    
    /* Cancel all pending tasks */
    pthread_mutex_lock(&pool->queue_mutex);
    
    Task *task = pool->queue_head;
    while (task) {
        Task *next = task->next;
        future_cancel(task->future);
        future_unref(task->future);
        free(task);
        task = next;
    }
    pool->queue_head = NULL;
    pool->queue_tail = NULL;
    pool->queue_size = 0;
    
    pthread_cond_broadcast(&pool->queue_not_empty);
    pthread_cond_broadcast(&pool->queue_not_full);
    pthread_mutex_unlock(&pool->queue_mutex);
    
    /* Wait for workers to finish */
    for (size_t i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }
}

/**
 * Destroy the thread pool and free resources.
 *
 * Pool must be shutdown before calling this function.
 *
 * @param pool The thread pool to destroy
 */
void threadpool_destroy(ThreadPool *pool) {
    if (!pool) {
        return;
    }
    
    pthread_cond_destroy(&pool->queue_not_full);
    pthread_cond_destroy(&pool->queue_not_empty);
    pthread_mutex_destroy(&pool->queue_mutex);
    free(pool->threads);
    free(pool);
}

/**
 * Get thread pool statistics.
 *
 * @param pool The thread pool
 * @param submitted Output: number of tasks submitted
 * @param completed Output: number of tasks completed
 * @param cancelled Output: number of tasks cancelled
 * @param pending Output: number of tasks in queue
 */
void threadpool_stats(ThreadPool *pool, size_t *submitted, size_t *completed,
                      size_t *cancelled, size_t *pending) {
    if (!pool) {
        return;
    }
    
    if (submitted) *submitted = atomic_load(&pool->tasks_submitted);
    if (completed) *completed = atomic_load(&pool->tasks_completed);
    if (cancelled) *cancelled = atomic_load(&pool->tasks_cancelled);
    
    if (pending) {
        pthread_mutex_lock(&pool->queue_mutex);
        *pending = pool->queue_size;
        pthread_mutex_unlock(&pool->queue_mutex);
    }
}

/* =============================================================================
 * DEMONSTRATION FUNCTIONS
 * =============================================================================
 */

/**
 * Sample computation function - factorial.
 */
void *compute_factorial(void *arg) {
    int n = *(int *)arg;
    long long result = 1;
    
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    
    /* Simulate some work */
    usleep(100000); /* 100ms */
    
    long long *res = malloc(sizeof(long long));
    *res = result;
    return res;
}

/**
 * Sample computation function - Fibonacci.
 */
void *compute_fibonacci(void *arg) {
    int n = *(int *)arg;
    
    if (n <= 1) {
        long long *res = malloc(sizeof(long long));
        *res = n;
        return res;
    }
    
    long long a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        long long temp = a + b;
        a = b;
        b = temp;
    }
    
    /* Simulate some work */
    usleep(150000); /* 150ms */
    
    long long *res = malloc(sizeof(long long));
    *res = b;
    return res;
}

/**
 * Long-running task for timeout demonstration.
 */
void *long_running_task(void *arg) {
    (void)arg;
    sleep(5);
    return NULL;
}

/**
 * Demonstrate basic future operations.
 */
void demo_basic_futures(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: Basic Future Operations                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    ThreadPool *pool = threadpool_create(4, 16);
    if (!pool) {
        fprintf(stderr, "Failed to create thread pool\n");
        return;
    }
    
    printf("Created thread pool with 4 workers\n\n");
    
    /* Submit factorial computations */
    int args[] = {5, 10, 15, 20};
    Future *futures[4];
    
    printf("Submitting factorial computations...\n");
    for (int i = 0; i < 4; i++) {
        futures[i] = threadpool_submit(pool, compute_factorial, &args[i]);
        printf("  Submitted: factorial(%d)\n", args[i]);
    }
    
    /* Wait for results */
    printf("\nWaiting for results...\n\n");
    for (int i = 0; i < 4; i++) {
        void *result;
        int rc = future_get(futures[i], &result);
        
        if (rc == 0) {
            printf("  factorial(%d) = %lld\n", args[i], *(long long *)result);
            free(result);
        } else {
            printf("  factorial(%d) failed with error %d\n", args[i], rc);
        }
        
        future_unref(futures[i]);
    }
    
    threadpool_shutdown(pool);
    threadpool_destroy(pool);
    
    printf("\nThread pool shutdown complete\n");
}

/**
 * Demonstrate timeout functionality.
 */
void demo_timeout(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: Timeout Functionality                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    ThreadPool *pool = threadpool_create(2, 8);
    if (!pool) {
        fprintf(stderr, "Failed to create thread pool\n");
        return;
    }
    
    /* Submit a long-running task */
    printf("Submitting long-running task (5 seconds)...\n");
    Future *future = threadpool_submit(pool, long_running_task, NULL);
    
    /* Try to get result with timeout */
    printf("Attempting to get result with 500ms timeout...\n");
    void *result;
    int rc = future_get_timeout(future, &result, 500);
    
    if (rc == -2) {
        printf("  Result: TIMEOUT (as expected)\n");
    } else if (rc == 0) {
        printf("  Result: completed (unexpected)\n");
    } else {
        printf("  Result: error %d\n", rc);
    }
    
    printf("\nWaiting for task to complete naturally...\n");
    rc = future_get(future, &result);
    printf("  Task completed with return code: %d\n", rc);
    
    future_unref(future);
    
    threadpool_shutdown(pool);
    threadpool_destroy(pool);
}

/**
 * Demonstrate cancellation.
 */
void demo_cancellation(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: Task Cancellation                                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Use single worker to demonstrate pending tasks */
    ThreadPool *pool = threadpool_create(1, 16);
    if (!pool) {
        fprintf(stderr, "Failed to create thread pool\n");
        return;
    }
    
    printf("Created thread pool with 1 worker\n\n");
    
    /* Submit many tasks to fill the queue */
    int args[10];
    Future *futures[10];
    
    printf("Submitting 10 tasks (first will run, rest will queue)...\n");
    for (int i = 0; i < 10; i++) {
        args[i] = i + 5;
        futures[i] = threadpool_submit(pool, compute_factorial, &args[i]);
    }
    
    /* Give first task time to start */
    usleep(10000);
    
    /* Try to cancel tasks */
    printf("\nAttempting to cancel all tasks...\n");
    int cancelled = 0;
    for (int i = 0; i < 10; i++) {
        if (future_cancel(futures[i])) {
            printf("  Task %d (factorial(%d)): CANCELLED\n", i, args[i]);
            cancelled++;
        } else {
            printf("  Task %d (factorial(%d)): Could not cancel (already running)\n", i, args[i]);
        }
    }
    
    printf("\nTotal cancelled: %d out of 10\n", cancelled);
    
    /* Get results for non-cancelled tasks */
    printf("\nGetting results for remaining tasks...\n");
    for (int i = 0; i < 10; i++) {
        void *result;
        int rc = future_get(futures[i], &result);
        
        if (rc == 0) {
            printf("  Task %d: factorial(%d) = %lld\n", i, args[i], *(long long *)result);
            free(result);
        } else if (rc == -1) {
            printf("  Task %d: CANCELLED\n", i);
        }
        
        future_unref(futures[i]);
    }
    
    threadpool_shutdown(pool);
    threadpool_destroy(pool);
}

/**
 * Demonstrate statistics and shutdown modes.
 */
void demo_statistics(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: Statistics and Shutdown Modes                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    ThreadPool *pool = threadpool_create(2, 32);
    if (!pool) {
        fprintf(stderr, "Failed to create thread pool\n");
        return;
    }
    
    /* Submit many tasks */
    int args[20];
    Future *futures[20];
    
    printf("Submitting 20 tasks...\n");
    for (int i = 0; i < 20; i++) {
        args[i] = i + 1;
        futures[i] = threadpool_submit(pool, compute_fibonacci, &args[i]);
    }
    
    /* Check statistics while running */
    usleep(200000);
    
    size_t submitted, completed, cancelled, pending;
    threadpool_stats(pool, &submitted, &completed, &cancelled, &pending);
    
    printf("\nStatistics (mid-execution):\n");
    printf("  Submitted:  %zu\n", submitted);
    printf("  Completed:  %zu\n", completed);
    printf("  Cancelled:  %zu\n", cancelled);
    printf("  Pending:    %zu\n", pending);
    
    /* Immediate shutdown */
    printf("\nPerforming immediate shutdown...\n");
    threadpool_shutdown_now(pool);
    
    /* Final statistics */
    threadpool_stats(pool, &submitted, &completed, &cancelled, &pending);
    printf("\nFinal statistics:\n");
    printf("  Submitted:  %zu\n", submitted);
    printf("  Completed:  %zu\n", completed);
    printf("  Cancelled:  %zu\n", cancelled);
    printf("  Pending:    %zu\n", pending);
    
    /* Clean up futures */
    for (int i = 0; i < 20; i++) {
        FutureState state = future_get_state(futures[i]);
        if (state == FUTURE_COMPLETED) {
            void *result;
            future_get(futures[i], &result);
            free(result);
        }
        future_unref(futures[i]);
    }
    
    threadpool_destroy(pool);
}

/**
 * Demonstrate parallel computation pattern.
 */
void demo_parallel_computation(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: Parallel Computation Pattern                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    ThreadPool *pool = threadpool_create(4, 32);
    if (!pool) {
        fprintf(stderr, "Failed to create thread pool\n");
        return;
    }
    
    printf("Computing Fibonacci sequence in parallel...\n\n");
    
    /* Submit Fibonacci computations */
    int n_values[] = {35, 36, 37, 38, 39, 40, 41, 42};
    int count = sizeof(n_values) / sizeof(n_values[0]);
    Future **futures = malloc(count * sizeof(Future *));
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < count; i++) {
        futures[i] = threadpool_submit(pool, compute_fibonacci, &n_values[i]);
    }
    
    /* Collect results */
    printf("Results:\n");
    for (int i = 0; i < count; i++) {
        void *result;
        int rc = future_get(futures[i], &result);
        
        if (rc == 0) {
            printf("  fib(%d) = %lld\n", n_values[i], *(long long *)result);
            free(result);
        }
        
        future_unref(futures[i]);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    double elapsed = (end.tv_sec - start.tv_sec) + 
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("\nTotal time: %.3f seconds\n", elapsed);
    printf("Sequential time would be: %.3f seconds (estimated)\n", 
           count * 0.15); /* Each takes ~150ms */
    printf("Speedup: %.2fx\n", (count * 0.15) / elapsed);
    
    free(futures);
    
    threadpool_shutdown(pool);
    threadpool_destroy(pool);
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     HOMEWORK 1 SOLUTION: Thread Pool with Futures             ║\n");
    printf("║                                                               ║\n");
    printf("║     Week 20: Parallel and Concurrent Programming              ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    demo_basic_futures();
    demo_timeout();
    demo_cancellation();
    demo_statistics();
    demo_parallel_computation();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    DEMONSTRATION COMPLETE                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

/**
 * =============================================================================
 * WEEK 20: PARALLEL AND CONCURRENT PROGRAMMING
 * Complete Working Example
 * =============================================================================
 *
 * This example demonstrates:
 *   1. POSIX threads creation and management
 *   2. Mutex-based critical section protection
 *   3. Semaphores for resource counting
 *   4. Condition variables for signalling
 *   5. Producer-consumer pattern (bounded buffer)
 *   6. Thread pool implementation
 *   7. Parallel merge sort with task-based parallelism
 *   8. C11 atomics and lock-free programming
 *   9. Deadlock demonstration and prevention
 *  10. Performance analysis with Amdahl's Law
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -pthread -o example1 example1.c
 * Usage: ./example1
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
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

/* =============================================================================
 * CONSTANTS AND CONFIGURATION
 * =============================================================================
 */

#define NUM_THREADS 4
#define BUFFER_SIZE 8
#define NUM_ITEMS 20
#define ARRAY_SIZE 100000
#define PARALLEL_THRESHOLD 1000
#define THREAD_POOL_SIZE 4
#define MAX_TASKS 64

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Get current time in milliseconds for benchmarking
 */
double get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

/**
 * Sleep for specified milliseconds
 */
void sleep_ms(int ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

/**
 * Generate random integer in range [min, max]
 */
int random_range(int min, int max) {
    return min + rand() % (max - min + 1);
}

/* =============================================================================
 * PART 1: BASIC THREAD CREATION AND MANAGEMENT
 * =============================================================================
 */

/**
 * Simple thread function - prints a greeting
 */
void *thread_hello(void *arg) {
    int thread_id = *(int *)arg;
    printf("  Thread %d: Hello from thread! (pthread_self = %lu)\n", 
           thread_id, (unsigned long)pthread_self());
    
    /* Simulate some work */
    sleep_ms(random_range(50, 150));
    
    printf("  Thread %d: Goodbye!\n", thread_id);
    return NULL;
}

/**
 * Demonstrates basic thread creation and joining
 */
void demo_basic_threads(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: BASIC THREAD CREATION                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    
    printf("Creating %d threads...\n\n", NUM_THREADS);
    
    /* Create threads */
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        int rc = pthread_create(&threads[i], NULL, thread_hello, &thread_ids[i]);
        if (rc != 0) {
            fprintf(stderr, "Error: pthread_create failed with code %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }
    
    /* Wait for all threads to complete */
    printf("\nMain thread: Waiting for all threads to complete...\n\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\n[✓] All threads completed successfully.\n");
}

/* =============================================================================
 * PART 2: RACE CONDITIONS AND MUTEX PROTECTION
 * =============================================================================
 */

/* Shared counter without protection */
static int unsafe_counter = 0;

/* Shared counter with mutex protection */
static int safe_counter = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Increment counter WITHOUT synchronisation (demonstrates race condition)
 */
void *unsafe_increment(void *arg) {
    (void)arg;
    for (int i = 0; i < 100000; i++) {
        unsafe_counter++;  /* NOT atomic - race condition! */
    }
    return NULL;
}

/**
 * Increment counter WITH mutex protection (thread-safe)
 */
void *safe_increment(void *arg) {
    (void)arg;
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&counter_mutex);
        safe_counter++;  /* Protected by mutex */
        pthread_mutex_unlock(&counter_mutex);
    }
    return NULL;
}

/**
 * Demonstrates race conditions and mutex protection
 */
void demo_mutex(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: RACE CONDITIONS AND MUTEX PROTECTION             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    pthread_t threads[NUM_THREADS];
    
    /* Test 1: Unsafe increment (race condition) */
    printf("Test 1: Incrementing counter WITHOUT synchronisation\n");
    printf("  Each of %d threads increments 100,000 times\n", NUM_THREADS);
    printf("  Expected final value: %d\n\n", NUM_THREADS * 100000);
    
    unsafe_counter = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, unsafe_increment, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("  Actual value (unsafe): %d\n", unsafe_counter);
    printf("  Lost updates: %d\n\n", NUM_THREADS * 100000 - unsafe_counter);
    
    /* Test 2: Safe increment (mutex protected) */
    printf("Test 2: Incrementing counter WITH mutex protection\n");
    
    safe_counter = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, safe_increment, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("  Actual value (safe): %d\n", safe_counter);
    printf("  Lost updates: %d\n\n", NUM_THREADS * 100000 - safe_counter);
    
    printf("[✓] Mutex demonstration complete.\n");
    printf("    Notice how the unsafe version loses updates due to race conditions!\n");
}

/* =============================================================================
 * PART 3: SEMAPHORES
 * =============================================================================
 */

static sem_t resource_sem;
static int resource_in_use = 0;
static pthread_mutex_t resource_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Worker that acquires a limited resource using semaphore
 */
void *semaphore_worker(void *arg) {
    int worker_id = *(int *)arg;
    
    printf("  Worker %d: Waiting to acquire resource...\n", worker_id);
    
    /* Wait for semaphore (decrements counter) */
    sem_wait(&resource_sem);
    
    /* Update shared state safely */
    pthread_mutex_lock(&resource_mutex);
    resource_in_use++;
    printf("  Worker %d: ACQUIRED (resources in use: %d)\n", 
           worker_id, resource_in_use);
    pthread_mutex_unlock(&resource_mutex);
    
    /* Simulate using the resource */
    sleep_ms(random_range(100, 300));
    
    /* Release the resource */
    pthread_mutex_lock(&resource_mutex);
    resource_in_use--;
    printf("  Worker %d: RELEASED (resources in use: %d)\n", 
           worker_id, resource_in_use);
    pthread_mutex_unlock(&resource_mutex);
    
    /* Signal semaphore (increments counter) */
    sem_post(&resource_sem);
    
    return NULL;
}

/**
 * Demonstrates semaphore usage for resource limiting
 */
void demo_semaphores(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: SEMAPHORES FOR RESOURCE LIMITING                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    const int num_workers = 6;
    const int max_concurrent = 2;
    
    pthread_t threads[6];
    int worker_ids[6];
    
    printf("Scenario: %d workers, but only %d can use the resource at once\n\n",
           num_workers, max_concurrent);
    
    /* Initialise semaphore with max_concurrent permits */
    sem_init(&resource_sem, 0, max_concurrent);
    
    /* Create worker threads */
    for (int i = 0; i < num_workers; i++) {
        worker_ids[i] = i;
        pthread_create(&threads[i], NULL, semaphore_worker, &worker_ids[i]);
        sleep_ms(50);  /* Stagger start times for clearer output */
    }
    
    /* Wait for all workers */
    for (int i = 0; i < num_workers; i++) {
        pthread_join(threads[i], NULL);
    }
    
    sem_destroy(&resource_sem);
    
    printf("\n[✓] Semaphore demonstration complete.\n");
    printf("    Notice how at most %d workers use the resource simultaneously.\n",
           max_concurrent);
}

/* =============================================================================
 * PART 4: CONDITION VARIABLES
 * =============================================================================
 */

static pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t data_ready = PTHREAD_COND_INITIALIZER;
static bool data_available = false;
static int shared_data = 0;

/**
 * Producer thread - generates data and signals consumer
 */
void *condition_producer(void *arg) {
    (void)arg;
    
    for (int i = 1; i <= 5; i++) {
        sleep_ms(200);  /* Simulate producing data */
        
        pthread_mutex_lock(&cond_mutex);
        shared_data = i * 10;
        data_available = true;
        printf("  Producer: Generated data = %d\n", shared_data);
        pthread_cond_signal(&data_ready);
        pthread_mutex_unlock(&cond_mutex);
    }
    
    return NULL;
}

/**
 * Consumer thread - waits for data using condition variable
 */
void *condition_consumer(void *arg) {
    (void)arg;
    
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&cond_mutex);
        
        /* Wait until data is available */
        while (!data_available) {
            printf("  Consumer: Waiting for data...\n");
            pthread_cond_wait(&data_ready, &cond_mutex);
        }
        
        printf("  Consumer: Received data = %d\n", shared_data);
        data_available = false;
        
        pthread_mutex_unlock(&cond_mutex);
    }
    
    return NULL;
}

/**
 * Demonstrates condition variables for thread coordination
 */
void demo_condition_variables(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: CONDITION VARIABLES                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    pthread_t producer, consumer;
    
    printf("Starting producer-consumer with condition variable...\n\n");
    
    pthread_create(&consumer, NULL, condition_consumer, NULL);
    sleep_ms(100);  /* Let consumer start waiting first */
    pthread_create(&producer, NULL, condition_producer, NULL);
    
    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);
    
    printf("\n[✓] Condition variable demonstration complete.\n");
}

/* =============================================================================
 * PART 5: PRODUCER-CONSUMER WITH BOUNDED BUFFER
 * =============================================================================
 */

typedef struct {
    int *buffer;
    int capacity;
    int count;
    int head;
    int tail;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
    bool shutdown;
} BoundedBuffer;

/**
 * Create a new bounded buffer
 */
BoundedBuffer *buffer_create(int capacity) {
    BoundedBuffer *bb = malloc(sizeof(BoundedBuffer));
    bb->buffer = malloc(capacity * sizeof(int));
    bb->capacity = capacity;
    bb->count = 0;
    bb->head = 0;
    bb->tail = 0;
    bb->shutdown = false;
    pthread_mutex_init(&bb->mutex, NULL);
    pthread_cond_init(&bb->not_full, NULL);
    pthread_cond_init(&bb->not_empty, NULL);
    return bb;
}

/**
 * Destroy the bounded buffer
 */
void buffer_destroy(BoundedBuffer *bb) {
    pthread_mutex_destroy(&bb->mutex);
    pthread_cond_destroy(&bb->not_full);
    pthread_cond_destroy(&bb->not_empty);
    free(bb->buffer);
    free(bb);
}

/**
 * Put an item into the buffer (blocks if full)
 */
bool buffer_put(BoundedBuffer *bb, int item) {
    pthread_mutex_lock(&bb->mutex);
    
    while (bb->count == bb->capacity && !bb->shutdown) {
        pthread_cond_wait(&bb->not_full, &bb->mutex);
    }
    
    if (bb->shutdown) {
        pthread_mutex_unlock(&bb->mutex);
        return false;
    }
    
    bb->buffer[bb->tail] = item;
    bb->tail = (bb->tail + 1) % bb->capacity;
    bb->count++;
    
    pthread_cond_signal(&bb->not_empty);
    pthread_mutex_unlock(&bb->mutex);
    return true;
}

/**
 * Get an item from the buffer (blocks if empty)
 */
bool buffer_get(BoundedBuffer *bb, int *item) {
    pthread_mutex_lock(&bb->mutex);
    
    while (bb->count == 0 && !bb->shutdown) {
        pthread_cond_wait(&bb->not_empty, &bb->mutex);
    }
    
    if (bb->count == 0 && bb->shutdown) {
        pthread_mutex_unlock(&bb->mutex);
        return false;
    }
    
    *item = bb->buffer[bb->head];
    bb->head = (bb->head + 1) % bb->capacity;
    bb->count--;
    
    pthread_cond_signal(&bb->not_full);
    pthread_mutex_unlock(&bb->mutex);
    return true;
}

/**
 * Signal buffer shutdown
 */
void buffer_shutdown(BoundedBuffer *bb) {
    pthread_mutex_lock(&bb->mutex);
    bb->shutdown = true;
    pthread_cond_broadcast(&bb->not_full);
    pthread_cond_broadcast(&bb->not_empty);
    pthread_mutex_unlock(&bb->mutex);
}

/* Producer-consumer thread data */
typedef struct {
    BoundedBuffer *buffer;
    int id;
    int items_produced;
    int items_consumed;
} WorkerData;

static atomic_int total_produced = 0;

/**
 * Producer thread function
 */
void *producer_thread(void *arg) {
    WorkerData *data = (WorkerData *)arg;
    data->items_produced = 0;
    
    while (1) {
        int current = atomic_fetch_add(&total_produced, 1);
        if (current >= NUM_ITEMS) {
            atomic_fetch_sub(&total_produced, 1);
            break;
        }
        
        int item = current + 1;
        if (!buffer_put(data->buffer, item)) break;
        
        printf("  Producer %d: put item %d (buffer size: %d)\n",
               data->id, item, data->buffer->count);
        data->items_produced++;
        
        sleep_ms(random_range(10, 50));
    }
    
    return NULL;
}

/**
 * Consumer thread function
 */
void *consumer_thread(void *arg) {
    WorkerData *data = (WorkerData *)arg;
    data->items_consumed = 0;
    
    while (1) {
        int item;
        if (!buffer_get(data->buffer, &item)) break;
        
        printf("  Consumer %d: got item %d (buffer size: %d)\n",
               data->id, item, data->buffer->count);
        data->items_consumed++;
        
        sleep_ms(random_range(20, 80));
    }
    
    return NULL;
}

/**
 * Demonstrates producer-consumer pattern with bounded buffer
 */
void demo_producer_consumer(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: PRODUCER-CONSUMER WITH BOUNDED BUFFER            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    const int num_producers = 2;
    const int num_consumers = 3;
    
    BoundedBuffer *bb = buffer_create(BUFFER_SIZE);
    
    pthread_t producers[2], consumers[3];
    WorkerData prod_data[2], cons_data[3];
    
    printf("Configuration:\n");
    printf("  Buffer capacity: %d\n", BUFFER_SIZE);
    printf("  Producers: %d\n", num_producers);
    printf("  Consumers: %d\n", num_consumers);
    printf("  Items to produce: %d\n\n", NUM_ITEMS);
    
    atomic_store(&total_produced, 0);
    
    /* Start consumers first */
    for (int i = 0; i < num_consumers; i++) {
        cons_data[i].buffer = bb;
        cons_data[i].id = i;
        pthread_create(&consumers[i], NULL, consumer_thread, &cons_data[i]);
    }
    
    /* Start producers */
    for (int i = 0; i < num_producers; i++) {
        prod_data[i].buffer = bb;
        prod_data[i].id = i;
        pthread_create(&producers[i], NULL, producer_thread, &prod_data[i]);
    }
    
    /* Wait for producers to finish */
    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }
    
    /* Give consumers time to drain buffer, then signal shutdown */
    sleep_ms(500);
    buffer_shutdown(bb);
    
    /* Wait for consumers */
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumers[i], NULL);
    }
    
    printf("\nResults:\n");
    int total_prod = 0, total_cons = 0;
    for (int i = 0; i < num_producers; i++) {
        printf("  Producer %d produced: %d items\n", i, prod_data[i].items_produced);
        total_prod += prod_data[i].items_produced;
    }
    for (int i = 0; i < num_consumers; i++) {
        printf("  Consumer %d consumed: %d items\n", i, cons_data[i].items_consumed);
        total_cons += cons_data[i].items_consumed;
    }
    printf("  Total produced: %d, Total consumed: %d\n", total_prod, total_cons);
    
    buffer_destroy(bb);
    
    printf("\n[✓] Producer-consumer demonstration complete.\n");
}

/* =============================================================================
 * PART 6: THREAD POOL
 * =============================================================================
 */

typedef void (*TaskFunction)(void *);

typedef struct {
    TaskFunction function;
    void *argument;
} Task;

typedef struct {
    pthread_t *threads;
    int num_threads;
    Task *task_queue;
    int queue_size;
    int queue_head;
    int queue_tail;
    int queue_count;
    pthread_mutex_t queue_mutex;
    pthread_cond_t queue_not_empty;
    pthread_cond_t queue_not_full;
    bool shutdown;
} ThreadPool;

/**
 * Thread pool worker function
 */
void *threadpool_worker(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;
    
    while (1) {
        pthread_mutex_lock(&pool->queue_mutex);
        
        /* Wait for task or shutdown */
        while (pool->queue_count == 0 && !pool->shutdown) {
            pthread_cond_wait(&pool->queue_not_empty, &pool->queue_mutex);
        }
        
        if (pool->shutdown && pool->queue_count == 0) {
            pthread_mutex_unlock(&pool->queue_mutex);
            break;
        }
        
        /* Dequeue task */
        Task task = pool->task_queue[pool->queue_head];
        pool->queue_head = (pool->queue_head + 1) % pool->queue_size;
        pool->queue_count--;
        
        pthread_cond_signal(&pool->queue_not_full);
        pthread_mutex_unlock(&pool->queue_mutex);
        
        /* Execute task */
        task.function(task.argument);
    }
    
    return NULL;
}

/**
 * Create a thread pool
 */
ThreadPool *threadpool_create(int num_threads, int queue_size) {
    ThreadPool *pool = malloc(sizeof(ThreadPool));
    pool->num_threads = num_threads;
    pool->threads = malloc(num_threads * sizeof(pthread_t));
    pool->task_queue = malloc(queue_size * sizeof(Task));
    pool->queue_size = queue_size;
    pool->queue_head = 0;
    pool->queue_tail = 0;
    pool->queue_count = 0;
    pool->shutdown = false;
    
    pthread_mutex_init(&pool->queue_mutex, NULL);
    pthread_cond_init(&pool->queue_not_empty, NULL);
    pthread_cond_init(&pool->queue_not_full, NULL);
    
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&pool->threads[i], NULL, threadpool_worker, pool);
    }
    
    return pool;
}

/**
 * Submit a task to the thread pool
 */
bool threadpool_submit(ThreadPool *pool, TaskFunction func, void *arg) {
    pthread_mutex_lock(&pool->queue_mutex);
    
    while (pool->queue_count == pool->queue_size && !pool->shutdown) {
        pthread_cond_wait(&pool->queue_not_full, &pool->queue_mutex);
    }
    
    if (pool->shutdown) {
        pthread_mutex_unlock(&pool->queue_mutex);
        return false;
    }
    
    pool->task_queue[pool->queue_tail].function = func;
    pool->task_queue[pool->queue_tail].argument = arg;
    pool->queue_tail = (pool->queue_tail + 1) % pool->queue_size;
    pool->queue_count++;
    
    pthread_cond_signal(&pool->queue_not_empty);
    pthread_mutex_unlock(&pool->queue_mutex);
    
    return true;
}

/**
 * Shutdown and destroy the thread pool
 */
void threadpool_destroy(ThreadPool *pool) {
    pthread_mutex_lock(&pool->queue_mutex);
    pool->shutdown = true;
    pthread_cond_broadcast(&pool->queue_not_empty);
    pthread_cond_broadcast(&pool->queue_not_full);
    pthread_mutex_unlock(&pool->queue_mutex);
    
    for (int i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    
    pthread_mutex_destroy(&pool->queue_mutex);
    pthread_cond_destroy(&pool->queue_not_empty);
    pthread_cond_destroy(&pool->queue_not_full);
    free(pool->threads);
    free(pool->task_queue);
    free(pool);
}

/* Sample task for thread pool demo */
typedef struct {
    int task_id;
    int result;
} PoolTaskData;

static atomic_int completed_tasks = 0;

void sample_task(void *arg) {
    PoolTaskData *data = (PoolTaskData *)arg;
    
    /* Simulate work */
    sleep_ms(random_range(50, 150));
    data->result = data->task_id * 2;
    
    printf("  Task %d completed by thread %lu (result: %d)\n",
           data->task_id, (unsigned long)pthread_self(), data->result);
    
    atomic_fetch_add(&completed_tasks, 1);
}

/**
 * Demonstrates thread pool usage
 */
void demo_thread_pool(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: THREAD POOL                                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    const int num_tasks = 12;
    ThreadPool *pool = threadpool_create(THREAD_POOL_SIZE, MAX_TASKS);
    PoolTaskData tasks[12];
    
    printf("Thread pool created with %d workers\n", THREAD_POOL_SIZE);
    printf("Submitting %d tasks...\n\n", num_tasks);
    
    atomic_store(&completed_tasks, 0);
    
    for (int i = 0; i < num_tasks; i++) {
        tasks[i].task_id = i;
        tasks[i].result = 0;
        threadpool_submit(pool, sample_task, &tasks[i]);
    }
    
    /* Wait for all tasks to complete */
    while (atomic_load(&completed_tasks) < num_tasks) {
        sleep_ms(50);
    }
    
    printf("\n[✓] All %d tasks completed.\n", num_tasks);
    
    threadpool_destroy(pool);
}

/* =============================================================================
 * PART 7: C11 ATOMICS
 * =============================================================================
 */

static atomic_int atomic_counter = 0;

/**
 * Increment atomic counter
 */
void *atomic_increment(void *arg) {
    (void)arg;
    for (int i = 0; i < 100000; i++) {
        atomic_fetch_add(&atomic_counter, 1);
    }
    return NULL;
}

/**
 * Demonstrates C11 atomic operations
 */
void demo_atomics(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 7: C11 ATOMICS                                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    pthread_t threads[NUM_THREADS];
    
    printf("C11 Atomic Operations\n");
    printf("  Each of %d threads increments 100,000 times\n", NUM_THREADS);
    printf("  Expected: %d\n\n", NUM_THREADS * 100000);
    
    atomic_store(&atomic_counter, 0);
    
    double start = get_time_ms();
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, atomic_increment, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    double elapsed = get_time_ms() - start;
    
    printf("  Result: %d (expected: %d)\n", 
           atomic_load(&atomic_counter), NUM_THREADS * 100000);
    printf("  Time: %.2f ms\n", elapsed);
    
    /* Compare-and-swap demonstration */
    printf("\nCompare-And-Swap (CAS) Demonstration:\n");
    
    atomic_int cas_var = 100;
    int expected = 100;
    int desired = 200;
    
    printf("  Initial value: %d\n", atomic_load(&cas_var));
    printf("  Expected: %d, Desired: %d\n", expected, desired);
    
    if (atomic_compare_exchange_strong(&cas_var, &expected, desired)) {
        printf("  CAS succeeded! New value: %d\n", atomic_load(&cas_var));
    } else {
        printf("  CAS failed! Actual value was: %d\n", expected);
    }
    
    /* Try again with wrong expected value */
    expected = 100;  /* Wrong - actual is now 200 */
    desired = 300;
    
    printf("\n  Attempting CAS with wrong expected value...\n");
    printf("  Expected: %d, Desired: %d, Actual: %d\n", 
           expected, desired, atomic_load(&cas_var));
    
    if (atomic_compare_exchange_strong(&cas_var, &expected, desired)) {
        printf("  CAS succeeded!\n");
    } else {
        printf("  CAS failed as expected! (expected updated to actual: %d)\n", expected);
    }
    
    printf("\n[✓] Atomics demonstration complete.\n");
}

/* =============================================================================
 * PART 8: PARALLEL MERGE SORT
 * =============================================================================
 */

/**
 * Merge two sorted subarrays
 */
void merge(int *arr, int *temp, int left, int mid, int right) {
    int i = left, j = mid + 1, k = left;
    
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }
    
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
    
    for (i = left; i <= right; i++) {
        arr[i] = temp[i];
    }
}

/**
 * Sequential merge sort
 */
void merge_sort_sequential(int *arr, int *temp, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort_sequential(arr, temp, left, mid);
        merge_sort_sequential(arr, temp, mid + 1, right);
        merge(arr, temp, left, mid, right);
    }
}

/* Parallel merge sort structures */
typedef struct {
    int *arr;
    int *temp;
    int left;
    int right;
    int depth;
} MergeSortTask;

static atomic_int active_threads = 0;
static int max_depth = 2;  /* Limit parallelism depth */

/**
 * Parallel merge sort worker
 */
void *parallel_merge_sort_worker(void *arg) {
    MergeSortTask *task = (MergeSortTask *)arg;
    
    if (task->left < task->right) {
        int mid = task->left + (task->right - task->left) / 2;
        int size = task->right - task->left + 1;
        
        /* Parallelize only for large subarrays and if depth allows */
        if (size > PARALLEL_THRESHOLD && task->depth < max_depth &&
            atomic_load(&active_threads) < NUM_THREADS) {
            
            atomic_fetch_add(&active_threads, 2);
            
            MergeSortTask left_task = {
                task->arr, task->temp, task->left, mid, task->depth + 1
            };
            MergeSortTask right_task = {
                task->arr, task->temp, mid + 1, task->right, task->depth + 1
            };
            
            pthread_t left_thread, right_thread;
            pthread_create(&left_thread, NULL, parallel_merge_sort_worker, &left_task);
            pthread_create(&right_thread, NULL, parallel_merge_sort_worker, &right_task);
            
            pthread_join(left_thread, NULL);
            pthread_join(right_thread, NULL);
            
            atomic_fetch_sub(&active_threads, 2);
        } else {
            /* Sequential sort for small subarrays */
            merge_sort_sequential(task->arr, task->temp, task->left, mid);
            merge_sort_sequential(task->arr, task->temp, mid + 1, task->right);
        }
        
        merge(task->arr, task->temp, task->left, mid, task->right);
    }
    
    return NULL;
}

/**
 * Parallel merge sort entry point
 */
void parallel_merge_sort(int *arr, int n) {
    int *temp = malloc(n * sizeof(int));
    atomic_store(&active_threads, 0);
    
    MergeSortTask task = {arr, temp, 0, n - 1, 0};
    parallel_merge_sort_worker(&task);
    
    free(temp);
}

/**
 * Check if array is sorted
 */
bool is_sorted(int *arr, int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i] < arr[i - 1]) return false;
    }
    return true;
}

/**
 * Demonstrates parallel merge sort
 */
void demo_parallel_sort(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 8: PARALLEL MERGE SORT                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int n = ARRAY_SIZE;
    int *arr1 = malloc(n * sizeof(int));
    int *arr2 = malloc(n * sizeof(int));
    int *temp = malloc(n * sizeof(int));
    
    /* Initialize with random data */
    srand(42);
    for (int i = 0; i < n; i++) {
        arr1[i] = rand() % 1000000;
        arr2[i] = arr1[i];
    }
    
    printf("Array size: %d elements\n", n);
    printf("Threshold for parallelism: %d elements\n\n", PARALLEL_THRESHOLD);
    
    /* Sequential sort */
    printf("Sequential Merge Sort:\n");
    double start = get_time_ms();
    merge_sort_sequential(arr1, temp, 0, n - 1);
    double seq_time = get_time_ms() - start;
    printf("  Time: %.2f ms\n", seq_time);
    printf("  Sorted: %s\n\n", is_sorted(arr1, n) ? "YES" : "NO");
    
    /* Parallel sort */
    printf("Parallel Merge Sort:\n");
    start = get_time_ms();
    parallel_merge_sort(arr2, n);
    double par_time = get_time_ms() - start;
    printf("  Time: %.2f ms\n", par_time);
    printf("  Sorted: %s\n", is_sorted(arr2, n) ? "YES" : "NO");
    printf("  Speedup: %.2fx\n", seq_time / par_time);
    
    free(arr1);
    free(arr2);
    free(temp);
    
    printf("\n[✓] Parallel sort demonstration complete.\n");
}

/* =============================================================================
 * PART 9: AMDAHL'S LAW DEMONSTRATION
 * =============================================================================
 */

/**
 * Simulates parallel work with configurable serial fraction
 */
double simulate_parallel_work(double total_work_ms, double serial_fraction, int num_threads) {
    double serial_time = total_work_ms * serial_fraction;
    double parallel_time = total_work_ms * (1 - serial_fraction) / num_threads;
    return serial_time + parallel_time;
}

/**
 * Demonstrates Amdahl's Law
 */
void demo_amdahl(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 9: AMDAHL'S LAW                                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    double total_work = 1000.0;  /* milliseconds */
    
    printf("Amdahl's Law: S(n) = 1 / ((1-P) + P/n)\n");
    printf("Where P = parallel fraction, n = number of processors\n\n");
    
    printf("Theoretical speedup for 1000ms workload:\n\n");
    printf("╔════════════╦════════════════════════════════════════════════════╗\n");
    printf("║   Threads  ║     Parallel Fraction (P)                          ║\n");
    printf("║            ╠══════════╦══════════╦══════════╦══════════╦════════╣\n");
    printf("║            ║   50%%    ║   75%%    ║   90%%    ║   95%%    ║  99%%  ║\n");
    printf("╠════════════╬══════════╬══════════╬══════════╬══════════╬════════╣\n");
    
    int thread_counts[] = {1, 2, 4, 8, 16, 32, 64, 128};
    double parallel_fractions[] = {0.50, 0.75, 0.90, 0.95, 0.99};
    
    for (int i = 0; i < 8; i++) {
        int n = thread_counts[i];
        printf("║    %3d     ║", n);
        
        for (int j = 0; j < 5; j++) {
            double p = parallel_fractions[j];
            double speedup = 1.0 / ((1 - p) + p / n);
            printf("  %5.2fx  ║", speedup);
        }
        printf("\n");
    }
    
    printf("╚════════════╩══════════╩══════════╩══════════╩══════════╩════════╝\n");
    
    printf("\nKey Insight:\n");
    printf("  Even with 95%% parallelizable code, maximum speedup is only 20x!\n");
    printf("  The serial portion becomes the bottleneck as thread count increases.\n");
    
    printf("\n[✓] Amdahl's Law demonstration complete.\n");
}

/* =============================================================================
 * PART 10: DEADLOCK DEMONSTRATION (Dining Philosophers)
 * =============================================================================
 */

#define NUM_PHILOSOPHERS 5

static pthread_mutex_t forks[NUM_PHILOSOPHERS];
static bool deadlock_prevention = false;

/**
 * Philosopher thread (potential deadlock version)
 */
void *philosopher_thread(void *arg) {
    int id = *(int *)arg;
    int left_fork = id;
    int right_fork = (id + 1) % NUM_PHILOSOPHERS;
    
    /* Deadlock prevention: always pick up lower-numbered fork first */
    if (deadlock_prevention && left_fork > right_fork) {
        int temp = left_fork;
        left_fork = right_fork;
        right_fork = temp;
    }
    
    for (int i = 0; i < 3; i++) {
        /* Think */
        printf("  Philosopher %d: thinking...\n", id);
        sleep_ms(random_range(10, 50));
        
        /* Pick up forks */
        printf("  Philosopher %d: picking up fork %d\n", id, left_fork);
        pthread_mutex_lock(&forks[left_fork]);
        
        sleep_ms(10);  /* Increase deadlock probability */
        
        printf("  Philosopher %d: picking up fork %d\n", id, right_fork);
        pthread_mutex_lock(&forks[right_fork]);
        
        /* Eat */
        printf("  Philosopher %d: EATING (has forks %d and %d)\n", 
               id, left_fork, right_fork);
        sleep_ms(random_range(20, 50));
        
        /* Put down forks */
        pthread_mutex_unlock(&forks[right_fork]);
        pthread_mutex_unlock(&forks[left_fork]);
        printf("  Philosopher %d: put down forks\n", id);
    }
    
    printf("  Philosopher %d: DONE\n", id);
    return NULL;
}

/**
 * Demonstrates deadlock (and prevention)
 */
void demo_deadlock(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 10: DEADLOCK (DINING PHILOSOPHERS)                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];
    
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }
    
    printf("Dining Philosophers Problem:\n");
    printf("  %d philosophers sitting around a table with %d forks\n", 
           NUM_PHILOSOPHERS, NUM_PHILOSOPHERS);
    printf("  Each needs 2 forks to eat\n\n");
    
    /* Run with deadlock prevention enabled */
    deadlock_prevention = true;
    printf("Running WITH deadlock prevention (resource ordering):\n\n");
    
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher_thread, &ids[i]);
    }
    
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }
    
    printf("\n[✓] All philosophers finished eating (no deadlock).\n");
    printf("\nDeadlock prevention strategy: Resource ordering\n");
    printf("  Each philosopher picks up the lower-numbered fork first.\n");
    printf("  This breaks the circular wait condition.\n");
    
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&forks[i]);
    }
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 20: PARALLEL AND CONCURRENT PROGRAMMING              ║\n");
    printf("║                    Complete Example                           ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Initialize random seed */
    srand(time(NULL));
    
    /* Run all demonstrations */
    demo_basic_threads();
    demo_mutex();
    demo_semaphores();
    demo_condition_variables();
    demo_producer_consumer();
    demo_thread_pool();
    demo_atomics();
    demo_parallel_sort();
    demo_amdahl();
    demo_deadlock();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    DEMONSTRATION COMPLETE                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

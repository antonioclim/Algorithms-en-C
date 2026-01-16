/**
 * =============================================================================
 * WEEK 20: PARALLEL AND CONCURRENT PROGRAMMING
 * Exercise 1: Multi-Threaded Log Processor
 * =============================================================================
 *
 * Implement a producer-consumer system for processing log files.
 * Multiple producer threads read log entries from files, multiple
 * consumer threads process and categorise the entries.
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -pthread -o exercise1 exercise1.c
 * Usage: ./exercise1 [logfile1] [logfile2] ...
 *
 * =============================================================================
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

/* =============================================================================
 * CONFIGURATION
 * =============================================================================
 */

#define MAX_LINE_LENGTH 512
#define BUFFER_SIZE 16
#define NUM_CONSUMERS 4
#define MAX_FILES 10

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

/**
 * Log entry structure
 */
typedef struct {
    char message[MAX_LINE_LENGTH];
    int level;          /* 0=DEBUG, 1=INFO, 2=WARN, 3=ERROR */
    int source_file;    /* Which file this came from */
} LogEntry;

/**
 * Bounded buffer for log entries
 *
 * TODO: Complete this structure
 * - Add buffer array
 * - Add synchronisation primitives (mutex, condition variables)
 * - Add buffer management fields (head, tail, count)
 */
typedef struct {
    LogEntry *entries;          /* Array of log entries */
    int capacity;               /* Maximum number of entries */
    int count;                  /* Current number of entries */
    int head;                   /* Read position */
    int tail;                   /* Write position */
    
    /* TODO: Add synchronisation primitives */
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
    
    bool shutdown;              /* Signal for termination */
} LogBuffer;

/**
 * Producer thread data
 */
typedef struct {
    LogBuffer *buffer;
    const char *filename;
    int producer_id;
    int entries_read;
} ProducerData;

/**
 * Consumer thread data
 */
typedef struct {
    LogBuffer *buffer;
    int consumer_id;
    int entries_processed;
    int count_debug;
    int count_info;
    int count_warn;
    int count_error;
} ConsumerData;

/**
 * Global statistics
 */
typedef struct {
    atomic_int total_produced;
    atomic_int total_consumed;
    atomic_int debug_count;
    atomic_int info_count;
    atomic_int warn_count;
    atomic_int error_count;
    double start_time;
    double end_time;
} Statistics;

static Statistics stats;

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Get current time in seconds
 */
double get_time_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1000000000.0;
}

/**
 * Parse log level from log line
 * Returns: 0=DEBUG, 1=INFO, 2=WARN, 3=ERROR, -1=UNKNOWN
 */
int parse_log_level(const char *line) {
    if (strstr(line, "DEBUG") || strstr(line, "debug")) return 0;
    if (strstr(line, "INFO") || strstr(line, "info")) return 1;
    if (strstr(line, "WARN") || strstr(line, "warn")) return 2;
    if (strstr(line, "ERROR") || strstr(line, "error")) return 3;
    return 1;  /* Default to INFO */
}

/**
 * Get log level name
 */
const char *level_name(int level) {
    switch (level) {
        case 0: return "DEBUG";
        case 1: return "INFO";
        case 2: return "WARN";
        case 3: return "ERROR";
        default: return "UNKNOWN";
    }
}

/* =============================================================================
 * BUFFER OPERATIONS
 * =============================================================================
 */

/**
 * Create a new log buffer
 *
 * TODO: Implement this function
 * - Allocate memory for the buffer structure
 * - Allocate memory for the entry array
 * - Initialize all synchronisation primitives
 * - Initialize buffer management fields
 */
LogBuffer *buffer_create(int capacity) {
    LogBuffer *buf = malloc(sizeof(LogBuffer));
    if (!buf) return NULL;
    
    buf->entries = malloc(capacity * sizeof(LogEntry));
    if (!buf->entries) {
        free(buf);
        return NULL;
    }
    
    buf->capacity = capacity;
    buf->count = 0;
    buf->head = 0;
    buf->tail = 0;
    buf->shutdown = false;
    
    /* TODO: Initialize synchronisation primitives */
    pthread_mutex_init(&buf->mutex, NULL);
    pthread_cond_init(&buf->not_full, NULL);
    pthread_cond_init(&buf->not_empty, NULL);
    
    return buf;
}

/**
 * Destroy the log buffer
 *
 * TODO: Implement this function
 * - Destroy all synchronisation primitives
 * - Free allocated memory
 */
void buffer_destroy(LogBuffer *buf) {
    if (!buf) return;
    
    /* TODO: Destroy synchronisation primitives */
    pthread_mutex_destroy(&buf->mutex);
    pthread_cond_destroy(&buf->not_full);
    pthread_cond_destroy(&buf->not_empty);
    
    free(buf->entries);
    free(buf);
}

/**
 * Put a log entry into the buffer
 *
 * TODO: Implement this function
 * - Lock the mutex
 * - Wait while buffer is full (use condition variable)
 * - Add entry to buffer
 * - Update head/tail pointers
 * - Signal that buffer is not empty
 * - Unlock mutex
 *
 * Returns: true on success, false on shutdown
 */
bool buffer_put(LogBuffer *buf, const LogEntry *entry) {
    pthread_mutex_lock(&buf->mutex);
    
    /* TODO: Wait while buffer is full */
    while (buf->count == buf->capacity && !buf->shutdown) {
        pthread_cond_wait(&buf->not_full, &buf->mutex);
    }
    
    if (buf->shutdown) {
        pthread_mutex_unlock(&buf->mutex);
        return false;
    }
    
    /* TODO: Add entry to buffer */
    buf->entries[buf->tail] = *entry;
    buf->tail = (buf->tail + 1) % buf->capacity;
    buf->count++;
    
    /* TODO: Signal not empty */
    pthread_cond_signal(&buf->not_empty);
    
    pthread_mutex_unlock(&buf->mutex);
    return true;
}

/**
 * Get a log entry from the buffer
 *
 * TODO: Implement this function
 * - Lock the mutex
 * - Wait while buffer is empty (use condition variable)
 * - Remove entry from buffer
 * - Update head/tail pointers
 * - Signal that buffer is not full
 * - Unlock mutex
 *
 * Returns: true on success, false on shutdown with empty buffer
 */
bool buffer_get(LogBuffer *buf, LogEntry *entry) {
    pthread_mutex_lock(&buf->mutex);
    
    /* TODO: Wait while buffer is empty */
    while (buf->count == 0 && !buf->shutdown) {
        pthread_cond_wait(&buf->not_empty, &buf->mutex);
    }
    
    if (buf->count == 0 && buf->shutdown) {
        pthread_mutex_unlock(&buf->mutex);
        return false;
    }
    
    /* TODO: Remove entry from buffer */
    *entry = buf->entries[buf->head];
    buf->head = (buf->head + 1) % buf->capacity;
    buf->count--;
    
    /* TODO: Signal not full */
    pthread_cond_signal(&buf->not_full);
    
    pthread_mutex_unlock(&buf->mutex);
    return true;
}

/**
 * Signal buffer shutdown
 */
void buffer_shutdown(LogBuffer *buf) {
    pthread_mutex_lock(&buf->mutex);
    buf->shutdown = true;
    pthread_cond_broadcast(&buf->not_full);
    pthread_cond_broadcast(&buf->not_empty);
    pthread_mutex_unlock(&buf->mutex);
}

/* =============================================================================
 * PRODUCER THREAD
 * =============================================================================
 */

/**
 * Producer thread function
 *
 * Reads log entries from a file and puts them into the buffer.
 *
 * TODO: Implement this function
 * - Open the input file
 * - Read lines and parse into LogEntry structures
 * - Put each entry into the buffer
 * - Update statistics
 * - Close the file
 */
void *producer_thread(void *arg) {
    ProducerData *data = (ProducerData *)arg;
    data->entries_read = 0;
    
    FILE *file = fopen(data->filename, "r");
    if (!file) {
        fprintf(stderr, "Producer %d: Cannot open file %s\n", 
                data->producer_id, data->filename);
        return NULL;
    }
    
    char line[MAX_LINE_LENGTH];
    
    /* TODO: Read file and produce entries */
    while (fgets(line, sizeof(line), file)) {
        /* Remove trailing newline */
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        /* Skip empty lines */
        if (strlen(line) == 0) continue;
        
        /* Create log entry */
        LogEntry entry;
        strncpy(entry.message, line, MAX_LINE_LENGTH - 1);
        entry.message[MAX_LINE_LENGTH - 1] = '\0';
        entry.level = parse_log_level(line);
        entry.source_file = data->producer_id;
        
        /* Put into buffer */
        if (!buffer_put(data->buffer, &entry)) {
            break;  /* Shutdown signalled */
        }
        
        data->entries_read++;
        atomic_fetch_add(&stats.total_produced, 1);
    }
    
    fclose(file);
    
    printf("Producer %d: Read %d entries from %s\n", 
           data->producer_id, data->entries_read, data->filename);
    
    return NULL;
}

/* =============================================================================
 * CONSUMER THREAD
 * =============================================================================
 */

/**
 * Consumer thread function
 *
 * Gets log entries from the buffer and processes them.
 *
 * TODO: Implement this function
 * - Get entries from the buffer
 * - Categorise by log level
 * - Update local and global statistics
 */
void *consumer_thread(void *arg) {
    ConsumerData *data = (ConsumerData *)arg;
    data->entries_processed = 0;
    data->count_debug = 0;
    data->count_info = 0;
    data->count_warn = 0;
    data->count_error = 0;
    
    LogEntry entry;
    
    /* TODO: Process entries from buffer */
    while (buffer_get(data->buffer, &entry)) {
        /* Categorise by log level */
        switch (entry.level) {
            case 0:
                data->count_debug++;
                atomic_fetch_add(&stats.debug_count, 1);
                break;
            case 1:
                data->count_info++;
                atomic_fetch_add(&stats.info_count, 1);
                break;
            case 2:
                data->count_warn++;
                atomic_fetch_add(&stats.warn_count, 1);
                break;
            case 3:
                data->count_error++;
                atomic_fetch_add(&stats.error_count, 1);
                break;
        }
        
        data->entries_processed++;
        atomic_fetch_add(&stats.total_consumed, 1);
    }
    
    printf("Consumer %d: Processed %d entries (DEBUG:%d INFO:%d WARN:%d ERROR:%d)\n",
           data->consumer_id, data->entries_processed,
           data->count_debug, data->count_info, 
           data->count_warn, data->count_error);
    
    return NULL;
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

/**
 * Print usage information
 */
void print_usage(const char *program_name) {
    printf("Usage: %s [logfile1] [logfile2] ...\n\n", program_name);
    printf("Multi-threaded log processor using producer-consumer pattern.\n\n");
    printf("If no files specified, generates sample log data.\n");
}

/**
 * Generate sample log file for testing
 */
void generate_sample_log(const char *filename, int num_entries) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Cannot create sample file %s\n", filename);
        return;
    }
    
    const char *levels[] = {"DEBUG", "INFO", "WARN", "ERROR"};
    const char *messages[] = {
        "Application started",
        "Processing request",
        "Database connection established",
        "User logged in",
        "File not found",
        "Memory usage high",
        "Connection timeout",
        "Invalid input data",
        "Transaction completed",
        "Cache miss"
    };
    
    for (int i = 0; i < num_entries; i++) {
        int level = rand() % 4;
        int msg_idx = rand() % 10;
        fprintf(file, "[%s] %s - entry %d\n", 
                levels[level], messages[msg_idx], i);
    }
    
    fclose(file);
    printf("Generated %d log entries in %s\n", num_entries, filename);
}

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 1: MULTI-THREADED LOG PROCESSOR                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Initialize random seed */
    srand(time(NULL));
    
    /* Determine input files */
    int num_files;
    char *filenames[MAX_FILES];
    char sample_files[MAX_FILES][256];
    
    if (argc > 1) {
        /* Use command line arguments */
        num_files = (argc - 1 > MAX_FILES) ? MAX_FILES : argc - 1;
        for (int i = 0; i < num_files; i++) {
            filenames[i] = argv[i + 1];
        }
    } else {
        /* Generate sample files */
        printf("No input files specified. Generating sample data...\n\n");
        num_files = 2;
        for (int i = 0; i < num_files; i++) {
            snprintf(sample_files[i], sizeof(sample_files[i]), 
                    "/tmp/sample_log_%d.txt", i);
            filenames[i] = sample_files[i];
            generate_sample_log(filenames[i], 500 + rand() % 500);
        }
        printf("\n");
    }
    
    /* Initialize statistics */
    atomic_store(&stats.total_produced, 0);
    atomic_store(&stats.total_consumed, 0);
    atomic_store(&stats.debug_count, 0);
    atomic_store(&stats.info_count, 0);
    atomic_store(&stats.warn_count, 0);
    atomic_store(&stats.error_count, 0);
    
    /* Create buffer */
    LogBuffer *buffer = buffer_create(BUFFER_SIZE);
    if (!buffer) {
        fprintf(stderr, "Failed to create buffer\n");
        return EXIT_FAILURE;
    }
    
    printf("Configuration:\n");
    printf("  Buffer size: %d\n", BUFFER_SIZE);
    printf("  Producer threads: %d (one per file)\n", num_files);
    printf("  Consumer threads: %d\n", NUM_CONSUMERS);
    printf("  Input files:\n");
    for (int i = 0; i < num_files; i++) {
        printf("    [%d] %s\n", i, filenames[i]);
    }
    printf("\n");
    
    /* Create thread arrays */
    pthread_t *producers = malloc(num_files * sizeof(pthread_t));
    pthread_t consumers[NUM_CONSUMERS];
    ProducerData *prod_data = malloc(num_files * sizeof(ProducerData));
    ConsumerData cons_data[NUM_CONSUMERS];
    
    stats.start_time = get_time_sec();
    
    /* Start consumer threads */
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        cons_data[i].buffer = buffer;
        cons_data[i].consumer_id = i;
        pthread_create(&consumers[i], NULL, consumer_thread, &cons_data[i]);
    }
    
    /* Start producer threads */
    for (int i = 0; i < num_files; i++) {
        prod_data[i].buffer = buffer;
        prod_data[i].filename = filenames[i];
        prod_data[i].producer_id = i;
        pthread_create(&producers[i], NULL, producer_thread, &prod_data[i]);
    }
    
    /* Wait for producers to finish */
    for (int i = 0; i < num_files; i++) {
        pthread_join(producers[i], NULL);
    }
    
    /* Signal shutdown and wait for consumers */
    printf("\nAll producers finished. Signalling shutdown...\n");
    buffer_shutdown(buffer);
    
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }
    
    stats.end_time = get_time_sec();
    
    /* Print results */
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                         RESULTS                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Processing Statistics:\n");
    printf("  Total produced:  %d entries\n", atomic_load(&stats.total_produced));
    printf("  Total consumed:  %d entries\n", atomic_load(&stats.total_consumed));
    printf("\n");
    printf("Log Level Distribution:\n");
    printf("  DEBUG: %d\n", atomic_load(&stats.debug_count));
    printf("  INFO:  %d\n", atomic_load(&stats.info_count));
    printf("  WARN:  %d\n", atomic_load(&stats.warn_count));
    printf("  ERROR: %d\n", atomic_load(&stats.error_count));
    printf("\n");
    
    double elapsed = stats.end_time - stats.start_time;
    double throughput = atomic_load(&stats.total_consumed) / elapsed;
    
    printf("Performance:\n");
    printf("  Time elapsed:    %.3f seconds\n", elapsed);
    printf("  Throughput:      %.0f entries/second\n", throughput);
    printf("\n");
    
    /* Verify correctness */
    printf("SORTED: %s\n", 
           atomic_load(&stats.total_produced) == atomic_load(&stats.total_consumed) 
           ? "YES" : "NO");
    
    /* Cleanup */
    buffer_destroy(buffer);
    free(producers);
    free(prod_data);
    
    /* Remove sample files if generated */
    if (argc <= 1) {
        for (int i = 0; i < num_files; i++) {
            remove(sample_files[i]);
        }
    }
    
    return 0;
}

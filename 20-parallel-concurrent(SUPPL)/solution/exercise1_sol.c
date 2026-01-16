/**
 * =============================================================================
 * WEEK 20: PARALLEL AND CONCURRENT PROGRAMMING
 * Exercise 1 Solution: Producer-Consumer Log Processor
 * =============================================================================
 *
 * Complete implementation of a multi-threaded log processor using the
 * producer-consumer pattern with bounded buffer synchronisation.
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -pthread -o exercise1_sol exercise1_sol.c
 * Usage: ./exercise1_sol <log_file1> [log_file2] ...
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
#include <unistd.h>
#include <time.h>
#include <errno.h>

/* =============================================================================
 * CONFIGURATION CONSTANTS
 * =============================================================================
 */

#define MAX_LINE_LENGTH     1024
#define DEFAULT_BUFFER_SIZE 32
#define MAX_PRODUCERS       8
#define MAX_CONSUMERS       16
#define MAX_LOG_FILES       32

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

/**
 * Log entry structure representing a single parsed log line
 */
typedef struct {
    char     timestamp[64];
    char     level[16];
    char     message[MAX_LINE_LENGTH];
    char     source_file[256];
    uint32_t line_number;
} LogEntry;

/**
 * Log level enumeration for categorisation
 */
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_HTTP,
    LOG_UNKNOWN
} LogLevel;

/**
 * Thread-safe bounded buffer for producer-consumer communication
 */
typedef struct {
    LogEntry       **entries;
    size_t           capacity;
    size_t           count;
    size_t           head;
    size_t           tail;
    pthread_mutex_t  mutex;
    pthread_cond_t   not_full;
    pthread_cond_t   not_empty;
    bool             shutdown;
} BoundedBuffer;

/**
 * Statistics structure with atomic counters
 */
typedef struct {
    atomic_size_t total_entries;
    atomic_size_t debug_count;
    atomic_size_t info_count;
    atomic_size_t warn_count;
    atomic_size_t error_count;
    atomic_size_t http_count;
    atomic_size_t unknown_count;
    atomic_size_t files_processed;
} Statistics;

/**
 * Producer thread argument structure
 */
typedef struct {
    int            producer_id;
    const char    *filename;
    BoundedBuffer *buffer;
    Statistics    *stats;
} ProducerArg;

/**
 * Consumer thread argument structure
 */
typedef struct {
    int            consumer_id;
    BoundedBuffer *buffer;
    Statistics    *stats;
} ConsumerArg;

/* =============================================================================
 * BOUNDED BUFFER IMPLEMENTATION
 * =============================================================================
 */

/**
 * Creates a new bounded buffer with specified capacity
 *
 * @param capacity Maximum number of entries the buffer can hold
 * @return Pointer to newly created buffer, or NULL on failure
 */
BoundedBuffer *buffer_create(size_t capacity) {
    BoundedBuffer *buf = malloc(sizeof(BoundedBuffer));
    if (!buf) return NULL;

    buf->entries = calloc(capacity, sizeof(LogEntry *));
    if (!buf->entries) {
        free(buf);
        return NULL;
    }

    buf->capacity = capacity;
    buf->count = 0;
    buf->head = 0;
    buf->tail = 0;
    buf->shutdown = false;

    pthread_mutex_init(&buf->mutex, NULL);
    pthread_cond_init(&buf->not_full, NULL);
    pthread_cond_init(&buf->not_empty, NULL);

    return buf;
}

/**
 * Destroys a bounded buffer and frees all resources
 *
 * @param buf Buffer to destroy
 */
void buffer_destroy(BoundedBuffer *buf) {
    if (!buf) return;

    pthread_mutex_lock(&buf->mutex);
    
    /* Free any remaining entries */
    while (buf->count > 0) {
        free(buf->entries[buf->head]);
        buf->head = (buf->head + 1) % buf->capacity;
        buf->count--;
    }

    pthread_mutex_unlock(&buf->mutex);

    pthread_mutex_destroy(&buf->mutex);
    pthread_cond_destroy(&buf->not_full);
    pthread_cond_destroy(&buf->not_empty);

    free(buf->entries);
    free(buf);
}

/**
 * Adds an entry to the buffer (producer operation)
 * Blocks if buffer is full until space becomes available or shutdown
 *
 * @param buf    Target buffer
 * @param entry  Entry to add (ownership transferred to buffer)
 * @return true if entry was added, false if shutdown signalled
 */
bool buffer_put(BoundedBuffer *buf, LogEntry *entry) {
    pthread_mutex_lock(&buf->mutex);

    /* Wait while buffer is full and not shutting down */
    while (buf->count == buf->capacity && !buf->shutdown) {
        pthread_cond_wait(&buf->not_full, &buf->mutex);
    }

    if (buf->shutdown) {
        pthread_mutex_unlock(&buf->mutex);
        return false;
    }

    /* Add entry at tail */
    buf->entries[buf->tail] = entry;
    buf->tail = (buf->tail + 1) % buf->capacity;
    buf->count++;

    /* Signal waiting consumers */
    pthread_cond_signal(&buf->not_empty);
    pthread_mutex_unlock(&buf->mutex);

    return true;
}

/**
 * Retrieves an entry from the buffer (consumer operation)
 * Blocks if buffer is empty until entry becomes available or shutdown
 *
 * @param buf Target buffer
 * @return Retrieved entry (ownership transferred to caller), or NULL if shutdown
 */
LogEntry *buffer_get(BoundedBuffer *buf) {
    pthread_mutex_lock(&buf->mutex);

    /* Wait while buffer is empty and not shutting down */
    while (buf->count == 0 && !buf->shutdown) {
        pthread_cond_wait(&buf->not_empty, &buf->mutex);
    }

    if (buf->count == 0 && buf->shutdown) {
        pthread_mutex_unlock(&buf->mutex);
        return NULL;
    }

    /* Remove entry from head */
    LogEntry *entry = buf->entries[buf->head];
    buf->entries[buf->head] = NULL;
    buf->head = (buf->head + 1) % buf->capacity;
    buf->count--;

    /* Signal waiting producers */
    pthread_cond_signal(&buf->not_full);
    pthread_mutex_unlock(&buf->mutex);

    return entry;
}

/**
 * Signals shutdown to all waiting threads
 *
 * @param buf Buffer to shut down
 */
void buffer_shutdown(BoundedBuffer *buf) {
    pthread_mutex_lock(&buf->mutex);
    buf->shutdown = true;
    pthread_cond_broadcast(&buf->not_empty);
    pthread_cond_broadcast(&buf->not_full);
    pthread_mutex_unlock(&buf->mutex);
}

/* =============================================================================
 * LOG PARSING FUNCTIONS
 * =============================================================================
 */

/**
 * Determines the log level from a string representation
 *
 * @param level_str String representation of log level
 * @return Corresponding LogLevel enum value
 */
LogLevel parse_log_level(const char *level_str) {
    if (strstr(level_str, "DEBUG")) return LOG_DEBUG;
    if (strstr(level_str, "INFO"))  return LOG_INFO;
    if (strstr(level_str, "WARN"))  return LOG_WARN;
    if (strstr(level_str, "ERROR")) return LOG_ERROR;
    if (strstr(level_str, "GET") || strstr(level_str, "POST") ||
        strstr(level_str, "PUT") || strstr(level_str, "DELETE")) {
        return LOG_HTTP;
    }
    return LOG_UNKNOWN;
}

/**
 * Parses an error log line format: [timestamp] [LEVEL] message
 *
 * @param line       Raw log line
 * @param entry      Output entry structure
 * @param filename   Source filename
 * @param line_num   Line number in source file
 * @return true if parsing succeeded
 */
bool parse_error_log_line(const char *line, LogEntry *entry,
                          const char *filename, uint32_t line_num) {
    /* Format: [2026-01-15 10:15:32] [DEBUG] message */
    char level[32];
    
    if (sscanf(line, "[%63[^]]] [%31[^]]] %[^\n]",
               entry->timestamp, level, entry->message) >= 2) {
        strncpy(entry->level, level, sizeof(entry->level) - 1);
        strncpy(entry->source_file, filename, sizeof(entry->source_file) - 1);
        entry->line_number = line_num;
        return true;
    }
    return false;
}

/**
 * Parses an access log line (Apache combined format)
 *
 * @param line       Raw log line
 * @param entry      Output entry structure
 * @param filename   Source filename
 * @param line_num   Line number in source file
 * @return true if parsing succeeded
 */
bool parse_access_log_line(const char *line, LogEntry *entry,
                           const char *filename, uint32_t line_num) {
    /* Format: IP - - [timestamp] "METHOD /path HTTP/1.1" status size */
    char ip[64], method[16], path[256];
    int status;
    
    if (sscanf(line, "%63s %*s %*s [%63[^]]] \"%15s %255s %*[^\"]\" %d",
               ip, entry->timestamp, method, path, &status) >= 4) {
        snprintf(entry->level, sizeof(entry->level), "%s", method);
        snprintf(entry->message, sizeof(entry->message),
                 "%s %s -> %d", method, path, status);
        strncpy(entry->source_file, filename, sizeof(entry->source_file) - 1);
        entry->line_number = line_num;
        return true;
    }
    return false;
}

/**
 * Parses a log line, auto-detecting format
 *
 * @param line       Raw log line
 * @param entry      Output entry structure
 * @param filename   Source filename
 * @param line_num   Line number in source file
 * @return true if parsing succeeded
 */
bool parse_log_line(const char *line, LogEntry *entry,
                    const char *filename, uint32_t line_num) {
    /* Skip empty lines and comments */
    if (line[0] == '\0' || line[0] == '#' || line[0] == '\n') {
        return false;
    }

    /* Try error log format first */
    if (line[0] == '[') {
        return parse_error_log_line(line, entry, filename, line_num);
    }

    /* Try access log format */
    return parse_access_log_line(line, entry, filename, line_num);
}

/* =============================================================================
 * PRODUCER THREAD
 * =============================================================================
 */

/**
 * Producer thread function: reads log file and produces entries
 *
 * @param arg ProducerArg pointer
 * @return NULL
 */
void *producer_thread(void *arg) {
    ProducerArg *parg = (ProducerArg *)arg;
    
    printf("[Producer %d] Starting, reading file: %s\n",
           parg->producer_id, parg->filename);

    FILE *fp = fopen(parg->filename, "r");
    if (!fp) {
        fprintf(stderr, "[Producer %d] ERROR: Cannot open file: %s\n",
                parg->producer_id, parg->filename);
        return NULL;
    }

    char line[MAX_LINE_LENGTH];
    uint32_t line_num = 0;
    size_t entries_produced = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;

        /* Remove trailing newline */
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        /* Allocate and parse entry */
        LogEntry *entry = malloc(sizeof(LogEntry));
        if (!entry) {
            fprintf(stderr, "[Producer %d] ERROR: Memory allocation failed\n",
                    parg->producer_id);
            break;
        }

        memset(entry, 0, sizeof(LogEntry));

        if (parse_log_line(line, entry, parg->filename, line_num)) {
            if (!buffer_put(parg->buffer, entry)) {
                free(entry);
                break;  /* Shutdown signalled */
            }
            entries_produced++;
        } else {
            free(entry);
        }
    }

    fclose(fp);
    atomic_fetch_add(&parg->stats->files_processed, 1);

    printf("[Producer %d] Finished, produced %zu entries\n",
           parg->producer_id, entries_produced);

    return NULL;
}

/* =============================================================================
 * CONSUMER THREAD
 * =============================================================================
 */

/**
 * Updates statistics based on log level
 *
 * @param stats  Statistics structure
 * @param entry  Log entry to categorise
 */
void update_statistics(Statistics *stats, const LogEntry *entry) {
    LogLevel level = parse_log_level(entry->level);

    atomic_fetch_add(&stats->total_entries, 1);

    switch (level) {
        case LOG_DEBUG:   atomic_fetch_add(&stats->debug_count, 1);   break;
        case LOG_INFO:    atomic_fetch_add(&stats->info_count, 1);    break;
        case LOG_WARN:    atomic_fetch_add(&stats->warn_count, 1);    break;
        case LOG_ERROR:   atomic_fetch_add(&stats->error_count, 1);   break;
        case LOG_HTTP:    atomic_fetch_add(&stats->http_count, 1);    break;
        default:          atomic_fetch_add(&stats->unknown_count, 1); break;
    }
}

/**
 * Consumer thread function: retrieves entries and processes them
 *
 * @param arg ConsumerArg pointer
 * @return NULL
 */
void *consumer_thread(void *arg) {
    ConsumerArg *carg = (ConsumerArg *)arg;
    
    printf("[Consumer %d] Starting\n", carg->consumer_id);

    size_t entries_consumed = 0;

    while (true) {
        LogEntry *entry = buffer_get(carg->buffer);
        
        if (!entry) {
            break;  /* Shutdown and buffer empty */
        }

        /* Process entry */
        update_statistics(carg->stats, entry);
        entries_consumed++;

        /* Free entry */
        free(entry);
    }

    printf("[Consumer %d] Finished, consumed %zu entries\n",
           carg->consumer_id, entries_consumed);

    return NULL;
}

/* =============================================================================
 * STATISTICS REPORTING
 * =============================================================================
 */

/**
 * Initialises statistics structure
 *
 * @return Initialised statistics structure
 */
Statistics *stats_create(void) {
    Statistics *stats = malloc(sizeof(Statistics));
    if (!stats) return NULL;

    atomic_init(&stats->total_entries, 0);
    atomic_init(&stats->debug_count, 0);
    atomic_init(&stats->info_count, 0);
    atomic_init(&stats->warn_count, 0);
    atomic_init(&stats->error_count, 0);
    atomic_init(&stats->http_count, 0);
    atomic_init(&stats->unknown_count, 0);
    atomic_init(&stats->files_processed, 0);

    return stats;
}

/**
 * Prints final statistics report
 *
 * @param stats       Statistics to report
 * @param num_files   Number of input files
 */
void stats_print(const Statistics *stats, int num_files) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                LOG PROCESSOR - STATISTICS                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Processing Summary:\n");
    printf("  Total entries processed: %zu\n",
           atomic_load(&stats->total_entries));
    printf("  Files processed: %zu\n",
           atomic_load(&stats->files_processed));
    printf("\n");
    printf("Log Level Distribution:\n");
    printf("  DEBUG:  %zu\n", atomic_load(&stats->debug_count));
    printf("  INFO:   %zu\n", atomic_load(&stats->info_count));
    printf("  WARN:   %zu\n", atomic_load(&stats->warn_count));
    printf("  ERROR:  %zu\n", atomic_load(&stats->error_count));
    printf("  HTTP:   %zu\n", atomic_load(&stats->http_count));
    if (atomic_load(&stats->unknown_count) > 0) {
        printf("  UNKNOWN: %zu\n", atomic_load(&stats->unknown_count));
    }
    printf("\n");
    printf("Status: COMPLETE\n");
    printf("\n");
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 20: PARALLEL LOG PROCESSOR                           ║\n");
    printf("║            Producer-Consumer Solution                         ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");

    /* Default files if none provided */
    const char *default_files[] = {
        "data/access.log",
        "data/error.log"
    };
    int num_default = 2;

    const char **files;
    int num_files;

    if (argc > 1) {
        files = (const char **)&argv[1];
        num_files = argc - 1;
    } else {
        files = default_files;
        num_files = num_default;
    }

    /* Configuration */
    int num_producers = (num_files < MAX_PRODUCERS) ? num_files : MAX_PRODUCERS;
    int num_consumers = 4;

    printf("Configuration:\n");
    printf("  Input files: %d\n", num_files);
    printf("  Producers: %d\n", num_producers);
    printf("  Consumers: %d\n", num_consumers);
    printf("  Buffer size: %d\n", DEFAULT_BUFFER_SIZE);
    printf("\n");

    /* Create shared resources */
    BoundedBuffer *buffer = buffer_create(DEFAULT_BUFFER_SIZE);
    Statistics *stats = stats_create();

    if (!buffer || !stats) {
        fprintf(stderr, "ERROR: Failed to initialise shared resources\n");
        return EXIT_FAILURE;
    }

    /* Create thread arrays */
    pthread_t producers[MAX_PRODUCERS];
    pthread_t consumers[MAX_CONSUMERS];
    ProducerArg producer_args[MAX_PRODUCERS];
    ConsumerArg consumer_args[MAX_CONSUMERS];

    /* Start consumer threads */
    for (int i = 0; i < num_consumers; i++) {
        consumer_args[i].consumer_id = i;
        consumer_args[i].buffer = buffer;
        consumer_args[i].stats = stats;
        pthread_create(&consumers[i], NULL, consumer_thread, &consumer_args[i]);
    }

    /* Start producer threads */
    for (int i = 0; i < num_producers; i++) {
        producer_args[i].producer_id = i;
        producer_args[i].filename = files[i % num_files];
        producer_args[i].buffer = buffer;
        producer_args[i].stats = stats;
        pthread_create(&producers[i], NULL, producer_thread, &producer_args[i]);
    }

    /* Wait for all producers to finish */
    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }

    /* Signal shutdown to consumers */
    buffer_shutdown(buffer);

    /* Wait for all consumers to finish */
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumers[i], NULL);
    }

    /* Print statistics */
    stats_print(stats, num_files);

    /* Cleanup */
    buffer_destroy(buffer);
    free(stats);

    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    PROCESSING COMPLETE                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    return EXIT_SUCCESS;
}

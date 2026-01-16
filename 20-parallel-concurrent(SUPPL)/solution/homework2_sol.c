/**
 * =============================================================================
 * HOMEWORK 2 SOLUTION: Map-Reduce Word Count
 * Week 20: Parallel and Concurrent Programming
 * =============================================================================
 *
 * Complete implementation of a Map-Reduce framework for parallel word counting.
 *
 * Architecture:
 *   ┌─────────────────────────────────────────────────────────────────────┐
 *   │                         MAP-REDUCE PIPELINE                         │
 *   ├─────────────────────────────────────────────────────────────────────┤
 *   │                                                                     │
 *   │   INPUT              MAP                SHUFFLE            REDUCE   │
 *   │   ─────              ───                ───────            ──────   │
 *   │                                                                     │
 *   │   ┌─────┐        ┌──────────┐                                       │
 *   │   │File1│───────►│ Mapper 1 │──┐                                    │
 *   │   └─────┘        └──────────┘  │                                    │
 *   │                                │     ┌─────────┐    ┌──────────┐    │
 *   │   ┌─────┐        ┌──────────┐  ├────►│Partition│───►│Reducer 1 │    │
 *   │   │File2│───────►│ Mapper 2 │──┤     │   A-M   │    └──────────┘    │
 *   │   └─────┘        └──────────┘  │     └─────────┘                    │
 *   │                                │                                    │
 *   │   ┌─────┐        ┌──────────┐  │     ┌─────────┐    ┌──────────┐    │
 *   │   │File3│───────►│ Mapper 3 │──┴────►│Partition│───►│Reducer 2 │    │
 *   │   └─────┘        └──────────┘        │   N-Z   │    └──────────┘    │
 *   │                                      └─────────┘                    │
 *   │                                                                     │
 *   └─────────────────────────────────────────────────────────────────────┘
 *
 * Features:
 *   - Configurable number of mappers and reducers
 *   - Hash-based key partitioning
 *   - Thread-safe intermediate storage
 *   - Top-N word frequency output
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c11 -pthread -o homework2_sol homework2_sol.c
 *
 * Usage:
 *   ./homework2_sol [input_file] [num_mappers] [num_reducers]
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
#include <ctype.h>
#include <time.h>

/* =============================================================================
 * CONFIGURATION
 * =============================================================================
 */

#define MAX_WORD_LENGTH     64
#define INITIAL_BUCKETS     1024
#define LOAD_FACTOR_LIMIT   0.75
#define DEFAULT_MAPPERS     4
#define DEFAULT_REDUCERS    2
#define TOP_N_WORDS         20

/* =============================================================================
 * KEY-VALUE PAIR
 * =============================================================================
 */

typedef struct KeyValue {
    char *key;                  /* Word */
    int value;                  /* Count */
    struct KeyValue *next;      /* For chaining in hash table */
} KeyValue;

/* =============================================================================
 * HASH TABLE (Thread-Safe)
 * =============================================================================
 */

typedef struct HashTable {
    KeyValue **buckets;
    size_t num_buckets;
    size_t num_entries;
    pthread_mutex_t *bucket_locks;  /* Fine-grained locking */
    pthread_mutex_t resize_lock;    /* For resize operations */
} HashTable;

/**
 * DJB2 hash function.
 */
static uint32_t hash_string(const char *str) {
    uint32_t hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
}

/**
 * Create a new hash table.
 */
static HashTable *hashtable_create(size_t initial_buckets) {
    HashTable *ht = malloc(sizeof(HashTable));
    if (!ht) return NULL;
    
    ht->num_buckets = initial_buckets;
    ht->num_entries = 0;
    
    ht->buckets = calloc(initial_buckets, sizeof(KeyValue *));
    if (!ht->buckets) {
        free(ht);
        return NULL;
    }
    
    ht->bucket_locks = malloc(initial_buckets * sizeof(pthread_mutex_t));
    if (!ht->bucket_locks) {
        free(ht->buckets);
        free(ht);
        return NULL;
    }
    
    for (size_t i = 0; i < initial_buckets; i++) {
        pthread_mutex_init(&ht->bucket_locks[i], NULL);
    }
    
    pthread_mutex_init(&ht->resize_lock, NULL);
    
    return ht;
}

/**
 * Destroy hash table.
 */
static void hashtable_destroy(HashTable *ht) {
    if (!ht) return;
    
    for (size_t i = 0; i < ht->num_buckets; i++) {
        KeyValue *kv = ht->buckets[i];
        while (kv) {
            KeyValue *next = kv->next;
            free(kv->key);
            free(kv);
            kv = next;
        }
        pthread_mutex_destroy(&ht->bucket_locks[i]);
    }
    
    pthread_mutex_destroy(&ht->resize_lock);
    free(ht->bucket_locks);
    free(ht->buckets);
    free(ht);
}

/**
 * Insert or increment count for a key.
 */
static void hashtable_increment(HashTable *ht, const char *key, int amount) {
    uint32_t hash = hash_string(key);
    size_t idx = hash % ht->num_buckets;
    
    pthread_mutex_lock(&ht->bucket_locks[idx]);
    
    /* Search for existing key */
    KeyValue *kv = ht->buckets[idx];
    while (kv) {
        if (strcmp(kv->key, key) == 0) {
            kv->value += amount;
            pthread_mutex_unlock(&ht->bucket_locks[idx]);
            return;
        }
        kv = kv->next;
    }
    
    /* Key not found, create new entry */
    kv = malloc(sizeof(KeyValue));
    kv->key = strdup(key);
    kv->value = amount;
    kv->next = ht->buckets[idx];
    ht->buckets[idx] = kv;
    
    pthread_mutex_unlock(&ht->bucket_locks[idx]);
    
    atomic_fetch_add((atomic_size_t *)&ht->num_entries, 1);
}

/**
 * Get value for a key.
 */
static int hashtable_get(HashTable *ht, const char *key) {
    uint32_t hash = hash_string(key);
    size_t idx = hash % ht->num_buckets;
    
    pthread_mutex_lock(&ht->bucket_locks[idx]);
    
    KeyValue *kv = ht->buckets[idx];
    while (kv) {
        if (strcmp(kv->key, key) == 0) {
            int value = kv->value;
            pthread_mutex_unlock(&ht->bucket_locks[idx]);
            return value;
        }
        kv = kv->next;
    }
    
    pthread_mutex_unlock(&ht->bucket_locks[idx]);
    return 0;
}

/**
 * Iterate over all entries (not thread-safe, use after all operations complete).
 */
typedef void (*HashTableIterator)(const char *key, int value, void *user_data);

static void hashtable_iterate(HashTable *ht, HashTableIterator iter, void *user_data) {
    for (size_t i = 0; i < ht->num_buckets; i++) {
        KeyValue *kv = ht->buckets[i];
        while (kv) {
            iter(kv->key, kv->value, user_data);
            kv = kv->next;
        }
    }
}

/* =============================================================================
 * FILE CHUNK STRUCTURE
 * =============================================================================
 */

typedef struct FileChunk {
    char *data;         /* Chunk data */
    size_t size;        /* Chunk size */
    size_t start;       /* Start offset in original file */
} FileChunk;

/* =============================================================================
 * MAP-REDUCE CONTEXT
 * =============================================================================
 */

typedef struct MapReduceContext {
    /* Configuration */
    int num_mappers;
    int num_reducers;
    
    /* Input */
    char *input_file;
    FileChunk *chunks;          /* File chunks for mappers */
    
    /* Intermediate data (one hash table per reducer partition) */
    HashTable **intermediate;    /* Array of hash tables */
    
    /* Output */
    HashTable *output;          /* Final aggregated results */
    
    /* Synchronisation */
    pthread_barrier_t map_barrier;
    pthread_barrier_t reduce_barrier;
    
    /* Statistics */
    atomic_size_t total_words;
    atomic_size_t unique_words;
    double map_time;
    double shuffle_time;
    double reduce_time;
} MapReduceContext;

/* =============================================================================
 * MAP FUNCTION
 * =============================================================================
 */

typedef struct MapperArg {
    MapReduceContext *ctx;
    int mapper_id;
} MapperArg;

/**
 * Determine which reducer partition a word belongs to.
 */
static int get_partition(const char *word, int num_reducers) {
    return hash_string(word) % num_reducers;
}

/**
 * Mapper thread function.
 *
 * Each mapper:
 *   1. Reads its assigned file chunk
 *   2. Extracts words
 *   3. Emits (word, 1) pairs to appropriate reducer partitions
 */
static void *mapper_thread(void *arg) {
    MapperArg *marg = (MapperArg *)arg;
    MapReduceContext *ctx = marg->ctx;
    int mapper_id = marg->mapper_id;
    
    FileChunk *chunk = &ctx->chunks[mapper_id];
    
    char word[MAX_WORD_LENGTH];
    int word_len = 0;
    size_t local_words = 0;
    
    /* Process chunk character by character */
    for (size_t i = 0; i < chunk->size; i++) {
        char c = chunk->data[i];
        
        if (isalpha(c)) {
            if (word_len < MAX_WORD_LENGTH - 1) {
                word[word_len++] = tolower(c);
            }
        } else {
            if (word_len >= 3) { /* Minimum word length */
                word[word_len] = '\0';
                
                /* Emit to appropriate reducer partition */
                int partition = get_partition(word, ctx->num_reducers);
                hashtable_increment(ctx->intermediate[partition], word, 1);
                
                local_words++;
            }
            word_len = 0;
        }
    }
    
    /* Handle last word in chunk */
    if (word_len >= 3) {
        word[word_len] = '\0';
        int partition = get_partition(word, ctx->num_reducers);
        hashtable_increment(ctx->intermediate[partition], word, 1);
        local_words++;
    }
    
    atomic_fetch_add(&ctx->total_words, local_words);
    
    return NULL;
}

/* =============================================================================
 * REDUCE FUNCTION
 * =============================================================================
 */

typedef struct ReducerArg {
    MapReduceContext *ctx;
    int reducer_id;
} ReducerArg;

/**
 * Collect entries from intermediate hash table into output.
 */
static void collect_entry(const char *key, int value, void *user_data) {
    HashTable *output = (HashTable *)user_data;
    hashtable_increment(output, key, value);
}

/**
 * Reducer thread function.
 *
 * Each reducer:
 *   1. Waits for all mappers to complete
 *   2. Processes its assigned partition
 *   3. Aggregates counts for words in its partition
 */
static void *reducer_thread(void *arg) {
    ReducerArg *rarg = (ReducerArg *)arg;
    MapReduceContext *ctx = rarg->ctx;
    int reducer_id = rarg->reducer_id;
    
    /* Wait for map phase to complete */
    pthread_barrier_wait(&ctx->map_barrier);
    
    /* Process assigned partition */
    HashTable *partition = ctx->intermediate[reducer_id];
    
    /* Copy entries to final output */
    hashtable_iterate(partition, collect_entry, ctx->output);
    
    return NULL;
}

/* =============================================================================
 * FILE CHUNKING
 * =============================================================================
 */

/**
 * Split file into chunks for parallel processing.
 *
 * Chunks are aligned to word boundaries to avoid splitting words.
 */
static int split_file(MapReduceContext *ctx) {
    FILE *fp = fopen(ctx->input_file, "rb");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", ctx->input_file);
        return -1;
    }
    
    /* Get file size */
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    if (file_size == 0) {
        fclose(fp);
        fprintf(stderr, "Error: Empty file\n");
        return -1;
    }
    
    /* Read entire file */
    char *file_data = malloc(file_size + 1);
    if (!file_data) {
        fclose(fp);
        return -1;
    }
    
    size_t bytes_read = fread(file_data, 1, file_size, fp);
    fclose(fp);
    file_data[bytes_read] = '\0';
    
    /* Calculate chunk sizes */
    size_t chunk_size = bytes_read / ctx->num_mappers;
    
    ctx->chunks = malloc(ctx->num_mappers * sizeof(FileChunk));
    if (!ctx->chunks) {
        free(file_data);
        return -1;
    }
    
    size_t offset = 0;
    for (int i = 0; i < ctx->num_mappers; i++) {
        ctx->chunks[i].start = offset;
        
        if (i == ctx->num_mappers - 1) {
            /* Last chunk gets remainder */
            ctx->chunks[i].size = bytes_read - offset;
        } else {
            /* Find word boundary */
            size_t end = offset + chunk_size;
            if (end > bytes_read) end = bytes_read;
            
            /* Extend to word boundary */
            while (end < bytes_read && isalpha(file_data[end])) {
                end++;
            }
            
            ctx->chunks[i].size = end - offset;
        }
        
        /* Allocate and copy chunk data */
        ctx->chunks[i].data = malloc(ctx->chunks[i].size + 1);
        memcpy(ctx->chunks[i].data, file_data + offset, ctx->chunks[i].size);
        ctx->chunks[i].data[ctx->chunks[i].size] = '\0';
        
        offset += ctx->chunks[i].size;
    }
    
    free(file_data);
    return 0;
}

/* =============================================================================
 * MAP-REDUCE EXECUTION
 * =============================================================================
 */

/**
 * Execute the Map-Reduce pipeline.
 */
static int mapreduce_execute(MapReduceContext *ctx) {
    struct timespec start, end;
    
    /* Initialise intermediate storage */
    ctx->intermediate = malloc(ctx->num_reducers * sizeof(HashTable *));
    for (int i = 0; i < ctx->num_reducers; i++) {
        ctx->intermediate[i] = hashtable_create(INITIAL_BUCKETS);
    }
    
    ctx->output = hashtable_create(INITIAL_BUCKETS * 4);
    
    /* Initialise barriers */
    pthread_barrier_init(&ctx->map_barrier, NULL, 
                         ctx->num_mappers + ctx->num_reducers);
    pthread_barrier_init(&ctx->reduce_barrier, NULL, ctx->num_reducers + 1);
    
    /* Create threads */
    pthread_t *mapper_threads = malloc(ctx->num_mappers * sizeof(pthread_t));
    pthread_t *reducer_threads = malloc(ctx->num_reducers * sizeof(pthread_t));
    MapperArg *mapper_args = malloc(ctx->num_mappers * sizeof(MapperArg));
    ReducerArg *reducer_args = malloc(ctx->num_reducers * sizeof(ReducerArg));
    
    /* ═══════════════════════════════════════════════════════════════════════
     * MAP PHASE
     * ═══════════════════════════════════════════════════════════════════════
     */
    
    printf("Starting Map phase with %d mappers...\n", ctx->num_mappers);
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    /* Start mapper threads */
    for (int i = 0; i < ctx->num_mappers; i++) {
        mapper_args[i].ctx = ctx;
        mapper_args[i].mapper_id = i;
        pthread_create(&mapper_threads[i], NULL, mapper_thread, &mapper_args[i]);
    }
    
    /* Start reducer threads (they will wait at barrier) */
    for (int i = 0; i < ctx->num_reducers; i++) {
        reducer_args[i].ctx = ctx;
        reducer_args[i].reducer_id = i;
        pthread_create(&reducer_threads[i], NULL, reducer_thread, &reducer_args[i]);
    }
    
    /* Wait for mappers to complete */
    for (int i = 0; i < ctx->num_mappers; i++) {
        pthread_join(mapper_threads[i], NULL);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    ctx->map_time = (end.tv_sec - start.tv_sec) + 
                    (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("  Map phase completed in %.3f seconds\n", ctx->map_time);
    printf("  Total words processed: %zu\n", atomic_load(&ctx->total_words));
    
    /* ═══════════════════════════════════════════════════════════════════════
     * SHUFFLE PHASE (implicit - data already partitioned)
     * ═══════════════════════════════════════════════════════════════════════
     */
    
    printf("\nShuffle phase (data already partitioned by hash)...\n");
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    /* Display partition statistics */
    for (int i = 0; i < ctx->num_reducers; i++) {
        printf("  Partition %d: %zu unique words\n", 
               i, ctx->intermediate[i]->num_entries);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    ctx->shuffle_time = (end.tv_sec - start.tv_sec) + 
                        (end.tv_nsec - start.tv_nsec) / 1e9;
    
    /* ═══════════════════════════════════════════════════════════════════════
     * REDUCE PHASE
     * ═══════════════════════════════════════════════════════════════════════
     */
    
    printf("\nStarting Reduce phase with %d reducers...\n", ctx->num_reducers);
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    /* Signal reducers to proceed (they're waiting at map_barrier from their creation) */
    pthread_barrier_wait(&ctx->map_barrier);
    
    /* Wait for reducers to complete */
    for (int i = 0; i < ctx->num_reducers; i++) {
        pthread_join(reducer_threads[i], NULL);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    ctx->reduce_time = (end.tv_sec - start.tv_sec) + 
                       (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("  Reduce phase completed in %.3f seconds\n", ctx->reduce_time);
    
    /* Cleanup */
    free(mapper_threads);
    free(reducer_threads);
    free(mapper_args);
    free(reducer_args);
    
    pthread_barrier_destroy(&ctx->map_barrier);
    pthread_barrier_destroy(&ctx->reduce_barrier);
    
    return 0;
}

/* =============================================================================
 * RESULT PROCESSING
 * =============================================================================
 */

typedef struct WordCount {
    char *word;
    int count;
} WordCount;

static int compare_word_counts(const void *a, const void *b) {
    const WordCount *wa = (const WordCount *)a;
    const WordCount *wb = (const WordCount *)b;
    return wb->count - wa->count; /* Descending */
}

typedef struct CollectState {
    WordCount *words;
    size_t capacity;
    size_t count;
} CollectState;

static void collect_word(const char *key, int value, void *user_data) {
    CollectState *state = (CollectState *)user_data;
    
    if (state->count >= state->capacity) {
        state->capacity *= 2;
        state->words = realloc(state->words, state->capacity * sizeof(WordCount));
    }
    
    state->words[state->count].word = strdup(key);
    state->words[state->count].count = value;
    state->count++;
}

/**
 * Display top N most frequent words.
 */
static void display_top_words(MapReduceContext *ctx, int n) {
    /* Collect all words */
    CollectState state = {
        .words = malloc(1024 * sizeof(WordCount)),
        .capacity = 1024,
        .count = 0
    };
    
    hashtable_iterate(ctx->output, collect_word, &state);
    
    /* Sort by count */
    qsort(state.words, state.count, sizeof(WordCount), compare_word_counts);
    
    /* Display top N */
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    TOP %d MOST FREQUENT WORDS                  ║\n", n);
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║  Rank │ Word                              │ Count             ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    
    int display_count = (int)state.count < n ? (int)state.count : n;
    for (int i = 0; i < display_count; i++) {
        printf("║  %4d │ %-33s │ %17d ║\n",
               i + 1, state.words[i].word, state.words[i].count);
    }
    
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Cleanup */
    for (size_t i = 0; i < state.count; i++) {
        free(state.words[i].word);
    }
    free(state.words);
    
    ctx->unique_words = state.count;
}

/* =============================================================================
 * CONTEXT MANAGEMENT
 * =============================================================================
 */

static MapReduceContext *create_context(const char *input_file, 
                                         int num_mappers, int num_reducers) {
    MapReduceContext *ctx = calloc(1, sizeof(MapReduceContext));
    if (!ctx) return NULL;
    
    ctx->input_file = strdup(input_file);
    ctx->num_mappers = num_mappers;
    ctx->num_reducers = num_reducers;
    
    atomic_init(&ctx->total_words, 0);
    atomic_init(&ctx->unique_words, 0);
    
    return ctx;
}

static void destroy_context(MapReduceContext *ctx) {
    if (!ctx) return;
    
    free(ctx->input_file);
    
    if (ctx->chunks) {
        for (int i = 0; i < ctx->num_mappers; i++) {
            free(ctx->chunks[i].data);
        }
        free(ctx->chunks);
    }
    
    if (ctx->intermediate) {
        for (int i = 0; i < ctx->num_reducers; i++) {
            hashtable_destroy(ctx->intermediate[i]);
        }
        free(ctx->intermediate);
    }
    
    hashtable_destroy(ctx->output);
    
    free(ctx);
}

/* =============================================================================
 * SAMPLE TEXT GENERATION
 * =============================================================================
 */

static void generate_sample_text(const char *filename) {
    const char *sample_text =
        "The quick brown fox jumps over the lazy dog. "
        "A journey of a thousand miles begins with a single step. "
        "To be or not to be, that is the question. "
        "All that glitters is not gold. "
        "Actions speak louder than words. "
        "The early bird catches the worm. "
        "Where there is smoke, there is fire. "
        "Rome was not built in a day. "
        "When in Rome, do as the Romans do. "
        "A picture is worth a thousand words. "
        "The pen is mightier than the sword. "
        "Practice makes perfect. "
        "Time flies when you are having fun. "
        "Every cloud has a silver lining. "
        "Birds of a feather flock together. "
        "The grass is always greener on the other side. "
        "You cannot judge a book by its cover. "
        "Two wrongs do not make a right. "
        "Better late than never. "
        "Fortune favours the bold. "
        "Knowledge is power. "
        "The best things in life are free. "
        "Honesty is the best policy. "
        "If at first you do not succeed, try again. "
        "Patience is a virtue. "
        "All is fair in love and war. "
        "Beauty is in the eye of the beholder. "
        "Necessity is the mother of invention. "
        "The truth will set you free. "
        "Actions speak louder than words. ";
    
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Warning: Cannot create sample file\n");
        return;
    }
    
    /* Write sample text multiple times for larger file */
    for (int i = 0; i < 1000; i++) {
        fputs(sample_text, fp);
        fputc('\n', fp);
    }
    
    fclose(fp);
    printf("Generated sample text file: %s\n", filename);
}

/* =============================================================================
 * DEMONSTRATION
 * =============================================================================
 */

static void demo_small_file(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      DEMO 1: Small File Processing                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    const char *filename = "/tmp/sample_small.txt";
    
    /* Create small sample file */
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "hello world hello again world hello\n");
    fprintf(fp, "the quick brown fox jumps over the lazy dog\n");
    fprintf(fp, "hello from the other side\n");
    fclose(fp);
    
    MapReduceContext *ctx = create_context(filename, 2, 2);
    
    if (split_file(ctx) == 0) {
        mapreduce_execute(ctx);
        display_top_words(ctx, 10);
        
        printf("\nStatistics:\n");
        printf("  Total words:  %zu\n", atomic_load(&ctx->total_words));
        printf("  Unique words: %zu\n", ctx->unique_words);
        printf("  Map time:     %.3f s\n", ctx->map_time);
        printf("  Reduce time:  %.3f s\n", ctx->reduce_time);
    }
    
    destroy_context(ctx);
}

static void demo_large_file(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      DEMO 2: Large File Processing                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    const char *filename = "/tmp/sample_large.txt";
    
    generate_sample_text(filename);
    
    MapReduceContext *ctx = create_context(filename, 4, 2);
    
    if (split_file(ctx) == 0) {
        printf("\nFile chunks:\n");
        for (int i = 0; i < ctx->num_mappers; i++) {
            printf("  Chunk %d: %zu bytes (offset %zu)\n",
                   i, ctx->chunks[i].size, ctx->chunks[i].start);
        }
        printf("\n");
        
        mapreduce_execute(ctx);
        display_top_words(ctx, TOP_N_WORDS);
        
        printf("\nStatistics:\n");
        printf("  Total words:  %zu\n", atomic_load(&ctx->total_words));
        printf("  Unique words: %zu\n", ctx->unique_words);
        printf("  Map time:     %.3f s\n", ctx->map_time);
        printf("  Reduce time:  %.3f s\n", ctx->reduce_time);
        printf("  Total time:   %.3f s\n", 
               ctx->map_time + ctx->shuffle_time + ctx->reduce_time);
    }
    
    destroy_context(ctx);
}

static void demo_scalability(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      DEMO 3: Scalability Analysis                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    const char *filename = "/tmp/sample_large.txt";
    
    printf("Testing with varying mapper/reducer counts...\n\n");
    printf("┌──────────┬──────────┬──────────┬──────────┬──────────┐\n");
    printf("│ Mappers  │ Reducers │ Map Time │ Red Time │ Total    │\n");
    printf("├──────────┼──────────┼──────────┼──────────┼──────────┤\n");
    
    int configs[][2] = {
        {1, 1}, {2, 1}, {4, 1}, {4, 2}, {8, 2}, {8, 4}
    };
    int num_configs = sizeof(configs) / sizeof(configs[0]);
    
    for (int i = 0; i < num_configs; i++) {
        int mappers = configs[i][0];
        int reducers = configs[i][1];
        
        MapReduceContext *ctx = create_context(filename, mappers, reducers);
        
        if (split_file(ctx) == 0) {
            mapreduce_execute(ctx);
            
            double total = ctx->map_time + ctx->shuffle_time + ctx->reduce_time;
            printf("│    %2d    │    %2d    │  %6.3fs │  %6.3fs │  %6.3fs │\n",
                   mappers, reducers, ctx->map_time, ctx->reduce_time, total);
        }
        
        destroy_context(ctx);
    }
    
    printf("└──────────┴──────────┴──────────┴──────────┴──────────┘\n");
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

static void print_usage(const char *progname) {
    printf("Usage: %s [input_file] [num_mappers] [num_reducers]\n\n", progname);
    printf("Arguments:\n");
    printf("  input_file    Path to input text file (default: run demos)\n");
    printf("  num_mappers   Number of mapper threads (default: %d)\n", DEFAULT_MAPPERS);
    printf("  num_reducers  Number of reducer threads (default: %d)\n", DEFAULT_REDUCERS);
    printf("\nExample:\n");
    printf("  %s document.txt 8 4\n", progname);
}

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     HOMEWORK 2 SOLUTION: Map-Reduce Word Count                ║\n");
    printf("║                                                               ║\n");
    printf("║     Week 20: Parallel and Concurrent Programming              ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    if (argc == 1) {
        /* Run demonstrations */
        demo_small_file();
        demo_large_file();
        demo_scalability();
    } else if (argc >= 2) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        
        const char *input_file = argv[1];
        int num_mappers = argc > 2 ? atoi(argv[2]) : DEFAULT_MAPPERS;
        int num_reducers = argc > 3 ? atoi(argv[3]) : DEFAULT_REDUCERS;
        
        if (num_mappers < 1) num_mappers = 1;
        if (num_reducers < 1) num_reducers = 1;
        
        printf("\nProcessing: %s\n", input_file);
        printf("Configuration: %d mappers, %d reducers\n\n", 
               num_mappers, num_reducers);
        
        MapReduceContext *ctx = create_context(input_file, num_mappers, num_reducers);
        
        if (split_file(ctx) == 0) {
            mapreduce_execute(ctx);
            display_top_words(ctx, TOP_N_WORDS);
            
            printf("\nStatistics:\n");
            printf("  Total words:  %zu\n", atomic_load(&ctx->total_words));
            printf("  Unique words: %zu\n", ctx->unique_words);
            printf("  Map time:     %.3f s\n", ctx->map_time);
            printf("  Reduce time:  %.3f s\n", ctx->reduce_time);
            printf("  Total time:   %.3f s\n",
                   ctx->map_time + ctx->shuffle_time + ctx->reduce_time);
        }
        
        destroy_context(ctx);
    }
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    PROCESSING COMPLETE                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

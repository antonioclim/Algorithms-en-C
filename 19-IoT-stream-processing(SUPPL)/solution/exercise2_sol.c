/**
 * =============================================================================
 * WEEK 19: ALGORITHMS FOR IoT AND STREAM PROCESSING
 * Exercise 2: MQTT-Style Publish-Subscribe Broker - SOLUTION
 * =============================================================================
 *
 * This is the complete reference solution with all TODOs implemented.
 *
 * Key learning points demonstrated:
 *   1. Publish-subscribe pattern implementation
 *   2. Topic-based message routing
 *   3. Callback-based event handling
 *   4. Sensor simulation with drift and noise
 *   5. Statistics tracking per topic
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c -lm
 * Usage: ./exercise2_sol 20
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <unistd.h>

/* =============================================================================
 * CONFIGURATION
 * =============================================================================
 */

#define MAX_TOPICS        32
#define MAX_SUBSCRIBERS   64
#define MAX_TOPIC_LEN     64
#define MAX_LINE_LENGTH   256
#define DEFAULT_ITERATIONS 20

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

/* Forward declaration */
struct Broker;

/**
 * Callback function type for message delivery.
 *
 * @param topic   The topic the message was published to
 * @param value   The numeric value of the message
 * @param context User-provided context pointer
 */
typedef void (*MessageCallback)(const char *topic, double value, void *context);

/**
 * Subscriber entry.
 */
typedef struct {
    char topic_filter[MAX_TOPIC_LEN];  /* Topic to subscribe to */
    MessageCallback callback;           /* Function to call on message */
    void *context;                      /* User context for callback */
    bool active;                        /* Is this subscription active? */
    char name[32];                      /* Subscriber name for logging */
} Subscriber;

/**
 * Topic statistics tracker.
 */
typedef struct {
    char topic[MAX_TOPIC_LEN];  /* Topic name */
    size_t message_count;       /* Messages received */
    double sum;                 /* Sum for average calculation */
    double min;                 /* Minimum value seen */
    double max;                 /* Maximum value seen */
    bool active;                /* Is this topic active? */
} TopicStats;

/**
 * MQTT-style message broker.
 */
typedef struct Broker {
    Subscriber subscribers[MAX_SUBSCRIBERS];
    int subscriber_count;
    TopicStats topics[MAX_TOPICS];
    int topic_count;
    size_t total_messages;
    size_t total_deliveries;
} Broker;

/**
 * Simulated sensor.
 */
typedef struct {
    char topic[MAX_TOPIC_LEN];  /* Topic to publish to */
    double base_value;          /* Base reading value */
    double noise_level;         /* Random noise amplitude */
    double drift_rate;          /* Gradual drift per reading */
    double current_drift;       /* Accumulated drift */
    size_t readings;            /* Total readings generated */
} Sensor;

/* =============================================================================
 * SOLUTION 1: BROKER INITIALISATION
 * =============================================================================
 *
 * Implementation notes:
 *   - Zero all counters
 *   - Mark all slots as inactive so we know they're available
 *
 * Time Complexity: O(MAX_SUBSCRIBERS + MAX_TOPICS)
 */
void broker_init(Broker *broker) {
    if (!broker) return;
    
    /* SOLUTION: Initialise counters */
    broker->subscriber_count = 0;
    broker->topic_count = 0;
    broker->total_messages = 0;
    broker->total_deliveries = 0;
    
    /* SOLUTION: Mark all subscriber slots as inactive */
    for (int i = 0; i < MAX_SUBSCRIBERS; i++) {
        broker->subscribers[i].active = false;
        broker->subscribers[i].topic_filter[0] = '\0';
        broker->subscribers[i].callback = NULL;
        broker->subscribers[i].context = NULL;
        broker->subscribers[i].name[0] = '\0';
    }
    
    /* SOLUTION: Mark all topic slots as inactive */
    for (int i = 0; i < MAX_TOPICS; i++) {
        broker->topics[i].active = false;
        broker->topics[i].topic[0] = '\0';
        broker->topics[i].message_count = 0;
        broker->topics[i].sum = 0.0;
        broker->topics[i].min = DBL_MAX;
        broker->topics[i].max = -DBL_MAX;
    }
}

/* =============================================================================
 * SOLUTION 2: TOPIC MATCHING
 * =============================================================================
 *
 * Implementation notes:
 *   - Simple exact string matching for this exercise
 *   - Real MQTT supports wildcards:
 *     + matches single level (home/+/temp matches home/living/temp)
 *     # matches multi-level (home/# matches home/living/temp)
 *
 * Time Complexity: O(n) where n is topic length
 */
bool topic_matches(const char *topic, const char *filter) {
    if (!topic || !filter) return false;
    
    /* SOLUTION: Exact string matching using strcmp */
    return strcmp(topic, filter) == 0;
}

/* =============================================================================
 * SOLUTION 3: GET OR CREATE TOPIC STATISTICS
 * =============================================================================
 *
 * Implementation notes:
 *   - First search for existing topic (O(n) linear search)
 *   - If not found, find first inactive slot
 *   - Initialise with proper defaults (min=DBL_MAX, max=-DBL_MAX)
 *
 * Time Complexity: O(MAX_TOPICS)
 */
TopicStats *broker_get_or_create_topic(Broker *broker, const char *topic) {
    if (!broker || !topic) return NULL;
    
    /* SOLUTION: Search for existing topic */
    for (int i = 0; i < MAX_TOPICS; i++) {
        if (broker->topics[i].active && 
            strcmp(broker->topics[i].topic, topic) == 0) {
            /* Found existing topic */
            return &broker->topics[i];
        }
    }
    
    /* SOLUTION: Topic not found, create new entry */
    for (int i = 0; i < MAX_TOPICS; i++) {
        if (!broker->topics[i].active) {
            /* Found empty slot - initialise it */
            TopicStats *ts = &broker->topics[i];
            
            strncpy(ts->topic, topic, MAX_TOPIC_LEN - 1);
            ts->topic[MAX_TOPIC_LEN - 1] = '\0';
            ts->message_count = 0;
            ts->sum = 0.0;
            ts->min = DBL_MAX;    /* Will be updated with first message */
            ts->max = -DBL_MAX;   /* Will be updated with first message */
            ts->active = true;
            
            broker->topic_count++;
            
            return ts;
        }
    }
    
    /* No slots available */
    fprintf(stderr, "Warning: Maximum topic limit reached\n");
    return NULL;
}

/* =============================================================================
 * SOLUTION 4: SUBSCRIBE TO TOPIC
 * =============================================================================
 *
 * Implementation notes:
 *   - Find first inactive subscriber slot
 *   - Copy all subscription data safely
 *   - Return slot index as subscription ID
 *
 * Time Complexity: O(MAX_SUBSCRIBERS)
 */
int broker_subscribe(Broker *broker, const char *topic_filter,
                     MessageCallback callback, void *context, 
                     const char *name) {
    if (!broker || !topic_filter || !callback) return -1;
    
    /* SOLUTION: Find empty subscriber slot */
    for (int i = 0; i < MAX_SUBSCRIBERS; i++) {
        if (!broker->subscribers[i].active) {
            /* Found empty slot */
            Subscriber *sub = &broker->subscribers[i];
            
            /* SOLUTION: Initialise the subscriber entry */
            strncpy(sub->topic_filter, topic_filter, MAX_TOPIC_LEN - 1);
            sub->topic_filter[MAX_TOPIC_LEN - 1] = '\0';
            
            sub->callback = callback;
            sub->context = context;
            
            if (name) {
                strncpy(sub->name, name, sizeof(sub->name) - 1);
                sub->name[sizeof(sub->name) - 1] = '\0';
            } else {
                snprintf(sub->name, sizeof(sub->name), "Sub%d", i);
            }
            
            sub->active = true;
            broker->subscriber_count++;
            
            return i;  /* Return slot index as subscription ID */
        }
    }
    
    /* No slots available */
    fprintf(stderr, "Warning: Maximum subscriber limit reached\n");
    return -1;
}

/* =============================================================================
 * SOLUTION 5: PUBLISH MESSAGE
 * =============================================================================
 *
 * Implementation notes:
 *   - Update topic stats first (create if needed)
 *   - Then find and notify all matching subscribers
 *   - Track both messages and deliveries
 *
 * Time Complexity: O(MAX_SUBSCRIBERS)
 */
int broker_publish(Broker *broker, const char *topic, double value) {
    if (!broker || !topic) return 0;
    
    /* SOLUTION: Increment total message count */
    broker->total_messages++;
    
    /* SOLUTION: Update topic statistics */
    TopicStats *ts = broker_get_or_create_topic(broker, topic);
    if (ts) {
        ts->message_count++;
        ts->sum += value;
        
        /* Update min/max */
        if (value < ts->min) ts->min = value;
        if (value > ts->max) ts->max = value;
    }
    
    /* SOLUTION: Deliver to matching subscribers */
    int delivered = 0;
    
    for (int i = 0; i < MAX_SUBSCRIBERS; i++) {
        if (broker->subscribers[i].active &&
            topic_matches(topic, broker->subscribers[i].topic_filter)) {
            
            /* Call subscriber's callback */
            Subscriber *sub = &broker->subscribers[i];
            sub->callback(topic, value, sub->context);
            
            delivered++;
        }
    }
    
    broker->total_deliveries += delivered;
    
    return delivered;
}

/**
 * Unsubscribe from a topic.
 */
void broker_unsubscribe(Broker *broker, int subscription_id) {
    if (!broker || subscription_id < 0 || 
        subscription_id >= MAX_SUBSCRIBERS) return;
    
    if (broker->subscribers[subscription_id].active) {
        broker->subscribers[subscription_id].active = false;
        broker->subscriber_count--;
    }
}

/**
 * Print broker statistics.
 */
void broker_print_stats(Broker *broker) {
    if (!broker) return;
    
    printf("\nBroker Statistics:\n");
    printf("══════════════════\n");
    printf("  Total messages published: %zu\n", broker->total_messages);
    printf("  Total deliveries: %zu\n", broker->total_deliveries);
    printf("  Active subscribers: %d\n", broker->subscriber_count);
    printf("  Active topics: %d\n\n", broker->topic_count);
    
    if (broker->topic_count > 0) {
        printf("  Topic Statistics:\n");
        printf("  ┌──────────────────────────┬─────────┬─────────┬─────────┬─────────┐\n");
        printf("  │          Topic           │  Count  │   Avg   │   Min   │   Max   │\n");
        printf("  ├──────────────────────────┼─────────┼─────────┼─────────┼─────────┤\n");
        
        for (int i = 0; i < MAX_TOPICS; i++) {
            if (broker->topics[i].active) {
                double avg = broker->topics[i].message_count > 0 ?
                    broker->topics[i].sum / broker->topics[i].message_count : 0.0;
                printf("  │ %-24s │  %5zu  │ %7.2f │ %7.2f │ %7.2f │\n",
                       broker->topics[i].topic,
                       broker->topics[i].message_count,
                       avg,
                       broker->topics[i].min == DBL_MAX ? 0.0 : broker->topics[i].min,
                       broker->topics[i].max == -DBL_MAX ? 0.0 : broker->topics[i].max);
            }
        }
        printf("  └──────────────────────────┴─────────┴─────────┴─────────┴─────────┘\n");
    }
}

/* =============================================================================
 * SOLUTION 6: SENSOR READING
 * =============================================================================
 *
 * Implementation notes:
 *   - Combine base value, accumulated drift, and random noise
 *   - Drift simulates gradual sensor degradation or environmental change
 *   - Noise simulates measurement uncertainty
 *
 * Time Complexity: O(1)
 */
double sensor_read(Sensor *sensor) {
    if (!sensor) return 0.0;
    
    /* SOLUTION: Calculate reading */
    /* Generate noise: random value in range [-noise_level, +noise_level] */
    double noise = sensor->noise_level * (2.0 * ((double)rand() / RAND_MAX) - 1.0);
    
    /* Combine base + drift + noise */
    double value = sensor->base_value + sensor->current_drift + noise;
    
    /* SOLUTION: Update drift and counter */
    sensor->current_drift += sensor->drift_rate;
    sensor->readings++;
    
    return value;
}

/**
 * Initialise sensor.
 */
void sensor_init(Sensor *sensor, const char *topic, double base_value,
                 double noise_level, double drift_rate) {
    if (!sensor) return;
    
    strncpy(sensor->topic, topic, MAX_TOPIC_LEN - 1);
    sensor->topic[MAX_TOPIC_LEN - 1] = '\0';
    sensor->base_value = base_value;
    sensor->noise_level = noise_level;
    sensor->drift_rate = drift_rate;
    sensor->current_drift = 0.0;
    sensor->readings = 0;
}

/* =============================================================================
 * CALLBACK IMPLEMENTATIONS
 * =============================================================================
 */

/**
 * Logger callback - prints all received messages.
 */
void logger_callback(const char *topic, double value, void *context) {
    const char *name = (const char *)context;
    printf("    [%s] Received: %s = %.2f\n", 
           name ? name : "LOGGER", topic, value);
}

/**
 * Alert callback - prints warning if value exceeds threshold.
 */
void alert_callback(const char *topic, double value, void *context) {
    double *threshold = (double *)context;
    if (threshold && value > *threshold) {
        printf("    [ALERT] ⚠ %s = %.2f exceeds threshold %.2f!\n",
               topic, value, *threshold);
    }
}

/**
 * Stats callback - tracks running average (simple demonstration).
 */
typedef struct {
    double sum;
    int count;
    char name[32];
} StatsContext;

void stats_callback(const char *topic, double value, void *context) {
    (void)topic;
    StatsContext *ctx = (StatsContext *)context;
    if (ctx) {
        ctx->sum += value;
        ctx->count++;
    }
}

/* =============================================================================
 * SOLUTION 7: RUN SIMULATION
 * =============================================================================
 *
 * Implementation notes:
 *   - Set up broker with multiple subscribers
 *   - Each subscriber can have different behaviour (log, alert, stats)
 *   - Simulate multiple sensors publishing data
 *
 * This demonstrates the flexibility of publish-subscribe pattern.
 */
void run_simulation(int iterations) {
    printf("IoT Simulation - SOLUTION\n");
    printf("═════════════════════════\n\n");
    
    /* Seed random number generator */
    srand((unsigned int)time(NULL));
    
    /* SOLUTION: Initialise broker */
    Broker broker;
    broker_init(&broker);
    
    /* Create sensors */
    Sensor temp_sensor, humidity_sensor;
    sensor_init(&temp_sensor, "home/temperature", 22.0, 0.5, 0.05);
    sensor_init(&humidity_sensor, "home/humidity", 55.0, 2.0, 0.1);
    
    /* SOLUTION: Set up subscriptions */
    static double temp_threshold = 25.0;
    static const char *temp_logger_name = "TEMP";
    static const char *humidity_logger_name = "HUMID";
    
    /* Logger for temperature */
    broker_subscribe(&broker, "home/temperature", logger_callback, 
                     (void *)temp_logger_name, "TempLogger");
    
    /* Logger for humidity */
    broker_subscribe(&broker, "home/humidity", logger_callback,
                     (void *)humidity_logger_name, "HumidityLogger");
    
    /* Alert for temperature exceeding threshold */
    broker_subscribe(&broker, "home/temperature", alert_callback,
                     &temp_threshold, "TempAlert");
    
    /* Stats tracker for temperature */
    StatsContext temp_stats = {0.0, 0, "TempStats"};
    broker_subscribe(&broker, "home/temperature", stats_callback,
                     &temp_stats, "TempStats");
    
    printf("Configuration:\n");
    printf("  Iterations: %d\n", iterations);
    printf("  Sensors: temperature (22°C base), humidity (55%% base)\n");
    printf("  Alert threshold: temperature > %.1f°C\n", temp_threshold);
    printf("  Subscribers: %d active\n\n", broker.subscriber_count);
    
    /* SOLUTION: Run simulation loop */
    printf("Running simulation...\n\n");
    
    for (int i = 0; i < iterations; i++) {
        printf("  Iteration %d:\n", i + 1);
        
        /* SOLUTION: Read and publish temperature */
        double temp = sensor_read(&temp_sensor);
        broker_publish(&broker, temp_sensor.topic, temp);
        
        /* SOLUTION: Read and publish humidity */
        double humid = sensor_read(&humidity_sensor);
        broker_publish(&broker, humidity_sensor.topic, humid);
        
        printf("\n");
    }
    
    /* SOLUTION: Print statistics */
    broker_print_stats(&broker);
    
    /* Print subscriber-tracked stats */
    if (temp_stats.count > 0) {
        printf("\n  Subscriber-Tracked Statistics:\n");
        printf("  ─────────────────────────────\n");
        printf("  Temperature average (from callback): %.2f°C\n",
               temp_stats.sum / temp_stats.count);
    }
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

void print_usage(const char *prog) {
    printf("Usage: %s [iterations]\n", prog);
    printf("  iterations: Number of simulation cycles (default: %d)\n", 
           DEFAULT_ITERATIONS);
}

int main(int argc, char *argv[]) {
    int iterations = DEFAULT_ITERATIONS;
    
    /* Parse arguments */
    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        iterations = atoi(argv[1]);
        if (iterations <= 0) iterations = DEFAULT_ITERATIONS;
    }
    
    /* Check for stdin input */
    if (!isatty(fileno(stdin))) {
        /* Read iteration count from stdin */
        char line[MAX_LINE_LENGTH];
        if (fgets(line, sizeof(line), stdin)) {
            int n = atoi(line);
            if (n > 0) iterations = n;
        }
    }
    
    run_simulation(iterations);
    
    return 0;
}

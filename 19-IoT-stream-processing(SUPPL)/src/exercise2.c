/**
 * =============================================================================
 * WEEK 19: ALGORITHMS FOR IoT AND STREAM PROCESSING
 * Exercise 2: MQTT-Style Publish-Subscribe Broker
 * =============================================================================
 *
 * OBJECTIVE:
 * Implement a simplified MQTT-style message broker with:
 *   - Topic-based message routing
 *   - Subscription management
 *   - Per-topic statistics tracking
 *   - Callback-based message delivery
 *   - Sensor simulation
 *
 * INSTRUCTIONS:
 * Complete the functions marked with TODO. The broker should support
 * multiple topics and subscribers, routing messages appropriately.
 *
 * TESTING:
 *   make run-ex2
 *   ./exercise2 < data/mqtt_messages.txt
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
 * TODO 1: BROKER INITIALISATION
 * =============================================================================
 *
 * Initialise the message broker.
 *
 * Requirements:
 *   - Set all counters to zero
 *   - Mark all subscriber slots as inactive
 *   - Mark all topic slots as inactive
 *
 * Time Complexity: O(MAX_SUBSCRIBERS + MAX_TOPICS)
 */
void broker_init(Broker *broker) {
    if (!broker) return;
    
    /* TODO: Initialise counters */
    /* YOUR CODE HERE */
    
    /* TODO: Mark all subscriber slots as inactive */
    /* Hint: Loop through subscribers array, set active = false */
    /* YOUR CODE HERE */
    
    /* TODO: Mark all topic slots as inactive */
    /* Hint: Loop through topics array, set active = false */
    /* YOUR CODE HERE */
}

/* =============================================================================
 * TODO 2: TOPIC MATCHING
 * =============================================================================
 *
 * Check if a topic matches a subscription filter.
 *
 * For this simplified implementation, we use exact string matching.
 * (Real MQTT supports wildcards: + for single level, # for multi-level)
 *
 * Requirements:
 *   - Return true if topic exactly matches filter
 *   - Return false otherwise
 *
 * Time Complexity: O(n) where n is topic length
 *
 * Examples:
 *   topic_matches("home/temp", "home/temp") → true
 *   topic_matches("home/temp", "home/humidity") → false
 */
bool topic_matches(const char *topic, const char *filter) {
    if (!topic || !filter) return false;
    
    /* TODO: Implement exact string matching */
    /* Hint: Use strcmp() */
    
    /* YOUR CODE HERE */
    
    return false;  /* Replace with your implementation */
}

/* =============================================================================
 * TODO 3: GET OR CREATE TOPIC STATISTICS
 * =============================================================================
 *
 * Find existing topic stats or create new entry.
 *
 * Requirements:
 *   - Search for existing topic by name
 *   - If found, return pointer to it
 *   - If not found, find empty slot and initialise it
 *   - Return NULL if no slots available
 *
 * Time Complexity: O(MAX_TOPICS)
 */
TopicStats *broker_get_or_create_topic(Broker *broker, const char *topic) {
    if (!broker || !topic) return NULL;
    
    /* TODO: Search for existing topic */
    /* Hint: Loop through topics, check active && strcmp */
    /* YOUR CODE HERE */
    
    /* TODO: If not found, create new entry */
    /* Hint: Find first inactive slot, initialise it */
    /*   - Copy topic name
     *   - Set message_count = 0
     *   - Set sum = 0.0
     *   - Set min = DBL_MAX
     *   - Set max = -DBL_MAX
     *   - Set active = true
     *   - Increment topic_count
     */
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace with your implementation */
}

/* =============================================================================
 * TODO 4: SUBSCRIBE TO TOPIC
 * =============================================================================
 *
 * Register a new subscription.
 *
 * Requirements:
 *   - Find an inactive subscriber slot
 *   - Copy topic filter and callback information
 *   - Mark slot as active
 *   - Increment subscriber count
 *   - Return subscription ID (slot index) or -1 on failure
 *
 * Time Complexity: O(MAX_SUBSCRIBERS)
 */
int broker_subscribe(Broker *broker, const char *topic_filter,
                     MessageCallback callback, void *context, 
                     const char *name) {
    if (!broker || !topic_filter || !callback) return -1;
    
    /* TODO: Find empty subscriber slot */
    /* YOUR CODE HERE */
    
    /* TODO: Initialise the subscriber entry */
    /*   - Copy topic_filter (use strncpy for safety)
     *   - Set callback
     *   - Set context
     *   - Copy name
     *   - Set active = true
     *   - Increment subscriber_count
     */
    /* YOUR CODE HERE */
    
    return -1;  /* Replace with slot index on success */
}

/* =============================================================================
 * TODO 5: PUBLISH MESSAGE
 * =============================================================================
 *
 * Publish a message to a topic.
 *
 * Requirements:
 *   - Update topic statistics (create topic if needed)
 *   - Find all matching subscribers
 *   - Call each subscriber's callback
 *   - Track total messages and deliveries
 *   - Return number of subscribers notified
 *
 * Time Complexity: O(MAX_SUBSCRIBERS)
 */
int broker_publish(Broker *broker, const char *topic, double value) {
    if (!broker || !topic) return 0;
    
    /* TODO: Increment total message count */
    /* YOUR CODE HERE */
    
    /* TODO: Update topic statistics */
    /* Hint: Use broker_get_or_create_topic */
    /*   - Increment message_count
     *   - Add to sum
     *   - Update min if value < min
     *   - Update max if value > max
     */
    /* YOUR CODE HERE */
    
    /* TODO: Deliver to matching subscribers */
    /* Hint: Loop through subscribers, check active && topic_matches */
    /*   - Call subscriber's callback(topic, value, context)
     *   - Count deliveries
     */
    int delivered = 0;
    /* YOUR CODE HERE */
    
    return delivered;
}

/**
 * Unsubscribe from a topic.
 * (Provided)
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
 * (Provided)
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
 * TODO 6: SENSOR READING
 * =============================================================================
 *
 * Generate a simulated sensor reading.
 *
 * Requirements:
 *   - Start with base_value
 *   - Add accumulated drift
 *   - Add random noise: noise_level × random(-1, 1)
 *   - Update drift accumulator: current_drift += drift_rate
 *   - Increment reading count
 *
 * Time Complexity: O(1)
 *
 * Example:
 *   base=20, noise=0.5, drift_rate=0.01
 *   Reading 1: 20 + 0 + random noise ≈ 20.1
 *   Reading 2: 20 + 0.01 + random noise ≈ 20.2
 *   Reading 100: 20 + 1.0 + random noise ≈ 21.1
 */
double sensor_read(Sensor *sensor) {
    if (!sensor) return 0.0;
    
    /* TODO: Calculate reading */
    /*   value = base_value + current_drift + noise
     *   noise = noise_level × (random 0-1 × 2 - 1)
     */
    /* YOUR CODE HERE */
    
    /* TODO: Update drift and counter */
    /* YOUR CODE HERE */
    
    return 0.0;  /* Replace with calculated value */
}

/**
 * Initialise sensor.
 * (Provided)
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
 * CALLBACK IMPLEMENTATIONS (PROVIDED)
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
 * TODO 7: RUN SIMULATION
 * =============================================================================
 *
 * Run the IoT simulation with multiple sensors and subscribers.
 *
 * Requirements:
 *   - Create and initialise sensors
 *   - Set up subscriptions
 *   - Run simulation loop:
 *     * Read from each sensor
 *     * Publish readings to broker
 *   - Print final statistics
 */
void run_simulation(int iterations) {
    printf("IoT Simulation\n");
    printf("══════════════\n\n");
    
    /* Seed random number generator */
    srand((unsigned int)time(NULL));
    
    /* TODO: Initialise broker */
    Broker broker;
    /* YOUR CODE HERE */
    
    /* TODO: Create sensors */
    Sensor temp_sensor, humidity_sensor;
    sensor_init(&temp_sensor, "home/temperature", 22.0, 0.5, 0.05);
    sensor_init(&humidity_sensor, "home/humidity", 55.0, 2.0, 0.1);
    
    /* TODO: Set up subscriptions */
    /*   - Logger for "home/temperature"
     *   - Logger for "home/humidity"
     *   - Alert for "home/temperature" with threshold 25.0
     */
    static double temp_threshold = 25.0;
    /* YOUR CODE HERE */
    
    printf("Configuration:\n");
    printf("  Iterations: %d\n", iterations);
    printf("  Sensors: temperature (22°C base), humidity (55%% base)\n");
    printf("  Alert threshold: temperature > %.1f°C\n\n", temp_threshold);
    
    /* TODO: Run simulation loop */
    printf("Running simulation...\n\n");
    
    for (int i = 0; i < iterations; i++) {
        printf("  Iteration %d:\n", i + 1);
        
        /* TODO: Read and publish temperature */
        /* YOUR CODE HERE */
        
        /* TODO: Read and publish humidity */
        /* YOUR CODE HERE */
        
        printf("\n");
    }
    
    /* TODO: Print statistics */
    /* YOUR CODE HERE */
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

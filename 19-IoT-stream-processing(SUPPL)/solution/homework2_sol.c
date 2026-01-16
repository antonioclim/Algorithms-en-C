/**
 * =============================================================================
 * WEEK 19: ALGORITHMS FOR IoT AND STREAM PROCESSING
 * Homework 2: Event-Driven IoT Gateway - SOLUTION
 * =============================================================================
 *
 * This solution demonstrates:
 *   1. Rule engine for conditional action triggering
 *   2. Temporal event correlation with sliding windows
 *   3. Action dispatcher with rate limiting
 *   4. Complete event-driven architecture
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c -lm
 * Usage: ./homework2_sol
 *
 * Industrial Application:
 *   IoT gateways are the intelligence layer in smart buildings, factories,
 *   and infrastructure. This code demonstrates the event processing patterns
 *   used in systems like AWS IoT, Azure IoT Hub, and industrial SCADA.
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

/* =============================================================================
 * CONFIGURATION
 * =============================================================================
 */

#define MAX_RULES               32
#define MAX_ACTIONS             16
#define MAX_EVENTS              128
#define MAX_PATTERNS            16
#define MAX_PATTERN_EVENTS      8
#define TOPIC_MAX_LEN           64
#define NAME_MAX_LEN            32
#define EPSILON                 1e-6

/* =============================================================================
 * PART A: RULE ENGINE
 * =============================================================================
 */

/**
 * Condition types for rule evaluation.
 */
typedef enum {
    COND_GT,            /* Greater than threshold1 */
    COND_LT,            /* Less than threshold1 */
    COND_EQ,            /* Equal to threshold1 (within epsilon) */
    COND_GTE,           /* Greater than or equal */
    COND_LTE,           /* Less than or equal */
    COND_RANGE,         /* Within [threshold1, threshold2] */
    COND_OUTSIDE        /* Outside [threshold1, threshold2] */
} ConditionType;

/**
 * Individual rule definition.
 */
typedef struct {
    char topic_filter[TOPIC_MAX_LEN];   /* Topic pattern (supports wildcards) */
    ConditionType condition;
    double threshold1;
    double threshold2;                   /* For RANGE/OUTSIDE conditions */
    char action_name[NAME_MAX_LEN];
    int priority;                        /* Higher priority evaluated first */
    bool enabled;
    size_t trigger_count;                /* Statistics */
    time_t last_triggered;
} Rule;

/**
 * Rule engine container.
 */
typedef struct {
    Rule rules[MAX_RULES];
    size_t count;
    size_t capacity;
    size_t total_evaluations;
    size_t total_triggers;
} RuleEngine;

/**
 * Match topic against filter with wildcard support.
 * Supports '+' (single level) and '#' (multi-level) wildcards.
 * 
 * Examples:
 *   "sensors/+/temperature" matches "sensors/room1/temperature"
 *   "sensors/#" matches "sensors/room1/temperature/celsius"
 *
 * Time Complexity: O(n) where n = topic length
 */
static bool topic_matches(const char *filter, const char *topic) {
    const char *f = filter;
    const char *t = topic;
    
    while (*f && *t) {
        if (*f == '#') {
            /* Multi-level wildcard: matches rest of topic */
            return true;
        }
        
        if (*f == '+') {
            /* Single-level wildcard: skip until next '/' or end */
            while (*t && *t != '/') t++;
            f++;
            if (*f == '/' && *t == '/') {
                f++;
                t++;
            }
            continue;
        }
        
        if (*f != *t) {
            return false;
        }
        
        f++;
        t++;
    }
    
    /* Handle trailing '#' */
    if (*f == '#') return true;
    
    /* Both must reach end for exact match */
    return (*f == '\0' && *t == '\0');
}

/**
 * Evaluate a condition against a value.
 *
 * Time Complexity: O(1)
 */
static bool evaluate_condition(ConditionType cond, double value,
                                double th1, double th2) {
    switch (cond) {
        case COND_GT:
            return value > th1;
        case COND_LT:
            return value < th1;
        case COND_EQ:
            return fabs(value - th1) < EPSILON;
        case COND_GTE:
            return value >= th1;
        case COND_LTE:
            return value <= th1;
        case COND_RANGE:
            return value >= th1 && value <= th2;
        case COND_OUTSIDE:
            return value < th1 || value > th2;
        default:
            return false;
    }
}

/**
 * Create a new rule engine.
 *
 * Time Complexity: O(1)
 * Space Complexity: O(capacity)
 */
RuleEngine *rule_engine_create(size_t capacity) {
    RuleEngine *eng = calloc(1, sizeof(RuleEngine));
    if (!eng) return NULL;
    
    eng->capacity = (capacity > MAX_RULES) ? MAX_RULES : capacity;
    eng->count = 0;
    eng->total_evaluations = 0;
    eng->total_triggers = 0;
    
    return eng;
}

/**
 * Add a rule to the engine.
 * Rules are kept sorted by priority (descending).
 *
 * Time Complexity: O(n) for insertion sort
 *
 * @return Index of added rule, or -1 on failure
 */
int rule_engine_add(RuleEngine *eng, const char *topic_filter,
                    ConditionType condition, double threshold1,
                    double threshold2, const char *action_name,
                    int priority) {
    if (!eng || eng->count >= eng->capacity) return -1;
    
    /* Find insertion position (keep sorted by priority descending) */
    size_t pos = eng->count;
    for (size_t i = 0; i < eng->count; i++) {
        if (priority > eng->rules[i].priority) {
            pos = i;
            break;
        }
    }
    
    /* Shift rules to make room */
    for (size_t i = eng->count; i > pos; i--) {
        eng->rules[i] = eng->rules[i - 1];
    }
    
    /* Insert new rule */
    Rule *rule = &eng->rules[pos];
    strncpy(rule->topic_filter, topic_filter, TOPIC_MAX_LEN - 1);
    rule->topic_filter[TOPIC_MAX_LEN - 1] = '\0';
    rule->condition = condition;
    rule->threshold1 = threshold1;
    rule->threshold2 = threshold2;
    strncpy(rule->action_name, action_name, NAME_MAX_LEN - 1);
    rule->action_name[NAME_MAX_LEN - 1] = '\0';
    rule->priority = priority;
    rule->enabled = true;
    rule->trigger_count = 0;
    rule->last_triggered = 0;
    
    eng->count++;
    return (int)pos;
}

/* Forward declaration for dispatcher */
struct Dispatcher;
bool dispatcher_trigger(struct Dispatcher *disp, const char *name, void *data);

/**
 * Evaluate all rules against an incoming message.
 * Triggers matching actions through the dispatcher.
 *
 * Time Complexity: O(r × t) where r = rules, t = topic length
 *
 * @param eng Rule engine
 * @param topic Message topic
 * @param value Numeric value
 * @param disp Dispatcher for action execution (may be NULL)
 * @return Number of rules triggered
 */
int rule_engine_evaluate(RuleEngine *eng, const char *topic, double value,
                         struct Dispatcher *disp) {
    if (!eng) return 0;
    
    int triggered = 0;
    eng->total_evaluations++;
    
    for (size_t i = 0; i < eng->count; i++) {
        Rule *rule = &eng->rules[i];
        
        if (!rule->enabled) continue;
        
        /* Check topic match */
        if (!topic_matches(rule->topic_filter, topic)) continue;
        
        /* Check condition */
        if (evaluate_condition(rule->condition, value,
                               rule->threshold1, rule->threshold2)) {
            
            printf("    [RULE] '%s' triggered: %s = %.2f %s %.2f\n",
                   rule->action_name, topic, value,
                   rule->condition == COND_GT ? ">" :
                   rule->condition == COND_LT ? "<" :
                   rule->condition == COND_RANGE ? "in range" : "?",
                   rule->threshold1);
            
            rule->trigger_count++;
            rule->last_triggered = time(NULL);
            eng->total_triggers++;
            triggered++;
            
            /* Trigger action if dispatcher provided */
            if (disp) {
                dispatcher_trigger(disp, rule->action_name, (void *)&value);
            }
        }
    }
    
    return triggered;
}

/**
 * Get rule engine statistics.
 */
void rule_engine_print_stats(RuleEngine *eng) {
    if (!eng) return;
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    RULE ENGINE STATISTICS                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("  Total rules:        %zu\n", eng->count);
    printf("  Total evaluations:  %zu\n", eng->total_evaluations);
    printf("  Total triggers:     %zu\n", eng->total_triggers);
    printf("\n  Per-rule statistics:\n");
    
    for (size_t i = 0; i < eng->count; i++) {
        Rule *r = &eng->rules[i];
        printf("    [%zu] %-20s: %zu triggers (pri=%d, %s)\n",
               i, r->topic_filter, r->trigger_count, r->priority,
               r->enabled ? "enabled" : "disabled");
    }
}

/**
 * Free rule engine resources.
 */
void rule_engine_free(RuleEngine *eng) {
    free(eng);
}

/* =============================================================================
 * PART B: TEMPORAL EVENT CORRELATION
 * =============================================================================
 */

/**
 * Event structure for correlation.
 */
typedef struct {
    char event_name[NAME_MAX_LEN];
    time_t timestamp;
    double value;
    bool active;    /* Slot in use */
} Event;

/**
 * Correlation pattern definition.
 */
typedef struct {
    char pattern_name[NAME_MAX_LEN];
    char required_events[MAX_PATTERN_EVENTS][NAME_MAX_LEN];
    size_t event_count;
    double time_window_sec;
    void (*callback)(const char *pattern, Event *events, size_t count);
    size_t trigger_count;
    bool enabled;
} CorrelationPattern;

/**
 * Event correlator maintaining sliding window of events.
 */
typedef struct {
    Event events[MAX_EVENTS];
    size_t head;                    /* Next write position */
    size_t count;                   /* Current event count */
    CorrelationPattern patterns[MAX_PATTERNS];
    size_t pattern_count;
    double window_sec;              /* Global window for event retention */
} Correlator;

/**
 * Create a new event correlator.
 *
 * @param window_sec Default time window for event retention
 */
Correlator *correlator_create(double window_sec) {
    Correlator *corr = calloc(1, sizeof(Correlator));
    if (!corr) return NULL;
    
    corr->window_sec = window_sec;
    corr->head = 0;
    corr->count = 0;
    corr->pattern_count = 0;
    
    return corr;
}

/**
 * Add a correlation pattern to monitor.
 *
 * Example: Fire detection requires "smoke" AND "high_temp" within 30 seconds.
 *
 * @return Pattern index, or -1 on failure
 */
int correlator_add_pattern(Correlator *corr, const char *name,
                            const char **required_events, size_t count,
                            double window_sec,
                            void (*callback)(const char *, Event *, size_t)) {
    if (!corr || corr->pattern_count >= MAX_PATTERNS) return -1;
    if (count > MAX_PATTERN_EVENTS) return -1;
    
    CorrelationPattern *p = &corr->patterns[corr->pattern_count];
    strncpy(p->pattern_name, name, NAME_MAX_LEN - 1);
    p->pattern_name[NAME_MAX_LEN - 1] = '\0';
    
    for (size_t i = 0; i < count; i++) {
        strncpy(p->required_events[i], required_events[i], NAME_MAX_LEN - 1);
        p->required_events[i][NAME_MAX_LEN - 1] = '\0';
    }
    
    p->event_count = count;
    p->time_window_sec = window_sec;
    p->callback = callback;
    p->trigger_count = 0;
    p->enabled = true;
    
    return (int)(corr->pattern_count++);
}

/**
 * Purge events outside the retention window.
 */
static void correlator_purge_old(Correlator *corr, time_t now) {
    for (size_t i = 0; i < MAX_EVENTS; i++) {
        if (corr->events[i].active) {
            if (difftime(now, corr->events[i].timestamp) > corr->window_sec) {
                corr->events[i].active = false;
                if (corr->count > 0) corr->count--;
            }
        }
    }
}

/**
 * Check if a pattern is satisfied within its time window.
 * Uses a greedy approach: finds most recent matching event for each requirement.
 *
 * Time Complexity: O(r × e) where r = required events, e = stored events
 *
 * @return true if pattern matched
 */
static bool correlator_check_pattern(Correlator *corr, CorrelationPattern *pattern,
                                      time_t now, Event *matched_events) {
    if (!pattern->enabled) return false;
    
    /* Track which required events have been found */
    bool found[MAX_PATTERN_EVENTS] = {false};
    time_t earliest = now;
    time_t latest = 0;
    
    for (size_t i = 0; i < pattern->event_count; i++) {
        /* Find most recent matching event */
        time_t best_time = 0;
        int best_idx = -1;
        
        for (size_t j = 0; j < MAX_EVENTS; j++) {
            if (!corr->events[j].active) continue;
            
            if (strcmp(corr->events[j].event_name, pattern->required_events[i]) == 0) {
                if (corr->events[j].timestamp > best_time) {
                    best_time = corr->events[j].timestamp;
                    best_idx = (int)j;
                }
            }
        }
        
        if (best_idx >= 0) {
            found[i] = true;
            if (matched_events) {
                matched_events[i] = corr->events[best_idx];
            }
            if (best_time < earliest) earliest = best_time;
            if (best_time > latest) latest = best_time;
        }
    }
    
    /* Check all events found */
    for (size_t i = 0; i < pattern->event_count; i++) {
        if (!found[i]) return false;
    }
    
    /* Check time window constraint */
    double span = difftime(latest, earliest);
    if (span > pattern->time_window_sec) return false;
    
    return true;
}

/**
 * Add an event and check for pattern matches.
 *
 * Time Complexity: O(p × r × e) where p = patterns
 */
void correlator_add_event(Correlator *corr, const char *event_name,
                           double value, time_t timestamp) {
    if (!corr) return;
    
    time_t now = timestamp ? timestamp : time(NULL);
    
    /* Purge old events first */
    correlator_purge_old(corr, now);
    
    /* Add new event in circular buffer fashion */
    Event *e = &corr->events[corr->head];
    strncpy(e->event_name, event_name, NAME_MAX_LEN - 1);
    e->event_name[NAME_MAX_LEN - 1] = '\0';
    e->timestamp = now;
    e->value = value;
    e->active = true;
    
    corr->head = (corr->head + 1) % MAX_EVENTS;
    if (corr->count < MAX_EVENTS) corr->count++;
    
    printf("    [EVENT] Added: '%s' = %.2f\n", event_name, value);
    
    /* Check all patterns */
    Event matched[MAX_PATTERN_EVENTS];
    for (size_t i = 0; i < corr->pattern_count; i++) {
        CorrelationPattern *p = &corr->patterns[i];
        
        if (correlator_check_pattern(corr, p, now, matched)) {
            printf("    [CORRELATION] Pattern '%s' MATCHED!\n", p->pattern_name);
            p->trigger_count++;
            
            if (p->callback) {
                p->callback(p->pattern_name, matched, p->event_count);
            }
            
            /* Mark matched events as consumed (optional: prevents re-triggering) */
            /* For continuous monitoring, you might want to keep them */
        }
    }
}

/**
 * Print correlator statistics.
 */
void correlator_print_stats(Correlator *corr) {
    if (!corr) return;
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                  CORRELATOR STATISTICS                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("  Active events:  %zu\n", corr->count);
    printf("  Patterns:       %zu\n", corr->pattern_count);
    printf("\n  Pattern triggers:\n");
    
    for (size_t i = 0; i < corr->pattern_count; i++) {
        CorrelationPattern *p = &corr->patterns[i];
        printf("    [%zu] %-20s: %zu matches (window=%.1fs, %s)\n",
               i, p->pattern_name, p->trigger_count,
               p->time_window_sec, p->enabled ? "enabled" : "disabled");
    }
}

/**
 * Free correlator resources.
 */
void correlator_free(Correlator *corr) {
    free(corr);
}

/* =============================================================================
 * PART C: ACTION DISPATCHER WITH RATE LIMITING
 * =============================================================================
 */

/**
 * Action definition with rate limiting.
 */
typedef struct {
    char name[NAME_MAX_LEN];
    void (*execute)(const char *name, void *data);
    size_t min_interval_ms;     /* Minimum time between executions */
    time_t last_execution;      /* Timestamp of last execution (seconds) */
    long last_execution_ms;     /* Milliseconds component */
    size_t execution_count;
    size_t suppressed_count;    /* Times rate-limited */
    bool enabled;
} Action;

/**
 * Action dispatcher.
 */
typedef struct Dispatcher {
    Action actions[MAX_ACTIONS];
    size_t count;
} Dispatcher;

/**
 * Get current time in milliseconds (approximate using clock()).
 */
static long current_time_ms(void) {
    return (long)(clock() / (CLOCKS_PER_SEC / 1000));
}

/**
 * Create a new dispatcher.
 */
Dispatcher *dispatcher_create(void) {
    Dispatcher *disp = calloc(1, sizeof(Dispatcher));
    return disp;
}

/**
 * Register an action with rate limiting.
 *
 * @param name Action identifier
 * @param execute Function to call when triggered
 * @param min_interval_ms Minimum milliseconds between executions
 * @return Action index, or -1 on failure
 */
int dispatcher_register(Dispatcher *disp, const char *name,
                         void (*execute)(const char *, void *),
                         size_t min_interval_ms) {
    if (!disp || disp->count >= MAX_ACTIONS) return -1;
    
    Action *act = &disp->actions[disp->count];
    strncpy(act->name, name, NAME_MAX_LEN - 1);
    act->name[NAME_MAX_LEN - 1] = '\0';
    act->execute = execute;
    act->min_interval_ms = min_interval_ms;
    act->last_execution = 0;
    act->last_execution_ms = 0;
    act->execution_count = 0;
    act->suppressed_count = 0;
    act->enabled = true;
    
    return (int)(disp->count++);
}

/**
 * Find action by name.
 *
 * @return Action pointer or NULL
 */
static Action *dispatcher_find(Dispatcher *disp, const char *name) {
    for (size_t i = 0; i < disp->count; i++) {
        if (strcmp(disp->actions[i].name, name) == 0) {
            return &disp->actions[i];
        }
    }
    return NULL;
}

/**
 * Trigger an action with rate limiting.
 *
 * @return true if action was executed, false if rate-limited or not found
 */
bool dispatcher_trigger(Dispatcher *disp, const char *name, void *data) {
    if (!disp) return false;
    
    Action *act = dispatcher_find(disp, name);
    if (!act || !act->enabled) return false;
    
    long now_ms = current_time_ms();
    time_t now = time(NULL);
    
    /* Check rate limit */
    if (act->last_execution > 0) {
        /* Calculate elapsed time */
        double elapsed_sec = difftime(now, act->last_execution);
        long elapsed_ms = (long)(elapsed_sec * 1000) + (now_ms - act->last_execution_ms);
        
        if (elapsed_ms < (long)act->min_interval_ms) {
            act->suppressed_count++;
            printf("    [RATE-LIMITED] Action '%s' suppressed (wait %ldms)\n",
                   name, act->min_interval_ms - elapsed_ms);
            return false;
        }
    }
    
    /* Execute action */
    act->last_execution = now;
    act->last_execution_ms = now_ms;
    act->execution_count++;
    
    printf("    [ACTION] Executing '%s'\n", name);
    
    if (act->execute) {
        act->execute(name, data);
    }
    
    return true;
}

/**
 * Print dispatcher statistics.
 */
void dispatcher_print_stats(Dispatcher *disp) {
    if (!disp) return;
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                  DISPATCHER STATISTICS                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("  Registered actions: %zu\n", disp->count);
    printf("\n  Per-action statistics:\n");
    
    size_t total_exec = 0, total_supp = 0;
    for (size_t i = 0; i < disp->count; i++) {
        Action *a = &disp->actions[i];
        printf("    [%zu] %-20s: %zu executed, %zu suppressed (interval=%zums)\n",
               i, a->name, a->execution_count, a->suppressed_count,
               a->min_interval_ms);
        total_exec += a->execution_count;
        total_supp += a->suppressed_count;
    }
    
    printf("\n  Total: %zu executed, %zu suppressed (%.1f%% rate-limited)\n",
           total_exec, total_supp,
           (total_exec + total_supp) > 0 
               ? 100.0 * total_supp / (total_exec + total_supp) : 0.0);
}

/**
 * Free dispatcher resources.
 */
void dispatcher_free(Dispatcher *disp) {
    free(disp);
}

/* =============================================================================
 * INTEGRATED IoT GATEWAY
 * =============================================================================
 */

/**
 * Complete IoT gateway combining all components.
 */
typedef struct {
    RuleEngine *rules;
    Correlator *correlator;
    Dispatcher *dispatcher;
} IoTGateway;

/**
 * Create integrated gateway.
 */
IoTGateway *gateway_create(void) {
    IoTGateway *gw = calloc(1, sizeof(IoTGateway));
    if (!gw) return NULL;
    
    gw->rules = rule_engine_create(MAX_RULES);
    gw->correlator = correlator_create(60.0);  /* 60 second event window */
    gw->dispatcher = dispatcher_create();
    
    if (!gw->rules || !gw->correlator || !gw->dispatcher) {
        free(gw->rules);
        free(gw->correlator);
        free(gw->dispatcher);
        free(gw);
        return NULL;
    }
    
    return gw;
}

/**
 * Process incoming sensor message through gateway.
 */
void gateway_process(IoTGateway *gw, const char *topic, double value) {
    printf("\n  Processing: %s = %.2f\n", topic, value);
    
    /* Evaluate rules */
    rule_engine_evaluate(gw->rules, topic, value, gw->dispatcher);
    
    /* Extract event name from topic (last component) */
    const char *event_name = strrchr(topic, '/');
    event_name = event_name ? event_name + 1 : topic;
    
    /* Add to correlator */
    correlator_add_event(gw->correlator, event_name, value, 0);
}

/**
 * Print complete gateway statistics.
 */
void gateway_print_stats(IoTGateway *gw) {
    rule_engine_print_stats(gw->rules);
    correlator_print_stats(gw->correlator);
    dispatcher_print_stats(gw->dispatcher);
}

/**
 * Free gateway resources.
 */
void gateway_free(IoTGateway *gw) {
    if (!gw) return;
    rule_engine_free(gw->rules);
    correlator_free(gw->correlator);
    dispatcher_free(gw->dispatcher);
    free(gw);
}

/* =============================================================================
 * DEMONSTRATION CALLBACKS
 * =============================================================================
 */

/**
 * Sample action: send alert.
 */
void action_send_alert(const char *name, void *data) {
    double *value = (double *)data;
    printf("      → ALERT: %s triggered (value=%.2f)\n", name, value ? *value : 0.0);
}

/**
 * Sample action: activate cooling.
 */
void action_activate_cooling(const char *name, void *data) {
    (void)name;
    (void)data;
    printf("      → HVAC: Cooling system ACTIVATED\n");
}

/**
 * Sample action: emergency shutdown.
 */
void action_emergency_shutdown(const char *name, void *data) {
    (void)name;
    (void)data;
    printf("      → EMERGENCY: Initiating shutdown sequence!\n");
}

/**
 * Correlation callback: fire detection.
 */
void fire_detected_callback(const char *pattern, Event *events, size_t count) {
    printf("      → FIRE ALARM: Pattern '%s' detected!\n", pattern);
    printf("        Contributing events:\n");
    for (size_t i = 0; i < count; i++) {
        printf("          - %s = %.2f\n", events[i].event_name, events[i].value);
    }
}

/* =============================================================================
 * DEMONSTRATIONS
 * =============================================================================
 */

/**
 * Demonstrate rule engine functionality.
 */
void demo_rule_engine(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART A: RULE ENGINE DEMONSTRATION                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    RuleEngine *eng = rule_engine_create(10);
    
    /* Add rules */
    rule_engine_add(eng, "sensors/+/temperature", COND_GT, 30.0, 0, "cooling_alert", 10);
    rule_engine_add(eng, "sensors/+/temperature", COND_GT, 50.0, 0, "critical_temp", 20);
    rule_engine_add(eng, "sensors/#", COND_RANGE, 0.0, 100.0, "range_check", 1);
    rule_engine_add(eng, "sensors/room1/humidity", COND_LT, 20.0, 0, "low_humidity", 5);
    
    printf("Rules configured:\n");
    printf("  1. Temperature > 30°C → cooling_alert (priority 10)\n");
    printf("  2. Temperature > 50°C → critical_temp (priority 20)\n");
    printf("  3. Any sensor in [0,100] → range_check (priority 1)\n");
    printf("  4. Room1 humidity < 20%% → low_humidity (priority 5)\n\n");
    
    /* Test messages */
    printf("Processing test messages:\n");
    
    struct {
        const char *topic;
        double value;
    } messages[] = {
        {"sensors/room1/temperature", 25.0},
        {"sensors/room1/temperature", 35.0},
        {"sensors/room1/temperature", 55.0},
        {"sensors/room2/temperature", 28.0},
        {"sensors/room1/humidity", 45.0},
        {"sensors/room1/humidity", 15.0},
    };
    
    for (size_t i = 0; i < sizeof(messages) / sizeof(messages[0]); i++) {
        printf("\n  [%zu] Topic: %s, Value: %.1f\n", i + 1,
               messages[i].topic, messages[i].value);
        int triggered = rule_engine_evaluate(eng, messages[i].topic,
                                              messages[i].value, NULL);
        printf("      Rules triggered: %d\n", triggered);
    }
    
    rule_engine_print_stats(eng);
    rule_engine_free(eng);
}

/**
 * Demonstrate temporal correlation.
 */
void demo_correlation(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART B: TEMPORAL CORRELATION DEMONSTRATION               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Correlator *corr = correlator_create(30.0);
    
    /* Define fire detection pattern */
    const char *fire_events[] = {"smoke_detected", "high_temperature"};
    correlator_add_pattern(corr, "fire_detection",
                            fire_events, 2, 30.0, fire_detected_callback);
    
    /* Define intrusion pattern */
    const char *intrusion_events[] = {"motion_detected", "door_opened", "window_breach"};
    correlator_add_pattern(corr, "intrusion_detection",
                            intrusion_events, 3, 60.0, NULL);
    
    printf("Correlation patterns configured:\n");
    printf("  1. Fire detection: smoke + high_temp within 30s\n");
    printf("  2. Intrusion: motion + door + window within 60s\n\n");
    
    printf("Simulating event sequence:\n");
    
    time_t base_time = time(NULL);
    
    /* Scenario 1: Normal events (no pattern match) */
    printf("\n--- Scenario 1: Unrelated events ---\n");
    correlator_add_event(corr, "motion_detected", 1.0, base_time);
    correlator_add_event(corr, "temperature_normal", 22.0, base_time + 5);
    
    /* Scenario 2: Fire detection */
    printf("\n--- Scenario 2: Fire pattern ---\n");
    correlator_add_event(corr, "smoke_detected", 1.0, base_time + 10);
    correlator_add_event(corr, "high_temperature", 85.0, base_time + 15);
    
    /* Scenario 3: Intrusion (incomplete - only 2 of 3 events) */
    printf("\n--- Scenario 3: Partial intrusion pattern ---\n");
    correlator_add_event(corr, "motion_detected", 1.0, base_time + 100);
    correlator_add_event(corr, "door_opened", 1.0, base_time + 105);
    /* Missing window_breach - pattern won't trigger */
    
    correlator_print_stats(corr);
    correlator_free(corr);
}

/**
 * Demonstrate rate-limited action dispatcher.
 */
void demo_dispatcher(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART C: RATE-LIMITED DISPATCHER DEMONSTRATION            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Dispatcher *disp = dispatcher_create();
    
    /* Register actions with different rate limits */
    dispatcher_register(disp, "send_sms", action_send_alert, 5000);    /* 5 second cooldown */
    dispatcher_register(disp, "email_alert", action_send_alert, 10000); /* 10 second cooldown */
    dispatcher_register(disp, "log_event", action_send_alert, 100);     /* 100ms cooldown */
    
    printf("Actions registered:\n");
    printf("  1. send_sms: 5000ms minimum interval\n");
    printf("  2. email_alert: 10000ms minimum interval\n");
    printf("  3. log_event: 100ms minimum interval\n\n");
    
    printf("Triggering actions rapidly:\n");
    
    double test_value = 42.0;
    for (int i = 0; i < 5; i++) {
        printf("\n  Burst %d:\n", i + 1);
        dispatcher_trigger(disp, "send_sms", &test_value);
        dispatcher_trigger(disp, "email_alert", &test_value);
        dispatcher_trigger(disp, "log_event", &test_value);
        
        /* Small delay to allow log_event through */
        for (volatile int j = 0; j < 1000000; j++);
    }
    
    dispatcher_print_stats(disp);
    dispatcher_free(disp);
}

/**
 * Demonstrate complete integrated gateway.
 */
void demo_integrated_gateway(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      INTEGRATED IoT GATEWAY DEMONSTRATION                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    IoTGateway *gw = gateway_create();
    
    /* Configure rules */
    rule_engine_add(gw->rules, "sensors/+/temperature", COND_GT, 40.0, 0,
                    "activate_cooling", 10);
    rule_engine_add(gw->rules, "sensors/+/temperature", COND_GT, 60.0, 0,
                    "emergency_shutdown", 20);
    rule_engine_add(gw->rules, "sensors/+/smoke", COND_GT, 0.5, 0,
                    "fire_alert", 15);
    
    /* Configure actions */
    dispatcher_register(gw->dispatcher, "activate_cooling",
                         action_activate_cooling, 2000);
    dispatcher_register(gw->dispatcher, "emergency_shutdown",
                         action_emergency_shutdown, 30000);
    dispatcher_register(gw->dispatcher, "fire_alert",
                         action_send_alert, 5000);
    
    /* Configure correlation */
    const char *fire_pattern[] = {"smoke", "temperature"};
    correlator_add_pattern(gw->correlator, "fire_confirmed",
                            fire_pattern, 2, 30.0, fire_detected_callback);
    
    printf("Gateway configured with:\n");
    printf("  - 3 rules (cooling, emergency, fire)\n");
    printf("  - 3 rate-limited actions\n");
    printf("  - 1 correlation pattern (fire confirmation)\n\n");
    
    printf("Processing sensor stream:\n");
    
    /* Simulate sensor readings */
    gateway_process(gw, "sensors/room1/temperature", 35.0);
    gateway_process(gw, "sensors/room1/temperature", 45.0);  /* Triggers cooling */
    gateway_process(gw, "sensors/room1/temperature", 48.0);  /* Rate-limited */
    gateway_process(gw, "sensors/room1/smoke", 0.8);         /* Fire alert */
    gateway_process(gw, "sensors/room1/temperature", 65.0);  /* Emergency! */
    
    gateway_print_stats(gw);
    gateway_free(gw);
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 19: ALGORITHMS FOR IoT AND STREAM PROCESSING         ║\n");
    printf("║        Homework 2: Event-Driven IoT Gateway - SOLUTION        ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    demo_rule_engine();
    demo_correlation();
    demo_dispatcher();
    demo_integrated_gateway();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    DEMONSTRATION COMPLETE                     ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║  Key concepts demonstrated:                                   ║\n");
    printf("║    • Rule evaluation with topic wildcards                     ║\n");
    printf("║    • Temporal event correlation with sliding windows          ║\n");
    printf("║    • Rate-limited action dispatch                             ║\n");
    printf("║    • Integrated event-driven gateway architecture             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

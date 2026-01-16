# Week 19: Homework Requirements

## Overview

This week's homework focuses on practical IoT and stream processing applications.
You will implement two substantial systems that build upon the laboratory concepts.

---

## Homework 1: Multi-Sensor Fusion System (50 points)

### Problem Description

Industrial monitoring systems often combine readings from multiple redundant sensors
to improve reliability. Implement a sensor fusion system that:
- Manages an array of sensors (same type, e.g., temperature)
- Detects sensor failures (stuck, drift, spike)
- Produces a fused estimate using weighted averaging

### Part A: Sensor Array Management (15 points)

Implement a structure and functions to manage multiple sensors:

```c
typedef struct {
    char id[32];
    CircularBuffer *history;    /* Recent readings */
    WelfordState stats;         /* Running statistics */
    double weight;              /* Fusion weight (0-1) */
    SensorStatus status;        /* NORMAL, SUSPECTED, FAILED */
    size_t consecutive_errors;  /* For failure detection */
} SensorNode;

typedef struct {
    SensorNode *sensors;
    size_t count;
    size_t capacity;
} SensorArray;
```

Required functions:
- `SensorArray *sensor_array_create(size_t capacity)`
- `int sensor_array_add(SensorArray *arr, const char *id)`
- `void sensor_array_update(SensorArray *arr, int idx, double value)`
- `void sensor_array_free(SensorArray *arr)`

### Part B: Weighted Averaging Fusion (20 points)

Implement sensor fusion using inverse-variance weighting:

```
weight_i = 1 / variance_i
fused_value = Σ(weight_i × value_i) / Σ(weight_i)
```

Requirements:
- Only include sensors with status NORMAL
- Handle case where all sensors are failed
- Update weights dynamically based on recent variance

```c
double sensor_fusion_estimate(SensorArray *arr);
double sensor_fusion_confidence(SensorArray *arr);
```

### Part C: Failure Detection (15 points)

Implement three failure modes:

1. **Stuck Sensor**: Value unchanged for N readings
   ```c
   bool detect_stuck(SensorNode *node, int threshold);
   ```

2. **Drift Fault**: Gradual deviation from group consensus
   ```c
   bool detect_drift(SensorNode *node, double group_mean, double threshold);
   ```

3. **Spike Fault**: Sudden large deviation
   ```c
   bool detect_spike(SensorNode *node, double new_value, double threshold);
   ```

### Testing

Your submission will be tested with:
- 3-5 sensors with various failure injections
- Verification of fused estimate accuracy
- Correct failure detection timing

### Compilation

```bash
gcc -Wall -Wextra -std=c11 -o homework1 homework1.c -lm
./homework1 < test_data/fusion_test.txt
```

---

## Homework 2: Event-Driven IoT Gateway (50 points)

### Problem Description

IoT gateways process sensor data and trigger actions based on rules.
Implement an event-driven system with:
- Rule engine for conditional actions
- Temporal event correlation
- Action dispatcher with rate limiting

### Part A: Rule Engine (20 points)

Define rules that trigger actions based on conditions:

```c
typedef enum {
    COND_GT,      /* Greater than */
    COND_LT,      /* Less than */
    COND_EQ,      /* Equal (within epsilon) */
    COND_RANGE    /* Within range */
} ConditionType;

typedef struct {
    char topic_filter[64];
    ConditionType condition;
    double threshold1;
    double threshold2;         /* For RANGE: [threshold1, threshold2] */
    char action_name[32];
    void *action_data;
} Rule;

typedef struct {
    Rule *rules;
    size_t count;
    size_t capacity;
} RuleEngine;
```

Required functions:
- `RuleEngine *rule_engine_create(size_t capacity)`
- `int rule_engine_add(RuleEngine *eng, Rule *rule)`
- `void rule_engine_evaluate(RuleEngine *eng, const char *topic, double value)`
- `void rule_engine_free(RuleEngine *eng)`

### Part B: Temporal Event Correlation (15 points)

Detect patterns across multiple events within a time window:

```c
typedef struct {
    char event_name[32];
    time_t timestamp;
    double value;
} Event;

typedef struct {
    char pattern_name[32];
    char *required_events[8];    /* List of event names */
    size_t event_count;
    double time_window_sec;      /* All events must occur within this window */
    void (*callback)(const char *pattern, Event *events, size_t count);
} CorrelationPattern;
```

Example: Fire detection requires both "smoke_detected" AND "temperature_high"
within 30 seconds.

```c
bool correlator_check_pattern(Correlator *corr, CorrelationPattern *pattern);
void correlator_add_event(Correlator *corr, Event *event);
```

### Part C: Action Dispatcher (15 points)

Execute actions with rate limiting to prevent flooding:

```c
typedef struct {
    char name[32];
    void (*execute)(void *data);
    size_t min_interval_ms;      /* Minimum time between executions */
    time_t last_execution;
    size_t execution_count;
    size_t suppressed_count;
} Action;

typedef struct {
    Action *actions;
    size_t count;
} Dispatcher;
```

Required functions:
- `int dispatcher_register(Dispatcher *disp, const char *name, 
                           void (*execute)(void *), size_t interval_ms)`
- `bool dispatcher_trigger(Dispatcher *disp, const char *name, void *data)`
- `void dispatcher_print_stats(Dispatcher *disp)`

### Testing

Your submission will be tested with:
- Rule evaluation with various conditions
- Temporal correlation with event timing
- Rate limiting effectiveness

### Compilation

```bash
gcc -Wall -Wextra -std=c11 -o homework2 homework2.c -lm
./homework2 < test_data/gateway_test.txt
```

---

## Submission Guidelines

1. **Files to submit:**
   - `homework1.c` - Multi-sensor fusion implementation
   - `homework2.c` - Event-driven gateway implementation
   - `README.txt` - Brief description of your approach

2. **Code requirements:**
   - Must compile with `gcc -Wall -Wextra -std=c11 -pedantic`
   - No external libraries (only standard C11 + math.h)
   - Proper memory management (no leaks)
   - Comments explaining key algorithms

3. **Deadline:** As specified in course calendar

4. **Grading:**
   - Functionality: 70%
   - Code quality: 15%
   - Documentation: 15%

---

## Tips

1. **Start with data structures**: Design your structs carefully before coding functions

2. **Test incrementally**: Write and test one function at a time

3. **Handle edge cases**:
   - Empty arrays
   - All sensors failed
   - No matching rules
   - Rapid event sequences

4. **Memory management**: Use valgrind to check for leaks

5. **Read the example1.c**: Many patterns are demonstrated there

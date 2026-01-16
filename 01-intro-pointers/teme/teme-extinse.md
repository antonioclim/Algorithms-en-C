# Extended challenges for Week 01: higher-order design in C via function pointers

## 1. Scope and intent

The following challenges are optional enrichment activities intended for students who wish to deepen their understanding of function pointers beyond the laboratory core. Each challenge can be approached as a small research exercise in software design: you are asked to construct an interface, specify its invariants and then implement it in a manner that is correct, testable and portable.

Across all challenges the general expectations remain unchanged.

- The code must compile as ISO C11.
- The code must not rely on undefined behaviour.
- The code must be accompanied by a minimal test harness or a structured test plan.
- Dynamic allocation is permitted but must be justified and must be paired with deterministic deallocation.

Where the task involves a registry, an event bus or an FSM the central concept is the same: *an application-defined function pointer is stored in a data structure and invoked later under the control of a generic engine*.

## 2. Bonus model

Each correctly completed challenge contributes **+10 bonus points** to the final mark for the week, subject to course level rules. A submission is considered complete only if it includes a short design note at the top of the file explaining assumptions, edge cases and the chosen test strategy.

## 3. Challenge 1: a plugin registry for arithmetic operations

### 3.1 Design goal

Design a small plugin registry that allows operations to be registered, listed, looked up and unregistered at runtime. The registry is not dynamic linking in the operating system sense. It is a data structure level abstraction that models the *capability* of dynamic extension.

### 3.2 Core data structures

```c
typedef int (*Operation)(int, int);

typedef struct {
    char name[32];
    char symbol;
    Operation operation;
    char description[100];
} Plugin;

typedef struct {
    Plugin plugins[20];
    int count;
} PluginManager;
```

### 3.3 Required API

```c
void plugin_manager_init(PluginManager *pm);
int plugin_register(PluginManager *pm, const Plugin *plugin);
int plugin_unregister(PluginManager *pm, char symbol);
Plugin *plugin_find(PluginManager *pm, char symbol);
void plugin_list(const PluginManager *pm);
```

### 3.4 Invariants

- `pm->count` is the number of active plugins in the prefix of the `plugins` array.
- Each `symbol` is unique within the registry.
- `operation` must be non-null for an active plugin.

### 3.5 Algorithmic sketch

Registration may be implemented as linear search followed by insertion.

```
procedure register(pm, plugin)
    if pm.count == CAPACITY then return error
    if plugin.operation is null then return error
    if find(pm, plugin.symbol) exists then return error
    pm.plugins[pm.count] <- plugin (copy)
    pm.count <- pm.count + 1
    return ok
end procedure
```

Unregistration can preserve contiguity by moving the last element into the removed slot.

```
procedure unregister(pm, symbol)
    i <- index of plugin with symbol, or none
    if none then return not found
    pm.count <- pm.count - 1
    pm.plugins[i] <- pm.plugins[pm.count]
    return ok
end procedure
```

### 3.6 Testing suggestions

- Register at least five plugins including boundary cases such as the first and last slot.
- Attempt to register duplicates and confirm rejection.
- Unregister a plugin then ensure lookup fails.
- Execute all registered operations through `plugin_find` and confirm correctness.

## 4. Challenge 2: multi-criteria sorting

### 4.1 Design goal

Implement a wrapper that composes multiple comparator functions into a single ordering relation. This mirrors how multi-key ordering is expressed in database systems and in high-level languages.

### 4.2 Data model

```c
typedef struct {
    int (*comparator)(const void *, const void *);
    int ascending; /* 1 for ascending, 0 for descending */
} SortCriterion;
```

### 4.3 Required API

```c
void multi_sort(void *base, size_t nmemb, size_t size,
                SortCriterion *criteria, int num_criteria);
```

### 4.4 Core idea

The composite comparator applies criteria in sequence and returns the first non-zero result, optionally negated for descending order.

```
function composite_cmp(a, b)
    for each criterion c in criteria do
        r <- c.comparator(a, b)
        if r != 0 then
            if c.ascending then return r else return -r
        end if
    end for
    return 0
end function
```

Because `qsort` accepts only a comparator without user context, a portable implementation typically stores the `criteria` array in a static variable or uses `qsort_r` where available. For a bonus exercise you may assume `qsort_r` exists on your platform but you must document that assumption.

### 4.5 Complexity

If the underlying sort performs `O(n log n)` comparisons, the composite comparator may perform up to `num_criteria` primitive comparisons per comparator call, yielding a multiplicative constant factor. This is rarely problematic for small arrays but is analytically important.

## 5. Challenge 3: a generic finite state machine via dispatch tables

### 5.1 Design goal

Construct a finite state machine (FSM) engine where transitions are selected by a two-dimensional dispatch table indexed by `(state, event)`. This makes the transition relation explicit and supports reasoning about completeness.

### 5.2 Core types

```c
typedef enum { STATE_A, STATE_B, STATE_C, STATE_COUNT } State;
typedef enum { EVENT_X, EVENT_Y, EVENT_Z, EVENT_COUNT } Event;

typedef State (*TransitionFunc)(void *context);

typedef struct {
    TransitionFunc transitions[STATE_COUNT][EVENT_COUNT];
    void (*on_enter[STATE_COUNT])(void *context);
    void (*on_exit[STATE_COUNT])(void *context);
} FSM;
```

### 5.3 Processing algorithm

```
function process_event(fsm, current_state, event, context)
    t <- fsm.transitions[current_state][event]
    if t is null then
        return current_state  /* no transition defined */
    next_state <- t(context)
    if next_state != current_state then
        if fsm.on_exit[current_state] exists then call it
        if fsm.on_enter[next_state] exists then call it
    end if
    return next_state
end function
```

### 5.4 Practical instantiation

Choose a domain such as a traffic light controller or a vending machine. State names and events should be domain meaningful, not `STATE_A` and `EVENT_X`. Provide a transition table in the source file and a short trace demonstrating correct evolution.

### 5.5 Testing suggestions

- Verify that undefined transitions leave the state unchanged.
- Verify that `on_exit` and `on_enter` callbacks are invoked exactly when a state change occurs.
- Provide a table coverage test: iterate over all `(state, event)` pairs and assert that all expected transitions are present.

## 6. Challenge 4: generic map, filter and reduce

### 6.1 Design goal

Implement higher-order array combinators in C while keeping the interface byte-based and type-agnostic. This challenge makes explicit the relationship between abstract functional patterns and concrete memory representation.

### 6.2 Required APIs

```c
void array_map(void *dest, const void *src, size_t n, size_t elem_size,
               void (*transform)(void *dest_elem, const void *src_elem));

size_t array_filter(void *dest, const void *src, size_t n, size_t elem_size,
                    int (*predicate)(const void *elem));

void array_reduce(void *result, const void *arr, size_t n, size_t elem_size,
                  void (*reducer)(void *acc, const void *elem),
                  const void *initial);
```

### 6.3 Implementation notes

- `array_map` is a structured loop with deterministic writes.
- `array_filter` is a stable selection operator when implemented with a write index.
- `array_reduce` requires careful treatment of accumulator type and initial value.

A typical pointer arithmetic scheme is:

```c
const unsigned char *in = src;
unsigned char *out = dest;
for (size_t i = 0; i < n; i++) {
    transform(out + i * elem_size, in + i * elem_size);
}
```

### 6.4 Demonstration scenarios

- Integers: map doubling, filter positive, reduce sum.
- Structures: map applying a discount, filter in-stock, reduce total inventory value.

## 7. Challenge 5: an event bus

### 7.1 Design goal

Implement an event subscription system similar to those found in GUI frameworks. The bus stores subscriptions and emits an event by invoking all handlers whose `event_name` matches.

### 7.2 Data model

```c
typedef void (*EventHandler)(void *sender, void *event_data, void *user_data);

typedef struct {
    char event_name[32];
    EventHandler handler;
    void *user_data;
} Subscription;

typedef struct {
    Subscription subscriptions[100];
    int count;
} EventBus;
```

### 7.3 Required API

```c
void event_bus_init(EventBus *bus);
int event_subscribe(EventBus *bus, const char *event_name,
                    EventHandler handler, void *user_data);
int event_unsubscribe(EventBus *bus, const char *event_name,
                      EventHandler handler);
void event_emit(EventBus *bus, const char *event_name,
                void *sender, void *event_data);
```

### 7.4 Algorithmic sketch

```
procedure emit(bus, name, sender, data)
    for i from 0 to bus.count - 1 do
        if subscriptions[i].event_name equals name then
            call subscriptions[i].handler(sender, data, subscriptions[i].user_data)
        end if
    end for
end procedure
```

### 7.5 Testing suggestions

- Subscribe multiple handlers to the same event and confirm invocation order.
- Subscribe handlers to different events and confirm separation.
- Unsubscribe one handler and confirm it is no longer invoked.
- Exercise the capacity limit and confirm rejection behaviour.

## 8. Naming and submission conventions

Name each file according to the challenge number.

- `bonus1_plugins.c`
- `bonus2_multisort.c`
- `bonus3_fsm.c`
- `bonus4_map_filter_reduce.c`
- `bonus5_event_bus.c`

Each file should compile independently and include a minimal `main` function that exercises the API.

## 9. Bibliographic pointers

| Reference (APA 7th) | DOI |
|---|---|
| Bentley, J. L., & McIlroy, M. D. (1993). Engineering a sort function. *Software: Practice and Experience, 23*(11), 1249–1265. | https://doi.org/10.1002/spe.4380231105 |
| Hoare, C. A. R. (1962). Quicksort. *The Computer Journal, 5*(1), 10–16. | https://doi.org/10.1093/comjnl/5.1.10 |

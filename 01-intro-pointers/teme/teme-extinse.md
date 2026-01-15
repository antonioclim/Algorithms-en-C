# Provocări Extinse - Săptămâna 1

## 🚀 Provocări Avansate (Opționale)

Aceste exerciții sunt pentru studenții care doresc să aprofundeze conceptele și să obțină puncte bonus. Fiecare provocare rezolvată corect aduce **+10 puncte bonus** la nota finală.

---

## ⭐ Provocare 1: Sistem de Plugins (Dificultate: Medie)

### Descriere

Implementați un sistem simplu de plugins care permite încărcarea dinamică de operații matematice.

### Cerințe

1. **Structura Plugin**:
   ```c
   typedef struct {
       char name[32];
       char symbol;
       int (*operation)(int, int);
       char description[100];
   } Plugin;
   ```

2. **Manager de Plugins**:
   ```c
   typedef struct {
       Plugin plugins[20];
       int count;
   } PluginManager;

   void plugin_manager_init(PluginManager *pm);
   int plugin_register(PluginManager *pm, const Plugin *plugin);
   int plugin_unregister(PluginManager *pm, char symbol);
   Plugin* plugin_find(PluginManager *pm, char symbol);
   void plugin_list(const PluginManager *pm);
   ```

3. **Demonstrație**:
   - Înregistrați 5+ operații
   - Afișați lista de plugin-uri disponibile
   - Executați operații folosind plugin-urile
   - Dezînregistrați un plugin și verificați că nu mai este disponibil

### Puncte Bonus: +10

---

## ⭐ Provocare 2: Sortare Multi-Criteriu (Dificultate: Medie-Ridicată)

### Descriere

Implementați un sistem de sortare care poate combina multiple criterii de sortare.

### Cerințe

1. **Structură pentru criteriu de sortare**:
   ```c
   typedef struct {
       int (*comparator)(const void*, const void*);
       int ascending;  // 1 = ascending, 0 = descending
   } SortCriterion;
   ```

2. **Sortare cu multiple criterii**:
   ```c
   // Sortează mai întâi după primul criteriu, apoi după al doilea pentru elemente egale, etc.
   void multi_sort(void *base, size_t nmemb, size_t size,
                   SortCriterion *criteria, int num_criteria);
   ```

3. **Exemplu de utilizare**:
   - Sortați studenți mai întâi după an de studiu, apoi după medie (descrescător), apoi după nume

### Puncte Bonus: +10

---

## ⭐ Provocare 3: Mașină de Stări Finită (Dificultate: Ridicată)

### Descriere

Implementați un FSM (Finite State Machine) generic folosind tabele de dispatch.

### Cerințe

1. **Structuri de bază**:
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

2. **Funcții FSM**:
   ```c
   void fsm_init(FSM *fsm);
   void fsm_set_transition(FSM *fsm, State from, Event event, TransitionFunc func);
   void fsm_set_on_enter(FSM *fsm, State state, void (*callback)(void*));
   void fsm_set_on_exit(FSM *fsm, State state, void (*callback)(void*));
   State fsm_process_event(FSM *fsm, State current, Event event, void *context);
   ```

3. **Aplicație practică**: Implementați un semafor de trafic sau un automat de vânzare.

### Puncte Bonus: +10

---

## ⭐ Provocare 4: Generic Map/Filter/Reduce (Dificultate: Medie)

### Descriere

Implementați funcțiile de ordin superior map, filter și reduce în C.

### Cerințe

1. **Map**:
   ```c
   void array_map(void *dest, const void *src, size_t n, size_t elem_size,
                  void (*transform)(void *dest_elem, const void *src_elem));
   ```

2. **Filter**:
   ```c
   size_t array_filter(void *dest, const void *src, size_t n, size_t elem_size,
                       int (*predicate)(const void *elem));
   ```

3. **Reduce**:
   ```c
   void array_reduce(void *result, const void *arr, size_t n, size_t elem_size,
                     void (*reducer)(void *acc, const void *elem),
                     const void *initial);
   ```

4. **Demonstrație cu int și struct**:
   - Map: dublează fiecare element
   - Filter: păstrează doar elementele pozitive
   - Reduce: calculează suma/produsul

### Puncte Bonus: +10

---

## ⭐ Provocare 5: Event System (Dificultate: Ridicată)

### Descriere

Implementați un sistem de evenimente similar cu cel din GUI frameworks.

### Cerințe

1. **Structuri**:
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

2. **Funcții**:
   ```c
   void event_bus_init(EventBus *bus);
   int event_subscribe(EventBus *bus, const char *event_name,
                       EventHandler handler, void *user_data);
   int event_unsubscribe(EventBus *bus, const char *event_name,
                         EventHandler handler);
   void event_emit(EventBus *bus, const char *event_name,
                   void *sender, void *event_data);
   ```

3. **Demonstrație**:
   - Creați evenimente "button_click", "key_press", "timer_tick"
   - Înregistrați multiple handler-e pentru fiecare eveniment
   - Emiteți evenimente și verificați că toate handler-ele sunt apelate

### Puncte Bonus: +10

---

## 📊 Sistem de Punctare Bonus

| Provocări Completate | Bonus Total |
|---------------------|-------------|
| 1 provocare | +10 puncte |
| 2 provocări | +20 puncte |
| 3 provocări | +30 puncte |
| 4 provocări | +40 puncte |
| Toate 5 | +50 puncte + Insignă "Master Callbacks" 🏆 |

---

## 📤 Predare

1. Fișierele să fie numite `bonus1_plugins.c`, `bonus2_multisort.c`, etc.
2. Fiecare fișier trebuie să compileze independent
3. Includeți comentarii explicative
4. Testați cu Valgrind pentru memory leaks

---

## 💡 Sfaturi

1. **Începeți cu provocarea care vi se pare cea mai interesantă**
2. **Folosiți typedef extensiv** pentru claritate
3. **Testați incremental** - nu scrieți tot codul și apoi testați
4. **Desenați diagrame** pentru FSM și Event System
5. **Citiți documentația** - man pages pentru funcții standard

---

*Provocările sunt opționale dar recompensate. Succes! 🎯*

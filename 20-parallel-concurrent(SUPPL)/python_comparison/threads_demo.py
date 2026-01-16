#!/usr/bin/env python3
"""
=============================================================================
PYTHON COMPARISON: Threading Module
Week 20: Parallel and Concurrent Programming
=============================================================================

This file demonstrates Python's threading module for comparison with
C's pthread implementation. Python has a Global Interpreter Lock (GIL)
which limits true parallelism for CPU-bound tasks.

Key Differences from C pthreads:
  - GIL prevents true parallel execution of Python bytecode
  - Higher-level abstractions (Lock, Semaphore, Condition)
  - Automatic reference counting (no manual memory management)
  - Thread-safe built-in data structures
  - Easier syntax but less control

When to use Python threading:
  - I/O-bound tasks (file I/O, network requests)
  - GUI applications (keeping UI responsive)
  - When development speed is more important than raw performance

When NOT to use Python threading:
  - CPU-bound computations (use multiprocessing instead)
  - Real-time systems
  - High-performance applications

NOTE: This file is for educational comparison only, not for execution
      in the course exercises.

=============================================================================
"""

import threading
import queue
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from typing import List, Callable, Any
import random

# =============================================================================
# BASIC THREADING
# =============================================================================

def basic_thread_example():
    """
    Equivalent to pthread_create/pthread_join in C.
    
    C equivalent:
        pthread_t thread;
        pthread_create(&thread, NULL, worker, &arg);
        pthread_join(thread, NULL);
    """
    print("\n" + "="*60)
    print("BASIC THREADING")
    print("="*60 + "\n")
    
    def worker(name: str, duration: float) -> None:
        print(f"  Thread {name} starting (will run {duration:.1f}s)")
        time.sleep(duration)
        print(f"  Thread {name} completed")
    
    # Create threads
    threads = []
    for i in range(4):
        t = threading.Thread(
            target=worker,
            args=(f"Worker-{i}", random.uniform(0.5, 1.5))
        )
        threads.append(t)
        t.start()
    
    # Join all threads (wait for completion)
    for t in threads:
        t.join()
    
    print("\nAll threads completed")


# =============================================================================
# MUTEX (LOCK)
# =============================================================================

def mutex_example():
    """
    Equivalent to pthread_mutex_t in C.
    
    C equivalent:
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_lock(&mutex);
        // critical section
        pthread_mutex_unlock(&mutex);
    """
    print("\n" + "="*60)
    print("MUTEX (LOCK) EXAMPLE")
    print("="*60 + "\n")
    
    counter = 0
    counter_lock = threading.Lock()
    iterations = 100000
    
    def increment_unsafe(n: int) -> None:
        """Unsafe increment - demonstrates race condition."""
        nonlocal counter
        for _ in range(n):
            counter += 1  # NOT atomic!
    
    def increment_safe(n: int) -> None:
        """Safe increment using lock."""
        nonlocal counter
        for _ in range(n):
            with counter_lock:  # Equivalent to lock/unlock
                counter += 1
    
    # Test unsafe version
    counter = 0
    threads = [threading.Thread(target=increment_unsafe, args=(iterations,)) 
               for _ in range(4)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    print(f"  Unsafe result: {counter} (expected {iterations * 4})")
    
    # Test safe version
    counter = 0
    threads = [threading.Thread(target=increment_safe, args=(iterations,)) 
               for _ in range(4)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    print(f"  Safe result:   {counter} (expected {iterations * 4})")


# =============================================================================
# SEMAPHORE
# =============================================================================

def semaphore_example():
    """
    Equivalent to sem_t in C.
    
    C equivalent:
        sem_t sem;
        sem_init(&sem, 0, 3);  // Initial value 3
        sem_wait(&sem);        // Decrement (block if 0)
        sem_post(&sem);        // Increment
    """
    print("\n" + "="*60)
    print("SEMAPHORE EXAMPLE")
    print("="*60 + "\n")
    
    # Limit concurrent access to 3
    semaphore = threading.Semaphore(3)
    
    def limited_resource(name: str) -> None:
        print(f"  {name} waiting for resource...")
        with semaphore:
            print(f"  {name} acquired resource")
            time.sleep(1)
            print(f"  {name} releasing resource")
    
    threads = [
        threading.Thread(target=limited_resource, args=(f"Thread-{i}",))
        for i in range(6)
    ]
    
    for t in threads:
        t.start()
    for t in threads:
        t.join()


# =============================================================================
# CONDITION VARIABLE
# =============================================================================

def condition_variable_example():
    """
    Equivalent to pthread_cond_t in C.
    
    C equivalent:
        pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
        
        // Wait
        pthread_mutex_lock(&mutex);
        while (!condition) {
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
        
        // Signal
        pthread_mutex_lock(&mutex);
        condition = true;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    """
    print("\n" + "="*60)
    print("CONDITION VARIABLE EXAMPLE")
    print("="*60 + "\n")
    
    condition = threading.Condition()
    data_ready = False
    data = None
    
    def producer():
        nonlocal data_ready, data
        print("  Producer: preparing data...")
        time.sleep(2)
        
        with condition:
            data = "Hello from producer!"
            data_ready = True
            print("  Producer: data ready, notifying consumer")
            condition.notify()
    
    def consumer():
        nonlocal data_ready, data
        print("  Consumer: waiting for data...")
        
        with condition:
            while not data_ready:
                condition.wait()
            print(f"  Consumer: received '{data}'")
    
    consumer_thread = threading.Thread(target=consumer)
    producer_thread = threading.Thread(target=producer)
    
    consumer_thread.start()
    time.sleep(0.1)  # Ensure consumer starts waiting first
    producer_thread.start()
    
    consumer_thread.join()
    producer_thread.join()


# =============================================================================
# PRODUCER-CONSUMER WITH QUEUE
# =============================================================================

def producer_consumer_example():
    """
    Bounded buffer producer-consumer using thread-safe Queue.
    
    Python's queue.Queue is thread-safe and handles all
    synchronisation internally, unlike C where we must
    implement it manually with mutex and condition variables.
    """
    print("\n" + "="*60)
    print("PRODUCER-CONSUMER PATTERN")
    print("="*60 + "\n")
    
    buffer: queue.Queue = queue.Queue(maxsize=5)
    num_items = 10
    
    def producer(name: str):
        for i in range(num_items):
            item = f"Item-{i}"
            buffer.put(item)  # Blocks if full
            print(f"  {name} produced: {item}")
            time.sleep(random.uniform(0.1, 0.3))
        buffer.put(None)  # Poison pill
    
    def consumer(name: str):
        while True:
            item = buffer.get()  # Blocks if empty
            if item is None:
                print(f"  {name} received poison pill, exiting")
                break
            print(f"  {name} consumed: {item}")
            time.sleep(random.uniform(0.2, 0.4))
    
    producer_thread = threading.Thread(target=producer, args=("Producer",))
    consumer_thread = threading.Thread(target=consumer, args=("Consumer",))
    
    producer_thread.start()
    consumer_thread.start()
    
    producer_thread.join()
    consumer_thread.join()


# =============================================================================
# THREAD POOL EXECUTOR
# =============================================================================

def thread_pool_example():
    """
    High-level thread pool with futures.
    
    This is similar to our C homework implementation but with
    a much simpler API provided by concurrent.futures.
    """
    print("\n" + "="*60)
    print("THREAD POOL EXECUTOR")
    print("="*60 + "\n")
    
    def compute_factorial(n: int) -> int:
        result = 1
        for i in range(2, n + 1):
            result *= i
        time.sleep(0.5)  # Simulate work
        return result
    
    numbers = [5, 10, 15, 20, 25]
    
    with ThreadPoolExecutor(max_workers=4) as executor:
        # Submit all tasks
        future_to_num = {
            executor.submit(compute_factorial, n): n 
            for n in numbers
        }
        
        # Collect results as they complete
        for future in as_completed(future_to_num):
            n = future_to_num[future]
            try:
                result = future.result()
                print(f"  factorial({n}) = {result}")
            except Exception as exc:
                print(f"  factorial({n}) raised {exc}")


# =============================================================================
# READERS-WRITERS LOCK
# =============================================================================

def readers_writers_example():
    """
    Multiple readers, single writer pattern.
    
    Python provides RLock (reentrant lock) but not a built-in
    readers-writers lock. We can implement one or use threading.RLock.
    """
    print("\n" + "="*60)
    print("READERS-WRITERS PATTERN")
    print("="*60 + "\n")
    
    class ReadersWritersLock:
        """Simple readers-writers lock implementation."""
        
        def __init__(self):
            self.readers = 0
            self.readers_lock = threading.Lock()
            self.writers_lock = threading.Lock()
        
        def acquire_read(self):
            with self.readers_lock:
                self.readers += 1
                if self.readers == 1:
                    self.writers_lock.acquire()
        
        def release_read(self):
            with self.readers_lock:
                self.readers -= 1
                if self.readers == 0:
                    self.writers_lock.release()
        
        def acquire_write(self):
            self.writers_lock.acquire()
        
        def release_write(self):
            self.writers_lock.release()
    
    data = {"value": 0}
    rw_lock = ReadersWritersLock()
    
    def reader(name: str):
        rw_lock.acquire_read()
        print(f"  {name} reading: {data['value']}")
        time.sleep(0.2)
        rw_lock.release_read()
    
    def writer(name: str, value: int):
        rw_lock.acquire_write()
        print(f"  {name} writing: {value}")
        data["value"] = value
        time.sleep(0.3)
        rw_lock.release_write()
    
    threads = [
        threading.Thread(target=reader, args=(f"Reader-{i}",))
        for i in range(3)
    ] + [
        threading.Thread(target=writer, args=(f"Writer-{i}", i * 10))
        for i in range(2)
    ]
    
    random.shuffle(threads)
    for t in threads:
        t.start()
    for t in threads:
        t.join()


# =============================================================================
# BARRIER
# =============================================================================

def barrier_example():
    """
    Synchronisation barrier - all threads wait until all arrive.
    
    C equivalent:
        pthread_barrier_t barrier;
        pthread_barrier_init(&barrier, NULL, NUM_THREADS);
        pthread_barrier_wait(&barrier);
    """
    print("\n" + "="*60)
    print("BARRIER SYNCHRONISATION")
    print("="*60 + "\n")
    
    num_threads = 4
    barrier = threading.Barrier(num_threads)
    
    def worker(name: str, work_time: float):
        print(f"  {name} doing phase 1 work...")
        time.sleep(work_time)
        print(f"  {name} waiting at barrier")
        
        barrier.wait()  # Wait for all threads
        
        print(f"  {name} continuing to phase 2")
        time.sleep(0.2)
        print(f"  {name} completed")
    
    threads = [
        threading.Thread(
            target=worker,
            args=(f"Thread-{i}", random.uniform(0.5, 2.0))
        )
        for i in range(num_threads)
    ]
    
    for t in threads:
        t.start()
    for t in threads:
        t.join()


# =============================================================================
# EVENT
# =============================================================================

def event_example():
    """
    Threading Event for signalling between threads.
    
    Events are simpler than condition variables when you just
    need to signal that something happened.
    """
    print("\n" + "="*60)
    print("EVENT SIGNALLING")
    print("="*60 + "\n")
    
    startup_complete = threading.Event()
    
    def server():
        print("  Server: initialising...")
        time.sleep(2)
        print("  Server: ready!")
        startup_complete.set()
        print("  Server: running...")
    
    def client(name: str):
        print(f"  {name}: waiting for server...")
        startup_complete.wait()
        print(f"  {name}: server is ready, proceeding")
    
    threads = [
        threading.Thread(target=server),
        threading.Thread(target=client, args=("Client-1",)),
        threading.Thread(target=client, args=("Client-2",)),
    ]
    
    for t in threads:
        t.start()
    for t in threads:
        t.join()


# =============================================================================
# THREAD-LOCAL STORAGE
# =============================================================================

def thread_local_example():
    """
    Thread-local storage for thread-specific data.
    
    C equivalent:
        pthread_key_t key;
        pthread_key_create(&key, NULL);
        pthread_setspecific(key, value);
        pthread_getspecific(key);
    """
    print("\n" + "="*60)
    print("THREAD-LOCAL STORAGE")
    print("="*60 + "\n")
    
    local_data = threading.local()
    
    def worker(name: str, value: int):
        # Each thread has its own copy
        local_data.name = name
        local_data.value = value
        
        time.sleep(random.uniform(0.1, 0.3))
        
        print(f"  Thread {local_data.name} has value {local_data.value}")
    
    threads = [
        threading.Thread(target=worker, args=(f"Thread-{i}", i * 100))
        for i in range(4)
    ]
    
    for t in threads:
        t.start()
    for t in threads:
        t.join()


# =============================================================================
# COMPARISON TABLE
# =============================================================================

def print_comparison_table():
    """Print comparison between C pthreads and Python threading."""
    print("\n" + "="*60)
    print("C PTHREADS vs PYTHON THREADING COMPARISON")
    print("="*60 + "\n")
    
    table = """
┌─────────────────────────┬─────────────────────────┬─────────────────────────┐
│ Concept                 │ C (pthreads)            │ Python (threading)      │
├─────────────────────────┼─────────────────────────┼─────────────────────────┤
│ Thread creation         │ pthread_create()        │ threading.Thread()      │
│ Thread join             │ pthread_join()          │ thread.join()           │
│ Mutex                   │ pthread_mutex_t         │ threading.Lock()        │
│ Semaphore               │ sem_t                   │ threading.Semaphore()   │
│ Condition variable      │ pthread_cond_t          │ threading.Condition()   │
│ Barrier                 │ pthread_barrier_t       │ threading.Barrier()     │
│ Thread-local storage    │ pthread_key_t           │ threading.local()       │
│ True parallelism        │ Yes                     │ No (GIL)                │
│ Memory management       │ Manual                  │ Automatic (GC)          │
│ Error handling          │ Return codes            │ Exceptions              │
│ Thread-safe queue       │ Manual implementation   │ queue.Queue()           │
│ Thread pool             │ Manual implementation   │ ThreadPoolExecutor      │
└─────────────────────────┴─────────────────────────┴─────────────────────────┘
"""
    print(table)


# =============================================================================
# MAIN
# =============================================================================

def main():
    """Run all demonstrations."""
    print("\n" + "="*60)
    print("PYTHON THREADING COMPARISON")
    print("Week 20: Parallel and Concurrent Programming")
    print("="*60)
    
    print_comparison_table()
    basic_thread_example()
    mutex_example()
    semaphore_example()
    condition_variable_example()
    producer_consumer_example()
    thread_pool_example()
    readers_writers_example()
    barrier_example()
    event_example()
    thread_local_example()
    
    print("\n" + "="*60)
    print("ALL DEMONSTRATIONS COMPLETE")
    print("="*60 + "\n")


if __name__ == "__main__":
    main()

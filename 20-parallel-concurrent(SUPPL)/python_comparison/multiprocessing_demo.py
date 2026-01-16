#!/usr/bin/env python3
"""
=============================================================================
PYTHON COMPARISON: Multiprocessing Module
Week 20: Parallel and Concurrent Programming
=============================================================================

This file demonstrates Python's multiprocessing module for comparison with
C's fork() and pthread implementations.

Key Differences from threading:
  - True parallelism (bypasses the GIL)
  - Separate memory spaces for each process
  - Higher overhead for process creation
  - Inter-process communication via Queue, Pipe, shared memory

When to use multiprocessing:
  - CPU-bound tasks (mathematical computations)
  - Tasks that can be parallelised with minimal communication
  - When you need true parallel execution

When NOT to use multiprocessing:
  - I/O-bound tasks (use threading or asyncio)
  - Tasks requiring frequent shared state updates
  - When process creation overhead is significant

NOTE: This file is for educational comparison only, not for execution
      in the course exercises.

=============================================================================
"""

import multiprocessing as mp
from multiprocessing import Pool, Process, Queue, Pipe, Value, Array, Manager
from multiprocessing import Lock, Semaphore, Barrier, Event
from concurrent.futures import ProcessPoolExecutor, as_completed
import time
import random
import os
from typing import List, Tuple, Any

# =============================================================================
# BASIC PROCESS CREATION
# =============================================================================

def worker_function(name: str, duration: float) -> None:
    """Worker function for basic process demonstration."""
    pid = os.getpid()
    print(f"  Process {name} (PID: {pid}) starting...")
    time.sleep(duration)
    print(f"  Process {name} (PID: {pid}) completed")


def basic_process_example():
    """
    Equivalent to fork() in C, but safer and more portable.
    
    C equivalent:
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            worker_function();
            exit(0);
        } else {
            // Parent process
            waitpid(pid, NULL, 0);
        }
    """
    print("\n" + "="*60)
    print("BASIC PROCESS CREATION")
    print("="*60 + "\n")
    
    print(f"Main process PID: {os.getpid()}")
    
    processes = []
    for i in range(4):
        p = Process(
            target=worker_function,
            args=(f"Worker-{i}", random.uniform(0.5, 1.5))
        )
        processes.append(p)
        p.start()
    
    # Wait for all processes
    for p in processes:
        p.join()
    
    print("\nAll processes completed")


# =============================================================================
# PROCESS POOL
# =============================================================================

def compute_intensive_task(n: int) -> Tuple[int, int]:
    """CPU-intensive task that benefits from multiprocessing."""
    # Compute sum of squares (simulating heavy computation)
    result = sum(i * i for i in range(n))
    return n, result


def process_pool_example():
    """
    Process pool for CPU-bound tasks.
    
    This achieves true parallelism unlike Python threading.
    Equivalent to a thread pool in C, but with processes.
    """
    print("\n" + "="*60)
    print("PROCESS POOL EXAMPLE")
    print("="*60 + "\n")
    
    numbers = [10000000, 20000000, 15000000, 25000000]
    
    print("Using Pool.map() for parallel computation...")
    start = time.time()
    
    with Pool(processes=4) as pool:
        results = pool.map(compute_intensive_task, numbers)
    
    elapsed = time.time() - start
    
    for n, result in results:
        print(f"  sum of squares up to {n}: {result}")
    
    print(f"\nParallel time: {elapsed:.3f} seconds")
    
    # Compare with sequential
    print("\nSequential comparison...")
    start = time.time()
    results = [compute_intensive_task(n) for n in numbers]
    elapsed = time.time() - start
    print(f"Sequential time: {elapsed:.3f} seconds")


# =============================================================================
# INTER-PROCESS COMMUNICATION: QUEUE
# =============================================================================

def producer_with_queue(queue: Queue, num_items: int):
    """Producer process that puts items in a queue."""
    pid = os.getpid()
    for i in range(num_items):
        item = f"Item-{i} from PID {pid}"
        queue.put(item)
        print(f"  Producer ({pid}): sent {item}")
        time.sleep(random.uniform(0.1, 0.3))
    queue.put(None)  # Poison pill


def consumer_with_queue(queue: Queue):
    """Consumer process that gets items from a queue."""
    pid = os.getpid()
    while True:
        item = queue.get()
        if item is None:
            print(f"  Consumer ({pid}): received shutdown signal")
            break
        print(f"  Consumer ({pid}): received {item}")
        time.sleep(random.uniform(0.1, 0.2))


def queue_example():
    """
    Inter-process communication using Queue.
    
    Similar to message passing between processes.
    The Queue handles synchronisation automatically.
    """
    print("\n" + "="*60)
    print("INTER-PROCESS QUEUE COMMUNICATION")
    print("="*60 + "\n")
    
    queue = Queue()
    
    producer = Process(target=producer_with_queue, args=(queue, 5))
    consumer = Process(target=consumer_with_queue, args=(queue,))
    
    producer.start()
    consumer.start()
    
    producer.join()
    consumer.join()


# =============================================================================
# INTER-PROCESS COMMUNICATION: PIPE
# =============================================================================

def sender_with_pipe(conn):
    """Process that sends data through a pipe."""
    messages = ["Hello", "World", "From", "Pipe", "END"]
    for msg in messages:
        conn.send(msg)
        print(f"  Sender: sent '{msg}'")
        time.sleep(0.2)
    conn.close()


def receiver_with_pipe(conn):
    """Process that receives data from a pipe."""
    while True:
        msg = conn.recv()
        print(f"  Receiver: got '{msg}'")
        if msg == "END":
            break
    conn.close()


def pipe_example():
    """
    Bidirectional pipe communication between processes.
    
    C equivalent:
        int pipefd[2];
        pipe(pipefd);
        // pipefd[0] for reading, pipefd[1] for writing
    """
    print("\n" + "="*60)
    print("INTER-PROCESS PIPE COMMUNICATION")
    print("="*60 + "\n")
    
    parent_conn, child_conn = Pipe()
    
    sender = Process(target=sender_with_pipe, args=(parent_conn,))
    receiver = Process(target=receiver_with_pipe, args=(child_conn,))
    
    sender.start()
    receiver.start()
    
    sender.join()
    receiver.join()


# =============================================================================
# SHARED MEMORY
# =============================================================================

def increment_shared_value(shared_val, lock, iterations: int):
    """Increment a shared value safely using a lock."""
    for _ in range(iterations):
        with lock:
            shared_val.value += 1


def shared_memory_example():
    """
    Shared memory between processes.
    
    C equivalent:
        // Using mmap or shmget/shmat
        int *shared = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                           MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        
    Python's Value and Array provide typed shared memory with
    synchronisation built in.
    """
    print("\n" + "="*60)
    print("SHARED MEMORY EXAMPLE")
    print("="*60 + "\n")
    
    # Shared value with type 'i' (int)
    shared_counter = Value('i', 0)
    lock = Lock()
    iterations = 10000
    
    processes = []
    for _ in range(4):
        p = Process(
            target=increment_shared_value,
            args=(shared_counter, lock, iterations)
        )
        processes.append(p)
        p.start()
    
    for p in processes:
        p.join()
    
    print(f"  Final counter value: {shared_counter.value}")
    print(f"  Expected value: {iterations * 4}")
    
    # Shared array example
    print("\n  Shared array example:")
    shared_array = Array('d', [0.0, 0.0, 0.0, 0.0])  # 'd' = double
    
    def modify_array(arr, idx):
        arr[idx] = os.getpid()
    
    processes = []
    for i in range(4):
        p = Process(target=modify_array, args=(shared_array, i))
        processes.append(p)
        p.start()
    
    for p in processes:
        p.join()
    
    print(f"  Array values (PIDs): {list(shared_array)}")


# =============================================================================
# MANAGER FOR COMPLEX SHARED STATE
# =============================================================================

def update_dict(shared_dict, key: str, value: int):
    """Update a shared dictionary."""
    shared_dict[key] = value
    print(f"  Process {os.getpid()}: set {key} = {value}")


def manager_example():
    """
    Manager for sharing complex objects between processes.
    
    Managers provide proxy objects that can be shared:
    - dict, list, Namespace
    - Queue, Event, Lock, etc.
    
    Higher overhead but more flexibility than Value/Array.
    """
    print("\n" + "="*60)
    print("MANAGER FOR COMPLEX SHARED STATE")
    print("="*60 + "\n")
    
    with Manager() as manager:
        shared_dict = manager.dict()
        shared_list = manager.list()
        
        # Multiple processes updating shared dict
        processes = []
        for i in range(4):
            p = Process(
                target=update_dict,
                args=(shared_dict, f"key_{i}", i * 100)
            )
            processes.append(p)
            p.start()
        
        for p in processes:
            p.join()
        
        print(f"\n  Final dictionary: {dict(shared_dict)}")


# =============================================================================
# PROCESS POOL EXECUTOR (CONCURRENT.FUTURES)
# =============================================================================

def heavy_computation(n: int) -> int:
    """Simulate heavy computation."""
    result = 0
    for i in range(n):
        result += i * i
    time.sleep(0.5)
    return result


def process_pool_executor_example():
    """
    High-level process pool with futures.
    
    Similar to ThreadPoolExecutor but with true parallelism.
    This is the recommended approach for CPU-bound tasks.
    """
    print("\n" + "="*60)
    print("PROCESS POOL EXECUTOR WITH FUTURES")
    print("="*60 + "\n")
    
    numbers = [1000000, 2000000, 1500000, 2500000, 3000000]
    
    with ProcessPoolExecutor(max_workers=4) as executor:
        # Submit all tasks
        future_to_num = {
            executor.submit(heavy_computation, n): n
            for n in numbers
        }
        
        # Collect results as they complete
        for future in as_completed(future_to_num):
            n = future_to_num[future]
            try:
                result = future.result()
                print(f"  computation({n}) = {result}")
            except Exception as exc:
                print(f"  computation({n}) raised {exc}")


# =============================================================================
# MAP-REDUCE PATTERN
# =============================================================================

def map_function(chunk: str) -> dict:
    """Map function: count words in a chunk."""
    word_counts = {}
    for word in chunk.lower().split():
        word = ''.join(c for c in word if c.isalpha())
        if word:
            word_counts[word] = word_counts.get(word, 0) + 1
    return word_counts


def reduce_function(counts: List[dict]) -> dict:
    """Reduce function: merge word counts."""
    result = {}
    for count_dict in counts:
        for word, count in count_dict.items():
            result[word] = result.get(word, 0) + count
    return result


def mapreduce_example():
    """
    Simple Map-Reduce implementation using multiprocessing.
    
    This demonstrates how parallel processing can be used
    for data processing tasks like word count.
    """
    print("\n" + "="*60)
    print("MAP-REDUCE PATTERN")
    print("="*60 + "\n")
    
    text = """
    The quick brown fox jumps over the lazy dog.
    A journey of a thousand miles begins with a single step.
    To be or not to be, that is the question.
    All that glitters is not gold.
    The early bird catches the worm.
    """ * 100
    
    # Split into chunks
    words = text.split()
    chunk_size = len(words) // 4
    chunks = [
        ' '.join(words[i:i + chunk_size])
        for i in range(0, len(words), chunk_size)
    ]
    
    print(f"  Processing {len(words)} words in {len(chunks)} chunks")
    
    # Map phase (parallel)
    print("\n  Map phase...")
    with Pool(processes=4) as pool:
        mapped_results = pool.map(map_function, chunks)
    
    # Reduce phase
    print("  Reduce phase...")
    final_counts = reduce_function(mapped_results)
    
    # Display top 10
    print("\n  Top 10 words:")
    sorted_counts = sorted(
        final_counts.items(),
        key=lambda x: x[1],
        reverse=True
    )[:10]
    
    for word, count in sorted_counts:
        print(f"    {word}: {count}")


# =============================================================================
# SYNCHRONISATION PRIMITIVES
# =============================================================================

def sync_primitives_example():
    """
    Demonstrate multiprocessing synchronisation primitives.
    
    These work similarly to threading primitives but across processes.
    """
    print("\n" + "="*60)
    print("MULTIPROCESSING SYNCHRONISATION PRIMITIVES")
    print("="*60 + "\n")
    
    # Lock
    print("  1. Lock example:")
    lock = Lock()
    
    def with_lock(name: str, lock):
        with lock:
            print(f"     {name} has the lock")
            time.sleep(0.3)
            print(f"     {name} releasing lock")
    
    procs = [
        Process(target=with_lock, args=(f"P{i}", lock))
        for i in range(3)
    ]
    for p in procs:
        p.start()
    for p in procs:
        p.join()
    
    # Semaphore
    print("\n  2. Semaphore example:")
    sem = Semaphore(2)
    
    def with_semaphore(name: str, sem):
        sem.acquire()
        print(f"     {name} acquired semaphore")
        time.sleep(0.3)
        print(f"     {name} releasing semaphore")
        sem.release()
    
    procs = [
        Process(target=with_semaphore, args=(f"P{i}", sem))
        for i in range(4)
    ]
    for p in procs:
        p.start()
    for p in procs:
        p.join()
    
    # Event
    print("\n  3. Event example:")
    event = Event()
    
    def wait_for_event(name: str, event):
        print(f"     {name} waiting for event...")
        event.wait()
        print(f"     {name} event received!")
    
    procs = [
        Process(target=wait_for_event, args=(f"P{i}", event))
        for i in range(3)
    ]
    for p in procs:
        p.start()
    
    time.sleep(1)
    print("     Main: setting event")
    event.set()
    
    for p in procs:
        p.join()


# =============================================================================
# COMPARISON TABLE
# =============================================================================

def print_comparison_table():
    """Print comparison between threading and multiprocessing."""
    print("\n" + "="*60)
    print("THREADING vs MULTIPROCESSING COMPARISON")
    print("="*60 + "\n")
    
    table = """
┌─────────────────────────┬─────────────────────────┬─────────────────────────┐
│ Aspect                  │ threading               │ multiprocessing         │
├─────────────────────────┼─────────────────────────┼─────────────────────────┤
│ Parallelism             │ No (GIL limited)        │ Yes (separate PIDs)     │
│ Memory                  │ Shared                  │ Separate (copy-on-write)│
│ Creation overhead       │ Low                     │ High                    │
│ Communication           │ Direct (shared vars)    │ Queue, Pipe, Shared Mem │
│ Best for                │ I/O-bound tasks         │ CPU-bound tasks         │
│ Debugging               │ Easier                  │ Harder                  │
│ Platform independence   │ Good                    │ Varies (fork vs spawn)  │
│ Crash isolation         │ No (shared process)     │ Yes (separate process)  │
└─────────────────────────┴─────────────────────────┴─────────────────────────┘

When to use which:

  threading:
    - Network I/O (web requests, socket programming)
    - File I/O
    - GUI applications
    - When you need shared state with low overhead

  multiprocessing:
    - CPU-intensive calculations
    - Image/video processing
    - Scientific computing
    - When you need true parallelism
    - When crash isolation is important
"""
    print(table)


# =============================================================================
# MAIN
# =============================================================================

def main():
    """Run all demonstrations."""
    print("\n" + "="*60)
    print("PYTHON MULTIPROCESSING COMPARISON")
    print("Week 20: Parallel and Concurrent Programming")
    print("="*60)
    
    print_comparison_table()
    basic_process_example()
    process_pool_example()
    queue_example()
    pipe_example()
    shared_memory_example()
    manager_example()
    process_pool_executor_example()
    mapreduce_example()
    sync_primitives_example()
    
    print("\n" + "="*60)
    print("ALL DEMONSTRATIONS COMPLETE")
    print("="*60 + "\n")


if __name__ == "__main__":
    main()

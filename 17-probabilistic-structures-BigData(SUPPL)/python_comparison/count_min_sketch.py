#!/usr/bin/env python3
"""
Count-Min Sketch Implementation in Python
Week 17: Probabilistic Data Structures for Big Data
ATP Course - ASE-CSIE Bucharest

This file provides a Python perspective on the Count-Min Sketch for comparison
with the C implementation. It is NOT intended for execution within the course
exercises but serves as a reference for understanding the algorithm in a
higher-level language.

The Count-Min Sketch (Cormode & Muthukrishnan, 2005) is a probabilistic data
structure for estimating frequencies of elements in a data stream using
sublinear space.

Key Properties:
- Space: O(ε⁻¹ × log(δ⁻¹))
- Update time: O(log(δ⁻¹))
- Query time: O(log(δ⁻¹))
- Error guarantee: With probability 1-δ, estimate ≤ true count + ε×N
  where N is the total count of all elements
"""

import math
import hashlib
from typing import List, Dict, Optional, Tuple
from collections import Counter


class CountMinSketch:
    """
    A probabilistic data structure for frequency estimation in data streams.
    
    The Count-Min Sketch uses a 2D array of counters with dimensions
    width (w) × depth (d). Each row uses a different hash function.
    
    For a query, the minimum value across all rows is returned, providing
    an upper-bound estimate of the true frequency.
    
    Attributes:
        width (int): Number of columns (w)
        depth (int): Number of rows/hash functions (d)
        table (List[List[int]]): The 2D counter array
        total_count (int): Total number of updates
    """
    
    def __init__(self, width: int, depth: int):
        """
        Initialise a Count-Min Sketch with specified dimensions.
        
        Args:
            width: Number of columns (w), determines accuracy (ε ≈ e/w)
            depth: Number of rows (d), determines confidence (δ ≈ e^(-d))
        """
        self.width = width
        self.depth = depth
        self.table = [[0] * width for _ in range(depth)]
        self.total_count = 0
    
    @classmethod
    def create_from_error_bounds(cls, epsilon: float, delta: float) -> 'CountMinSketch':
        """
        Create a Count-Min Sketch with specified error guarantees.
        
        With probability 1-δ, the estimate for any item is at most
        true_count + ε × total_count.
        
        The optimal dimensions are:
        - width = ⌈e/ε⌉
        - depth = ⌈ln(1/δ)⌉
        
        Args:
            epsilon: Accuracy parameter (ε)
            delta: Confidence parameter (δ)
            
        Returns:
            CountMinSketch with appropriate dimensions
            
        Raises:
            ValueError: If parameters are invalid
        """
        if not (0 < epsilon < 1):
            raise ValueError("Epsilon must be between 0 and 1")
        if not (0 < delta < 1):
            raise ValueError("Delta must be between 0 and 1")
        
        width = math.ceil(math.e / epsilon)
        depth = math.ceil(math.log(1 / delta))
        
        return cls(width, depth)
    
    def _hash(self, item: str, row: int) -> int:
        """
        Generate a hash value for an item in a specific row.
        
        Uses SHA-256 with row-specific salt for pseudo-independence.
        
        Args:
            item: The item to hash
            row: The row index (determines which hash function)
            
        Returns:
            Hash value in range [0, width)
        """
        # Combine item with row-specific salt
        salted = f"{row}:{item}".encode('utf-8')
        hash_bytes = hashlib.sha256(salted).digest()
        
        # Convert first 8 bytes to integer and map to width
        hash_int = int.from_bytes(hash_bytes[:8], 'big')
        return hash_int % self.width
    
    def update(self, item: str, count: int = 1) -> None:
        """
        Add a count for an item (increment all corresponding counters).
        
        Time complexity: O(depth)
        
        Args:
            item: The item to update
            count: The count to add (default 1)
        """
        for row in range(self.depth):
            col = self._hash(item, row)
            self.table[row][col] += count
        self.total_count += count
    
    def query(self, item: str) -> int:
        """
        Estimate the frequency of an item.
        
        Returns the minimum value across all rows for this item,
        which provides an upper-bound estimate of the true frequency.
        
        Time complexity: O(depth)
        
        Args:
            item: The item to query
            
        Returns:
            Estimated frequency (upper bound)
        """
        min_count = float('inf')
        for row in range(self.depth):
            col = self._hash(item, row)
            min_count = min(min_count, self.table[row][col])
        return int(min_count)
    
    def memory_usage_bytes(self) -> int:
        """
        Return approximate memory usage in bytes.
        
        Assumes 4 bytes per counter (32-bit integers).
        """
        return self.width * self.depth * 4
    
    def merge(self, other: 'CountMinSketch') -> 'CountMinSketch':
        """
        Merge two Count-Min Sketches (for distributed counting).
        
        Both sketches must have the same dimensions.
        
        Args:
            other: Another Count-Min Sketch to merge
            
        Returns:
            New merged Count-Min Sketch
            
        Raises:
            ValueError: If dimensions don't match
        """
        if self.width != other.width or self.depth != other.depth:
            raise ValueError("Sketches must have the same dimensions")
        
        merged = CountMinSketch(self.width, self.depth)
        for row in range(self.depth):
            for col in range(self.width):
                merged.table[row][col] = self.table[row][col] + other.table[row][col]
        merged.total_count = self.total_count + other.total_count
        
        return merged
    
    def find_heavy_hitters(self, threshold_fraction: float) -> List[Tuple[str, int]]:
        """
        Find items with frequency above a threshold.
        
        Note: This is a basic implementation that requires maintaining
        a separate list of seen items. More sophisticated algorithms
        (like finding heavy hitters directly) exist.
        
        This method is provided for demonstration only.
        """
        raise NotImplementedError(
            "Heavy hitter detection requires additional tracking. "
            "See the C implementation for a complete solution."
        )


class ExactCounter:
    """
    Exact frequency counter for comparison with Count-Min Sketch.
    
    Uses a hash map to store exact counts. Demonstrates the space
    overhead that probabilistic structures avoid.
    """
    
    def __init__(self):
        self.counts: Counter = Counter()
    
    def update(self, item: str, count: int = 1) -> None:
        """Add a count for an item."""
        self.counts[item] += count
    
    def query(self, item: str) -> int:
        """Get the exact count for an item."""
        return self.counts[item]
    
    def memory_usage_estimate(self) -> int:
        """
        Estimate memory usage (very approximate).
        
        Assumes average key length of 10 characters + 8 bytes per count.
        """
        return len(self.counts) * (10 + 8)


def demo():
    """Demonstrate Count-Min Sketch usage."""
    print("=" * 60)
    print("     COUNT-MIN SKETCH DEMONSTRATION (Python)")
    print("=" * 60)
    print()
    
    # Create CMS with specific error bounds
    # ε = 0.001, δ = 0.01 (99% confidence, 0.1% error)
    epsilon = 0.001
    delta = 0.01
    cms = CountMinSketch.create_from_error_bounds(epsilon, delta)
    
    print(f"Count-Min Sketch Configuration:")
    print(f"  ε (epsilon): {epsilon}")
    print(f"  δ (delta): {delta}")
    print(f"  Width: {cms.width}")
    print(f"  Depth: {cms.depth}")
    print(f"  Memory: {cms.memory_usage_bytes()} bytes")
    print()
    
    # Simulate a data stream with known frequencies
    stream_data = {
        "apple": 1000,
        "banana": 500,
        "cherry": 100,
        "date": 50,
        "elderberry": 10,
    }
    
    # Also track with exact counter for comparison
    exact = ExactCounter()
    
    print("Processing data stream...")
    for item, count in stream_data.items():
        for _ in range(count):
            cms.update(item)
            exact.update(item)
    
    print(f"Total items processed: {cms.total_count}")
    print()
    
    # Compare estimates
    print("Frequency Estimates:")
    print("-" * 55)
    print(f"{'Item':<15} {'CMS Estimate':>12} {'Exact':>12} {'Error':>12}")
    print("-" * 55)
    
    for item in stream_data:
        cms_est = cms.query(item)
        exact_count = exact.query(item)
        error = cms_est - exact_count
        error_pct = (error / exact_count * 100) if exact_count > 0 else 0
        print(f"{item:<15} {cms_est:>12} {exact_count:>12} {error_pct:>10.2f}%")
    
    print("-" * 55)
    print()
    
    # Query for items not in the stream
    print("Query for items NOT in stream (false positives possible):")
    test_items = ["grape", "melon", "kiwi"]
    for item in test_items:
        est = cms.query(item)
        print(f"  '{item}': {est} (should be 0)")
    print()
    
    # Memory comparison
    print("Memory Comparison:")
    print(f"  Count-Min Sketch: {cms.memory_usage_bytes()} bytes")
    print(f"  Exact Counter: ~{exact.memory_usage_estimate()} bytes")
    print()
    
    # Demonstrate merging
    print("-" * 60)
    print("     DISTRIBUTED MERGING DEMONSTRATION")
    print("-" * 60)
    print()
    
    # Create two separate sketches
    cms1 = CountMinSketch(cms.width, cms.depth)
    cms2 = CountMinSketch(cms.width, cms.depth)
    
    # Simulate data split across two nodes
    for i, (item, count) in enumerate(stream_data.items()):
        target = cms1 if i % 2 == 0 else cms2
        for _ in range(count):
            target.update(item)
    
    # Merge
    merged = cms1.merge(cms2)
    
    print("Merged sketch query results:")
    for item in stream_data:
        print(f"  '{item}': {merged.query(item)} (expected: {stream_data[item]})")
    print()
    
    print("=" * 60)
    print("       DEMONSTRATION COMPLETE")
    print("=" * 60)


if __name__ == "__main__":
    demo()

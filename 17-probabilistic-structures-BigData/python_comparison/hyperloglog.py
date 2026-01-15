#!/usr/bin/env python3
"""
HyperLogLog Implementation in Python
Week 17: Probabilistic Data Structures for Big Data
ATP Course - ASE-CSIE Bucharest

This file provides a Python perspective on HyperLogLog for comparison
with the C implementation. It is NOT intended for execution within the
course exercises but serves as a reference for understanding the algorithm
in a higher-level language.

HyperLogLog (Flajolet et al., 2007) is a probabilistic data structure for
cardinality estimation (counting distinct elements) with remarkably low
memory usage: approximately 1.5 KB for counting billions of elements with
~2% relative error.

Key Properties:
- Space: O(log log n) bits for counting up to n elements
- Update time: O(1)
- Query time: O(m) where m is the number of registers
- Standard error: approximately 1.04/√m

The algorithm is based on the observation that the cardinality of a
multiset can be estimated by the maximum number of leading zeros in the
hash values of its elements.
"""

import math
import hashlib
from typing import Optional


class HyperLogLog:
    """
    A probabilistic data structure for cardinality estimation.
    
    HyperLogLog uses a set of registers to track the maximum number of
    leading zeros seen in hash values. The cardinality is estimated using
    the harmonic mean of the register values.
    
    Attributes:
        precision (int): The precision parameter (p), determines m = 2^p registers
        num_registers (int): Number of registers (m)
        registers (list): Array of register values
        alpha (float): Bias correction constant
    """
    
    # Alpha constants for bias correction
    ALPHA_MAP = {
        4: 0.673,
        5: 0.697,
        6: 0.709,
    }
    
    def __init__(self, precision: int = 14):
        """
        Initialise a HyperLogLog with specified precision.
        
        Args:
            precision: Number of bits for register addressing (p).
                      Determines the number of registers m = 2^p.
                      Typical values: 10-16. Higher = more accuracy, more memory.
                      
        Raises:
            ValueError: If precision is outside valid range [4, 18]
        """
        if not (4 <= precision <= 18):
            raise ValueError("Precision must be between 4 and 18")
        
        self.precision = precision
        self.num_registers = 1 << precision  # m = 2^p
        self.registers = [0] * self.num_registers
        
        # Calculate alpha (bias correction factor)
        if precision in self.ALPHA_MAP:
            self.alpha = self.ALPHA_MAP[precision]
        else:
            # General formula for p >= 7
            self.alpha = 0.7213 / (1 + 1.079 / self.num_registers)
    
    def _hash(self, item: str) -> int:
        """
        Generate a 64-bit hash value for an item.
        
        Args:
            item: The item to hash
            
        Returns:
            64-bit hash value
        """
        data = item.encode('utf-8')
        # Use SHA-256 and take first 8 bytes for 64-bit hash
        hash_bytes = hashlib.sha256(data).digest()[:8]
        return int.from_bytes(hash_bytes, 'big')
    
    def _count_leading_zeros(self, value: int, max_bits: int = 64) -> int:
        """
        Count leading zeros in the binary representation of a value.
        
        Args:
            value: The value to analyse
            max_bits: Maximum number of bits to consider
            
        Returns:
            Number of leading zeros (including the first 1)
        """
        if value == 0:
            return max_bits + 1
        
        # Count leading zeros and add 1 for the first '1' bit
        zeros = 0
        for i in range(max_bits - 1, -1, -1):
            if value & (1 << i):
                return zeros + 1
            zeros += 1
        return zeros + 1
    
    def add(self, item: str) -> None:
        """
        Add an item to the HyperLogLog.
        
        Time complexity: O(1)
        
        Args:
            item: The item to add
        """
        hash_value = self._hash(item)
        
        # Use first p bits to determine register index
        register_index = hash_value >> (64 - self.precision)
        
        # Use remaining bits for leading zero count
        remaining_bits = hash_value & ((1 << (64 - self.precision)) - 1)
        rho = self._count_leading_zeros(remaining_bits, 64 - self.precision)
        
        # Update register with maximum value seen
        self.registers[register_index] = max(self.registers[register_index], rho)
    
    def count(self) -> int:
        """
        Estimate the cardinality (number of distinct elements).
        
        Uses the harmonic mean of register values with bias correction.
        
        Time complexity: O(m)
        
        Returns:
            Estimated cardinality
        """
        # Calculate harmonic mean: Z = 1 / Σ(2^(-M[j]))
        harmonic_sum = sum(2.0 ** (-r) for r in self.registers)
        
        if harmonic_sum == 0:
            return 0
        
        # Raw estimate: E = alpha * m^2 / Z
        raw_estimate = self.alpha * (self.num_registers ** 2) / harmonic_sum
        
        # Apply corrections for small and large ranges
        if raw_estimate <= 2.5 * self.num_registers:
            # Small range correction
            # Count registers with value 0
            zero_registers = self.registers.count(0)
            if zero_registers > 0:
                # Use linear counting estimate
                return int(self.num_registers * math.log(self.num_registers / zero_registers))
        
        if raw_estimate > (1 << 32) / 30:
            # Large range correction (for very large cardinalities)
            # Apply formula: E* = -2^32 * log(1 - E/2^32)
            raw_estimate = -(1 << 32) * math.log(1 - raw_estimate / (1 << 32))
        
        return int(raw_estimate)
    
    def merge(self, other: 'HyperLogLog') -> 'HyperLogLog':
        """
        Merge two HyperLogLog structures.
        
        This is useful for distributed counting where different nodes
        process different subsets of data.
        
        Args:
            other: Another HyperLogLog to merge with
            
        Returns:
            New merged HyperLogLog
            
        Raises:
            ValueError: If precisions don't match
        """
        if self.precision != other.precision:
            raise ValueError("HyperLogLogs must have the same precision")
        
        merged = HyperLogLog(self.precision)
        for i in range(self.num_registers):
            merged.registers[i] = max(self.registers[i], other.registers[i])
        
        return merged
    
    def memory_usage_bytes(self) -> int:
        """
        Return the memory used by registers in bytes.
        
        Each register uses 1 byte (stores values 0-64).
        """
        return self.num_registers
    
    def relative_error(self) -> float:
        """
        Return the expected relative error for this precision.
        
        Standard error ≈ 1.04/√m where m is the number of registers.
        """
        return 1.04 / math.sqrt(self.num_registers)


class ExactCounter:
    """
    Exact cardinality counter using a set.
    
    Provided for comparison with HyperLogLog to demonstrate
    the space savings of probabilistic counting.
    """
    
    def __init__(self):
        self.items = set()
    
    def add(self, item: str) -> None:
        """Add an item."""
        self.items.add(item)
    
    def count(self) -> int:
        """Return the exact count."""
        return len(self.items)
    
    def memory_usage_estimate(self) -> int:
        """
        Estimate memory usage (very approximate).
        
        Assumes average item size of 20 characters + set overhead.
        """
        return len(self.items) * 50  # Rough estimate


def demo():
    """Demonstrate HyperLogLog usage."""
    print("=" * 60)
    print("       HYPERLOGLOG DEMONSTRATION (Python)")
    print("=" * 60)
    print()
    
    # Create HyperLogLog with precision 14 (16384 registers)
    precision = 14
    hll = HyperLogLog(precision)
    exact = ExactCounter()
    
    print(f"HyperLogLog Configuration:")
    print(f"  Precision (p): {precision}")
    print(f"  Registers (m): {hll.num_registers}")
    print(f"  Memory: {hll.memory_usage_bytes()} bytes")
    print(f"  Expected error: {hll.relative_error():.2%}")
    print()
    
    # Generate unique items
    print("Processing unique items...")
    test_sizes = [100, 1000, 10000, 100000]
    
    for target_size in test_sizes:
        # Reset for each test
        hll = HyperLogLog(precision)
        exact = ExactCounter()
        
        # Add items
        for i in range(target_size):
            item = f"item_{i}"
            hll.add(item)
            exact.add(item)
        
        hll_estimate = hll.count()
        exact_count = exact.count()
        error = abs(hll_estimate - exact_count) / exact_count * 100
        
        print(f"  n={target_size:>6}: HLL={hll_estimate:>6}, "
              f"Exact={exact_count:>6}, Error={error:>5.2f}%")
    
    print()
    
    # Demonstrate with duplicate items
    print("-" * 60)
    print("       DUPLICATE HANDLING")
    print("-" * 60)
    print()
    
    hll = HyperLogLog(14)
    exact = ExactCounter()
    
    # Add 10000 items, but only 1000 are unique
    for i in range(10000):
        item = f"item_{i % 1000}"  # Only 1000 unique items
        hll.add(item)
        exact.add(item)
    
    print(f"Added 10,000 items with 1,000 unique:")
    print(f"  HyperLogLog estimate: {hll.count()}")
    print(f"  Exact count: {exact.count()}")
    print()
    
    # Demonstrate merging
    print("-" * 60)
    print("       DISTRIBUTED MERGING")
    print("-" * 60)
    print()
    
    hll1 = HyperLogLog(14)
    hll2 = HyperLogLog(14)
    
    # Simulate two nodes processing different data
    # Node 1: items 0-4999
    for i in range(5000):
        hll1.add(f"item_{i}")
    
    # Node 2: items 2500-7499 (overlapping range)
    for i in range(2500, 7500):
        hll2.add(f"item_{i}")
    
    print(f"Node 1: items 0-4999, estimate = {hll1.count()}")
    print(f"Node 2: items 2500-7499, estimate = {hll2.count()}")
    
    # Merge
    merged = hll1.merge(hll2)
    print(f"Merged: unique items 0-7499, estimate = {merged.count()}")
    print(f"Expected: 7500 unique items")
    print()
    
    # Memory comparison
    print("-" * 60)
    print("       MEMORY COMPARISON (1M unique items)")
    print("-" * 60)
    print()
    
    n = 1_000_000
    hll_memory = HyperLogLog(14).memory_usage_bytes()
    exact_memory = n * 50  # Rough estimate for set
    
    print(f"  HyperLogLog (p=14): {hll_memory:,} bytes ({hll_memory/1024:.1f} KB)")
    print(f"  Exact set estimate: {exact_memory:,} bytes ({exact_memory/1024/1024:.1f} MB)")
    print(f"  Compression ratio: {exact_memory/hll_memory:.0f}:1")
    print()
    
    print("=" * 60)
    print("       DEMONSTRATION COMPLETE")
    print("=" * 60)


if __name__ == "__main__":
    demo()

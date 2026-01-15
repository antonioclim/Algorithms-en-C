#!/usr/bin/env python3
"""
Bloom Filter Implementation in Python
Week 17: Probabilistic Data Structures for Big Data
ATP Course - ASE-CSIE Bucharest

This file provides a Python perspective on Bloom filters for comparison
with the C implementation. It is NOT intended for execution within the
course exercises but serves as a reference for understanding the algorithm
in a higher-level language.

Differences from C Implementation:
- Python handles arbitrary-precision integers natively
- No manual memory management required
- Built-in bitarray module (or equivalent using bytearray)
- hashlib provides cryptographic hash functions
"""

import math
import hashlib
from typing import List, Optional


class BloomFilter:
    """
    A space-efficient probabilistic data structure for set membership testing.
    
    The Bloom filter supports two operations:
    - add(item): Add an item to the set
    - query(item): Test if an item might be in the set
    
    False positives are possible, but false negatives are not.
    The filter does not support deletion (see CountingBloomFilter).
    
    Attributes:
        num_bits (int): Size of the bit array (m)
        num_hashes (int): Number of hash functions (k)
        bits (bytearray): The bit array
        num_items (int): Number of items added
    """
    
    def __init__(self, num_bits: int, num_hashes: int):
        """
        Initialise a Bloom filter with specified parameters.
        
        Args:
            num_bits: Size of the bit array (m)
            num_hashes: Number of hash functions to use (k)
        """
        self.num_bits = num_bits
        self.num_hashes = num_hashes
        self.num_items = 0
        
        # Use bytearray for the bit array
        # Each byte stores 8 bits
        self.bits = bytearray((num_bits + 7) // 8)
    
    @classmethod
    def create_optimal(cls, expected_items: int, false_positive_rate: float) -> 'BloomFilter':
        """
        Create a Bloom filter with optimal parameters for given requirements.
        
        The optimal number of bits is: m = -n * ln(p) / (ln(2))^2
        The optimal number of hashes is: k = (m/n) * ln(2)
        
        Args:
            expected_items: Expected number of items to add (n)
            false_positive_rate: Desired false positive probability (p)
            
        Returns:
            BloomFilter with optimal parameters
            
        Raises:
            ValueError: If parameters are invalid
        """
        if expected_items <= 0:
            raise ValueError("Expected items must be positive")
        if not (0 < false_positive_rate < 1):
            raise ValueError("False positive rate must be between 0 and 1")
        
        # Calculate optimal number of bits
        # m = -n * ln(p) / (ln(2))^2
        ln2_sq = math.log(2) ** 2
        num_bits = int(-expected_items * math.log(false_positive_rate) / ln2_sq)
        
        # Calculate optimal number of hash functions
        # k = (m/n) * ln(2)
        num_hashes = max(1, int((num_bits / expected_items) * math.log(2)))
        
        return cls(num_bits, num_hashes)
    
    def _hash(self, item: str, seed: int) -> int:
        """
        Generate a hash value for an item with a given seed.
        
        Uses the Kirsch-Mitzenmacher technique: only two hash functions
        are needed to generate k hash values: h_i(x) = h1(x) + i * h2(x)
        
        Args:
            item: The item to hash
            seed: The seed/index for this hash function
            
        Returns:
            Hash value in range [0, num_bits)
        """
        # Use MD5 for h1 and SHA1 for h2 (for demonstration)
        # In production, use faster non-cryptographic hashes like MurmurHash
        data = item.encode('utf-8')
        
        h1 = int(hashlib.md5(data).hexdigest(), 16)
        h2 = int(hashlib.sha1(data).hexdigest(), 16)
        
        # Kirsch-Mitzenmacher technique
        combined = (h1 + seed * h2) % self.num_bits
        return combined
    
    def _set_bit(self, index: int) -> None:
        """Set the bit at the given index."""
        byte_index = index // 8
        bit_index = index % 8
        self.bits[byte_index] |= (1 << bit_index)
    
    def _get_bit(self, index: int) -> bool:
        """Get the bit at the given index."""
        byte_index = index // 8
        bit_index = index % 8
        return bool(self.bits[byte_index] & (1 << bit_index))
    
    def add(self, item: str) -> None:
        """
        Add an item to the Bloom filter.
        
        Sets k bits in the bit array, one for each hash function.
        
        Args:
            item: The item to add
        """
        for i in range(self.num_hashes):
            bit_index = self._hash(item, i)
            self._set_bit(bit_index)
        self.num_items += 1
    
    def query(self, item: str) -> bool:
        """
        Test if an item might be in the set.
        
        Returns True if the item might be in the set (with some probability
        of false positive), or False if the item is definitely not in the set.
        
        Args:
            item: The item to test
            
        Returns:
            True if item might be present, False if definitely absent
        """
        for i in range(self.num_hashes):
            bit_index = self._hash(item, i)
            if not self._get_bit(bit_index):
                return False
        return True
    
    def __contains__(self, item: str) -> bool:
        """Allow using 'in' operator."""
        return self.query(item)
    
    def theoretical_fp_rate(self) -> float:
        """
        Calculate the theoretical false positive rate.
        
        FP rate = (1 - e^(-k*n/m))^k
        
        Returns:
            Theoretical false positive probability
        """
        if self.num_items == 0:
            return 0.0
        
        k = self.num_hashes
        n = self.num_items
        m = self.num_bits
        
        # (1 - e^(-k*n/m))^k
        exponent = -k * n / m
        return (1 - math.exp(exponent)) ** k
    
    def memory_usage_bytes(self) -> int:
        """Return the memory used by the bit array in bytes."""
        return len(self.bits)


class CountingBloomFilter:
    """
    A Bloom filter variant that supports deletion by using counters instead of bits.
    
    Each position in the filter uses a small counter (typically 4 bits) instead
    of a single bit. This allows decrementing when items are removed.
    
    The trade-off is increased memory usage (4x or more compared to basic Bloom).
    """
    
    def __init__(self, num_counters: int, num_hashes: int, counter_bits: int = 4):
        """
        Initialise a Counting Bloom filter.
        
        Args:
            num_counters: Number of counter positions (m)
            num_hashes: Number of hash functions (k)
            counter_bits: Bits per counter (default 4, max value 15)
        """
        self.num_counters = num_counters
        self.num_hashes = num_hashes
        self.counter_bits = counter_bits
        self.max_count = (1 << counter_bits) - 1
        self.num_items = 0
        
        # Store counters as a list for simplicity
        # In production, pack counters more efficiently
        self.counters = [0] * num_counters
    
    def _hash(self, item: str, seed: int) -> int:
        """Generate hash value using Kirsch-Mitzenmacher technique."""
        data = item.encode('utf-8')
        h1 = int(hashlib.md5(data).hexdigest(), 16)
        h2 = int(hashlib.sha1(data).hexdigest(), 16)
        return (h1 + seed * h2) % self.num_counters
    
    def add(self, item: str) -> bool:
        """
        Add an item to the filter.
        
        Args:
            item: The item to add
            
        Returns:
            True if successful, False if any counter would overflow
        """
        indices = [self._hash(item, i) for i in range(self.num_hashes)]
        
        # Check for potential overflow
        for idx in indices:
            if self.counters[idx] >= self.max_count:
                return False  # Would overflow
        
        # Increment all counters
        for idx in indices:
            self.counters[idx] += 1
        
        self.num_items += 1
        return True
    
    def remove(self, item: str) -> bool:
        """
        Remove an item from the filter.
        
        Args:
            item: The item to remove
            
        Returns:
            True if removed, False if item was not present
        """
        if not self.query(item):
            return False
        
        # Decrement all counters
        for i in range(self.num_hashes):
            idx = self._hash(item, i)
            if self.counters[idx] > 0:
                self.counters[idx] -= 1
        
        self.num_items -= 1
        return True
    
    def query(self, item: str) -> bool:
        """Test if an item might be in the set."""
        for i in range(self.num_hashes):
            idx = self._hash(item, i)
            if self.counters[idx] == 0:
                return False
        return True


def demo():
    """Demonstrate Bloom filter usage."""
    print("=" * 60)
    print("       BLOOM FILTER DEMONSTRATION (Python)")
    print("=" * 60)
    print()
    
    # Create optimal filter for 1000 items with 1% FP rate
    bf = BloomFilter.create_optimal(1000, 0.01)
    print(f"Created Bloom filter:")
    print(f"  Bits: {bf.num_bits}")
    print(f"  Hash functions: {bf.num_hashes}")
    print(f"  Memory: {bf.memory_usage_bytes()} bytes")
    print()
    
    # Add some items
    words = ["apple", "banana", "cherry", "date", "elderberry"]
    for word in words:
        bf.add(word)
    
    print(f"Added {len(words)} items: {words}")
    print()
    
    # Query items
    test_words = ["apple", "banana", "grape", "melon", "cherry"]
    print("Query results:")
    for word in test_words:
        present = "possibly present" if word in bf else "definitely absent"
        actual = "✓ in set" if word in words else "✗ not in set"
        print(f"  '{word}': {present} ({actual})")
    print()
    
    print(f"Theoretical FP rate: {bf.theoretical_fp_rate():.4%}")
    print()
    
    # Demonstrate counting bloom filter
    print("-" * 60)
    print("       COUNTING BLOOM FILTER")
    print("-" * 60)
    print()
    
    cbf = CountingBloomFilter(1000, 3)
    cbf.add("hello")
    cbf.add("world")
    print(f"Added 'hello' and 'world'")
    print(f"  'hello' present: {cbf.query('hello')}")
    print(f"  'world' present: {cbf.query('world')}")
    print(f"  'foo' present: {cbf.query('foo')}")
    
    print()
    cbf.remove("hello")
    print("Removed 'hello'")
    print(f"  'hello' present: {cbf.query('hello')}")
    print(f"  'world' present: {cbf.query('world')}")
    
    print()
    print("=" * 60)
    print("       DEMONSTRATION COMPLETE")
    print("=" * 60)


if __name__ == "__main__":
    demo()

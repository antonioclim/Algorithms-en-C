# Week 11 Homework: Hash Tables

## 📋 General Information

- **Deadline:** End of Week 12 (Sunday, 23:59)
- **Points:** 100 (10% of final grade)
- **Language:** C (C11 standard)
- **Compiler:** GCC with `-Wall -Wextra -std=c11`
- **Memory check:** Valgrind must report no leaks

---

## 📝 Homework 1: Phone Directory (50 points)

### Description

Implement a phone directory using a chained hash table. The directory stores contacts with names as keys and phone numbers as values. The system must support adding, searching, updating and deleting contacts, as well as listing all contacts in alphabetical order.

### Requirements

1. **Data structures** (10 points)
   - Define a `Contact` structure with fields: `name` (max 50 chars), `phone` (max 15 chars)
   - Define appropriate hash table node and table structures
   - Use a hash table with initial size 53 (prime number)

2. **Hash function** (5 points)
   - Implement the djb2 hash function
   - Handle case-insensitive name lookup (store names in lowercase)

3. **Core operations** (20 points)
   - `add_contact(name, phone)` — Add new or update existing contact (5p)
   - `find_contact(name)` — Search and return phone number or NULL (5p)
   - `delete_contact(name)` — Remove contact, return success/failure (5p)
   - `list_all_contacts()` — Print all contacts sorted alphabetically (5p)

4. **Dynamic resizing** (10 points)
   - Implement rehashing when load factor exceeds 0.75
   - Correctly transfer all entries to the new table
   - Update statistics after rehashing

5. **Memory management** (5 points)
   - Free all allocated memory in `destroy_directory()`
   - No memory leaks (verified with Valgrind)

### Example Usage

```c
PhoneDirectory *dir = create_directory();

add_contact(dir, "Alice Smith", "0712-345-678");
add_contact(dir, "Bob Jones", "0723-456-789");
add_contact(dir, "Carol White", "0734-567-890");

char *phone = find_contact(dir, "alice smith");  // Case-insensitive
// phone = "0712-345-678"

delete_contact(dir, "Bob Jones");

list_all_contacts(dir);
// Output (alphabetically sorted):
// Alice Smith: 0712-345-678
// Carol White: 0734-567-890

destroy_directory(dir);
```

### Input/Output Format

The program should read commands from stdin:
```
ADD Alice Smith 0712-345-678
ADD Bob Jones 0723-456-789
FIND alice smith
DELETE Bob Jones
LIST
STATS
EXIT
```

### File: `homework1_phone_directory.c`

---

## 📝 Homework 2: Anagram Grouper (50 points)

### Description

Implement a program that groups anagrams together using a hash table. Two words are anagrams if they contain the same letters in different orders (e.g., "listen" and "silent"). The hash table should use sorted letters as the key and store lists of words sharing those letters.

### Requirements

1. **Key generation** (10 points)
   - Implement `get_sorted_key(word)` that returns letters sorted alphabetically
   - Example: "listen" → "eilnst", "silent" → "eilnst"
   - Convert to lowercase, remove non-alphabetic characters

2. **Hash table structure** (10 points)
   - Use chaining with linked lists
   - Each bucket stores anagram groups (sorted key → list of words)
   - Initial table size: 101 (prime number)

3. **Operations** (15 points)
   - `add_word(word)` — Add word to appropriate anagram group (5p)
   - `find_anagrams(word)` — Return all anagrams of given word (5p)
   - `get_all_groups()` — Return all anagram groups with 2+ words (5p)

4. **File processing** (10 points)
   - Read words from input file (one word per line)
   - Handle at least 10,000 words efficiently
   - Ignore words shorter than 2 characters

5. **Output and memory** (5 points)
   - Display anagram groups sorted by group size (largest first)
   - Free all memory correctly

### Example Usage

```c
AnagramTable *at = create_anagram_table();

add_word(at, "listen");
add_word(at, "silent");
add_word(at, "enlist");
add_word(at, "hello");
add_word(at, "world");
add_word(at, "tinsel");

// Group for "eilnst": [listen, silent, enlist, tinsel]
// Group for "ehllo": [hello]
// Group for "dlorw": [world]

WordList *anagrams = find_anagrams(at, "listen");
// Returns: [listen, silent, enlist, tinsel]

print_all_groups(at);
// Output:
// Group 1 (4 words): listen, silent, enlist, tinsel
// Group 2 (1 word): hello
// Group 3 (1 word): world

destroy_anagram_table(at);
```

### Input File Format

```
listen
silent
hello
world
enlist
tinsel
...
```

### File: `homework2_anagram_grouper.c`

---

## 📊 Evaluation Criteria

| Criterion | Points |
|-----------|--------|
| Functional correctness | 40 |
| Proper use of hash tables | 25 |
| Edge case handling | 15 |
| Code quality and style | 10 |
| No compiler warnings | 10 |

### Penalties

- **-10 points:** Compiler warnings with `-Wall -Wextra`
- **-20 points:** Memory leaks detected by Valgrind
- **-30 points:** Crashes on valid input
- **-50 points:** Plagiarism (automatic failure)
- **-5 points per day:** Late submission (max -25 points)

### Bonus Points

- **+5 points:** Implement collision statistics (chain length histogram)
- **+5 points:** Support Unicode characters in names/words
- **+5 points:** Implement serialisation (save/load from binary file)

---

## 📤 Submission

1. Submit two C source files:
   - `homework1_phone_directory.c`
   - `homework2_anagram_grouper.c`

2. Each file must be self-contained and compile independently

3. Include your name and student ID in a comment at the top

4. Submit via the course platform before the deadline

---

## 💡 Tips

1. **Start with the hash function** — Test it separately with known inputs before integrating

2. **Use a small table first** — Debug with size 7 or 11 to see collisions clearly

3. **Print bucket contents** — Add a debug function to visualise the table state

4. **Test edge cases** — Empty table, single entry, full bucket, non-existent keys

5. **Check memory incrementally** — Run Valgrind after each major feature addition

6. **Plan before coding** — Draw the data structures on paper first

---

## 📚 Reference Materials

- Lecture slides: Week 11 - Hash Tables
- Example code: `src/example1.c`
- Laboratory exercises: `src/exercise1.c`, `src/exercise2.c`
- Recommended reading: CLRS Chapter 11

---

*Good luck! Remember: hash tables are everywhere in software development.*
*Mastering them is essential for any serious programmer.*

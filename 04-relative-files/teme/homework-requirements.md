# Week 04 Homework: Linked Lists

## 📋 General Information

- **Deadline:** End of Week 05
- **Points:** 100 (10% of final grade)
- **Language:** C (C11 standard)
- **Compiler:** GCC with `-Wall -Wextra -std=c11`

---

## 📝 Homework 1: Music Playlist Manager (50 points)

### Description

Implement a **doubly linked list** to manage a music playlist with support for playback operations. The playlist should support adding, removing and rearranging songs, as well as simulating playback with next/previous functionality.

### Requirements

1. **Song Structure** (5 points)
   - Define a `Song` structure with fields: `title` (char[100]), `artist` (char[50]), `duration_seconds` (int), `prev` and `next` pointers
   - Define a `Playlist` structure with: `head`, `tail`, `current` (currently playing), `name`, `total_songs`

2. **Core Operations** (20 points)
   - `add_song_end(playlist, title, artist, duration)` - Add song at end (4p)
   - `add_song_beginning(playlist, title, artist, duration)` - Add song at start (4p)
   - `insert_song_after(playlist, existing_title, new_title, artist, duration)` - Insert after specific song (4p)
   - `remove_song(playlist, title)` - Remove song by title (4p)
   - `find_song(playlist, title)` - Search for song, return pointer (4p)

3. **Playback Operations** (15 points)
   - `play_next(playlist)` - Move to next song, wrap to beginning if at end (3p)
   - `play_previous(playlist)` - Move to previous song, wrap to end if at beginning (3p)
   - `shuffle_playlist(playlist)` - Randomly reorder songs (5p)
   - `reverse_playlist(playlist)` - Reverse the order of songs (4p)

4. **Display and Utility** (10 points)
   - `display_playlist(playlist)` - Show all songs with their position and duration (3p)
   - `display_current(playlist)` - Show currently playing song with "▶" indicator (2p)
   - `total_duration(playlist)` - Calculate total playlist duration in HH:MM:SS format (2p)
   - `free_playlist(playlist)` - Free all memory (3p)

### Example Usage

```c
Playlist *my_playlist = create_playlist("My Favourites");

add_song_end(my_playlist, "Bohemian Rhapsody", "Queen", 354);
add_song_end(my_playlist, "Stairway to Heaven", "Led Zeppelin", 482);
add_song_end(my_playlist, "Hotel California", "Eagles", 391);

display_playlist(my_playlist);
/*
My Favourites (3 songs, 20:27)
1. Bohemian Rhapsody - Queen (5:54)
2. Stairway to Heaven - Led Zeppelin (8:02)
3. Hotel California - Eagles (6:31)
*/

play_next(my_playlist);
play_next(my_playlist);
display_current(my_playlist);
/* ▶ Now playing: Stairway to Heaven - Led Zeppelin */

reverse_playlist(my_playlist);
display_playlist(my_playlist);
/* Order reversed */
```

### File: `homework1_playlist.c`

---

## 📝 Homework 2: Sparse Matrix Operations (50 points)

### Description

Implement a **sparse matrix** representation using linked lists. A sparse matrix is one where most elements are zero, making it inefficient to store as a traditional 2D array. Instead, only non-zero elements are stored.

### Requirements

1. **Data Structures** (5 points)
   - Define `MatrixNode` with: `row`, `col`, `value` (double), `next_in_row`, `next_in_col`
   - Define `SparseMatrix` with: `rows`, `cols`, row head array, column head array

2. **Matrix Creation** (15 points)
   - `create_sparse_matrix(rows, cols)` - Create empty sparse matrix (3p)
   - `set_element(matrix, row, col, value)` - Set/update element (if value ≈ 0, remove) (6p)
   - `get_element(matrix, row, col)` - Get element value (return 0 if not stored) (3p)
   - `load_from_file(filename)` - Load matrix from file (3p)

3. **Matrix Operations** (20 points)
   - `add_matrices(A, B)` - Add two sparse matrices, return new matrix (5p)
   - `multiply_matrices(A, B)` - Multiply two sparse matrices (7p)
   - `transpose(matrix)` - Return transposed matrix (4p)
   - `scalar_multiply(matrix, scalar)` - Multiply all elements by scalar (4p)

4. **Display and Analysis** (10 points)
   - `display_sparse(matrix)` - Show non-zero elements list: (row, col) = value (2p)
   - `display_dense(matrix)` - Show full matrix format (for small matrices) (3p)
   - `count_nonzero(matrix)` - Count non-zero elements (1p)
   - `sparsity_ratio(matrix)` - Calculate percentage of zero elements (2p)
   - `free_sparse_matrix(matrix)` - Free all memory (2p)

### Example Usage

```c
SparseMatrix *A = create_sparse_matrix(4, 4);

set_element(A, 0, 0, 5.0);
set_element(A, 0, 2, 3.0);
set_element(A, 1, 1, 8.0);
set_element(A, 2, 3, 6.0);
set_element(A, 3, 0, 2.0);

display_sparse(A);
/*
Sparse Matrix (4x4), 5 non-zero elements:
(0, 0) = 5.00
(0, 2) = 3.00
(1, 1) = 8.00
(2, 3) = 6.00
(3, 0) = 2.00
*/

display_dense(A);
/*
 5.00  0.00  3.00  0.00
 0.00  8.00  0.00  0.00
 0.00  0.00  0.00  6.00
 2.00  0.00  0.00  0.00
*/

printf("Sparsity: %.1f%%\n", sparsity_ratio(A));
/* Sparsity: 68.8% */
```

### Matrix File Format

```
4 4
0 0 5.0
0 2 3.0
1 1 8.0
2 3 6.0
3 0 2.0
```

### File: `homework2_sparse.c`

---

## 📊 Evaluation Criteria

| Criterion | Points |
|-----------|--------|
| Functional correctness | 40 |
| Proper use of linked list concepts | 25 |
| Edge case handling | 15 |
| Code quality and documentation | 10 |
| No compiler warnings | 10 |

### Penalties

- **-10 points:** Compiler warnings
- **-20 points:** Memory leaks (verified with Valgrind)
- **-30 points:** Crashes on valid input
- **-50 points:** Plagiarism (automatic fail)

### Bonus Points

- **+5 points:** Particularly elegant or efficient solution
- **+5 points:** Comprehensive error handling
- **+5 points:** Additional useful features

---

## 📤 Submission

1. Submit source files: `homework1_playlist.c` and `homework2_sparse.c`
2. Include a `README.txt` explaining your implementation choices
3. Submit via the course portal before the deadline
4. Ensure your code compiles without warnings using:
   ```bash
   gcc -Wall -Wextra -std=c11 -o homework1 homework1_playlist.c
   gcc -Wall -Wextra -std=c11 -lm -o homework2 homework2_sparse.c
   ```

---

## 💡 Tips

1. **Start with the data structures** - Define your structures clearly before implementing functions

2. **Test incrementally** - Write and test one function at a time

3. **Draw diagrams** - Visualise your linked list operations before coding them

4. **Handle edge cases first** - Empty list, single element, head/tail operations

5. **Use Valgrind early** - Check for memory leaks after implementing each function:
   ```bash
   valgrind --leak-check=full ./homework1
   ```

6. **Comment your pointer manipulations** - Complex pointer operations benefit from explanatory comments

---

*Good luck! Remember: linked lists are fundamental to many advanced data structures. Mastering them now will help you throughout your programming career.*

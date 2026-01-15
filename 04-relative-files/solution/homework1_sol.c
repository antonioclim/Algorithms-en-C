/**
 * =============================================================================
 * HOMEWORK 1 SOLUTION: Music Playlist Manager
 * =============================================================================
 *
 * INSTRUCTOR COPY - Complete working solution
 *
 * Implements a doubly linked list to manage a music playlist with
 * playback operations, shuffle and reverse functionality.
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_TITLE 100
#define MAX_ARTIST 50

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct Song {
    char title[MAX_TITLE];
    char artist[MAX_ARTIST];
    int duration_seconds;
    struct Song *prev;
    struct Song *next;
} Song;

typedef struct Playlist {
    Song *head;
    Song *tail;
    Song *current;  /* Currently playing */
    char name[MAX_TITLE];
    int total_songs;
} Playlist;

/* =============================================================================
 * FUNCTION IMPLEMENTATIONS
 * =============================================================================
 */

/**
 * Creates a new song node.
 */
Song* create_song(const char *title, const char *artist, int duration) {
    Song *song = (Song*)malloc(sizeof(Song));
    if (song == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    
    strncpy(song->title, title, MAX_TITLE - 1);
    song->title[MAX_TITLE - 1] = '\0';
    
    strncpy(song->artist, artist, MAX_ARTIST - 1);
    song->artist[MAX_ARTIST - 1] = '\0';
    
    song->duration_seconds = duration;
    song->prev = NULL;
    song->next = NULL;
    
    return song;
}

/**
 * Creates an empty playlist.
 */
Playlist* create_playlist(const char *name) {
    Playlist *playlist = (Playlist*)malloc(sizeof(Playlist));
    if (playlist == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    
    strncpy(playlist->name, name, MAX_TITLE - 1);
    playlist->name[MAX_TITLE - 1] = '\0';
    
    playlist->head = NULL;
    playlist->tail = NULL;
    playlist->current = NULL;
    playlist->total_songs = 0;
    
    return playlist;
}

/**
 * Adds a song at the end of the playlist.
 */
void add_song_end(Playlist *playlist, const char *title, const char *artist, int duration) {
    if (playlist == NULL) return;
    
    Song *song = create_song(title, artist, duration);
    if (song == NULL) return;
    
    if (playlist->tail == NULL) {
        /* Empty playlist */
        playlist->head = song;
        playlist->tail = song;
        playlist->current = song;
    } else {
        song->prev = playlist->tail;
        playlist->tail->next = song;
        playlist->tail = song;
    }
    
    playlist->total_songs++;
}

/**
 * Adds a song at the beginning of the playlist.
 */
void add_song_beginning(Playlist *playlist, const char *title, const char *artist, int duration) {
    if (playlist == NULL) return;
    
    Song *song = create_song(title, artist, duration);
    if (song == NULL) return;
    
    if (playlist->head == NULL) {
        playlist->head = song;
        playlist->tail = song;
        playlist->current = song;
    } else {
        song->next = playlist->head;
        playlist->head->prev = song;
        playlist->head = song;
    }
    
    playlist->total_songs++;
}

/**
 * Inserts a song after a song with the given title.
 */
bool insert_song_after(Playlist *playlist, const char *existing_title, 
                       const char *new_title, const char *artist, int duration) {
    if (playlist == NULL || playlist->head == NULL) return false;
    
    /* Find the existing song */
    Song *current = playlist->head;
    while (current != NULL && strcmp(current->title, existing_title) != 0) {
        current = current->next;
    }
    
    if (current == NULL) return false;  /* Not found */
    
    Song *new_song = create_song(new_title, artist, duration);
    if (new_song == NULL) return false;
    
    new_song->next = current->next;
    new_song->prev = current;
    
    if (current->next != NULL) {
        current->next->prev = new_song;
    } else {
        playlist->tail = new_song;
    }
    
    current->next = new_song;
    playlist->total_songs++;
    
    return true;
}

/**
 * Removes a song by title.
 */
bool remove_song(Playlist *playlist, const char *title) {
    if (playlist == NULL || playlist->head == NULL) return false;
    
    Song *current = playlist->head;
    while (current != NULL && strcmp(current->title, title) != 0) {
        current = current->next;
    }
    
    if (current == NULL) return false;
    
    /* Update current playing if needed */
    if (playlist->current == current) {
        playlist->current = current->next ? current->next : current->prev;
    }
    
    /* Update links */
    if (current->prev != NULL) {
        current->prev->next = current->next;
    } else {
        playlist->head = current->next;
    }
    
    if (current->next != NULL) {
        current->next->prev = current->prev;
    } else {
        playlist->tail = current->prev;
    }
    
    free(current);
    playlist->total_songs--;
    
    return true;
}

/**
 * Finds a song by title.
 */
Song* find_song(Playlist *playlist, const char *title) {
    if (playlist == NULL) return NULL;
    
    Song *current = playlist->head;
    while (current != NULL) {
        if (strcmp(current->title, title) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

/**
 * Moves to the next song, wrapping to beginning if at end.
 */
void play_next(Playlist *playlist) {
    if (playlist == NULL || playlist->current == NULL) return;
    
    if (playlist->current->next != NULL) {
        playlist->current = playlist->current->next;
    } else {
        playlist->current = playlist->head;  /* Wrap to beginning */
    }
}

/**
 * Moves to the previous song, wrapping to end if at beginning.
 */
void play_previous(Playlist *playlist) {
    if (playlist == NULL || playlist->current == NULL) return;
    
    if (playlist->current->prev != NULL) {
        playlist->current = playlist->current->prev;
    } else {
        playlist->current = playlist->tail;  /* Wrap to end */
    }
}

/**
 * Shuffles the playlist using Fisher-Yates algorithm.
 */
void shuffle_playlist(Playlist *playlist) {
    if (playlist == NULL || playlist->total_songs < 2) return;
    
    /* Create array of song pointers */
    Song **songs = (Song**)malloc(playlist->total_songs * sizeof(Song*));
    if (songs == NULL) return;
    
    Song *current = playlist->head;
    for (int i = 0; i < playlist->total_songs; i++) {
        songs[i] = current;
        current = current->next;
    }
    
    /* Fisher-Yates shuffle */
    srand(time(NULL));
    for (int i = playlist->total_songs - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Song *temp = songs[i];
        songs[i] = songs[j];
        songs[j] = temp;
    }
    
    /* Rebuild links */
    playlist->head = songs[0];
    playlist->head->prev = NULL;
    
    for (int i = 1; i < playlist->total_songs; i++) {
        songs[i-1]->next = songs[i];
        songs[i]->prev = songs[i-1];
    }
    
    playlist->tail = songs[playlist->total_songs - 1];
    playlist->tail->next = NULL;
    
    free(songs);
}

/**
 * Reverses the playlist order.
 */
void reverse_playlist(Playlist *playlist) {
    if (playlist == NULL || playlist->total_songs < 2) return;
    
    Song *current = playlist->head;
    Song *temp = NULL;
    
    /* Swap prev and next for all nodes */
    while (current != NULL) {
        temp = current->prev;
        current->prev = current->next;
        current->next = temp;
        current = current->prev;  /* Move to original next */
    }
    
    /* Swap head and tail */
    temp = playlist->head;
    playlist->head = playlist->tail;
    playlist->tail = temp;
}

/**
 * Formats duration as MM:SS or HH:MM:SS.
 */
void format_duration(int seconds, char *buffer, size_t size) {
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    
    if (hours > 0) {
        snprintf(buffer, size, "%d:%02d:%02d", hours, minutes, secs);
    } else {
        snprintf(buffer, size, "%d:%02d", minutes, secs);
    }
}

/**
 * Calculates total playlist duration.
 */
int total_duration(Playlist *playlist) {
    if (playlist == NULL) return 0;
    
    int total = 0;
    Song *current = playlist->head;
    
    while (current != NULL) {
        total += current->duration_seconds;
        current = current->next;
    }
    
    return total;
}

/**
 * Displays the playlist.
 */
void display_playlist(Playlist *playlist) {
    if (playlist == NULL) {
        printf("No playlist\n");
        return;
    }
    
    char duration_str[20];
    format_duration(total_duration(playlist), duration_str, sizeof(duration_str));
    
    printf("\n%s (%d songs, %s)\n", playlist->name, playlist->total_songs, duration_str);
    printf("────────────────────────────────────────────\n");
    
    if (playlist->head == NULL) {
        printf("  (empty playlist)\n");
    } else {
        int pos = 1;
        Song *current = playlist->head;
        
        while (current != NULL) {
            char song_duration[10];
            format_duration(current->duration_seconds, song_duration, sizeof(song_duration));
            
            char marker = (current == playlist->current) ? '>' : ' ';
            printf("%c %2d. %s - %s (%s)\n", marker, pos, current->title, 
                   current->artist, song_duration);
            
            current = current->next;
            pos++;
        }
    }
    printf("────────────────────────────────────────────\n");
}

/**
 * Displays the currently playing song.
 */
void display_current(Playlist *playlist) {
    if (playlist == NULL || playlist->current == NULL) {
        printf("No song currently playing\n");
        return;
    }
    
    char duration_str[10];
    format_duration(playlist->current->duration_seconds, duration_str, sizeof(duration_str));
    
    printf("▶ Now playing: %s - %s (%s)\n", 
           playlist->current->title, 
           playlist->current->artist,
           duration_str);
}

/**
 * Frees all memory.
 */
void free_playlist(Playlist *playlist) {
    if (playlist == NULL) return;
    
    Song *current = playlist->head;
    Song *next;
    
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    
    free(playlist);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     Music Playlist Manager - SOLUTION                         ║\n");
    printf("║     Homework 1 - Linked Lists                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    Playlist *my_playlist = create_playlist("My Favourites");
    
    /* Add songs */
    printf("\nAdding songs...\n");
    add_song_end(my_playlist, "Bohemian Rhapsody", "Queen", 354);
    add_song_end(my_playlist, "Stairway to Heaven", "Led Zeppelin", 482);
    add_song_end(my_playlist, "Hotel California", "Eagles", 391);
    add_song_end(my_playlist, "Comfortably Numb", "Pink Floyd", 382);
    add_song_end(my_playlist, "Sweet Child O' Mine", "Guns N' Roses", 356);
    
    display_playlist(my_playlist);
    
    /* Test playback */
    printf("\n--- Playback Test ---\n");
    display_current(my_playlist);
    
    printf("\nPlaying next...\n");
    play_next(my_playlist);
    display_current(my_playlist);
    
    printf("\nPlaying next...\n");
    play_next(my_playlist);
    display_current(my_playlist);
    
    printf("\nPlaying previous...\n");
    play_previous(my_playlist);
    display_current(my_playlist);
    
    /* Test insert after */
    printf("\n--- Insert After Test ---\n");
    insert_song_after(my_playlist, "Hotel California", "Layla", "Derek and the Dominos", 424);
    display_playlist(my_playlist);
    
    /* Test remove */
    printf("\n--- Remove Test ---\n");
    printf("Removing 'Stairway to Heaven'...\n");
    remove_song(my_playlist, "Stairway to Heaven");
    display_playlist(my_playlist);
    
    /* Test reverse */
    printf("\n--- Reverse Test ---\n");
    reverse_playlist(my_playlist);
    display_playlist(my_playlist);
    
    /* Test shuffle */
    printf("\n--- Shuffle Test ---\n");
    shuffle_playlist(my_playlist);
    display_playlist(my_playlist);
    
    /* Cleanup */
    free_playlist(my_playlist);
    
    printf("\n--- Program finished (no memory leaks) ---\n\n");
    
    return 0;
}

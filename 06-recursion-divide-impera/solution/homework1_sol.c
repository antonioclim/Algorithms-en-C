/**
 * =============================================================================
 * HOMEWORK 1: HOT POTATO GAME SIMULATION - SOLUTION
 * =============================================================================
 *
 * Complete solution for the Hot Potato elimination game using circular queue.
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 * Usage: ./homework1_sol [players_file]
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_PLAYERS 20
#define MAX_NAME_LENGTH 32
#define MIN_PASSES 1
#define MAX_PASSES 10

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    bool is_active;
} Player;

typedef struct {
    int data[MAX_PLAYERS];
    int front;
    int rear;
    int count;
    int capacity;
} CircularQueue;

typedef struct {
    int total_rounds;
    int total_passes;
    char elimination_order[MAX_PLAYERS][MAX_NAME_LENGTH];
    int elimination_count;
} GameStats;

/* =============================================================================
 * QUEUE IMPLEMENTATION
 * =============================================================================
 */

void queue_init(CircularQueue *q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
    q->capacity = MAX_PLAYERS;
}

bool queue_is_empty(const CircularQueue *q) {
    return q->count == 0;
}

bool queue_enqueue(CircularQueue *q, int player_id) {
    if (q->count >= q->capacity) return false;
    
    q->data[q->rear] = player_id;
    q->rear = (q->rear + 1) % q->capacity;
    q->count++;
    return true;
}

bool queue_dequeue(CircularQueue *q, int *player_id) {
    if (queue_is_empty(q)) return false;
    
    *player_id = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->count--;
    return true;
}

int queue_size(const CircularQueue *q) {
    return q->count;
}

/* =============================================================================
 * GAME FUNCTIONS
 * =============================================================================
 */

int load_players(const char *filename, Player players[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        /* Use default players if file not found */
        printf("  Note: Using default player list\n");
        const char *default_names[] = {"Alice", "Bob", "Charlie", "Diana", "Eve"};
        int n = 5;
        for (int i = 0; i < n; i++) {
            players[i].id = i;
            strncpy(players[i].name, default_names[i], MAX_NAME_LENGTH - 1);
            players[i].is_active = true;
        }
        return n;
    }
    
    int count = 0;
    char line[MAX_NAME_LENGTH + 10];
    
    while (fgets(line, sizeof(line), file) && count < MAX_PLAYERS) {
        /* Skip comments and empty lines */
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        
        /* Remove newline */
        line[strcspn(line, "\n\r")] = 0;
        
        if (strlen(line) > 0) {
            players[count].id = count;
            strncpy(players[count].name, line, MAX_NAME_LENGTH - 1);
            players[count].name[MAX_NAME_LENGTH - 1] = '\0';
            players[count].is_active = true;
            count++;
        }
    }
    
    fclose(file);
    return count;
}

void print_players(const Player players[], int count) {
    printf("  Players: ");
    for (int i = 0; i < count; i++) {
        printf("%s", players[i].name);
        if (i < count - 1) printf(", ");
    }
    printf("\n\n");
}

void play_game(Player players[], int player_count, GameStats *stats) {
    CircularQueue queue;
    queue_init(&queue);
    
    /* Add all players to the queue */
    for (int i = 0; i < player_count; i++) {
        queue_enqueue(&queue, i);
    }
    
    stats->total_rounds = 0;
    stats->total_passes = 0;
    stats->elimination_count = 0;
    
    printf("  ═══════════════════════════════════════════════════════════════\n");
    
    /* Continue until one player remains */
    while (queue_size(&queue) > 1) {
        stats->total_rounds++;
        
        /* Random number of passes (1-10) */
        int passes = MIN_PASSES + (rand() % (MAX_PASSES - MIN_PASSES + 1));
        stats->total_passes += passes;
        
        /* Pass the potato */
        for (int i = 0; i < passes; i++) {
            int player_id;
            queue_dequeue(&queue, &player_id);
            queue_enqueue(&queue, player_id);
        }
        
        /* Eliminate the current holder */
        int eliminated_id;
        queue_dequeue(&queue, &eliminated_id);
        players[eliminated_id].is_active = false;
        
        /* Record elimination */
        strncpy(stats->elimination_order[stats->elimination_count], 
                players[eliminated_id].name, MAX_NAME_LENGTH);
        stats->elimination_count++;
        
        printf("  Round %2d: Passed %2d times - %s eliminated!\n",
               stats->total_rounds, passes, players[eliminated_id].name);
    }
    
    printf("  ═══════════════════════════════════════════════════════════════\n\n");
    
    /* Announce winner */
    int winner_id;
    queue_dequeue(&queue, &winner_id);
    
    printf("  🏆 Winner: %s!\n\n", players[winner_id].name);
}

void print_statistics(const GameStats *stats) {
    printf("  ═══════════════════════════════════════════════════════════════\n");
    printf("  Statistics:\n");
    printf("  ───────────────────────────────────────────────────────────────\n");
    printf("    Total rounds:   %d\n", stats->total_rounds);
    printf("    Total passes:   %d\n", stats->total_passes);
    printf("    Average passes: %.2f per round\n", 
           stats->total_rounds > 0 ? (float)stats->total_passes / stats->total_rounds : 0);
    printf("\n  Elimination order:\n");
    for (int i = 0; i < stats->elimination_count; i++) {
        printf("    %d. %s\n", i + 1, stats->elimination_order[i]);
    }
    printf("  ═══════════════════════════════════════════════════════════════\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     HOMEWORK 1: HOT POTATO GAME - SOLUTION                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Seed random number generator */
    srand((unsigned int)time(NULL));
    
    /* Load players */
    Player players[MAX_PLAYERS];
    const char *filename = (argc > 1) ? argv[1] : NULL;
    
    int player_count;
    if (filename) {
        player_count = load_players(filename, players);
        printf("  Loaded %d players from '%s'\n", player_count, filename);
    } else {
        player_count = load_players(NULL, players);
    }
    
    if (player_count < 2) {
        fprintf(stderr, "Error: Need at least 2 players\n");
        return 1;
    }
    
    printf("\n  === Hot Potato Game ===\n");
    print_players(players, player_count);
    
    /* Play the game */
    GameStats stats;
    play_game(players, player_count, &stats);
    
    /* Print final statistics */
    print_statistics(&stats);
    
    printf("\n");
    return 0;
}

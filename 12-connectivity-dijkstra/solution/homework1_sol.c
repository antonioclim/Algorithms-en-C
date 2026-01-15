/**
 * =============================================================================
 * HOMEWORK 1 SOLUTION: Social Network Analysis
 * =============================================================================
 *
 * INSTRUCTOR SOLUTION - DO NOT DISTRIBUTE TO STUDENTS
 *
 * This solution implements:
 *   1. Graph construction from adjacency list format
 *   2. Friend recommendations based on mutual connections
 *   3. Degrees of separation using BFS
 *   4. Community detection using connected components
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

#define MAX_USERS 10000
#define MAX_LINE 256
#define INFINITY_DIST INT_MAX

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct AdjNode {
    int user_id;
    struct AdjNode *next;
} AdjNode;

typedef struct {
    int num_users;
    int num_friendships;
    AdjNode *adj[MAX_USERS];
} SocialNetwork;

typedef struct {
    int items[MAX_USERS];
    int front;
    int rear;
} Queue;

typedef struct {
    int user_id;
    int mutual_count;
} Recommendation;

/* =============================================================================
 * QUEUE OPERATIONS
 * =============================================================================
 */

void queue_init(Queue *q) {
    q->front = 0;
    q->rear = -1;
}

bool queue_is_empty(Queue *q) {
    return q->rear < q->front;
}

void queue_enqueue(Queue *q, int value) {
    q->items[++q->rear] = value;
}

int queue_dequeue(Queue *q) {
    return q->items[q->front++];
}

/* =============================================================================
 * NETWORK OPERATIONS
 * =============================================================================
 */

void network_init(SocialNetwork *net, int n) {
    net->num_users = n;
    net->num_friendships = 0;
    
    for (int i = 0; i < n; i++) {
        net->adj[i] = NULL;
    }
}

AdjNode *create_node(int user_id) {
    AdjNode *node = (AdjNode *)malloc(sizeof(AdjNode));
    if (node == NULL) {
        return NULL;
    }
    
    node->user_id = user_id;
    node->next = NULL;
    
    return node;
}

void network_add_friendship(SocialNetwork *net, int u, int v) {
    /* Add v to u's friend list */
    AdjNode *node_v = create_node(v);
    if (node_v != NULL) {
        node_v->next = net->adj[u];
        net->adj[u] = node_v;
    }
    
    /* Add u to v's friend list */
    AdjNode *node_u = create_node(u);
    if (node_u != NULL) {
        node_u->next = net->adj[v];
        net->adj[v] = node_u;
    }
    
    net->num_friendships++;
}

bool are_friends(SocialNetwork *net, int u, int v) {
    AdjNode *curr = net->adj[u];
    while (curr != NULL) {
        if (curr->user_id == v) {
            return true;
        }
        curr = curr->next;
    }
    return false;
}

void network_destroy(SocialNetwork *net) {
    for (int i = 0; i < net->num_users; i++) {
        AdjNode *curr = net->adj[i];
        while (curr != NULL) {
            AdjNode *temp = curr;
            curr = curr->next;
            free(temp);
        }
        net->adj[i] = NULL;
    }
    
    net->num_users = 0;
    net->num_friendships = 0;
}

/* =============================================================================
 * FRIEND RECOMMENDATIONS
 * =============================================================================
 */

int compare_recommendations(const void *a, const void *b) {
    const Recommendation *ra = (const Recommendation *)a;
    const Recommendation *rb = (const Recommendation *)b;
    
    /* Sort by mutual count descending */
    return rb->mutual_count - ra->mutual_count;
}

void find_recommendations(SocialNetwork *net, int user_id) {
    printf("Recommendations for user %d:\n", user_id);
    
    /* Get user's friends into a set for quick lookup */
    bool is_friend[MAX_USERS] = {false};
    is_friend[user_id] = true;  /* Cannot recommend self */
    
    AdjNode *curr = net->adj[user_id];
    while (curr != NULL) {
        is_friend[curr->user_id] = true;
        curr = curr->next;
    }
    
    /* Count mutual friends for each non-friend user */
    int mutual_count[MAX_USERS] = {0};
    
    /* For each friend of user_id */
    curr = net->adj[user_id];
    while (curr != NULL) {
        int friend_id = curr->user_id;
        
        /* For each friend of friend_id (friend of friend) */
        AdjNode *fof = net->adj[friend_id];
        while (fof != NULL) {
            int fof_id = fof->user_id;
            
            /* If not already a friend, increment mutual count */
            if (!is_friend[fof_id]) {
                mutual_count[fof_id]++;
            }
            
            fof = fof->next;
        }
        
        curr = curr->next;
    }
    
    /* Build recommendations array (users with >= 2 mutual friends) */
    Recommendation recs[MAX_USERS];
    int rec_count = 0;
    
    for (int i = 0; i < net->num_users; i++) {
        if (mutual_count[i] >= 2) {
            recs[rec_count].user_id = i;
            recs[rec_count].mutual_count = mutual_count[i];
            rec_count++;
        }
    }
    
    if (rec_count == 0) {
        printf("  No recommendations available\n");
        return;
    }
    
    /* Sort by mutual count descending */
    qsort(recs, rec_count, sizeof(Recommendation), compare_recommendations);
    
    /* Print recommendations */
    for (int i = 0; i < rec_count; i++) {
        printf("  User %d (%d mutual friends)\n", 
               recs[i].user_id, recs[i].mutual_count);
    }
}

/* =============================================================================
 * DEGREES OF SEPARATION (BFS)
 * =============================================================================
 */

void find_distance(SocialNetwork *net, int source, int target) {
    int dist[MAX_USERS];
    int parent[MAX_USERS];
    
    /* Initialise */
    for (int i = 0; i < net->num_users; i++) {
        dist[i] = INFINITY_DIST;
        parent[i] = -1;
    }
    
    dist[source] = 0;
    
    Queue q;
    queue_init(&q);
    queue_enqueue(&q, source);
    
    /* BFS */
    while (!queue_is_empty(&q)) {
        int u = queue_dequeue(&q);
        
        if (u == target) {
            break;  /* Found target */
        }
        
        AdjNode *curr = net->adj[u];
        while (curr != NULL) {
            int v = curr->user_id;
            
            if (dist[v] == INFINITY_DIST) {
                dist[v] = dist[u] + 1;
                parent[v] = u;
                queue_enqueue(&q, v);
            }
            
            curr = curr->next;
        }
    }
    
    /* Print result */
    if (dist[target] == INFINITY_DIST) {
        printf("Users %d and %d are not connected\n", source, target);
        return;
    }
    
    printf("Distance from %d to %d: %d\n", source, target, dist[target]);
    
    /* Reconstruct path */
    int path[MAX_USERS];
    int path_len = 0;
    
    int current = target;
    while (current != -1) {
        path[path_len++] = current;
        current = parent[current];
    }
    
    /* Print path in correct order */
    printf("Path: ");
    for (int i = path_len - 1; i >= 0; i--) {
        printf("%d", path[i]);
        if (i > 0) {
            printf(" -> ");
        }
    }
    printf("\n");
}

/* =============================================================================
 * COMMUNITY DETECTION (Connected Components)
 * =============================================================================
 */

void bfs_component(SocialNetwork *net, int start, bool visited[], int *size) {
    Queue q;
    queue_init(&q);
    
    visited[start] = true;
    queue_enqueue(&q, start);
    *size = 0;
    
    while (!queue_is_empty(&q)) {
        int u = queue_dequeue(&q);
        (*size)++;
        
        AdjNode *curr = net->adj[u];
        while (curr != NULL) {
            int v = curr->user_id;
            
            if (!visited[v]) {
                visited[v] = true;
                queue_enqueue(&q, v);
            }
            
            curr = curr->next;
        }
    }
}

void find_communities(SocialNetwork *net) {
    bool visited[MAX_USERS] = {false};
    int community_sizes[MAX_USERS];
    int num_communities = 0;
    int largest_size = 0;
    int largest_id = 0;
    
    for (int i = 0; i < net->num_users; i++) {
        if (!visited[i]) {
            int size = 0;
            bfs_component(net, i, visited, &size);
            
            community_sizes[num_communities] = size;
            
            if (size > largest_size) {
                largest_size = size;
                largest_id = num_communities;
            }
            
            num_communities++;
        }
    }
    
    printf("Found %d communities:\n", num_communities);
    for (int i = 0; i < num_communities; i++) {
        printf("  Community %d: %d users", i + 1, community_sizes[i]);
        if (i == largest_id) {
            printf(" (largest)");
        }
        printf("\n");
    }
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    SocialNetwork net;
    int n, m;
    
    /* Read network size */
    if (scanf("%d %d", &n, &m) != 2) {
        fprintf(stderr, "Error: Failed to read network dimensions\n");
        return 1;
    }
    
    if (n <= 0 || n > MAX_USERS) {
        fprintf(stderr, "Error: Invalid number of users\n");
        return 1;
    }
    
    network_init(&net, n);
    
    /* Read friendships */
    for (int i = 0; i < m; i++) {
        int u, v;
        if (scanf("%d %d", &u, &v) != 2) {
            fprintf(stderr, "Error: Failed to read friendship %d\n", i + 1);
            network_destroy(&net);
            return 1;
        }
        
        if (u < 0 || u >= n || v < 0 || v >= n) {
            fprintf(stderr, "Error: Invalid user ID in friendship %d\n", i + 1);
            continue;
        }
        
        network_add_friendship(&net, u, v);
    }
    
    /* Read number of queries */
    int q;
    if (scanf("%d", &q) != 1) {
        fprintf(stderr, "Error: Failed to read number of queries\n");
        network_destroy(&net);
        return 1;
    }
    
    /* Process queries */
    char command[MAX_LINE];
    
    for (int i = 0; i < q; i++) {
        if (scanf("%s", command) != 1) {
            continue;
        }
        
        if (strcmp(command, "RECOMMEND") == 0) {
            int user_id;
            if (scanf("%d", &user_id) == 1 && user_id >= 0 && user_id < n) {
                find_recommendations(&net, user_id);
            }
        } else if (strcmp(command, "DISTANCE") == 0) {
            int u, v;
            if (scanf("%d %d", &u, &v) == 2 && 
                u >= 0 && u < n && v >= 0 && v < n) {
                find_distance(&net, u, v);
            }
        } else if (strcmp(command, "COMMUNITIES") == 0) {
            find_communities(&net);
        }
        
        printf("\n");
    }
    
    /* Clean up */
    network_destroy(&net);
    
    return 0;
}

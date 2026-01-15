/**
 * =============================================================================
 * HOMEWORK 1 SOLUTION: Log File Analyser
 * =============================================================================
 *
 * Analyses web server access logs in Common Log Format (CLF) and generates
 * a comprehensive statistics report.
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 * USAGE: ./homework1_sol access.log report.txt
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

#define MAX_LINE_LENGTH 4096
#define MAX_IP_LENGTH 64
#define MAX_METHOD_LENGTH 16
#define MAX_PATH_LENGTH 2048
#define MAX_ENTRIES 100000
#define MAX_UNIQUE_PATHS 1000
#define MAX_UNIQUE_IPS 1000

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

typedef struct {
    char ip[MAX_IP_LENGTH];
    char method[MAX_METHOD_LENGTH];
    char path[MAX_PATH_LENGTH];
    int status;
    long bytes;
} LogEntry;

typedef struct {
    char path[MAX_PATH_LENGTH];
    int count;
} PathCount;

typedef struct {
    char ip[MAX_IP_LENGTH];
    int count;
} IPCount;

typedef struct {
    int total_requests;
    int successful_parses;
    int failed_parses;
    long total_bytes;
    
    /* Method counts */
    int get_count;
    int post_count;
    int put_count;
    int delete_count;
    int other_method_count;
    
    /* Status code counts */
    int status_2xx;
    int status_3xx;
    int status_4xx;
    int status_5xx;
    
    /* Top paths and IPs */
    PathCount top_paths[MAX_UNIQUE_PATHS];
    int path_count;
    
    IPCount top_ips[MAX_UNIQUE_IPS];
    int ip_count;
} Statistics;

/* =============================================================================
 * PARSING FUNCTIONS
 * =============================================================================
 */

/**
 * Parse a single log line in Common Log Format.
 * Format: IP - - [DATE] "METHOD PATH PROTOCOL" STATUS SIZE
 */
int parse_log_line(const char *line, LogEntry *entry) {
    /* Extract IP address (first field) */
    int offset = 0;
    if (sscanf(line, "%63s%n", entry->ip, &offset) != 1) {
        return 0;
    }
    
    /* Skip to the quoted request section */
    const char *quote_start = strchr(line + offset, '"');
    if (quote_start == NULL) {
        return 0;
    }
    quote_start++;  /* Move past the opening quote */
    
    /* Parse method and path from quoted section */
    char protocol[32];
    if (sscanf(quote_start, "%15s %2047s %31[^\"]", 
               entry->method, entry->path, protocol) < 2) {
        return 0;
    }
    
    /* Find the closing quote and parse status and bytes */
    const char *quote_end = strchr(quote_start, '"');
    if (quote_end == NULL) {
        return 0;
    }
    
    if (sscanf(quote_end + 1, "%d %ld", &entry->status, &entry->bytes) != 2) {
        /* Try with just status (bytes might be "-") */
        if (sscanf(quote_end + 1, "%d", &entry->status) != 1) {
            return 0;
        }
        entry->bytes = 0;
    }
    
    return 1;
}

/* =============================================================================
 * STATISTICS FUNCTIONS
 * =============================================================================
 */

/**
 * Update path statistics.
 */
void update_path_stats(Statistics *stats, const char *path) {
    /* Check if path already exists */
    for (int i = 0; i < stats->path_count; i++) {
        if (strcmp(stats->top_paths[i].path, path) == 0) {
            stats->top_paths[i].count++;
            return;
        }
    }
    
    /* Add new path if space available */
    if (stats->path_count < MAX_UNIQUE_PATHS) {
        strncpy(stats->top_paths[stats->path_count].path, path, MAX_PATH_LENGTH - 1);
        stats->top_paths[stats->path_count].path[MAX_PATH_LENGTH - 1] = '\0';
        stats->top_paths[stats->path_count].count = 1;
        stats->path_count++;
    }
}

/**
 * Update IP statistics.
 */
void update_ip_stats(Statistics *stats, const char *ip) {
    /* Check if IP already exists */
    for (int i = 0; i < stats->ip_count; i++) {
        if (strcmp(stats->top_ips[i].ip, ip) == 0) {
            stats->top_ips[i].count++;
            return;
        }
    }
    
    /* Add new IP if space available */
    if (stats->ip_count < MAX_UNIQUE_IPS) {
        strncpy(stats->top_ips[stats->ip_count].ip, ip, MAX_IP_LENGTH - 1);
        stats->top_ips[stats->ip_count].ip[MAX_IP_LENGTH - 1] = '\0';
        stats->top_ips[stats->ip_count].count = 1;
        stats->ip_count++;
    }
}

/**
 * Process a parsed log entry.
 */
void process_entry(Statistics *stats, const LogEntry *entry) {
    stats->total_requests++;
    stats->total_bytes += entry->bytes;
    
    /* Count by method */
    if (strcmp(entry->method, "GET") == 0) {
        stats->get_count++;
    } else if (strcmp(entry->method, "POST") == 0) {
        stats->post_count++;
    } else if (strcmp(entry->method, "PUT") == 0) {
        stats->put_count++;
    } else if (strcmp(entry->method, "DELETE") == 0) {
        stats->delete_count++;
    } else {
        stats->other_method_count++;
    }
    
    /* Count by status code */
    int status_class = entry->status / 100;
    switch (status_class) {
        case 2: stats->status_2xx++; break;
        case 3: stats->status_3xx++; break;
        case 4: stats->status_4xx++; break;
        case 5: stats->status_5xx++; break;
    }
    
    /* Update path and IP statistics */
    update_path_stats(stats, entry->path);
    update_ip_stats(stats, entry->ip);
}

/* Comparison function for sorting */
int compare_counts(const void *a, const void *b) {
    return ((PathCount *)b)->count - ((PathCount *)a)->count;
}

int compare_ip_counts(const void *a, const void *b) {
    return ((IPCount *)b)->count - ((IPCount *)a)->count;
}

/**
 * Read and process log file.
 */
int process_log_file(const char *filename, Statistics *stats) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening log file");
        return -1;
    }
    
    /* Initialise statistics */
    memset(stats, 0, sizeof(Statistics));
    
    char line[MAX_LINE_LENGTH];
    LogEntry entry;
    int line_num = 0;
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        line_num++;
        
        /* Skip empty lines */
        if (line[0] == '\n' || line[0] == '\0') {
            continue;
        }
        
        if (parse_log_line(line, &entry)) {
            stats->successful_parses++;
            process_entry(stats, &entry);
        } else {
            stats->failed_parses++;
            fprintf(stderr, "Warning: Failed to parse line %d\n", line_num);
        }
    }
    
    fclose(fp);
    
    /* Sort paths and IPs by count */
    qsort(stats->top_paths, stats->path_count, sizeof(PathCount), compare_counts);
    qsort(stats->top_ips, stats->ip_count, sizeof(IPCount), compare_ip_counts);
    
    return 0;
}

/**
 * Write report to file.
 */
int write_report(const char *filename, const Statistics *stats) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error creating report file");
        return -1;
    }
    
    fprintf(fp, "═══════════════════════════════════════════════════════════════\n");
    fprintf(fp, "                    WEB SERVER LOG ANALYSIS                     \n");
    fprintf(fp, "═══════════════════════════════════════════════════════════════\n\n");
    
    /* Summary */
    fprintf(fp, "Summary\n");
    fprintf(fp, "───────────────────────────────────────────────────────────────\n");
    fprintf(fp, "Total requests:     %d\n", stats->total_requests);
    fprintf(fp, "Successful parses:  %d\n", stats->successful_parses);
    fprintf(fp, "Failed parses:      %d\n", stats->failed_parses);
    fprintf(fp, "Total bytes:        %ld\n\n", stats->total_bytes);
    
    /* Requests by Method */
    fprintf(fp, "Requests by Method\n");
    fprintf(fp, "───────────────────────────────────────────────────────────────\n");
    int total = stats->total_requests > 0 ? stats->total_requests : 1;
    fprintf(fp, "GET:        %6d (%5.1f%%)\n", stats->get_count, 
            100.0 * stats->get_count / total);
    fprintf(fp, "POST:       %6d (%5.1f%%)\n", stats->post_count,
            100.0 * stats->post_count / total);
    fprintf(fp, "PUT:        %6d (%5.1f%%)\n", stats->put_count,
            100.0 * stats->put_count / total);
    fprintf(fp, "DELETE:     %6d (%5.1f%%)\n", stats->delete_count,
            100.0 * stats->delete_count / total);
    fprintf(fp, "Other:      %6d (%5.1f%%)\n\n", stats->other_method_count,
            100.0 * stats->other_method_count / total);
    
    /* Status Code Distribution */
    fprintf(fp, "Status Code Distribution\n");
    fprintf(fp, "───────────────────────────────────────────────────────────────\n");
    fprintf(fp, "2xx (Success):      %6d (%5.1f%%)\n", stats->status_2xx,
            100.0 * stats->status_2xx / total);
    fprintf(fp, "3xx (Redirect):     %6d (%5.1f%%)\n", stats->status_3xx,
            100.0 * stats->status_3xx / total);
    fprintf(fp, "4xx (Client Error): %6d (%5.1f%%)\n", stats->status_4xx,
            100.0 * stats->status_4xx / total);
    fprintf(fp, "5xx (Server Error): %6d (%5.1f%%)\n\n", stats->status_5xx,
            100.0 * stats->status_5xx / total);
    
    /* Top 5 Requested Paths */
    fprintf(fp, "Top 5 Requested Paths\n");
    fprintf(fp, "───────────────────────────────────────────────────────────────\n");
    int limit = stats->path_count < 5 ? stats->path_count : 5;
    for (int i = 0; i < limit; i++) {
        fprintf(fp, "%d. %-40s (%d requests)\n", i + 1,
                stats->top_paths[i].path, stats->top_paths[i].count);
    }
    fprintf(fp, "\n");
    
    /* Top 5 Active IPs */
    fprintf(fp, "Top 5 Active IPs\n");
    fprintf(fp, "───────────────────────────────────────────────────────────────\n");
    limit = stats->ip_count < 5 ? stats->ip_count : 5;
    for (int i = 0; i < limit; i++) {
        fprintf(fp, "%d. %-20s (%d requests)\n", i + 1,
                stats->top_ips[i].ip, stats->top_ips[i].count);
    }
    
    fprintf(fp, "\n═══════════════════════════════════════════════════════════════\n");
    
    fclose(fp);
    return 0;
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_log> <output_report>\n", argv[0]);
        return 1;
    }
    
    const char *input_file = argv[1];
    const char *output_file = argv[2];
    
    printf("Log File Analyser\n");
    printf("=================\n\n");
    printf("Processing: %s\n", input_file);
    
    Statistics stats;
    
    if (process_log_file(input_file, &stats) != 0) {
        return 1;
    }
    
    printf("Parsed %d entries (%d failed)\n", 
           stats.successful_parses, stats.failed_parses);
    
    if (write_report(output_file, &stats) != 0) {
        return 1;
    }
    
    printf("Report written to: %s\n", output_file);
    
    return 0;
}

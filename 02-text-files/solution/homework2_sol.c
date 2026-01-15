/**
 * =============================================================================
 * HOMEWORK 2 SOLUTION: Configuration File Manager
 * =============================================================================
 *
 * Reads, modifies and writes INI-style configuration files whilst preserving
 * comments and structure.
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 * USAGE: ./homework2_sol config.ini <command> [args...]
 *
 * Commands:
 *   get <section> <key>         - Get a value
 *   set <section> <key> <value> - Set a value
 *   delete <section> <key>      - Delete a key
 *   list-sections               - List all sections
 *   list-keys <section>         - List keys in a section
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

#define MAX_LINE_LENGTH 1024
#define MAX_KEY_LENGTH 128
#define MAX_VALUE_LENGTH 512
#define MAX_SECTION_LENGTH 64
#define MAX_LINES 10000
#define MAX_ENTRIES 1000

/* Line types */
#define LINE_EMPTY    0
#define LINE_COMMENT  1
#define LINE_SECTION  2
#define LINE_KEYVALUE 3
#define LINE_UNKNOWN  4

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

typedef struct {
    int type;
    char raw[MAX_LINE_LENGTH];       /* Original line content */
    char section[MAX_SECTION_LENGTH]; /* Section name (if section line) */
    char key[MAX_KEY_LENGTH];        /* Key (if key-value line) */
    char value[MAX_VALUE_LENGTH];    /* Value (if key-value line) */
} ConfigLine;

typedef struct {
    ConfigLine lines[MAX_LINES];
    int line_count;
    char current_section[MAX_SECTION_LENGTH];
} ConfigFile;

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Trim whitespace from both ends of a string.
 */
void trim(char *str) {
    if (str == NULL || *str == '\0') return;
    
    /* Trim leading */
    char *start = str;
    while (*start && isspace((unsigned char)*start)) start++;
    
    if (*start == '\0') {
        *str = '\0';
        return;
    }
    
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
    
    /* Trim trailing */
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
}

/**
 * Case-insensitive string comparison.
 */
int strcasecmp_local(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (diff != 0) return diff;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

/* =============================================================================
 * PARSING FUNCTIONS
 * =============================================================================
 */

/**
 * Parse a single line and determine its type.
 */
int parse_line(const char *raw, ConfigLine *line) {
    /* Store raw content */
    strncpy(line->raw, raw, MAX_LINE_LENGTH - 1);
    line->raw[MAX_LINE_LENGTH - 1] = '\0';
    
    /* Remove trailing newline from raw */
    line->raw[strcspn(line->raw, "\r\n")] = '\0';
    
    /* Make a working copy for parsing */
    char work[MAX_LINE_LENGTH];
    strncpy(work, line->raw, sizeof(work) - 1);
    work[sizeof(work) - 1] = '\0';
    trim(work);
    
    /* Check for empty line */
    if (work[0] == '\0') {
        line->type = LINE_EMPTY;
        return LINE_EMPTY;
    }
    
    /* Check for comment */
    if (work[0] == ';' || work[0] == '#') {
        line->type = LINE_COMMENT;
        return LINE_COMMENT;
    }
    
    /* Check for section header */
    if (work[0] == '[') {
        char *end = strchr(work, ']');
        if (end != NULL) {
            *end = '\0';
            strncpy(line->section, work + 1, MAX_SECTION_LENGTH - 1);
            line->section[MAX_SECTION_LENGTH - 1] = '\0';
            trim(line->section);
            line->type = LINE_SECTION;
            return LINE_SECTION;
        }
    }
    
    /* Check for key-value pair */
    char *equals = strchr(work, '=');
    if (equals != NULL) {
        *equals = '\0';
        
        strncpy(line->key, work, MAX_KEY_LENGTH - 1);
        line->key[MAX_KEY_LENGTH - 1] = '\0';
        trim(line->key);
        
        strncpy(line->value, equals + 1, MAX_VALUE_LENGTH - 1);
        line->value[MAX_VALUE_LENGTH - 1] = '\0';
        trim(line->value);
        
        line->type = LINE_KEYVALUE;
        return LINE_KEYVALUE;
    }
    
    line->type = LINE_UNKNOWN;
    return LINE_UNKNOWN;
}

/**
 * Load configuration file.
 */
int load_config(const char *filename, ConfigFile *config) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening config file");
        return -1;
    }
    
    config->line_count = 0;
    config->current_section[0] = '\0';
    
    char line[MAX_LINE_LENGTH];
    
    while (fgets(line, sizeof(line), fp) != NULL && config->line_count < MAX_LINES) {
        ConfigLine *cl = &config->lines[config->line_count];
        
        int type = parse_line(line, cl);
        
        /* Track current section for key-value lines */
        if (type == LINE_SECTION) {
            strncpy(config->current_section, cl->section, MAX_SECTION_LENGTH - 1);
        } else if (type == LINE_KEYVALUE) {
            strncpy(cl->section, config->current_section, MAX_SECTION_LENGTH - 1);
        }
        
        config->line_count++;
    }
    
    fclose(fp);
    return 0;
}

/**
 * Save configuration file.
 */
int save_config(const char *filename, const ConfigFile *config) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error writing config file");
        return -1;
    }
    
    for (int i = 0; i < config->line_count; i++) {
        const ConfigLine *cl = &config->lines[i];
        
        switch (cl->type) {
            case LINE_EMPTY:
                fprintf(fp, "\n");
                break;
            case LINE_COMMENT:
                fprintf(fp, "%s\n", cl->raw);
                break;
            case LINE_SECTION:
                fprintf(fp, "[%s]\n", cl->section);
                break;
            case LINE_KEYVALUE:
                fprintf(fp, "%s = %s\n", cl->key, cl->value);
                break;
            default:
                fprintf(fp, "%s\n", cl->raw);
        }
    }
    
    fclose(fp);
    return 0;
}

/* =============================================================================
 * OPERATION FUNCTIONS
 * =============================================================================
 */

/**
 * Get a value by section and key.
 */
const char *config_get(const ConfigFile *config, const char *section, const char *key) {
    for (int i = 0; i < config->line_count; i++) {
        const ConfigLine *cl = &config->lines[i];
        
        if (cl->type == LINE_KEYVALUE &&
            strcasecmp_local(cl->section, section) == 0 &&
            strcasecmp_local(cl->key, key) == 0) {
            return cl->value;
        }
    }
    return NULL;
}

/**
 * Set a value. Updates existing or creates new.
 */
int config_set(ConfigFile *config, const char *section, const char *key, const char *value) {
    /* First, try to update existing key */
    for (int i = 0; i < config->line_count; i++) {
        ConfigLine *cl = &config->lines[i];
        
        if (cl->type == LINE_KEYVALUE &&
            strcasecmp_local(cl->section, section) == 0 &&
            strcasecmp_local(cl->key, key) == 0) {
            strncpy(cl->value, value, MAX_VALUE_LENGTH - 1);
            cl->value[MAX_VALUE_LENGTH - 1] = '\0';
            return 0;
        }
    }
    
    /* Find or create the section */
    int section_end = -1;
    int section_found = 0;
    
    for (int i = 0; i < config->line_count; i++) {
        const ConfigLine *cl = &config->lines[i];
        
        if (cl->type == LINE_SECTION) {
            if (strcasecmp_local(cl->section, section) == 0) {
                section_found = 1;
                section_end = i;
            } else if (section_found) {
                /* Reached next section */
                break;
            }
        } else if (section_found && cl->type == LINE_KEYVALUE) {
            section_end = i;
        }
    }
    
    /* Create new section if not found */
    if (!section_found) {
        if (config->line_count >= MAX_LINES - 2) {
            fprintf(stderr, "Error: Config file is full\n");
            return -1;
        }
        
        /* Add empty line before new section */
        config->lines[config->line_count].type = LINE_EMPTY;
        config->lines[config->line_count].raw[0] = '\0';
        config->line_count++;
        
        /* Add section header */
        section_end = config->line_count;
        config->lines[section_end].type = LINE_SECTION;
        strncpy(config->lines[section_end].section, section, MAX_SECTION_LENGTH - 1);
        config->line_count++;
    }
    
    /* Insert new key-value after section_end */
    if (config->line_count >= MAX_LINES) {
        fprintf(stderr, "Error: Config file is full\n");
        return -1;
    }
    
    /* Shift lines if needed */
    int insert_pos = section_end + 1;
    if (insert_pos < config->line_count) {
        memmove(&config->lines[insert_pos + 1], 
                &config->lines[insert_pos],
                (config->line_count - insert_pos) * sizeof(ConfigLine));
    }
    
    /* Insert new line */
    ConfigLine *new_line = &config->lines[insert_pos];
    new_line->type = LINE_KEYVALUE;
    strncpy(new_line->section, section, MAX_SECTION_LENGTH - 1);
    strncpy(new_line->key, key, MAX_KEY_LENGTH - 1);
    strncpy(new_line->value, value, MAX_VALUE_LENGTH - 1);
    config->line_count++;
    
    return 0;
}

/**
 * Delete a key.
 */
int config_delete(ConfigFile *config, const char *section, const char *key) {
    for (int i = 0; i < config->line_count; i++) {
        ConfigLine *cl = &config->lines[i];
        
        if (cl->type == LINE_KEYVALUE &&
            strcasecmp_local(cl->section, section) == 0 &&
            strcasecmp_local(cl->key, key) == 0) {
            /* Shift remaining lines */
            memmove(&config->lines[i], 
                    &config->lines[i + 1],
                    (config->line_count - i - 1) * sizeof(ConfigLine));
            config->line_count--;
            return 0;
        }
    }
    
    fprintf(stderr, "Key not found: [%s] %s\n", section, key);
    return -1;
}

/**
 * List all sections.
 */
void config_list_sections(const ConfigFile *config) {
    printf("Sections:\n");
    
    for (int i = 0; i < config->line_count; i++) {
        if (config->lines[i].type == LINE_SECTION) {
            printf("  [%s]\n", config->lines[i].section);
        }
    }
}

/**
 * List all keys in a section.
 */
void config_list_keys(const ConfigFile *config, const char *section) {
    printf("Keys in [%s]:\n", section);
    
    int found = 0;
    for (int i = 0; i < config->line_count; i++) {
        const ConfigLine *cl = &config->lines[i];
        
        if (cl->type == LINE_KEYVALUE &&
            strcasecmp_local(cl->section, section) == 0) {
            printf("  %s = %s\n", cl->key, cl->value);
            found = 1;
        }
    }
    
    if (!found) {
        printf("  (no keys found)\n");
    }
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s <config_file> <command> [args...]\n\n", prog_name);
    fprintf(stderr, "Commands:\n");
    fprintf(stderr, "  get <section> <key>           - Get a value\n");
    fprintf(stderr, "  set <section> <key> <value>   - Set a value\n");
    fprintf(stderr, "  delete <section> <key>        - Delete a key\n");
    fprintf(stderr, "  list-sections                 - List all sections\n");
    fprintf(stderr, "  list-keys <section>           - List keys in a section\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }
    
    const char *config_file = argv[1];
    const char *command = argv[2];
    
    ConfigFile config;
    
    /* Load configuration */
    if (load_config(config_file, &config) != 0) {
        return 1;
    }
    
    /* Process command */
    if (strcmp(command, "get") == 0) {
        if (argc != 5) {
            fprintf(stderr, "Usage: %s <file> get <section> <key>\n", argv[0]);
            return 1;
        }
        
        const char *value = config_get(&config, argv[3], argv[4]);
        if (value != NULL) {
            printf("%s\n", value);
        } else {
            fprintf(stderr, "Key not found: [%s] %s\n", argv[3], argv[4]);
            return 1;
        }
        
    } else if (strcmp(command, "set") == 0) {
        if (argc != 6) {
            fprintf(stderr, "Usage: %s <file> set <section> <key> <value>\n", argv[0]);
            return 1;
        }
        
        if (config_set(&config, argv[3], argv[4], argv[5]) == 0) {
            if (save_config(config_file, &config) == 0) {
                printf("Set [%s] %s = %s\n", argv[3], argv[4], argv[5]);
            }
        } else {
            return 1;
        }
        
    } else if (strcmp(command, "delete") == 0) {
        if (argc != 5) {
            fprintf(stderr, "Usage: %s <file> delete <section> <key>\n", argv[0]);
            return 1;
        }
        
        if (config_delete(&config, argv[3], argv[4]) == 0) {
            if (save_config(config_file, &config) == 0) {
                printf("Deleted [%s] %s\n", argv[3], argv[4]);
            }
        } else {
            return 1;
        }
        
    } else if (strcmp(command, "list-sections") == 0) {
        config_list_sections(&config);
        
    } else if (strcmp(command, "list-keys") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Usage: %s <file> list-keys <section>\n", argv[0]);
            return 1;
        }
        config_list_keys(&config, argv[3]);
        
    } else {
        fprintf(stderr, "Unknown command: %s\n", command);
        print_usage(argv[0]);
        return 1;
    }
    
    return 0;
}

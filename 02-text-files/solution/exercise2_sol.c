/**
 * =============================================================================
 * EXERCISE 2 SOLUTION: CSV Transformer
 * =============================================================================
 *
 * INSTRUCTOR NOTES:
 *   This is the reference solution for Exercise 2. Students should arrive at
 *   a similar implementation by completing the TODO markers in exercise2.c.
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c
 * USAGE: ./exercise2_sol
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

#define MAX_LINE_LENGTH 512
#define MAX_FIELD_LENGTH 128
#define MAX_PRODUCTS 200
#define MAX_FIELDS 10
#define INPUT_FILE "data/products.csv"

/* Output format options */
#define FORMAT_CSV   0
#define FORMAT_TSV   1
#define FORMAT_FIXED 2

/* Sort options */
#define SORT_NONE    0
#define SORT_PRICE   1
#define SORT_STOCK   2
#define SORT_NAME    3

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

typedef struct {
    char id[MAX_FIELD_LENGTH];
    char name[MAX_FIELD_LENGTH];
    char category[MAX_FIELD_LENGTH];
    double price;
    int stock;
} Product;

typedef struct {
    int total_lines;
    int successful_parses;
    int failed_parses;
    int empty_lines;
} ParseStats;

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Remove leading and trailing whitespace from a string.
 */
void trim_whitespace(char *str) {
    if (str == NULL || *str == '\0') {
        return;
    }
    
    /* Find first non-whitespace character */
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    
    /* All whitespace */
    if (*start == '\0') {
        *str = '\0';
        return;
    }
    
    /* Move to beginning if needed */
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
    
    /* Find last non-whitespace character */
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    
    /* Terminate after last non-whitespace */
    *(end + 1) = '\0';
}

/**
 * Split a CSV line into fields.
 * NOTE: Modifies the input line!
 */
int parse_csv_line(char *line, char *fields[], int max_fields) {
    /* Remove trailing newline */
    line[strcspn(line, "\r\n")] = '\0';
    
    int count = 0;
    char *token = strtok(line, ",");
    
    while (token != NULL && count < max_fields) {
        trim_whitespace(token);
        fields[count++] = token;
        token = strtok(NULL, ",");
    }
    
    return count;
}

/**
 * Convert parsed fields into a Product structure.
 */
int parse_product(char *fields[], int field_count, Product *p) {
    if (field_count < 5) {
        return 0;
    }
    
    /* Copy string fields safely */
    strncpy(p->id, fields[0], MAX_FIELD_LENGTH - 1);
    p->id[MAX_FIELD_LENGTH - 1] = '\0';
    
    strncpy(p->name, fields[1], MAX_FIELD_LENGTH - 1);
    p->name[MAX_FIELD_LENGTH - 1] = '\0';
    
    strncpy(p->category, fields[2], MAX_FIELD_LENGTH - 1);
    p->category[MAX_FIELD_LENGTH - 1] = '\0';
    
    /* Convert numeric fields */
    char *endptr;
    p->price = strtod(fields[3], &endptr);
    if (*endptr != '\0' && !isspace((unsigned char)*endptr)) {
        return 0;  /* Invalid price */
    }
    
    p->stock = (int)strtol(fields[4], &endptr, 10);
    if (*endptr != '\0' && !isspace((unsigned char)*endptr)) {
        return 0;  /* Invalid stock */
    }
    
    /* Validate ranges */
    if (p->price < 0 || p->stock < 0) {
        return 0;
    }
    
    return 1;
}

/**
 * Read products from CSV file.
 */
int read_products_from_csv(const char *filename, Product products[], 
                           int max_products, ParseStats *stats) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening CSV file");
        return -1;
    }
    
    /* Initialise statistics */
    stats->total_lines = 0;
    stats->successful_parses = 0;
    stats->failed_parses = 0;
    stats->empty_lines = 0;
    
    char line[MAX_LINE_LENGTH];
    char *fields[MAX_FIELDS];
    int count = 0;
    int is_header = 1;
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        stats->total_lines++;
        
        /* Check for empty line */
        char *check = line;
        while (*check && isspace((unsigned char)*check)) check++;
        if (*check == '\0') {
            stats->empty_lines++;
            continue;
        }
        
        /* Skip header row */
        if (is_header) {
            is_header = 0;
            continue;
        }
        
        /* Make a copy for parsing (strtok modifies string) */
        char line_copy[MAX_LINE_LENGTH];
        strncpy(line_copy, line, sizeof(line_copy) - 1);
        line_copy[sizeof(line_copy) - 1] = '\0';
        
        int field_count = parse_csv_line(line_copy, fields, MAX_FIELDS);
        
        if (count < max_products && parse_product(fields, field_count, &products[count])) {
            count++;
            stats->successful_parses++;
        } else {
            stats->failed_parses++;
            fprintf(stderr, "Warning: Failed to parse line %d\n", stats->total_lines);
        }
    }
    
    fclose(fp);
    return count;
}

/* =============================================================================
 * OUTPUT FUNCTIONS
 * =============================================================================
 */

void print_product_csv(const Product *p, FILE *out) {
    fprintf(out, "%s,%s,%s,%.2f,%d\n",
            p->id, p->name, p->category, p->price, p->stock);
}

void print_product_tsv(const Product *p, FILE *out) {
    fprintf(out, "%s\t%s\t%s\t%.2f\t%d\n",
            p->id, p->name, p->category, p->price, p->stock);
}

void print_product_fixed(const Product *p, FILE *out) {
    fprintf(out, "%-8s %-25s %-15s %12.2f %8d\n",
            p->id, p->name, p->category, p->price, p->stock);
}

void print_products(const Product products[], int count, int format, FILE *out) {
    if (format == FORMAT_FIXED) {
        fprintf(out, "%-8s %-25s %-15s %12s %8s\n",
                "ID", "Name", "Category", "Price", "Stock");
        fprintf(out, "%-8s %-25s %-15s %12s %8s\n",
                "--------", "-------------------------", "---------------",
                "------------", "--------");
    }
    
    for (int i = 0; i < count; i++) {
        switch (format) {
            case FORMAT_CSV:
                print_product_csv(&products[i], out);
                break;
            case FORMAT_TSV:
                print_product_tsv(&products[i], out);
                break;
            case FORMAT_FIXED:
                print_product_fixed(&products[i], out);
                break;
        }
    }
}

/* =============================================================================
 * DATA PROCESSING FUNCTIONS
 * =============================================================================
 */

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

/**
 * Filter products by category.
 */
int filter_by_category(const Product products[], int count, 
                       const char *category, Product filtered[], int max_filtered) {
    int filtered_count = 0;
    
    for (int i = 0; i < count && filtered_count < max_filtered; i++) {
        if (strcasecmp_local(products[i].category, category) == 0) {
            filtered[filtered_count++] = products[i];
        }
    }
    
    return filtered_count;
}

/* Comparison functions for qsort */
int compare_by_price(const void *a, const void *b) {
    const Product *pa = (const Product *)a;
    const Product *pb = (const Product *)b;
    
    if (pa->price < pb->price) return -1;
    if (pa->price > pb->price) return 1;
    return 0;
}

int compare_by_stock(const void *a, const void *b) {
    const Product *pa = (const Product *)a;
    const Product *pb = (const Product *)b;
    
    /* Descending order */
    return pb->stock - pa->stock;
}

int compare_by_name(const void *a, const void *b) {
    const Product *pa = (const Product *)a;
    const Product *pb = (const Product *)b;
    
    return strcmp(pa->name, pb->name);
}

/**
 * Sort products array.
 */
void sort_products(Product products[], int count, int sort_by) {
    if (sort_by == SORT_NONE || count <= 1) {
        return;
    }
    
    int (*cmp)(const void *, const void *) = NULL;
    
    switch (sort_by) {
        case SORT_PRICE:
            cmp = compare_by_price;
            break;
        case SORT_STOCK:
            cmp = compare_by_stock;
            break;
        case SORT_NAME:
            cmp = compare_by_name;
            break;
        default:
            return;
    }
    
    qsort(products, count, sizeof(Product), cmp);
}

/**
 * Print statistics about products.
 */
void print_statistics(const Product products[], int count) {
    if (count == 0) {
        printf("\n=== Statistics ===\n");
        printf("No products to analyse.\n");
        return;
    }
    
    double total_value = 0.0;
    double total_price = 0.0;
    int max_price_idx = 0;
    int min_stock_idx = 0;
    
    /* Track unique categories */
    char categories[MAX_PRODUCTS][MAX_FIELD_LENGTH];
    int category_count = 0;
    
    for (int i = 0; i < count; i++) {
        total_value += products[i].price * products[i].stock;
        total_price += products[i].price;
        
        if (products[i].price > products[max_price_idx].price) {
            max_price_idx = i;
        }
        
        if (products[i].stock < products[min_stock_idx].stock) {
            min_stock_idx = i;
        }
        
        /* Check if category is unique */
        int found = 0;
        for (int j = 0; j < category_count; j++) {
            if (strcasecmp_local(categories[j], products[i].category) == 0) {
                found = 1;
                break;
            }
        }
        if (!found && category_count < MAX_PRODUCTS) {
            strncpy(categories[category_count++], products[i].category, MAX_FIELD_LENGTH - 1);
        }
    }
    
    printf("\n=== Statistics ===\n");
    printf("Total products:        %d\n", count);
    printf("Unique categories:     %d\n", category_count);
    printf("Total inventory value: £%.2f\n", total_value);
    printf("Average price:         £%.2f\n", total_price / count);
    printf("Most expensive:        %s (£%.2f)\n", 
           products[max_price_idx].name, products[max_price_idx].price);
    printf("Lowest stock:          %s (%d units)\n",
           products[min_stock_idx].name, products[min_stock_idx].stock);
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║            EXERCISE 2: CSV Transformer                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Product products[MAX_PRODUCTS];
    Product filtered[MAX_PRODUCTS];
    ParseStats stats;
    
    /* Read products from CSV */
    int count = read_products_from_csv(INPUT_FILE, products, MAX_PRODUCTS, &stats);
    
    if (count < 0) {
        fprintf(stderr, "Failed to read CSV file.\n");
        return 1;
    }
    
    printf("Loaded %d products (%d failed, %d empty lines)\n\n",
           stats.successful_parses, stats.failed_parses, stats.empty_lines);
    
    /* Display all products in fixed-width format */
    printf("=== All Products (Fixed Width) ===\n");
    print_products(products, count, FORMAT_FIXED, stdout);
    
    /* Sort by price and display */
    printf("\n=== Sorted by Price (Ascending) ===\n");
    sort_products(products, count, SORT_PRICE);
    print_products(products, count, FORMAT_FIXED, stdout);
    
    /* Filter by category */
    printf("\n=== Filtered: Electronics ===\n");
    int filtered_count = filter_by_category(products, count, "Electronics", 
                                            filtered, MAX_PRODUCTS);
    print_products(filtered, filtered_count, FORMAT_FIXED, stdout);
    printf("(%d products in Electronics category)\n", filtered_count);
    
    /* Print statistics */
    print_statistics(products, count);
    
    printf("\nExercise completed successfully.\n");
    return 0;
}

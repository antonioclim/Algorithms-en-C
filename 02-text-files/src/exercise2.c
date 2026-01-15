/**
 * =============================================================================
 * EXERCISE 2: CSV Transformer
 * =============================================================================
 *
 * OBJECTIVE:
 *   Create a utility that reads CSV files, performs data transformations
 *   and outputs in multiple formats. This exercise practises string parsing
 *   with strtok, dynamic data handling and formatted output.
 *
 * INPUT FILE FORMAT (data/products.csv):
 *   CSV with header row: ID,Name,Category,Price,Stock
 *   Example: 001,Laptop,Electronics,2499.99,15
 *
 * REQUIREMENTS:
 *   1. Parse CSV file with proper handling of the header row
 *   2. Support filtering by category
 *   3. Support sorting by price or stock
 *   4. Output in CSV, TSV (tab-separated) or fixed-width format
 *   5. Handle malformed input gracefully with error messages
 *
 * EXPECTED OUTPUT:
 *   - Parsed records displayed in selected format
 *   - Statistics about the data
 *   - Error messages for malformed lines
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 * USAGE: ./exercise2
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

/**
 * TODO 1: Complete the Product structure
 *
 * The structure should contain:
 *   - id: character array for product ID (use MAX_FIELD_LENGTH)
 *   - name: character array for product name
 *   - category: character array for category
 *   - price: double for the price
 *   - stock: integer for stock quantity
 *
 * Some fields are provided; complete the rest if needed.
 */
typedef struct {
    char id[MAX_FIELD_LENGTH];
    char name[MAX_FIELD_LENGTH];
    /* TODO 1: Verify all fields are present */
    char category[MAX_FIELD_LENGTH];
    double price;
    int stock;
} Product;


/**
 * Structure to hold parsing statistics
 */
typedef struct {
    int total_lines;
    int successful_parses;
    int failed_parses;
    int empty_lines;
} ParseStats;

/* =============================================================================
 * FUNCTION DECLARATIONS
 * =============================================================================
 */

void trim_whitespace(char *str);
int parse_csv_line(char *line, char *fields[], int max_fields);
int parse_product(char *fields[], int field_count, Product *p);
int read_products_from_csv(const char *filename, Product products[], 
                           int max_products, ParseStats *stats);
void print_product_csv(const Product *p, FILE *out);
void print_product_tsv(const Product *p, FILE *out);
void print_product_fixed(const Product *p, FILE *out);
void print_products(const Product products[], int count, int format, FILE *out);
int filter_by_category(const Product products[], int count, 
                       const char *category, Product filtered[], int max_filtered);
void sort_products(Product products[], int count, int sort_by);
void print_statistics(const Product products[], int count);

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * TODO 2: Implement trim_whitespace
 *
 * Remove leading and trailing whitespace from a string.
 * Modifies the string in place.
 *
 * @param str The string to trim
 *
 * Steps:
 *   1. Find the first non-whitespace character (use isspace())
 *   2. Move string contents to start if needed (use memmove)
 *   3. Find the last non-whitespace character
 *   4. Place null terminator after it
 *
 * Hint: Handle edge case of all-whitespace string
 */
void trim_whitespace(char *str) {
    /* YOUR CODE HERE */
}

/**
 * TODO 3: Implement parse_csv_line
 *
 * Split a CSV line into individual fields.
 * NOTE: This function modifies the input line!
 *
 * @param line The CSV line to parse (will be modified)
 * @param fields Array of pointers to store field starts
 * @param max_fields Maximum number of fields to parse
 * @return Number of fields found
 *
 * Steps:
 *   1. Remove trailing newline from line
 *   2. Use strtok with "," delimiter
 *   3. Store each token pointer in fields array
 *   4. Trim whitespace from each field
 *   5. Return the count of fields
 *
 * Hint: First call strtok(line, ","), then strtok(NULL, ",")
 */
int parse_csv_line(char *line, char *fields[], int max_fields) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}

/**
 * TODO 4: Implement parse_product
 *
 * Convert parsed fields into a Product structure.
 *
 * @param fields Array of field strings
 * @param field_count Number of fields
 * @param p Pointer to Product structure to populate
 * @return 1 on success, 0 on error
 *
 * Steps:
 *   1. Check that field_count is at least 5
 *   2. Copy fields[0] to p->id (use strncpy for safety)
 *   3. Copy fields[1] to p->name
 *   4. Copy fields[2] to p->category
 *   5. Convert fields[3] to double for p->price (use atof or strtod)
 *   6. Convert fields[4] to int for p->stock (use atoi or strtol)
 *   7. Validate: price >= 0, stock >= 0
 *   8. Return 1 for success
 *
 * Hint: strncpy(dest, src, n) copies at most n characters
 */
int parse_product(char *fields[], int field_count, Product *p) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}

/**
 * TODO 5: Implement read_products_from_csv
 *
 * Read all products from a CSV file, tracking statistics.
 *
 * @param filename Path to CSV file
 * @param products Array to store products
 * @param max_products Maximum products to read
 * @param stats Pointer to ParseStats to populate
 * @return Number of products read, or -1 on file error
 *
 * Steps:
 *   1. Open file for reading
 *   2. Initialise stats to zeros
 *   3. Read first line (header) - skip it
 *   4. Loop: read lines with fgets
 *      a. Increment stats->total_lines
 *      b. Skip empty lines (increment stats->empty_lines)
 *      c. Parse line and convert to product
 *      d. On success: store product, increment stats->successful_parses
 *      e. On failure: print warning with line number, increment stats->failed_parses
 *   5. Close file
 *   6. Return count of products
 */
int read_products_from_csv(const char *filename, Product products[], 
                           int max_products, ParseStats *stats) {
    /* YOUR CODE HERE */
    return -1;  /* Replace this */
}

/* =============================================================================
 * OUTPUT FUNCTIONS
 * =============================================================================
 */

/**
 * TODO 6: Implement print_product_csv
 *
 * Print a product in CSV format.
 *
 * @param p Pointer to Product
 * @param out Output file stream
 *
 * Format: id,name,category,price,stock
 * Example: 001,Laptop,Electronics,2499.99,15
 */
void print_product_csv(const Product *p, FILE *out) {
    /* YOUR CODE HERE */
}

/**
 * TODO 7: Implement print_product_tsv
 *
 * Print a product in tab-separated format.
 *
 * @param p Pointer to Product
 * @param out Output file stream
 *
 * Format: id\tname\tcategory\tprice\tstock
 */
void print_product_tsv(const Product *p, FILE *out) {
    /* YOUR CODE HERE */
}

/**
 * TODO 8: Implement print_product_fixed
 *
 * Print a product in fixed-width column format.
 *
 * @param p Pointer to Product
 * @param out Output file stream
 *
 * Use widths: ID=8, Name=25, Category=15, Price=12, Stock=8
 * Example: 001      Laptop                   Electronics     2499.99      15
 */
void print_product_fixed(const Product *p, FILE *out) {
    /* YOUR CODE HERE */
}

/**
 * Print all products in the specified format
 */
void print_products(const Product products[], int count, int format, FILE *out) {
    /* Print header for fixed format */
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
 * TODO 9: Implement filter_by_category
 *
 * Filter products by category name.
 *
 * @param products Source array of products
 * @param count Number of products in source
 * @param category Category to filter by (case-insensitive)
 * @param filtered Destination array for filtered products
 * @param max_filtered Maximum products in filtered array
 * @return Number of products matching the filter
 *
 * Steps:
 *   1. Loop through all products
 *   2. Compare category (case-insensitive - use strcasecmp or manual comparison)
 *   3. If match, copy product to filtered array
 *   4. Return count of matches
 *
 * Hint: For case-insensitive compare, convert both to lowercase
 */
int filter_by_category(const Product products[], int count, 
                       const char *category, Product filtered[], int max_filtered) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}

/**
 * TODO 10: Implement comparison functions for qsort
 *
 * Create comparison functions for sorting by price and stock.
 * 
 * Signature: int compare_func(const void *a, const void *b)
 * Return: negative if a < b, zero if equal, positive if a > b
 */

/* Compare by price (ascending) */
int compare_by_price(const void *a, const void *b) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}

/* Compare by stock (descending - highest stock first) */
int compare_by_stock(const void *a, const void *b) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}

/* Compare by name (alphabetical) */
int compare_by_name(const void *a, const void *b) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}

/**
 * TODO 11: Implement sort_products
 *
 * Sort products array using qsort.
 *
 * @param products Array of products to sort (modified in place)
 * @param count Number of products
 * @param sort_by Sort option (SORT_PRICE, SORT_STOCK, SORT_NAME)
 *
 * Steps:
 *   1. Check if sort_by is SORT_NONE (return early)
 *   2. Select appropriate comparison function
 *   3. Call qsort with the comparison function
 *
 * Hint: qsort(array, count, sizeof(element), compare_func)
 */
void sort_products(Product products[], int count, int sort_by) {
    /* YOUR CODE HERE */
}

/**
 * TODO 12: Implement print_statistics
 *
 * Calculate and print statistics about the products.
 *
 * @param products Array of products
 * @param count Number of products
 *
 * Statistics to calculate:
 *   - Total number of products
 *   - Total inventory value (sum of price * stock for each product)
 *   - Average price
 *   - Most expensive product
 *   - Product with lowest stock
 *   - Number of categories (count unique categories)
 */
void print_statistics(const Product products[], int count) {
    printf("\n=== Statistics ===\n");
    /* YOUR CODE HERE */
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
    
    /**
     * TODO 13: Complete the main programme
     *
     * Steps:
     *   1. Read products from CSV file
     *   2. Print parsing statistics
     *   3. Print all products in fixed-width format
     *   4. Print products sorted by price
     *   5. Filter and print only "Electronics" category
     *   6. Print overall statistics
     *
     * Example flow:
     *   - Read data
     *   - Print: "Loaded X products (Y failed, Z empty lines)"
     *   - Print table of all products
     *   - Sort by price, print again
     *   - Filter Electronics, print filtered
     *   - Print statistics
     */
    
    /* YOUR CODE HERE */
    
    printf("\nExercise completed.\n");
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Handle quoted fields in CSV (e.g., "Smith, John" as a single field)
 *    This requires a more sophisticated parser than strtok.
 *
 * 2. Add command-line arguments for:
 *    - Input filename
 *    - Output format (--csv, --tsv, --fixed)
 *    - Sort option (--sort=price, --sort=stock)
 *    - Filter (--category=Electronics)
 *
 * 3. Implement output to file instead of stdout
 *
 * 4. Add support for reading multiple CSV files and merging data
 *
 * 5. Implement a search function (find products by name substring)
 *
 * =============================================================================
 */

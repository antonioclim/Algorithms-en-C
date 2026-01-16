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

/* =============================================================================
 * COMMAND-LINE INTERFACE AND PORTABILITY UTILITIES
 * =============================================================================
 *
 * The exercises are distributed with default input data under data/. For
 * deterministic regression tests and for controlled demonstration experiments
 * it is convenient to allow an explicit input path and a selectable category
 * filter.
 *
 * Supported options:
 *   --input <path>     Override the CSV input path
 *   --category <name>  Select the category used for filtering
 *   --test             Emit deterministic stdout suitable for snapshot diffs
 */

static void print_usage(const char *argv0) {
    fprintf(stderr,
            "Usage: %s [--input <file>] [--category <name>] [--test]\n",
            argv0);
}

static int strcasecmp_local(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (diff != 0) {
            return diff;
        }
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}


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
    if (str == NULL || *str == '\0') {
        return;
    }

    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    if (*start == '\0') {
        *str = '\0';
        return;
    }

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }

    *(end + 1) = '\0';
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
    if (line == NULL || fields == NULL || max_fields <= 0) {
        return 0;
    }

    /* Remove trailing newline(s). */
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
    if (p == NULL || fields == NULL || field_count < 5) {
        return 0;
    }

    strncpy(p->id, fields[0], MAX_FIELD_LENGTH - 1);
    p->id[MAX_FIELD_LENGTH - 1] = '\0';
    strncpy(p->name, fields[1], MAX_FIELD_LENGTH - 1);
    p->name[MAX_FIELD_LENGTH - 1] = '\0';
    strncpy(p->category, fields[2], MAX_FIELD_LENGTH - 1);
    p->category[MAX_FIELD_LENGTH - 1] = '\0';

    char *endptr = NULL;
    p->price = strtod(fields[3], &endptr);
    if (endptr == fields[3] || (*endptr != '\0' && !isspace((unsigned char)*endptr))) {
        return 0;
    }

    endptr = NULL;
    long stock_long = strtol(fields[4], &endptr, 10);
    if (endptr == fields[4] || (*endptr != '\0' && !isspace((unsigned char)*endptr))) {
        return 0;
    }
    p->stock = (int)stock_long;

    if (p->price < 0.0 || p->stock < 0) {
        return 0;
    }

    return 1;
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
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening CSV file");
        return -1;
    }

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

        char *check = line;
        while (*check && isspace((unsigned char)*check)) {
            check++;
        }
        if (*check == '\0') {
            stats->empty_lines++;
            continue;
        }

        if (is_header) {
            is_header = 0;
            continue;
        }

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
    fprintf(out, "%s,%s,%s,%.2f,%d\n",
            p->id, p->name, p->category, p->price, p->stock);
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
    fprintf(out, "%s\t%s\t%s\t%.2f\t%d\n",
            p->id, p->name, p->category, p->price, p->stock);
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
    fprintf(out, "%-8s %-25s %-15s %12.2f %8d\n",
            p->id, p->name, p->category, p->price, p->stock);
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
    if (products == NULL || filtered == NULL || category == NULL || max_filtered <= 0) {
        return 0;
    }

    int filtered_count = 0;
    for (int i = 0; i < count && filtered_count < max_filtered; i++) {
        if (strcasecmp_local(products[i].category, category) == 0) {
            filtered[filtered_count++] = products[i];
        }
    }

    return filtered_count;
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
    const Product *pa = (const Product *)a;
    const Product *pb = (const Product *)b;
    if (pa->price < pb->price) return -1;
    if (pa->price > pb->price) return 1;
    return 0;
}

/* Compare by stock (descending - highest stock first) */
int compare_by_stock(const void *a, const void *b) {
    const Product *pa = (const Product *)a;
    const Product *pb = (const Product *)b;
    /* Descending order: higher stock first. */
    if (pa->stock < pb->stock) return 1;
    if (pa->stock > pb->stock) return -1;
    return 0;
}

/* Compare by name (alphabetical) */
int compare_by_name(const void *a, const void *b) {
    const Product *pa = (const Product *)a;
    const Product *pb = (const Product *)b;
    return strcmp(pa->name, pb->name);
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
    if (products == NULL || count <= 1 || sort_by == SORT_NONE) {
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

    qsort(products, (size_t)count, sizeof(Product), cmp);
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
    if (products == NULL || count <= 0) {
        printf("No products to analyse.\n");
        return;
    }

    double total_value = 0.0;
    double total_price = 0.0;
    int max_price_idx = 0;
    int min_stock_idx = 0;

    char categories[MAX_PRODUCTS][MAX_FIELD_LENGTH];
    int category_count = 0;

    for (int i = 0; i < count; i++) {
        total_value += products[i].price * (double)products[i].stock;
        total_price += products[i].price;

        if (products[i].price > products[max_price_idx].price) {
            max_price_idx = i;
        }

        if (products[i].stock < products[min_stock_idx].stock) {
            min_stock_idx = i;
        }

        int found = 0;
        for (int j = 0; j < category_count; j++) {
            if (strcasecmp_local(categories[j], products[i].category) == 0) {
                found = 1;
                break;
            }
        }
        if (!found && category_count < MAX_PRODUCTS) {
            strncpy(categories[category_count], products[i].category, MAX_FIELD_LENGTH - 1);
            categories[category_count][MAX_FIELD_LENGTH - 1] = '\0';
            category_count++;
        }
    }

    printf("Total products:        %d\n", count);
    printf("Unique categories:     %d\n", category_count);
    printf("Total inventory value: £%.2f\n", total_value);
    printf("Average price:         £%.2f\n", total_price / (double)count);
    printf("Most expensive:        %s (£%.2f)\n",
           products[max_price_idx].name, products[max_price_idx].price);
    printf("Lowest stock:          %s (%d units)\n",
           products[min_stock_idx].name, products[min_stock_idx].stock);
}

static void print_statistics_terse(const Product products[], int count) {
    printf("\n=== Statistics ===\n");
    if (products == NULL || count <= 0) {
        printf("Total products: 0\n");
        printf("Total inventory value: 0.00\n");
        printf("Average price: 0.00\n");
        return;
    }

    double total_value = 0.0;
    double total_price = 0.0;
    for (int i = 0; i < count; i++) {
        total_value += products[i].price * (double)products[i].stock;
        total_price += products[i].price;
    }

    printf("Total products: %d\n", count);
    printf("Total inventory value: %.2f\n", total_value);
    printf("Average price: %.2f\n", total_price / (double)count);
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

static void print_products_fixed_rows(const Product products[], int count, FILE *out) {
    for (int i = 0; i < count; i++) {
        print_product_fixed(&products[i], out);
    }
}

int main(int argc, char **argv) {
    const char *input_path = INPUT_FILE;
    const char *category_filter = "Electronics";
    int test_mode = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            input_path = argv[++i];
        } else if (strcmp(argv[i], "--category") == 0 && i + 1 < argc) {
            category_filter = argv[++i];
        } else if (strcmp(argv[i], "--test") == 0) {
            test_mode = 1;
        } else {
            print_usage(argv[0]);
            return 2;
        }
    }

    if (!test_mode) {
        printf("╔═══════════════════════════════════════════════════════════════╗\n");
        printf("║            EXERCISE 2: CSV Transformer                        ║\n");
        printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    }

    Product products[MAX_PRODUCTS];
    Product filtered[MAX_PRODUCTS];
    ParseStats stats;

    int count = read_products_from_csv(input_path, products, MAX_PRODUCTS, &stats);
    if (count < 0) {
        fprintf(stderr, "Failed to read CSV file.\n");
        return 1;
    }

    printf("Loaded %d products (%d failed, %d empty lines)\n\n",
           stats.successful_parses, stats.failed_parses, stats.empty_lines);

    /* Section 1: all products. */
    printf("=== All Products (Fixed Width) ===\n");
    print_products(products, count, FORMAT_FIXED, stdout);

    /* Section 2: sorted by price. Preserve original ordering for independent filtering. */
    Product products_sorted[MAX_PRODUCTS];
    memcpy(products_sorted, products, (size_t)count * sizeof(Product));
    sort_products(products_sorted, count, SORT_PRICE);
    printf("\n=== Sorted by Price ===\n");
    if (test_mode) {
        print_products_fixed_rows(products_sorted, count, stdout);
    } else {
        print_products(products_sorted, count, FORMAT_FIXED, stdout);
    }

    /* Section 3: filtered by category, based on the original list. */
    int filtered_count = filter_by_category(products, count, category_filter, filtered, MAX_PRODUCTS);
    printf("\n=== Filtered: %s ===\n", category_filter);
    if (test_mode) {
        print_products_fixed_rows(filtered, filtered_count, stdout);
    } else {
        print_products(filtered, filtered_count, FORMAT_FIXED, stdout);
        printf("(%d products in %s category)\n", filtered_count, category_filter);
    }

    /* Section 4: statistics. */
    if (test_mode) {
        print_statistics_terse(products, count);
    } else {
        print_statistics(products, count);
        printf("\nExercise completed successfully.\n");
    }

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

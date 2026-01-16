/**
 * =============================================================================
 * EXERCISE 2: Binary File Index System
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement an indexed binary file system with random access capabilities
 *   for a product inventory database.
 *
 * REQUIREMENTS:
 *   1. Create a Product structure with code, name, price and quantity
 *   2. Implement a separate index file mapping product codes to file offsets
 *   3. Implement add_product() that updates both data and index files
 *   4. Implement search_product() using the index for O(1) lookup
 *   5. Implement update_price() using random access
 *   6. Implement delete_product() using a tombstone marker
 *   7. Implement rebuild_index() to regenerate index from data file
 *   8. Track statistics: total, active and deleted records
 *   9. Implement binary search on sorted index for O(log n) lookup
 *   10. Display formatted output with proper alignment
 *
 * EXAMPLE INPUT:
 *   4
 *   101 Laptop 999.99 50
 *   102 Mouse 29.99 200
 *   103 Keyboard 79.99 150
 *   104 Monitor 349.99 75
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

#define MAX_NAME_LENGTH     40
#define DATA_FILE           "products.bin"
#define INDEX_FILE          "products.idx"
#define TOMBSTONE_MARKER    -1      /* Marks deleted records */

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * TODO 1: Define the Product structure
 *
 * The structure should contain:
 *   - deleted:  int32_t flag (0 = active, TOMBSTONE_MARKER = deleted)
 *   - code:     int32_t unique product code
 *   - name:     char array of MAX_NAME_LENGTH
 *   - price:    double for product price
 *   - quantity: int32_t for stock quantity
 *
 * Note: The 'deleted' field enables tombstone-based deletion
 */
typedef struct {
    int32_t deleted;                /* 0 = active, -1 = deleted */
    int32_t code;                   /* Unique product code */
    char    name[MAX_NAME_LENGTH];  /* Product name */
    double  price;                  /* Price in currency units */
    int32_t quantity;               /* Stock quantity */
} Product;

/**
 * TODO 2: Define the IndexEntry structure
 *
 * Maps a product code to its byte offset in the data file.
 * Used for O(log n) binary search lookups.
 */
typedef struct {
    int32_t code;                   /* Product code (search key) */
    long    offset;                 /* Byte offset in data file */
} IndexEntry;

/**
 * Database statistics structure
 */
typedef struct {
    size_t total_records;
    size_t active_records;
    size_t deleted_records;
    long   file_size;
} DatabaseStats;

/* =============================================================================
 * FUNCTION PROTOTYPES
 * =============================================================================
 */

/* Core operations */
int add_product(const Product *product);
Product *search_product(int32_t code);
int update_price(int32_t code, double new_price);
int delete_product(int32_t code);

/* Index operations */
int rebuild_index(void);
int load_index(IndexEntry **index, size_t *count);
int save_index(const IndexEntry *index, size_t count);

/* Utility functions */
DatabaseStats get_stats(void);
void print_product(const Product *p);
void print_all_products(void);
void print_stats(const DatabaseStats *stats);

/* =============================================================================
 * UTILITY FUNCTIONS (PROVIDED)
 * =============================================================================
 */

/**
 * Initialise a Product structure safely
 */
static void init_product(Product *p, int32_t code, const char *name,
                         double price, int32_t quantity) {
    memset(p, 0, sizeof(Product));
    p->deleted = 0;  /* Active record */
    p->code = code;
    strncpy(p->name, name, MAX_NAME_LENGTH - 1);
    p->name[MAX_NAME_LENGTH - 1] = '\0';
    p->price = price;
    p->quantity = quantity;
}

/**
 * Comparison function for qsort and bsearch on IndexEntry
 */
static int compare_index(const void *a, const void *b) {
    const IndexEntry *ia = (const IndexEntry *)a;
    const IndexEntry *ib = (const IndexEntry *)b;
    if (ia->code < ib->code) {
        return -1;
    }
    if (ia->code > ib->code) {
        return 1;
    }
    return 0;
}

/**
 * Replace underscores with spaces
 */
static void replace_underscores(char *str) {
    for (char *p = str; *p; p++) {
        if (*p == '_') *p = ' ';
    }
}

/* =============================================================================
 * FUNCTIONS TO IMPLEMENT
 * =============================================================================
 */

/**
 * TODO 3: Implement add_product()
 *
 * Adds a new product to the database and updates the index.
 *
 * @param product  Pointer to the product to add
 * @return         0 on success, -1 on failure
 *
 * Steps:
 *   1. Open data file in append binary mode ("ab")
 *   2. Get current file position (this will be the offset for index)
 *   3. Write the product record
 *   4. Close the data file
 *   5. Load existing index (or create empty one)
 *   6. Add new entry to index with code and offset
 *   7. Sort the index by code for binary search
 *   8. Save the updated index
 *   9. Return 0 on success
 *
 * Hint: Use ftell() BEFORE writing to get the offset
 */
int add_product(const Product *product) {
    if (product == NULL) {
        return -1;
    }

    /* Step 1-4: append to the data file and capture the record offset */
    FILE *data_fp = fopen(DATA_FILE, "ab");
    if (data_fp == NULL) {
        return -1;
    }

    long offset = ftell(data_fp);
    if (offset < 0) {
        fclose(data_fp);
        return -1;
    }

    if (fwrite(product, sizeof(Product), 1, data_fp) != 1) {
        fclose(data_fp);
        return -1;
    }

    fclose(data_fp);

    /* Step 5-8: load, extend, sort and persist the index */
    IndexEntry *index = NULL;
    size_t count = 0;

    if (load_index(&index, &count) != 0) {
        free(index);
        return -1;
    }

    IndexEntry *grown = realloc(index, (count + 1) * sizeof(IndexEntry));
    if (grown == NULL) {
        free(index);
        return -1;
    }
    index = grown;

    index[count].code = product->code;
    index[count].offset = offset;
    count++;

    if (count > 1) {
        qsort(index, count, sizeof(IndexEntry), compare_index);
    }

    int result = save_index(index, count);
    free(index);
    return result;
}

/**
 * TODO 4: Implement load_index()
 *
 * Loads the index from the index file into memory.
 *
 * @param index  Output: pointer to index array (caller must free)
 * @param count  Output: number of entries in the index
 * @return       0 on success, -1 on failure
 *
 * Steps:
 *   1. Open index file in read binary mode
 *   2. If file doesn't exist, set *count = 0, *index = NULL, return 0
 *   3. Calculate number of entries from file size
 *   4. Allocate memory for index entries
 *   5. Read all entries
 *   6. Close file and return
 */
int load_index(IndexEntry **index, size_t *count) {
    if (index == NULL || count == NULL) {
        return -1;
    }

    *index = NULL;
    *count = 0;

    FILE *fp = fopen(INDEX_FILE, "rb");
    if (fp == NULL) {
        return 0;  /* missing index is not an error */
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return -1;
    }
    long file_size = ftell(fp);
    if (file_size < 0) {
        fclose(fp);
        return -1;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return -1;
    }

    if (file_size == 0) {
        fclose(fp);
        return 0;
    }

    size_t entries = (size_t)file_size / sizeof(IndexEntry);
    if (entries == 0) {
        fclose(fp);
        return 0;
    }

    IndexEntry *buffer = malloc(entries * sizeof(IndexEntry));
    if (buffer == NULL) {
        fclose(fp);
        return -1;
    }

    size_t read = fread(buffer, sizeof(IndexEntry), entries, fp);
    fclose(fp);

    if (read != entries) {
        free(buffer);
        return -1;
    }

    *index = buffer;
    *count = entries;
    return 0;
}

/**
 * TODO 5: Implement save_index()
 *
 * Saves the index array to the index file.
 *
 * @param index  Array of index entries
 * @param count  Number of entries to save
 * @return       0 on success, -1 on failure
 *
 * Steps:
 *   1. Open index file in write binary mode ("wb")
 *   2. Write all index entries
 *   3. Close file
 */
int save_index(const IndexEntry *index, size_t count) {
    FILE *fp = fopen(INDEX_FILE, "wb");
    if (fp == NULL) {
        return -1;
    }

    if (count > 0) {
        if (index == NULL) {
            fclose(fp);
            return -1;
        }

        size_t written = fwrite(index, sizeof(IndexEntry), count, fp);
        if (written != count) {
            fclose(fp);
            return -1;
        }
    }

    fclose(fp);
    return 0;
}

/**
 * TODO 6: Implement search_product()
 *
 * Searches for a product using the index for fast lookup.
 *
 * @param code  Product code to search for
 * @return      Pointer to static Product (or NULL if not found)
 *
 * Steps:
 *   1. Load the index into memory
 *   2. Use bsearch() to find the entry with matching code
 *   3. If not found, return NULL
 *   4. Open data file
 *   5. Seek to the offset from the index entry
 *   6. Read the product record
 *   7. Check if record is deleted (tombstone)
 *   8. Return pointer to static Product, or NULL if deleted
 *
 * Hint: Use a static variable to return the product
 */
Product *search_product(int32_t code) {
    static Product found_product;

    IndexEntry *index = NULL;
    size_t index_count = 0;

    if (load_index(&index, &index_count) != 0 || index_count == 0) {
        free(index);
        return NULL;
    }

    IndexEntry key;
    key.code = code;
    key.offset = 0;

    IndexEntry *found = bsearch(&key, index, index_count, sizeof(IndexEntry), compare_index);
    if (found == NULL) {
        free(index);
        return NULL;
    }

    long offset = found->offset;
    free(index);

    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        return NULL;
    }

    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        return NULL;
    }

    if (fread(&found_product, sizeof(Product), 1, fp) != 1) {
        fclose(fp);
        return NULL;
    }

    fclose(fp);

    if (found_product.deleted == TOMBSTONE_MARKER) {
        return NULL;
    }

    return &found_product;
}

/**
 * TODO 7: Implement update_price()
 *
 * Updates the price of a product using random access.
 *
 * @param code       Product code to update
 * @param new_price  New price value
 * @return           0 on success, -1 if not found or error
 *
 * Steps:
 *   1. Load index and find the entry using bsearch
 *   2. If not found, return -1
 *   3. Open data file in read+write mode ("r+b")
 *   4. Seek to the record offset
 *   5. Read the existing product
 *   6. Check if deleted - if so, return -1
 *   7. Update the price field
 *   8. Seek back to the same position
 *   9. Write the updated product
 *   10. Flush and close file
 */
int update_price(int32_t code, double new_price) {
    IndexEntry *index = NULL;
    size_t index_count = 0;

    if (load_index(&index, &index_count) != 0 || index_count == 0) {
        free(index);
        return -1;
    }

    IndexEntry key;
    key.code = code;
    key.offset = 0;

    IndexEntry *found = bsearch(&key, index, index_count, sizeof(IndexEntry), compare_index);
    if (found == NULL) {
        free(index);
        return -1;
    }

    long offset = found->offset;
    free(index);

    FILE *fp = fopen(DATA_FILE, "r+b");
    if (fp == NULL) {
        return -1;
    }

    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        return -1;
    }

    Product p;
    if (fread(&p, sizeof(Product), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    if (p.deleted == TOMBSTONE_MARKER) {
        fclose(fp);
        return -1;
    }

    p.price = new_price;

    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        return -1;
    }

    if (fwrite(&p, sizeof(Product), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    fflush(fp);
    fclose(fp);
    return 0;
}

/**
 * TODO 8: Implement delete_product()
 *
 * Marks a product as deleted using a tombstone marker.
 * Does NOT physically remove the record.
 *
 * @param code  Product code to delete
 * @return      0 on success, -1 if not found or error
 *
 * Steps:
 *   1. Load index and find the entry
 *   2. If not found, return -1
 *   3. Open data file in read+write mode
 *   4. Seek to the record
 *   5. Read the product
 *   6. Set deleted = TOMBSTONE_MARKER
 *   7. Seek back and write
 *   8. Close file
 *   9. Do NOT remove from index (for audit trail)
 */
int delete_product(int32_t code) {
    IndexEntry *index = NULL;
    size_t index_count = 0;

    if (load_index(&index, &index_count) != 0 || index_count == 0) {
        free(index);
        return -1;
    }

    IndexEntry key;
    key.code = code;
    key.offset = 0;

    IndexEntry *found = bsearch(&key, index, index_count, sizeof(IndexEntry), compare_index);
    if (found == NULL) {
        free(index);
        return -1;
    }

    long offset = found->offset;
    free(index);

    FILE *fp = fopen(DATA_FILE, "r+b");
    if (fp == NULL) {
        return -1;
    }

    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        return -1;
    }

    Product p;
    if (fread(&p, sizeof(Product), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    p.deleted = TOMBSTONE_MARKER;

    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        return -1;
    }

    if (fwrite(&p, sizeof(Product), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    fflush(fp);
    fclose(fp);
    return 0;
}

/**
 * TODO 9: Implement rebuild_index()
 *
 * Regenerates the index by scanning the entire data file.
 * This recovers from index corruption or creates initial index.
 *
 * @return  0 on success, -1 on failure
 *
 * Steps:
 *   1. Open data file for reading
 *   2. If doesn't exist, remove index file and return 0
 *   3. Count total records (file_size / sizeof(Product))
 *   4. Allocate index array
 *   5. Read each product and add to index if not deleted
 *   6. Sort the index
 *   7. Save the index
 *   8. Free memory and return
 *
 * Note: Only include active (non-deleted) records in the new index
 */
int rebuild_index(void) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        remove(INDEX_FILE);
        return 0;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return -1;
    }
    long file_size = ftell(fp);
    if (file_size < 0) {
        fclose(fp);
        return -1;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return -1;
    }

    if (file_size == 0) {
        fclose(fp);
        remove(INDEX_FILE);
        return 0;
    }

    size_t total_records = (size_t)file_size / sizeof(Product);
    if (total_records == 0) {
        fclose(fp);
        remove(INDEX_FILE);
        return 0;
    }

    IndexEntry *index = malloc(total_records * sizeof(IndexEntry));
    if (index == NULL) {
        fclose(fp);
        return -1;
    }

    size_t count = 0;
    Product p;

    for (size_t i = 0; i < total_records; i++) {
        long offset = ftell(fp);
        if (offset < 0) {
            break;
        }

        if (fread(&p, sizeof(Product), 1, fp) != 1) {
            break;
        }

        if (p.deleted != TOMBSTONE_MARKER) {
            index[count].code = p.code;
            index[count].offset = offset;
            count++;
        }
    }

    fclose(fp);

    if (count > 1) {
        qsort(index, count, sizeof(IndexEntry), compare_index);
    }

    int result = save_index(index, count);
    free(index);
    return result;
}

/**
 * TODO 10: Implement get_stats()
 *
 * Calculates database statistics by scanning the data file.
 *
 * @return  DatabaseStats structure with counts
 *
 * Steps:
 *   1. Open data file for reading
 *   2. Get file size
 *   3. Count total records
 *   4. Read each record and count active vs deleted
 *   5. Return the stats structure
 */
DatabaseStats get_stats(void) {
    DatabaseStats stats = {0, 0, 0, 0};

    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        return stats;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return stats;
    }
    stats.file_size = ftell(fp);
    if (stats.file_size < 0) {
        fclose(fp);
        stats.file_size = 0;
        return stats;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return stats;
    }

    stats.total_records = (size_t)stats.file_size / sizeof(Product);

    Product p;
    while (fread(&p, sizeof(Product), 1, fp) == 1) {
        if (p.deleted == TOMBSTONE_MARKER) {
            stats.deleted_records++;
        } else {
            stats.active_records++;
        }
    }

    fclose(fp);
    return stats;
}

/**
 * TODO 11: Implement print_product()
 *
 * Prints a single product in table row format.
 *
 * @param p  Pointer to the product to print
 *
 * Format: │ code │ name (left-aligned) │ price (right) │ qty │ status │
 */
void print_product(const Product *p) {
    const char *status = (p->deleted == TOMBSTONE_MARKER) ? "DELETED" : "Active";
    
    /* YOUR CODE HERE */
    printf("│ %5d │ %-25s │ %10.2f │ %6d │ %-7s │\n",
           p->code, p->name, p->price, p->quantity, status);
}

/**
 * TODO 12: Implement print_all_products()
 *
 * Reads and prints all products from the data file.
 * Shows both active and deleted records.
 */
void print_all_products(void) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("[INFO] No products in database\n");
        return;
    }
    
    /* YOUR CODE HERE */
    /* Print table header */
    printf("┌───────┬───────────────────────────┬────────────┬────────┬─────────┐\n");
    printf("│ Code  │ Name                      │      Price │    Qty │ Status  │\n");
    printf("├───────┼───────────────────────────┼────────────┼────────┼─────────┤\n");
    
    /* Read and print each product */
    Product p;
    while (fread(&p, sizeof(Product), 1, fp) == 1) {
        print_product(&p);
    }
    
    printf("└───────┴───────────────────────────┴────────────┴────────┴─────────┘\n");
    
    fclose(fp);
}

/**
 * Print database statistics
 */
void print_stats(const DatabaseStats *stats) {
    printf("\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                    DATABASE STATISTICS                         ║\n");
    printf("╠════════════════════════════════════════════════════════════════╣\n");
    printf("║  Total Records:%12zu%36s║\n", stats->total_records, "");
    printf("║  Active Records:%11zu%36s║\n", stats->active_records, "");
    printf("║  Deleted Records:%10zu%36s║\n", stats->deleted_records, "");
    printf("║  File Size:%16ld B%34s║\n", stats->file_size, "");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
}

/* =============================================================================
 * MAIN PROGRAM (PROVIDED - DO NOT MODIFY)
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║              INDEXED PRODUCT DATABASE SYSTEM                   ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
    
    /* Clean start */
    remove(DATA_FILE);
    remove(INDEX_FILE);
    
    /* Read number of products */
    int n;
    printf("Enter number of products: ");
    if (scanf("%d", &n) != 1 || n < 0) {
        printf("[ERROR] Invalid input\n");
        return 1;
    }
    
    /* Read and add products */
    printf("\n[INFO] Adding %d products to database...\n", n);
    
    for (int i = 0; i < n; i++) {
        int32_t code, quantity;
        char name[MAX_NAME_LENGTH];
        double price;
        
        if (scanf("%d %39s %lf %d", &code, name, &price, &quantity) != 4) {
            printf("[ERROR] Invalid input for product %d\n", i + 1);
            continue;
        }
        
        replace_underscores(name);
        
        Product p;
        init_product(&p, code, name, price, quantity);
        
        if (add_product(&p) == 0) {
            printf("[OK] Product %d added: %s\n", code, name);
        } else {
            printf("[ERROR] Failed to add product %d\n", code);
        }
    }
    
    /* Display all products */
    printf("\n[INFO] Current database contents:\n");
    print_all_products();
    
    /* Test search */
    if (n > 0) {
        printf("\n[INFO] Testing index-based search...\n");
        
        /* Rebuild index to ensure it's correct */
        rebuild_index();
        
        /* Search for first product */
        printf("Enter product code to search: ");
        int32_t search_code;
        if (scanf("%d", &search_code) == 1) {
            Product *found = search_product(search_code);
            if (found != NULL) {
                printf("[OK] Found: %s - $%.2f (%d in stock)\n",
                       found->name, found->price, found->quantity);
            } else {
                printf("[INFO] Product %d not found\n", search_code);
            }
        }
        
        /* Test price update */
        printf("\nEnter code and new price to update (or 0 0 to skip): ");
        int32_t update_code;
        double new_price;
        if (scanf("%d %lf", &update_code, &new_price) == 2 && update_code != 0) {
            if (update_price(update_code, new_price) == 0) {
                printf("[OK] Price updated for product %d\n", update_code);
            } else {
                printf("[ERROR] Failed to update price\n");
            }
        }
        
        /* Test deletion */
        printf("\nEnter code to delete (or 0 to skip): ");
        int32_t delete_code;
        if (scanf("%d", &delete_code) == 1 && delete_code != 0) {
            if (delete_product(delete_code) == 0) {
                printf("[OK] Product %d marked as deleted\n", delete_code);
            } else {
                printf("[ERROR] Failed to delete product\n");
            }
        }
    }
    
    /* Final display */
    printf("\n[INFO] Final database state:\n");
    print_all_products();
    
    /* Statistics */
    DatabaseStats stats = get_stats();
    print_stats(&stats);
    
    printf("\n[INFO] Operations complete\n\n");
    
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Implement compact_database() that creates a new file without deleted
 *    records and replaces the original.
 *
 * 2. Add transaction logging: write operations to a separate log file
 *    that can be replayed for crash recovery.
 *
 * 3. Implement a B-tree index structure for even faster lookups.
 *
 * 4. Add support for multiple indices (by code, by name, by price).
 *
 * 5. Implement optimistic locking using a version field to detect
 *    concurrent modifications.
 *
 * =============================================================================
 */

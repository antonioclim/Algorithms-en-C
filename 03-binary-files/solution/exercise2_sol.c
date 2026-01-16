/**
 * =============================================================================
 * EXERCISE 2: Binary File Index System - SOLUTION
 * =============================================================================
 *
 * INSTRUCTOR COPY - Contains complete implementation
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c
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
#define TOMBSTONE_MARKER    -1

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct {
    int32_t deleted;
    int32_t code;
    char    name[MAX_NAME_LENGTH];
    double  price;
    int32_t quantity;
} Product;

typedef struct {
    int32_t code;
    long    offset;
} IndexEntry;

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

int add_product(const Product *product);
Product *search_product(int32_t code);
int update_price(int32_t code, double new_price);
int delete_product(int32_t code);
int rebuild_index(void);
int load_index(IndexEntry **index, size_t *count);
int save_index(const IndexEntry *index, size_t count);
DatabaseStats get_stats(void);
void print_product(const Product *p);
void print_all_products(void);
void print_stats(const DatabaseStats *stats);

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

static void init_product(Product *p, int32_t code, const char *name,
                         double price, int32_t quantity) {
    memset(p, 0, sizeof(Product));
    p->deleted = 0;
    p->code = code;
    strncpy(p->name, name, MAX_NAME_LENGTH - 1);
    p->name[MAX_NAME_LENGTH - 1] = '\0';
    p->price = price;
    p->quantity = quantity;
}

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

static void replace_underscores(char *str) {
    for (char *p = str; *p; p++) {
        if (*p == '_') *p = ' ';
    }
}

/* =============================================================================
 * SOLUTION IMPLEMENTATIONS
 * =============================================================================
 */

/**
 * SOLUTION: load_index()
 */
int load_index(IndexEntry **index, size_t *count) {
    *index = NULL;
    *count = 0;
    
    FILE *fp = fopen(INDEX_FILE, "rb");
    if (fp == NULL) {
        return 0;  /* No index file yet - not an error */
    }
    
    /* Get file size */
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    if (file_size == 0) {
        fclose(fp);
        return 0;
    }
    
    /* Calculate entry count */
    *count = file_size / sizeof(IndexEntry);
    
    /* Allocate memory */
    *index = malloc(*count * sizeof(IndexEntry));
    if (*index == NULL) {
        fclose(fp);
        *count = 0;
        return -1;
    }
    
    /* Read entries */
    size_t read = fread(*index, sizeof(IndexEntry), *count, fp);
    fclose(fp);
    
    if (read != *count) {
        free(*index);
        *index = NULL;
        *count = 0;
        return -1;
    }
    
    return 0;
}

/**
 * SOLUTION: save_index()
 */
int save_index(const IndexEntry *index, size_t count) {
    FILE *fp = fopen(INDEX_FILE, "wb");
    if (fp == NULL) {
        return -1;
    }
    
    if (count > 0 && index != NULL) {
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
 * SOLUTION: add_product()
 */
int add_product(const Product *product) {
    /* Open data file in append mode */
    FILE *data_fp = fopen(DATA_FILE, "ab");
    if (data_fp == NULL) {
        return -1;
    }
    
    /* Get offset BEFORE writing */
    long offset = ftell(data_fp);
    
    /* Write product */
    if (fwrite(product, sizeof(Product), 1, data_fp) != 1) {
        fclose(data_fp);
        return -1;
    }
    
    fclose(data_fp);
    
    /* Load existing index */
    IndexEntry *index = NULL;
    size_t index_count = 0;
    load_index(&index, &index_count);
    
    /* Create new index array with one more entry */
    IndexEntry *new_index = realloc(index, (index_count + 1) * sizeof(IndexEntry));
    if (new_index == NULL) {
        free(index);
        return -1;
    }
    index = new_index;
    
    /* Add new entry */
    index[index_count].code = product->code;
    index[index_count].offset = offset;
    index_count++;
    
    /* Sort by code for binary search */
    qsort(index, index_count, sizeof(IndexEntry), compare_index);
    
    /* Save updated index */
    int result = save_index(index, index_count);
    
    free(index);
    return result;
}

/**
 * SOLUTION: search_product()
 */
Product *search_product(int32_t code) {
    static Product found_product;
    
    /* Load index */
    IndexEntry *index = NULL;
    size_t index_count = 0;
    
    if (load_index(&index, &index_count) != 0 || index_count == 0) {
        if (index) free(index);
        return NULL;
    }
    
    /* Binary search for the code */
    IndexEntry key = { .code = code };
    IndexEntry *found = bsearch(&key, index, index_count, 
                                 sizeof(IndexEntry), compare_index);
    
    if (found == NULL) {
        free(index);
        return NULL;
    }
    
    long offset = found->offset;
    free(index);
    
    /* Open data file and read the record */
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        return NULL;
    }
    
    fseek(fp, offset, SEEK_SET);
    
    if (fread(&found_product, sizeof(Product), 1, fp) != 1) {
        fclose(fp);
        return NULL;
    }
    
    fclose(fp);
    
    /* Check if deleted */
    if (found_product.deleted == TOMBSTONE_MARKER) {
        return NULL;
    }
    
    return &found_product;
}

/**
 * SOLUTION: update_price()
 */
int update_price(int32_t code, double new_price) {
    /* Load index */
    IndexEntry *index = NULL;
    size_t index_count = 0;
    
    if (load_index(&index, &index_count) != 0 || index_count == 0) {
        if (index) free(index);
        return -1;
    }
    
    /* Find entry */
    IndexEntry key = { .code = code };
    IndexEntry *found = bsearch(&key, index, index_count,
                                 sizeof(IndexEntry), compare_index);
    
    if (found == NULL) {
        free(index);
        return -1;
    }
    
    long offset = found->offset;
    free(index);
    
    /* Open data file for read+write */
    FILE *fp = fopen(DATA_FILE, "r+b");
    if (fp == NULL) {
        return -1;
    }
    
    /* Seek to record */
    fseek(fp, offset, SEEK_SET);
    
    /* Read existing product */
    Product p;
    if (fread(&p, sizeof(Product), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }
    
    /* Check if deleted */
    if (p.deleted == TOMBSTONE_MARKER) {
        fclose(fp);
        return -1;
    }
    
    /* Update price */
    p.price = new_price;
    
    /* Seek back and write */
    fseek(fp, offset, SEEK_SET);
    
    if (fwrite(&p, sizeof(Product), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }
    
    fflush(fp);
    fclose(fp);
    
    return 0;
}

/**
 * SOLUTION: delete_product()
 */
int delete_product(int32_t code) {
    /* Load index */
    IndexEntry *index = NULL;
    size_t index_count = 0;
    
    if (load_index(&index, &index_count) != 0 || index_count == 0) {
        if (index) free(index);
        return -1;
    }
    
    /* Find entry */
    IndexEntry key = { .code = code };
    IndexEntry *found = bsearch(&key, index, index_count,
                                 sizeof(IndexEntry), compare_index);
    
    if (found == NULL) {
        free(index);
        return -1;
    }
    
    long offset = found->offset;
    free(index);
    
    /* Open data file for read+write */
    FILE *fp = fopen(DATA_FILE, "r+b");
    if (fp == NULL) {
        return -1;
    }
    
    /* Seek to record */
    fseek(fp, offset, SEEK_SET);
    
    /* Read existing product */
    Product p;
    if (fread(&p, sizeof(Product), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }
    
    /* Mark as deleted */
    p.deleted = TOMBSTONE_MARKER;
    
    /* Seek back and write */
    fseek(fp, offset, SEEK_SET);
    
    if (fwrite(&p, sizeof(Product), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }
    
    fflush(fp);
    fclose(fp);
    
    return 0;
}

/**
 * SOLUTION: rebuild_index()
 */
int rebuild_index(void) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        remove(INDEX_FILE);
        return 0;
    }
    
    /* Get file size */
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    if (file_size == 0) {
        fclose(fp);
        remove(INDEX_FILE);
        return 0;
    }
    
    /* Calculate record count */
    size_t total_records = file_size / sizeof(Product);
    
    /* Allocate maximum possible index size */
    IndexEntry *index = malloc(total_records * sizeof(IndexEntry));
    if (index == NULL) {
        fclose(fp);
        return -1;
    }
    
    /* Read each record and add to index if not deleted */
    size_t index_count = 0;
    Product p;
    
    for (size_t i = 0; i < total_records; i++) {
        long offset = ftell(fp);
        
        if (fread(&p, sizeof(Product), 1, fp) != 1) {
            break;
        }
        
        /* Only include active records */
        if (p.deleted != TOMBSTONE_MARKER) {
            index[index_count].code = p.code;
            index[index_count].offset = offset;
            index_count++;
        }
    }
    
    fclose(fp);
    
    /* Sort the index */
    if (index_count > 0) {
        qsort(index, index_count, sizeof(IndexEntry), compare_index);
    }
    
    /* Save the index */
    int result = save_index(index, index_count);
    
    free(index);
    return result;
}

/**
 * SOLUTION: get_stats()
 */
DatabaseStats get_stats(void) {
    DatabaseStats stats = {0, 0, 0, 0};
    
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        return stats;
    }
    
    /* Get file size */
    fseek(fp, 0, SEEK_END);
    stats.file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    /* Calculate total records */
    stats.total_records = stats.file_size / sizeof(Product);
    
    /* Count active and deleted */
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
 * SOLUTION: print_product()
 */
void print_product(const Product *p) {
    const char *status = (p->deleted == TOMBSTONE_MARKER) ? "DELETED" : "Active";
    printf("│ %5d │ %-25s │ %10.2f │ %6d │ %-7s │\n",
           p->code, p->name, p->price, p->quantity, status);
}

/**
 * SOLUTION: print_all_products()
 */
void print_all_products(void) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("[INFO] No products in database\n");
        return;
    }
    
    printf("┌───────┬───────────────────────────┬────────────┬────────┬─────────┐\n");
    printf("│ Code  │ Name                      │      Price │    Qty │ Status  │\n");
    printf("├───────┼───────────────────────────┼────────────┼────────┼─────────┤\n");
    
    Product p;
    while (fread(&p, sizeof(Product), 1, fp) == 1) {
        print_product(&p);
    }
    
    printf("└───────┴───────────────────────────┴────────────┴────────┴─────────┘\n");
    
    fclose(fp);
}

/**
 * Print statistics
 */
void print_stats(const DatabaseStats *stats) {
    /* Printed verbatim to match the published reference output for Week 03. */
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
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║              INDEXED PRODUCT DATABASE SYSTEM                   ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
    
    remove(DATA_FILE);
    remove(INDEX_FILE);
    
    int n;
    printf("Enter number of products: ");
    if (scanf("%d", &n) != 1 || n < 0) {
        printf("[ERROR] Invalid input\n");
        return 1;
    }
    
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
    
    printf("\n[INFO] Current database contents:\n");
    print_all_products();
    
    if (n > 0) {
        printf("\n[INFO] Testing index-based search...\n");
        rebuild_index();
        
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
    
    printf("\n[INFO] Final database state:\n");
    print_all_products();
    
    DatabaseStats stats = get_stats();
    print_stats(&stats);
    
    printf("\n[INFO] Operations complete\n\n");
    
    return 0;
}

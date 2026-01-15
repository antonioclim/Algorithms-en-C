/**
 * =============================================================================
 * HOMEWORK 1: Library Book Catalogue - SOLUTION
 * =============================================================================
 *
 * INSTRUCTOR COPY - Contains complete implementation
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

/* =============================================================================
 * CONSTANTS AND MACROS
 * =============================================================================
 */

#define CATALOGUE_FILE      "library.bin"
#define INDEX_FILE          "library.idx"
#define LOG_FILE            "library.log"
#define MAGIC_NUMBER        "LIBR"
#define FILE_VERSION        1

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct {
    char     magic[4];
    uint32_t version;
    uint32_t record_count;
    uint32_t record_size;
    time_t   created;
    time_t   modified;
    uint8_t  reserved[32];
} CatalogueHeader;

typedef struct {
    int64_t  isbn;
    char     title[100];
    char     author[60];
    int32_t  year_published;
    double   price;
    int32_t  copies_available;
    int32_t  times_borrowed;
} Book;

typedef struct {
    int64_t isbn;
    long    offset;
} BookIndex;

/* =============================================================================
 * LOGGING FUNCTIONS
 * =============================================================================
 */

static void log_operation(const char *operation, const Book *book) {
    FILE *log_fp = fopen(LOG_FILE, "a");
    if (log_fp == NULL) return;
    
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    
    if (book != NULL) {
        fprintf(log_fp, "[%s] %s: ISBN=%ld, Title=\"%s\", Author=\"%s\"\n",
                timestamp, operation, (long)book->isbn, book->title, book->author);
    } else {
        fprintf(log_fp, "[%s] %s\n", timestamp, operation);
    }
    
    fclose(log_fp);
}

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

static void init_book(Book *b, int64_t isbn, const char *title, 
                      const char *author, int32_t year, double price,
                      int32_t copies, int32_t borrowed) {
    memset(b, 0, sizeof(Book));
    b->isbn = isbn;
    strncpy(b->title, title, sizeof(b->title) - 1);
    strncpy(b->author, author, sizeof(b->author) - 1);
    b->year_published = year;
    b->price = price;
    b->copies_available = copies;
    b->times_borrowed = borrowed;
}

static int compare_index(const void *a, const void *b) {
    int64_t diff = ((const BookIndex *)a)->isbn - ((const BookIndex *)b)->isbn;
    return (diff > 0) - (diff < 0);  /* Return -1, 0, or 1 */
}

/* =============================================================================
 * HEADER FUNCTIONS
 * =============================================================================
 */

static int write_header(FILE *fp, uint32_t record_count) {
    CatalogueHeader header;
    memset(&header, 0, sizeof(header));
    memcpy(header.magic, MAGIC_NUMBER, 4);
    header.version = FILE_VERSION;
    header.record_count = record_count;
    header.record_size = sizeof(Book);
    header.created = time(NULL);
    header.modified = header.created;
    
    fseek(fp, 0, SEEK_SET);
    return fwrite(&header, sizeof(header), 1, fp) == 1 ? 0 : -1;
}

static int read_header(FILE *fp, CatalogueHeader *header) {
    fseek(fp, 0, SEEK_SET);
    if (fread(header, sizeof(*header), 1, fp) != 1) {
        return -1;
    }
    
    if (memcmp(header->magic, MAGIC_NUMBER, 4) != 0) {
        return -2;  /* Invalid magic number */
    }
    
    if (header->version > FILE_VERSION) {
        return -3;  /* Unsupported version */
    }
    
    return 0;
}

/* =============================================================================
 * INDEX FUNCTIONS
 * =============================================================================
 */

static int load_index(BookIndex **index, size_t *count) {
    *index = NULL;
    *count = 0;
    
    FILE *fp = fopen(INDEX_FILE, "rb");
    if (fp == NULL) return 0;
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    if (size == 0) {
        fclose(fp);
        return 0;
    }
    
    *count = size / sizeof(BookIndex);
    *index = malloc(*count * sizeof(BookIndex));
    
    if (*index == NULL) {
        fclose(fp);
        return -1;
    }
    
    fread(*index, sizeof(BookIndex), *count, fp);
    fclose(fp);
    
    return 0;
}

static int save_index(const BookIndex *index, size_t count) {
    FILE *fp = fopen(INDEX_FILE, "wb");
    if (fp == NULL) return -1;
    
    if (count > 0) {
        fwrite(index, sizeof(BookIndex), count, fp);
    }
    
    fclose(fp);
    return 0;
}

int rebuild_index(void) {
    FILE *fp = fopen(CATALOGUE_FILE, "rb");
    if (fp == NULL) {
        remove(INDEX_FILE);
        return 0;
    }
    
    CatalogueHeader header;
    if (read_header(fp, &header) != 0) {
        fclose(fp);
        return -1;
    }
    
    BookIndex *index = malloc(header.record_count * sizeof(BookIndex));
    if (index == NULL) {
        fclose(fp);
        return -1;
    }
    
    Book book;
    size_t index_count = 0;
    
    for (uint32_t i = 0; i < header.record_count; i++) {
        long offset = ftell(fp);
        if (fread(&book, sizeof(Book), 1, fp) == 1) {
            index[index_count].isbn = book.isbn;
            index[index_count].offset = offset;
            index_count++;
        }
    }
    
    qsort(index, index_count, sizeof(BookIndex), compare_index);
    save_index(index, index_count);
    
    free(index);
    fclose(fp);
    
    log_operation("INDEX_REBUILD", NULL);
    return 0;
}

/* =============================================================================
 * BOOK OPERATIONS
 * =============================================================================
 */

int add_book(const char *filename, const Book *book) {
    FILE *fp = fopen(filename, "r+b");
    
    if (fp == NULL) {
        /* Create new file with header */
        fp = fopen(filename, "wb");
        if (fp == NULL) return -1;
        
        write_header(fp, 0);
    }
    
    /* Read current header */
    CatalogueHeader header;
    if (read_header(fp, &header) != 0) {
        /* New file, initialise header */
        memset(&header, 0, sizeof(header));
        memcpy(header.magic, MAGIC_NUMBER, 4);
        header.version = FILE_VERSION;
        header.record_size = sizeof(Book);
        header.created = time(NULL);
    }
    
    /* Seek to end of data */
    fseek(fp, sizeof(CatalogueHeader) + header.record_count * sizeof(Book), SEEK_SET);
    long offset = ftell(fp);
    
    /* Write book */
    if (fwrite(book, sizeof(Book), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }
    
    /* Update header */
    header.record_count++;
    header.modified = time(NULL);
    write_header(fp, header.record_count);
    
    fclose(fp);
    
    /* Update index */
    BookIndex *index = NULL;
    size_t index_count = 0;
    load_index(&index, &index_count);
    
    index = realloc(index, (index_count + 1) * sizeof(BookIndex));
    if (index != NULL) {
        index[index_count].isbn = book->isbn;
        index[index_count].offset = offset;
        index_count++;
        qsort(index, index_count, sizeof(BookIndex), compare_index);
        save_index(index, index_count);
        free(index);
    }
    
    log_operation("ADD_BOOK", book);
    return 0;
}

Book *find_by_isbn(const char *filename, int64_t isbn) {
    static Book found_book;
    
    BookIndex *index = NULL;
    size_t index_count = 0;
    
    if (load_index(&index, &index_count) != 0 || index_count == 0) {
        if (index) free(index);
        return NULL;
    }
    
    BookIndex key = { .isbn = isbn };
    BookIndex *found = bsearch(&key, index, index_count, 
                                sizeof(BookIndex), compare_index);
    
    if (found == NULL) {
        free(index);
        return NULL;
    }
    
    long offset = found->offset;
    free(index);
    
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) return NULL;
    
    fseek(fp, offset, SEEK_SET);
    if (fread(&found_book, sizeof(Book), 1, fp) != 1) {
        fclose(fp);
        return NULL;
    }
    
    fclose(fp);
    return &found_book;
}

int find_by_author(const char *filename, const char *author, 
                   Book **results, size_t *count) {
    *results = NULL;
    *count = 0;
    
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) return -1;
    
    CatalogueHeader header;
    if (read_header(fp, &header) != 0) {
        fclose(fp);
        return -1;
    }
    
    /* First pass: count matches */
    size_t matches = 0;
    Book book;
    
    for (uint32_t i = 0; i < header.record_count; i++) {
        if (fread(&book, sizeof(Book), 1, fp) == 1) {
            if (strstr(book.author, author) != NULL) {
                matches++;
            }
        }
    }
    
    if (matches == 0) {
        fclose(fp);
        return 0;
    }
    
    /* Allocate results array */
    *results = malloc(matches * sizeof(Book));
    if (*results == NULL) {
        fclose(fp);
        return -1;
    }
    
    /* Second pass: collect matches */
    fseek(fp, sizeof(CatalogueHeader), SEEK_SET);
    
    for (uint32_t i = 0; i < header.record_count; i++) {
        if (fread(&book, sizeof(Book), 1, fp) == 1) {
            if (strstr(book.author, author) != NULL) {
                (*results)[*count] = book;
                (*count)++;
            }
        }
    }
    
    fclose(fp);
    return 0;
}

int find_by_year_range(const char *filename, int32_t start_year, 
                       int32_t end_year, Book **results, size_t *count) {
    *results = NULL;
    *count = 0;
    
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) return -1;
    
    CatalogueHeader header;
    if (read_header(fp, &header) != 0) {
        fclose(fp);
        return -1;
    }
    
    /* Temporary storage */
    Book *temp = malloc(header.record_count * sizeof(Book));
    if (temp == NULL) {
        fclose(fp);
        return -1;
    }
    
    Book book;
    for (uint32_t i = 0; i < header.record_count; i++) {
        if (fread(&book, sizeof(Book), 1, fp) == 1) {
            if (book.year_published >= start_year && 
                book.year_published <= end_year) {
                temp[*count] = book;
                (*count)++;
            }
        }
    }
    
    fclose(fp);
    
    if (*count == 0) {
        free(temp);
        return 0;
    }
    
    /* Resize to exact size */
    Book *resized = realloc(temp, *count * sizeof(Book));
    if (resized == NULL) {
        *results = temp;  /* Use original if realloc fails */
    } else {
        *results = resized;
    }
    
    return 0;
}

/* =============================================================================
 * DISPLAY FUNCTIONS
 * =============================================================================
 */

void print_book(const Book *b) {
    printf("│ %-13ld │ %-40.40s │ %-25.25s │ %4d │ %7.2f │ %3d │\n",
           (long)b->isbn, b->title, b->author, b->year_published,
           b->price, b->copies_available);
}

void print_catalogue(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("[INFO] Catalogue is empty\n");
        return;
    }
    
    CatalogueHeader header;
    if (read_header(fp, &header) != 0) {
        printf("[ERROR] Invalid catalogue file\n");
        fclose(fp);
        return;
    }
    
    printf("\n╔══════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                           LIBRARY CATALOGUE                                              ║\n");
    printf("╠═══════════════╤══════════════════════════════════════════╤═══════════════════════════╤══════╤═════════╤═════╣\n");
    printf("║     ISBN      │ Title                                    │ Author                    │ Year │  Price  │ Qty ║\n");
    printf("╠═══════════════╪══════════════════════════════════════════╪═══════════════════════════╪══════╪═════════╪═════╣\n");
    
    Book book;
    for (uint32_t i = 0; i < header.record_count; i++) {
        if (fread(&book, sizeof(Book), 1, fp) == 1) {
            print_book(&book);
        }
    }
    
    printf("╚═══════════════╧══════════════════════════════════════════╧═══════════════════════════╧══════╧═════════╧═════╝\n");
    printf("\nTotal books: %u\n", header.record_count);
    
    fclose(fp);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║              LIBRARY BOOK CATALOGUE SYSTEM                     ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
    
    /* Clean start */
    remove(CATALOGUE_FILE);
    remove(INDEX_FILE);
    remove(LOG_FILE);
    
    /* Add sample books */
    Book books[] = {
        {0}, {0}, {0}, {0}, {0}
    };
    
    init_book(&books[0], 9780131103627, "The C Programming Language", 
              "Kernighan & Ritchie", 1988, 45.99, 5, 150);
    init_book(&books[1], 9780201633610, "Design Patterns", 
              "Gang of Four", 1994, 54.99, 3, 89);
    init_book(&books[2], 9780596007126, "Head First Design Patterns",
              "Freeman & Freeman", 2004, 49.99, 7, 67);
    init_book(&books[3], 9780132350884, "Clean Code",
              "Robert C. Martin", 2008, 39.99, 4, 102);
    init_book(&books[4], 9780596517748, "JavaScript: The Good Parts",
              "Douglas Crockford", 2008, 29.99, 6, 78);
    
    printf("[INFO] Adding sample books to catalogue...\n\n");
    
    for (int i = 0; i < 5; i++) {
        if (add_book(CATALOGUE_FILE, &books[i]) == 0) {
            printf("[OK] Added: %s\n", books[i].title);
        }
    }
    
    /* Display catalogue */
    print_catalogue(CATALOGUE_FILE);
    
    /* Test search by ISBN */
    printf("\n[INFO] Searching for ISBN 9780131103627...\n");
    Book *found = find_by_isbn(CATALOGUE_FILE, 9780131103627);
    if (found != NULL) {
        printf("[OK] Found: \"%s\" by %s\n", found->title, found->author);
    }
    
    /* Test search by author */
    printf("\n[INFO] Searching for books by 'Martin'...\n");
    Book *author_results = NULL;
    size_t author_count = 0;
    
    if (find_by_author(CATALOGUE_FILE, "Martin", &author_results, &author_count) == 0) {
        printf("[OK] Found %zu book(s):\n", author_count);
        for (size_t i = 0; i < author_count; i++) {
            printf("     - %s\n", author_results[i].title);
        }
        free(author_results);
    }
    
    /* Test search by year range */
    printf("\n[INFO] Searching for books from 2000-2010...\n");
    Book *year_results = NULL;
    size_t year_count = 0;
    
    if (find_by_year_range(CATALOGUE_FILE, 2000, 2010, &year_results, &year_count) == 0) {
        printf("[OK] Found %zu book(s):\n", year_count);
        for (size_t i = 0; i < year_count; i++) {
            printf("     - %s (%d)\n", year_results[i].title, year_results[i].year_published);
        }
        free(year_results);
    }
    
    /* Show log */
    printf("\n[INFO] Transaction log:\n");
    FILE *log = fopen(LOG_FILE, "r");
    if (log != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), log)) {
            printf("     %s", line);
        }
        fclose(log);
    }
    
    printf("\n[INFO] Operations complete\n\n");
    
    return 0;
}

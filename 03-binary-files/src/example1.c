/**
 * =============================================================================
 * WEEK 03: BINARY FILES IN C
 * Complete Working Example
 * =============================================================================
 *
 * This comprehensive example demonstrates:
 *   1. Opening and closing binary files with various modes
 *   2. Writing structures to binary files using fwrite()
 *   3. Reading structures from binary files using fread()
 *   4. Random access with fseek() and ftell()
 *   5. Calculating file size and record count
 *   6. Updating records in place
 *   7. Implementing a simple index for fast lookup
 *   8. Error handling best practices
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Usage: ./example1
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

#define MAX_NAME_LENGTH     50
#define DATABASE_FILE       "example_students.bin"
#define INDEX_FILE          "example_students.idx"
#define HEADER_MAGIC        "STDB"
#define FILE_VERSION        1

/* ANSI colour codes for formatted output */
#define COLOUR_RESET        "\033[0m"
#define COLOUR_GREEN        "\033[0;32m"
#define COLOUR_YELLOW       "\033[0;33m"
#define COLOUR_CYAN         "\033[0;36m"
#define COLOUR_RED          "\033[0;31m"
#define COLOUR_BOLD         "\033[1m"

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

/**
 * Student record structure
 * 
 * Fixed-size structure suitable for binary file storage.
 * Total size: 4 + 50 + 4 + 4 = 62 bytes (may vary with padding)
 */
typedef struct {
    int32_t id;                     /* Unique student identifier */
    char    name[MAX_NAME_LENGTH];  /* Student name (null-terminated) */
    float   gpa;                    /* Grade point average (0.0 - 4.0) */
    int32_t year;                   /* Enrollment year */
} Student;

/**
 * File header structure
 * 
 * Contains metadata for file validation and versioning.
 */
typedef struct {
    char     magic[4];              /* File type identifier "STDB" */
    uint32_t version;               /* File format version */
    uint32_t record_count;          /* Number of records in file */
    uint32_t record_size;           /* Size of each record */
    time_t   created;               /* File creation timestamp */
    time_t   modified;              /* Last modification timestamp */
    uint8_t  reserved[32];          /* Reserved for future use */
} FileHeader;

/**
 * Index entry structure
 * 
 * Maps student ID to file offset for O(1) lookup.
 */
typedef struct {
    int32_t id;                     /* Student ID (search key) */
    long    offset;                 /* Byte offset in data file */
} IndexEntry;

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Print a horizontal separator line
 */
static void print_separator(void) {
    printf("═══════════════════════════════════════════════════════════════\n");
}

/**
 * Print a section header with box drawing characters
 */
static void print_header(const char *title) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  %-61s ║\n", title);
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
}

/**
 * Print a student record in formatted table row
 */
static void print_student(const Student *s) {
    printf("│ %6d │ %-30s │ %5.2f │ %4d │\n",
           s->id, s->name, s->gpa, s->year);
}

/**
 * Print table header for student listing
 */
static void print_table_header(void) {
    printf("┌────────┬────────────────────────────────┬───────┬──────┐\n");
    printf("│ %6s │ %-30s │ %5s │ %4s │\n", "ID", "Name", "GPA", "Year");
    printf("├────────┼────────────────────────────────┼───────┼──────┤\n");
}

/**
 * Print table footer
 */
static void print_table_footer(void) {
    printf("└────────┴────────────────────────────────┴───────┴──────┘\n");
}

/**
 * Initialise a student structure with provided values
 * 
 * Ensures the name array is properly null-terminated and
 * the structure is zero-initialised to avoid uninitialised bytes.
 */
static void init_student(Student *s, int32_t id, const char *name, 
                         float gpa, int32_t year) {
    memset(s, 0, sizeof(Student));  /* Clear any padding bytes */
    s->id = id;
    strncpy(s->name, name, MAX_NAME_LENGTH - 1);
    s->name[MAX_NAME_LENGTH - 1] = '\0';  /* Ensure null-termination */
    s->gpa = gpa;
    s->year = year;
}

/* =============================================================================
 * PART 1: BASIC BINARY FILE OPERATIONS
 * =============================================================================
 */

/**
 * Demonstrate basic binary file writing
 * 
 * Creates a new binary file and writes multiple student records.
 */
static void demo_basic_writing(void) {
    print_header("PART 1: Basic Binary File Writing");
    
    /* Properly initialise students to avoid padding issues */
    Student sample_students[5];
    init_student(&sample_students[0], 1001, "Alice Johnson", 3.75f, 2024);
    init_student(&sample_students[1], 1002, "Bob Smith", 3.42f, 2023);
    init_student(&sample_students[2], 1003, "Carol Williams", 3.91f, 2024);
    init_student(&sample_students[3], 1004, "David Brown", 3.28f, 2022);
    init_student(&sample_students[4], 1005, "Eve Davis", 3.85f, 2023);
    
    size_t num_students = sizeof(sample_students) / sizeof(sample_students[0]);
    
    printf("%sOpening file for binary writing...%s\n", COLOUR_CYAN, COLOUR_RESET);
    
    /* Open file in write binary mode - creates new or truncates existing */
    FILE *fp = fopen(DATABASE_FILE, "wb");
    if (fp == NULL) {
        perror(COLOUR_RED "Error opening file" COLOUR_RESET);
        return;
    }
    
    printf("%s✓ File opened successfully%s\n\n", COLOUR_GREEN, COLOUR_RESET);
    
    /* Write all records at once */
    printf("Writing %zu student records...\n", num_students);
    
    size_t written = fwrite(sample_students, sizeof(Student), num_students, fp);
    
    if (written != num_students) {
        fprintf(stderr, "%s✗ Error: only wrote %zu of %zu records%s\n",
                COLOUR_RED, written, num_students, COLOUR_RESET);
    } else {
        printf("%s✓ Successfully wrote %zu records%s\n", 
               COLOUR_GREEN, written, COLOUR_RESET);
    }
    
    /* Display what was written */
    printf("\nRecords written:\n");
    print_table_header();
    for (size_t i = 0; i < num_students; i++) {
        print_student(&sample_students[i]);
    }
    print_table_footer();
    
    /* Important: close the file to flush buffers */
    fclose(fp);
    printf("\n%s✓ File closed%s\n", COLOUR_GREEN, COLOUR_RESET);
    
    /* Display file information */
    printf("\nFile information:\n");
    printf("  • Filename: %s\n", DATABASE_FILE);
    printf("  • Record size: %zu bytes\n", sizeof(Student));
    printf("  • Total records: %zu\n", num_students);
    printf("  • Expected file size: %zu bytes\n", num_students * sizeof(Student));
}

/* =============================================================================
 * PART 2: READING BINARY FILES
 * =============================================================================
 */

/**
 * Demonstrate binary file reading
 * 
 * Opens the previously created file and reads all records.
 */
static void demo_basic_reading(void) {
    print_header("PART 2: Basic Binary File Reading");
    
    printf("%sOpening file for binary reading...%s\n", COLOUR_CYAN, COLOUR_RESET);
    
    /* Open file in read binary mode */
    FILE *fp = fopen(DATABASE_FILE, "rb");
    if (fp == NULL) {
        perror(COLOUR_RED "Error opening file" COLOUR_RESET);
        return;
    }
    
    printf("%s✓ File opened successfully%s\n\n", COLOUR_GREEN, COLOUR_RESET);
    
    /* Calculate number of records using fseek/ftell */
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);  /* Return to beginning */
    
    size_t record_count = file_size / sizeof(Student);
    
    printf("File statistics:\n");
    printf("  • File size: %ld bytes\n", file_size);
    printf("  • Record size: %zu bytes\n", sizeof(Student));
    printf("  • Record count: %zu\n\n", record_count);
    
    /* Read all records using a loop */
    printf("Reading records one by one:\n");
    print_table_header();
    
    Student s;
    int count = 0;
    
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        print_student(&s);
        count++;
    }
    
    print_table_footer();
    
    /* Check why we stopped reading */
    if (feof(fp)) {
        printf("\n%s✓ Reached end of file after %d records%s\n", 
               COLOUR_GREEN, count, COLOUR_RESET);
    } else if (ferror(fp)) {
        printf("\n%s✗ Error occurred while reading%s\n", 
               COLOUR_RED, COLOUR_RESET);
    }
    
    fclose(fp);
}

/* =============================================================================
 * PART 3: RANDOM ACCESS
 * =============================================================================
 */

/**
 * Demonstrate random access capabilities
 * 
 * Shows how to jump directly to any record using fseek().
 */
static void demo_random_access(void) {
    print_header("PART 3: Random Access with fseek()");
    
    FILE *fp = fopen(DATABASE_FILE, "rb");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }
    
    /* Calculate total records */
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    size_t total_records = file_size / sizeof(Student);
    
    printf("Total records in file: %zu\n\n", total_records);
    
    /* Access specific records by index */
    size_t indices_to_access[] = {2, 0, 4, 1};
    size_t num_accesses = sizeof(indices_to_access) / sizeof(indices_to_access[0]);
    
    printf("Accessing records in non-sequential order:\n");
    print_table_header();
    
    Student s;
    for (size_t i = 0; i < num_accesses; i++) {
        size_t index = indices_to_access[i];
        
        if (index >= total_records) {
            printf("│ Index %zu out of range                              │\n", index);
            continue;
        }
        
        /* Calculate byte offset */
        long offset = (long)(index * sizeof(Student));
        
        /* Seek to the record */
        if (fseek(fp, offset, SEEK_SET) != 0) {
            perror("Seek failed");
            continue;
        }
        
        /* Read the record */
        if (fread(&s, sizeof(Student), 1, fp) == 1) {
            printf("│ [idx=%zu] ", index);
            printf("%4d │ %-30s │ %5.2f │ %4d │\n",
                   s.id, s.name, s.gpa, s.year);
        }
    }
    
    print_table_footer();
    
    /* Demonstrate reading the last record using SEEK_END */
    printf("\nReading last record using SEEK_END:\n");
    
    /* Position at last record: offset is negative from end */
    fseek(fp, -(long)sizeof(Student), SEEK_END);
    
    if (fread(&s, sizeof(Student), 1, fp) == 1) {
        printf("Last record: ID=%d, Name=%s\n", s.id, s.name);
    }
    
    fclose(fp);
    printf("\n%s✓ Random access demonstration complete%s\n", 
           COLOUR_GREEN, COLOUR_RESET);
}

/* =============================================================================
 * PART 4: UPDATING RECORDS IN PLACE
 * =============================================================================
 */

/**
 * Demonstrate in-place record updates
 * 
 * Uses "r+b" mode to read and write to the same file.
 */
static void demo_update_records(void) {
    print_header("PART 4: Updating Records In-Place");
    
    /* Open in read/write binary mode */
    FILE *fp = fopen(DATABASE_FILE, "r+b");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }
    
    printf("Updating student ID 1003 (index 2):\n");
    printf("  • Old GPA will be changed to 4.00 (perfect score!)\n\n");
    
    /* Read the record at index 2 */
    size_t target_index = 2;
    long offset = (long)(target_index * sizeof(Student));
    
    fseek(fp, offset, SEEK_SET);
    
    Student s;
    if (fread(&s, sizeof(Student), 1, fp) != 1) {
        printf("Failed to read record\n");
        fclose(fp);
        return;
    }
    
    printf("Before update:\n");
    print_table_header();
    print_student(&s);
    print_table_footer();
    
    /* Modify the record */
    float old_gpa = s.gpa;
    s.gpa = 4.00f;
    
    /* IMPORTANT: Seek back to the same position before writing */
    fseek(fp, offset, SEEK_SET);
    
    /* Write the modified record */
    if (fwrite(&s, sizeof(Student), 1, fp) != 1) {
        printf("Failed to write record\n");
        fclose(fp);
        return;
    }
    
    /* Flush to ensure write is committed */
    fflush(fp);
    
    printf("\nAfter update:\n");
    print_table_header();
    print_student(&s);
    print_table_footer();
    
    printf("\n%s✓ Record updated: GPA changed from %.2f to %.2f%s\n",
           COLOUR_GREEN, old_gpa, s.gpa, COLOUR_RESET);
    
    fclose(fp);
}

/* =============================================================================
 * PART 5: FILE HEADER AND METADATA
 * =============================================================================
 */

/**
 * Demonstrate proper file format with header
 * 
 * Shows how to create a professional binary file format with
 * magic number, version, and metadata.
 */
static void demo_file_header(void) {
    print_header("PART 5: File Header and Metadata");
    
    const char *header_file = "example_with_header.bin";
    
    /* Create and initialise header */
    FileHeader header;
    memset(&header, 0, sizeof(FileHeader));
    memcpy(header.magic, HEADER_MAGIC, 4);
    header.version = FILE_VERSION;
    header.record_count = 3;
    header.record_size = sizeof(Student);
    header.created = time(NULL);
    header.modified = header.created;
    
    /* Sample data */
    Student students[3];
    init_student(&students[0], 2001, "Header Test Student 1", 3.50f, 2024);
    init_student(&students[1], 2002, "Header Test Student 2", 3.60f, 2024);
    init_student(&students[2], 2003, "Header Test Student 3", 3.70f, 2024);
    
    /* Write file with header */
    printf("Creating file with header...\n\n");
    
    FILE *fp = fopen(header_file, "wb");
    if (fp == NULL) {
        perror("Error creating file");
        return;
    }
    
    /* Write header first */
    fwrite(&header, sizeof(FileHeader), 1, fp);
    
    /* Write data records */
    fwrite(students, sizeof(Student), 3, fp);
    
    fclose(fp);
    
    /* Display header information */
    printf("Header contents:\n");
    printf("  • Magic number: %.4s\n", header.magic);
    printf("  • Version: %u\n", header.version);
    printf("  • Record count: %u\n", header.record_count);
    printf("  • Record size: %u bytes\n", header.record_size);
    printf("  • Created: %s", ctime(&header.created));
    
    /* Now read and validate the file */
    printf("\nReading and validating file...\n\n");
    
    fp = fopen(header_file, "rb");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }
    
    FileHeader read_header;
    fread(&read_header, sizeof(FileHeader), 1, fp);
    
    /* Validate magic number */
    if (memcmp(read_header.magic, HEADER_MAGIC, 4) != 0) {
        printf("%s✗ Invalid file format (wrong magic number)%s\n",
               COLOUR_RED, COLOUR_RESET);
        fclose(fp);
        return;
    }
    
    /* Validate version */
    if (read_header.version > FILE_VERSION) {
        printf("%s✗ File version %u is newer than supported version %u%s\n",
               COLOUR_RED, read_header.version, FILE_VERSION, COLOUR_RESET);
        fclose(fp);
        return;
    }
    
    printf("%s✓ File validation passed%s\n\n", COLOUR_GREEN, COLOUR_RESET);
    
    /* Read and display records */
    printf("Records from file:\n");
    print_table_header();
    
    Student s;
    for (uint32_t i = 0; i < read_header.record_count; i++) {
        if (fread(&s, sizeof(Student), 1, fp) == 1) {
            print_student(&s);
        }
    }
    
    print_table_footer();
    
    fclose(fp);
    
    /* Clean up */
    remove(header_file);
}

/* =============================================================================
 * PART 6: INDEX FILE IMPLEMENTATION
 * =============================================================================
 */

/**
 * Comparison function for qsort and bsearch
 */
static int compare_index_entries(const void *a, const void *b) {
    const IndexEntry *ia = (const IndexEntry *)a;
    const IndexEntry *ib = (const IndexEntry *)b;
    return ia->id - ib->id;
}

/**
 * Demonstrate index file for fast lookups
 * 
 * Creates a separate index file that maps IDs to file offsets,
 * enabling O(log n) search using binary search.
 */
static void demo_index_file(void) {
    print_header("PART 6: Index File for Fast Lookup");
    
    /* First, read all records and build index */
    FILE *data_fp = fopen(DATABASE_FILE, "rb");
    if (data_fp == NULL) {
        perror("Error opening data file");
        return;
    }
    
    /* Get record count */
    fseek(data_fp, 0, SEEK_END);
    long file_size = ftell(data_fp);
    fseek(data_fp, 0, SEEK_SET);
    
    size_t record_count = file_size / sizeof(Student);
    
    /* Allocate index array */
    IndexEntry *index = malloc(record_count * sizeof(IndexEntry));
    if (index == NULL) {
        printf("Memory allocation failed\n");
        fclose(data_fp);
        return;
    }
    
    /* Build index by reading all records */
    printf("Building index from %zu records...\n\n", record_count);
    
    Student s;
    for (size_t i = 0; i < record_count; i++) {
        long offset = ftell(data_fp);
        if (fread(&s, sizeof(Student), 1, data_fp) == 1) {
            index[i].id = s.id;
            index[i].offset = offset;
        }
    }
    
    /* Sort index by ID for binary search */
    qsort(index, record_count, sizeof(IndexEntry), compare_index_entries);
    
    /* Display index */
    printf("Index contents (sorted by ID):\n");
    printf("┌──────────┬──────────────┐\n");
    printf("│    ID    │    Offset    │\n");
    printf("├──────────┼──────────────┤\n");
    for (size_t i = 0; i < record_count; i++) {
        printf("│ %8d │ %12ld │\n", index[i].id, index[i].offset);
    }
    printf("└──────────┴──────────────┘\n\n");
    
    /* Save index to file */
    FILE *idx_fp = fopen(INDEX_FILE, "wb");
    if (idx_fp != NULL) {
        fwrite(index, sizeof(IndexEntry), record_count, idx_fp);
        fclose(idx_fp);
        printf("%s✓ Index saved to %s%s\n\n", 
               COLOUR_GREEN, INDEX_FILE, COLOUR_RESET);
    }
    
    /* Demonstrate fast lookup using bsearch */
    printf("Demonstrating O(log n) lookup using binary search:\n\n");
    
    int search_ids[] = {1003, 1001, 1005, 9999};
    size_t num_searches = sizeof(search_ids) / sizeof(search_ids[0]);
    
    for (size_t i = 0; i < num_searches; i++) {
        IndexEntry key = { .id = search_ids[i] };
        
        IndexEntry *found = bsearch(&key, index, record_count,
                                    sizeof(IndexEntry), compare_index_entries);
        
        printf("Searching for ID %d: ", search_ids[i]);
        
        if (found != NULL) {
            /* Use the offset to read the actual record */
            fseek(data_fp, found->offset, SEEK_SET);
            if (fread(&s, sizeof(Student), 1, data_fp) == 1) {
                printf("%s✓ Found: %s (GPA: %.2f)%s\n",
                       COLOUR_GREEN, s.name, s.gpa, COLOUR_RESET);
            }
        } else {
            printf("%s✗ Not found%s\n", COLOUR_YELLOW, COLOUR_RESET);
        }
    }
    
    /* Cleanup */
    free(index);
    fclose(data_fp);
}

/* =============================================================================
 * PART 7: ERROR HANDLING DEMONSTRATION
 * =============================================================================
 */

/**
 * Demonstrate proper error handling for file operations
 */
static void demo_error_handling(void) {
    print_header("PART 7: Error Handling Best Practices");
    
    /* Demonstrate various error scenarios */
    printf("1. Attempting to open non-existent file for reading:\n");
    FILE *fp = fopen("nonexistent_file.bin", "rb");
    if (fp == NULL) {
        perror("   Expected error");
        printf("   %s✓ Properly handled missing file%s\n\n", 
               COLOUR_GREEN, COLOUR_RESET);
    }
    
    /* Demonstrate partial read detection */
    printf("2. Creating file with incomplete record:\n");
    fp = fopen("partial_test.bin", "wb");
    if (fp != NULL) {
        /* Write a partial record (intentionally corrupt) */
        Student s;
        init_student(&s, 9999, "Partial Record", 3.0f, 2024);
        fwrite(&s, sizeof(Student) - 10, 1, fp);  /* Incomplete! */
        fclose(fp);
        
        /* Now try to read it */
        fp = fopen("partial_test.bin", "rb");
        if (fp != NULL) {
            Student read_s;
            size_t read = fread(&read_s, sizeof(Student), 1, fp);
            
            if (read != 1) {
                if (feof(fp)) {
                    printf("   %s✓ Detected incomplete record (EOF)%s\n", 
                           COLOUR_GREEN, COLOUR_RESET);
                } else if (ferror(fp)) {
                    printf("   Error flag set\n");
                }
            }
            fclose(fp);
        }
        remove("partial_test.bin");
    }
    
    /* Demonstrate fseek error handling */
    printf("\n3. Demonstrating fseek return value checking:\n");
    fp = fopen(DATABASE_FILE, "rb");
    if (fp != NULL) {
        int result = fseek(fp, 0, SEEK_SET);
        if (result == 0) {
            printf("   %s✓ fseek succeeded (returned 0)%s\n",
                   COLOUR_GREEN, COLOUR_RESET);
        } else {
            printf("   fseek failed (returned %d)\n", result);
        }
        
        /* Try seeking to a very large offset */
        result = fseek(fp, 1000000000L, SEEK_SET);
        if (result == 0) {
            printf("   Note: fseek to large offset succeeded\n");
            printf("   (fseek doesn't validate offset against file size)\n");
        }
        
        fclose(fp);
    }
    
    printf("\n%s✓ Error handling demonstration complete%s\n",
           COLOUR_GREEN, COLOUR_RESET);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     %sWEEK 03: BINARY FILES IN C - Complete Example%s            ║\n",
           COLOUR_BOLD, COLOUR_RESET);
    printf("║     Algorithms and Programming Techniques                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    printf("\nThis example demonstrates comprehensive binary file operations.\n");
    printf("Structure size: %zu bytes\n", sizeof(Student));
    printf("FileHeader size: %zu bytes\n\n", sizeof(FileHeader));
    
    /* Run all demonstrations */
    demo_basic_writing();
    demo_basic_reading();
    demo_random_access();
    demo_update_records();
    demo_file_header();
    demo_index_file();
    demo_error_handling();
    
    /* Cleanup generated files */
    printf("\n");
    print_separator();
    printf("Cleaning up generated files...\n");
    remove(DATABASE_FILE);
    remove(INDEX_FILE);
    printf("%s✓ Cleanup complete%s\n", COLOUR_GREEN, COLOUR_RESET);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     %s✓ All demonstrations completed successfully%s              ║\n",
           COLOUR_GREEN, COLOUR_RESET);
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

/* =============================================================================
 * END OF FILE
 * =============================================================================
 */

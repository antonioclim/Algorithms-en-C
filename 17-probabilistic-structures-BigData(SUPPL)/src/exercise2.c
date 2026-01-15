/**
 * =============================================================================
 * WEEK 17: PROBABILISTIC DATA STRUCTURES FOR BIG DATA
 * Exercise 2: Network Traffic Analysis with Count-Min Sketch and HyperLogLog
 * =============================================================================
 *
 * OBJECTIVE:
 * Process a simulated network traffic log to identify heavy hitters (frequent
 * IPs) and count unique source addresses. Compare your probabilistic results
 * with exact counting to demonstrate the space-accuracy trade-off.
 *
 * TASKS:
 *   1. Implement Count-Min Sketch for frequency estimation
 *   2. Implement HyperLogLog for cardinality estimation
 *   3. Process streaming log data
 *   4. Detect heavy hitters exceeding threshold frequency
 *   5. Estimate cardinality of unique source IPs
 *   6. Compare with exact methods and report accuracy
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c -lm
 * Usage: ./exercise2 <traffic_log_file>
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#define MAX_LINE_LENGTH 256
#define MAX_IPS_EXACT 100000    /* Limit for exact counting */

/* =============================================================================
 * HASH FUNCTION (PROVIDED)
 * =============================================================================
 */

uint64_t murmur_hash64(const void *data, size_t len, uint64_t seed) {
    const uint64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;
    
    uint64_t h = seed ^ (len * m);
    
    const uint64_t *blocks = (const uint64_t *)data;
    size_t nblocks = len / 8;
    
    for (size_t i = 0; i < nblocks; i++) {
        uint64_t k = blocks[i];
        k *= m;
        k ^= k >> r;
        k *= m;
        h ^= k;
        h *= m;
    }
    
    const uint8_t *tail = (const uint8_t *)(data) + nblocks * 8;
    uint64_t k = 0;
    
    switch (len & 7) {
        case 7: k ^= (uint64_t)tail[6] << 48; /* fallthrough */
        case 6: k ^= (uint64_t)tail[5] << 40; /* fallthrough */
        case 5: k ^= (uint64_t)tail[4] << 32; /* fallthrough */
        case 4: k ^= (uint64_t)tail[3] << 24; /* fallthrough */
        case 3: k ^= (uint64_t)tail[2] << 16; /* fallthrough */
        case 2: k ^= (uint64_t)tail[1] << 8;  /* fallthrough */
        case 1: k ^= (uint64_t)tail[0];
                k *= m;
                h ^= k;
    }
    
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    
    return h;
}

uint64_t get_hash_i(const void *data, size_t len, int i) {
    uint64_t h1 = murmur_hash64(data, len, 0);
    uint64_t h2 = murmur_hash64(data, len, h1);
    return h1 + i * h2;
}

/* =============================================================================
 * COUNT-MIN SKETCH STRUCTURE
 * =============================================================================
 */

typedef struct {
    uint32_t **table;       /* 2D array of counters */
    size_t width;           /* Number of columns (w) */
    size_t depth;           /* Number of rows (d) */
    size_t total;           /* Total count */
} CountMinSketch;

/* =============================================================================
 * TODO: IMPLEMENT COUNT-MIN SKETCH FUNCTIONS
 * =============================================================================
 */

/**
 * TODO 1: Create Count-Min Sketch with specified dimensions.
 * 
 * @param width Number of columns
 * @param depth Number of rows (hash functions)
 * @return Pointer to new CountMinSketch, or NULL on failure
 *
 * Hint:
 *   - Allocate the CMS structure
 *   - Allocate array of row pointers (depth × sizeof(uint32_t*))
 *   - For each row, allocate columns (width × sizeof(uint32_t))
 *   - Use calloc to zero-initialise counters
 */
CountMinSketch *cms_create(size_t width, size_t depth) {
    /* TODO: Implement this function */
    (void)width;
    (void)depth;
    return NULL;
}

/**
 * TODO 2: Create optimal CMS given error parameters.
 * 
 * @param epsilon Relative error bound (e.g., 0.01 for 1%)
 * @param delta Probability of exceeding error bound
 * @return Pointer to new CountMinSketch with optimal parameters
 *
 * Formulas:
 *   width = ceil(e / epsilon)    where e ≈ 2.718
 *   depth = ceil(ln(1 / delta))
 */
CountMinSketch *cms_create_optimal(double epsilon, double delta) {
    /* TODO: Implement this function */
    (void)epsilon;
    (void)delta;
    return NULL;
}

/**
 * TODO 3: Free all memory associated with a Count-Min Sketch.
 */
void cms_free(CountMinSketch *cms) {
    /* TODO: Implement this function */
    (void)cms;
}

/**
 * TODO 4: Update count for an element.
 * 
 * @param cms Pointer to CountMinSketch
 * @param data Pointer to data
 * @param len Length of data
 * @param count Amount to add (usually 1)
 *
 * Algorithm:
 *   For each row i from 0 to depth-1:
 *     1. Compute hash: h = get_hash_i(data, len, i)
 *     2. Compute column: col = h % width
 *     3. Increment: table[i][col] += count
 *   Update total count
 */
void cms_update(CountMinSketch *cms, const void *data, size_t len, int32_t count) {
    /* TODO: Implement this function */
    (void)cms;
    (void)data;
    (void)len;
    (void)count;
}

/**
 * TODO 5: Query estimated frequency for an element.
 * 
 * @param cms Pointer to CountMinSketch
 * @param data Pointer to data
 * @param len Length of data
 * @return Minimum counter value across all rows
 *
 * Algorithm:
 *   min_val = UINT32_MAX
 *   For each row i from 0 to depth-1:
 *     1. Compute hash: h = get_hash_i(data, len, i)
 *     2. Compute column: col = h % width
 *     3. If table[i][col] < min_val, update min_val
 *   Return min_val
 */
uint32_t cms_query(const CountMinSketch *cms, const void *data, size_t len) {
    /* TODO: Implement this function */
    (void)cms;
    (void)data;
    (void)len;
    return 0;
}

/* =============================================================================
 * HYPERLOGLOG STRUCTURE
 * =============================================================================
 */

typedef struct {
    uint8_t *registers;     /* Register array */
    uint8_t precision;      /* Precision (p), gives m = 2^p registers */
    uint32_t num_registers; /* Number of registers (m) */
} HyperLogLog;

/* =============================================================================
 * TODO: IMPLEMENT HYPERLOGLOG FUNCTIONS
 * =============================================================================
 */

/**
 * Helper: Count leading zeros in 64-bit value.
 */
static inline uint8_t count_leading_zeros(uint64_t x) {
    if (x == 0) return 64;
    uint8_t n = 0;
    if ((x & 0xFFFFFFFF00000000ULL) == 0) { n += 32; x <<= 32; }
    if ((x & 0xFFFF000000000000ULL) == 0) { n += 16; x <<= 16; }
    if ((x & 0xFF00000000000000ULL) == 0) { n += 8;  x <<= 8; }
    if ((x & 0xF000000000000000ULL) == 0) { n += 4;  x <<= 4; }
    if ((x & 0xC000000000000000ULL) == 0) { n += 2;  x <<= 2; }
    if ((x & 0x8000000000000000ULL) == 0) { n += 1; }
    return n;
}

/**
 * TODO 6: Create HyperLogLog with specified precision.
 * 
 * @param precision Number of bits for register selection (4-18)
 * @return Pointer to new HyperLogLog, or NULL on failure
 *
 * Notes:
 *   - num_registers = 2^precision
 *   - Default precision 14 gives ~0.81% error with ~12KB memory
 */
HyperLogLog *hll_create(uint8_t precision) {
    /* TODO: Implement this function */
    (void)precision;
    return NULL;
}

/**
 * TODO 7: Free all memory associated with HyperLogLog.
 */
void hll_free(HyperLogLog *hll) {
    /* TODO: Implement this function */
    (void)hll;
}

/**
 * TODO 8: Add an element to HyperLogLog.
 * 
 * @param hll Pointer to HyperLogLog
 * @param data Pointer to data
 * @param len Length of data
 *
 * Algorithm:
 *   1. Compute hash: h = murmur_hash64(data, len, seed)
 *   2. Select register: idx = h >> (64 - precision)
 *   3. Compute ρ (position of first 1-bit in remaining bits):
 *      remaining = (h << precision) | (1 << (precision-1))
 *      rho = count_leading_zeros(remaining) + 1
 *   4. Update register: if rho > registers[idx], registers[idx] = rho
 */
void hll_add(HyperLogLog *hll, const void *data, size_t len) {
    /* TODO: Implement this function */
    (void)hll;
    (void)data;
    (void)len;
}

/**
 * TODO 9: Estimate cardinality using harmonic mean.
 * 
 * @param hll Pointer to HyperLogLog
 * @return Estimated number of distinct elements
 *
 * Algorithm:
 *   1. Compute bias correction factor alpha:
 *      - For m=16: alpha = 0.673
 *      - For m=32: alpha = 0.697
 *      - For m=64: alpha = 0.709
 *      - Otherwise: alpha = 0.7213 / (1 + 1.079/m)
 *   
 *   2. Compute harmonic mean sum:
 *      sum = Σ (1 / 2^registers[i]) for all i
 *      zeros = count of registers equal to 0
 *   
 *   3. Raw estimate:
 *      estimate = alpha * m * m / sum
 *   
 *   4. Small range correction:
 *      if estimate <= 2.5 * m AND zeros > 0:
 *        estimate = m * ln(m / zeros)
 */
uint64_t hll_count(const HyperLogLog *hll) {
    /* TODO: Implement this function */
    (void)hll;
    return 0;
}

/* =============================================================================
 * EXACT COUNTING (FOR COMPARISON)
 * =============================================================================
 */

typedef struct {
    uint32_t ip;
    uint32_t count;
} IPCount;

typedef struct {
    IPCount *entries;
    size_t size;
    size_t capacity;
} ExactCounter;

ExactCounter *exact_create(size_t capacity) {
    ExactCounter *ec = malloc(sizeof(ExactCounter));
    if (!ec) return NULL;
    
    ec->entries = calloc(capacity, sizeof(IPCount));
    if (!ec->entries) {
        free(ec);
        return NULL;
    }
    
    ec->size = 0;
    ec->capacity = capacity;
    return ec;
}

void exact_free(ExactCounter *ec) {
    if (ec) {
        free(ec->entries);
        free(ec);
    }
}

void exact_update(ExactCounter *ec, uint32_t ip) {
    /* Linear search (inefficient but simple for comparison) */
    for (size_t i = 0; i < ec->size; i++) {
        if (ec->entries[i].ip == ip) {
            ec->entries[i].count++;
            return;
        }
    }
    
    /* New IP */
    if (ec->size < ec->capacity) {
        ec->entries[ec->size].ip = ip;
        ec->entries[ec->size].count = 1;
        ec->size++;
    }
}

uint32_t exact_query(ExactCounter *ec, uint32_t ip) {
    for (size_t i = 0; i < ec->size; i++) {
        if (ec->entries[i].ip == ip) {
            return ec->entries[i].count;
        }
    }
    return 0;
}

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Parse IP address string to uint32_t.
 */
uint32_t parse_ip(const char *ip_str) {
    unsigned int a, b, c, d;
    if (sscanf(ip_str, "%u.%u.%u.%u", &a, &b, &c, &d) != 4) {
        return 0;
    }
    return (a << 24) | (b << 16) | (c << 8) | d;
}

/**
 * Format uint32_t IP to string.
 */
void format_ip(uint32_t ip, char *buffer, size_t size) {
    snprintf(buffer, size, "%u.%u.%u.%u",
             (ip >> 24) & 0xFF,
             (ip >> 16) & 0xFF,
             (ip >> 8) & 0xFF,
             ip & 0xFF);
}

/**
 * Generate simulated traffic data if file doesn't exist.
 */
void generate_traffic_data(const char *filename, size_t num_packets) {
    FILE *file = fopen(filename, "w");
    if (!file) return;
    
    fprintf(file, "timestamp,src_ip,dst_ip,bytes\n");
    
    srand((unsigned int)time(NULL));
    time_t base_time = time(NULL);
    
    /* Create some "heavy hitter" IPs */
    uint32_t heavy_hitters[] = {
        parse_ip("192.168.1.100"),
        parse_ip("192.168.1.101"),
        parse_ip("10.0.0.50")
    };
    
    for (size_t i = 0; i < num_packets; i++) {
        uint32_t src_ip;
        
        /* 30% of traffic from heavy hitters */
        if (rand() % 100 < 30) {
            src_ip = heavy_hitters[rand() % 3];
        } else {
            /* Random IPs */
            src_ip = (192 << 24) | (168 << 16) | (rand() % 256 << 8) | (rand() % 256);
        }
        
        uint32_t dst_ip = (10 << 24) | (0 << 16) | (0 << 8) | (rand() % 100);
        int bytes = 100 + rand() % 1400;
        
        char src_str[16], dst_str[16];
        format_ip(src_ip, src_str, sizeof(src_str));
        format_ip(dst_ip, dst_str, sizeof(dst_str));
        
        fprintf(file, "%ld,%s,%s,%d\n", 
                (long)(base_time + i), src_str, dst_str, bytes);
    }
    
    fclose(file);
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    const char *traffic_file;
    
    if (argc < 2) {
        printf("No traffic file specified, generating sample data...\n");
        traffic_file = "sample_traffic.csv";
        generate_traffic_data(traffic_file, 10000);
    } else {
        traffic_file = argv[1];
    }
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 2: NETWORK TRAFFIC ANALYSIS                      ║\n");
    printf("║     Count-Min Sketch + HyperLogLog                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Create probabilistic structures */
    printf("Creating probabilistic structures...\n");
    
    CountMinSketch *cms = cms_create_optimal(0.01, 0.01);
    if (!cms) {
        fprintf(stderr, "Error: Failed to create Count-Min Sketch\n");
        return 1;
    }
    
    HyperLogLog *hll = hll_create(14);
    if (!hll) {
        fprintf(stderr, "Error: Failed to create HyperLogLog\n");
        cms_free(cms);
        return 1;
    }
    
    /* Create exact counter for comparison */
    ExactCounter *exact = exact_create(MAX_IPS_EXACT);
    
    printf("  Count-Min Sketch: %zu × %zu = %zu cells (%zu bytes)\n",
           cms->width, cms->depth, 
           cms->width * cms->depth,
           cms->width * cms->depth * sizeof(uint32_t));
    printf("  HyperLogLog: %u registers (%u bytes)\n",
           hll->num_registers, hll->num_registers);
    
    /* Process traffic log */
    printf("\nProcessing traffic log '%s'...\n", traffic_file);
    
    FILE *file = fopen(traffic_file, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open '%s'\n", traffic_file);
        cms_free(cms);
        hll_free(hll);
        exact_free(exact);
        return 1;
    }
    
    char line[MAX_LINE_LENGTH];
    size_t total_packets = 0;
    
    /* Skip header */
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        cms_free(cms);
        hll_free(hll);
        exact_free(exact);
        return 1;
    }
    
    /* Process each packet */
    while (fgets(line, sizeof(line), file)) {
        char src_ip[16];
        
        /* Parse: timestamp,src_ip,dst_ip,bytes */
        if (sscanf(line, "%*[^,],%15[^,],%*s", src_ip) != 1) {
            continue;
        }
        
        uint32_t ip = parse_ip(src_ip);
        if (ip == 0) continue;
        
        /* Update structures */
        cms_update(cms, &ip, sizeof(ip), 1);
        hll_add(hll, &ip, sizeof(ip));
        
        if (exact && exact->size < MAX_IPS_EXACT) {
            exact_update(exact, ip);
        }
        
        total_packets++;
    }
    
    fclose(file);
    
    printf("  Processed %zu packets\n", total_packets);
    
    /* Report results */
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     RESULTS                                                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Cardinality estimation */
    uint64_t hll_estimate = hll_count(hll);
    size_t exact_unique = exact ? exact->size : 0;
    
    printf("Unique Source IPs:\n");
    printf("  HyperLogLog estimate: %lu\n", hll_estimate);
    printf("  Exact count:          %zu\n", exact_unique);
    if (exact_unique > 0) {
        double error = 100.0 * fabs((double)hll_estimate - exact_unique) / exact_unique;
        printf("  Error:                %.2f%%\n", error);
    }
    
    /* Heavy hitter detection */
    printf("\nHeavy Hitters (>1%% of traffic = >%zu packets):\n",
           total_packets / 100);
    printf("────────────────────────────────────────────────────────────────\n");
    
    /* Check known heavy hitter IPs */
    uint32_t test_ips[] = {
        parse_ip("192.168.1.100"),
        parse_ip("192.168.1.101"),
        parse_ip("10.0.0.50"),
        parse_ip("192.168.100.100")  /* Should not be heavy hitter */
    };
    
    for (size_t i = 0; i < 4; i++) {
        char ip_str[16];
        format_ip(test_ips[i], ip_str, sizeof(ip_str));
        
        uint32_t cms_count = cms_query(cms, &test_ips[i], sizeof(test_ips[i]));
        uint32_t exact_count = exact ? exact_query(exact, test_ips[i]) : 0;
        
        bool is_heavy = cms_count > total_packets / 100;
        
        printf("  %-15s: CMS=%5u, Exact=%5u %s\n",
               ip_str, cms_count, exact_count,
               is_heavy ? "[HEAVY HITTER]" : "");
    }
    
    /* Memory comparison */
    printf("\nMemory Usage Comparison:\n");
    printf("────────────────────────────────────────────────────────────────\n");
    
    size_t cms_memory = cms->width * cms->depth * sizeof(uint32_t);
    size_t hll_memory = hll->num_registers;
    size_t exact_memory = exact ? exact->capacity * sizeof(IPCount) : 0;
    
    printf("  Count-Min Sketch: %zu bytes\n", cms_memory);
    printf("  HyperLogLog:      %zu bytes\n", hll_memory);
    printf("  Exact counters:   %zu bytes\n", exact_memory);
    printf("  Savings factor:   %.1fx\n", 
           (double)exact_memory / (cms_memory + hll_memory));
    
    /* Cleanup */
    cms_free(cms);
    hll_free(hll);
    exact_free(exact);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    EXERCISE COMPLETE                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

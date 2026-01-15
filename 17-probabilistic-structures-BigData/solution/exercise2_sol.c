/**
 * =============================================================================
 * WEEK 17: PROBABILISTIC DATA STRUCTURES FOR BIG DATA
 * Exercise 2 SOLUTION: Network Traffic Analysis
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
#define MAX_IPS_EXACT 100000

/* Hash function */
uint64_t murmur_hash64(const void *data, size_t len, uint64_t seed) {
    const uint64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;
    uint64_t h = seed ^ (len * m);
    const uint64_t *blocks = (const uint64_t *)data;
    size_t nblocks = len / 8;
    
    for (size_t i = 0; i < nblocks; i++) {
        uint64_t k = blocks[i];
        k *= m; k ^= k >> r; k *= m;
        h ^= k; h *= m;
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
        case 1: k ^= (uint64_t)tail[0]; k *= m; h ^= k;
    }
    h ^= h >> r; h *= m; h ^= h >> r;
    return h;
}

uint64_t get_hash_i(const void *data, size_t len, int i) {
    uint64_t h1 = murmur_hash64(data, len, 0);
    uint64_t h2 = murmur_hash64(data, len, h1);
    return h1 + i * h2;
}

/* Count-Min Sketch */
typedef struct {
    uint32_t **table;
    size_t width;
    size_t depth;
    size_t total;
} CountMinSketch;

/* SOLUTION: Create CMS */
CountMinSketch *cms_create(size_t width, size_t depth) {
    CountMinSketch *cms = malloc(sizeof(CountMinSketch));
    if (!cms) return NULL;
    
    cms->width = width;
    cms->depth = depth;
    cms->total = 0;
    
    cms->table = malloc(depth * sizeof(uint32_t *));
    if (!cms->table) {
        free(cms);
        return NULL;
    }
    
    for (size_t i = 0; i < depth; i++) {
        cms->table[i] = calloc(width, sizeof(uint32_t));
        if (!cms->table[i]) {
            for (size_t j = 0; j < i; j++) free(cms->table[j]);
            free(cms->table);
            free(cms);
            return NULL;
        }
    }
    
    return cms;
}

/* SOLUTION: Create optimal CMS */
CountMinSketch *cms_create_optimal(double epsilon, double delta) {
    size_t width = (size_t)ceil(exp(1.0) / epsilon);
    size_t depth = (size_t)ceil(log(1.0 / delta));
    return cms_create(width, depth);
}

/* SOLUTION: Free CMS */
void cms_free(CountMinSketch *cms) {
    if (cms) {
        for (size_t i = 0; i < cms->depth; i++) {
            free(cms->table[i]);
        }
        free(cms->table);
        free(cms);
    }
}

/* SOLUTION: Update CMS */
void cms_update(CountMinSketch *cms, const void *data, size_t len, int32_t count) {
    for (size_t i = 0; i < cms->depth; i++) {
        uint64_t hash = get_hash_i(data, len, i);
        size_t col = hash % cms->width;
        cms->table[i][col] += count;
    }
    cms->total += count;
}

/* SOLUTION: Query CMS */
uint32_t cms_query(const CountMinSketch *cms, const void *data, size_t len) {
    uint32_t min_val = UINT32_MAX;
    for (size_t i = 0; i < cms->depth; i++) {
        uint64_t hash = get_hash_i(data, len, i);
        size_t col = hash % cms->width;
        if (cms->table[i][col] < min_val) {
            min_val = cms->table[i][col];
        }
    }
    return min_val;
}

/* HyperLogLog */
typedef struct {
    uint8_t *registers;
    uint8_t precision;
    uint32_t num_registers;
} HyperLogLog;

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

/* SOLUTION: Create HLL */
HyperLogLog *hll_create(uint8_t precision) {
    if (precision < 4 || precision > 18) precision = 14;
    
    HyperLogLog *hll = malloc(sizeof(HyperLogLog));
    if (!hll) return NULL;
    
    hll->precision = precision;
    hll->num_registers = 1U << precision;
    hll->registers = calloc(hll->num_registers, 1);
    
    if (!hll->registers) {
        free(hll);
        return NULL;
    }
    
    return hll;
}

/* SOLUTION: Free HLL */
void hll_free(HyperLogLog *hll) {
    if (hll) {
        free(hll->registers);
        free(hll);
    }
}

/* SOLUTION: Add to HLL */
void hll_add(HyperLogLog *hll, const void *data, size_t len) {
    uint64_t hash = murmur_hash64(data, len, 0x5f61767a);
    uint32_t idx = hash >> (64 - hll->precision);
    uint64_t remaining = (hash << hll->precision) | (1ULL << (hll->precision - 1));
    uint8_t rho = count_leading_zeros(remaining) + 1;
    
    if (rho > hll->registers[idx]) {
        hll->registers[idx] = rho;
    }
}

/* SOLUTION: Count HLL */
uint64_t hll_count(const HyperLogLog *hll) {
    double alpha;
    uint32_t m = hll->num_registers;
    
    switch (hll->precision) {
        case 4:  alpha = 0.673; break;
        case 5:  alpha = 0.697; break;
        case 6:  alpha = 0.709; break;
        default: alpha = 0.7213 / (1 + 1.079 / m); break;
    }
    
    double sum = 0;
    uint32_t zeros = 0;
    
    for (uint32_t i = 0; i < m; i++) {
        sum += 1.0 / (1ULL << hll->registers[i]);
        if (hll->registers[i] == 0) zeros++;
    }
    
    double estimate = alpha * m * m / sum;
    
    if (estimate <= 2.5 * m && zeros > 0) {
        estimate = m * log((double)m / zeros);
    }
    
    return (uint64_t)estimate;
}

/* Exact counter for comparison */
typedef struct { uint32_t ip; uint32_t count; } IPCount;
typedef struct { IPCount *entries; size_t size; size_t capacity; } ExactCounter;

ExactCounter *exact_create(size_t capacity) {
    ExactCounter *ec = malloc(sizeof(ExactCounter));
    if (!ec) return NULL;
    ec->entries = calloc(capacity, sizeof(IPCount));
    if (!ec->entries) { free(ec); return NULL; }
    ec->size = 0;
    ec->capacity = capacity;
    return ec;
}

void exact_free(ExactCounter *ec) {
    if (ec) { free(ec->entries); free(ec); }
}

void exact_update(ExactCounter *ec, uint32_t ip) {
    for (size_t i = 0; i < ec->size; i++) {
        if (ec->entries[i].ip == ip) { ec->entries[i].count++; return; }
    }
    if (ec->size < ec->capacity) {
        ec->entries[ec->size].ip = ip;
        ec->entries[ec->size].count = 1;
        ec->size++;
    }
}

uint32_t exact_query(ExactCounter *ec, uint32_t ip) {
    for (size_t i = 0; i < ec->size; i++) {
        if (ec->entries[i].ip == ip) return ec->entries[i].count;
    }
    return 0;
}

/* Utility functions */
uint32_t parse_ip(const char *ip_str) {
    unsigned int a, b, c, d;
    if (sscanf(ip_str, "%u.%u.%u.%u", &a, &b, &c, &d) != 4) return 0;
    return (a << 24) | (b << 16) | (c << 8) | d;
}

void format_ip(uint32_t ip, char *buffer, size_t size) {
    snprintf(buffer, size, "%u.%u.%u.%u",
             (ip >> 24) & 0xFF, (ip >> 16) & 0xFF,
             (ip >> 8) & 0xFF, ip & 0xFF);
}

void generate_traffic_data(const char *filename, size_t num_packets) {
    FILE *file = fopen(filename, "w");
    if (!file) return;
    
    fprintf(file, "timestamp,src_ip,dst_ip,bytes\n");
    srand((unsigned int)time(NULL));
    time_t base_time = time(NULL);
    
    uint32_t heavy_hitters[] = {
        parse_ip("192.168.1.100"),
        parse_ip("192.168.1.101"),
        parse_ip("10.0.0.50")
    };
    
    for (size_t i = 0; i < num_packets; i++) {
        uint32_t src_ip;
        if (rand() % 100 < 30) {
            src_ip = heavy_hitters[rand() % 3];
        } else {
            src_ip = (192 << 24) | (168 << 16) | (rand() % 256 << 8) | (rand() % 256);
        }
        
        uint32_t dst_ip = (10 << 24) | (0 << 16) | (0 << 8) | (rand() % 100);
        int bytes = 100 + rand() % 1400;
        
        char src_str[16], dst_str[16];
        format_ip(src_ip, src_str, sizeof(src_str));
        format_ip(dst_ip, dst_str, sizeof(dst_str));
        
        fprintf(file, "%ld,%s,%s,%d\n", (long)(base_time + i), src_str, dst_str, bytes);
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    const char *traffic_file;
    
    if (argc < 2) {
        traffic_file = "sample_traffic.csv";
        generate_traffic_data(traffic_file, 10000);
    } else {
        traffic_file = argv[1];
    }
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 2 SOLUTION: TRAFFIC ANALYSIS                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    CountMinSketch *cms = cms_create_optimal(0.01, 0.01);
    HyperLogLog *hll = hll_create(14);
    ExactCounter *exact = exact_create(MAX_IPS_EXACT);
    
    printf("CMS: %zu × %zu cells\n", cms->width, cms->depth);
    printf("HLL: %u registers\n", hll->num_registers);
    
    FILE *file = fopen(traffic_file, "r");
    if (!file) { perror("fopen"); return 1; }
    
    char line[MAX_LINE_LENGTH];
    size_t total_packets = 0;
    
    fgets(line, sizeof(line), file); /* Skip header */
    
    while (fgets(line, sizeof(line), file)) {
        char src_ip[16];
        if (sscanf(line, "%*[^,],%15[^,],%*s", src_ip) != 1) continue;
        
        uint32_t ip = parse_ip(src_ip);
        if (ip == 0) continue;
        
        cms_update(cms, &ip, sizeof(ip), 1);
        hll_add(hll, &ip, sizeof(ip));
        if (exact->size < MAX_IPS_EXACT) exact_update(exact, ip);
        
        total_packets++;
    }
    fclose(file);
    
    printf("\nProcessed %zu packets\n", total_packets);
    printf("\nUnique IPs: HLL=%lu, Exact=%zu\n", hll_count(hll), exact->size);
    
    printf("\nHeavy Hitters (>%zu packets):\n", total_packets / 100);
    
    uint32_t test_ips[] = {
        parse_ip("192.168.1.100"),
        parse_ip("192.168.1.101"),
        parse_ip("10.0.0.50")
    };
    
    for (int i = 0; i < 3; i++) {
        char ip_str[16];
        format_ip(test_ips[i], ip_str, sizeof(ip_str));
        uint32_t est = cms_query(cms, &test_ips[i], sizeof(test_ips[i]));
        uint32_t act = exact_query(exact, test_ips[i]);
        printf("  %s: CMS=%u, Exact=%u\n", ip_str, est, act);
    }
    
    cms_free(cms);
    hll_free(hll);
    exact_free(exact);
    
    printf("\n✓ Exercise 2 Solution Complete\n\n");
    return 0;
}

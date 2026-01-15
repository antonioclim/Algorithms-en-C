/**
 * =============================================================================
 * HOMEWORK 2: Binary Image Processor (PGM) - SOLUTION
 * =============================================================================
 *
 * INSTRUCTOR COPY - Contains complete implementation
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct {
    char     magic[3];      /* "P5\0" for binary PGM */
    int32_t  width;
    int32_t  height;
    int32_t  max_value;
    uint8_t *pixels;        /* width * height bytes */
} PGMImage;

/* =============================================================================
 * FUNCTION PROTOTYPES
 * =============================================================================
 */

/* File I/O */
int read_pgm(const char *filename, PGMImage *img);
int write_pgm(const char *filename, const PGMImage *img);
void free_pgm(PGMImage *img);

/* Image Processing */
void invert_image(PGMImage *img);
void threshold_image(PGMImage *img, uint8_t threshold);
int rotate_90(PGMImage *img);
void flip_horizontal(PGMImage *img);

/* Analysis */
void compute_histogram(const PGMImage *img, int histogram[256]);
double find_brightness(const PGMImage *img);
int find_contrast(const PGMImage *img);

/* =============================================================================
 * FILE I/O FUNCTIONS
 * =============================================================================
 */

/**
 * Skip whitespace and comments in PGM header
 */
static void skip_whitespace_and_comments(FILE *fp) {
    int c;
    
    /* Skip whitespace */
    while ((c = fgetc(fp)) != EOF && isspace(c));
    
    /* Check for comment */
    while (c == '#') {
        /* Skip until end of line */
        while ((c = fgetc(fp)) != EOF && c != '\n');
        /* Skip whitespace after comment */
        while ((c = fgetc(fp)) != EOF && isspace(c));
    }
    
    /* Put back the non-whitespace, non-comment character */
    if (c != EOF) {
        ungetc(c, fp);
    }
}

/**
 * Read a PGM file into the image structure
 */
int read_pgm(const char *filename, PGMImage *img) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Error opening file");
        return -1;
    }
    
    /* Initialise structure */
    memset(img, 0, sizeof(PGMImage));
    
    /* Read magic number */
    char magic[3];
    if (fread(magic, 1, 2, fp) != 2) {
        fclose(fp);
        return -1;
    }
    magic[2] = '\0';
    
    if (strcmp(magic, "P5") != 0) {
        fprintf(stderr, "Error: not a binary PGM file (magic: %s)\n", magic);
        fclose(fp);
        return -1;
    }
    
    strcpy(img->magic, "P5");
    
    /* Skip whitespace/comments and read width */
    skip_whitespace_and_comments(fp);
    if (fscanf(fp, "%d", &img->width) != 1 || img->width <= 0) {
        fprintf(stderr, "Error: invalid width\n");
        fclose(fp);
        return -1;
    }
    
    /* Skip whitespace/comments and read height */
    skip_whitespace_and_comments(fp);
    if (fscanf(fp, "%d", &img->height) != 1 || img->height <= 0) {
        fprintf(stderr, "Error: invalid height\n");
        fclose(fp);
        return -1;
    }
    
    /* Skip whitespace/comments and read max value */
    skip_whitespace_and_comments(fp);
    if (fscanf(fp, "%d", &img->max_value) != 1 || 
        img->max_value <= 0 || img->max_value > 255) {
        fprintf(stderr, "Error: invalid max value\n");
        fclose(fp);
        return -1;
    }
    
    /* Skip single whitespace character before binary data */
    fgetc(fp);
    
    /* Allocate pixel buffer */
    size_t pixel_count = (size_t)img->width * img->height;
    img->pixels = malloc(pixel_count);
    
    if (img->pixels == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        fclose(fp);
        return -1;
    }
    
    /* Read pixel data */
    size_t read = fread(img->pixels, 1, pixel_count, fp);
    
    if (read != pixel_count) {
        fprintf(stderr, "Error: incomplete pixel data (read %zu of %zu)\n",
                read, pixel_count);
        free(img->pixels);
        img->pixels = NULL;
        fclose(fp);
        return -1;
    }
    
    fclose(fp);
    return 0;
}

/**
 * Write the image structure to a PGM file
 */
int write_pgm(const char *filename, const PGMImage *img) {
    if (img == NULL || img->pixels == NULL) {
        return -1;
    }
    
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        perror("Error opening file for writing");
        return -1;
    }
    
    /* Write header */
    fprintf(fp, "P5\n");
    fprintf(fp, "# Created by PGM Processor\n");
    fprintf(fp, "%d %d\n", img->width, img->height);
    fprintf(fp, "%d\n", img->max_value);
    
    /* Write pixel data */
    size_t pixel_count = (size_t)img->width * img->height;
    size_t written = fwrite(img->pixels, 1, pixel_count, fp);
    
    fclose(fp);
    
    if (written != pixel_count) {
        fprintf(stderr, "Error: incomplete write\n");
        return -1;
    }
    
    return 0;
}

/**
 * Free image resources
 */
void free_pgm(PGMImage *img) {
    if (img != NULL) {
        if (img->pixels != NULL) {
            free(img->pixels);
            img->pixels = NULL;
        }
        img->width = 0;
        img->height = 0;
    }
}

/* =============================================================================
 * IMAGE PROCESSING FUNCTIONS
 * =============================================================================
 */

/**
 * Invert image (create negative)
 */
void invert_image(PGMImage *img) {
    if (img == NULL || img->pixels == NULL) return;
    
    size_t pixel_count = (size_t)img->width * img->height;
    
    for (size_t i = 0; i < pixel_count; i++) {
        img->pixels[i] = (uint8_t)(img->max_value - img->pixels[i]);
    }
}

/**
 * Convert to black and white using threshold
 */
void threshold_image(PGMImage *img, uint8_t threshold) {
    if (img == NULL || img->pixels == NULL) return;
    
    size_t pixel_count = (size_t)img->width * img->height;
    
    for (size_t i = 0; i < pixel_count; i++) {
        img->pixels[i] = (img->pixels[i] >= threshold) ? 
                         (uint8_t)img->max_value : 0;
    }
}

/**
 * Rotate image 90 degrees clockwise
 */
int rotate_90(PGMImage *img) {
    if (img == NULL || img->pixels == NULL) return -1;
    
    int new_width = img->height;
    int new_height = img->width;
    
    /* Allocate new pixel buffer */
    uint8_t *new_pixels = malloc((size_t)new_width * new_height);
    if (new_pixels == NULL) {
        return -1;
    }
    
    /* Copy pixels with rotation */
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            /* Original position */
            size_t old_idx = (size_t)y * img->width + x;
            
            /* New position: (x, y) -> (height-1-y, x) in new coords */
            /* which is (new_height - 1 - y, x) but we're rotating so:
             * new_x = height - 1 - y
             * new_y = x */
            int new_x = img->height - 1 - y;
            int new_y = x;
            size_t new_idx = (size_t)new_y * new_width + new_x;
            
            new_pixels[new_idx] = img->pixels[old_idx];
        }
    }
    
    /* Replace old pixels */
    free(img->pixels);
    img->pixels = new_pixels;
    img->width = new_width;
    img->height = new_height;
    
    return 0;
}

/**
 * Flip image horizontally (mirror)
 */
void flip_horizontal(PGMImage *img) {
    if (img == NULL || img->pixels == NULL) return;
    
    for (int y = 0; y < img->height; y++) {
        /* Swap pixels from left and right sides */
        for (int x = 0; x < img->width / 2; x++) {
            size_t left_idx = (size_t)y * img->width + x;
            size_t right_idx = (size_t)y * img->width + (img->width - 1 - x);
            
            uint8_t temp = img->pixels[left_idx];
            img->pixels[left_idx] = img->pixels[right_idx];
            img->pixels[right_idx] = temp;
        }
    }
}

/* =============================================================================
 * ANALYSIS FUNCTIONS
 * =============================================================================
 */

/**
 * Compute histogram of grey levels
 */
void compute_histogram(const PGMImage *img, int histogram[256]) {
    /* Initialise histogram to zeros */
    memset(histogram, 0, 256 * sizeof(int));
    
    if (img == NULL || img->pixels == NULL) return;
    
    size_t pixel_count = (size_t)img->width * img->height;
    
    for (size_t i = 0; i < pixel_count; i++) {
        histogram[img->pixels[i]]++;
    }
}

/**
 * Calculate average brightness
 */
double find_brightness(const PGMImage *img) {
    if (img == NULL || img->pixels == NULL) return 0.0;
    
    size_t pixel_count = (size_t)img->width * img->height;
    
    if (pixel_count == 0) return 0.0;
    
    double sum = 0.0;
    for (size_t i = 0; i < pixel_count; i++) {
        sum += img->pixels[i];
    }
    
    return sum / pixel_count;
}

/**
 * Calculate contrast (range: max - min)
 */
int find_contrast(const PGMImage *img) {
    if (img == NULL || img->pixels == NULL) return 0;
    
    size_t pixel_count = (size_t)img->width * img->height;
    
    if (pixel_count == 0) return 0;
    
    uint8_t min_val = 255;
    uint8_t max_val = 0;
    
    for (size_t i = 0; i < pixel_count; i++) {
        if (img->pixels[i] < min_val) min_val = img->pixels[i];
        if (img->pixels[i] > max_val) max_val = img->pixels[i];
    }
    
    return max_val - min_val;
}

/* =============================================================================
 * DEMONSTRATION FUNCTIONS
 * =============================================================================
 */

/**
 * Create a sample test image
 */
static void create_test_image(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) return;
    
    int width = 64;
    int height = 64;
    
    fprintf(fp, "P5\n");
    fprintf(fp, "# Test image with gradient\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "255\n");
    
    /* Create gradient pattern */
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            /* Diagonal gradient */
            uint8_t value = (uint8_t)((x + y) * 255 / (width + height - 2));
            fputc(value, fp);
        }
    }
    
    fclose(fp);
}

/**
 * Print image information
 */
static void print_image_info(const PGMImage *img, const char *name) {
    printf("\n%s:\n", name);
    printf("  Dimensions: %d x %d\n", img->width, img->height);
    printf("  Max value: %d\n", img->max_value);
    printf("  Pixel count: %d\n", img->width * img->height);
    printf("  Brightness: %.2f\n", find_brightness(img));
    printf("  Contrast: %d\n", find_contrast(img));
}

/**
 * Print histogram (simplified)
 */
static void print_histogram_summary(const int histogram[256]) {
    printf("\n  Histogram (simplified):\n");
    
    /* Find ranges */
    int dark = 0, mid = 0, bright = 0;
    
    for (int i = 0; i < 256; i++) {
        if (i < 85) dark += histogram[i];
        else if (i < 170) mid += histogram[i];
        else bright += histogram[i];
    }
    
    int total = dark + mid + bright;
    if (total > 0) {
        printf("    Dark (0-84):    %5d (%5.1f%%)\n", dark, 100.0 * dark / total);
        printf("    Mid (85-169):   %5d (%5.1f%%)\n", mid, 100.0 * mid / total);
        printf("    Bright (170-255): %5d (%5.1f%%)\n", bright, 100.0 * bright / total);
    }
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                   PGM IMAGE PROCESSOR                          ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    
    const char *test_file = "test_image.pgm";
    const char *inverted_file = "test_inverted.pgm";
    const char *threshold_file = "test_threshold.pgm";
    const char *rotated_file = "test_rotated.pgm";
    const char *flipped_file = "test_flipped.pgm";
    
    /* Create test image */
    printf("\n[INFO] Creating test image...\n");
    create_test_image(test_file);
    
    /* Read the image */
    PGMImage img;
    if (read_pgm(test_file, &img) != 0) {
        printf("[ERROR] Failed to read test image\n");
        return 1;
    }
    
    printf("[OK] Image loaded successfully\n");
    print_image_info(&img, "Original Image");
    
    /* Compute and display histogram */
    int histogram[256];
    compute_histogram(&img, histogram);
    print_histogram_summary(histogram);
    
    /* Test inversion */
    printf("\n[INFO] Testing image inversion...\n");
    PGMImage inverted = img;
    inverted.pixels = malloc((size_t)img.width * img.height);
    memcpy(inverted.pixels, img.pixels, (size_t)img.width * img.height);
    
    invert_image(&inverted);
    write_pgm(inverted_file, &inverted);
    printf("[OK] Inverted image saved to %s\n", inverted_file);
    print_image_info(&inverted, "Inverted Image");
    free(inverted.pixels);
    
    /* Test threshold */
    printf("\n[INFO] Testing threshold conversion...\n");
    PGMImage threshold_img = img;
    threshold_img.pixels = malloc((size_t)img.width * img.height);
    memcpy(threshold_img.pixels, img.pixels, (size_t)img.width * img.height);
    
    threshold_image(&threshold_img, 128);
    write_pgm(threshold_file, &threshold_img);
    printf("[OK] Threshold image saved to %s\n", threshold_file);
    print_image_info(&threshold_img, "Threshold Image (128)");
    free(threshold_img.pixels);
    
    /* Test rotation */
    printf("\n[INFO] Testing 90° rotation...\n");
    PGMImage rotated = img;
    rotated.pixels = malloc((size_t)img.width * img.height);
    memcpy(rotated.pixels, img.pixels, (size_t)img.width * img.height);
    
    rotate_90(&rotated);
    write_pgm(rotated_file, &rotated);
    printf("[OK] Rotated image saved to %s\n", rotated_file);
    print_image_info(&rotated, "Rotated Image (90° CW)");
    free(rotated.pixels);
    
    /* Test horizontal flip */
    printf("\n[INFO] Testing horizontal flip...\n");
    PGMImage flipped = img;
    flipped.pixels = malloc((size_t)img.width * img.height);
    memcpy(flipped.pixels, img.pixels, (size_t)img.width * img.height);
    
    flip_horizontal(&flipped);
    write_pgm(flipped_file, &flipped);
    printf("[OK] Flipped image saved to %s\n", flipped_file);
    free(flipped.pixels);
    
    /* Clean up */
    free_pgm(&img);
    
    printf("\n[INFO] All operations completed successfully\n");
    printf("\nGenerated files:\n");
    printf("  • %s (original)\n", test_file);
    printf("  • %s\n", inverted_file);
    printf("  • %s\n", threshold_file);
    printf("  • %s\n", rotated_file);
    printf("  • %s\n", flipped_file);
    
    printf("\n");
    
    return 0;
}

/**
 * =============================================================================
 * SOLUTION: Tema 1 - Generic Filter System
 * =============================================================================
 * INSTRUCTOR USE ONLY - Do not distribute to students
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =============================================================================
 * PRODUCT STRUCTURE
 * =============================================================================
 */

typedef struct {
    int id;
    char name[50];
    float price;
    int quantity;
} Product;

/* =============================================================================
 * GENERIC FILTER FUNCTION
 * =============================================================================
 */

/**
 * Generic filter function that works with any data type
 *
 * @param src Source array
 * @param dest Destination array (pre-allocated)
 * @param n Number of elements in source
 * @param elem_size Size of each element in bytes
 * @param predicate Function that returns 1 to keep element, 0 to discard
 * @return Number of elements in destination array
 */
int filter_array(void *src, void *dest, int n, size_t elem_size,
                 int (*predicate)(const void *)) {
    char *src_ptr = (char *)src;
    char *dest_ptr = (char *)dest;
    int count = 0;

    for (int i = 0; i < n; i++) {
        void *current = src_ptr + i * elem_size;

        if (predicate(current)) {
            memcpy(dest_ptr + count * elem_size, current, elem_size);
            count++;
        }
    }

    return count;
}

/* =============================================================================
 * INTEGER PREDICATES
 * =============================================================================
 */

int is_positive(const void *elem) {
    return *(const int *)elem > 0;
}

int is_even(const void *elem) {
    return *(const int *)elem % 2 == 0;
}

int is_greater_than_10(const void *elem) {
    return *(const int *)elem > 10;
}

int is_negative(const void *elem) {
    return *(const int *)elem < 0;
}

/* =============================================================================
 * PRODUCT PREDICATES
 * =============================================================================
 */

int is_in_stock(const void *elem) {
    const Product *p = (const Product *)elem;
    return p->quantity > 0;
}

int is_expensive(const void *elem) {
    const Product *p = (const Product *)elem;
    return p->price > 100.0f;
}

int is_low_stock(const void *elem) {
    const Product *p = (const Product *)elem;
    return p->quantity > 0 && p->quantity < 5;
}

int is_cheap(const void *elem) {
    const Product *p = (const Product *)elem;
    return p->price <= 50.0f;
}

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

void print_int_array(int *arr, int n, const char *title) {
    printf("%s: ", title);
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void print_products(Product *products, int n, const char *title) {
    printf("\n%s (%d products):\n", title, n);
    printf("  ┌──────┬────────────────────────┬──────────┬──────────┐\n");
    printf("  │  ID  │ Name                   │   Price  │ Quantity │\n");
    printf("  ├──────┼────────────────────────┼──────────┼──────────┤\n");

    for (int i = 0; i < n; i++) {
        printf("  │ %4d │ %-22s │ %8.2f │ %8d │\n",
               products[i].id, products[i].name,
               products[i].price, products[i].quantity);
    }

    printf("  └──────┴────────────────────────┴──────────┴──────────┘\n");
}

/* =============================================================================
 * MAIN DEMONSTRATION
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           GENERIC FILTER SYSTEM - SOLUTION                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    /* ==========================================================
     * PART 1: Filtering Integers
     * ==========================================================
     */
    printf("\n─── PART 1: Filtering Integers ───\n\n");

    int numbers[] = {-5, 12, 3, -8, 7, 0, 15, -2, 9, 4};
    int n = sizeof(numbers) / sizeof(numbers[0]);
    int filtered[10];
    int count;

    print_int_array(numbers, n, "Original");

    /* Filter positive numbers */
    count = filter_array(numbers, filtered, n, sizeof(int), is_positive);
    print_int_array(filtered, count, "Positive");

    /* Filter even numbers */
    count = filter_array(numbers, filtered, n, sizeof(int), is_even);
    print_int_array(filtered, count, "Even    ");

    /* Filter > 10 */
    count = filter_array(numbers, filtered, n, sizeof(int), is_greater_than_10);
    print_int_array(filtered, count, "> 10    ");

    /* Filter negative */
    count = filter_array(numbers, filtered, n, sizeof(int), is_negative);
    print_int_array(filtered, count, "Negative");

    /* ==========================================================
     * PART 2: Filtering Products
     * ==========================================================
     */
    printf("\n─── PART 2: Filtering Products ───\n");

    Product products[] = {
        {1, "Laptop ASUS",      2500.00, 10},
        {2, "Mouse Logitech",   45.00,   50},
        {3, "Keyboard Corsair", 150.00,  0},
        {4, "Monitor LG",       800.00,  3},
        {5, "USB Cable",        15.00,   100},
        {6, "Webcam HD",        120.00,  2},
        {7, "Mousepad XL",      25.00,   0},
        {8, "Headphones Sony",  350.00,  4}
    };
    int num_products = sizeof(products) / sizeof(products[0]);
    Product filtered_products[10];

    print_products(products, num_products, "All Products");

    /* Filter in stock */
    count = filter_array(products, filtered_products, num_products,
                         sizeof(Product), is_in_stock);
    print_products(filtered_products, count, "In Stock");

    /* Filter expensive (> 100) */
    count = filter_array(products, filtered_products, num_products,
                         sizeof(Product), is_expensive);
    print_products(filtered_products, count, "Expensive (> 100)");

    /* Filter low stock */
    count = filter_array(products, filtered_products, num_products,
                         sizeof(Product), is_low_stock);
    print_products(filtered_products, count, "Low Stock (< 5)");

    /* Filter cheap (<= 50) */
    count = filter_array(products, filtered_products, num_products,
                         sizeof(Product), is_cheap);
    print_products(filtered_products, count, "Cheap (<= 50)");

    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    DEMONSTRATION COMPLETE                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    return 0;
}

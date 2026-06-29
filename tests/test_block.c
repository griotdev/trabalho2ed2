#include "block.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static int failures = 0;

#define ASSERT_TRUE(value) assert_true((value), #value, __LINE__)
#define ASSERT_FALSE(value) assert_false((value), #value, __LINE__)
#define ASSERT_STR_EQ(expected, actual) assert_str_eq((expected), (actual), #actual, __LINE__)
#define ASSERT_DOUBLE_EQ(expected, actual) assert_double_eq((expected), (actual), #actual, __LINE__)

static void assert_true(int value, const char *expr, int line) {
    if (!value) {
        printf("Falha linha %d: esperado verdadeiro para %s\n", line, expr);
        failures++;
    }
}

static void assert_false(int value, const char *expr, int line) {
    if (value) {
        printf("Falha linha %d: esperado falso para %s\n", line, expr);
        failures++;
    }
}

static void assert_str_eq(const char *expected, const char *actual, const char *expr, int line) {
    if (actual == NULL || strcmp(expected, actual) != 0) {
        printf("Falha linha %d: esperado \"%s\" em %s, recebido \"%s\"\n",
               line,
               expected,
               expr,
               actual == NULL ? "(null)" : actual);
        failures++;
    }
}

static void assert_double_eq(double expected, double actual, const char *expr, int line) {
    if (fabs(expected - actual) > 0.000001) {
        printf("Falha linha %d: esperado %.6f em %s, recebido %.6f\n", line, expected, expr, actual);
        failures++;
    }
}

static void test_preserves_block_fields(void) {
    Block *block = block_create("cep15", 100.0, 200.0, 40.0, 30.0);

    ASSERT_TRUE(block != NULL);
    ASSERT_STR_EQ("cep15", block_cep(block));
    ASSERT_DOUBLE_EQ(100.0, block_x(block));
    ASSERT_DOUBLE_EQ(200.0, block_y(block));
    ASSERT_DOUBLE_EQ(40.0, block_width(block));
    ASSERT_DOUBLE_EQ(30.0, block_height(block));

    block_destroy(block);
}

static void test_calculates_address_from_southeast_anchor(void) {
    Block *block = block_create("cep15", 100.0, 200.0, 40.0, 30.0);
    double x = 0.0;
    double y = 0.0;

    ASSERT_TRUE(block_address_point(block, 'S', 10.0, &x, &y));
    ASSERT_DOUBLE_EQ(90.0, x);
    ASSERT_DOUBLE_EQ(200.0, y);

    ASSERT_TRUE(block_address_point(block, 'N', 10.0, &x, &y));
    ASSERT_DOUBLE_EQ(90.0, x);
    ASSERT_DOUBLE_EQ(230.0, y);

    ASSERT_TRUE(block_address_point(block, 'L', 10.0, &x, &y));
    ASSERT_DOUBLE_EQ(100.0, x);
    ASSERT_DOUBLE_EQ(210.0, y);

    ASSERT_TRUE(block_address_point(block, 'O', 10.0, &x, &y));
    ASSERT_DOUBLE_EQ(60.0, x);
    ASSERT_DOUBLE_EQ(210.0, y);

    block_destroy(block);
}

static void test_rejects_invalid_face(void) {
    Block *block = block_create("cep15", 100.0, 200.0, 40.0, 30.0);
    double x = 0.0;
    double y = 0.0;

    ASSERT_FALSE(block_address_point(block, 'X', 10.0, &x, &y));

    block_destroy(block);
}

int main(void) {
    test_preserves_block_fields();
    test_calculates_address_from_southeast_anchor();
    test_rejects_invalid_face();

    if (failures == 0) {
        puts("test_block: todos os testes passaram");
    }

    return failures == 0 ? 0 : 1;
}

#include "block.h"
#include "unity.h"

void setUp(void) {
}

void tearDown(void) {
}

static void test_preserves_block_fields(void) {
    Block *block = block_create("cep15", 100.0, 200.0, 40.0, 30.0);

    TEST_ASSERT_NOT_NULL(block);
    TEST_ASSERT_EQUAL_STRING("cep15", block_cep(block));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 100.0, block_x(block));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 200.0, block_y(block));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 40.0, block_width(block));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 30.0, block_height(block));

    block_destroy(block);
}

static void test_calculates_address_from_southeast_anchor(void) {
    Block *block = block_create("cep15", 100.0, 200.0, 40.0, 30.0);
    double x = 0.0;
    double y = 0.0;

    TEST_ASSERT_NOT_NULL(block);

    TEST_ASSERT_TRUE(block_address_point(block, 'S', 10.0, &x, &y));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 90.0, x);
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 200.0, y);

    TEST_ASSERT_TRUE(block_address_point(block, 'N', 10.0, &x, &y));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 90.0, x);
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 230.0, y);

    TEST_ASSERT_TRUE(block_address_point(block, 'L', 10.0, &x, &y));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 100.0, x);
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 210.0, y);

    TEST_ASSERT_TRUE(block_address_point(block, 'O', 10.0, &x, &y));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 60.0, x);
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 210.0, y);

    block_destroy(block);
}

static void test_rejects_invalid_face(void) {
    Block *block = block_create("cep15", 100.0, 200.0, 40.0, 30.0);
    double x = 0.0;
    double y = 0.0;

    TEST_ASSERT_NOT_NULL(block);
    TEST_ASSERT_FALSE(block_address_point(block, 'X', 10.0, &x, &y));

    block_destroy(block);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_preserves_block_fields);
    RUN_TEST(test_calculates_address_from_southeast_anchor);
    RUN_TEST(test_rejects_invalid_face);
    return UNITY_END();
}

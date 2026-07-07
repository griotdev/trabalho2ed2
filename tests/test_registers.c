#include "registers.h"
#include "unity.h"

void setUp(void) {
}

void tearDown(void) {
}

static void test_stores_and_reads_coordinates(void) {
    Registers registers = registers_create();

    TEST_ASSERT_NOT_NULL(registers);
    TEST_ASSERT_FALSE(registers_is_set(registers, 3));
    TEST_ASSERT_TRUE(registers_set(registers, 3, 12.5, 40.75));
    TEST_ASSERT_TRUE(registers_is_set(registers, 3));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 12.5, registers_x(registers, 3));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 40.75, registers_y(registers, 3));

    registers_destroy(registers);
}

static void test_rejects_invalid_index(void) {
    Registers registers = registers_create();

    TEST_ASSERT_NOT_NULL(registers);
    TEST_ASSERT_FALSE(registers_set(registers, -1, 1.0, 2.0));
    TEST_ASSERT_FALSE(registers_set(registers, 11, 1.0, 2.0));
    TEST_ASSERT_FALSE(registers_is_set(registers, 11));

    registers_destroy(registers);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_stores_and_reads_coordinates);
    RUN_TEST(test_rejects_invalid_index);
    return UNITY_END();
}

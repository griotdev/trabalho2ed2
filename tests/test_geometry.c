#include "geometry.h"
#include "unity.h"

void setUp(void) {
}

void tearDown(void) {
}

static void test_point_preserves_coordinates(void) {
    Point point = point_create(12.5, 30.75);

    TEST_ASSERT_NOT_NULL(point);
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 12.5, point_x(point));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 30.75, point_y(point));

    point_destroy(point);
}

static void test_rect_preserves_fields(void) {
    Rect rect = rect_create(10.0, 20.0, 30.0, 40.0);

    TEST_ASSERT_NOT_NULL(rect);
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 10.0, rect_x(rect));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 20.0, rect_y(rect));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 30.0, rect_width(rect));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 40.0, rect_height(rect));

    rect_destroy(rect);
}

static void test_rect_contains_internal_and_border_points(void) {
    Rect rect = rect_create(10.0, 20.0, 30.0, 40.0);

    TEST_ASSERT_NOT_NULL(rect);
    TEST_ASSERT_TRUE(rect_contains_point(rect, 25.0, 35.0));
    TEST_ASSERT_TRUE(rect_contains_point(rect, 10.0, 20.0));
    TEST_ASSERT_TRUE(rect_contains_point(rect, 40.0, 60.0));

    rect_destroy(rect);
}

static void test_rect_rejects_external_points(void) {
    Rect rect = rect_create(10.0, 20.0, 30.0, 40.0);

    TEST_ASSERT_NOT_NULL(rect);
    TEST_ASSERT_FALSE(rect_contains_point(rect, 9.9, 35.0));
    TEST_ASSERT_FALSE(rect_contains_point(rect, 25.0, 60.1));

    rect_destroy(rect);
}

static void test_rejects_negative_dimensions(void) {
    TEST_ASSERT_NULL(rect_create(0.0, 0.0, -1.0, 10.0));
    TEST_ASSERT_NULL(rect_create(0.0, 0.0, 10.0, -1.0));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_point_preserves_coordinates);
    RUN_TEST(test_rect_preserves_fields);
    RUN_TEST(test_rect_contains_internal_and_border_points);
    RUN_TEST(test_rect_rejects_external_points);
    RUN_TEST(test_rejects_negative_dimensions);
    return UNITY_END();
}

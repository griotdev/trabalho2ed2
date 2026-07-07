#include "geo.h"
#include "unity.h"

#include <stdio.h>

static const char *test_geo_path = "../out/test_geo_basic.geo";

void setUp(void) {
}

void tearDown(void) {
    remove(test_geo_path);
}

static void write_file(const char *content) {
    FILE *file = fopen(test_geo_path, "w");

    TEST_ASSERT_NOT_NULL(file);
    TEST_ASSERT_NOT_EQUAL(EOF, fputs(content, file));
    TEST_ASSERT_EQUAL(0, fclose(file));
}

static void test_loads_blocks_from_geo(void) {
    Geo geo;
    Block first;
    Block second;

    write_file("q cep1 10 20 30 40\nq cep2 50 60 70 80\n");
    geo = geo_load(test_geo_path);

    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NULL(geo_error(geo));
    TEST_ASSERT_EQUAL_INT(2, geo_block_count(geo));

    first = geo_block_at(geo, 0);
    second = geo_block_at(geo, 1);
    TEST_ASSERT_EQUAL_STRING("cep1", block_cep(first));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 10.0, block_x(first));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 40.0, block_height(first));
    TEST_ASSERT_EQUAL_STRING("cep2", block_cep(second));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 70.0, block_width(second));

    geo_destroy(geo);
}

static void test_applies_cq_style_to_following_blocks(void) {
    Geo geo;
    Block first;
    Block second;

    write_file("cq 2.5px red black\nq cep1 10 20 30 40\ncq 1 blue gray\nq cep2 50 60 70 80\n");
    geo = geo_load(test_geo_path);

    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NULL(geo_error(geo));

    first = geo_find_block(geo, "cep1");
    second = geo_find_block(geo, "cep2");
    TEST_ASSERT_NOT_NULL(first);
    TEST_ASSERT_NOT_NULL(second);
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 2.5, block_stroke_width(first));
    TEST_ASSERT_EQUAL_STRING("red", block_fill_color(first));
    TEST_ASSERT_EQUAL_STRING("black", block_stroke_color(first));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 1.0, block_stroke_width(second));
    TEST_ASSERT_EQUAL_STRING("blue", block_fill_color(second));
    TEST_ASSERT_EQUAL_STRING("gray", block_stroke_color(second));

    geo_destroy(geo);
}

static void test_reports_missing_file(void) {
    Geo geo = geo_load("../out/arquivo_inexistente.geo");

    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NOT_NULL(geo_error(geo));
    TEST_ASSERT_EQUAL_INT(0, geo_block_count(geo));

    geo_destroy(geo);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_loads_blocks_from_geo);
    RUN_TEST(test_applies_cq_style_to_following_blocks);
    RUN_TEST(test_reports_missing_file);
    return UNITY_END();
}

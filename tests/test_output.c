#include "geo.h"
#include "output.h"
#include "registers.h"
#include "unity.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *test_geo_path = "../out/test_output.geo";
static const char *test_txt_path = "../out/test_output.txt";
static const char *test_svg_path = "../out/test_output.svg";

void setUp(void) {
}

void tearDown(void) {
    remove(test_geo_path);
    remove(test_txt_path);
    remove(test_svg_path);
}

static void write_file(const char *path, const char *content) {
    FILE *file = fopen(path, "w");

    TEST_ASSERT_NOT_NULL(file);
    TEST_ASSERT_NOT_EQUAL(EOF, fputs(content, file));
    TEST_ASSERT_EQUAL(0, fclose(file));
}

static char *read_file(const char *path) {
    FILE *file = fopen(path, "rb");
    char *content;
    long length;

    TEST_ASSERT_NOT_NULL(file);
    TEST_ASSERT_EQUAL(0, fseek(file, 0, SEEK_END));
    length = ftell(file);
    TEST_ASSERT_GREATER_OR_EQUAL(0, length);
    TEST_ASSERT_EQUAL(0, fseek(file, 0, SEEK_SET));

    content = malloc((size_t)length + 1U);
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_EQUAL((size_t)length, fread(content, 1U, (size_t)length, file));
    content[length] = '\0';
    TEST_ASSERT_EQUAL(0, fclose(file));
    return content;
}

static Geo *load_sample_geo(void) {
    write_file(test_geo_path, "cq 2 red black\nq cep1 10 20 30 40\n");
    return geo_load(test_geo_path);
}

static void test_writes_txt_with_block_summary(void) {
    Geo *geo = load_sample_geo();
    char *content;

    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NULL(geo_error(geo));
    TEST_ASSERT_TRUE(output_write_txt(test_txt_path, geo));
    TEST_ASSERT_NULL(output_error());

    content = read_file(test_txt_path);
    TEST_ASSERT_NOT_NULL(strstr(content, "Quadras: 1"));
    TEST_ASSERT_NOT_NULL(strstr(content, "q cep1 10.00 20.00 30.00 40.00"));

    free(content);
    geo_destroy(geo);
}

static void test_writes_svg_rectangles_with_style(void) {
    Geo *geo = load_sample_geo();
    char *content;

    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NULL(geo_error(geo));
    TEST_ASSERT_TRUE(output_write_svg(test_svg_path, geo));
    TEST_ASSERT_NULL(output_error());

    content = read_file(test_svg_path);
    TEST_ASSERT_NOT_NULL(strstr(content, "<svg"));
    TEST_ASSERT_NOT_NULL(strstr(content, "<rect"));
    TEST_ASSERT_NOT_NULL(strstr(content, "fill=\"red\""));
    TEST_ASSERT_NOT_NULL(strstr(content, "stroke=\"black\""));
    TEST_ASSERT_NOT_NULL(strstr(content, "stroke-width=\"2.00\""));

    free(content);
    geo_destroy(geo);
}

static void test_writes_svg_register_markers(void) {
    Geo *geo = load_sample_geo();
    Registers *registers = registers_create();
    char *content;

    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NOT_NULL(registers);
    TEST_ASSERT_TRUE(registers_set(registers, 2, 90.0, 200.0));
    TEST_ASSERT_TRUE(output_write_svg_with_registers(test_svg_path, geo, registers));
    TEST_ASSERT_NULL(output_error());

    content = read_file(test_svg_path);
    TEST_ASSERT_NOT_NULL(strstr(content, "<line x1=\"90.00\""));
    TEST_ASSERT_NOT_NULL(strstr(content, "stroke=\"red\""));
    TEST_ASSERT_NOT_NULL(strstr(content, "stroke-dasharray=\"4 4\""));
    TEST_ASSERT_NOT_NULL(strstr(content, ">R2</text>"));

    free(content);
    registers_destroy(registers);
    geo_destroy(geo);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_writes_txt_with_block_summary);
    RUN_TEST(test_writes_svg_rectangles_with_style);
    RUN_TEST(test_writes_svg_register_markers);
    return UNITY_END();
}

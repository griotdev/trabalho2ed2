#include "args.h"
#include "unity.h"

void setUp(void) {
}

void tearDown(void) {
}

static void test_accepts_required_arguments(void) {
    char *argv[] = {"ted", "-f", "cidade.geo", "-o", "saida"};
    AppArgs *args = args_create();

    TEST_ASSERT_NOT_NULL(args);
    TEST_ASSERT_TRUE(args_parse(args, 5, argv));
    TEST_ASSERT_EQUAL_STRING("cidade.geo", args_geo_file(args));
    TEST_ASSERT_EQUAL_STRING("saida", args_output_dir(args));

    args_destroy(args);
}

static void test_accepts_optional_arguments(void) {
    char *argv[] = {"ted", "-e", "entrada", "-f", "cidade.geo", "-q", "consulta.qry", "-v", "vias.via", "-o", "saida"};
    AppArgs *args = args_create();

    TEST_ASSERT_NOT_NULL(args);
    TEST_ASSERT_TRUE(args_parse(args, 11, argv));
    TEST_ASSERT_EQUAL_STRING("entrada", args_input_dir(args));
    TEST_ASSERT_EQUAL_STRING("consulta.qry", args_query_file(args));
    TEST_ASSERT_EQUAL_STRING("vias.via", args_via_file(args));

    args_destroy(args);
}

static void test_rejects_missing_geo_file(void) {
    char *argv[] = {"ted", "-o", "saida"};
    AppArgs *args = args_create();

    TEST_ASSERT_NOT_NULL(args);
    TEST_ASSERT_FALSE(args_parse(args, 3, argv));
    TEST_ASSERT_NOT_NULL(args_error(args));

    args_destroy(args);
}

static void test_rejects_missing_option_value(void) {
    char *argv[] = {"ted", "-f", "cidade.geo", "-o"};
    AppArgs *args = args_create();

    TEST_ASSERT_NOT_NULL(args);
    TEST_ASSERT_FALSE(args_parse(args, 4, argv));
    TEST_ASSERT_NOT_NULL(args_error(args));

    args_destroy(args);
}

static void test_rejects_unknown_option(void) {
    char *argv[] = {"ted", "-x", "valor", "-f", "cidade.geo", "-o", "saida"};
    AppArgs *args = args_create();

    TEST_ASSERT_NOT_NULL(args);
    TEST_ASSERT_FALSE(args_parse(args, 7, argv));
    TEST_ASSERT_NOT_NULL(args_error(args));

    args_destroy(args);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_accepts_required_arguments);
    RUN_TEST(test_accepts_optional_arguments);
    RUN_TEST(test_rejects_missing_geo_file);
    RUN_TEST(test_rejects_missing_option_value);
    RUN_TEST(test_rejects_unknown_option);
    return UNITY_END();
}

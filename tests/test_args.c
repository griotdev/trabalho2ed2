#include "args.h"

#include <stdio.h>
#include <string.h>

static int failures = 0;

#define ASSERT_TRUE(value) assert_true((value), #value, __LINE__)
#define ASSERT_FALSE(value) assert_false((value), #value, __LINE__)
#define ASSERT_STR_EQ(expected, actual) assert_str_eq((expected), (actual), #actual, __LINE__)

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

static void test_accepts_required_arguments(void) {
    char *argv[] = {"ted", "-f", "cidade.geo", "-o", "saida"};
    AppArgs *args = args_create();

    ASSERT_TRUE(args_parse(args, 5, argv));
    ASSERT_STR_EQ("cidade.geo", args_geo_file(args));
    ASSERT_STR_EQ("saida", args_output_dir(args));

    args_destroy(args);
}

static void test_accepts_optional_arguments(void) {
    char *argv[] = {"ted", "-e", "entrada", "-f", "cidade.geo", "-q", "consulta.qry", "-v", "vias.via", "-o", "saida"};
    AppArgs *args = args_create();

    ASSERT_TRUE(args_parse(args, 11, argv));
    ASSERT_STR_EQ("entrada", args_input_dir(args));
    ASSERT_STR_EQ("consulta.qry", args_query_file(args));
    ASSERT_STR_EQ("vias.via", args_via_file(args));

    args_destroy(args);
}

static void test_rejects_missing_geo_file(void) {
    char *argv[] = {"ted", "-o", "saida"};
    AppArgs *args = args_create();

    ASSERT_FALSE(args_parse(args, 3, argv));
    ASSERT_TRUE(args_error(args) != NULL);

    args_destroy(args);
}

static void test_rejects_missing_option_value(void) {
    char *argv[] = {"ted", "-f", "cidade.geo", "-o"};
    AppArgs *args = args_create();

    ASSERT_FALSE(args_parse(args, 4, argv));
    ASSERT_TRUE(args_error(args) != NULL);

    args_destroy(args);
}

static void test_rejects_unknown_option(void) {
    char *argv[] = {"ted", "-x", "valor", "-f", "cidade.geo", "-o", "saida"};
    AppArgs *args = args_create();

    ASSERT_FALSE(args_parse(args, 7, argv));
    ASSERT_TRUE(args_error(args) != NULL);

    args_destroy(args);
}

int main(void) {
    test_accepts_required_arguments();
    test_accepts_optional_arguments();
    test_rejects_missing_geo_file();
    test_rejects_missing_option_value();
    test_rejects_unknown_option();

    if (failures == 0) {
        puts("test_args: todos os testes passaram");
    }

    return failures == 0 ? 0 : 1;
}

#include "file_paths.h"
#include "unity.h"

void setUp(void) {
}

void tearDown(void) {
}

static void test_builds_required_paths_without_input_dir(void) {
    FilePaths paths = file_paths_create(NULL, "cidade.geo", NULL, NULL, "saida");

    TEST_ASSERT_NOT_NULL(paths);
    TEST_ASSERT_NULL(file_paths_error(paths));
    TEST_ASSERT_EQUAL_STRING("cidade.geo", file_paths_geo_path(paths));
    TEST_ASSERT_NULL(file_paths_query_path(paths));
    TEST_ASSERT_NULL(file_paths_via_path(paths));
    TEST_ASSERT_EQUAL_STRING("saida/cidade.txt", file_paths_txt_path(paths));
    TEST_ASSERT_EQUAL_STRING("saida/cidade.svg", file_paths_svg_path(paths));

    file_paths_destroy(paths);
}

static void test_joins_input_dir_to_relative_input_files(void) {
    FilePaths paths = file_paths_create("entrada", "cidade.geo", "consulta.qry", "vias.via", "saida");

    TEST_ASSERT_NOT_NULL(paths);
    TEST_ASSERT_NULL(file_paths_error(paths));
    TEST_ASSERT_EQUAL_STRING("entrada/cidade.geo", file_paths_geo_path(paths));
    TEST_ASSERT_EQUAL_STRING("entrada/consulta.qry", file_paths_query_path(paths));
    TEST_ASSERT_EQUAL_STRING("entrada/vias.via", file_paths_via_path(paths));
    TEST_ASSERT_EQUAL_STRING("saida/cidade-consulta.txt", file_paths_txt_path(paths));
    TEST_ASSERT_EQUAL_STRING("saida/cidade-consulta.svg", file_paths_svg_path(paths));

    file_paths_destroy(paths);
}

static void test_preserves_absolute_input_files(void) {
    FilePaths paths = file_paths_create("entrada", "C:/dados/cidade.geo", "D:/dados/consulta.qry", NULL, "saida");

    TEST_ASSERT_NOT_NULL(paths);
    TEST_ASSERT_NULL(file_paths_error(paths));
    TEST_ASSERT_EQUAL_STRING("C:/dados/cidade.geo", file_paths_geo_path(paths));
    TEST_ASSERT_EQUAL_STRING("D:/dados/consulta.qry", file_paths_query_path(paths));

    file_paths_destroy(paths);
}

static void test_uses_geo_basename_for_outputs(void) {
    FilePaths paths = file_paths_create(NULL, "mapas/cidade.central.geo", NULL, NULL, "saida");

    TEST_ASSERT_NOT_NULL(paths);
    TEST_ASSERT_NULL(file_paths_error(paths));
    TEST_ASSERT_EQUAL_STRING("saida/cidade.central.txt", file_paths_txt_path(paths));
    TEST_ASSERT_EQUAL_STRING("saida/cidade.central.svg", file_paths_svg_path(paths));

    file_paths_destroy(paths);
}

static void test_combines_geo_and_query_basenames_for_outputs(void) {
    FilePaths paths = file_paths_create(NULL, "mapas/cidade.central.geo", "consultas/rota.01.qry", NULL, "saida");

    TEST_ASSERT_NOT_NULL(paths);
    TEST_ASSERT_NULL(file_paths_error(paths));
    TEST_ASSERT_EQUAL_STRING("saida/cidade.central-rota.01.txt", file_paths_txt_path(paths));
    TEST_ASSERT_EQUAL_STRING("saida/cidade.central-rota.01.svg", file_paths_svg_path(paths));

    file_paths_destroy(paths);
}

static void test_rejects_missing_required_paths(void) {
    FilePaths missing_geo = file_paths_create(NULL, NULL, NULL, NULL, "saida");
    FilePaths missing_output = file_paths_create(NULL, "cidade.geo", NULL, NULL, NULL);

    TEST_ASSERT_NOT_NULL(missing_geo);
    TEST_ASSERT_NOT_NULL(file_paths_error(missing_geo));
    TEST_ASSERT_NOT_NULL(missing_output);
    TEST_ASSERT_NOT_NULL(file_paths_error(missing_output));

    file_paths_destroy(missing_geo);
    file_paths_destroy(missing_output);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_builds_required_paths_without_input_dir);
    RUN_TEST(test_joins_input_dir_to_relative_input_files);
    RUN_TEST(test_preserves_absolute_input_files);
    RUN_TEST(test_uses_geo_basename_for_outputs);
    RUN_TEST(test_combines_geo_and_query_basenames_for_outputs);
    RUN_TEST(test_rejects_missing_required_paths);
    return UNITY_END();
}

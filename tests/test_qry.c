#include "geo.h"
#include "graph.h"
#include "qry.h"
#include "registers.h"
#include "unity.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *test_geo_path = "../out/test_qry.geo";
static const char *test_qry_path = "../out/test_qry.qry";
static const char *test_txt_path = "../out/test_qry.txt";

void setUp(void) {
}

void tearDown(void) {
    remove(test_geo_path);
    remove(test_qry_path);
    remove(test_txt_path);
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

static void test_processes_address_query(void) {
    Geo *geo;
    Registers *registers;
    char *content;

    write_file(test_geo_path, "q cep1 100 200 40 30\n");
    write_file(test_qry_path, "@o? R2 cep1 S 10\n");
    write_file(test_txt_path, "base\n");

    geo = geo_load(test_geo_path);
    registers = registers_create();
    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NULL(geo_error(geo));
    TEST_ASSERT_NOT_NULL(registers);

    TEST_ASSERT_TRUE(qry_process(test_qry_path, geo, NULL, registers, test_txt_path));
    TEST_ASSERT_NULL(qry_error());
    TEST_ASSERT_TRUE(registers_is_set(registers, 2));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 90.0, registers_x(registers, 2));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 200.0, registers_y(registers, 2));

    content = read_file(test_txt_path);
    TEST_ASSERT_NOT_NULL(strstr(content, "@o? R2 cep1 S 10.00 -> 90.00 200.00"));

    free(content);
    registers_destroy(registers);
    geo_destroy(geo);
}

static void test_reports_missing_cep(void) {
    Geo *geo;
    Registers *registers;

    write_file(test_geo_path, "q cep1 100 200 40 30\n");
    write_file(test_qry_path, "@o? R2 cepX S 10\n");
    write_file(test_txt_path, "base\n");

    geo = geo_load(test_geo_path);
    registers = registers_create();
    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NOT_NULL(registers);

    TEST_ASSERT_FALSE(qry_process(test_qry_path, geo, NULL, registers, test_txt_path));
    TEST_ASSERT_NOT_NULL(qry_error());
    TEST_ASSERT_FALSE(registers_is_set(registers, 2));

    registers_destroy(registers);
    geo_destroy(geo);
}

static void test_processes_mvm_query(void) {
    Geo *geo;
    Graph *graph;
    Registers *registers;
    char *content;
    int from;

    write_file(test_geo_path, "q cep1 100 200 40 30\n");
    write_file(test_qry_path, "mvm 12 0 0 30 30\n");
    write_file(test_txt_path, "base\n");

    geo = geo_load(test_geo_path);
    graph = graph_create();
    registers = registers_create();
    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NOT_NULL(graph);
    TEST_ASSERT_NOT_NULL(registers);

    graph_add_vertex(graph, "v1", 10.0, 10.0);
    graph_add_vertex(graph, "v2", 20.0, 20.0);
    graph_add_vertex(graph, "v3", 100.0, 100.0);
    graph_add_edge(graph, "v1", "v2", "cepR", "cepL", 25.5, 8.5, "Rua_A");
    graph_add_edge(graph, "v1", "v3", "cepR", "cepL", 90.0, 4.0, "Rua_B");

    TEST_ASSERT_TRUE(qry_process(test_qry_path, geo, graph, registers, test_txt_path));
    TEST_ASSERT_NULL(qry_error());

    from = graph_find_vertex(graph, "v1");
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 12.0, graph_edge_speed(graph, from, 0));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 4.0, graph_edge_speed(graph, from, 1));

    content = read_file(test_txt_path);
    TEST_ASSERT_NOT_NULL(strstr(content, "mvm 12.00 0.00 0.00 30.00 30.00 -> 1 arestas atualizadas"));

    free(content);
    registers_destroy(registers);
    graph_destroy(graph);
    geo_destroy(geo);
}

static void test_reports_mvm_without_graph(void) {
    Geo *geo;
    Registers *registers;

    write_file(test_geo_path, "q cep1 100 200 40 30\n");
    write_file(test_qry_path, "mvm 12 0 0 30 30\n");
    write_file(test_txt_path, "base\n");

    geo = geo_load(test_geo_path);
    registers = registers_create();
    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NOT_NULL(registers);

    TEST_ASSERT_FALSE(qry_process(test_qry_path, geo, NULL, registers, test_txt_path));
    TEST_ASSERT_NOT_NULL(qry_error());

    registers_destroy(registers);
    geo_destroy(geo);
}

static Graph *create_path_graph(void) {
    Graph *graph = graph_create();

    TEST_ASSERT_NOT_NULL(graph);
    graph_add_vertex(graph, "a", 0.0, 0.0);
    graph_add_vertex(graph, "b", 10.0, 0.0);
    graph_add_vertex(graph, "c", 20.0, 0.0);
    graph_add_vertex(graph, "d", 30.0, 0.0);
    graph_add_edge(graph, "a", "b", "cepR", "cepL", 10.0, 1.0, "Rua_AB");
    graph_add_edge(graph, "b", "d", "cepR", "cepL", 10.0, 1.0, "Rua_BD");
    graph_add_edge(graph, "a", "c", "cepR", "cepL", 30.0, 30.0, "Rua_AC");
    graph_add_edge(graph, "c", "d", "cepR", "cepL", 30.0, 30.0, "Rua_CD");

    return graph;
}

static void test_processes_path_query(void) {
    Geo *geo;
    Graph *graph;
    Registers *registers;
    char *content;

    write_file(test_geo_path, "q cep1 100 200 40 30\n");
    write_file(test_qry_path, "p? R1 R2 red blue\n");
    write_file(test_txt_path, "base\n");

    geo = geo_load(test_geo_path);
    graph = create_path_graph();
    registers = registers_create();
    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NOT_NULL(registers);
    TEST_ASSERT_TRUE(registers_set(registers, 1, 1.0, 0.0));
    TEST_ASSERT_TRUE(registers_set(registers, 2, 31.0, 0.0));

    TEST_ASSERT_TRUE(qry_process(test_qry_path, geo, graph, registers, test_txt_path));
    TEST_ASSERT_NULL(qry_error());

    content = read_file(test_txt_path);
    TEST_ASSERT_NOT_NULL(strstr(content, "p? R1 R2 red blue"));
    TEST_ASSERT_NOT_NULL(strstr(content, "menor caminho: a -> b -> d | peso 20.00"));
    TEST_ASSERT_NOT_NULL(strstr(content, "caminho mais rapido: a -> c -> d | peso 2.00"));

    free(content);
    registers_destroy(registers);
    graph_destroy(graph);
    geo_destroy(geo);
}
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_processes_address_query);
    RUN_TEST(test_reports_missing_cep);
    RUN_TEST(test_processes_mvm_query);
    RUN_TEST(test_reports_mvm_without_graph);
    RUN_TEST(test_processes_path_query);
    return UNITY_END();
}


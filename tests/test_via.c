#include "via.h"
#include "unity.h"

#include <stdio.h>

static const char *test_via_path = "../out/test_via_basic.via";

void setUp(void) {
}

void tearDown(void) {
    remove(test_via_path);
}

static void write_file(const char *content) {
    FILE *file = fopen(test_via_path, "w");

    TEST_ASSERT_NOT_NULL(file);
    TEST_ASSERT_NOT_EQUAL(EOF, fputs(content, file));
    TEST_ASSERT_EQUAL(0, fclose(file));
}

static void test_loads_vertices_and_edges(void) {
    Via *via;
    const Graph *graph;
    int from;
    int to;

    write_file("2\nv v1 10 20\nv v2 30 40\ne v1 v2 cepR cepL 25.5 8.5 Rua_A\n");
    via = via_load(test_via_path);

    TEST_ASSERT_NOT_NULL(via);
    TEST_ASSERT_NULL(via_error(via));
    graph = via_graph(via);
    TEST_ASSERT_NOT_NULL(graph);

    from = graph_find_vertex(graph, "v1");
    to = graph_find_vertex(graph, "v2");
    TEST_ASSERT_EQUAL_INT(2, graph_vertex_count(graph));
    TEST_ASSERT_EQUAL_INT(1, graph_edge_count(graph));
    TEST_ASSERT_EQUAL_INT(to, graph_edge_to(graph, from, 0));
    TEST_ASSERT_EQUAL_STRING("Rua_A", graph_edge_name(graph, from, 0));
    TEST_ASSERT_EQUAL_STRING("cepR", graph_edge_right_cep(graph, from, 0));
    TEST_ASSERT_EQUAL_STRING("cepL", graph_edge_left_cep(graph, from, 0));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 25.5, graph_edge_length(graph, from, 0));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 8.5, graph_edge_speed(graph, from, 0));

    via_destroy(via);
}

static void test_reports_missing_via_file(void) {
    Via *via = via_load("../out/arquivo_inexistente.via");

    TEST_ASSERT_NOT_NULL(via);
    TEST_ASSERT_NOT_NULL(via_error(via));
    TEST_ASSERT_NOT_NULL(via_graph(via));
    TEST_ASSERT_EQUAL_INT(0, graph_vertex_count(via_graph(via)));

    via_destroy(via);
}

static void test_reports_edge_with_missing_vertex(void) {
    Via *via;

    write_file("1\nv v1 10 20\ne v1 v2 cepR cepL 25.5 8.5 Rua_A\n");
    via = via_load(test_via_path);

    TEST_ASSERT_NOT_NULL(via);
    TEST_ASSERT_NOT_NULL(via_error(via));
    TEST_ASSERT_EQUAL_INT(1, graph_vertex_count(via_graph(via)));
    TEST_ASSERT_EQUAL_INT(0, graph_edge_count(via_graph(via)));

    via_destroy(via);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_loads_vertices_and_edges);
    RUN_TEST(test_reports_missing_via_file);
    RUN_TEST(test_reports_edge_with_missing_vertex);
    return UNITY_END();
}

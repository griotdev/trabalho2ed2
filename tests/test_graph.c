#include "graph.h"
#include "unity.h"

void setUp(void) {
}

void tearDown(void) {
}

static void test_adds_vertices(void) {
    Graph *graph = graph_create();
    int v1;
    int v2;

    TEST_ASSERT_NOT_NULL(graph);
    v1 = graph_add_vertex(graph, "v1", 10.0, 20.0);
    v2 = graph_add_vertex(graph, "v2", 30.0, 40.0);

    TEST_ASSERT_EQUAL_INT(0, v1);
    TEST_ASSERT_EQUAL_INT(1, v2);
    TEST_ASSERT_EQUAL_INT(2, graph_vertex_count(graph));
    TEST_ASSERT_EQUAL_STRING("v1", graph_vertex_id(graph, 0));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 30.0, graph_vertex_x(graph, 1));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 40.0, graph_vertex_y(graph, 1));

    graph_destroy(graph);
}

static void test_rejects_duplicate_vertices(void) {
    Graph *graph = graph_create();

    TEST_ASSERT_NOT_NULL(graph);
    TEST_ASSERT_EQUAL_INT(0, graph_add_vertex(graph, "v1", 10.0, 20.0));
    TEST_ASSERT_EQUAL_INT(-1, graph_add_vertex(graph, "v1", 30.0, 40.0));
    TEST_ASSERT_EQUAL_INT(1, graph_vertex_count(graph));

    graph_destroy(graph);
}

static void test_adds_directed_edges_as_adjacencies(void) {
    Graph *graph = graph_create();
    int from;
    int to;

    TEST_ASSERT_NOT_NULL(graph);
    graph_add_vertex(graph, "v1", 10.0, 20.0);
    graph_add_vertex(graph, "v2", 30.0, 40.0);

    TEST_ASSERT_TRUE(graph_add_edge(graph, "v1", "v2", "cepR", "cepL", 25.5, 8.5, "Rua_A"));
    from = graph_find_vertex(graph, "v1");
    to = graph_find_vertex(graph, "v2");

    TEST_ASSERT_EQUAL_INT(1, graph_edge_count(graph));
    TEST_ASSERT_EQUAL_INT(1, graph_out_degree(graph, from));
    TEST_ASSERT_EQUAL_INT(0, graph_out_degree(graph, to));
    TEST_ASSERT_EQUAL_INT(to, graph_edge_to(graph, from, 0));
    TEST_ASSERT_EQUAL_STRING("Rua_A", graph_edge_name(graph, from, 0));
    TEST_ASSERT_EQUAL_STRING("cepR", graph_edge_right_cep(graph, from, 0));
    TEST_ASSERT_EQUAL_STRING("cepL", graph_edge_left_cep(graph, from, 0));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 25.5, graph_edge_length(graph, from, 0));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 8.5, graph_edge_speed(graph, from, 0));

    graph_destroy(graph);
}

static void test_rejects_edges_with_missing_vertices(void) {
    Graph *graph = graph_create();

    TEST_ASSERT_NOT_NULL(graph);
    graph_add_vertex(graph, "v1", 10.0, 20.0);
    TEST_ASSERT_FALSE(graph_add_edge(graph, "v1", "v2", "cepR", "cepL", 25.5, 8.5, "Rua_A"));
    TEST_ASSERT_EQUAL_INT(0, graph_edge_count(graph));

    graph_destroy(graph);
}

static void test_updates_speeds_inside_rect(void) {
    Graph *graph = graph_create();
    int from;

    TEST_ASSERT_NOT_NULL(graph);
    graph_add_vertex(graph, "v1", 10.0, 10.0);
    graph_add_vertex(graph, "v2", 20.0, 20.0);
    graph_add_vertex(graph, "v3", 100.0, 100.0);
    graph_add_edge(graph, "v1", "v2", "cepR", "cepL", 25.5, 8.5, "Rua_A");
    graph_add_edge(graph, "v1", "v3", "cepR", "cepL", 90.0, 4.0, "Rua_B");

    from = graph_find_vertex(graph, "v1");
    TEST_ASSERT_EQUAL_INT(1, graph_update_speeds_in_rect(graph, 12.0, 0.0, 0.0, 30.0, 30.0));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 12.0, graph_edge_speed(graph, from, 0));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 4.0, graph_edge_speed(graph, from, 1));

    graph_destroy(graph);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_adds_vertices);
    RUN_TEST(test_rejects_duplicate_vertices);
    RUN_TEST(test_adds_directed_edges_as_adjacencies);
    RUN_TEST(test_rejects_edges_with_missing_vertices);
    RUN_TEST(test_updates_speeds_inside_rect);
    return UNITY_END();
}

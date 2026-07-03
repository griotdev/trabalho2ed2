#include "graph.h"
#include "road_expansion.h"
#include "unity.h"

void setUp(void) {
}

void tearDown(void) {
}

static Graph *create_expansion_graph(void) {
    Graph *graph = graph_create();

    TEST_ASSERT_NOT_NULL(graph);
    graph_add_vertex(graph, "a", 0.0, 0.0);
    graph_add_vertex(graph, "b", 10.0, 0.0);
    graph_add_vertex(graph, "c", 20.0, 0.0);
    graph_add_vertex(graph, "d", 30.0, 0.0);

    graph_add_edge(graph, "a", "b", "cepR", "cepL", 1.0, 10.0, "Rua_AB");
    graph_add_edge(graph, "b", "c", "cepR", "cepL", 1.0, 40.0, "Rua_BC");
    graph_add_edge(graph, "c", "d", "cepR", "cepL", 1.0, 20.0, "Rua_CD");
    graph_add_edge(graph, "a", "c", "cepR", "cepL", 5.0, 5.0, "Rua_AC");

    return graph;
}

static void test_expands_slow_edges_from_minimum_spanning_forest(void) {
    Graph *graph = create_expansion_graph();
    RoadExpansion *expansion = road_expansion_apply(graph, 30.0);
    int a = graph_find_vertex(graph, "a");
    int b = graph_find_vertex(graph, "b");
    int c = graph_find_vertex(graph, "c");

    TEST_ASSERT_NOT_NULL(expansion);
    TEST_ASSERT_EQUAL_INT(2, road_expansion_count(expansion));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 15.0, graph_edge_speed(graph, a, 0));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 40.0, graph_edge_speed(graph, b, 0));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 30.0, graph_edge_speed(graph, c, 0));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 5.0, graph_edge_speed(graph, a, 1));

    road_expansion_destroy(expansion);
    graph_destroy(graph);
}

static void test_rejects_invalid_inputs(void) {
    Graph *graph = graph_create();

    TEST_ASSERT_NOT_NULL(graph);
    TEST_ASSERT_NULL(road_expansion_apply(NULL, 30.0));
    TEST_ASSERT_NULL(road_expansion_apply(graph, -1.0));

    graph_destroy(graph);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_expands_slow_edges_from_minimum_spanning_forest);
    RUN_TEST(test_rejects_invalid_inputs);
    return UNITY_END();
}

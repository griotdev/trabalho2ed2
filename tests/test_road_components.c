#include "graph.h"
#include "road_components.h"
#include "unity.h"

void setUp(void) {
}

void tearDown(void) {
}

static Graph *create_components_graph(void) {
    Graph *graph = graph_create();

    TEST_ASSERT_NOT_NULL(graph);
    graph_add_vertex(graph, "a", 0.0, 0.0);
    graph_add_vertex(graph, "b", 10.0, 5.0);
    graph_add_vertex(graph, "c", 100.0, 100.0);
    graph_add_vertex(graph, "d", 120.0, 130.0);
    graph_add_vertex(graph, "e", 300.0, 300.0);

    graph_add_edge(graph, "a", "b", "cepR", "cepL", 10.0, 20.0, "Rua_AB");
    graph_add_edge(graph, "b", "c", "cepR", "cepL", 90.0, 80.0, "Rua_BC");
    graph_add_edge(graph, "c", "d", "cepR", "cepL", 10.0, 15.0, "Rua_CD");
    graph_add_edge(graph, "e", "a", "cepR", "cepL", 10.0, 100.0, "Rua_EA");

    return graph;
}

static void test_finds_slow_components_as_undirected(void) {
    Graph *graph = create_components_graph();
    RoadComponents *components = road_components_find_slow(graph, 30.0);

    TEST_ASSERT_NOT_NULL(components);
    TEST_ASSERT_EQUAL_INT(2, road_components_count(components));

    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 0.0, road_components_min_x(components, 0));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 0.0, road_components_min_y(components, 0));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 10.0, road_components_max_x(components, 0));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 5.0, road_components_max_y(components, 0));

    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 100.0, road_components_min_x(components, 1));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 100.0, road_components_min_y(components, 1));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 120.0, road_components_max_x(components, 1));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 130.0, road_components_max_y(components, 1));

    road_components_destroy(components);
    graph_destroy(graph);
}

static void test_ignores_fast_edges(void) {
    Graph *graph = create_components_graph();
    RoadComponents *components = road_components_find_slow(graph, 10.0);

    TEST_ASSERT_NOT_NULL(components);
    TEST_ASSERT_EQUAL_INT(0, road_components_count(components));

    road_components_destroy(components);
    graph_destroy(graph);
}

static void test_rejects_invalid_inputs(void) {
    Graph *graph = graph_create();

    TEST_ASSERT_NOT_NULL(graph);
    TEST_ASSERT_NULL(road_components_find_slow(NULL, 30.0));
    TEST_ASSERT_NULL(road_components_find_slow(graph, -1.0));

    graph_destroy(graph);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_finds_slow_components_as_undirected);
    RUN_TEST(test_ignores_fast_edges);
    RUN_TEST(test_rejects_invalid_inputs);
    return UNITY_END();
}

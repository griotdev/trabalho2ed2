#include "graph.h"
#include "route.h"
#include "unity.h"

void setUp(void) {
}

void tearDown(void) {
}

static Graph *create_route_graph(void) {
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

static void test_finds_shortest_by_length(void) {
    Graph *graph = create_route_graph();
    Route *route = route_shortest_path(graph, graph_find_vertex(graph, "a"), graph_find_vertex(graph, "d"), ROUTE_METRIC_LENGTH);

    TEST_ASSERT_NOT_NULL(route);
    TEST_ASSERT_TRUE(route_found(route));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 20.0, route_total_weight(route));
    TEST_ASSERT_EQUAL_INT(3, route_vertex_count(route));
    TEST_ASSERT_EQUAL_INT(graph_find_vertex(graph, "a"), route_vertex_at(route, 0));
    TEST_ASSERT_EQUAL_INT(graph_find_vertex(graph, "b"), route_vertex_at(route, 1));
    TEST_ASSERT_EQUAL_INT(graph_find_vertex(graph, "d"), route_vertex_at(route, 2));
    TEST_ASSERT_EQUAL_INT(2, route_edge_count(route));
    TEST_ASSERT_EQUAL_STRING("Rua_AB", graph_edge_name(graph, route_edge_from(route, 0), route_edge_index(route, 0)));
    TEST_ASSERT_EQUAL_STRING("Rua_BD", graph_edge_name(graph, route_edge_from(route, 1), route_edge_index(route, 1)));

    route_destroy(route);
    graph_destroy(graph);
}

static void test_finds_fastest_by_time(void) {
    Graph *graph = create_route_graph();
    Route *route = route_shortest_path(graph, graph_find_vertex(graph, "a"), graph_find_vertex(graph, "d"), ROUTE_METRIC_TIME);

    TEST_ASSERT_NOT_NULL(route);
    TEST_ASSERT_TRUE(route_found(route));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 2.0, route_total_weight(route));
    TEST_ASSERT_EQUAL_INT(3, route_vertex_count(route));
    TEST_ASSERT_EQUAL_INT(graph_find_vertex(graph, "a"), route_vertex_at(route, 0));
    TEST_ASSERT_EQUAL_INT(graph_find_vertex(graph, "c"), route_vertex_at(route, 1));
    TEST_ASSERT_EQUAL_INT(graph_find_vertex(graph, "d"), route_vertex_at(route, 2));
    TEST_ASSERT_EQUAL_STRING("Rua_AC", graph_edge_name(graph, route_edge_from(route, 0), route_edge_index(route, 0)));
    TEST_ASSERT_EQUAL_STRING("Rua_CD", graph_edge_name(graph, route_edge_from(route, 1), route_edge_index(route, 1)));

    route_destroy(route);
    graph_destroy(graph);
}

static void test_reports_unreachable_destination(void) {
    Graph *graph = graph_create();
    Route *route;

    TEST_ASSERT_NOT_NULL(graph);
    graph_add_vertex(graph, "a", 0.0, 0.0);
    graph_add_vertex(graph, "b", 10.0, 0.0);
    route = route_shortest_path(graph, graph_find_vertex(graph, "a"), graph_find_vertex(graph, "b"), ROUTE_METRIC_LENGTH);

    TEST_ASSERT_NOT_NULL(route);
    TEST_ASSERT_FALSE(route_found(route));
    TEST_ASSERT_EQUAL_INT(0, route_vertex_count(route));
    TEST_ASSERT_EQUAL_INT(0, route_edge_count(route));

    route_destroy(route);
    graph_destroy(graph);
}

static void test_origin_can_equal_destination(void) {
    Graph *graph = graph_create();
    Route *route;

    TEST_ASSERT_NOT_NULL(graph);
    graph_add_vertex(graph, "a", 0.0, 0.0);
    route = route_shortest_path(graph, graph_find_vertex(graph, "a"), graph_find_vertex(graph, "a"), ROUTE_METRIC_LENGTH);

    TEST_ASSERT_NOT_NULL(route);
    TEST_ASSERT_TRUE(route_found(route));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 0.0, route_total_weight(route));
    TEST_ASSERT_EQUAL_INT(1, route_vertex_count(route));
    TEST_ASSERT_EQUAL_INT(0, route_edge_count(route));
    TEST_ASSERT_EQUAL_INT(graph_find_vertex(graph, "a"), route_vertex_at(route, 0));

    route_destroy(route);
    graph_destroy(graph);
}

static void test_rejects_invalid_inputs(void) {
    Graph *graph = create_route_graph();

    TEST_ASSERT_NULL(route_shortest_path(NULL, 0, 1, ROUTE_METRIC_LENGTH));
    TEST_ASSERT_NULL(route_shortest_path(graph, -1, 1, ROUTE_METRIC_LENGTH));
    TEST_ASSERT_NULL(route_shortest_path(graph, 0, -1, ROUTE_METRIC_LENGTH));
    TEST_ASSERT_NULL(route_shortest_path(graph, 0, 1, 99));

    graph_destroy(graph);
}
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_finds_shortest_by_length);
    RUN_TEST(test_finds_fastest_by_time);
    RUN_TEST(test_reports_unreachable_destination);
    RUN_TEST(test_origin_can_equal_destination);
    RUN_TEST(test_rejects_invalid_inputs);
    return UNITY_END();
}

#include "geo.h"
#include "graph.h"
#include "output.h"
#include "registers.h"
#include "road_routes.h"
#include "route.h"
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

static Geo load_sample_geo(void) {
    write_file(test_geo_path, "cq 2 red black\nq cep1 10 20 30 40\n");
    return geo_load(test_geo_path);
}

static void test_creates_empty_txt_report(void) {
    Geo geo = load_sample_geo();
    char *content;

    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NULL(geo_error(geo));
    TEST_ASSERT_TRUE(output_write_txt(test_txt_path, geo));
    TEST_ASSERT_NULL(output_error());

    content = read_file(test_txt_path);
    TEST_ASSERT_EQUAL_STRING("", content);

    free(content);
    geo_destroy(geo);
}

static void test_writes_svg_rectangles_with_style(void) {
    Geo geo = load_sample_geo();
    char *content;

    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NULL(geo_error(geo));
    TEST_ASSERT_TRUE(output_write_svg(test_svg_path, geo));
    TEST_ASSERT_NULL(output_error());

    content = read_file(test_svg_path);
    TEST_ASSERT_NOT_NULL(strstr(content, "<svg"));
    TEST_ASSERT_NOT_NULL(strstr(content, "viewBox=\"-40.00 0.00 70.00 80.00\""));
    TEST_ASSERT_NOT_NULL(strstr(content, "<rect"));
    TEST_ASSERT_NOT_NULL(strstr(content, "fill=\"red\""));
    TEST_ASSERT_NOT_NULL(strstr(content, "stroke=\"black\""));
    TEST_ASSERT_NOT_NULL(strstr(content, "stroke-width=\"2.00\""));

    free(content);
    geo_destroy(geo);
}

static void test_writes_svg_register_markers(void) {
    Geo geo = load_sample_geo();
    Registers registers = registers_create();
    char *content;

    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NOT_NULL(registers);
    TEST_ASSERT_TRUE(registers_set(registers, 2, 90.0, 200.0));
    TEST_ASSERT_TRUE(output_write_svg_with_registers(test_svg_path, geo, registers));
    TEST_ASSERT_NULL(output_error());

    content = read_file(test_svg_path);
    TEST_ASSERT_NOT_NULL(strstr(content, "viewBox=\"-40.00 -20.00 153.00 240.00\""));
    TEST_ASSERT_NOT_NULL(strstr(content, "<line x1=\"90.00\""));
    TEST_ASSERT_NOT_NULL(strstr(content, "stroke=\"red\""));
    TEST_ASSERT_NOT_NULL(strstr(content, "stroke-dasharray=\"4 4\""));
    TEST_ASSERT_NOT_NULL(strstr(content, ">R2</text>"));

    free(content);
    registers_destroy(registers);
    geo_destroy(geo);
}

static void test_writes_svg_graph_edges(void) {
    Geo geo = load_sample_geo();
    Graph graph = graph_create();
    char *content;

    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NOT_NULL(graph);
    graph_add_vertex(graph, "v1", 10.0, 20.0);
    graph_add_vertex(graph, "v2", 30.0, 40.0);
    graph_add_edge(graph, "v1", "v2", "cepR", "cepL", 25.5, 8.5, "Rua_A");
    graph_add_edge(graph, "v2", "v1", "cepR", "cepL", 25.5, 8.5, "Rua_A_volta");

    TEST_ASSERT_TRUE(output_write_svg_with_graph(test_svg_path, geo, graph, NULL, NULL, NULL, NULL));
    TEST_ASSERT_NULL(output_error());

    content = read_file(test_svg_path);
    TEST_ASSERT_NOT_NULL(strstr(content, "viewBox=\"-40.00 0.00 90.00 80.00\""));
    TEST_ASSERT_NOT_NULL(strstr(content, "x1=\"10.00\" y1=\"20.00\" x2=\"30.00\" y2=\"40.00\""));
    TEST_ASSERT_NOT_NULL(strstr(content, "stroke=\"#777\""));
    TEST_ASSERT_NOT_NULL(strstr(content, ">Rua_A</title>"));
    TEST_ASSERT_NOT_NULL(strstr(content, "<circle cx=\"10.00\" cy=\"20.00\""));

    free(content);
    graph_destroy(graph);
    geo_destroy(geo);
}

static void test_writes_svg_road_component_boxes(void) {
    Geo geo = load_sample_geo();
    Graph graph = graph_create();
    RoadComponents components;
    char *content;

    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NOT_NULL(graph);
    graph_add_vertex(graph, "v1", 10.0, 20.0);
    graph_add_vertex(graph, "v2", 30.0, 40.0);
    graph_add_edge(graph, "v1", "v2", "cepR", "cepL", 25.5, 8.5, "Rua_A");
    graph_add_edge(graph, "v2", "v1", "cepR", "cepL", 25.5, 8.5, "Rua_A_volta");
    components = road_components_find_slow(graph, 10.0);
    TEST_ASSERT_NOT_NULL(components);

    TEST_ASSERT_TRUE(output_write_svg_with_graph(test_svg_path, geo, graph, NULL, components, NULL, NULL));
    TEST_ASSERT_NULL(output_error());

    content = read_file(test_svg_path);
    TEST_ASSERT_NOT_NULL(strstr(content, "x=\"10.00\" y=\"20.00\" width=\"20.00\" height=\"20.00\""));
    TEST_ASSERT_NOT_NULL(strstr(content, "fill-opacity=\"0.50\""));
    TEST_ASSERT_NOT_NULL(strstr(content, "stroke-width=\"2.00\""));

    free(content);
    road_components_destroy(components);
    graph_destroy(graph);
    geo_destroy(geo);
}
static void test_writes_svg_road_expansion(void) {
    Geo geo = load_sample_geo();
    Graph graph = graph_create();
    RoadExpansion expansion;
    char *content;

    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NOT_NULL(graph);
    graph_add_vertex(graph, "v1", 10.0, 20.0);
    graph_add_vertex(graph, "v2", 30.0, 40.0);
    graph_add_edge(graph, "v1", "v2", "cepR", "cepL", 25.5, 8.5, "Rua_A");
    expansion = road_expansion_apply(graph, 10.0);
    TEST_ASSERT_NOT_NULL(expansion);

    TEST_ASSERT_TRUE(output_write_svg_with_graph(test_svg_path, geo, graph, NULL, NULL, expansion, NULL));
    TEST_ASSERT_NULL(output_error());

    content = read_file(test_svg_path);
    TEST_ASSERT_NOT_NULL(strstr(content, "stroke=\"red\" stroke-width=\"4.00\""));
    TEST_ASSERT_NOT_NULL(strstr(content, ">exp Rua_A</title>"));

    free(content);
    road_expansion_destroy(expansion);
    graph_destroy(graph);
    geo_destroy(geo);
}
static void test_writes_svg_routes(void) {
    Geo geo = load_sample_geo();
    Graph graph = graph_create();
    Route route;
    RoadRoutes routes = road_routes_create();
    char *content;

    TEST_ASSERT_NOT_NULL(geo);
    TEST_ASSERT_NOT_NULL(graph);
    TEST_ASSERT_NOT_NULL(routes);
    graph_add_vertex(graph, "v1", 10.0, 20.0);
    graph_add_vertex(graph, "v2", 30.0, 40.0);
    graph_add_edge(graph, "v1", "v2", "cepR", "cepL", 25.5, 8.5, "Rua_A");
    route = route_shortest_path(graph, graph_find_vertex(graph, "v1"), graph_find_vertex(graph, "v2"), ROUTE_METRIC_LENGTH);
    TEST_ASSERT_NOT_NULL(route);
    TEST_ASSERT_TRUE(road_routes_add(routes, route, "purple", "menor caminho"));

    TEST_ASSERT_TRUE(output_write_svg_with_graph(test_svg_path, geo, graph, NULL, NULL, NULL, routes));
    TEST_ASSERT_NULL(output_error());

    content = read_file(test_svg_path);
    TEST_ASSERT_NOT_NULL(strstr(content, "stroke=\"purple\" stroke-width=\"3.00\""));
    TEST_ASSERT_NOT_NULL(strstr(content, ">menor caminho Rua_A</title>"));
    TEST_ASSERT_NOT_NULL(strstr(content, "<path id=\"rota-0\""));
    TEST_ASSERT_NOT_NULL(strstr(content, "<animateMotion dur=\"6s\""));
    TEST_ASSERT_NOT_NULL(strstr(content, "<mpath href=\"#rota-0\""));
    TEST_ASSERT_NOT_NULL(strstr(content, ">I</text>"));
    TEST_ASSERT_NOT_NULL(strstr(content, ">F</text>"));

    free(content);
    route_destroy(route);
    road_routes_destroy(routes);
    graph_destroy(graph);
    geo_destroy(geo);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_creates_empty_txt_report);
    RUN_TEST(test_writes_svg_rectangles_with_style);
    RUN_TEST(test_writes_svg_register_markers);
    RUN_TEST(test_writes_svg_graph_edges);
    RUN_TEST(test_writes_svg_road_component_boxes);
    RUN_TEST(test_writes_svg_road_expansion);
    RUN_TEST(test_writes_svg_routes);
    return UNITY_END();
}

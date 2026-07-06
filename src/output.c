#include "output.h"

#include <stdio.h>

static char last_error[160];

static void clear_error(void) {
    last_error[0] = '\0';
}

static void set_error(const char *message) {
    snprintf(last_error, sizeof(last_error), "%s", message);
}

static int open_output(FILE **file, const char *path) {
    clear_error();

    if (path == NULL || path[0] == '\0') {
        set_error("Caminho de saida nao informado");
        return 0;
    }

    *file = fopen(path, "w");
    if (*file == NULL) {
        set_error("Nao foi possivel criar arquivo de saida");
        return 0;
    }

    return 1;
}

static void write_svg_blocks(FILE *file, const Geo *geo) {
    int i;

    for (i = 0; i < geo_block_count(geo); i++) {
        const Block *block = geo_block_at(geo, i);
        fprintf(file,
                "  <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" fill=\"%s\" stroke=\"%s\" stroke-width=\"%.2f\" />\n",
                block_x(block) - block_width(block),
                block_y(block),
                block_width(block),
                block_height(block),
                block_fill_color(block),
                block_stroke_color(block),
                block_stroke_width(block));
    }
}

static void write_svg_graph(FILE *file, const Graph *graph) {
    int i;

    if (graph == NULL) {
        return;
    }

    for (i = 0; i < graph_vertex_count(graph); i++) {
        int edge_index;

        for (edge_index = 0; edge_index < graph_out_degree(graph, i); edge_index++) {
            int to = graph_edge_to(graph, i, edge_index);

            if (to >= 0) {
                fprintf(file,
                        "  <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" stroke=\"#777\" stroke-width=\"1.50\">\n",
                        graph_vertex_x(graph, i),
                        graph_vertex_y(graph, i),
                        graph_vertex_x(graph, to),
                        graph_vertex_y(graph, to));
                fprintf(file, "    <title>%s</title>\n", graph_edge_name(graph, i, edge_index));
                fputs("  </line>\n", file);
            }
        }
    }

    for (i = 0; i < graph_vertex_count(graph); i++) {
        fprintf(file,
                "  <circle cx=\"%.2f\" cy=\"%.2f\" r=\"2.00\" fill=\"#222\" />\n",
                graph_vertex_x(graph, i),
                graph_vertex_y(graph, i));
    }
}

static const char *component_color(int index) {
    static const char *colors[] = {"#e41a1c", "#377eb8", "#4daf4a", "#984ea3", "#ff7f00", "#a65628"};
    int count = (int)(sizeof(colors) / sizeof(colors[0]));

    return colors[index % count];
}

static void write_svg_road_expansion(FILE *file, const Graph *graph, const RoadExpansion *expansion) {
    int i;

    if (graph == NULL || expansion == NULL) {
        return;
    }

    for (i = 0; i < road_expansion_count(expansion); i++) {
        int from = road_expansion_edge_from(expansion, i);
        int to = road_expansion_edge_to(expansion, i);
        int edge_index = road_expansion_edge_index(expansion, i);

        if (from >= 0 && to >= 0) {
            fprintf(file,
                    "  <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" stroke=\"red\" stroke-width=\"4.00\">\n",
                    graph_vertex_x(graph, from),
                    graph_vertex_y(graph, from),
                    graph_vertex_x(graph, to),
                    graph_vertex_y(graph, to));
            fprintf(file, "    <title>exp %s</title>\n", graph_edge_name(graph, from, edge_index));
            fputs("  </line>\n", file);
        }
    }
}

static void write_svg_road_routes(FILE *file, const Graph *graph, const RoadRoutes *routes) {
    int i;

    if (graph == NULL || routes == NULL) {
        return;
    }

    for (i = 0; i < road_routes_count(routes); i++) {
        int step;
        const char *color = road_routes_color(routes, i);
        const char *label = road_routes_label(routes, i);

        for (step = 0; step < road_routes_step_count(routes, i); step++) {
            int from = road_routes_step_from(routes, i, step);
            int to = road_routes_step_to(routes, i, step);
            int edge_index = road_routes_step_edge_index(routes, i, step);

            if (from >= 0 && to >= 0) {
                fprintf(file,
                        "  <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" stroke=\"%s\" stroke-width=\"3.00\" stroke-opacity=\"0.85\" stroke-linecap=\"round\">\n",
                        graph_vertex_x(graph, from),
                        graph_vertex_y(graph, from),
                        graph_vertex_x(graph, to),
                        graph_vertex_y(graph, to),
                        color == NULL ? "black" : color);
                fprintf(file, "    <title>%s %s</title>\n", label == NULL ? "rota" : label, graph_edge_name(graph, from, edge_index));
                fputs("  </line>\n", file);
            }
        }
    }
}
static void write_svg_road_components(FILE *file, const RoadComponents *components) {
    int i;

    if (components == NULL) {
        return;
    }

    for (i = 0; i < road_components_count(components); i++) {
        double min_x = road_components_min_x(components, i);
        double min_y = road_components_min_y(components, i);
        double max_x = road_components_max_x(components, i);
        double max_y = road_components_max_y(components, i);
        double width = max_x - min_x;
        double height = max_y - min_y;

        fprintf(file,
                "  <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" fill=\"%s\" fill-opacity=\"0.50\" stroke=\"%s\" stroke-width=\"2.00\" />\n",
                min_x,
                min_y,
                width,
                height,
                component_color(i),
                component_color(i));
    }
}

static void write_svg_registers(FILE *file, const Registers *registers) {
    int i;

    if (registers == NULL) {
        return;
    }

    for (i = 0; i <= 10; i++) {
        if (registers_is_set(registers, i)) {
            double x = registers_x(registers, i);
            double y = registers_y(registers, i);

            fprintf(file,
                    "  <line x1=\"%.2f\" y1=\"0.00\" x2=\"%.2f\" y2=\"%.2f\" stroke=\"red\" stroke-dasharray=\"4 4\" />\n",
                    x,
                    x,
                    y);
            fprintf(file,
                    "  <text x=\"%.2f\" y=\"%.2f\" fill=\"red\">R%d</text>\n",
                    x + 3.0,
                    y,
                    i);
        }
    }
}

int output_write_txt(const char *path, const Geo *geo) {
    FILE *file;
    int i;

    if (!open_output(&file, path)) {
        return 0;
    }

    fprintf(file, "Quadras: %d\n", geo_block_count(geo));
    for (i = 0; i < geo_block_count(geo); i++) {
        const Block *block = geo_block_at(geo, i);
        fprintf(file,
                "q %s %.2f %.2f %.2f %.2f\n",
                block_cep(block),
                block_x(block),
                block_y(block),
                block_width(block),
                block_height(block));
    }

    if (fclose(file) != 0) {
        set_error("Erro ao fechar arquivo TXT");
        return 0;
    }

    return 1;
}

int output_write_svg(const char *path, const Geo *geo) {
    return output_write_svg_with_graph(path, geo, NULL, NULL, NULL, NULL, NULL);
}

int output_write_svg_with_registers(const char *path, const Geo *geo, const Registers *registers) {
    return output_write_svg_with_graph(path, geo, NULL, registers, NULL, NULL, NULL);
}

int output_write_svg_with_graph(const char *path,
                                const Geo *geo,
                                const Graph *graph,
                                const Registers *registers,
                                const RoadComponents *road_components,
                                const RoadExpansion *road_expansion,
                                const RoadRoutes *road_routes) {
    FILE *file;

    if (!open_output(&file, path)) {
        return 0;
    }

    fputs("<svg xmlns=\"http://www.w3.org/2000/svg\">\n", file);
    write_svg_blocks(file, geo);
    write_svg_graph(file, graph);
    write_svg_road_components(file, road_components);
    write_svg_road_expansion(file, graph, road_expansion);
    write_svg_road_routes(file, graph, road_routes);
    write_svg_registers(file, registers);
    fputs("</svg>\n", file);

    if (fclose(file) != 0) {
        set_error("Erro ao fechar arquivo SVG");
        return 0;
    }

    return 1;
}

const char *output_error(void) {
    return last_error[0] == '\0' ? NULL : last_error;
}

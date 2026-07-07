#include "output.h"

#include <float.h>
#include <stdio.h>

static char last_error[160];

typedef struct {
    int has_points;
    double min_x;
    double min_y;
    double max_x;
    double max_y;
} SvgBounds;

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

static void bounds_add_point(SvgBounds *bounds, double x, double y) {
    if (!bounds->has_points) {
        bounds->min_x = x;
        bounds->min_y = y;
        bounds->max_x = x;
        bounds->max_y = y;
        bounds->has_points = 1;
        return;
    }

    if (x < bounds->min_x) {
        bounds->min_x = x;
    }
    if (y < bounds->min_y) {
        bounds->min_y = y;
    }
    if (x > bounds->max_x) {
        bounds->max_x = x;
    }
    if (y > bounds->max_y) {
        bounds->max_y = y;
    }
}

static void bounds_add_rect(SvgBounds *bounds, double x, double y, double width, double height) {
    bounds_add_point(bounds, x, y);
    bounds_add_point(bounds, x + width, y + height);
}

static void collect_svg_bounds(SvgBounds *bounds,
                               const Geo *geo,
                               const Graph *graph,
                               const Registers *registers,
                               const RoadComponents *road_components) {
    int i;

    bounds->has_points = 0;
    bounds->min_x = DBL_MAX;
    bounds->min_y = DBL_MAX;
    bounds->max_x = -DBL_MAX;
    bounds->max_y = -DBL_MAX;

    for (i = 0; i < geo_block_count(geo); i++) {
        const Block *block = geo_block_at(geo, i);
        bounds_add_rect(bounds,
                        block_x(block) - block_width(block),
                        block_y(block),
                        block_width(block),
                        block_height(block));
    }

    if (graph != NULL) {
        for (i = 0; i < graph_vertex_count(graph); i++) {
            bounds_add_point(bounds, graph_vertex_x(graph, i), graph_vertex_y(graph, i));
        }
    }

    if (registers != NULL) {
        for (i = 0; i <= 10; i++) {
            if (registers_is_set(registers, i)) {
                double x = registers_x(registers, i);
                double y = registers_y(registers, i);

                bounds_add_point(bounds, x, 0.0);
                bounds_add_point(bounds, x + 3.0, y);
            }
        }
    }

    if (road_components != NULL) {
        for (i = 0; i < road_components_count(road_components); i++) {
            bounds_add_rect(bounds,
                            road_components_min_x(road_components, i),
                            road_components_min_y(road_components, i),
                            road_components_max_x(road_components, i) - road_components_min_x(road_components, i),
                            road_components_max_y(road_components, i) - road_components_min_y(road_components, i));
        }
    }
}

static void write_svg_header(FILE *file,
                             const Geo *geo,
                             const Graph *graph,
                             const Registers *registers,
                             const RoadComponents *road_components) {
    SvgBounds bounds;
    double padding = 20.0;
    double x;
    double y;
    double width;
    double height;

    collect_svg_bounds(&bounds, geo, graph, registers, road_components);
    if (!bounds.has_points) {
        fputs("<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0.00 0.00 100.00 100.00\">\n", file);
        return;
    }

    x = bounds.min_x - padding;
    y = bounds.min_y - padding;
    width = bounds.max_x - bounds.min_x + 2.0 * padding;
    height = bounds.max_y - bounds.min_y + 2.0 * padding;
    if (width <= 0.0) {
        width = 2.0 * padding;
    }
    if (height <= 0.0) {
        height = 2.0 * padding;
    }

    fprintf(file,
            "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"%.2f %.2f %.2f %.2f\">\n",
            x,
            y,
            width,
            height);
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
    int markers_written = 0;

    if (graph == NULL || routes == NULL) {
        return;
    }

    for (i = 0; i < road_routes_count(routes); i++) {
        int step;
        int step_count = road_routes_step_count(routes, i);
        int first_from;
        int last_to;
        const char *color = road_routes_color(routes, i);
        const char *label = road_routes_label(routes, i);

        if (step_count <= 0) {
            continue;
        }

        first_from = road_routes_step_from(routes, i, 0);
        last_to = road_routes_step_to(routes, i, step_count - 1);
        if (first_from < 0 || last_to < 0) {
            continue;
        }

        for (step = 0; step < step_count; step++) {
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

        fprintf(file,
                "  <path id=\"rota-%d\" d=\"M %.2f %.2f",
                i,
                graph_vertex_x(graph, first_from),
                graph_vertex_y(graph, first_from));
        for (step = 0; step < step_count; step++) {
            int to = road_routes_step_to(routes, i, step);
            if (to >= 0) {
                fprintf(file, " L %.2f %.2f", graph_vertex_x(graph, to), graph_vertex_y(graph, to));
            }
        }
        fputs("\" fill=\"none\" stroke=\"none\" />\n", file);
        fprintf(file, "  <circle r=\"3.00\" fill=\"%s\">\n", color == NULL ? "black" : color);
        fprintf(file, "    <animateMotion dur=\"6s\" repeatCount=\"indefinite\">\n");
        fprintf(file, "      <mpath href=\"#rota-%d\" />\n", i);
        fprintf(file, "    </animateMotion>\n");
        fputs("  </circle>\n", file);

        if (!markers_written) {
            fprintf(file,
                    "  <text x=\"%.2f\" y=\"%.2f\" fill=\"black\" font-weight=\"bold\">I</text>\n",
                    graph_vertex_x(graph, first_from) + 4.0,
                    graph_vertex_y(graph, first_from) - 4.0);
            fprintf(file,
                    "  <text x=\"%.2f\" y=\"%.2f\" fill=\"black\" font-weight=\"bold\">F</text>\n",
                    graph_vertex_x(graph, last_to) + 4.0,
                    graph_vertex_y(graph, last_to) - 4.0);
            markers_written = 1;
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

    write_svg_header(file, geo, graph, registers, road_components);
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

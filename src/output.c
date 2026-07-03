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
    return output_write_svg_with_graph(path, geo, NULL, NULL);
}

int output_write_svg_with_registers(const char *path, const Geo *geo, const Registers *registers) {
    return output_write_svg_with_graph(path, geo, NULL, registers);
}

int output_write_svg_with_graph(const char *path, const Geo *geo, const Graph *graph, const Registers *registers) {
    FILE *file;

    if (!open_output(&file, path)) {
        return 0;
    }

    fputs("<svg xmlns=\"http://www.w3.org/2000/svg\">\n", file);
    write_svg_blocks(file, geo);
    write_svg_graph(file, graph);
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

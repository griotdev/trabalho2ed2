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
    FILE *file;
    int i;

    if (!open_output(&file, path)) {
        return 0;
    }

    fputs("<svg xmlns=\"http://www.w3.org/2000/svg\">\n", file);
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

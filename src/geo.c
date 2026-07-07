#include "geo.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    Block **blocks;
    int count;
    int capacity;
    char error[200];
} GeoData;

typedef struct {
    double stroke_width;
    char fill_color[64];
    char stroke_color[64];
} GeoStyle;

static void set_error(GeoData *geo, const char *message) {
    snprintf(geo->error, sizeof(geo->error), "%s", message);
}

static int ensure_capacity(GeoData *geo) {
    Block **new_blocks;
    int new_capacity;

    if (geo->count < geo->capacity) {
        return 1;
    }

    new_capacity = geo->capacity == 0 ? 8 : geo->capacity * 2;
    new_blocks = realloc(geo->blocks, (size_t)new_capacity * sizeof(Block *));
    if (new_blocks == NULL) {
        set_error(geo, "Memoria insuficiente ao armazenar quadras");
        return 0;
    }

    geo->blocks = new_blocks;
    geo->capacity = new_capacity;
    return 1;
}

static int add_block(GeoData *geo, Block *block) {
    if (!ensure_capacity(geo)) {
        return 0;
    }

    geo->blocks[geo->count] = block;
    geo->count++;
    return 1;
}

static int parse_style(GeoData *geo, GeoStyle *style, const char *line) {
    char stroke_width_text[64];
    char fill_color[64];
    char stroke_color[64];
    char *end;
    double stroke_width;

    if (sscanf(line, "cq %63s %63s %63s", stroke_width_text, fill_color, stroke_color) != 3) {
        set_error(geo, "Comando cq malformado");
        return 0;
    }

    errno = 0;
    stroke_width = strtod(stroke_width_text, &end);
    if (errno != 0 || end == stroke_width_text || (*end != '\0' && strcmp(end, "px") != 0) || stroke_width < 0.0) {
        set_error(geo, "Espessura de cq invalida");
        return 0;
    }

    style->stroke_width = stroke_width;
    snprintf(style->fill_color, sizeof(style->fill_color), "%s", fill_color);
    snprintf(style->stroke_color, sizeof(style->stroke_color), "%s", stroke_color);
    return 1;
}

static int parse_block(GeoData *geo, const GeoStyle *style, const char *line) {
    Block *block;
    char cep[64];
    double x;
    double y;
    double width;
    double height;

    if (sscanf(line, "q %63s %lf %lf %lf %lf", cep, &x, &y, &width, &height) != 5) {
        set_error(geo, "Comando q malformado");
        return 0;
    }

    block = block_create(cep, x, y, width, height);
    if (block == NULL) {
        set_error(geo, "Nao foi possivel criar quadra");
        return 0;
    }

    block_set_style(block, style->stroke_width, style->fill_color, style->stroke_color);
    if (!add_block(geo, block)) {
        block_destroy(block);
        return 0;
    }

    return 1;
}

static int parse_line(GeoData *geo, GeoStyle *style, const char *line) {
    char command[16];

    if (sscanf(line, "%15s", command) != 1) {
        return 1;
    }

    if (strcmp(command, "cq") == 0) {
        return parse_style(geo, style, line);
    }

    if (strcmp(command, "q") == 0) {
        return parse_block(geo, style, line);
    }

    return 1;
}

static void read_geo_file(GeoData *geo, const char *path) {
    FILE *file;
    char line[512];
    GeoStyle style = {1.0, "none", "black"};

    if (path == NULL) {
        set_error(geo, "Caminho GEO nao informado");
        return;
    }

    file = fopen(path, "r");
    if (file == NULL) {
        set_error(geo, "Nao foi possivel abrir arquivo GEO");
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        if (!parse_line(geo, &style, line)) {
            break;
        }
    }

    fclose(file);
}

Geo *geo_load(const char *path) {
    GeoData *geo = calloc(1, sizeof(GeoData));

    if (geo == NULL) {
        return NULL;
    }

    read_geo_file(geo, path);
    return geo;
}

void geo_destroy(Geo *geo) {
    GeoData *data = geo;
    int i;

    if (data == NULL) {
        return;
    }

    for (i = 0; i < data->count; i++) {
        block_destroy(data->blocks[i]);
    }

    free(data->blocks);
    free(data);
}

int geo_block_count(const Geo *geo) {
    const GeoData *data = geo;

    return data == NULL ? 0 : data->count;
}

const Block *geo_block_at(const Geo *geo, int index) {
    const GeoData *data = geo;

    if (data == NULL || index < 0 || index >= data->count) {
        return NULL;
    }

    return data->blocks[index];
}

const Block *geo_find_block(const Geo *geo, const char *cep) {
    const GeoData *data = geo;
    int i;

    if (data == NULL || cep == NULL) {
        return NULL;
    }

    for (i = 0; i < data->count; i++) {
        if (strcmp(block_cep(data->blocks[i]), cep) == 0) {
            return data->blocks[i];
        }
    }

    return NULL;
}

const char *geo_error(const Geo *geo) {
    const GeoData *data = geo;

    if (data == NULL || data->error[0] == '\0') {
        return NULL;
    }

    return data->error;
}

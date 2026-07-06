#include "block.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *cep;
    double x;
    double y;
    double width;
    double height;
    double stroke_width;
    char *fill_color;
    char *stroke_color;
} BlockData;

static char *copy_string(const char *text) {
    char *copy;
    size_t length;

    if (text == NULL) {
        return NULL;
    }

    length = strlen(text) + 1;
    copy = malloc(length);
    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, text, length);
    return copy;
}

Block *block_create(const char *cep, double x, double y, double width, double height) {
    BlockData *block;

    if (cep == NULL || width < 0.0 || height < 0.0) {
        return NULL;
    }

    block = malloc(sizeof(BlockData));
    if (block == NULL) {
        return NULL;
    }

    block->cep = copy_string(cep);
    if (block->cep == NULL) {
        free(block);
        return NULL;
    }

    block->x = x;
    block->y = y;
    block->width = width;
    block->height = height;
    block->stroke_width = 1.0;
    block->fill_color = copy_string("none");
    block->stroke_color = copy_string("black");
    if (block->fill_color == NULL || block->stroke_color == NULL) {
        block_destroy(block);
        return NULL;
    }
    return block;
}

void block_destroy(Block *block) {
    BlockData *data = block;

    if (block == NULL) {
        return;
    }

    free(data->cep);
    free(data->fill_color);
    free(data->stroke_color);
    free(data);
}

void block_set_style(Block *block, double stroke_width, const char *fill_color, const char *stroke_color) {
    BlockData *data = block;
    char *new_fill;
    char *new_stroke;

    if (data == NULL || fill_color == NULL || stroke_color == NULL || stroke_width < 0.0) {
        return;
    }

    new_fill = copy_string(fill_color);
    new_stroke = copy_string(stroke_color);
    if (new_fill == NULL || new_stroke == NULL) {
        free(new_fill);
        free(new_stroke);
        return;
    }

    free(data->fill_color);
    free(data->stroke_color);
    data->stroke_width = stroke_width;
    data->fill_color = new_fill;
    data->stroke_color = new_stroke;
}

const char *block_cep(const Block *block) {
    const BlockData *data = block;

    return data == NULL ? NULL : data->cep;
}

double block_x(const Block *block) {
    const BlockData *data = block;

    return data == NULL ? 0.0 : data->x;
}

double block_y(const Block *block) {
    const BlockData *data = block;

    return data == NULL ? 0.0 : data->y;
}

double block_width(const Block *block) {
    const BlockData *data = block;

    return data == NULL ? 0.0 : data->width;
}

double block_height(const Block *block) {
    const BlockData *data = block;

    return data == NULL ? 0.0 : data->height;
}

double block_stroke_width(const Block *block) {
    const BlockData *data = block;

    return data == NULL ? 0.0 : data->stroke_width;
}

const char *block_fill_color(const Block *block) {
    const BlockData *data = block;

    return data == NULL ? NULL : data->fill_color;
}

const char *block_stroke_color(const Block *block) {
    const BlockData *data = block;

    return data == NULL ? NULL : data->stroke_color;
}

int block_address_point(const Block *block, char face, double number, double *x, double *y) {
    const BlockData *data = block;
    char normalized_face;

    if (data == NULL || x == NULL || y == NULL || number < 0.0) {
        return 0;
    }

    normalized_face = (char)toupper((unsigned char)face);

    switch (normalized_face) {
        case 'S':
            if (number > data->width) {
                return 0;
            }
            *x = data->x - number;
            *y = data->y;
            return 1;
        case 'N':
            if (number > data->width) {
                return 0;
            }
            *x = data->x - number;
            *y = data->y + data->height;
            return 1;
        case 'L':
            if (number > data->height) {
                return 0;
            }
            *x = data->x;
            *y = data->y + number;
            return 1;
        case 'O':
            if (number > data->height) {
                return 0;
            }
            *x = data->x - data->width;
            *y = data->y + number;
            return 1;
        default:
            return 0;
    }
}

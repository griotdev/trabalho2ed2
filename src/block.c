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
    return block;
}

void block_destroy(Block *block) {
    BlockData *data = block;

    if (block == NULL) {
        return;
    }

    free(data->cep);
    free(data);
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

int block_address_point(const Block *block, char face, double number, double *x, double *y) {
    const BlockData *data = block;
    char normalized_face;

    if (data == NULL || x == NULL || y == NULL || number < 0.0) {
        return 0;
    }

    normalized_face = (char)toupper((unsigned char)face);

    switch (normalized_face) {
        case 'S':
            *x = data->x - number;
            *y = data->y;
            return 1;
        case 'N':
            *x = data->x - number;
            *y = data->y + data->height;
            return 1;
        case 'L':
            *x = data->x;
            *y = data->y + number;
            return 1;
        case 'O':
            *x = data->x - data->width;
            *y = data->y + number;
            return 1;
        default:
            return 0;
    }
}

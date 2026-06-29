#include "block.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct Block {
    char *cep;
    double x;
    double y;
    double width;
    double height;
};

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
    Block *block;

    if (cep == NULL || width < 0.0 || height < 0.0) {
        return NULL;
    }

    block = malloc(sizeof(Block));
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
    if (block == NULL) {
        return;
    }

    free(block->cep);
    free(block);
}

const char *block_cep(const Block *block) {
    return block == NULL ? NULL : block->cep;
}

double block_x(const Block *block) {
    return block == NULL ? 0.0 : block->x;
}

double block_y(const Block *block) {
    return block == NULL ? 0.0 : block->y;
}

double block_width(const Block *block) {
    return block == NULL ? 0.0 : block->width;
}

double block_height(const Block *block) {
    return block == NULL ? 0.0 : block->height;
}

int block_address_point(const Block *block, char face, double number, double *x, double *y) {
    char normalized_face;

    if (block == NULL || x == NULL || y == NULL || number < 0.0) {
        return 0;
    }

    normalized_face = (char)toupper((unsigned char)face);

    switch (normalized_face) {
        case 'S':
            *x = block->x - number;
            *y = block->y;
            return 1;
        case 'N':
            *x = block->x - number;
            *y = block->y + block->height;
            return 1;
        case 'L':
            *x = block->x;
            *y = block->y + number;
            return 1;
        case 'O':
            *x = block->x - block->width;
            *y = block->y + number;
            return 1;
        default:
            return 0;
    }
}

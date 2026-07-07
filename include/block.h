#ifndef BLOCK_H
#define BLOCK_H

typedef void *Block;

Block block_create(const char *cep, double x, double y, double width, double height);
void block_destroy(Block block);

void block_set_style(Block block, double stroke_width, const char *fill_color, const char *stroke_color);
const char *block_cep(const Block block);
double block_x(const Block block);
double block_y(const Block block);
double block_width(const Block block);
double block_height(const Block block);
double block_stroke_width(const Block block);
const char *block_fill_color(const Block block);
const char *block_stroke_color(const Block block);

int block_address_point(const Block block, char face, double number, double *x, double *y);

#endif

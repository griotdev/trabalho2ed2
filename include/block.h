#ifndef BLOCK_H
#define BLOCK_H

typedef struct Block Block;

Block *block_create(const char *cep, double x, double y, double width, double height);
void block_destroy(Block *block);

const char *block_cep(const Block *block);
double block_x(const Block *block);
double block_y(const Block *block);
double block_width(const Block *block);
double block_height(const Block *block);

int block_address_point(const Block *block, char face, double number, double *x, double *y);

#endif

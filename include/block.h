#ifndef BLOCK_H
#define BLOCK_H

typedef struct Block Block;

Block *block_create(const char *cep, double x, double y, double width, double height);
void block_destroy(Block *block);

#endif

#ifndef GEO_H
#define GEO_H

#include "block.h"

typedef void Geo;

Geo *geo_load(const char *path);
void geo_destroy(Geo *geo);

int geo_block_count(const Geo *geo);
const Block *geo_block_at(const Geo *geo, int index);
const Block *geo_find_block(const Geo *geo, const char *cep);
const char *geo_error(const Geo *geo);

#endif

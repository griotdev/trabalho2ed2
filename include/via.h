#ifndef VIA_H
#define VIA_H

#include "graph.h"

typedef void *Via;

Via via_load(const char *path);
void via_destroy(Via via);

Graph via_graph(Via via);
const char *via_error(const Via via);

#endif



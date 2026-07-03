#ifndef ROAD_EXPANSION_H
#define ROAD_EXPANSION_H

#include "graph.h"

typedef void RoadExpansion;

RoadExpansion *road_expansion_apply(Graph *graph, double speed_limit);
void road_expansion_destroy(RoadExpansion *expansion);

int road_expansion_count(const RoadExpansion *expansion);
int road_expansion_edge_from(const RoadExpansion *expansion, int index);
int road_expansion_edge_to(const RoadExpansion *expansion, int index);
int road_expansion_edge_index(const RoadExpansion *expansion, int index);

#endif

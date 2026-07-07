#ifndef ROAD_COMPONENTS_H
#define ROAD_COMPONENTS_H

#include "graph.h"

typedef void *RoadComponents;

RoadComponents road_components_find_slow(const Graph graph, double speed_limit);
void road_components_destroy(RoadComponents components);

int road_components_count(const RoadComponents components);
double road_components_min_x(const RoadComponents components, int index);
double road_components_min_y(const RoadComponents components, int index);
double road_components_max_x(const RoadComponents components, int index);
double road_components_max_y(const RoadComponents components, int index);

#endif

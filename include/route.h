#ifndef ROUTE_H
#define ROUTE_H

#include "graph.h"

typedef void Route;

#define ROUTE_METRIC_LENGTH 0
#define ROUTE_METRIC_TIME 1

Route *route_shortest_path(const Graph *graph, int origin, int destination, int metric);
void route_destroy(Route *route);

int route_found(const Route *route);
double route_total_weight(const Route *route);
int route_vertex_count(const Route *route);
int route_vertex_at(const Route *route, int index);
int route_edge_count(const Route *route);
int route_edge_from(const Route *route, int index);
int route_edge_to(const Route *route, int index);
int route_edge_index(const Route *route, int index);

#endif

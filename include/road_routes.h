#ifndef ROAD_ROUTES_H
#define ROAD_ROUTES_H

#include "route.h"

/* Rotas calculadas por comandos p?, preservadas para desenho no SVG. */
typedef void *RoadRoutes;

RoadRoutes road_routes_create(void);
void road_routes_destroy(RoadRoutes routes);

/* Copia uma rota encontrada para a colecao, junto com cor e rotulo. */
int road_routes_add(RoadRoutes routes, const Route route, const char *color, const char *label);

/* Acessores das rotas e de seus passos. */
int road_routes_count(const RoadRoutes routes);
const char *road_routes_color(const RoadRoutes routes, int index);
const char *road_routes_label(const RoadRoutes routes, int index);
int road_routes_step_count(const RoadRoutes routes, int index);
int road_routes_step_from(const RoadRoutes routes, int route_index, int step_index);
int road_routes_step_to(const RoadRoutes routes, int route_index, int step_index);
int road_routes_step_edge_index(const RoadRoutes routes, int route_index, int step_index);

#endif

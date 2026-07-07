#ifndef QRY_H
#define QRY_H

#include "geo.h"
#include "graph.h"
#include "registers.h"
#include "road_components.h"
#include "road_expansion.h"
#include "road_routes.h"

int qry_process(const char *qry_path,
                const Geo geo,
                Graph graph,
                Registers registers,
                RoadComponents *road_components,
                RoadExpansion *road_expansion,
                RoadRoutes road_routes,
                const char *txt_path);
const char *qry_error(void);

#endif

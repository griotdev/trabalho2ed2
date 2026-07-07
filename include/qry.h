#ifndef QRY_H
#define QRY_H

#include "geo.h"
#include "graph.h"
#include "registers.h"
#include "road_components.h"
#include "road_expansion.h"
#include "road_routes.h"

/* Processa comandos QRY e acrescenta as respostas no TXT.
 * road_components e road_expansion sao parametros de saida opcionais.
 */
int qry_process(const char *qry_path,
                const Geo geo,
                Graph graph,
                Registers registers,
                RoadComponents *road_components,
                RoadExpansion *road_expansion,
                RoadRoutes road_routes,
                const char *txt_path);

/* Mensagem do ultimo erro de processamento QRY, ou NULL quando nao ha erro. */
const char *qry_error(void);

#endif

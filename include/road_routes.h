#ifndef ROAD_ROUTES_H
#define ROAD_ROUTES_H

#include "route.h"

/* Modulo de acumulacao de rotas para o SVG.
 *
 * RoadRoutes guarda copias das rotas calculadas por comandos p?, junto com
 * cor e rotulo. Isso permite preservar varios resultados no mesmo SVG.
 */
typedef void *RoadRoutes;

/* Cria uma colecao vazia de rotas. Retorna NULL em erro de memoria. */
RoadRoutes road_routes_create(void);

/* Libera rotas, cores e rotulos armazenados. Aceita NULL. */
void road_routes_destroy(RoadRoutes routes);

/* Copia uma rota encontrada para a colecao, junto com cor e rotulo.
 * Rotas nao encontradas ou vazias nao devem ser adicionadas. Retorna 1 em
 * sucesso e 0 em erro.
 */
int road_routes_add(RoadRoutes routes, const Route route, const char *color, const char *label);

/* Acessores das rotas e de seus passos.
 * Cada passo guarda vertice de origem, vertice de destino e indice da aresta
 * usada no grafo original. Indices invalidos retornam NULL ou -1.
 */
int road_routes_count(const RoadRoutes routes);
const char *road_routes_color(const RoadRoutes routes, int index);
const char *road_routes_label(const RoadRoutes routes, int index);
int road_routes_step_count(const RoadRoutes routes, int index);
int road_routes_step_from(const RoadRoutes routes, int route_index, int step_index);
int road_routes_step_to(const RoadRoutes routes, int route_index, int step_index);
int road_routes_step_edge_index(const RoadRoutes routes, int route_index, int step_index);

#endif

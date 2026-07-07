#ifndef ROAD_COMPONENTS_H
#define ROAD_COMPONENTS_H

#include "graph.h"

/* Resultado de regs: caixas envolventes dos componentes fortemente conexos
 * formados por arestas viarias lentas.
 */
typedef void *RoadComponents;

/* Calcula componentes fortemente conexos considerando apenas arestas com
 * velocidade menor que speed_limit. Arestas rapidas sao desabilitadas
 * temporariamente e restauradas antes do retorno.
 */
RoadComponents road_components_find_slow(const Graph graph, double speed_limit);
void road_components_destroy(RoadComponents components);

/* Acessores das caixas envolventes de cada componente. */
int road_components_count(const RoadComponents components);
double road_components_min_x(const RoadComponents components, int index);
double road_components_min_y(const RoadComponents components, int index);
double road_components_max_x(const RoadComponents components, int index);
double road_components_max_y(const RoadComponents components, int index);

#endif

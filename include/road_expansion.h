#ifndef ROAD_EXPANSION_H
#define ROAD_EXPANSION_H

#include "graph.h"

/* Resultado de exp: arestas da AGM/floresta geradora minima que foram ampliadas. */
typedef void *RoadExpansion;

/* Calcula uma AGM em visao nao direcionada do grafo e aumenta em 50% a
 * velocidade das arestas selecionadas com velocidade menor que speed_limit.
 */
RoadExpansion road_expansion_apply(Graph graph, double speed_limit);
void road_expansion_destroy(RoadExpansion expansion);

/* Acessores das arestas efetivamente alteradas. */
int road_expansion_count(const RoadExpansion expansion);
int road_expansion_edge_from(const RoadExpansion expansion, int index);
int road_expansion_edge_to(const RoadExpansion expansion, int index);
int road_expansion_edge_index(const RoadExpansion expansion, int index);

#endif

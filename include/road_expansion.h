#ifndef ROAD_EXPANSION_H
#define ROAD_EXPANSION_H

#include "graph.h"

/* Modulo do comando exp.
 *
 * RoadExpansion armazena as arestas da arvore/floresta geradora minima que
 * tiveram velocidade ampliada e precisam ser destacadas no SVG.
 */
typedef void *RoadExpansion;

/* Calcula uma AGM em visao nao direcionada do grafo.
 * Arestas selecionadas com velocidade menor que speed_limit recebem aumento
 * de 50% na velocidade media. Retorna NULL em erro de memoria/entrada.
 */
RoadExpansion road_expansion_apply(Graph graph, double speed_limit);

/* Libera o resultado de expansao. Aceita NULL. */
void road_expansion_destroy(RoadExpansion expansion);

/* Acessores das arestas efetivamente alteradas.
 * from/to sao indices de vertices no grafo; edge_index e a aresta na lista
 * de adjacencia do vertice from. Indices invalidos retornam -1.
 */
int road_expansion_count(const RoadExpansion expansion);
int road_expansion_edge_from(const RoadExpansion expansion, int index);
int road_expansion_edge_to(const RoadExpansion expansion, int index);
int road_expansion_edge_index(const RoadExpansion expansion, int index);

#endif

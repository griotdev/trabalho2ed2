#ifndef ROAD_COMPONENTS_H
#define ROAD_COMPONENTS_H

#include "graph.h"

/* Modulo do comando regs.
 *
 * RoadComponents armazena as caixas envolventes dos componentes fortemente
 * conexos obtidos apos remover temporariamente as arestas lentas do grafo.
 */
typedef void *RoadComponents;

/* Calcula componentes fortemente conexos desabilitando arestas com
 * velocidade menor que speed_limit.
 * As arestas sao restauradas antes do retorno, inclusive em caso de erro.
 * Retorna NULL em erro de memoria ou entrada invalida.
 */
RoadComponents road_components_find_slow(const Graph graph, double speed_limit);

/* Libera as caixas armazenadas. Aceita NULL. */
void road_components_destroy(RoadComponents components);

/* Acrescenta ao destino as caixas existentes em source. Usado para preservar
 * resultados graficos de multiplos comandos regs no mesmo SVG.
 * Retorna 1 em sucesso e 0 em erro.
 */
int road_components_append_all(RoadComponents destination, const RoadComponents source);

/* Acessores das caixas envolventes de cada componente.
 * Indices invalidos retornam 0.0.
 */
int road_components_count(const RoadComponents components);
double road_components_min_x(const RoadComponents components, int index);
double road_components_min_y(const RoadComponents components, int index);
double road_components_max_x(const RoadComponents components, int index);
double road_components_max_y(const RoadComponents components, int index);

#endif

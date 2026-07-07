#ifndef ROUTE_H
#define ROUTE_H

#include "graph.h"

/* Caminho calculado no grafo viario.
 * Tipo opaco: vertices, arestas e pesos acumulados ficam em route.c.
 */
typedef void *Route;

/* Metricas aceitas por route_shortest_path. */
#define ROUTE_METRIC_LENGTH 0
#define ROUTE_METRIC_TIME 1

/* Executa Dijkstra entre vertices. Respeita arestas desabilitadas do grafo. */
Route route_shortest_path(const Graph graph, int origin, int destination, int metric);
void route_destroy(Route route);

/* Acessores do resultado. route_found indica se o destino foi alcancado. */
int route_found(const Route route);
double route_total_weight(const Route route);
int route_vertex_count(const Route route);
int route_vertex_at(const Route route, int index);

/* Passos da rota em termos de aresta usada no grafo original. */
int route_edge_count(const Route route);
int route_edge_from(const Route route, int index);
int route_edge_to(const Route route, int index);
int route_edge_index(const Route route, int index);

#endif

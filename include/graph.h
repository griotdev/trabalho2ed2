#ifndef GRAPH_H
#define GRAPH_H

/* Grafo viario dirigido em listas de adjacencia.
 * Tipo opaco: vertices, arestas e listas internas ficam em graph.c.
 */
typedef void *Graph;

/* Cria/destroi um grafo vazio. */
Graph graph_create(void);
void graph_destroy(Graph graph);

/* Adiciona vertice identificado por id. Retorna indice do vertice ou -1 em erro. */
int graph_add_vertex(Graph graph, const char *id, double x, double y);

/* Adiciona aresta dirigida from_id -> to_id com dados viarios.
 * Retorna 1 em sucesso e 0 em erro.
 */
int graph_add_edge(Graph graph,
                   const char *from_id,
                   const char *to_id,
                   const char *right_cep,
                   const char *left_cep,
                   double length,
                   double speed,
                   const char *name);

/* Consultas globais de vertices/arestas. */
int graph_vertex_count(const Graph graph);
int graph_edge_count(const Graph graph);
int graph_find_vertex(const Graph graph, const char *id);
int graph_nearest_vertex(const Graph graph, double x, double y);

/* Acessores de vertices. Indices invalidos retornam NULL ou 0.0. */
const char *graph_vertex_id(const Graph graph, int vertex_index);
double graph_vertex_x(const Graph graph, int vertex_index);
double graph_vertex_y(const Graph graph, int vertex_index);

/* Acessores de arestas por lista de adjacencia do vertice de origem. */
int graph_out_degree(const Graph graph, int vertex_index);
int graph_edge_to(const Graph graph, int vertex_index, int edge_index);
const char *graph_edge_name(const Graph graph, int vertex_index, int edge_index);
const char *graph_edge_right_cep(const Graph graph, int vertex_index, int edge_index);
const char *graph_edge_left_cep(const Graph graph, int vertex_index, int edge_index);
double graph_edge_length(const Graph graph, int vertex_index, int edge_index);
double graph_edge_speed(const Graph graph, int vertex_index, int edge_index);
int graph_set_edge_speed(Graph graph, int vertex_index, int edge_index, double speed);

/* Estado temporario de arestas para algoritmos que precisam desabilitar/reabilitar vias. */
int graph_edge_enabled(const Graph graph, int vertex_index, int edge_index);
int graph_set_edge_enabled(Graph graph, int vertex_index, int edge_index, int enabled);

/* Atualiza velocidade das arestas cujas duas extremidades estao no retangulo. */
int graph_update_speeds_in_rect(Graph graph, double speed, double x, double y, double width, double height);

#endif


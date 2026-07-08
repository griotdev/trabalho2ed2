#ifndef GRAPH_H
#define GRAPH_H

/* Modulo do grafo viario.
 *
 * Graph e um grafo dirigido armazenado por listas de adjacencia. O tipo e
 * opaco: vertices, arestas, capacidades e listas internas ficam em graph.c.
 */
typedef void *Graph;

/* Cria um grafo vazio. Retorna NULL em erro de memoria. */
Graph graph_create(void);

/* Libera vertices, arestas e strings do grafo. Aceita NULL. */
void graph_destroy(Graph graph);

/* Adiciona um vertice identificado por id nas coordenadas x/y.
 * Retorna o indice do vertice ou -1 em erro/duplicidade.
 */
int graph_add_vertex(Graph graph, const char *id, double x, double y);

/* Adiciona uma aresta dirigida from_id -> to_id com dados viarios.
 * right_cep/left_cep indicam as quadras adjacentes, length e o comprimento,
 * speed e a velocidade media, e name e o nome da via. Retorna 1 em sucesso.
 */
int graph_add_edge(Graph graph,
                   const char *from_id,
                   const char *to_id,
                   const char *right_cep,
                   const char *left_cep,
                   double length,
                   double speed,
                   const char *name);

/* Consultas globais de vertices/arestas.
 * graph_find_vertex retorna -1 quando id nao existe.
 * graph_nearest_vertex retorna o indice do vertice mais proximo de x/y.
 */
int graph_vertex_count(const Graph graph);
int graph_edge_count(const Graph graph);
int graph_find_vertex(const Graph graph, const char *id);
int graph_nearest_vertex(const Graph graph, double x, double y);

/* Acessores de vertices. Indices invalidos retornam NULL ou 0.0. */
const char *graph_vertex_id(const Graph graph, int vertex_index);
double graph_vertex_x(const Graph graph, int vertex_index);
double graph_vertex_y(const Graph graph, int vertex_index);

/* Acessores de arestas pela lista de adjacencia do vertice de origem.
 * vertex_index identifica a origem; edge_index identifica a posicao da aresta
 * nessa lista. Indices invalidos retornam NULL, -1 ou 0.0 conforme o tipo.
 */
int graph_out_degree(const Graph graph, int vertex_index);
int graph_edge_to(const Graph graph, int vertex_index, int edge_index);
const char *graph_edge_name(const Graph graph, int vertex_index, int edge_index);
const char *graph_edge_right_cep(const Graph graph, int vertex_index, int edge_index);
const char *graph_edge_left_cep(const Graph graph, int vertex_index, int edge_index);
double graph_edge_length(const Graph graph, int vertex_index, int edge_index);
double graph_edge_speed(const Graph graph, int vertex_index, int edge_index);

/* Altera a velocidade media de uma aresta existente. Retorna 1 em sucesso. */
int graph_set_edge_speed(Graph graph, int vertex_index, int edge_index, double speed);

/* Estado temporario de arestas.
 * Usado por algoritmos como componentes fortemente conexos, que precisam
 * desabilitar vias lentas e restaura-las ao final.
 */
int graph_edge_enabled(const Graph graph, int vertex_index, int edge_index);
int graph_set_edge_enabled(Graph graph, int vertex_index, int edge_index, int enabled);

/* Atualiza a velocidade das arestas cujas duas extremidades estao no retangulo.
 * Retorna a quantidade de arestas alteradas.
 */
int graph_update_speeds_in_rect(Graph graph, double speed, double x, double y, double width, double height);

#endif

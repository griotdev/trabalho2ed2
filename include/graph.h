#ifndef GRAPH_H
#define GRAPH_H

typedef void Graph;

Graph *graph_create(void);
void graph_destroy(Graph *graph);

int graph_add_vertex(Graph *graph, const char *id, double x, double y);
int graph_add_edge(Graph *graph,
                   const char *from_id,
                   const char *to_id,
                   const char *right_cep,
                   const char *left_cep,
                   double length,
                   double speed,
                   const char *name);

int graph_vertex_count(const Graph *graph);
int graph_edge_count(const Graph *graph);
int graph_find_vertex(const Graph *graph, const char *id);
int graph_nearest_vertex(const Graph *graph, double x, double y);
const char *graph_vertex_id(const Graph *graph, int vertex_index);
double graph_vertex_x(const Graph *graph, int vertex_index);
double graph_vertex_y(const Graph *graph, int vertex_index);

int graph_out_degree(const Graph *graph, int vertex_index);
int graph_edge_to(const Graph *graph, int vertex_index, int edge_index);
const char *graph_edge_name(const Graph *graph, int vertex_index, int edge_index);
const char *graph_edge_right_cep(const Graph *graph, int vertex_index, int edge_index);
const char *graph_edge_left_cep(const Graph *graph, int vertex_index, int edge_index);
double graph_edge_length(const Graph *graph, int vertex_index, int edge_index);
double graph_edge_speed(const Graph *graph, int vertex_index, int edge_index);
int graph_set_edge_speed(Graph *graph, int vertex_index, int edge_index, double speed);
int graph_update_speeds_in_rect(Graph *graph, double speed, double x, double y, double width, double height);

#endif


#include "graph.h"

#include <stdlib.h>
#include <string.h>

typedef struct EdgeData EdgeData;

typedef struct {
    char *id;
    double x;
    double y;
    EdgeData *first_edge;
    EdgeData *last_edge;
    int out_degree;
} VertexData;

struct EdgeData {
    int to;
    char *right_cep;
    char *left_cep;
    double length;
    double speed;
    char *name;
    EdgeData *next;
};

typedef struct {
    VertexData *vertices;
    int vertex_count;
    int vertex_capacity;
    int edge_count;
} GraphData;

static char *copy_string(const char *text) {
    char *copy;
    size_t length;

    if (text == NULL) {
        return NULL;
    }

    length = strlen(text) + 1U;
    copy = malloc(length);
    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, text, length);
    return copy;
}

static int ensure_vertex_capacity(GraphData *graph) {
    VertexData *new_vertices;
    int new_capacity;

    if (graph->vertex_count < graph->vertex_capacity) {
        return 1;
    }

    new_capacity = graph->vertex_capacity == 0 ? 8 : graph->vertex_capacity * 2;
    new_vertices = realloc(graph->vertices, (size_t)new_capacity * sizeof(VertexData));
    if (new_vertices == NULL) {
        return 0;
    }

    graph->vertices = new_vertices;
    graph->vertex_capacity = new_capacity;
    return 1;
}

static EdgeData *edge_at(const GraphData *graph, int vertex_index, int edge_index) {
    EdgeData *edge;
    int i;

    if (graph == NULL || vertex_index < 0 || vertex_index >= graph->vertex_count || edge_index < 0) {
        return NULL;
    }

    edge = graph->vertices[vertex_index].first_edge;
    for (i = 0; edge != NULL && i < edge_index; i++) {
        edge = edge->next;
    }

    return edge;
}

Graph *graph_create(void) {
    GraphData *graph = calloc(1, sizeof(GraphData));

    return graph;
}

void graph_destroy(Graph *graph) {
    GraphData *data = graph;
    int i;

    if (data == NULL) {
        return;
    }

    for (i = 0; i < data->vertex_count; i++) {
        EdgeData *edge = data->vertices[i].first_edge;

        while (edge != NULL) {
            EdgeData *next = edge->next;
            free(edge->right_cep);
            free(edge->left_cep);
            free(edge->name);
            free(edge);
            edge = next;
        }

        free(data->vertices[i].id);
    }

    free(data->vertices);
    free(data);
}

int graph_add_vertex(Graph *graph, const char *id, double x, double y) {
    GraphData *data = graph;
    VertexData *vertex;

    if (data == NULL || id == NULL || graph_find_vertex(graph, id) >= 0 || !ensure_vertex_capacity(data)) {
        return -1;
    }

    vertex = &data->vertices[data->vertex_count];
    vertex->id = copy_string(id);
    if (vertex->id == NULL) {
        return -1;
    }

    vertex->x = x;
    vertex->y = y;
    vertex->first_edge = NULL;
    vertex->last_edge = NULL;
    vertex->out_degree = 0;

    data->vertex_count++;
    return data->vertex_count - 1;
}

int graph_add_edge(Graph *graph,
                   const char *from_id,
                   const char *to_id,
                   const char *right_cep,
                   const char *left_cep,
                   double length,
                   double speed,
                   const char *name) {
    GraphData *data = graph;
    EdgeData *edge;
    int from;
    int to;

    if (data == NULL || from_id == NULL || to_id == NULL || right_cep == NULL || left_cep == NULL || name == NULL) {
        return 0;
    }

    from = graph_find_vertex(graph, from_id);
    to = graph_find_vertex(graph, to_id);
    if (from < 0 || to < 0 || length < 0.0 || speed < 0.0) {
        return 0;
    }

    edge = calloc(1, sizeof(EdgeData));
    if (edge == NULL) {
        return 0;
    }

    edge->right_cep = copy_string(right_cep);
    edge->left_cep = copy_string(left_cep);
    edge->name = copy_string(name);
    if (edge->right_cep == NULL || edge->left_cep == NULL || edge->name == NULL) {
        free(edge->right_cep);
        free(edge->left_cep);
        free(edge->name);
        free(edge);
        return 0;
    }

    edge->to = to;
    edge->length = length;
    edge->speed = speed;

    if (data->vertices[from].last_edge == NULL) {
        data->vertices[from].first_edge = edge;
    } else {
        data->vertices[from].last_edge->next = edge;
    }
    data->vertices[from].last_edge = edge;
    data->vertices[from].out_degree++;
    data->edge_count++;
    return 1;
}

int graph_vertex_count(const Graph *graph) {
    const GraphData *data = graph;

    return data == NULL ? 0 : data->vertex_count;
}

int graph_edge_count(const Graph *graph) {
    const GraphData *data = graph;

    return data == NULL ? 0 : data->edge_count;
}

int graph_find_vertex(const Graph *graph, const char *id) {
    const GraphData *data = graph;
    int i;

    if (data == NULL || id == NULL) {
        return -1;
    }

    for (i = 0; i < data->vertex_count; i++) {
        if (strcmp(data->vertices[i].id, id) == 0) {
            return i;
        }
    }

    return -1;
}

int graph_nearest_vertex(const Graph *graph, double x, double y) {
    const GraphData *data = graph;
    double best_distance = 0.0;
    int best_index = -1;
    int i;

    if (data == NULL || data->vertex_count == 0) {
        return -1;
    }

    for (i = 0; i < data->vertex_count; i++) {
        double dx = data->vertices[i].x - x;
        double dy = data->vertices[i].y - y;
        double distance = dx * dx + dy * dy;

        if (best_index < 0 || distance < best_distance) {
            best_distance = distance;
            best_index = i;
        }
    }

    return best_index;
}

const char *graph_vertex_id(const Graph *graph, int vertex_index) {
    const GraphData *data = graph;

    if (data == NULL || vertex_index < 0 || vertex_index >= data->vertex_count) {
        return NULL;
    }

    return data->vertices[vertex_index].id;
}

double graph_vertex_x(const Graph *graph, int vertex_index) {
    const GraphData *data = graph;

    if (data == NULL || vertex_index < 0 || vertex_index >= data->vertex_count) {
        return 0.0;
    }

    return data->vertices[vertex_index].x;
}

double graph_vertex_y(const Graph *graph, int vertex_index) {
    const GraphData *data = graph;

    if (data == NULL || vertex_index < 0 || vertex_index >= data->vertex_count) {
        return 0.0;
    }

    return data->vertices[vertex_index].y;
}

int graph_out_degree(const Graph *graph, int vertex_index) {
    const GraphData *data = graph;

    if (data == NULL || vertex_index < 0 || vertex_index >= data->vertex_count) {
        return 0;
    }

    return data->vertices[vertex_index].out_degree;
}

int graph_edge_to(const Graph *graph, int vertex_index, int edge_index) {
    const GraphData *data = graph;
    EdgeData *edge = edge_at(data, vertex_index, edge_index);

    return edge == NULL ? -1 : edge->to;
}

const char *graph_edge_name(const Graph *graph, int vertex_index, int edge_index) {
    const GraphData *data = graph;
    EdgeData *edge = edge_at(data, vertex_index, edge_index);

    return edge == NULL ? NULL : edge->name;
}

const char *graph_edge_right_cep(const Graph *graph, int vertex_index, int edge_index) {
    const GraphData *data = graph;
    EdgeData *edge = edge_at(data, vertex_index, edge_index);

    return edge == NULL ? NULL : edge->right_cep;
}

const char *graph_edge_left_cep(const Graph *graph, int vertex_index, int edge_index) {
    const GraphData *data = graph;
    EdgeData *edge = edge_at(data, vertex_index, edge_index);

    return edge == NULL ? NULL : edge->left_cep;
}

double graph_edge_length(const Graph *graph, int vertex_index, int edge_index) {
    const GraphData *data = graph;
    EdgeData *edge = edge_at(data, vertex_index, edge_index);

    return edge == NULL ? 0.0 : edge->length;
}

double graph_edge_speed(const Graph *graph, int vertex_index, int edge_index) {
    const GraphData *data = graph;
    EdgeData *edge = edge_at(data, vertex_index, edge_index);

    return edge == NULL ? 0.0 : edge->speed;
}

static int graph_point_inside_rect(double px, double py, double x, double y, double width, double height) {
    return px >= x && px <= x + width && py >= y && py <= y + height;
}

int graph_update_speeds_in_rect(Graph *graph, double speed, double x, double y, double width, double height) {
    GraphData *data = graph;
    int updated = 0;
    int i;

    if (data == NULL || speed < 0.0 || width < 0.0 || height < 0.0) {
        return -1;
    }

    for (i = 0; i < data->vertex_count; i++) {
        EdgeData *edge;
        int from_inside = graph_point_inside_rect(data->vertices[i].x, data->vertices[i].y, x, y, width, height);

        if (!from_inside) {
            continue;
        }

        edge = data->vertices[i].first_edge;
        while (edge != NULL) {
            VertexData *to = &data->vertices[edge->to];
            if (graph_point_inside_rect(to->x, to->y, x, y, width, height)) {
                edge->speed = speed;
                updated++;
            }
            edge = edge->next;
        }
    }

    return updated;
}



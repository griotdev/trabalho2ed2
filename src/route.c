#include "route.h"

#include <float.h>
#include <stdlib.h>

typedef struct {
    int from;
    int to;
    int edge_index;
} RouteStepData;

typedef struct {
    int found;
    double total_weight;
    int vertex_count;
    int *vertices;
    int step_count;
    RouteStepData *steps;
} RouteData;

static RouteData *route_create_empty(int found) {
    RouteData *route = calloc(1, sizeof(RouteData));

    if (route == NULL) {
        return NULL;
    }

    route->found = found;
    return route;
}

static double edge_weight(const Graph graph, int vertex, int edge_index, int metric) {
    double length = graph_edge_length(graph, vertex, edge_index);
    double speed;

    if (metric == ROUTE_METRIC_LENGTH) {
        return length;
    }

    speed = graph_edge_speed(graph, vertex, edge_index);
    if (speed <= 0.0) {
        return DBL_MAX;
    }

    return length / speed;
}

static int find_next_vertex(const double *distances, const int *visited, int count) {
    double best = DBL_MAX;
    int best_index = -1;
    int i;

    for (i = 0; i < count; i++) {
        if (!visited[i] && distances[i] < best) {
            best = distances[i];
            best_index = i;
        }
    }

    return best_index;
}

static RouteData *build_route(int origin,
                              int destination,
                              const double *distances,
                              const int *previous_vertex,
                              const int *previous_edge) {
    RouteData *route;
    int cursor;
    int count = 1;
    int index;

    cursor = destination;
    while (cursor != origin) {
        cursor = previous_vertex[cursor];
        if (cursor < 0) {
            return route_create_empty(0);
        }
        count++;
    }

    route = route_create_empty(1);
    if (route == NULL) {
        return NULL;
    }

    route->total_weight = distances[destination];
    route->vertex_count = count;
    route->step_count = count - 1;
    route->vertices = malloc((size_t)count * sizeof(int));
    if (route->vertices == NULL) {
        route_destroy(route);
        return NULL;
    }

    if (route->step_count > 0) {
        route->steps = malloc((size_t)route->step_count * sizeof(RouteStepData));
        if (route->steps == NULL) {
            route_destroy(route);
            return NULL;
        }
    }

    cursor = destination;
    for (index = count - 1; index >= 0; index--) {
        route->vertices[index] = cursor;
        if (index > 0) {
            route->steps[index - 1].from = previous_vertex[cursor];
            route->steps[index - 1].to = cursor;
            route->steps[index - 1].edge_index = previous_edge[cursor];
        }
        cursor = previous_vertex[cursor];
    }
    return route;
}

Route route_shortest_path(const Graph graph, int origin, int destination, int metric) {
    double *distances;
    int *previous_vertex;
    int *previous_edge;
    int *visited;
    int count;
    int i;
    RouteData *route;

    if (graph == NULL || origin < 0 || destination < 0 ||
        (metric != ROUTE_METRIC_LENGTH && metric != ROUTE_METRIC_TIME)) {
        return NULL;
    }

    count = graph_vertex_count(graph);
    if (origin >= count || destination >= count) {
        return NULL;
    }

    distances = malloc((size_t)count * sizeof(double));
    previous_vertex = malloc((size_t)count * sizeof(int));
    previous_edge = malloc((size_t)count * sizeof(int));
    visited = calloc((size_t)count, sizeof(int));
    if (distances == NULL || previous_vertex == NULL || previous_edge == NULL || visited == NULL) {
        free(distances);
        free(previous_vertex);
        free(previous_edge);
        free(visited);
        return NULL;
    }

    for (i = 0; i < count; i++) {
        distances[i] = DBL_MAX;
        previous_vertex[i] = -1;
        previous_edge[i] = -1;
    }
    distances[origin] = 0.0;

    for (;;) {
        int vertex = find_next_vertex(distances, visited, count);
        int degree;
        int edge_index;

        if (vertex < 0 || vertex == destination) {
            break;
        }

        visited[vertex] = 1;
        degree = graph_out_degree(graph, vertex);
        for (edge_index = 0; edge_index < degree; edge_index++) {
            int to = graph_edge_to(graph, vertex, edge_index);
            double weight = edge_weight(graph, vertex, edge_index, metric);
            double candidate;

            if (!graph_edge_enabled(graph, vertex, edge_index)) {
                continue;
            }
            if (to < 0 || visited[to] || weight == DBL_MAX || distances[vertex] == DBL_MAX) {
                continue;
            }

            candidate = distances[vertex] + weight;
            if (candidate < distances[to]) {
                distances[to] = candidate;
                previous_vertex[to] = vertex;
                previous_edge[to] = edge_index;
            }
        }
    }

    if (distances[destination] == DBL_MAX) {
        route = route_create_empty(0);
    } else {
        route = build_route(origin, destination, distances, previous_vertex, previous_edge);
    }

    free(distances);
    free(previous_vertex);
    free(previous_edge);
    free(visited);
    return route;
}

void route_destroy(Route route) {
    RouteData *data = route;

    if (data == NULL) {
        return;
    }

    free(data->vertices);
    free(data->steps);
    free(data);
}

int route_found(const Route route) {
    const RouteData *data = route;

    return data == NULL ? 0 : data->found;
}

double route_total_weight(const Route route) {
    const RouteData *data = route;

    return data == NULL ? 0.0 : data->total_weight;
}

int route_vertex_count(const Route route) {
    const RouteData *data = route;

    return data == NULL ? 0 : data->vertex_count;
}

int route_vertex_at(const Route route, int index) {
    const RouteData *data = route;

    if (data == NULL || index < 0 || index >= data->vertex_count) {
        return -1;
    }

    return data->vertices[index];
}

int route_edge_count(const Route route) {
    const RouteData *data = route;

    return data == NULL ? 0 : data->step_count;
}

int route_edge_from(const Route route, int index) {
    const RouteData *data = route;

    if (data == NULL || index < 0 || index >= data->step_count) {
        return -1;
    }

    return data->steps[index].from;
}

int route_edge_to(const Route route, int index) {
    const RouteData *data = route;

    if (data == NULL || index < 0 || index >= data->step_count) {
        return -1;
    }

    return data->steps[index].to;
}

int route_edge_index(const Route route, int index) {
    const RouteData *data = route;

    if (data == NULL || index < 0 || index >= data->step_count) {
        return -1;
    }

    return data->steps[index].edge_index;
}


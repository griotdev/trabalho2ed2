#include "road_components.h"

#include <stdlib.h>

typedef struct {
    double min_x;
    double min_y;
    double max_x;
    double max_y;
} RoadComponentData;

typedef struct {
    RoadComponentData *items;
    int count;
    int capacity;
} RoadComponentsData;

static int ensure_capacity(RoadComponentsData *components) {
    RoadComponentData *new_items;
    int new_capacity;

    if (components->count < components->capacity) {
        return 1;
    }

    new_capacity = components->capacity == 0 ? 4 : components->capacity * 2;
    new_items = realloc(components->items, (size_t)new_capacity * sizeof(RoadComponentData));
    if (new_items == NULL) {
        return 0;
    }

    components->items = new_items;
    components->capacity = new_capacity;
    return 1;
}

static int has_slow_connection(const Graph *graph, int a, int b, double speed_limit) {
    int edge_index;

    for (edge_index = 0; edge_index < graph_out_degree(graph, a); edge_index++) {
        if (graph_edge_to(graph, a, edge_index) == b && graph_edge_speed(graph, a, edge_index) < speed_limit) {
            return 1;
        }
    }

    for (edge_index = 0; edge_index < graph_out_degree(graph, b); edge_index++) {
        if (graph_edge_to(graph, b, edge_index) == a && graph_edge_speed(graph, b, edge_index) < speed_limit) {
            return 1;
        }
    }

    return 0;
}

static int vertex_has_slow_edge(const Graph *graph, int vertex, double speed_limit) {
    int other;

    for (other = 0; other < graph_vertex_count(graph); other++) {
        if (other != vertex && has_slow_connection(graph, vertex, other, speed_limit)) {
            return 1;
        }
    }

    return 0;
}

static void expand_box(RoadComponentData *component, const Graph *graph, int vertex) {
    double x = graph_vertex_x(graph, vertex);
    double y = graph_vertex_y(graph, vertex);

    if (x < component->min_x) {
        component->min_x = x;
    }
    if (y < component->min_y) {
        component->min_y = y;
    }
    if (x > component->max_x) {
        component->max_x = x;
    }
    if (y > component->max_y) {
        component->max_y = y;
    }
}

static int append_component(RoadComponentsData *components, RoadComponentData component) {
    if (!ensure_capacity(components)) {
        return 0;
    }

    components->items[components->count] = component;
    components->count++;
    return 1;
}

static int build_component(const Graph *graph,
                           double speed_limit,
                           int start,
                           int *visited,
                           int *stack,
                           RoadComponentsData *components) {
    RoadComponentData component;
    int top = 0;

    component.min_x = graph_vertex_x(graph, start);
    component.max_x = component.min_x;
    component.min_y = graph_vertex_y(graph, start);
    component.max_y = component.min_y;

    visited[start] = 1;
    stack[top++] = start;

    while (top > 0) {
        int vertex = stack[--top];
        int other;

        expand_box(&component, graph, vertex);
        for (other = 0; other < graph_vertex_count(graph); other++) {
            if (!visited[other] && other != vertex && has_slow_connection(graph, vertex, other, speed_limit)) {
                visited[other] = 1;
                stack[top++] = other;
            }
        }
    }

    return append_component(components, component);
}

RoadComponents *road_components_find_slow(const Graph *graph, double speed_limit) {
    RoadComponentsData *components;
    int *visited;
    int *stack;
    int vertex_count;
    int i;

    if (graph == NULL || speed_limit < 0.0) {
        return NULL;
    }

    vertex_count = graph_vertex_count(graph);
    components = calloc(1, sizeof(RoadComponentsData));
    if (components == NULL) {
        return NULL;
    }
    if (vertex_count == 0) {
        return components;
    }

    visited = calloc((size_t)vertex_count, sizeof(int));
    stack = malloc((size_t)vertex_count * sizeof(int));
    if (visited == NULL || stack == NULL) {
        road_components_destroy(components);
        free(visited);
        free(stack);
        return NULL;
    }

    for (i = 0; i < vertex_count; i++) {
        if (!visited[i] && vertex_has_slow_edge(graph, i, speed_limit)) {
            if (!build_component(graph, speed_limit, i, visited, stack, components)) {
                road_components_destroy(components);
                free(visited);
                free(stack);
                return NULL;
            }
        }
    }

    free(visited);
    free(stack);
    return components;
}

void road_components_destroy(RoadComponents *components) {
    RoadComponentsData *data = components;

    if (data == NULL) {
        return;
    }

    free(data->items);
    free(data);
}

int road_components_count(const RoadComponents *components) {
    const RoadComponentsData *data = components;

    return data == NULL ? 0 : data->count;
}

double road_components_min_x(const RoadComponents *components, int index) {
    const RoadComponentsData *data = components;

    if (data == NULL || index < 0 || index >= data->count) {
        return 0.0;
    }

    return data->items[index].min_x;
}

double road_components_min_y(const RoadComponents *components, int index) {
    const RoadComponentsData *data = components;

    if (data == NULL || index < 0 || index >= data->count) {
        return 0.0;
    }

    return data->items[index].min_y;
}

double road_components_max_x(const RoadComponents *components, int index) {
    const RoadComponentsData *data = components;

    if (data == NULL || index < 0 || index >= data->count) {
        return 0.0;
    }

    return data->items[index].max_x;
}

double road_components_max_y(const RoadComponents *components, int index) {
    const RoadComponentsData *data = components;

    if (data == NULL || index < 0 || index >= data->count) {
        return 0.0;
    }

    return data->items[index].max_y;
}


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

typedef struct {
    int from;
    int edge_index;
    int enabled;
} EdgeStateData;

typedef struct TarjanContextData TarjanContextData;

struct TarjanContextData {
    Graph graph;
    int *indices;
    int *lowlinks;
    int *on_stack;
    int *stack;
    int *component_vertices;
    int stack_size;
    int next_index;
    int ok;
    RoadComponentsData *components;
};

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

static int append_component(RoadComponentsData *components, RoadComponentData component) {
    if (!ensure_capacity(components)) {
        return 0;
    }

    components->items[components->count] = component;
    components->count++;
    return 1;
}

static int save_and_filter_edges(Graph graph, double speed_limit, EdgeStateData **states, int *state_count) {
    EdgeStateData *items;
    int count = 0;
    int vertex;

    *states = NULL;
    *state_count = 0;

    if (graph_edge_count(graph) == 0) {
        return 1;
    }

    items = malloc((size_t)graph_edge_count(graph) * sizeof(EdgeStateData));
    if (items == NULL) {
        return 0;
    }

    for (vertex = 0; vertex < graph_vertex_count(graph); vertex++) {
        int edge_index;

        for (edge_index = 0; edge_index < graph_out_degree(graph, vertex); edge_index++) {
            int enabled = graph_edge_enabled(graph, vertex, edge_index);

            items[count].from = vertex;
            items[count].edge_index = edge_index;
            items[count].enabled = enabled;
            count++;

            if (enabled && graph_edge_speed(graph, vertex, edge_index) >= speed_limit) {
                graph_set_edge_enabled(graph, vertex, edge_index, 0);
            }
        }
    }

    *states = items;
    *state_count = count;
    return 1;
}

static void restore_edge_states(Graph graph, EdgeStateData *states, int state_count) {
    int i;

    for (i = 0; i < state_count; i++) {
        graph_set_edge_enabled(graph, states[i].from, states[i].edge_index, states[i].enabled);
    }
}

static int component_has_self_loop(const Graph graph, const int *vertices, int count) {
    int i;

    for (i = 0; i < count; i++) {
        int vertex = vertices[i];
        int edge_index;

        for (edge_index = 0; edge_index < graph_out_degree(graph, vertex); edge_index++) {
            if (graph_edge_enabled(graph, vertex, edge_index) && graph_edge_to(graph, vertex, edge_index) == vertex) {
                return 1;
            }
        }
    }

    return 0;
}

static void expand_box(RoadComponentData *component, const Graph graph, int vertex) {
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

static int append_strong_component(TarjanContextData *context, int count) {
    RoadComponentData component;
    int i;

    if (count <= 0 || (count == 1 && !component_has_self_loop(context->graph, context->component_vertices, count))) {
        return 1;
    }

    component.min_x = graph_vertex_x(context->graph, context->component_vertices[0]);
    component.max_x = component.min_x;
    component.min_y = graph_vertex_y(context->graph, context->component_vertices[0]);
    component.max_y = component.min_y;

    for (i = 1; i < count; i++) {
        expand_box(&component, context->graph, context->component_vertices[i]);
    }

    return append_component(context->components, component);
}

static void strong_connect(TarjanContextData *context, int vertex) {
    int edge_index;

    context->indices[vertex] = context->next_index;
    context->lowlinks[vertex] = context->next_index;
    context->next_index++;
    context->stack[context->stack_size++] = vertex;
    context->on_stack[vertex] = 1;

    for (edge_index = 0; edge_index < graph_out_degree(context->graph, vertex); edge_index++) {
        int other;

        if (!graph_edge_enabled(context->graph, vertex, edge_index)) {
            continue;
        }

        other = graph_edge_to(context->graph, vertex, edge_index);
        if (other < 0) {
            continue;
        }

        if (context->indices[other] < 0) {
            strong_connect(context, other);
            if (!context->ok) {
                return;
            }
            if (context->lowlinks[other] < context->lowlinks[vertex]) {
                context->lowlinks[vertex] = context->lowlinks[other];
            }
        } else if (context->on_stack[other] && context->indices[other] < context->lowlinks[vertex]) {
            context->lowlinks[vertex] = context->indices[other];
        }
    }

    if (context->lowlinks[vertex] == context->indices[vertex]) {
        int count = 0;
        int other;

        do {
            other = context->stack[--context->stack_size];
            context->on_stack[other] = 0;
            context->component_vertices[count++] = other;
        } while (other != vertex);

        if (!append_strong_component(context, count)) {
            context->ok = 0;
        }
    }
}

static int find_strong_components(const Graph graph, RoadComponentsData *components) {
    TarjanContextData context;
    int vertex_count = graph_vertex_count(graph);
    int i;

    if (vertex_count == 0) {
        return 1;
    }

    context.graph = graph;
    context.indices = malloc((size_t)vertex_count * sizeof(int));
    context.lowlinks = malloc((size_t)vertex_count * sizeof(int));
    context.on_stack = calloc((size_t)vertex_count, sizeof(int));
    context.stack = malloc((size_t)vertex_count * sizeof(int));
    context.component_vertices = malloc((size_t)vertex_count * sizeof(int));
    context.stack_size = 0;
    context.next_index = 0;
    context.ok = 1;
    context.components = components;

    if (context.indices == NULL || context.lowlinks == NULL || context.on_stack == NULL ||
        context.stack == NULL || context.component_vertices == NULL) {
        free(context.indices);
        free(context.lowlinks);
        free(context.on_stack);
        free(context.stack);
        free(context.component_vertices);
        return 0;
    }

    for (i = 0; i < vertex_count; i++) {
        context.indices[i] = -1;
        context.lowlinks[i] = -1;
    }

    for (i = 0; i < vertex_count && context.ok; i++) {
        if (context.indices[i] < 0) {
            strong_connect(&context, i);
        }
    }

    free(context.indices);
    free(context.lowlinks);
    free(context.on_stack);
    free(context.stack);
    free(context.component_vertices);
    return context.ok;
}

RoadComponents road_components_find_slow(const Graph graph, double speed_limit) {
    RoadComponentsData *components;
    EdgeStateData *states;
    int state_count;
    int ok;

    if (graph == NULL || speed_limit < 0.0) {
        return NULL;
    }

    components = calloc(1, sizeof(RoadComponentsData));
    if (components == NULL) {
        return NULL;
    }

    if (!save_and_filter_edges(graph, speed_limit, &states, &state_count)) {
        road_components_destroy(components);
        return NULL;
    }

    ok = find_strong_components(graph, components);
    restore_edge_states(graph, states, state_count);
    free(states);

    if (!ok) {
        road_components_destroy(components);
        return NULL;
    }

    return components;
}

void road_components_destroy(RoadComponents components) {
    RoadComponentsData *data = components;

    if (data == NULL) {
        return;
    }

    free(data->items);
    free(data);
}

int road_components_append_all(RoadComponents destination, const RoadComponents source) {
    RoadComponentsData *dest = destination;
    const RoadComponentsData *src = source;
    int i;

    if (dest == NULL || src == NULL) {
        return 0;
    }

    for (i = 0; i < src->count; i++) {
        if (!append_component(dest, src->items[i])) {
            return 0;
        }
    }

    return 1;
}

int road_components_count(const RoadComponents components) {
    const RoadComponentsData *data = components;

    return data == NULL ? 0 : data->count;
}

double road_components_min_x(const RoadComponents components, int index) {
    const RoadComponentsData *data = components;

    if (data == NULL || index < 0 || index >= data->count) {
        return 0.0;
    }

    return data->items[index].min_x;
}

double road_components_min_y(const RoadComponents components, int index) {
    const RoadComponentsData *data = components;

    if (data == NULL || index < 0 || index >= data->count) {
        return 0.0;
    }

    return data->items[index].min_y;
}

double road_components_max_x(const RoadComponents components, int index) {
    const RoadComponentsData *data = components;

    if (data == NULL || index < 0 || index >= data->count) {
        return 0.0;
    }

    return data->items[index].max_x;
}

double road_components_max_y(const RoadComponents components, int index) {
    const RoadComponentsData *data = components;

    if (data == NULL || index < 0 || index >= data->count) {
        return 0.0;
    }

    return data->items[index].max_y;
}

#include "road_routes.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    int from;
    int to;
    int edge_index;
} RoadRouteStepData;

typedef struct {
    char *color;
    char *label;
    int step_count;
    RoadRouteStepData *steps;
} RoadRouteData;

typedef struct {
    RoadRouteData *items;
    int count;
    int capacity;
} RoadRoutesData;

static char *duplicate_text(const char *text) {
    char *copy;
    size_t length;

    if (text == NULL) {
        return NULL;
    }

    length = strlen(text);
    copy = malloc(length + 1U);
    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, text, length + 1U);
    return copy;
}

static void clear_route(RoadRouteData *route) {
    free(route->color);
    free(route->label);
    free(route->steps);
    route->color = NULL;
    route->label = NULL;
    route->steps = NULL;
    route->step_count = 0;
}

static int ensure_capacity(RoadRoutesData *routes) {
    RoadRouteData *new_items;
    int new_capacity;

    if (routes->count < routes->capacity) {
        return 1;
    }

    new_capacity = routes->capacity == 0 ? 4 : routes->capacity * 2;
    new_items = realloc(routes->items, (size_t)new_capacity * sizeof(RoadRouteData));
    if (new_items == NULL) {
        return 0;
    }

    routes->items = new_items;
    routes->capacity = new_capacity;
    return 1;
}

RoadRoutes *road_routes_create(void) {
    return calloc(1, sizeof(RoadRoutesData));
}

void road_routes_destroy(RoadRoutes *routes) {
    RoadRoutesData *data = routes;
    int i;

    if (data == NULL) {
        return;
    }

    for (i = 0; i < data->count; i++) {
        clear_route(&data->items[i]);
    }
    free(data->items);
    free(data);
}

int road_routes_add(RoadRoutes *routes, const Route *route, const char *color, const char *label) {
    RoadRoutesData *data = routes;
    RoadRouteData item;
    int i;

    if (data == NULL || route == NULL || color == NULL || label == NULL || !route_found(route)) {
        return 0;
    }

    memset(&item, 0, sizeof(item));
    item.color = duplicate_text(color);
    item.label = duplicate_text(label);
    item.step_count = route_edge_count(route);
    if (item.color == NULL || item.label == NULL) {
        clear_route(&item);
        return 0;
    }

    if (item.step_count > 0) {
        item.steps = malloc((size_t)item.step_count * sizeof(RoadRouteStepData));
        if (item.steps == NULL) {
            clear_route(&item);
            return 0;
        }
    }

    for (i = 0; i < item.step_count; i++) {
        item.steps[i].from = route_edge_from(route, i);
        item.steps[i].to = route_edge_to(route, i);
        item.steps[i].edge_index = route_edge_index(route, i);
    }

    if (!ensure_capacity(data)) {
        clear_route(&item);
        return 0;
    }

    data->items[data->count] = item;
    data->count++;
    return 1;
}

int road_routes_count(const RoadRoutes *routes) {
    const RoadRoutesData *data = routes;

    return data == NULL ? 0 : data->count;
}

const char *road_routes_color(const RoadRoutes *routes, int index) {
    const RoadRoutesData *data = routes;

    if (data == NULL || index < 0 || index >= data->count) {
        return NULL;
    }

    return data->items[index].color;
}

const char *road_routes_label(const RoadRoutes *routes, int index) {
    const RoadRoutesData *data = routes;

    if (data == NULL || index < 0 || index >= data->count) {
        return NULL;
    }

    return data->items[index].label;
}

int road_routes_step_count(const RoadRoutes *routes, int index) {
    const RoadRoutesData *data = routes;

    if (data == NULL || index < 0 || index >= data->count) {
        return 0;
    }

    return data->items[index].step_count;
}

static const RoadRouteStepData *step_at(const RoadRoutes *routes, int route_index, int step_index) {
    const RoadRoutesData *data = routes;

    if (data == NULL || route_index < 0 || route_index >= data->count || step_index < 0 ||
        step_index >= data->items[route_index].step_count) {
        return NULL;
    }

    return &data->items[route_index].steps[step_index];
}

int road_routes_step_from(const RoadRoutes *routes, int route_index, int step_index) {
    const RoadRouteStepData *step = step_at(routes, route_index, step_index);

    return step == NULL ? -1 : step->from;
}

int road_routes_step_to(const RoadRoutes *routes, int route_index, int step_index) {
    const RoadRouteStepData *step = step_at(routes, route_index, step_index);

    return step == NULL ? -1 : step->to;
}

int road_routes_step_edge_index(const RoadRoutes *routes, int route_index, int step_index) {
    const RoadRouteStepData *step = step_at(routes, route_index, step_index);

    return step == NULL ? -1 : step->edge_index;
}

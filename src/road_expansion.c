#include "road_expansion.h"

#include <stdlib.h>

typedef struct {
    int from;
    int to;
    int edge_index;
    double length;
    double speed;
} CandidateData;

typedef struct {
    int from;
    int to;
    int edge_index;
} ExpansionEdgeData;

typedef struct {
    ExpansionEdgeData *items;
    int count;
    int capacity;
} RoadExpansionData;

typedef struct {
    int *parent;
    int *rank;
    int count;
} UnionFindData;

static int compare_candidates(const void *left, const void *right) {
    const CandidateData *a = left;
    const CandidateData *b = right;

    if (a->length < b->length) {
        return -1;
    }
    if (a->length > b->length) {
        return 1;
    }
    return 0;
}

static int ensure_capacity(RoadExpansionData *expansion) {
    ExpansionEdgeData *new_items;
    int new_capacity;

    if (expansion->count < expansion->capacity) {
        return 1;
    }

    new_capacity = expansion->capacity == 0 ? 4 : expansion->capacity * 2;
    new_items = realloc(expansion->items, (size_t)new_capacity * sizeof(ExpansionEdgeData));
    if (new_items == NULL) {
        return 0;
    }

    expansion->items = new_items;
    expansion->capacity = new_capacity;
    return 1;
}

static int append_expansion(RoadExpansionData *expansion, int from, int to, int edge_index) {
    if (!ensure_capacity(expansion)) {
        return 0;
    }

    expansion->items[expansion->count].from = from;
    expansion->items[expansion->count].to = to;
    expansion->items[expansion->count].edge_index = edge_index;
    expansion->count++;
    return 1;
}

static UnionFindData *union_find_create(int count) {
    UnionFindData *uf = calloc(1, sizeof(UnionFindData));
    int i;

    if (uf == NULL) {
        return NULL;
    }

    uf->parent = malloc((size_t)count * sizeof(int));
    uf->rank = calloc((size_t)count, sizeof(int));
    if (uf->parent == NULL || uf->rank == NULL) {
        free(uf->parent);
        free(uf->rank);
        free(uf);
        return NULL;
    }

    uf->count = count;
    for (i = 0; i < count; i++) {
        uf->parent[i] = i;
    }
    return uf;
}

static void union_find_destroy(UnionFindData *uf) {
    if (uf == NULL) {
        return;
    }

    free(uf->parent);
    free(uf->rank);
    free(uf);
}

static int union_find_root(UnionFindData *uf, int value) {
    if (uf->parent[value] != value) {
        uf->parent[value] = union_find_root(uf, uf->parent[value]);
    }
    return uf->parent[value];
}

static int union_find_join(UnionFindData *uf, int a, int b) {
    int root_a = union_find_root(uf, a);
    int root_b = union_find_root(uf, b);

    if (root_a == root_b) {
        return 0;
    }

    if (uf->rank[root_a] < uf->rank[root_b]) {
        uf->parent[root_a] = root_b;
    } else if (uf->rank[root_a] > uf->rank[root_b]) {
        uf->parent[root_b] = root_a;
    } else {
        uf->parent[root_b] = root_a;
        uf->rank[root_a]++;
    }

    return 1;
}

static CandidateData *collect_candidates(const Graph graph, int *candidate_count) {
    CandidateData *candidates;
    int count = 0;
    int vertex;

    *candidate_count = 0;
    candidates = malloc((size_t)graph_edge_count(graph) * sizeof(CandidateData));
    if (graph_edge_count(graph) > 0 && candidates == NULL) {
        return NULL;
    }

    for (vertex = 0; vertex < graph_vertex_count(graph); vertex++) {
        int edge_index;
        for (edge_index = 0; edge_index < graph_out_degree(graph, vertex); edge_index++) {
            if (!graph_edge_enabled(graph, vertex, edge_index)) {
                continue;
            }
            candidates[count].from = vertex;
            candidates[count].to = graph_edge_to(graph, vertex, edge_index);
            candidates[count].edge_index = edge_index;
            candidates[count].length = graph_edge_length(graph, vertex, edge_index);
            candidates[count].speed = graph_edge_speed(graph, vertex, edge_index);
            count++;
        }
    }

    *candidate_count = count;
    return candidates;
}

RoadExpansion road_expansion_apply(Graph graph, double speed_limit) {
    RoadExpansionData *expansion;
    CandidateData *candidates;
    UnionFindData *uf;
    int candidate_count;
    int i;

    if (graph == NULL || speed_limit < 0.0) {
        return NULL;
    }

    expansion = calloc(1, sizeof(RoadExpansionData));
    if (expansion == NULL) {
        return NULL;
    }

    if (graph_vertex_count(graph) == 0) {
        return expansion;
    }
    candidates = collect_candidates(graph, &candidate_count);
    if (candidates == NULL && graph_edge_count(graph) > 0) {
        road_expansion_destroy(expansion);
        return NULL;
    }

    uf = union_find_create(graph_vertex_count(graph));
    if (uf == NULL) {
        free(candidates);
        road_expansion_destroy(expansion);
        return NULL;
    }

    qsort(candidates, (size_t)candidate_count, sizeof(CandidateData), compare_candidates);
    for (i = 0; i < candidate_count; i++) {
        CandidateData *candidate = &candidates[i];

        if (candidate->to < 0 || !union_find_join(uf, candidate->from, candidate->to)) {
            continue;
        }

        if (candidate->speed < speed_limit) {
            if (!graph_set_edge_speed(graph, candidate->from, candidate->edge_index, candidate->speed * 1.5) ||
                !append_expansion(expansion, candidate->from, candidate->to, candidate->edge_index)) {
                free(candidates);
                union_find_destroy(uf);
                road_expansion_destroy(expansion);
                return NULL;
            }
        }
    }

    free(candidates);
    union_find_destroy(uf);
    return expansion;
}

void road_expansion_destroy(RoadExpansion expansion) {
    RoadExpansionData *data = expansion;

    if (data == NULL) {
        return;
    }

    free(data->items);
    free(data);
}

int road_expansion_count(const RoadExpansion expansion) {
    const RoadExpansionData *data = expansion;

    return data == NULL ? 0 : data->count;
}

int road_expansion_edge_from(const RoadExpansion expansion, int index) {
    const RoadExpansionData *data = expansion;

    if (data == NULL || index < 0 || index >= data->count) {
        return -1;
    }

    return data->items[index].from;
}

int road_expansion_edge_to(const RoadExpansion expansion, int index) {
    const RoadExpansionData *data = expansion;

    if (data == NULL || index < 0 || index >= data->count) {
        return -1;
    }

    return data->items[index].to;
}

int road_expansion_edge_index(const RoadExpansion expansion, int index) {
    const RoadExpansionData *data = expansion;

    if (data == NULL || index < 0 || index >= data->count) {
        return -1;
    }

    return data->items[index].edge_index;
}

#include "via.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    Graph *graph;
    char error[200];
} ViaData;

static void set_error(ViaData *via, const char *message) {
    snprintf(via->error, sizeof(via->error), "%s", message);
}

static int parse_vertex(ViaData *via, const char *line) {
    char id[64];
    double x;
    double y;

    if (sscanf(line, "v %63s %lf %lf", id, &x, &y) != 3) {
        set_error(via, "Vertice VIA malformado");
        return 0;
    }

    if (graph_add_vertex(via->graph, id, x, y) < 0) {
        set_error(via, "Nao foi possivel inserir vertice VIA");
        return 0;
    }

    return 1;
}

static int parse_edge(ViaData *via, const char *line) {
    char from_id[64];
    char to_id[64];
    char right_cep[64];
    char left_cep[64];
    char name[128];
    double length;
    double speed;

    if (sscanf(line,
               "e %63s %63s %63s %63s %lf %lf %127s",
               from_id,
               to_id,
               right_cep,
               left_cep,
               &length,
               &speed,
               name) != 7) {
        set_error(via, "Aresta VIA malformada");
        return 0;
    }

    if (!graph_add_edge(via->graph, from_id, to_id, right_cep, left_cep, length, speed, name)) {
        set_error(via, "Nao foi possivel inserir aresta VIA");
        return 0;
    }

    return 1;
}

static int parse_line(ViaData *via, const char *line) {
    char command[16];

    if (sscanf(line, "%15s", command) != 1) {
        return 1;
    }

    if (strcmp(command, "v") == 0) {
        return parse_vertex(via, line);
    }

    if (strcmp(command, "e") == 0) {
        return parse_edge(via, line);
    }

    return 1;
}

static void read_via_file(ViaData *via, const char *path) {
    FILE *file;
    char line[512];

    if (path == NULL) {
        set_error(via, "Caminho VIA nao informado");
        return;
    }

    file = fopen(path, "r");
    if (file == NULL) {
        set_error(via, "Nao foi possivel abrir arquivo VIA");
        return;
    }

    if (fgets(line, sizeof(line), file) == NULL) {
        set_error(via, "Arquivo VIA vazio");
        fclose(file);
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        if (!parse_line(via, line)) {
            break;
        }
    }

    fclose(file);
}

Via *via_load(const char *path) {
    ViaData *via = calloc(1, sizeof(ViaData));

    if (via == NULL) {
        return NULL;
    }

    via->graph = graph_create();
    if (via->graph == NULL) {
        free(via);
        return NULL;
    }

    read_via_file(via, path);
    return via;
}

void via_destroy(Via *via) {
    ViaData *data = via;

    if (data == NULL) {
        return;
    }

    graph_destroy(data->graph);
    free(data);
}

const Graph *via_graph(const Via *via) {
    const ViaData *data = via;

    return data == NULL ? NULL : data->graph;
}

const char *via_error(const Via *via) {
    const ViaData *data = via;

    if (data == NULL || data->error[0] == '\0') {
        return NULL;
    }

    return data->error;
}

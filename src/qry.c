#include "qry.h"
#include "route.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char last_error[200];

static void clear_error(void) {
    last_error[0] = '\0';
}

static void set_error(const char *message) {
    snprintf(last_error, sizeof(last_error), "%s", message);
}

static int parse_register(const char *text) {
    char *end;
    long value;

    if (text == NULL || text[0] != 'R') {
        return -1;
    }

    value = strtol(text + 1, &end, 10);
    if (*end != '\0' || value < 0 || value > 10) {
        return -1;
    }

    return (int)value;
}

static int process_address_query(const Geo geo, Registers registers, FILE *txt, const char *line) {
    Block block;
    char reg_text[16];
    char cep[64];
    char face_text[8];
    int reg;
    double number;
    double x;
    double y;

    if (sscanf(line, "@o? %15s %63s %7s %lf", reg_text, cep, face_text, &number) != 4) {
        set_error("Comando @o? malformado");
        return 0;
    }

    reg = parse_register(reg_text);
    if (reg < 0) {
        set_error("Registrador invalido em @o?");
        return 0;
    }

    block = geo_find_block(geo, cep);
    if (block == NULL) {
        set_error("CEP nao encontrado em @o?");
        return 0;
    }

    if (!block_address_point(block, face_text[0], number, &x, &y)) {
        set_error("Endereco invalido em @o?");
        return 0;
    }

    if (!registers_set(registers, reg, x, y)) {
        set_error("Nao foi possivel armazenar registrador");
        return 0;
    }

    fprintf(txt, "@o? R%d %s %c %.2f -> %.2f %.2f\n", reg, cep, face_text[0], number, x, y);
    return 1;
}

static int process_mvm(Graph graph, FILE *txt, const char *line) {
    double speed;
    double x;
    double y;
    double width;
    double height;
    int updated;

    if (sscanf(line, "mvm %lf %lf %lf %lf %lf", &speed, &x, &y, &width, &height) != 5) {
        set_error("Comando mvm malformado");
        return 0;
    }

    if (graph == NULL) {
        set_error("Comando mvm requer arquivo VIA");
        return 0;
    }

    updated = graph_update_speeds_in_rect(graph, speed, x, y, width, height);
    if (updated < 0) {
        set_error("Regiao invalida em mvm");
        return 0;
    }

    fprintf(txt, "mvm %.2f %.2f %.2f %.2f %.2f -> %d arestas atualizadas\n", speed, x, y, width, height, updated);
    return 1;
}

static int process_regs(Graph graph, RoadComponents *road_components, FILE *txt, const char *line) {
    RoadComponents new_components;
    double speed_limit;
    int count;

    if (sscanf(line, "regs %lf", &speed_limit) != 1) {
        set_error("Comando regs malformado");
        return 0;
    }

    if (graph == NULL) {
        set_error("Comando regs requer arquivo VIA");
        return 0;
    }

    new_components = road_components_find_slow(graph, speed_limit);
    if (new_components == NULL) {
        set_error("Nao foi possivel calcular componentes em regs");
        return 0;
    }

    count = road_components_count(new_components);
    fprintf(txt, "regs %.2f -> %d componentes conexos\n", speed_limit, count);

    if (road_components != NULL) {
        road_components_destroy(*road_components);
        *road_components = new_components;
    } else {
        road_components_destroy(new_components);
    }

    return 1;
}

static int process_exp(Graph graph, RoadExpansion *road_expansion, FILE *txt, const char *line) {
    RoadExpansion new_expansion;
    double speed_limit;
    int count;

    if (sscanf(line, "exp %lf", &speed_limit) != 1) {
        set_error("Comando exp malformado");
        return 0;
    }

    if (graph == NULL) {
        set_error("Comando exp requer arquivo VIA");
        return 0;
    }

    new_expansion = road_expansion_apply(graph, speed_limit);
    if (new_expansion == NULL) {
        set_error("Nao foi possivel calcular expansao em exp");
        return 0;
    }

    count = road_expansion_count(new_expansion);
    fprintf(txt, "exp %.2f -> %d arestas ampliadas\n", speed_limit, count);

    if (road_expansion != NULL) {
        road_expansion_destroy(*road_expansion);
        *road_expansion = new_expansion;
    } else {
        road_expansion_destroy(new_expansion);
    }

    return 1;
}

static int nearest_registered_vertex(const Graph graph, const Registers registers, int reg) {
    if (!registers_is_set(registers, reg)) {
        return -1;
    }

    return graph_nearest_vertex(graph, registers_x(registers, reg), registers_y(registers, reg));
}

static void write_route_street_summary(FILE *txt, const Graph graph, const Route route) {
    int edge_count = route_edge_count(route);
    int start = 0;

    if (edge_count == 0) {
        fprintf(txt, " | ruas: origem e destino coincidem");
        return;
    }

    fprintf(txt, " | ruas: ");
    while (start < edge_count) {
        const char *street = graph_edge_name(graph, route_edge_from(route, start), route_edge_index(route, start));
        double total_length = 0.0;
        int end = start;

        while (end < edge_count) {
            const char *current = graph_edge_name(graph, route_edge_from(route, end), route_edge_index(route, end));
            if (strcmp(street, current) != 0) {
                break;
            }

            total_length += graph_edge_length(graph, route_edge_from(route, end), route_edge_index(route, end));
            end++;
        }

        if (start > 0) {
            fprintf(txt, " ; ");
        }
        fprintf(txt,
                "%s ate %s (%.2f)",
                street,
                graph_vertex_id(graph, route_vertex_at(route, end)),
                total_length);
        start = end;
    }
}

static void write_route(FILE *txt, const Graph graph, const char *label, const Route route) {
    int i;

    fprintf(txt, "%s: ", label);
    if (!route_found(route)) {
        fprintf(txt, "Destino inacessivel\n");
        return;
    }

    for (i = 0; i < route_vertex_count(route); i++) {
        if (i > 0) {
            fprintf(txt, " -> ");
        }
        fprintf(txt, "%s", graph_vertex_id(graph, route_vertex_at(route, i)));
    }
    fprintf(txt, " | peso %.2f", route_total_weight(route));
    write_route_street_summary(txt, graph, route);
    fputc('\n', txt);
}

static int process_path_query(Graph graph, Registers registers, RoadRoutes road_routes, FILE *txt, const char *line) {
    char origin_text[16];
    char destination_text[16];
    char shortest_color[32];
    char fastest_color[32];
    int origin_reg;
    int destination_reg;
    int origin;
    int destination;
    Route shortest;
    Route fastest;

    if (sscanf(line, "p? %15s %15s %31s %31s", origin_text, destination_text, shortest_color, fastest_color) != 4) {
        set_error("Comando p? malformado");
        return 0;
    }

    if (graph == NULL) {
        set_error("Comando p? requer arquivo VIA");
        return 0;
    }

    origin_reg = parse_register(origin_text);
    destination_reg = parse_register(destination_text);
    if (origin_reg < 0 || destination_reg < 0 || !registers_is_set(registers, origin_reg) ||
        !registers_is_set(registers, destination_reg)) {
        set_error("Registrador invalido em p?");
        return 0;
    }

    origin = nearest_registered_vertex(graph, registers, origin_reg);
    destination = nearest_registered_vertex(graph, registers, destination_reg);
    if (origin < 0 || destination < 0) {
        set_error("Vertice viario nao encontrado em p?");
        return 0;
    }

    shortest = route_shortest_path(graph, origin, destination, ROUTE_METRIC_LENGTH);
    fastest = route_shortest_path(graph, origin, destination, ROUTE_METRIC_TIME);
    if (shortest == NULL || fastest == NULL) {
        route_destroy(shortest);
        route_destroy(fastest);
        set_error("Nao foi possivel calcular rota em p?");
        return 0;
    }

    fprintf(txt, "p? R%d R%d %s %s\n", origin_reg, destination_reg, shortest_color, fastest_color);
    write_route(txt, graph, "menor caminho", shortest);
    write_route(txt, graph, "caminho mais rapido", fastest);

    if ((route_found(shortest) && road_routes != NULL &&
         !road_routes_add(road_routes, shortest, shortest_color, "menor caminho")) ||
        (route_found(fastest) && road_routes != NULL &&
         !road_routes_add(road_routes, fastest, fastest_color, "caminho mais rapido"))) {
        route_destroy(shortest);
        route_destroy(fastest);
        set_error("Nao foi possivel registrar rota em p?");
        return 0;
    }

    route_destroy(shortest);
    route_destroy(fastest);
    return 1;
}

static int process_line(const Geo geo,
                        Graph graph,
                        Registers registers,
                        RoadComponents *road_components,
                        RoadExpansion *road_expansion,
                        RoadRoutes road_routes,
                        FILE *txt,
                        const char *line) {
    char command[16];

    if (sscanf(line, "%15s", command) != 1) {
        return 1;
    }

    if (strcmp(command, "@o?") == 0) {
        return process_address_query(geo, registers, txt, line);
    }

    if (strcmp(command, "mvm") == 0) {
        return process_mvm(graph, txt, line);
    }

    if (strcmp(command, "regs") == 0) {
        return process_regs(graph, road_components, txt, line);
    }

    if (strcmp(command, "exp") == 0) {
        return process_exp(graph, road_expansion, txt, line);
    }

    if (strcmp(command, "p?") == 0) {
        return process_path_query(graph, registers, road_routes, txt, line);
    }

    return 1;
}

int qry_process(const char *qry_path,
                const Geo geo,
                Graph graph,
                Registers registers,
                RoadComponents *road_components,
                RoadExpansion *road_expansion,
                RoadRoutes road_routes,
                const char *txt_path) {
    FILE *qry;
    FILE *txt;
    char line[512];

    clear_error();

    if (qry_path == NULL || geo == NULL || registers == NULL || txt_path == NULL) {
        set_error("Parametros insuficientes para processar QRY");
        return 0;
    }

    qry = fopen(qry_path, "r");
    if (qry == NULL) {
        set_error("Nao foi possivel abrir arquivo QRY");
        return 0;
    }

    txt = fopen(txt_path, "a");
    if (txt == NULL) {
        fclose(qry);
        set_error("Nao foi possivel abrir TXT para QRY");
        return 0;
    }

    while (fgets(line, sizeof(line), qry) != NULL) {
        if (!process_line(geo, graph, registers, road_components, road_expansion, road_routes, txt, line)) {
            fclose(txt);
            fclose(qry);
            return 0;
        }
    }

    fclose(txt);
    fclose(qry);
    return 1;
}

const char *qry_error(void) {
    return last_error[0] == '\0' ? NULL : last_error;
}

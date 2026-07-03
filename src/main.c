#include "args.h"
#include "file_paths.h"
#include "geo.h"
#include "output.h"
#include "qry.h"
#include "registers.h"
#include "road_components.h"
#include "road_expansion.h"
#include "via.h"

#include <stdio.h>

static void print_usage(FILE *stream) {
    fputs("Uso: ted [-e path] -f arq.geo [-q consulta.qry] [-v arqvias.via] -o dir\n", stream);
}

int main(int argc, char **argv) {
    AppArgs *args = args_create();
    FilePaths *paths;
    Geo *geo;
    Via *via = NULL;
    Registers *registers = NULL;
    RoadComponents *road_components = NULL;
    RoadExpansion *road_expansion = NULL;

    if (args == NULL) {
        fputs("Erro: memoria insuficiente\n", stderr);
        return 1;
    }

    if (!args_parse(args, argc, argv)) {
        fprintf(stderr, "Erro: %s\n", args_error(args));
        print_usage(stderr);
        args_destroy(args);
        return 1;
    }

    paths = file_paths_create(args_input_dir(args),
                              args_geo_file(args),
                              args_query_file(args),
                              args_via_file(args),
                              args_output_dir(args));
    if (paths == NULL) {
        fputs("Erro: memoria insuficiente\n", stderr);
        args_destroy(args);
        return 1;
    }

    if (file_paths_error(paths) != NULL) {
        fprintf(stderr, "Erro: %s\n", file_paths_error(paths));
        file_paths_destroy(paths);
        args_destroy(args);
        return 1;
    }

    geo = geo_load(file_paths_geo_path(paths));
    if (geo == NULL) {
        fputs("Erro: memoria insuficiente\n", stderr);
        file_paths_destroy(paths);
        args_destroy(args);
        return 1;
    }

    if (geo_error(geo) != NULL) {
        fprintf(stderr, "Erro: %s\n", geo_error(geo));
        geo_destroy(geo);
        file_paths_destroy(paths);
        args_destroy(args);
        return 1;
    }

    if (file_paths_via_path(paths) != NULL) {
        via = via_load(file_paths_via_path(paths));
        if (via == NULL) {
            fputs("Erro: memoria insuficiente\n", stderr);
            geo_destroy(geo);
            file_paths_destroy(paths);
            args_destroy(args);
            return 1;
        }

        if (via_error(via) != NULL) {
            fprintf(stderr, "Erro: %s\n", via_error(via));
            via_destroy(via);
            geo_destroy(geo);
            file_paths_destroy(paths);
            args_destroy(args);
            return 1;
        }
    }

    printf("GEO: %s\n", file_paths_geo_path(paths));
    printf("Quadras: %d\n", geo_block_count(geo));
    if (via != NULL) {
        printf("VIA: %s\n", file_paths_via_path(paths));
        printf("Vertices: %d\n", graph_vertex_count(via_graph(via)));
        printf("Arestas: %d\n", graph_edge_count(via_graph(via)));
    }
    printf("TXT: %s\n", file_paths_txt_path(paths));
    printf("SVG: %s\n", file_paths_svg_path(paths));

    if (!output_write_txt(file_paths_txt_path(paths), geo)) {
        fprintf(stderr, "Erro: %s\n", output_error());
        via_destroy(via);
        geo_destroy(geo);
        file_paths_destroy(paths);
        args_destroy(args);
        return 1;
    }

    if (file_paths_query_path(paths) != NULL) {
        registers = registers_create();
        if (registers == NULL) {
            fputs("Erro: memoria insuficiente\n", stderr);
            via_destroy(via);
            geo_destroy(geo);
            file_paths_destroy(paths);
            args_destroy(args);
            return 1;
        }

        if (!qry_process(file_paths_query_path(paths),
                         geo,
                         via == NULL ? NULL : via_graph(via),
                         registers,
                         &road_components,
                         &road_expansion,
                         file_paths_txt_path(paths))) {
            fprintf(stderr, "Erro: %s\n", qry_error());
            road_expansion_destroy(road_expansion);
            road_components_destroy(road_components);
            registers_destroy(registers);
            via_destroy(via);
            geo_destroy(geo);
            file_paths_destroy(paths);
            args_destroy(args);
            return 1;
        }
    }

    if (!output_write_svg_with_graph(file_paths_svg_path(paths),
                                     geo,
                                     via == NULL ? NULL : via_graph(via),
                                     registers,
                                     road_components,
                                     road_expansion)) {
        fprintf(stderr, "Erro: %s\n", output_error());
        road_expansion_destroy(road_expansion);
        road_components_destroy(road_components);
        registers_destroy(registers);
        via_destroy(via);
        geo_destroy(geo);
        file_paths_destroy(paths);
        args_destroy(args);
        return 1;
    }

    road_expansion_destroy(road_expansion);
    road_components_destroy(road_components);
    registers_destroy(registers);
    via_destroy(via);
    geo_destroy(geo);
    file_paths_destroy(paths);
    args_destroy(args);
    return 0;
}

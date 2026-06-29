#include "args.h"
#include "file_paths.h"

#include <stdio.h>

static void print_usage(FILE *stream) {
    fputs("Uso: ted [-e path] -f arq.geo [-q consulta.qry] [-v arqvias.via] -o dir\n", stream);
}

int main(int argc, char **argv) {
    AppArgs *args = args_create();
    FilePaths *paths;

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

    printf("GEO: %s\n", file_paths_geo_path(paths));
    printf("TXT: %s\n", file_paths_txt_path(paths));
    printf("SVG: %s\n", file_paths_svg_path(paths));

    file_paths_destroy(paths);
    args_destroy(args);
    return 0;
}

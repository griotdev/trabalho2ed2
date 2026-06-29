#include "args.h"

#include <stdio.h>

static void print_usage(FILE *stream) {
    fputs("Uso: ted [-e path] -f arq.geo [-q consulta.qry] [-v arqvias.via] -o dir\n", stream);
}

int main(int argc, char **argv) {
    AppArgs *args = args_create();

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

    puts("ted: argumentos validos; processamento ainda nao implementado");
    args_destroy(args);
    return 0;
}

#include "args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct AppArgs {
    char *input_dir;
    char *geo_file;
    char *query_file;
    char *via_file;
    char *output_dir;
    char error[160];
};

static char *copy_string(const char *text) {
    char *copy;
    size_t length;

    if (text == NULL) {
        return NULL;
    }

    length = strlen(text) + 1;
    copy = malloc(length);
    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, text, length);
    return copy;
}

static void free_values(AppArgs *args) {
    free(args->input_dir);
    free(args->geo_file);
    free(args->query_file);
    free(args->via_file);
    free(args->output_dir);

    args->input_dir = NULL;
    args->geo_file = NULL;
    args->query_file = NULL;
    args->via_file = NULL;
    args->output_dir = NULL;
}

static void set_error(AppArgs *args, const char *message) {
    snprintf(args->error, sizeof(args->error), "%s", message);
}

static int set_value(AppArgs *args, char **field, const char *option, const char *value) {
    char message[160];

    if (*field != NULL) {
        snprintf(message, sizeof(message), "Parametro repetido: %s", option);
        set_error(args, message);
        return 0;
    }

    *field = copy_string(value);
    if (*field == NULL) {
        set_error(args, "Memoria insuficiente ao ler parametros");
        return 0;
    }

    return 1;
}

static int read_option_value(AppArgs *args, int argc, char **argv, int *index, const char *option, char **field) {
    if (*index + 1 >= argc) {
        char message[160];
        snprintf(message, sizeof(message), "Parametro sem valor: %s", option);
        set_error(args, message);
        return 0;
    }

    (*index)++;
    return set_value(args, field, option, argv[*index]);
}

AppArgs *args_create(void) {
    AppArgs *args = calloc(1, sizeof(AppArgs));

    return args;
}

void args_destroy(AppArgs *args) {
    if (args == NULL) {
        return;
    }

    free_values(args);
    free(args);
}

int args_parse(AppArgs *args, int argc, char **argv) {
    int i;

    if (args == NULL) {
        return 0;
    }

    free_values(args);
    args->error[0] = '\0';

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0) {
            if (!read_option_value(args, argc, argv, &i, "-e", &args->input_dir)) {
                return 0;
            }
        } else if (strcmp(argv[i], "-f") == 0) {
            if (!read_option_value(args, argc, argv, &i, "-f", &args->geo_file)) {
                return 0;
            }
        } else if (strcmp(argv[i], "-q") == 0) {
            if (!read_option_value(args, argc, argv, &i, "-q", &args->query_file)) {
                return 0;
            }
        } else if (strcmp(argv[i], "-v") == 0) {
            if (!read_option_value(args, argc, argv, &i, "-v", &args->via_file)) {
                return 0;
            }
        } else if (strcmp(argv[i], "-o") == 0) {
            if (!read_option_value(args, argc, argv, &i, "-o", &args->output_dir)) {
                return 0;
            }
        } else {
            char message[160];
            snprintf(message, sizeof(message), "Parametro desconhecido: %s", argv[i]);
            set_error(args, message);
            return 0;
        }
    }

    if (args->geo_file == NULL) {
        set_error(args, "Parametro obrigatorio ausente: -f");
        return 0;
    }

    if (args->output_dir == NULL) {
        set_error(args, "Parametro obrigatorio ausente: -o");
        return 0;
    }

    return 1;
}

const char *args_input_dir(const AppArgs *args) {
    return args == NULL ? NULL : args->input_dir;
}

const char *args_geo_file(const AppArgs *args) {
    return args == NULL ? NULL : args->geo_file;
}

const char *args_query_file(const AppArgs *args) {
    return args == NULL ? NULL : args->query_file;
}

const char *args_via_file(const AppArgs *args) {
    return args == NULL ? NULL : args->via_file;
}

const char *args_output_dir(const AppArgs *args) {
    return args == NULL ? NULL : args->output_dir;
}

const char *args_error(const AppArgs *args) {
    if (args == NULL || args->error[0] == '\0') {
        return NULL;
    }

    return args->error;
}

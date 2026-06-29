#ifndef ARGS_H
#define ARGS_H

typedef void AppArgs;

AppArgs *args_create(void);
void args_destroy(AppArgs *args);

int args_parse(AppArgs *args, int argc, char **argv);

const char *args_input_dir(const AppArgs *args);
const char *args_geo_file(const AppArgs *args);
const char *args_query_file(const AppArgs *args);
const char *args_via_file(const AppArgs *args);
const char *args_output_dir(const AppArgs *args);
const char *args_error(const AppArgs *args);

#endif

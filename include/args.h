#ifndef ARGS_H
#define ARGS_H

/* Argumentos de linha de comando da aplicacao.
 * Tipo opaco: a implementacao fica restrita a args.c.
 */
typedef void *AppArgs;

/* Cria/destroi o acumulador de argumentos. */
AppArgs args_create(void);
void args_destroy(AppArgs args);

/* Processa argc/argv. Retorna 1 em sucesso e 0 em erro. */
int args_parse(AppArgs args, int argc, char **argv);

/* Acessores dos parametros reconhecidos. Retornam NULL quando ausentes. */
const char *args_input_dir(const AppArgs args);
const char *args_geo_file(const AppArgs args);
const char *args_query_file(const AppArgs args);
const char *args_via_file(const AppArgs args);
const char *args_output_dir(const AppArgs args);

/* Mensagem do ultimo erro de parsing, ou NULL quando nao ha erro. */
const char *args_error(const AppArgs args);

#endif

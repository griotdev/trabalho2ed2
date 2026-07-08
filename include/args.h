#ifndef ARGS_H
#define ARGS_H

/* Modulo de leitura dos argumentos da aplicacao.
 *
 * AppArgs e um tipo opaco. O chamador manipula apenas o ponteiro retornado
 * por args_create; todos os campos internos ficam restritos a args.c.
 */
typedef void *AppArgs;

/* Cria um acumulador vazio de argumentos.
 * Retorna NULL se nao houver memoria.
 */
AppArgs args_create(void);

/* Libera o acumulador e todas as strings armazenadas nele. Aceita NULL. */
void args_destroy(AppArgs args);

/* Processa argc/argv no formato:
 * ted [-e path] -f arq.geo [-q consulta.qry] [-v arqvias.via] -o dir
 * Retorna 1 em sucesso e 0 em erro; consulte args_error em caso de falha.
 */
int args_parse(AppArgs args, int argc, char **argv);

/* Acessores dos parametros reconhecidos.
 * Parametros opcionais nao informados retornam NULL.
 * As strings pertencem ao AppArgs e nao devem ser liberadas pelo chamador.
 */
const char *args_input_dir(const AppArgs args);
const char *args_geo_file(const AppArgs args);
const char *args_query_file(const AppArgs args);
const char *args_via_file(const AppArgs args);
const char *args_output_dir(const AppArgs args);

/* Retorna a mensagem do ultimo erro de parsing, ou NULL quando nao ha erro. */
const char *args_error(const AppArgs args);

#endif

#ifndef VIA_H
#define VIA_H

#include "graph.h"

/* Modulo de leitura do arquivo VIA.
 *
 * Via e um tipo opaco que possui o grafo viario dirigido lido do arquivo.
 */
typedef void *Via;

/* Carrega vertices e arestas de um .via.
 * Retorna NULL apenas em erro de memoria. Erros de abertura ou dados invalidos
 * ficam disponiveis por via_error.
 */
Via via_load(const char *path);

/* Libera o VIA e seu grafo interno. Aceita NULL. */
void via_destroy(Via via);

/* Retorna o grafo pertencente ao objeto Via.
 * O grafo nao deve ser destruido separadamente pelo chamador.
 */
Graph via_graph(Via via);

/* Mensagem de erro do carregamento, ou NULL em sucesso. */
const char *via_error(const Via via);

#endif

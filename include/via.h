#ifndef VIA_H
#define VIA_H

#include "graph.h"

/* Arquivo VIA carregado, contendo o grafo viario dirigido. */
typedef void *Via;

/* Carrega vertices e arestas de um .via. Use via_error para falhas de leitura. */
Via via_load(const char *path);
void via_destroy(Via via);

/* Retorna o grafo pertencente ao objeto Via; nao deve ser destruido separadamente. */
Graph via_graph(Via via);

/* Mensagem de erro do carregamento, ou NULL em sucesso. */
const char *via_error(const Via via);

#endif



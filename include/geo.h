#ifndef GEO_H
#define GEO_H

#include "block.h"

/* Colecao de quadras carregadas de um arquivo GEO.
 * Tipo opaco: as quadras e a capacidade interna ficam em geo.c.
 */
typedef void *Geo;

/* Carrega um arquivo .geo. Retorna objeto valido mesmo quando ha erro de leitura;
 * use geo_error para consultar a falha.
 */
Geo geo_load(const char *path);
void geo_destroy(Geo geo);

/* Consulta as quadras carregadas. Indices invalidos retornam NULL. */
int geo_block_count(const Geo geo);
Block geo_block_at(const Geo geo, int index);
Block geo_find_block(const Geo geo, const char *cep);

/* Mensagem de erro do carregamento, ou NULL quando o GEO foi lido sem falhas. */
const char *geo_error(const Geo geo);

#endif

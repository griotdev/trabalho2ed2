#ifndef GEO_H
#define GEO_H

#include "block.h"

/* Modulo de leitura do arquivo GEO.
 *
 * Geo e uma colecao opaca de quadras. O vetor interno, capacidade e estado
 * de erro ficam encapsulados em geo.c.
 */
typedef void *Geo;

/* Carrega um arquivo .geo.
 * Retorna NULL apenas em erro de memoria. Erros de abertura, sintaxe ou dados
 * sao armazenados no proprio objeto e devem ser consultados com geo_error.
 */
Geo geo_load(const char *path);

/* Libera o GEO e todas as quadras carregadas. Aceita NULL. */
void geo_destroy(Geo geo);

/* Retorna a quantidade de quadras carregadas. */
int geo_block_count(const Geo geo);

/* Retorna a quadra no indice informado, ou NULL se o indice for invalido. */
Block geo_block_at(const Geo geo, int index);

/* Busca uma quadra pelo CEP. Retorna NULL quando nao encontrada. */
Block geo_find_block(const Geo geo, const char *cep);

/* Mensagem de erro do carregamento, ou NULL quando o GEO foi lido sem falhas. */
const char *geo_error(const Geo geo);

#endif

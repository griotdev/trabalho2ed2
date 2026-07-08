#ifndef BLOCK_H
#define BLOCK_H

/* Modulo de quadras do arquivo GEO.
 *
 * Block e um tipo opaco. As coordenadas, dimensoes e atributos visuais ficam
 * encapsulados em block.c, preservando a regra de nao expor estruturas em .h.
 */
typedef void *Block;

/* Cria uma quadra identificada por cep.
 * x/y representam o ponto de ancoragem usado pelo projeto; width e height
 * devem ser nao negativos. Retorna NULL em erro de alocacao ou dimensoes
 * invalidas.
 */
Block block_create(const char *cep, double x, double y, double width, double height);

/* Libera a quadra. Aceita NULL. */
void block_destroy(Block block);

/* Define o estilo SVG da quadra. As cores sao copiadas internamente. */
void block_set_style(Block block, double stroke_width, const char *fill_color, const char *stroke_color);

/* Acessores dos dados geometricos, identificador e estilo.
 * Ponteiros retornados pertencem ao Block.
 */
const char *block_cep(const Block block);
double block_x(const Block block);
double block_y(const Block block);
double block_width(const Block block);
double block_height(const Block block);
double block_stroke_width(const Block block);
const char *block_fill_color(const Block block);
const char *block_stroke_color(const Block block);

/* Calcula o ponto de um endereco em uma face da quadra.
 * face aceita N, S, L ou O. number e a distancia ao longo da face.
 * Retorna 1 se o endereco e valido e grava x/y; retorna 0 caso contrario.
 */
int block_address_point(const Block block, char face, double number, double *x, double *y);

#endif

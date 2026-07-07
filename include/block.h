#ifndef BLOCK_H
#define BLOCK_H

/* Quadra lida de arquivo GEO.
 * Tipo opaco: nenhum campo interno e exposto no header.
 */
typedef void *Block;

/* Cria/destroi uma quadra. Retorna NULL em erro de alocacao ou dimensoes invalidas. */
Block block_create(const char *cep, double x, double y, double width, double height);
void block_destroy(Block block);

/* Define estilo usado na geracao SVG da quadra. */
void block_set_style(Block block, double stroke_width, const char *fill_color, const char *stroke_color);

/* Acessores dos dados geometricos, identificador e estilo. */
const char *block_cep(const Block block);
double block_x(const Block block);
double block_y(const Block block);
double block_width(const Block block);
double block_height(const Block block);
double block_stroke_width(const Block block);
const char *block_fill_color(const Block block);
const char *block_stroke_color(const Block block);

/* Calcula o ponto de um endereco na face N/S/L/O.
 * Retorna 1 se o endereco esta na face e grava x/y; retorna 0 caso contrario.
 */
int block_address_point(const Block block, char face, double number, double *x, double *y);

#endif

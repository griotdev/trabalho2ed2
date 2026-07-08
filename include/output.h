#ifndef OUTPUT_H
#define OUTPUT_H

#include "geo.h"
#include "graph.h"
#include "registers.h"
#include "road_components.h"
#include "road_expansion.h"
#include "road_routes.h"

/* Modulo de escrita das saidas TXT e SVG.
 * As funcoes retornam 1 em sucesso e 0 em erro; consulte output_error.
 */

/* Cria/limpa o arquivo TXT de saida.
 * As respostas textuais dos comandos QRY sao acrescentadas por qry_process.
 */
int output_write_txt(const char *path, const Geo geo);

/* Gera SVG somente com as quadras do GEO. */
int output_write_svg(const char *path, const Geo geo);

/* Gera SVG com quadras e marcadores de registradores. */
int output_write_svg_with_registers(const char *path, const Geo geo, const Registers registers);

/* Gera SVG completo.
 * A ordem de desenho preserva o mapa-base e sobrepoe componentes de regs,
 * expansoes de exp, percursos de p? e marcadores.
 */
int output_write_svg_with_graph(const char *path,
                                const Geo geo,
                                const Graph graph,
                                const Registers registers,
                                const RoadComponents road_components,
                                const RoadExpansion road_expansion,
                                const RoadRoutes road_routes);

/* Mensagem do ultimo erro de escrita, ou NULL quando nao ha erro. */
const char *output_error(void);

#endif

#ifndef FILE_PATHS_H
#define FILE_PATHS_H

/* Caminhos derivados das opcoes de entrada.
 * Tipo opaco: a montagem das strings fica encapsulada em file_paths.c.
 */
typedef void *FilePaths;

/* Monta caminhos absolutos/relativos para GEO, QRY, VIA, TXT e SVG. */
FilePaths file_paths_create(const char *input_dir,
                             const char *geo_file,
                             const char *query_file,
                             const char *via_file,
                             const char *output_dir);
void file_paths_destroy(FilePaths paths);

/* Acessores dos caminhos calculados. Caminhos opcionais podem retornar NULL. */
const char *file_paths_geo_path(const FilePaths paths);
const char *file_paths_query_path(const FilePaths paths);
const char *file_paths_via_path(const FilePaths paths);
const char *file_paths_txt_path(const FilePaths paths);
const char *file_paths_svg_path(const FilePaths paths);

/* Mensagem de erro de validacao/montagem de caminho, ou NULL em sucesso. */
const char *file_paths_error(const FilePaths paths);

#endif

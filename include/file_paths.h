#ifndef FILE_PATHS_H
#define FILE_PATHS_H

/* Modulo de montagem de caminhos de entrada e saida.
 *
 * FilePaths e opaco. Ele guarda as strings finais para GEO, QRY, VIA, TXT e
 * SVG. Quando ha QRY, as saidas usam o formato nomegeo-nomeqry.txt/.svg.
 * A concatenacao de diretorios relativos usa '/' para manter compatibilidade
 * com ambientes Linux.
 */
typedef void *FilePaths;

/* Monta os caminhos usados pela execucao.
 * input_dir e opcional. geo_file e output_dir sao obrigatorios.
 * query_file e via_file podem ser NULL quando a execucao nao usa QRY/VIA.
 * Retorna um objeto mesmo quando ha erro de validacao; consulte
 * file_paths_error antes de usar os caminhos.
 */
FilePaths file_paths_create(const char *input_dir,
                             const char *geo_file,
                             const char *query_file,
                             const char *via_file,
                             const char *output_dir);

/* Libera todos os caminhos armazenados. Aceita NULL. */
void file_paths_destroy(FilePaths paths);

/* Acessores dos caminhos calculados.
 * Caminhos opcionais podem retornar NULL. As strings pertencem ao FilePaths.
 */
const char *file_paths_geo_path(const FilePaths paths);
const char *file_paths_query_path(const FilePaths paths);
const char *file_paths_via_path(const FilePaths paths);
const char *file_paths_txt_path(const FilePaths paths);
const char *file_paths_svg_path(const FilePaths paths);

/* Mensagem de erro de validacao/montagem de caminho, ou NULL em sucesso. */
const char *file_paths_error(const FilePaths paths);

#endif

#include "file_paths.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *geo_path;
    char *query_path;
    char *via_path;
    char *txt_path;
    char *svg_path;
    char error[160];
} FilePathsData;

static char *copy_string(const char *text) {
    char *copy;
    size_t length;

    if (text == NULL) {
        return NULL;
    }

    length = strlen(text) + 1;
    copy = malloc(length);
    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, text, length);
    return copy;
}

static int is_absolute_path(const char *path) {
    if (path == NULL || path[0] == '\0') {
        return 0;
    }

    if (path[0] == '/' || path[0] == '\\') {
        return 1;
    }

    return path[0] != '\0' && path[1] == ':';
}

static int ends_with_separator(const char *path) {
    size_t length;

    if (path == NULL || path[0] == '\0') {
        return 0;
    }

    length = strlen(path);
    return path[length - 1] == '/' || path[length - 1] == '\\';
}

static char *join_path(const char *dir, const char *file) {
    char *path;
    size_t dir_length;
    size_t file_length;
    size_t separator_length;

    if (file == NULL) {
        return NULL;
    }

    if (dir == NULL || dir[0] == '\0' || is_absolute_path(file)) {
        return copy_string(file);
    }

    dir_length = strlen(dir);
    file_length = strlen(file);
    separator_length = ends_with_separator(dir) ? 0U : 1U;

    path = malloc(dir_length + separator_length + file_length + 1U);
    if (path == NULL) {
        return NULL;
    }

    memcpy(path, dir, dir_length);
    if (separator_length == 1U) {
        path[dir_length] = '/';
    }
    memcpy(path + dir_length + separator_length, file, file_length + 1U);

    return path;
}

static const char *last_path_component(const char *path) {
    const char *slash;
    const char *backslash;

    slash = strrchr(path, '/');
    backslash = strrchr(path, '\\');

    if (slash == NULL && backslash == NULL) {
        return path;
    }

    if (slash > backslash) {
        return slash + 1;
    }

    return backslash + 1;
}

static char *base_name_without_extension(const char *path) {
    const char *name;
    const char *dot;
    char *base;
    size_t length;

    name = last_path_component(path);
    dot = strrchr(name, '.');
    length = dot == NULL ? strlen(name) : (size_t)(dot - name);

    base = malloc(length + 1U);
    if (base == NULL) {
        return NULL;
    }

    memcpy(base, name, length);
    base[length] = '\0';
    return base;
}

static char *join_base_names(const char *geo_base_name, const char *query_base_name) {
    char *base_name;
    size_t geo_length;
    size_t query_length;

    if (query_base_name == NULL || query_base_name[0] == '\0') {
        return copy_string(geo_base_name);
    }

    geo_length = strlen(geo_base_name);
    query_length = strlen(query_base_name);
    base_name = malloc(geo_length + 1U + query_length + 1U);
    if (base_name == NULL) {
        return NULL;
    }

    memcpy(base_name, geo_base_name, geo_length);
    base_name[geo_length] = '-';
    memcpy(base_name + geo_length + 1U, query_base_name, query_length + 1U);
    return base_name;
}

static char *output_path(const char *output_dir, const char *base_name, const char *extension) {
    char *file_name;
    char *path;
    size_t base_length;
    size_t extension_length;

    base_length = strlen(base_name);
    extension_length = strlen(extension);

    file_name = malloc(base_length + extension_length + 1U);
    if (file_name == NULL) {
        return NULL;
    }

    memcpy(file_name, base_name, base_length);
    memcpy(file_name + base_length, extension, extension_length + 1U);

    path = join_path(output_dir, file_name);
    free(file_name);
    return path;
}

static void set_error(FilePathsData *paths, const char *message) {
    snprintf(paths->error, sizeof(paths->error), "%s", message);
}

static int build_paths(FilePathsData *paths,
                       const char *input_dir,
                       const char *geo_file,
                       const char *query_file,
                       const char *via_file,
                       const char *output_dir) {
    char *geo_base_name;
    char *query_base_name = NULL;
    char *output_base_name;

    if (geo_file == NULL || geo_file[0] == '\0') {
        set_error(paths, "Arquivo GEO nao informado");
        return 0;
    }

    if (output_dir == NULL || output_dir[0] == '\0') {
        set_error(paths, "Diretorio de saida nao informado");
        return 0;
    }

    paths->geo_path = join_path(input_dir, geo_file);
    paths->query_path = join_path(input_dir, query_file);
    paths->via_path = join_path(input_dir, via_file);
    geo_base_name = base_name_without_extension(geo_file);
    if (query_file != NULL && query_file[0] != '\0') {
        query_base_name = base_name_without_extension(query_file);
    }

    if (paths->geo_path == NULL ||
        geo_base_name == NULL ||
        (query_file != NULL && query_file[0] != '\0' && query_base_name == NULL)) {
        free(geo_base_name);
        free(query_base_name);
        set_error(paths, "Memoria insuficiente ao montar caminhos");
        return 0;
    }

    output_base_name = join_base_names(geo_base_name, query_base_name);
    free(geo_base_name);
    free(query_base_name);
    if (output_base_name == NULL) {
        set_error(paths, "Memoria insuficiente ao montar saidas");
        return 0;
    }

    paths->txt_path = output_path(output_dir, output_base_name, ".txt");
    paths->svg_path = output_path(output_dir, output_base_name, ".svg");
    free(output_base_name);

    if (paths->txt_path == NULL || paths->svg_path == NULL) {
        set_error(paths, "Memoria insuficiente ao montar saidas");
        return 0;
    }

    return 1;
}

FilePaths file_paths_create(const char *input_dir,
                             const char *geo_file,
                             const char *query_file,
                             const char *via_file,
                             const char *output_dir) {
    FilePathsData *paths = calloc(1, sizeof(FilePathsData));

    if (paths == NULL) {
        return NULL;
    }

    build_paths(paths, input_dir, geo_file, query_file, via_file, output_dir);
    return paths;
}

void file_paths_destroy(FilePaths paths) {
    FilePathsData *data = paths;

    if (data == NULL) {
        return;
    }

    free(data->geo_path);
    free(data->query_path);
    free(data->via_path);
    free(data->txt_path);
    free(data->svg_path);
    free(data);
}

const char *file_paths_geo_path(const FilePaths paths) {
    const FilePathsData *data = paths;

    return data == NULL ? NULL : data->geo_path;
}

const char *file_paths_query_path(const FilePaths paths) {
    const FilePathsData *data = paths;

    return data == NULL ? NULL : data->query_path;
}

const char *file_paths_via_path(const FilePaths paths) {
    const FilePathsData *data = paths;

    return data == NULL ? NULL : data->via_path;
}

const char *file_paths_txt_path(const FilePaths paths) {
    const FilePathsData *data = paths;

    return data == NULL ? NULL : data->txt_path;
}

const char *file_paths_svg_path(const FilePaths paths) {
    const FilePathsData *data = paths;

    return data == NULL ? NULL : data->svg_path;
}

const char *file_paths_error(const FilePaths paths) {
    const FilePathsData *data = paths;

    if (data == NULL || data->error[0] == '\0') {
        return NULL;
    }

    return data->error;
}

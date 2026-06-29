#ifndef FILE_PATHS_H
#define FILE_PATHS_H

typedef void FilePaths;

FilePaths *file_paths_create(void);
void file_paths_destroy(FilePaths *paths);

#endif

#ifndef ARGS_H
#define ARGS_H

typedef struct AppArgs AppArgs;

AppArgs *args_create(void);
void args_destroy(AppArgs *args);

#endif

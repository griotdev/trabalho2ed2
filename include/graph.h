#ifndef GRAPH_H
#define GRAPH_H

typedef struct Graph Graph;

Graph *graph_create(void);
void graph_destroy(Graph *graph);

#endif

#ifndef OUTPUT_H
#define OUTPUT_H

#include "geo.h"
#include "graph.h"
#include "registers.h"
#include "road_components.h"

int output_write_txt(const char *path, const Geo *geo);
int output_write_svg(const char *path, const Geo *geo);
int output_write_svg_with_registers(const char *path, const Geo *geo, const Registers *registers);
int output_write_svg_with_graph(const char *path,
                                const Geo *geo,
                                const Graph *graph,
                                const Registers *registers,
                                const RoadComponents *road_components);
const char *output_error(void);

#endif

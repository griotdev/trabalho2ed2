#ifndef OUTPUT_H
#define OUTPUT_H

#include "geo.h"
#include "registers.h"

int output_write_txt(const char *path, const Geo *geo);
int output_write_svg(const char *path, const Geo *geo);
int output_write_svg_with_registers(const char *path, const Geo *geo, const Registers *registers);
const char *output_error(void);

#endif

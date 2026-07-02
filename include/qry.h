#ifndef QRY_H
#define QRY_H

#include "geo.h"
#include "registers.h"

int qry_process(const char *qry_path, const Geo *geo, Registers *registers, const char *txt_path);
const char *qry_error(void);

#endif

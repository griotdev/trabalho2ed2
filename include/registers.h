#ifndef REGISTERS_H
#define REGISTERS_H

typedef void *Registers;

Registers registers_create(void);
void registers_destroy(Registers registers);

int registers_set(Registers registers, int index, double x, double y);
int registers_is_set(const Registers registers, int index);
double registers_x(const Registers registers, int index);
double registers_y(const Registers registers, int index);

#endif

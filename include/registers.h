#ifndef REGISTERS_H
#define REGISTERS_H

/* Registradores R0..R10 usados pelos comandos QRY.
 * Tipo opaco para esconder armazenamento interno.
 */
typedef void *Registers;

Registers registers_create(void);
void registers_destroy(Registers registers);

/* Armazena/coleta coordenadas. Indices invalidos retornam 0 ou coordenada 0.0. */
int registers_set(Registers registers, int index, double x, double y);
int registers_is_set(const Registers registers, int index);
double registers_x(const Registers registers, int index);
double registers_y(const Registers registers, int index);

#endif

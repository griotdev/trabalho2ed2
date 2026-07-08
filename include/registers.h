#ifndef REGISTERS_H
#define REGISTERS_H

/* Modulo dos registradores geograficos R0..R10.
 *
 * Registers e opaco e guarda, para cada registrador, se ha coordenada valida
 * e qual ponto foi associado pelos comandos @o?.
 */
typedef void *Registers;

/* Cria um conjunto vazio de registradores. Retorna NULL em erro de memoria. */
Registers registers_create(void);

/* Libera os registradores. Aceita NULL. */
void registers_destroy(Registers registers);

/* Armazena uma coordenada em R[index].
 * Indices validos vao de 0 a 10. Retorna 1 em sucesso.
 */
int registers_set(Registers registers, int index, double x, double y);

/* Retorna 1 quando R[index] possui coordenada definida. */
int registers_is_set(const Registers registers, int index);

/* Acessores da coordenada armazenada; retornam 0.0 para indice invalido. */
double registers_x(const Registers registers, int index);
double registers_y(const Registers registers, int index);

#endif

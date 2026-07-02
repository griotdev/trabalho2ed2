#include "registers.h"

#include <stdlib.h>

#define REGISTER_COUNT 11

typedef struct {
    int set[REGISTER_COUNT];
    double x[REGISTER_COUNT];
    double y[REGISTER_COUNT];
} RegistersData;

static int valid_index(int index) {
    return index >= 0 && index < REGISTER_COUNT;
}

Registers *registers_create(void) {
    RegistersData *registers = calloc(1, sizeof(RegistersData));

    return registers;
}

void registers_destroy(Registers *registers) {
    free(registers);
}

int registers_set(Registers *registers, int index, double x, double y) {
    RegistersData *data = registers;

    if (data == NULL || !valid_index(index)) {
        return 0;
    }

    data->set[index] = 1;
    data->x[index] = x;
    data->y[index] = y;
    return 1;
}

int registers_is_set(const Registers *registers, int index) {
    const RegistersData *data = registers;

    if (data == NULL || !valid_index(index)) {
        return 0;
    }

    return data->set[index];
}

double registers_x(const Registers *registers, int index) {
    const RegistersData *data = registers;

    if (data == NULL || !valid_index(index) || !data->set[index]) {
        return 0.0;
    }

    return data->x[index];
}

double registers_y(const Registers *registers, int index) {
    const RegistersData *data = registers;

    if (data == NULL || !valid_index(index) || !data->set[index]) {
        return 0.0;
    }

    return data->y[index];
}

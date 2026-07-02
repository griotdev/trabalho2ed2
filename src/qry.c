#include "qry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char last_error[200];

static void clear_error(void) {
    last_error[0] = '\0';
}

static void set_error(const char *message) {
    snprintf(last_error, sizeof(last_error), "%s", message);
}

static int parse_register(const char *text) {
    char *end;
    long value;

    if (text == NULL || text[0] != 'R') {
        return -1;
    }

    value = strtol(text + 1, &end, 10);
    if (*end != '\0' || value < 0 || value > 10) {
        return -1;
    }

    return (int)value;
}

static int process_address_query(const Geo *geo, Registers *registers, FILE *txt, const char *line) {
    const Block *block;
    char reg_text[16];
    char cep[64];
    char face_text[8];
    int reg;
    double number;
    double x;
    double y;

    if (sscanf(line, "@o? %15s %63s %7s %lf", reg_text, cep, face_text, &number) != 4) {
        set_error("Comando @o? malformado");
        return 0;
    }

    reg = parse_register(reg_text);
    if (reg < 0) {
        set_error("Registrador invalido em @o?");
        return 0;
    }

    block = geo_find_block(geo, cep);
    if (block == NULL) {
        set_error("CEP nao encontrado em @o?");
        return 0;
    }

    if (!block_address_point(block, face_text[0], number, &x, &y)) {
        set_error("Endereco invalido em @o?");
        return 0;
    }

    if (!registers_set(registers, reg, x, y)) {
        set_error("Nao foi possivel armazenar registrador");
        return 0;
    }

    fprintf(txt, "@o? R%d %s %c %.2f -> %.2f %.2f\n", reg, cep, face_text[0], number, x, y);
    return 1;
}

static int process_line(const Geo *geo, Registers *registers, FILE *txt, const char *line) {
    char command[16];

    if (sscanf(line, "%15s", command) != 1) {
        return 1;
    }

    if (strcmp(command, "@o?") == 0) {
        return process_address_query(geo, registers, txt, line);
    }

    return 1;
}

int qry_process(const char *qry_path, const Geo *geo, Registers *registers, const char *txt_path) {
    FILE *qry;
    FILE *txt;
    char line[512];

    clear_error();

    if (qry_path == NULL || geo == NULL || registers == NULL || txt_path == NULL) {
        set_error("Parametros insuficientes para processar QRY");
        return 0;
    }

    qry = fopen(qry_path, "r");
    if (qry == NULL) {
        set_error("Nao foi possivel abrir arquivo QRY");
        return 0;
    }

    txt = fopen(txt_path, "a");
    if (txt == NULL) {
        fclose(qry);
        set_error("Nao foi possivel abrir TXT para QRY");
        return 0;
    }

    while (fgets(line, sizeof(line), qry) != NULL) {
        if (!process_line(geo, registers, txt, line)) {
            fclose(txt);
            fclose(qry);
            return 0;
        }
    }

    fclose(txt);
    fclose(qry);
    return 1;
}

const char *qry_error(void) {
    return last_error[0] == '\0' ? NULL : last_error;
}

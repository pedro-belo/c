#ifndef __H_SYMBOL__

#define __H_SYMBOL__

#define FREQ_SZ  256

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "./defs.h"
#include "./list.h"
#include "./tree.h"

struct SymbolConfig {
    BOOL show_code;
    BOOL show_fabs;
    BOOL show_frel;
};

struct Symbol{

    struct SymbolConfig config;

    // Frequência absoluta
    uint_t freq_abs[FREQ_SZ];

    // Frequência relativa
    float freq_rel[FREQ_SZ];

    // Quantos bits exitem em cada simbolo
    uint_t total_bits[FREQ_SZ];

    // Tabela de mapeamento bit por símbolo
    uint_t table_code[FREQ_SZ][sizeof(uint_t) * 8];

    // Somatório da frêquencia absoluta
    uint_t fa_sum;

    // Quantidade de simbolos
    uint_t count;

};
typedef struct Symbol Symbol;

void create_symbol(BYTE *buffer, uint_t len);
void set_freq_abs(BYTE symb, uint_t fab);
Symbol *symbol_instance();
void symbol_destroy();
THuffman *grow();
void print_fa();
void print_fr();
void print_far();

void print_short_table_code(THuffman *th);
void print_full_table_code();
void create_table_code(THuffman *th);
#endif

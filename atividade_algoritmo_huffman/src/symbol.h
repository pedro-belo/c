#ifndef __H_SYMBOL__

#define __H_SYMBOL__

#define FREQ_SZ  256

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "./defs.h"
#include "./list.h"
#include "./tree.h"

struct Symbol{

    // Frequência absoluta
    uint32_t freq_abs[FREQ_SZ];

    // Frequência relativa
    float freq_rel[FREQ_SZ];

    // Quantos bits exitem em cada simbolo
    uint32_t total_bits[FREQ_SZ];

    // Tabela de mapeamento bit por símbolo
    uint32_t table_code[FREQ_SZ][sizeof(uint32_t) * 8];

    // Somatório da frêquencia absoluta
    uint32_t fa_sum;

    // Quantidade de simbolos
    uint32_t count;

    void (*destroy)(void);
};
typedef struct Symbol Symbol;

Symbol *symbol();
void symbol_init();
BOOL has_symbol(BYTE symb);
void symbol_inc(BYTE symb);
void update_freq();
void load_symbol(FILE *handle, uint32_t length);
void set_freq_abs(BYTE symb, uint32_t fab);
void print_symbol(BOOL symb, BOOL freq_abs, BOOL freq_rel, BOOL ascii_code, BOOL huffman_code);
THuffman *grow();
void create_table_code(THuffman *th);
#endif

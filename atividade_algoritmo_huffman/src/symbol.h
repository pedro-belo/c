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

};
typedef struct Symbol Symbol;

void create_symbol(BYTE *buffer, uint32_t len);
void set_freq_abs(BYTE symb, uint32_t fab);
Symbol *symbol_instance();
void symbol_destroy();
THuffman *grow();
void print_symbol(BOOL symb, BOOL freq_abs, BOOL freq_rel, BOOL ascii_code, BOOL huffman_code);
void print_short_table_code(THuffman *th);
void print_full_table_code();
void create_table_code(THuffman *th);
#endif

#include "./symbol.h"

static void update_symbol_count();
static void reset_symbol();
static void update_fa_sum();
static void update_freq_rel();
static void destroy();

static Symbol *_symbol;

void symbol_init() {

    _symbol = (Symbol *)malloc(sizeof(Symbol));

    if(!_symbol) {
        fprintf(stderr, "Memória insuficiente.\n");
        exit(R_ERROR);
    }
    
    _symbol->destroy = &destroy;

    reset_symbol();
}


Symbol *symbol() {

    if(!_symbol) {
        fprintf(stderr, "Symbol não foi inicializado\n");
        exit(R_ERROR);
    }

    return _symbol;
}

BOOL has_symbol(BYTE symb) {
    return symbol()->freq_abs[symb] > 0;
}

void symbol_inc(BYTE symb) {
    symbol()->freq_abs[symb] += 1;
}

void update_freq() {
    update_fa_sum();
    update_symbol_count();
    update_freq_rel();
}

void print_symbol(BOOL symb, BOOL freq_abs, BOOL freq_rel, BOOL ascii_code, BOOL huffman_code) {   

    fprintf(stdout, "|");
    if(symb) fprintf(stdout, "---------");
    if(freq_abs) fprintf(stdout, "----------------");
    if(freq_rel) fprintf(stdout, "-----------");
    if(ascii_code) fprintf(stdout, "-------------");
    if(huffman_code) fprintf(stdout, "-------------");
    fprintf(stdout, "\n");

    fprintf(stdout, "|");
    if(symb) fprintf(stdout, " SYMBOL |");
    if(freq_abs) fprintf(stdout, "   FREQ. ABS  |");
    if(freq_rel) fprintf(stdout, " FREQ REL  |");
    if(ascii_code) fprintf(stdout, " ASCII CODE |");
    if(huffman_code) fprintf(stdout, " HUFFMAN CODE");
    fprintf(stdout, "\n");

    for (size_t index = 0; index < FREQ_SZ ; index++) {

        if(!has_symbol(index))
            continue;

        fprintf(stdout, "|");
        if(symb) fprintf(stdout, "  %#4lx  |", index);
        if(freq_abs) fprintf(stdout, " %12d |", symbol()->freq_abs[index]);
        if(freq_rel) fprintf(stdout, "    %4.3f  |", symbol()->freq_rel[index]);
        
        if(ascii_code){

            if(index >= 32 && index <= 126)
                fprintf(stdout, "    '%c'     | ", (char)index);
            else
                switch (index) {
                case 0:
                    fprintf(stdout, "    \\0      | ");
                    break;
                
                case 9:
                    fprintf(stdout, "    \\t      | ");
                    break;
                
                case 10:
                    fprintf(stdout, "    \\n      | ");
                    break;

                case 13:
                    fprintf(stdout, "    \\b      | ");
                    break;

                    default: fprintf(stdout, "     .      | ");
                }

        }
                

        if(huffman_code){
            for (size_t bit = 0; bit < symbol()->total_bits[index] ; bit++)
                printf("%d", symbol()->table_code[index][bit]);            
        } fprintf(stdout, "\n");

    }
    fprintf(stdout, "|");
    if(symb) fprintf(stdout, "---------");
    if(freq_abs) fprintf(stdout, "----------------");
    if(freq_rel) fprintf(stdout, "-----------");
    if(ascii_code) fprintf(stdout, "-------------");
    if(huffman_code) fprintf(stdout, "-------------");
    fprintf(stdout, "\n");

}

THuffman *grow() {

    THuffman *th = NULL;
    TNode *node = NULL;
    List *list = new_list();

    for (size_t index = 0; index < FREQ_SZ ; index++){

        if(!has_symbol(index))
            continue;

        node = new_node(symbol()->freq_abs[index]);
        node->label = index;
        add(list, node);
    }

    th = build(list);

    free(list);
    return th;
}

void create_table_code(THuffman *th){

    TNode *node = NULL;

    for (size_t index = 0; index < FREQ_SZ ; index++ ) {

        if(!has_symbol(index))
            continue;

        int bits = 0;
        node = get_leaf(th, index);

        BOOL leaf_order[sizeof(uint32_t) * 8] = {0};

        for (size_t pos = 0; node != th->root ; pos++ ) {
            leaf_order[pos] = node->edge;
            node = node->father;
            bits += 1;  
        }

        symbol()->total_bits[index] += bits;

        for (size_t bit = bits, pos = 0; bit > 0 ; bit--, pos++ )
            symbol()->table_code[index][pos] = leaf_order[bit - 1];

    }

}


void load_symbol(FILE *handle, uint32_t length) {

    BYTE *buffer = (BYTE *)malloc(sizeof(BYTE) * BLOCK_SIZE);
    if(!buffer) {
        fprintf(stderr, "Memória insuficiente.\n");
        fclose(handle);
        exit(R_ERROR);
    }

    while (length){

        uint32_t bytes_read = (uint32_t)fread(
            buffer, sizeof(BYTE), BLOCK_SIZE, handle);

        for (uint32_t index = 0; index < bytes_read ; index++ )
            symbol_inc(buffer[index]);

        length -= bytes_read;
    }free(buffer);

    update_freq();
}

void set_freq_abs(BYTE symb, uint32_t fab) {

    if(!has_symbol(symb))
        symbol()->count += 1;

    symbol()->fa_sum -= symbol()->freq_abs[symb];
    symbol()->fa_sum += fab;

    symbol()->freq_abs[symb] = fab;
    update_freq();
}

// static functions
static void update_symbol_count() {

    symbol()->count = 0;

    for (size_t index = 0; index < FREQ_SZ ; index++ ){

        if(!has_symbol(index))
            continue;

        symbol()->count += 1;
    }

}

static void reset_symbol() {

    for (size_t index = 0; index < FREQ_SZ ; index++ ) {

        symbol()->freq_abs[index] = 0;
        symbol()->freq_rel[index] = 0;
        symbol()->total_bits[index] = 0;

        for (size_t bit = 0 ; bit < sizeof(uint32_t) * 8 ; bit++)
            symbol()->table_code[index][bit] = 0;
    }

    symbol()->fa_sum = 0;
    symbol()->count = 0;

}

static void update_fa_sum() {

    symbol()->fa_sum = 0;

    for (size_t index = 0; index < FREQ_SZ ; index++ ){

        if(!has_symbol(index))
            continue;

        symbol()->fa_sum += symbol()->freq_abs[index];
    }

}

static void update_freq_rel() {

    if(symbol()->fa_sum == 0)
        return;

    for (size_t index = 0; index < FREQ_SZ ; index++ ){

        if(!has_symbol(index))
            continue;

        symbol()->freq_rel[index] = (float)symbol()->freq_abs[index] / symbol()->fa_sum;
    }

}

static void destroy() {
    free(_symbol);
    _symbol = NULL;
}
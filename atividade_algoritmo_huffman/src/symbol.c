#include "./symbol.h"

// Prototypes
static void load_freq(BYTE *buffer, size_t len);
static void reset_symbol();
static void _load_freq_abs(BYTE *buffer, size_t len);
static void _load_freq_rel();
static void update_symbol_count();

const BOOL ascii_ext = True;

static Symbol *symbol;

Symbol *symbol_instance() {

    if(!symbol) {
        fprintf(stderr, "Incorret Inicialization!\n");
        exit(R_ERROR);
    }

    return symbol;
}

void create_symbol(BYTE *buffer, uint32_t len) {

    symbol = (Symbol *)malloc(sizeof(Symbol));

    if(!symbol) {
        fprintf(stderr, "Inicializacao falhou.\n");
        exit(R_ERROR);
    }

    reset_symbol();

    if(buffer)
        load_freq(buffer, len);
}

void symbol_destroy() {
    free(symbol);
    symbol = NULL;
}

BOOL has_symbol(BYTE symb) {
    return symbol_instance()->freq_abs[symb] > 0;
}

static void load_freq(BYTE *buffer, size_t len) {
    _load_freq_abs(buffer, len);
    _load_freq_rel();
}

void set_freq_abs(BYTE symb, uint32_t fab) {

    if(!has_symbol(symb))
        symbol_instance()->count += 1;

    symbol_instance()->fa_sum -= symbol_instance()->freq_abs[symb];
    symbol_instance()->fa_sum += fab;

    symbol_instance()->freq_abs[symb] = fab;
    _load_freq_rel();
}

THuffman *grow() {

    THuffman *th = NULL;
    TNode *node = NULL;
    List *list = new_list();

    for (size_t index = 0; index < FREQ_SZ ; index++){

        if(symbol_instance()->freq_abs[index] == 0)
            continue;

        node = new_node(symbol_instance()->freq_abs[index]);
        node->label = index;
        add(list, node);
    }

    th = build(list);

    free(list);
    return th;
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

        if(symbol_instance()->freq_abs[index] == 0)
            continue;

        fprintf(stdout, "|");
        if(symb) fprintf(stdout, "  %#4lx  |", index);
        if(freq_abs) fprintf(stdout, " %12d |", symbol_instance()->freq_abs[index]);
        if(freq_rel) fprintf(stdout, "    %4.3f  |", symbol_instance()->freq_rel[index]);
        
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
            for (size_t bit = 0; bit < symbol_instance()->total_bits[index] ; bit++)
                printf("%d", symbol_instance()->table_code[index][bit]);            
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

void create_table_code(THuffman *th){

    TNode *node = NULL;

    for (size_t index = 0; index < FREQ_SZ ; index++ ) {

        if(symbol_instance()->freq_abs[index] == 0)
            continue;

        int bits = 0;
        node = get_leaf(th, index);

        BOOL leaf_order[sizeof(uint32_t) * 8] = {0};

        for (size_t pos = 0; node != th->root ; pos++ ) {
            leaf_order[pos] = node->edge;
            node = node->father;
            bits += 1;  
        }

        symbol_instance()->total_bits[index] += bits;

        for (size_t bit = bits, pos = 0; bit > 0 ; bit--, pos++ )
            symbol_instance()->table_code[index][pos] = leaf_order[bit - 1];

    }

}

// Static Functions

static void reset_symbol() {

    for (size_t index = 0; index < FREQ_SZ ; index++ ) {

        symbol_instance()->freq_abs[index] = 0;
        symbol_instance()->freq_rel[index] = 0;
        symbol_instance()->total_bits[index] = 0;

        for (size_t bit = 0 ; bit < sizeof(uint32_t) * 8 ; bit++)
            symbol_instance()->table_code[index][bit] = 0;
    }

    symbol_instance()->fa_sum = 0;
    symbol_instance()->count = 0;

}

static void _load_freq_abs(BYTE *buffer, size_t len) {

    symbol_instance()->fa_sum = 0;

    for (size_t index = 0; index < len; index++ ){

        if( !ascii_ext && buffer[index] > 127){
            fprintf(stdout,"Há caracteres nao permitidos.\n");
            exit(R_ERROR);
        }

        symbol_instance()->freq_abs[buffer[index]] += 1;
        symbol_instance()->fa_sum += 1;
    }

}

static void _load_freq_rel() {

    if(symbol_instance()->fa_sum == 0)
        return;

    for (size_t index = 0; index < FREQ_SZ ; index++ ){

        if(symbol_instance()->freq_abs[index] == 0)
            continue;

        symbol_instance()->freq_rel[index] = (float)symbol_instance()->freq_abs[index] / symbol_instance()->fa_sum;
    }

    update_symbol_count();
}

static void update_symbol_count() {

    symbol_instance()->count = 0;

    for (size_t index = 0; index < FREQ_SZ ; index++ ){

        if(symbol_instance()->freq_abs[index] == 0)
            continue;

        symbol_instance()->count += 1;
    }

}

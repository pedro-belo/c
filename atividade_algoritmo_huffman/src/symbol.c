#include "./symbol.h"

// Prototypes
static void load_freq(BYTE *buffer, size_t len);
static void _print_full_table_code();
static void _print_short_table_code(THuffman *th, char fmt);
static void print_symbol();
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

void create_symbol(BYTE *buffer, uint_t len) {

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

void set_freq_abs(BYTE symb, uint_t fab) {

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

void print_fa(){
    symbol_instance()->config.show_fabs = True;
    symbol_instance()->config.show_frel = False;
    print_symbol();
}

void print_far(){
    symbol_instance()->config.show_fabs = True;
    symbol_instance()->config.show_frel = True;
    print_symbol();
}

void print_fr(){
    symbol_instance()->config.show_fabs = False;
    symbol_instance()->config.show_frel = True;
    print_symbol();
}

void print_short_table_code(THuffman *th){
    _print_short_table_code(th, 'x');
}

void print_full_table_code() {
    _print_full_table_code();
}

void create_table_code(THuffman *th){

    TNode *node = NULL;

    for (size_t index = 0; index < FREQ_SZ ; index++ ) {

        if(symbol_instance()->freq_abs[index] == 0)
            continue;

        int bits = 0;
        node = get_leaf(th, index);

        BOOL leaf_order[sizeof(uint_t) * 8] = {0};
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
static void _print_full_table_code() {

    fprintf(stdout, "!-------------[TABLE CODE - F]-------------!\n");
    for (size_t index = 0; index < FREQ_SZ ; index++ ) {
        
        if(symbol_instance()->freq_abs[index] == 0)
            continue;

        fprintf(stdout, "CODE[%3.ld]=", index);
        for (size_t pos = 0; pos < sizeof(uint_t) * 8 ; pos++ ) {
            fprintf(stdout, "%d", symbol_instance()->table_code[index][pos]);
        }
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "!-------------------------------------------!\n\n");
}

static void _print_short_table_code(THuffman *th, char fmt) {

    TNode *node = NULL;

    fprintf(stdout, "!-------------[TABLE CODE - S]-------------!\n");
    for (size_t index = 0; index < FREQ_SZ ; index++ ) {

        if(symbol_instance()->freq_abs[index] == 0)
            continue;

        switch (fmt) {
            case 'x': fprintf(stdout, "CODE[%#lx]=", index);
                break;
            
            case 'i': fprintf(stdout, "CODE[%ld]=", index);
                break;

            case 's': default:
                fprintf(stdout, "CODE[%c]=", (BYTE)index);
                break;
        }

        node = get_leaf(th, index);

        for (size_t bit = 0; node != th->root ; bit++ ) {
            fprintf(stdout, "%d", node->edge);
            node = node->father;
        }
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "!-------------------------------------------!\n\n");
}

static void print_symbol() {

    fprintf(stdout, "\n#--------------[FREQUENCE TABLE]---------------#\n");

    for (size_t index = 0;  index < FREQ_SZ ;  index++ ){

        if(symbol_instance()->freq_abs[index] == 0)
            continue;        

        fprintf(stdout, "CODE: %#lx\n", index);

        switch(index){

            case '\n':
                if(symbol_instance()->config.show_fabs) fprintf(stdout, "FA(\\n) = %d\n", symbol_instance()->freq_abs[index]);
                if(symbol_instance()->config.show_frel) fprintf(stdout, "FR(\\n) = %.6f (%.6f %%)\n", symbol_instance()->freq_rel[index], symbol_instance()->freq_rel[index]*100);
                break;

            case '\t':
                if(symbol_instance()->config.show_fabs) fprintf(stdout, "FA(\\n) = %d\n", symbol_instance()->freq_abs[index]);
                if(symbol_instance()->config.show_frel) fprintf(stdout, "FR(\\t) = %.6f (%.6f %%)\n", symbol_instance()->freq_rel[index], symbol_instance()->freq_rel[index]*100);
                break;

            default:
                if( index >= 0x20 && index <= 0x7E ){
                    if(symbol_instance()->config.show_fabs) fprintf(stdout, "FA(%c) = %d\n", (char)index , symbol_instance()->freq_abs[index]);
                    if(symbol_instance()->config.show_frel) fprintf(stdout, "FR(%c) = %.6f (%.6f %%)\n", (char)index , symbol_instance()->freq_rel[index], symbol_instance()->freq_rel[index]*100);
                }
                else{
                    if(symbol_instance()->config.show_fabs) fprintf(stdout, "FA{%ld} = %d\n", index, symbol_instance()->freq_abs[index]);
                    if(symbol_instance()->config.show_frel) fprintf(stdout, "FR{%ld} = %.6f (%.6f %%)\n", index, symbol_instance()->freq_rel[index], symbol_instance()->freq_rel[index] * 100);
                }
        }

        if(symbol_instance()->config.show_fabs && symbol_instance()->config.show_frel)
            fprintf(stdout, "\n");

    }
    fprintf(stdout, "----------------------------------------------#\n");
}

static void reset_symbol() {

    for (size_t index = 0; index < FREQ_SZ ; index++ ) {

        symbol_instance()->freq_abs[index] = 0;
        symbol_instance()->freq_rel[index] = 0;
        symbol_instance()->total_bits[index] = 0;

        for (size_t bit = 0 ; bit < sizeof(uint_t) * 8 ; bit++)
            symbol_instance()->table_code[index][bit] = 0;
    }

    symbol_instance()->config.show_code = False;
    symbol_instance()->config.show_fabs = True;
    symbol_instance()->config.show_frel = True;

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

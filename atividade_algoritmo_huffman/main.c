#include "./main.h"


int main(int argc, char **argv) {

    initialize(argc, argv);

    switch(get_option()) {

        case SYMBOLS_LIST:
            app()->run_symbol_list();
            break;

        case COMPRESS:
            app()->run_compress();
            break;

        case DECOMPRESS:
            app()->run_decompress();
            break;

        case HELP:
            app()->run_help();
            break;

        default:
            fprintf(stderr, "Comando nao reconhecido.\n");

    }

    return (R_SUCCESS);
}

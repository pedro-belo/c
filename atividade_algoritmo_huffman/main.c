#include "./main.h"


int main(int argc, char **argv) {

    initialize(argc, argv);

    switch(get_option()) {

        case SYMBOLS_LIST:
            app_instance()->run_symbol_list();
            break;

        case COMPRESS:
            app_instance()->run_compress();
            break;

        case DECOMPRESS:
            app_instance()->run_decompress();
            break;

        case HELP:
            app_instance()->run_help();
            break;

        default:
            fprintf(stderr, "Comando nao reconhecido.\n");

    }

    return (R_SUCCESS);
}

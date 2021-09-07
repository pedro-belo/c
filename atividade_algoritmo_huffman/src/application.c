#include "./application.h"

// Prototypes
static void help_text(void);
static void run_compress(void);
static void run_symbol_list(void);
static void run_decompress(void);
static void read_cmd_args(int argc, char **argv);

// Global Variables;
static Application *app = NULL;

// Functions;
Application *app_instance() {

    if(!app) {
        fprintf(stderr, "A aplicacao nao foi inicializada.\n");
        exit(R_ERROR);
    }

    return app;
}

Application *initialize(int argc, char **argv) {

    if(app) return app;

    app = (Application *)malloc(sizeof(Application));

    app->args.in_filename = NULL;
    app->args.exec_filename = NULL;
    app->args.out_filename = NULL;

    app->args._fa = False;
    app->args._fr = False;
    app->args._far = False;
    app->args._ts = False;
    app->args._tf = False;
    app->args._d = False;
    app->args._s = False;
    app->args._c = False;
    app->args._h = False;
    app->args._dbg = False;

    app->run_help = &help_text;
    app->run_compress = &run_compress;
    app->run_symbol_list = &run_symbol_list;
    app->run_decompress = &run_decompress;

    read_cmd_args(argc, argv);

    return app;
}

int get_option() {

    if(app_instance()->args._h)
        return HELP;

    if(app_instance()->args._s)
        return SYMBOLS_LIST;

    if(app_instance()->args._d)
        return DECOMPRESS;

    if(app_instance()->args._c)
        return COMPRESS;

    return -1;
}


// Static Functions
static void run_symbol_list(){

    if(!app_instance()->args.in_filename){
        app_instance()->run_help();
        exit(R_ERROR);        
    }

    uint_t length;
    BYTE *buffer = read_file(app_instance()->args.in_filename, &length);

    if(!buffer){
        fprintf(stderr, "Arquivo '%s' não foi encontrado.\n", app_instance()->args.in_filename);
        exit(R_ERROR);
    }

    create_symbol(buffer, length);

    if(app_instance()->args._far)
        print_far();

    else if(app_instance()->args._fr)
        print_fr();

    else if(app_instance()->args._fa)
        print_fa();
    
    else print_far();

    symbol_destroy();
    free(buffer);
}

static void run_compress(){

    if(!app_instance()->args.in_filename){
        app_instance()->run_help();
        exit(R_ERROR);
    }

    THuffman *th = NULL;

    uint_t length = 0;
    BYTE *buffer = read_file(app_instance()->args.in_filename, &length);

    if(!buffer){
        fprintf(stderr, "Leitura do arquivo '%s' falhou durante a compressao.\n", app_instance()->args.in_filename);
        exit(R_ERROR);
    }

    create_symbol(buffer, length);

    th = grow();

    create_table_code(th);

    if(app_instance()->args._tf)
        print_full_table_code();

    if(app_instance()->args._ts)
        print_short_table_code(th);

    encode(
        buffer, length,
        app_instance()->args.out_filename ?
            app_instance()->args.out_filename : NULL);

    th->destroy(&th);
    symbol_destroy();
    free(buffer);
}

static void run_decompress(){

    PNFile *pn = NULL;
    THuffman *th = NULL;

    uint_t length = 0;
    BYTE *buffer = read_file(app_instance()->args.in_filename, &length);

    if(!buffer){
        fprintf(stderr, "Leitura do arquivo '%s' falhou durante a compressao.\n", app_instance()->args.in_filename);
        exit(R_ERROR);
    }

    create_symbol(NULL, 0);

    pn = new_pn_file(buffer, length);
    if(!pn) {
        fprintf(stderr, "Descompressao falhou.\n");
        symbol_destroy();
        free(buffer);
        exit(R_ERROR);
    }

    load_meta(pn);

    th = grow();

    create_table_code(th);

    if(app_instance()->args._tf)
        print_full_table_code();

    if(app_instance()->args._ts)
        print_short_table_code(th);

    decode(
        th, pn,
        app_instance()->args.out_filename ?
            app_instance()->args.out_filename : NULL);

    th->destroy(&th);
    free(buffer);
    free(pn);
}

static void help_text() {
    fprintf(stdout, "\nCompressão de Huffman – Análise de frequência símbolos e compressão de Huffman\n");
    fprintf(stdout, "Uso: %s [-options] <file>\n", app_instance()->args.exec_filename);
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "\t-h Mostra este texto de ajuda\n");
    fprintf(stdout, "\t-c Realiza a compressão\n");
    fprintf(stdout, "\t-d Realiza a descompressão\n");
    fprintf(stdout, "\t-s Realiza apenas a análise de frequência e imprime a tabela de símbolos\n");
    fprintf(stdout, "\t-f <file> Indica o arquivo a ser processado (comprimido, descomprimido ou para apresentar a tabela de símbolos)\n");
}

static void read_cmd_args(int argc, char **argv) {

    app_instance()->args.exec_filename = argv[0];

    for (size_t i = 1; i < argc; i++) {

        if(strcmp(argv[i], "-f") == 0){

            if(argc < i + 1){
                fprintf(stderr, "Informe o nome do arquivo de entrada.\n");
                exit(R_ERROR);
            }

            app_instance()->args.in_filename = argv[i + 1];

        }

        if(strcmp(argv[i], "-o") == 0){

            if(argc < i + 1){
                fprintf(stderr, "Informe o nome do arquivo de saida.\n");
                exit(R_ERROR);
            }

            app_instance()->args.out_filename = argv[i + 1];

        }

        if(strcmp(argv[i], "-c") == 0) app_instance()->args._c = True;
        if(strcmp(argv[i], "-d") == 0) app_instance()->args._d = True;
        if(strcmp(argv[i], "-s") == 0) app_instance()->args._s = True;
        if(strcmp(argv[i], "-h") == 0) app_instance()->args._h = True;
        if(strcmp(argv[i], "-fa") == 0) app_instance()->args._fa = True;
        if(strcmp(argv[i], "-fr") == 0) app_instance()->args._fr = True;
        if(strcmp(argv[i], "-far") == 0) app_instance()->args._far = True;
        if(strcmp(argv[i], "-ts") == 0) app_instance()->args._ts = True;
        if(strcmp(argv[i], "-tf") == 0) app_instance()->args._tf = True;
        #ifdef DEBUG
            if(strcmp(argv[i], "-dbg") == 0) app_instance()->args._dbg = True;
        #endif

    }

}
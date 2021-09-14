#include "./application.h"

// Prototypes
static void help_text(void);
static void run_compress(void);
static void run_symbol_list(void);
static void run_decompress(void);
static void read_cmd_args(int argc, char **argv);

// Global Variables;
static Application *_app = NULL;

// Functions;
Application *app() {

    if(!_app) {
        fprintf(stderr, "A aplicação não foi inicializada.\n");
        exit(R_ERROR);
    }

    return _app;
}

Application *initialize(int argc, char **argv) {

    if(_app) return _app;

    _app = (Application *)malloc(sizeof(Application));

    _app->args.in_filename = NULL;
    _app->args.exec_filename = NULL;
    _app->args.out_filename = NULL;

    _app->args._fa = False;
    _app->args._fr = False;
    _app->args._ac = False;
    _app->args._bc = False;
    _app->args._hc = False;
    _app->args._s = False;
    _app->args._c = False;
    _app->args._d = False;
    _app->args._h = False;

    _app->run_help = &help_text;
    _app->run_compress = &run_compress;
    _app->run_symbol_list = &run_symbol_list;
    _app->run_decompress = &run_decompress;

    read_cmd_args(argc, argv);

    return _app;
}

int get_option() {

    if(app()->args._h)
        return HELP;

    if(app()->args._s)
        return SYMBOLS_LIST;

    if(app()->args._d)
        return DECOMPRESS;

    if(app()->args._c)
        return COMPRESS;

    return HELP;
}


// Static Functions
static void run_symbol_list(){

    THuffman *th = NULL;
    FILE *symbols = NULL;
    uint32_t length;

    BOOL all = True;
    BOOL symb = False,
         freq_abs = False,
         freq_rel = False,
         ascii_code = False,
         huffman_code = False;

    if(!(symbols=fopen(app()->args.in_filename, "rb"))){
        fprintf(stderr, "Não foi possível abrir o arquivo %s.\n", app()->args.in_filename);
        exit(R_ERROR);        
    }

    length = (uint32_t)file_length(app()->args.in_filename);

    symbol_init();
    load_symbol(symbols, length);

    th = grow();

    create_table_code(th);

    if(app()->args._hc)
        { symb = True; all = False; }
    
    if(app()->args._fa)
        { freq_abs = True; all = False; }

    if(app()->args._fr)
        { freq_rel = True; all = False; }

    if(app()->args._ac)
        { ascii_code = True; all = False; }

    if(app()->args._bc)
        { huffman_code = True; all = False; }

    if(all)
        symb = freq_abs = freq_rel = ascii_code = huffman_code = True;

    print_symbol(symb, freq_abs, freq_rel, ascii_code, huffman_code);

    th->destroy(&th);
    symbol()->destroy();
    fclose(symbols);
}

static void run_compress(){

    THuffman *th = NULL;
    FILE *compress = NULL;
    uint32_t length;

    if(!(compress=fopen(app()->args.in_filename, "rb"))){
        fprintf(stderr, "Não foi possível abrir o arquivo %s.\n", app()->args.in_filename);
        exit(R_ERROR);        
    }

    length = (uint32_t)file_length(app()->args.in_filename);

    symbol_init();
    load_symbol(compress, length);

    th = grow();

    create_table_code(th);

    fprintf(stdout, "Comprimindo:\n%s\n", app()->args.in_filename);
    
    encode(compress, length, app()->args.out_filename);

    th->destroy(&th);
    symbol()->destroy();
    fclose(compress);
}

static void run_decompress() {

    THuffman *th = NULL;
    PNFile *pn = NULL;
    FILE *handle = NULL;
    uint32_t length;

    if(!(handle=fopen(app()->args.in_filename, "rb"))){
        fprintf(stderr, "Não foi possível abrir o arquivo %s.\n", app()->args.in_filename);
        exit(R_ERROR);        
    }

    length = (uint32_t)file_length(app()->args.in_filename);

    symbol_init();

    pn = new_pn_file(handle, length);
    load_meta(pn);

    th = grow();

    create_table_code(th);

    fprintf(stdout, "Descomprimindo:\n%s\n", app()->args.in_filename);

    decode(handle, th, pn, app()->args.out_filename);

    th->destroy(&th);
    pn_file_destroy(&pn);
    fclose(handle);
}

static void help_text() {
    fprintf(stdout, "\nCompressão de Huffman – Análise de frequência símbolos e compressão de Huffman\n");
    fprintf(stdout, "Uso: %s [-options] <file>\n", app()->args.exec_filename);
    fprintf(stdout, "\nOptions:\n");
    fprintf(stdout, "-h Mostra este texto de ajuda\n");
    fprintf(stdout, "-c Realiza a compressão\n");
    fprintf(stdout, "-d Realiza a descompressão\n");
    fprintf(stdout, "-s Realiza apenas a análise de frequência e imprime a tabela de símbolos\n");
    fprintf(stdout, "\t-fa - Frequência absoluta\n");
    fprintf(stdout, "\t-fr - Frequência relativa\n");
    fprintf(stdout, "\t-ac - Código ASCII\n");
    fprintf(stdout, "\t-bc - Código de Huffman\n");
    fprintf(stdout, "\t-hc - Código Hexadecimal\n");
    fprintf(stdout, "-f <arquivo> Indica o arquivo a ser processado (comprimido, descomprimido ou para apresentar a tabela de símbolos)\n");
    fprintf(stdout, "-o <arquivo> Arquivo de saída\n");
}

static void read_cmd_args(int argc, char **argv) {

    app()->args.exec_filename = argv[0];

    for (size_t i = 1; i < argc; i++) {

        if(strcmp(argv[i], "-f") == 0){

            if(argc < i + 1){
                fprintf(stderr, "Informe o nome do arquivo de entrada.\n");
                exit(R_ERROR);
            }

            app()->args.in_filename = argv[i + 1];

        }

        if(strcmp(argv[i], "-o") == 0){

            if(argc < i + 1){
                fprintf(stderr, "Informe o nome do arquivo de saida.\n");
                exit(R_ERROR);
            }

            app()->args.out_filename = argv[i + 1];

        }

        if(strcmp(argv[i], "-c") == 0) app()->args._c = True;
        if(strcmp(argv[i], "-d") == 0) app()->args._d = True;
        if(strcmp(argv[i], "-s") == 0) app()->args._s = True;
        if(strcmp(argv[i], "-h") == 0) app()->args._s = True;

        if(strcmp(argv[i], "-fa") == 0) app()->args._fa = True;
        if(strcmp(argv[i], "-fr") == 0) app()->args._fr = True;
        if(strcmp(argv[i], "-bc") == 0) app()->args._bc = True;
        if(strcmp(argv[i], "-ac") == 0) app()->args._ac = True;
        if(strcmp(argv[i], "-hc") == 0) app()->args._hc = True;

    }

    if( (app()->args._s ||
         app()->args._c ||
         app()->args._d) &&
        !app()->args.in_filename){
        
        fprintf(stderr, "Nenhum arquivo de entrada foi informado.");
        exit(R_ERROR);
    }

}
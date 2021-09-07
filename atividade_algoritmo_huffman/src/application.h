#ifndef __H_APPLICATION__

#define __H_APPLICATION__

#define HELP 0
#define SYMBOLS_LIST 1
#define COMPRESS 2
#define DECOMPRESS 3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./file.h"
#include "./tree.h"
#include "./symbol.h"
#include "./encdec.h"

struct CMDArgs {

    char *exec_filename;
    char *in_filename;
    char *out_filename;

    BOOL _fa;
    BOOL _fr;
    BOOL _far;
    BOOL _ts;
    BOOL _tf;
    BOOL _d;
    BOOL _s;
    BOOL _c;
    BOOL _h;
    BOOL _dbg;

};

struct Application {

    struct CMDArgs args;

    void (*run_help)();
    void (*run_symbol_list)();
    void (*run_compress)();
    void (*run_decompress)();
};
typedef struct Application Application;

Application *initialize(int argc, char **argv);
Application *app_instance();
int get_option();

#endif

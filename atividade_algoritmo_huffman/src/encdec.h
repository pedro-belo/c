#ifndef __H_ENCDEC__

#define __H_ENCDEC__

#include <string.h>

#include "./symbol.h"
#include "./defs.h"
#include "./list.h"
#include <string.h>

#define MIN_PN_SIZE 16

struct PNFile {

    BYTE magic[2];
    uint_t header_len;
    BYTE *symbol_entry;

    uint_t block_len;
    uint_t padding;
    BYTE *data_entry;

};
typedef struct PNFile PNFile;

PNFile *new_pn_file(BYTE *buffer, size_t length);
void load_meta(PNFile *pn);
void decode(THuffman *th, PNFile *pn, char *filename);
void encode(BYTE *buffer, size_t length, char *filename);
BOOL magic_number_is_valid(PNFile *pn);
#endif
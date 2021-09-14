#ifndef __H_ENCDEC__

#define __H_ENCDEC__

#include <string.h>
#include "./symbol.h"
#include "./defs.h"
#include "./list.h"

#define MIN_PN_SIZE 18

struct PNFile {

    BYTE magic[2];
    uint32_t header_len;
    BYTE *symbol_entry;

    uint64_t block_len;
    uint32_t padding;

};
typedef struct PNFile PNFile;

PNFile *new_pn_file(FILE *handle, uint32_t length);
void load_meta(PNFile *pn);
void decode(FILE *decompress, THuffman *th, PNFile *pn, char *filename);
void encode(FILE *handle, uint32_t length, char *filename);
void pn_file_destroy(PNFile **pn);
BOOL magic_number_is_valid(PNFile *pn);
#endif

#include "./encdec.h"

const char default_cmp_filename[] = "compressed.pn";
const char default_dcmp_filename[] = "decompressed";
const char MAGIC[] = "PN";

void write_meta(FILE *handle){

    uint32_t length = symbol_instance()->count * (sizeof(BYTE) + sizeof(uint32_t));

    fwrite(MAGIC, sizeof(BYTE), 2, handle);

    fwrite(&length, sizeof(uint32_t), 1, handle);

    for (size_t index = 0; index < FREQ_SZ ; index++ ){

        if(symbol_instance()->freq_abs[index] == 0)
            continue;

        BYTE chr = index;
        uint32_t freq = symbol_instance()->freq_abs[index];

        fwrite(&chr, sizeof(BYTE), 1, handle);
        fwrite(&freq, sizeof(uint32_t), 1, handle);
    }

 }

void write_data(FILE *handle, BYTE *buffer, uint32_t length) {

    uint64_t copied_bits = 0;
    uint64_t total_bits = 0;
    uint64_t total_bytes = 0;
    uint32_t padding = 0;

    for (size_t index = 0; index < FREQ_SZ; index++ ) {
            
        if(symbol_instance()->freq_abs[index] == 0)
            continue;

        total_bits += symbol_instance()->total_bits[index] * symbol_instance()->freq_abs[index];
    }

    padding = total_bits % 8;
    total_bytes = (uint64_t)((total_bits - padding) / 8) + 1;

    #ifdef DEBUG
        printf("WRITE_DATA:\n\ttotal_bits=%ld,\n\ttotal_bytes=%ld,\n\tpadding=%d\n", total_bits, total_bytes, padding);
    #endif

    fwrite(&total_bytes, sizeof(uint64_t), 1, handle);
    fwrite(&padding, sizeof(uint32_t), 1, handle);

    BYTE block = 0;

    for (uint32_t item = 0; item < length; item++ ) {

            if(symbol_instance()->freq_abs[buffer[item]] == 0)
                continue;

            for (uint32_t bit = 0; bit < symbol_instance()->total_bits[buffer[item]] ; bit++){

                if(symbol_instance()->table_code[buffer[item]][bit])
                    block = block | (1 << (copied_bits % 8));

                copied_bits += 1;
                if(copied_bits % 8 == 0){
                    fwrite(&block, sizeof(BYTE), 1, handle);
                    block = 0;                    
                }

            }
    }

    if(padding > 0)
        fwrite(&block, sizeof(BYTE), 1, handle);

    fprintf(
        stdout,
        "\nTaxa de compressão:\n%.3f %%\n\n",
        ((double)total_bytes / length) * 100);
}

void encode(BYTE *buffer, uint32_t length, char *filename){

    FILE *handle = fopen(filename ? filename : default_cmp_filename, "wb");

    if(!handle){
        fprintf(stderr, "Não foi possível abrir o arquivo '%s'.\n", filename);
        exit(R_ERROR);
    }

    write_meta(handle);
    write_data(handle, buffer, length);

    fclose(handle);
}

void pn_inspect(PNFile *pn) {
    fprintf(stdout, "|------[META SECTION]-------\n");
    fprintf(stdout, "|                           \n");
    fprintf(stdout, "| MAGIC: %c%c\n",                 pn->magic[0], pn->magic[1]);
    fprintf(stdout, "| HEADER LENGTH: %d\n",           pn->header_len);
    fprintf(stdout, "| SYMBOL COUNT: %ld\n",           (uint32_t)pn->header_len / (sizeof(BYTE) + sizeof(uint32_t)));
    fprintf(stdout, "| SYMBOL ENTRY: %#lx:%p\n",           (uint32_t)pn->header_len + (2 * sizeof(BYTE)), pn->symbol_entry);
    fprintf(stdout, "|                           \n");
    fprintf(stdout, "|------[DATA SECTION]-------\n");
    fprintf(stdout, "|                           \n");
    fprintf(stdout, "| DATA LENGTH: %ld\n",              pn->block_len);
    fprintf(stdout, "| PADDING: %d\n",                  pn->padding);
    fprintf(stdout, "| DATA ENTRY: %#lx:%p\n",              (2 * sizeof(BYTE)) + (2 * sizeof(uint32_t)) + sizeof(uint64_t)  + (pn->header_len), pn->data_entry);
    fprintf(stdout, "|                           \n");
    fprintf(stdout, "|---------------------------\n");
}

PNFile *new_pn_file(BYTE *buffer, uint32_t length) {

    if(length < MIN_PN_SIZE)
        return NULL;

    PNFile *pn;

    uint32_t sz_dbyte = sizeof(BYTE) * 2;
    uint32_t sz_32int = sizeof(uint32_t);
    uint32_t sz_64int = sizeof(uint64_t);

    pn = (PNFile *)calloc(sizeof(PNFile), 1);

    if(!pn)
        return NULL;

    memcpy(
        &pn->magic[0],
        &buffer[0],
        sz_dbyte);

    if (!magic_number_is_valid(pn))
        return NULL;

    memcpy(
        &pn->header_len,
        &buffer[sz_dbyte],
        sz_32int);

    if(pn->header_len > length){
        free(pn);
        return NULL;
    }

    pn->symbol_entry = &buffer[sz_dbyte + sz_32int];

    memcpy(
        &pn->block_len,
        &buffer[sz_dbyte + sz_32int + pn->header_len],
        sz_64int);

    memcpy(
        &pn->padding,
        &buffer[sz_dbyte + sz_32int + sz_64int + pn->header_len],
        sz_32int);

    pn->data_entry = &buffer[sz_dbyte + (2 * sz_32int) + sz_64int + pn->header_len];

    return pn;
}

BOOL magic_number_is_valid(PNFile *pn) {
    return (pn->magic[0] == MAGIC[0] && pn->magic[1] == MAGIC[1]);
}

void load_meta(PNFile *pn){
    for (uint32_t i = 0 ; i < (uint32_t)(pn->header_len / (sizeof(BYTE) + sizeof(uint32_t))); i++){

        uint32_t freq;
        BYTE symb;
        uint32_t offset = sizeof(uint32_t) * i;

        memcpy(&symb, &pn->symbol_entry[i + offset], sizeof(BYTE));
        memcpy(&freq, &pn->symbol_entry[i + offset + sizeof(BYTE)], sizeof(uint32_t));

        set_freq_abs(symb, freq);

        offset += sizeof(uint32_t);
    }
}

void decode(THuffman *th, PNFile *pn, char *filename) {

    TNode *node = NULL;
    FILE *handle = fopen(filename ? filename : default_dcmp_filename, "wb");

    uint64_t block_in_bits = ((pn->block_len - 1) * 8) + pn->padding;
    uint64_t bytes = 0;

    for (uint64_t bit = 0; bit < block_in_bits ; bit++ ){

        BOOL is_high = pn->data_entry[(uint64_t)(bit / 8)] & (1 << bit % 8);

        node = get_node_bin( !node ? th->root : node, is_high ? 1 : 0);

        if(is_leaf(node)){
            fwrite(&node->label, sizeof(BYTE), 1, handle);
            bytes += 1;
            node = NULL;
        }
    }

    #ifdef DEBUG
        fprintf(stdout, "DECODE:block_len=%ld, padding=%d\n", pn->block_len, pn->padding);
    #endif

    fprintf(
        stdout,
        "\nTaxa de Descompressão:\n%.3f %%\n\n",
        ((double)pn->block_len/bytes ) * 100);

    fclose(handle);
}

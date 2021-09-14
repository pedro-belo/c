#include "./encdec.h"

const char default_cmp_filename[] = "compressed.pn";
const char default_dcmp_filename[] = "decompressed";
const char MAGIC[] = "PN";

void write_meta(FILE *handle){

    uint32_t header_len = symbol()->count * (sizeof(BYTE) + sizeof(uint32_t));

    fwrite(MAGIC, sizeof(BYTE), 2, handle);

    fwrite(&header_len, sizeof(uint32_t), 1, handle);

    for (size_t index = 0; index < FREQ_SZ ; index++ ){

        if(!has_symbol(index))
            continue;

        BYTE chr = index;
        uint32_t freq = symbol()->freq_abs[index];

        fwrite(&chr, sizeof(BYTE), 1, handle);
        fwrite(&freq, sizeof(uint32_t), 1, handle);
    }

 }

void write_data(FILE *handle, FILE *output, uint32_t length) {

    uint64_t copied_bits = 0;
    uint64_t total_bits = 0;
    uint64_t total_bytes = 0;
    uint32_t padding = 0;
    uint32_t file_len = length;

    for (size_t index = 0; index < FREQ_SZ; index++ ) {
            
        if(!has_symbol(index))
            continue;

        total_bits += symbol()->total_bits[index] * symbol()->freq_abs[index];
    }

    padding = total_bits % 8;
    total_bytes = (uint64_t)((total_bits - padding) / 8) + 1;

    #ifdef DEBUG
        printf("WRITE_DATA:\n\ttotal_bits=%ld,\n\ttotal_bytes=%ld,\n\tpadding=%d\n", total_bits, total_bytes, padding);
    #endif

    fwrite(&total_bytes, sizeof(uint64_t), 1, output);
    fwrite(&padding, sizeof(uint32_t), 1, output);

    BYTE *buffer = (BYTE *)malloc(sizeof(BYTE) * BLOCK_SIZE);

    BYTE block = 0;
    while(copied_bits < total_bits){
        
        while (length) {
                
            uint32_t bytes_read = fread(buffer, sizeof(BYTE), BLOCK_SIZE, handle);
            length -= bytes_read;

            for (uint32_t i = 0; i < bytes_read; i++ ) {

                for (uint32_t bit = 0; bit < symbol()->total_bits[buffer[i]] ; bit++){

                    if(symbol()->table_code[buffer[i]][bit])
                        block = block | (1 << (copied_bits % 8));

                    copied_bits += 1;

                    if(copied_bits % 8 == 0){
                        fwrite(&block, sizeof(BYTE), 1, output);
                        block = 0;                    
                    }

                }

            }

        }        

    }

    if(padding > 0)
        fwrite(&block, sizeof(BYTE), 1, output);

    fprintf(
        stdout,
        "\nTaxa de compressão:\n%.3f %%\n\n",
        ((double)total_bytes / file_len) * 100);
}

void encode(FILE *handle, uint32_t length, char *filename){

    FILE *output = fopen(filename ? filename : default_cmp_filename, "wb");

    if(!output){
        fprintf(stderr, "Não foi possível criar '%s'.\n", filename);
        exit(R_ERROR);
    }

    fseek(handle, 0, SEEK_SET);

    write_meta(output);
    write_data(handle, output, length);

    fclose(output);
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
    fprintf(stdout, "|                           \n");
    fprintf(stdout, "|---------------------------\n");
}

PNFile *new_pn_file(FILE *handle, uint32_t length) {

    if(length < MIN_PN_SIZE) {
        fprintf(stderr, "Arquivo mal formado.");
        exit(R_ERROR);
    }

    PNFile *pn;

    pn = (PNFile *)calloc(sizeof(PNFile), 1);

    if(!pn) {
        fprintf(stderr, "Memória insuficiente.\n");
        exit(R_ERROR);
    }

    fread(&pn->magic[0], sizeof(BYTE), 2, handle);

    if (!magic_number_is_valid(pn)) {
        fprintf(stderr, "Formato de arquivo não suportado.\n");
        exit(R_ERROR);        
    }

    fread(&pn->header_len, sizeof(uint32_t), 1, handle);

    if(pn->header_len > length){
        free(pn);
        return NULL;
    }

    pn->symbol_entry = (BYTE *)malloc(pn->header_len * sizeof(BYTE));

    fread(pn->symbol_entry, sizeof(BYTE), pn->header_len, handle);

    fread(&pn->block_len, sizeof(uint64_t), 1, handle);

    fread(&pn->padding, sizeof(int32_t), 1, handle);

    return pn;
}

void pn_file_destroy(PNFile **pn) {
    free((*pn)->symbol_entry);
    free(*pn);
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

void decode(FILE *decompress, THuffman *th, PNFile *pn, char *filename) {

    TNode *node = NULL;
    BYTE *buffer = NULL;
    FILE *output = NULL;

    output = fopen(filename ? filename : default_dcmp_filename, "wb");
    if(!output) {
        fprintf(stdout, "Criação de arquivo descompactado falhou.\n");
        exit(R_ERROR);
    }

    buffer = (BYTE *)malloc(sizeof(BYTE) * BLOCK_SIZE);
    if(!buffer) {
        fprintf(stdout, "Criação de arquivo descompactado falhou.\n");
        fclose(output);
        exit(R_ERROR);
    }

    uint64_t block_in_bits = (pn->block_len - 1) * 8;
    uint64_t total_bits = block_in_bits + pn->padding;
    uint64_t copied_bits = 0;

    node = th->root;
    while (copied_bits < total_bits) {

        uint32_t read_bytes = fread(buffer, sizeof(BYTE), BLOCK_SIZE, decompress);

        for (uint32_t index = 0; index < read_bytes; index++){

            uint32_t bits = copied_bits >= block_in_bits ? pn->padding : 8;

            for (size_t bit = 0; bit < bits; bit++){

                BOOL is_high = (buffer[index] & (1 << bit)) > 0 ? True : False;
                node = get_node_bin(node, is_high);

                if(is_leaf(node)){
                    fwrite(&node->label, sizeof(BYTE), 1, output);
                    node = th->root;
                }

                copied_bits += 1;
            }

        }
    }

    // fprintf(
    //     stdout,
    //     "\nTaxa de Descompressão:\n%.3f %%\n\n",
    //     ((double)pn->block_len/ ((copied_bits - pn->block_len) / 8) ) * 100);
    fclose(output);
    free(buffer);
}

#include "./file.h"

long file_length(char *filename) {

    FILE *handle = NULL;
    long length = 0;

    handle = fopen(filename, "rb");
    if(!handle)
        return -1;

    fseek(handle, 0, SEEK_END);
    length = ftell(handle);

    fclose(handle);
    return length;
}

BYTE *read_file(char *filename, uint_t *read_len) {

    if(!filename)
        return NULL;

    long length = file_length(filename);
    if(length < 1)
        return NULL;

    FILE *handle = fopen(filename, "rb");
    if(!handle) return NULL;

    BYTE *buffer = (BYTE *)malloc(sizeof(BYTE) * length);
    if(!buffer){
        fclose(handle);
        return NULL;
    }

    if(read_len)
        *read_len = (uint_t)fread(buffer, sizeof(BYTE), length, handle);
    else
        fread(buffer, sizeof(BYTE), length, handle);

    fclose(handle);
    return buffer;
}

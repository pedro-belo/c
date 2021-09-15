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

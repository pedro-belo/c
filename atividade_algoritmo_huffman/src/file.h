#ifndef __H_FILE__

#define __H_FILE__

#include <stdlib.h>
#include <stdio.h>
#include "./defs.h"

long file_length(char *filename);
BYTE *read_file(char *filename, uint32_t *read_len);
#endif

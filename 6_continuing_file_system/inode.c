#include "block.h"
#include "free.h"
#include <stdio.h>

#define BLOCK_SIZE 4096

unsigned char data[BLOCK_SIZE];

int ialloc(void)
{
    unsigned char *temp = bread(0, data);
    int index = find_free(temp);
    set_free(data, index, 1);
    bwrite(0, data);
    return index;
}

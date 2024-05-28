#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "image.h"
#include "free.h"

#define FREE_BLOCK_MAP_NUM 2
#define BLOCK_SIZE 4096
unsigned char datablock[BLOCK_SIZE];

unsigned char *bread(int block_num, unsigned char *block)
{
    off_t offset = (off_t) block_num * BLOCK_SIZE;

    lseek(image_fd, offset, SEEK_SET);

    read(image_fd, block, BLOCK_SIZE);

    return block;
}

void bwrite(int block_num, unsigned char *block)
{
    off_t offset = (off_t) block_num * BLOCK_SIZE;

    lseek(image_fd, offset, SEEK_SET);

    write(image_fd, block, BLOCK_SIZE);
}

int alloc(void)
{
    int index = find_free(bread(FREE_BLOCK_MAP_NUM, datablock));
    set_free(datablock, index, 1);
    bwrite(2, datablock);
    return index;
}
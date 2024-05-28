#include "inode.h"
#include "block.h"
#include "pack.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define INITIAL_DIR_SIZE 64
#define BLOCK_SIZE 4096
#define INODE_DATA_BLOCKS_START_OFFSET 4

struct directory {
    struct inode *inode;
    unsigned int offset;
};

struct directory_entry {
    unsigned int inode_num;
    char name[16];
};

void mkfs()
{
    unsigned char block[BLOCK_SIZE];
    int inode_num;
    int returned_block_num;
    struct inode *temp = ialloc();

    inode_num = temp->inode_num;
    // printf("Root inode num: %d\n", inode_num);
    returned_block_num = alloc(); // alloc returns index from block map of fist free block
    // printf("Returned Block num: %d\n", returned_block_num);

    temp->flags = 2;
    temp->size = INITIAL_DIR_SIZE;
    temp->block_ptr[0] = returned_block_num; // Fist free block index numebr set here.

    const char *current = ".";
    const char *parent = "..";
    
    write_u16(&block[0], inode_num);
    strcpy((char *)block + 2, current);

    write_u16(&block[32], inode_num);
    strcpy((char *)block + 32 + 2, parent);

    // printf("Block content starting from the third byte: %s\n", block + 2);
    // printf("Block content starting from the 35th byte: %s\n", block + 32 + 2);
    
    // printf("INODE_DATA_BLOCKS_START_OFFSET + returned_block_num: %d\n", INODE_DATA_BLOCKS_START_OFFSET + returned_block_num);

    bwrite(INODE_DATA_BLOCKS_START_OFFSET + returned_block_num, block);
    iput(temp);
}

struct directory *directory_open(int inode_num)
{
    struct inode *inode = iget(inode_num);
    if (inode == NULL){
        return NULL;
    }

    struct directory *dir = (struct directory *)malloc(sizeof(struct directory));

    dir->inode = inode;
    dir->offset = 0;

    return dir;

}

int directory_get(struct directory *dir, struct directory_entry *ent)
{
    
    int size = dir->inode->size;
    int offset = dir->offset;
    if (offset >= size)
    {
        return -1;
    }

    unsigned char block[BLOCK_SIZE];

    int data_block_index = offset / 4096;

    // printf("data_block_index: %d\n", data_block_index);
    int data_block_num = dir->inode->block_ptr[data_block_index]; // data_block_num contains index of first free block found form alloc.
    
    bread(data_block_num + INODE_DATA_BLOCKS_START_OFFSET, block);

    int offset_in_block = offset % 4096;
    // printf("offset_in_block: %d\n", offset_in_block);

    ent->inode_num = read_u16(&block[offset_in_block]);
    strcpy(ent->name, (char *)&block[offset_in_block + 2]);

    dir->offset += 32;

    return 0;
}

void directory_close(struct directory *d)
{
    iput(d->inode);
    free(d);
}
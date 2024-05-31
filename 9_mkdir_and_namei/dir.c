#include "inode.h"
#include "block.h"
#include "pack.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dirbasename.h"
#define INITIAL_DIR_SIZE 64
#define BLOCK_SIZE 4096
#define FILE_DATA_BLOCK_START_OFFSET 7
#define ROOT_INODE_NUM 0

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
    temp->block_ptr[0] = FILE_DATA_BLOCK_START_OFFSET + returned_block_num; // Fist free block index numebr set here.
    printf("block pointer: %d\n", FILE_DATA_BLOCK_START_OFFSET + returned_block_num);
    const char *current = ".";
    const char *parent = "..";
    
    write_u16(&block[0], inode_num);
    strcpy((char *)block + 2, current);

    write_u16(&block[32], inode_num);
    strcpy((char *)block + 32 + 2, parent);
    // printf("Block content starting from the third byte: %s\n", block + 2);
    // printf("Block content starting from the 35th byte: %s\n", block + 32 + 2);
    

    // printf("FILE_DATA_BLOCK_START_OFFSET + returned_block_num: %d\n", FILE_DATA_BLOCK_START_OFFSET + returned_block_num);
    printf("\n");
    printf("\n");
    printf("block root is writen in: %d\n", FILE_DATA_BLOCK_START_OFFSET + returned_block_num);
    printf("\n");
    bwrite(FILE_DATA_BLOCK_START_OFFSET + returned_block_num, block);
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
    //printf("size: %d\n", size);
    //printf("offset: %d\n", offset);
    if (offset >= size)
    {
        return -1;
    }

    unsigned char block[BLOCK_SIZE];

    int data_block_index = offset / 4096;

    // printf("data_block_index: %d\n", data_block_index);
    int data_block_num = dir->inode->block_ptr[data_block_index]; // data_block_num contains index of first free block found form alloc.
    
    bread(data_block_num, block); // This is probalby wrong. But Why?
    printf("data block number + 7: %d\n", data_block_num + FILE_DATA_BLOCK_START_OFFSET);
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

struct inode *namei(char *path)
{
    (void)path;
    return iget(ROOT_INODE_NUM);
}

int directory_make(char *path)
{
    if (*path != '/')
    {
        return -1;
    }
    
    char dirpath[1024];
    get_dirname(path, dirpath);
    //printf("dirpath: %s\n", dirpath);

    char dirname[1024];
    get_basename(path, dirname);
    //printf("dirname: %s\n", dirname);
    
    // Obtain inode of root
    struct inode *root = namei(dirpath);

    // Allocate inode of new dir
    struct inode *new_dir = ialloc();
    int inode_num = new_dir->inode_num;
    //printf("new dir inode_num: %d\n", inode_num);
    int returned_block_num = alloc();
    printf("returned_block_num: %d\n", returned_block_num);
    
    // Prepare to write subordinate inodes into the inode blocks.
    unsigned char block[BLOCK_SIZE];

    new_dir->flags = 2;
    new_dir->size = INITIAL_DIR_SIZE;
    new_dir->block_ptr[0] = returned_block_num;

    const char *current = dirname;
    const char *parent = "..";
    
    write_u16(&block[0], inode_num);
    strcpy((char *)block + 2, current);

    write_u16(&block[32], ROOT_INODE_NUM);
    strcpy((char *)block + 32 + 2, parent);

    bwrite(FILE_DATA_BLOCK_START_OFFSET + returned_block_num, block);
    

    unsigned int root_dir_size = root->size;
    // printf("dir root size: %d\n", root_dir_size);
    int *block_ptr = (int *)root->block_ptr;
    (void)block_ptr;

    // 10. Read that block into memory unless you're creating a new one (bread()), and add the new directory entry to it.
    // ? We would only create a new one if the first block was full right?
    unsigned char dir_block[BLOCK_SIZE];
    // printf("block_ptr: %d\n", *block_ptr + FILE_DATA_BLOCK_START_OFFSET);
    bread(FILE_DATA_BLOCK_START_OFFSET, dir_block);

    write_u16(&dir_block[0 + root_dir_size], inode_num);
    strcpy((char *)&dir_block[0 + 2 + root_dir_size], current);
    bwrite(FILE_DATA_BLOCK_START_OFFSET, dir_block);
    root->size += 32;

    iput(new_dir);
    iput(root);
    return 0;
}
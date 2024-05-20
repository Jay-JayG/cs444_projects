#include "block.h"
#include "free.h"
#include <stdio.h>

#define BLOCK_SIZE 4096
#define INODE_SIZE 64
#define INODE_FIRST_BLOCK 3
#define MAX_SYS_OPEN_FILES 64
#define INODE_PTR_COUNT 16

#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)

struct inode
{
    unsigned int size;
    unsigned short owner_id;
    unsigned char permissions;
    unsigned char flags;
    unsigned char link_count;
    unsigned short block_ptr[INODE_PTR_COUNT];

    unsigned int ref_count; // in-core only
    unsigned int inode_num; // in-core only
};

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

unsigned char data[BLOCK_SIZE];

struct inode *ialloc(void)
{
    unsigned char *temp = bread(0, data);
    int index = find_free(temp);
    bwrite(0, temp);
    if (index == -1)
    {
        return NULL;
    }
    struct inode *in = iget(index);
    if (in == NULL)
    {
        return NULL;
    }
    set_free(temp, index, 1);
    in->size = 0;
    in->owner_id = 0;
    in->permissions = 0;
    in->flags = 0;
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        in->block_ptr[i] = 0;
    }
    in->inode_num = index;
    write_inode(in);
    return in;

}

struct inode *incore_find_free(void)
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
    {
        if (incore[i].ref_count == 0)
        {
            return &incore[i];
        }
    }
    return NULL;
}

struct inode *incore_find(unsigned int inode_num)
{

    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
    {
        if (incore[i].ref_count > 0 && incore[i].inode_num == inode_num)
        {
            return &incore[i];
        }
    }
    return NULL;
}

void incore_free_all(void)
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
    {
        incore[i].ref_count = 0;
    }
}

void read_inode(struct inode *in, int inode_num)
{
    unsigned char buffer[BLOCK_SIZE] = {0};
    int block_num = inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    int block_offset = inode_num % INODES_PER_BLOCK;
    int block_offset_bytes = block_offset * INODE_SIZE;

    unsigned char block = bread(block_num, buffer);
    in->size = read_u32(buffer[block_offset_bytes]);
    in->owner_id = read_u16(buffer[block_offset_bytes + 4]);
    in->permissions = read_u8(buffer[block_offset_bytes + 6]);
    in->flags = read_u8(buffer[block_offset_bytes + 7]);
    in->link_count = read_u8(buffer[block_offset_bytes + 8]);
    in->block_ptr[0] = read_u16(buffer[block_offset_bytes + 9]);
    in->block_ptr[1] = read_u16(buffer[block_offset_bytes + 11]);
    in->block_ptr[2] = read_u16(buffer[block_offset_bytes + 13]);
    in->block_ptr[3] = read_u16(buffer[block_offset_bytes + 15]);
    in->block_ptr[4] = read_u16(buffer[block_offset_bytes + 17]);
    in->block_ptr[5] = read_u16(buffer[block_offset_bytes + 19]);
    in->block_ptr[6] = read_u16(buffer[block_offset_bytes + 21]);
    in->block_ptr[7] = read_u16(buffer[block_offset_bytes + 23]);
    in->block_ptr[8] = read_u16(buffer[block_offset_bytes + 25]);
    in->block_ptr[9] = read_u16(buffer[block_offset_bytes + 27]);
    in->block_ptr[10] = read_u16(buffer[block_offset_bytes + 29]);
    in->block_ptr[11] = read_u16(buffer[block_offset_bytes + 31]);
    in->block_ptr[12] = read_u16(buffer[block_offset_bytes + 33]);
    in->block_ptr[13] = read_u16(buffer[block_offset_bytes + 35]);
    in->block_ptr[14] = read_u16(buffer[block_offset_bytes + 37]);
    in->block_ptr[15] = read_u16(buffer[block_offset_bytes + 39]);
}

void write_inode(struct inode *in)
{
    unsigned char buffer[INODE_SIZE] = {0};
    unsigned int inode_num = in->inode_num;
    int block_num = inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    int block_offset = inode_num % INODES_PER_BLOCK;
    int block_offset_bytes = block_offset * INODE_SIZE;
    unsigned char *block = bread(block_num, buffer);
    write_u32(block[block_offset_bytes], in->size);
    write_u16(buffer[block_offset_bytes + 4], in->owner_id);
    write_u8(buffer[block_offset_bytes + 6], in->permissions);
    write_u8(buffer[block_offset_bytes + 7], in->flags);
    write_u8(buffer[block_offset_bytes + 8], in->link_count);
    write_u16(buffer[block_offset_bytes + 9], in->block_ptr[0]);
    write_u16(buffer[block_offset_bytes + 11], in->block_ptr[1]);
    write_u16(buffer[block_offset_bytes + 13], in->block_ptr[2]);
    write_u16(buffer[block_offset_bytes + 15], in->block_ptr[3]);
    write_u16(buffer[block_offset_bytes + 17], in->block_ptr[4]);
    write_u16(buffer[block_offset_bytes + 19], in->block_ptr[5]);
    write_u16(buffer[block_offset_bytes + 21], in->block_ptr[6]);
    write_u16(buffer[block_offset_bytes + 23], in->block_ptr[7]);
    write_u16(buffer[block_offset_bytes + 25], in->block_ptr[8]);
    write_u16(buffer[block_offset_bytes + 27], in->block_ptr[9]);
    write_u16(buffer[block_offset_bytes + 29], in->block_ptr[10]);
    write_u16(buffer[block_offset_bytes + 31], in->block_ptr[11]);
    write_u16(buffer[block_offset_bytes + 33], in->block_ptr[12]);
    write_u16(buffer[block_offset_bytes + 35], in->block_ptr[13]);
    write_u16(buffer[block_offset_bytes + 36], in->block_ptr[14]);
    write_u16(buffer[block_offset_bytes + 39], in->block_ptr[15]);
    bwrite(block_num, buffer);
}

struct inode *iget(int inode_num)
{
    struct inode *temp = incore_find(inode_num);
    if (temp != NULL)
    {
        temp->ref_count += 1;
        return temp;
    }
    temp = find_incore_free(inode_num);
    if (temp == NULL)
    {
        return NULL;
    }
    read_inode(temp, inode_num);
    temp->ref_count = 1;
    temp->inode_num = inode_num;
    return temp;
}

void iput(struct inode *in)
{
    if (in->ref_count == 0)
    {
        return;
    }
    in->ref_count -= 1;
    if (in->ref_count == 0)
    {
        write_inode(in);
    }
}
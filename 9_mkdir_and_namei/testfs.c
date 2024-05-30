#include "image.h"
#include "block.h"
#include "ctest.h"
#include "free.h"
#include "inode.h"
#include <stdio.h>
#include "pack.h"
#include <string.h>
#include "dir.h"
#include "ls.h"

#define BLOCK_SIZE 4096
#define SUPERBLOCK_NUM 0
#define FREE_INODE_MAP_NUM 1
#define FREE_BLOCK_MAP_NUM 2
#define INODE_DATA_BLOCK_NUM 3
#define INODE_DATA_BLOCKS_START_OFFSET 4

void test_write_read(void)
{
    unsigned char inbuffer[BLOCK_SIZE] = "Hello";
    unsigned char outbuffer[BLOCK_SIZE];
    bwrite(SUPERBLOCK_NUM, inbuffer);
    CTEST_ASSERT(strcmp((char*)bread(0, outbuffer), "Hello") == 0, "Testing writing and reading from disk.\n");
}

void test_overwrite(void)
{
    unsigned char inbuffer[BLOCK_SIZE] = "Hi";
    unsigned char inbuffer2[BLOCK_SIZE] = "Goodbye";
    unsigned char outbuffer[BLOCK_SIZE];
    bwrite(SUPERBLOCK_NUM, inbuffer);

    bwrite(SUPERBLOCK_NUM, inbuffer2);
    CTEST_ASSERT(strcmp((char*)bread(0, outbuffer), "Goodbye") == 0, "Testing if overwritten text is read correctly.");
}

void test_set_first_bit()
{
    unsigned char data[2] = {0b00000000, 0b00000000};
    set_free(data, 0, 1);
    CTEST_ASSERT( data[0]== 0b00000001, "Testing setting first bit to 1.");
}

void test_set_ninth_bit()
{
    unsigned char data[2] = {0b00000000, 0b00000000};
    set_free(data, 8, 1);
    CTEST_ASSERT( data[1]== 0b00000001, "Testing setting bit index 8 (9th bit) to 1.");
}

void test_free_first_bit()
{
    unsigned char data[2] = {0b11111111, 0b11111111};
    set_free(data, 0, 0);
    CTEST_ASSERT( data[0]== 0b11111110, "Testing setting first bit to 0.");
}

void test_free_ninth_bit()
{
    unsigned char data[2] = {0b11111111, 0b11111111};
    set_free(data, 8, 0);
    CTEST_ASSERT( data[1]== 0b11111110, "Testing setting bit index 8 (9th bit) bit to 0.");
}
void test_inode_ialloc()
{
    unsigned char outbuff[BLOCK_SIZE] = {0};
    ialloc();
    bread(FREE_INODE_MAP_NUM, outbuff);
    // printf("%d\n", outbuff[0]);
    CTEST_ASSERT(outbuff[0] == 0b00000001,  "Testing allocating inode and marking first inode as not free.");
    ialloc();
    bread(FREE_INODE_MAP_NUM, outbuff);
    CTEST_ASSERT(outbuff[0] == 0b00000011,  "Testing allocating inode and marking second inode as not free.");
}

void test_block_alloc()
{
    unsigned char outbuff2[BLOCK_SIZE] = {0};
    // unsigned char outbuff2[BLOCK_SIZE];
    alloc();
    bread(FREE_BLOCK_MAP_NUM, outbuff2);
    CTEST_ASSERT(outbuff2[0] == 0b00000001,  "Testing allocating block and marking first block as not free.");
    alloc();
    bread(FREE_BLOCK_MAP_NUM, outbuff2);
    CTEST_ASSERT(outbuff2[0] == 0b00000011,  "Testing allocating block and marking second block as not free.");
}

void test_inode_iget_iput_write_read()
{
    unsigned char outbuff3[BLOCK_SIZE] = {0};
    unsigned char outbuff4[BLOCK_SIZE] = {0};

    ialloc();
    ialloc();
    bread(FREE_BLOCK_MAP_NUM, outbuff3);
    CTEST_ASSERT(iget(3)->ref_count == 2,  "Testing getting inode and reading ref count.");

    struct inode *temp = incore_find(0);
    temp->size = 1;
    CTEST_ASSERT(incore_find(0)->size == 1,  "Testing settign inode size.");
    CTEST_ASSERT(incore_find(0)->ref_count == 1,  "Testing finding inode and reading ref count.");
    
    iput(temp);
    bread(INODE_DATA_BLOCK_NUM, outbuff4);
    CTEST_ASSERT(read_u32(&outbuff4[0]) == 1,  "Testing iput, writing inode into disk and reading from disk previously set inode size is 1.");

}

void test_inode_find_and_free()
{
    CTEST_ASSERT(incore_find(1)->ref_count == 1,  "Testing finding inode and reading ref count.");
    incore_free_all();
    CTEST_ASSERT(incore_find(0) == NULL,  "Testing freeing all inodes, trying to find first and getting NULL.");

}

void test_mkfs()
{
    unsigned char outbuff5[BLOCK_SIZE] = {0};
    unsigned char outbuff6[BLOCK_SIZE] = {0};
    mkfs();
    bread(FREE_BLOCK_MAP_NUM, outbuff5);
    CTEST_ASSERT(outbuff5[0] == 0b00000001,  "Testing allocating block and marking first block as not free for root directory.");

    bread(INODE_DATA_BLOCKS_START_OFFSET, outbuff6);
    CTEST_ASSERT(strcmp((char *)outbuff6 + 2, ".") == 0,  "Testing current directory file name \".\" written to data block.");
    CTEST_ASSERT(strcmp((char *)outbuff6 + 32 + 2, "..") == 0,  "Testing parent directory file name \"..\" written to data block.");
}

void test_directory_open_and_get()
{

    struct directory *dir = directory_open(0);
    CTEST_ASSERT(dir != 0,  "Testing successfully found inode 0");

    struct directory_entry *dir_ent = (struct directory_entry *)malloc(sizeof(struct directory_entry));


    directory_get(dir, dir_ent);
    CTEST_ASSERT(strcmp(dir_ent->name, ".") == 0,  "Testing directory_get() obtains first inode file name \".\" written to data block.");
    CTEST_ASSERT(dir->offset == 32,  "Testing directory_get() sets offset correctly to 32 after one call.");


    directory_get(dir, dir_ent);
    CTEST_ASSERT(strcmp(dir_ent->name, "..") == 0,  "Testing directory_get() obtains second inode file name \"..\" written to data block.");
    CTEST_ASSERT(dir->offset == 64,  "Testing directory_get() sets offset correctly to 64 after two calls.");
}

void test_namei()
{
    char path[] = "/";
    struct inode *root_inode = namei(path);
    CTEST_ASSERT(root_inode->inode_num == 0,  "Testing namei() obtains root inode.");
    CTEST_ASSERT(root_inode->size == 64,  "Testing namei() acutally obtains root inode whose size should be 64 if not blank.");
}

void test_directory_make()
{
    directory_make("/foo");
    ls();
}

int main()
{
    CTEST_VERBOSE(1);


    // image.c and block.c test set 1 VVV
    char *file = "image_bloc_tests.txt";
    int trunc = 1;
    image_open(file, trunc);

    test_write_read();
    test_overwrite();
    image_close();

    // free.c tests VVV
    test_set_first_bit();
    test_set_ninth_bit();
    test_free_first_bit();
    test_free_ninth_bit();

    // inode.c tests set 2
    char *file1 = "inode.txt";
    image_open(file1, trunc);

    test_inode_ialloc();
    test_block_alloc();
    test_inode_iget_iput_write_read();
    test_inode_find_and_free();

    image_close();

    // mkfs() & ls() tests
    char *file2 = "mkfs.txt";
    image_open(file2, trunc);
    
    test_mkfs();
    test_directory_open_and_get();

    //namei() & directory_make() tests
    test_namei();
    test_directory_make();
    image_close();


    CTEST_RESULTS();
    CTEST_EXIT();

    return (0);
}
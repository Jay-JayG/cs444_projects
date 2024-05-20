#include "image.h"
#include "block.h"
#include "ctest.h"
#include "free.h"
#include "inode.h"
#include <stdio.h>
#include <string.h>

#define BLOCK_SIZE 4096

void test_write_read(void)
{
    unsigned char inbuffer[BLOCK_SIZE] = "Hello";
    unsigned char outbuffer[BLOCK_SIZE];
    bwrite(0, inbuffer);
    CTEST_ASSERT(strcmp((char*)bread(0, outbuffer), "Hello") == 0, "Testing writing and reading from disk.\n");
}

void test_overwrite(void)
{
    unsigned char inbuffer[BLOCK_SIZE] = "Hi";
    unsigned char inbuffer2[BLOCK_SIZE] = "Goodbye";
    unsigned char outbuffer[BLOCK_SIZE];
    bwrite(0, inbuffer);

    bwrite(0, inbuffer2);
    CTEST_ASSERT(strcmp((char*)bread(0, outbuffer), "Goodbye") == 0, "Testing if overwritten text is read correctly.\n");
}

void test_set_first_bit()
{
    unsigned char data[2] = {0b00000000, 0b00000000};
    set_free(data, 0, 1);
    CTEST_ASSERT( data[0]== 0b00000001, "Testing setting first bit to 1");
}

void test_set_ninth_bit()
{
    unsigned char data[2] = {0b00000000, 0b00000000};
    set_free(data, 8, 1);
    CTEST_ASSERT( data[1]== 0b00000001, "Testing setting bit index 8 (9th bit) to 1");
}

void test_free_first_bit()
{
    unsigned char data[2] = {0b11111111, 0b11111111};
    set_free(data, 0, 0);
    CTEST_ASSERT( data[0]== 0b11111110, "Testing setting first bit to 0");
}

void test_free_ninth_bit()
{
    unsigned char data[2] = {0b11111111, 0b11111111};
    set_free(data, 8, 0);
    CTEST_ASSERT( data[1]== 0b11111110, "Testing setting bit index 8 (9th bit) bit to 0");
}
void test_inode_ialloc()
{
    unsigned char outbuff[BLOCK_SIZE] = {0};
    ialloc();
    bread(1, outbuff);
    CTEST_ASSERT(outbuff[0] == 0b00000001,  "Testing allocating inode and marking first inode as not free");
    ialloc();
    bread(1, outbuff);
    CTEST_ASSERT(outbuff[0] == 0b00000011,  "Testing allocating inode and marking second inode as not free");
}

void test_inode_alloc()
{
    unsigned char outbuff[BLOCK_SIZE] = {0};
    // unsigned char outbuff2[BLOCK_SIZE];
    alloc();
    bread(2, outbuff);
    CTEST_ASSERT(outbuff[0] == 0b00000001,  "Testing allocating block and marking first block as not free");
    alloc();
    bread(2, outbuff);
    CTEST_ASSERT(outbuff[0] == 0b00000011,  "Testing allocating block and marking second block as not free");
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
    test_inode_alloc();

    image_close();

    CTEST_RESULTS();

    CTEST_EXIT();
    return (0);
}
#include "image.h"
#include "block.h"
#include "ctest.h"
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
    unsigned char outbuffer2[BLOCK_SIZE];
    bwrite(0, inbuffer);
    CTEST_ASSERT(strcmp((char*)bread(0, outbuffer), "Hi") == 0, "Testing initial writing and reading from disk.\n");

    bwrite(0, inbuffer2);
    CTEST_ASSERT(strcmp((char*)bread(0, outbuffer2), "Goodbye") == 0, "Testing if overwritten text is read correctly.\n");
}

int main()
{
    CTEST_VERBOSE(1);
    char *file = "Hello.txt";
    int trunc = 1;
    image_open(file, trunc);

    test_bwrite_bread();
    test_over_bwrite();

    CTEST_RESULTS();

    CTEST_EXIT();
    image_close();
    return (0);
}
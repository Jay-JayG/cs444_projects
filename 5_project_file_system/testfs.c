#include "image.h"
#include "block.h"
#include <stdio.h>
#define BLOCK_SIZE 4096


int main() {
    unsigned char inbuffer[BLOCK_SIZE] = "Hello";
    unsigned char outbuffer[BLOCK_SIZE];
    char *file = "Hello.txt";
    int trunc = 1;
    image_open(file, trunc);
    bwrite(0, inbuffer);
    printf("Result %s\n", bread(0, outbuffer));
    bwrite(3, inbuffer);
    printf("Result %s\n", bread(3, outbuffer));
    image_close();
    return(0);
}
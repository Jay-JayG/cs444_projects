#include <stdio.h>

#define NUM_BYTES 4096

void set_free(unsigned char *block, int num, int set)
{
    int byte_num = num / 8;
    // printf("byte num: %d\n", byte_num);
    int bit_num = num % 8;
    // printf("bit num: %d\n", bit_num);
    if (set == 1)
    {
        block[byte_num] |= (1 << bit_num);
    }
    else
    {
        block[byte_num] &= ~(1 << bit_num);
    }
}

int find_low_clear_bit(unsigned char x)
{
    for (int i = 0; i < 8; i++)
        if (!(x & (1 << i)))
            return i;

    return -1;
}

int find_free(unsigned char *block)
{
    int bit;
    for (int byte = 0; byte < NUM_BYTES; byte++)
    {
        // printf("Curre byte: %d\n", byte);
        bit = find_low_clear_bit(block[byte]);
        if (bit == 0)
        {
            // printf("Num found: %d\n", ((byte * 8) + bit));
            return ((byte * 8) + bit);
        }
    }
    return -1;
}
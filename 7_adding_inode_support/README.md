# Starting the File System

## Building

Command line:

* `make` to build. An executable called `testfs` will be produced.

## Files

* `testfs.c`: The main code
* `image.c`: Contains functions used to open and close the disk file (imitated using a text file).
* `image.h`: header file for image.c.
* `block.c`: Contains functions for reading and writing from the text disk file. Also contains alloc() for allocating free blocks.
* `block.h`: header file for block.c.
* `libvvsfs.a`: Library file that contains the code from image.c and block.c.
* `ctest.h`: Testing framework, contains macros for implementing testing.
* `free.c`: Contains function for setting a bit to 1 or 0, for finding the index of a 0 bin in a byte, and for finding the lowest free bit in a block.
* `free.h`: Header file for free.c.
* `inode.c`: contains ialloc() that allocates the free inodes.
* `inode.h`: Header file for inode.c.

## Data

- Hello.txt is a file created via the image_open() function from image.c. It imitates the functionality of a disk. Data is written and read from this file.

## Functions

* `testf.c`
  * `main()`
    * `image_open(file, trunc)`: Create/opens the disk.
    * `test_write_read()`: Test function - Writes text into the disk file, then attempts to read that same text and check if it is the same.
    * `test_over_bwrite()`: Test function - Writes text into the disk then writes over that text with some new text and reads the new text checking if it is the same.
    * `test_set_first_bit()`: Tests setting first bit in first byte in block to 1.
    * `test_set_ninth_bit()`: Tests setting first bit in second byte in block to 1.
    * `test_free_first_bit()`: Tests setting first bit in first byte in block to 0.
    * `test_free_ninth_bit()`: Tests setting first bit in second byte in block to 0.
    * `test_inode_ialloc()`: Tests allocating first free inode.
    * `test_inode_alloc()`: Tests allocating first free block.

* `image.c`
  * `image_open(char *filename, int truncate)`: Creates/opens the disk. Opens a file descriptor and returns it.
  * `image_close(void)`: Closes the file descriptor opened in image_open() using close() and returns the result of the close().
 
* `block.c`
  * `bread(int block_num, unsigned char *block)`: Takes a block number and a pointer to a block sized char array. It will read the contents of the specified block from the disk into the provided block pointer.
  * `bwrite(int block_num, unsigned char *block)`: Takes a block number and a pointer to a block sized char array. It will write the contents of the passed block pointer into the specified block on the disk.
  * `int alloc(void)`: Obtains the index of the first previous free block, sets it as in use and returns the index of its location.
 
* `free.c`
  * `void set_free(unsigned char *block, int num, int set)`: Takes an block, uses the passed num to find the corresponding bit to set to the value of set which is 1 or 0.
  * `int find_low_clear_bit(unsigned char x)`: Finds the lowest 0 bit in a byte.
  * `int find_free(unsigned char *block)`: Finds the lowest 0 bit in a block.
 
* `inode.c`:
  * `int ialloc(void)`: Obtains the index of the first previous free inode, sets it as in use and returns the index of its location.

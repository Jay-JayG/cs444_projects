# Starting the File System

## Building

Command line:

* `make` to build. An executable called `testfs` will be produced.

## Files

* `testfs.c`: The main code
* `image.c`: Contains functions used to open and close the disk file (imitated using a text file).
* `image.h`: header file for image.c.
* `block.c`: Contains functions for reading and writing from the text disk file.
* `block.h`: header file for block.c.
* `libvvsfs.a`: Library file that contains the code from image.c and block.c.
* `ctest.h`: Testing framework, contains macros for implementing testing.

## Data

- Hello.txt is a file created via the image_open() function from image.c. It imitates the functionality of a disk. Data is written and read from this file.

## Functions

* `testf.c`
  * `main()`
	* `image_open(file, trunc)`: Create/opens the disk.
	* `test_write_read()`: Test function - Writes text into the disk file, then attempts to read that same text and check if it is the same.
	* `test_over_bwrite()`: Test function - Writes text into the disk then writes over that text with some new text and reads the new text checking if it is the same.

* `image.c`
  * `image_open(char *filename, int truncate)`: Creates/opens the disk. Opens a file descriptor and returns it.
  * `image_close(void)`: Closes the file descriptor opened in image_open() using close() and returns the result of the close().
 
* `block.c`
  * `bread(int block_num, unsigned char *block)`: Takes a block number and a pointer to a block sized char array. It will read the contents of the specified block from the disk into the provided block pointer.
  * `bwrite(int block_num, unsigned char *block)`: Takes a block number and a pointer to a block sized char array. It will write the contents of the passed block pointer into the specified block on the disk.

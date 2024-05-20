CC=gcc
CCFLAGS=-Wall -Wextra -Werror -DCTEST_ENABLE

.PHONY: clean pristine

testfs: testfs.o libvvsfs.a
	$(CC) $(CCFLAGS) testfs.o -L. -lvvsfs -o testfs
testfs.o: testfs.c image.h block.h ctest.h free.h inode.h pack.h
	$(CC) $(CCFLAGS) -c testfs.c
libvvsfs.a: image.o block.o free.o inode.o
	ar rcs $@ $^
image.o: image.c
	$(CC) $(CCFLAGS) -c image.c
block.o: block.c
	$(CC) $(CCFLAGS) -c block.c
free.o: free.c
	$(CC) $(CCFLAGS) -c free.c
inode.o: inode.c
	$(CC) $(CCFLAGS) -c inode.c
pack.o: pack.c
	$(CC) $(CCFLAGS) -c pack.c

clean:
	rm -f *.o

pristine: 
	rm -f *.o
	rm -f testfs libvvsfs.a
	rm -f *.txt
	rm -f free
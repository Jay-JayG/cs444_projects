CC=gcc
CCFLAGS=-Wall -Wextra -Werror -DCTEST_ENABLE

.PHONY: clean pristine

testfs: testfs.o libvvsfs.a
	$(CC) $(CCFLAGS) testfs.o -L. -lvvsfs -o testfs
testfs.o: testfs.c image.h block.h ctest.h
	$(CC) $(CCFLAGS) -c testfs.c
libvvsfs.a: image.o block.o
	ar rcs $@ $^
image.o: image.c
	$(CC) $(CCFLAGS) -c image.c
block.o: block.c
	$(CC) $(CCFLAGS) -c block.c

clean:
	rm -f *.o

pristine: 
	rm -f testfs libvvsfs.a
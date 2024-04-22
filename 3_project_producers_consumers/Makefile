CC=gcc
CCOPTS=-Wall -Wextra -Werror
LIBS=

SRCS = pc.c eventbuf.c
TARGETS = pc

.PHONY: all clean

all: $(TARGETS)


$(TARGETS): $(SRCS)
	$(CC) $(CCOPTS) -o $@ $^ $(LIBS)
    
clean:
	rm -f $(TARGETS)
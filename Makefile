CC := gcc
CFLAGS := -std=gnu99 -Wall -Wextra -pedantic -Iinclude/
OBJS := $(patsubst src/%.c,build/%.o,$(wildcard src/*.c))

.PHONY: huff all clean distclean

all: huff

clean:
	-$(RM) $(OBJS) 

distclean: clean
	-$(RM) huff

huff: $(OBJS)
	$(CC) -o $@ $^

build/%.o: src/%.c include/bitstream.h
	$(shell mkdir -p $(dir $@))
	$(CC) -c $(DEFS) $(CFLAGS) -o $@ $< 

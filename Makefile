CC := gcc
DEFS :=
CFLAGS := -std=gnu99 -Wall -Wextra -pedantic -Iinclude/
OBJS := $(patsubst src/%.c,build/%.o,$(wildcard src/*.c))

.PHONY: compress decompress all clean distclean

all: compress decompress

clean:
	-$(RM) $(OBJS) $(COBJS) $(DOBJS)

distclean: clean
	-$(RM) compress decompress

compress: DEFS += -DCOMPRESS
compress: $(OBJS)
	$(CC) -o $@ $^

decompress: DEFS += -DDECOMPRESS
decompress: $(OBJS)
	$(CC) -o $@ $^

build/%.o: src/%.c include/bitstream.h
	$(shell mkdir -p $(dir $@))
	$(CC) -c $(DEFS) $(CFLAGS) -o $@ $< 

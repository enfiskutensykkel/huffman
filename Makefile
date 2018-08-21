PROJECT := huff
CC := gcc
CFLAGS := -std=gnu99 -Wall -Wextra -pedantic -Iinclude/
OBJS := $(patsubst src/%.c,build/%.o,$(wildcard src/*.c))

.PHONY: $(PROJECT) all clean

all: $(PROJECT)

clean:
	-$(RM) $(OBJS)

$(PROJECT): $(OBJS)
	$(CC) -o $@ $^

build/%.o: src/%.c
	$(shell mkdir -p $(dir $@))
	$(CC) -c $(CFLAGS) -o $@ $< 

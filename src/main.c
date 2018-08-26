#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include "tree.h"
#include "entry.h"
#include "coding.h"
#include "bitstream.h"
#include <sys/stat.h>


static void print_entry(const struct HEntry* entry)
{
    int i;
    char symbol = ' ';
    if (0x20 <= entry->symbol && entry->symbol <= 0x7e)
    {
        symbol = entry->symbol;
    }

    fprintf(stderr, "%02x '%c' %3u: ", entry->symbol, symbol, entry->depth);
    for (i = 0; i < entry->depth; ++i)
    {
        char bit = !!(entry->code & (1 << i));
        fprintf(stderr, "%c", '0' + bit);
    }

    for (; i < 8; ++i)
    {
        fprintf(stderr, " ");
    }

    fprintf(stderr, " %12zu\n", entry->freq);
}


static void print_table(struct HEntry** const table)
{
    for (int i = 0; i < 256; ++i)
    {
        if (table[i] != NULL)
        {
            print_entry(table[i]);
        }
    }
}


static void count_byte_frequency(size_t* frequency, const void* data, size_t length)
{
    const unsigned char* ptr = data;
    const unsigned char* end = ptr + length;

    while (ptr != end)
    {
        ++frequency[*ptr++];
    }
}


static size_t count_compressed_size(struct HEntry** const table, const struct HTree* root)
{
    size_t bits = 0;

    for (int i = 0; i < 256; ++i)
    {
        if (table[i] != NULL)
        {
            bits += table[i]->freq * table[i]->depth;
            bits += 8;
        }
    }

    bits += tree_size(root);

    return bits / 8 + bits % 8 != 0;
}


static void give_usage(const char* name)
{
}


int main(int argc, char** argv)
{
    size_t frequency[256];
    for (int i = 0; i < 256; ++i)
    {
        frequency[i] = 0;
    }

    static struct option opts[] = 
    {
        { .name = "help", .has_arg = no_argument, .flag = NULL, .val = 'h' },
        { .name = "extract", .has_arg = no_argument, .flag = NULL, .val = 'x' },
        { .name = "list", .has_arg = no_argument, .flag = NULL, .val = 'l' },
        { .name = NULL, .has_arg = 0, .flag = NULL, .val = 0 }
    };

    int opt;
    int idx;
    bool extract = false;
    bool show_tables = false;
    bool list_files = true;

    while ((opt = getopt_long(argc, argv, ":hxlt", opts, &idx)) != -1)
    {
        switch (opt)
        {
            case '?':
                fprintf(stderr, "Option `%s' is unknown\n", argv[optind - 1]);
                return '?';

            case ':':
                fprintf(stderr, "Missing argument for option `%s'\n", argv[optind - 1]);
                return ':';

            case 'h':
                give_usage(argv[0]);
                return 1;

            case 'x':
                extract = true;
                break;

            case 'l':
                list_files = true;
                break;

            case 't':
                show_tables = true;
                break;

        }
    }

    if (optind == argc)
    {
        fprintf(stderr, "Missing arguments!\n");
        give_usage(argv[0]);
        return 1;
    }

    // Check all files
    for (int i = optind; i < argc; ++i)
    {
        
    }

    return 0;
}

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "entry.h"
#include "coding.h"
#include "bitstream.h"


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


int main(int argc, char** argv)
{
    size_t frequency[256];
    for (int i = 0; i < 256; ++i)
    {
        frequency[i] = 0;
    }

    //const char* str = "the zen of css design";
    const char* str = "SIR NORMAN BETTISON, ACCUSED OF BLAMING LIVERPOOL FANS FOR THE 1989 DISASTER, WILL NOT BE PROSECUTED.";
    //const char* str = "heello";
    size_t n = strlen(str);
    struct HEntry* table[256];
    struct HTree* tree;

    count_byte_frequency(frequency, str, n);

    tree_from_freq(&tree, table, frequency);

    void* symbols;
    size_t size;
    tree_string(&symbols, &size, tree);

    bitpos_t bytestream = STREAM_INIT;
    char buff[n];
    encode(table, str, n, &buff, &bytestream);


    struct HEntry* table2[256];
    struct HTree* tree2;
    tree_from_string(&tree2, table2, symbols);

    fprintf(stderr, "%s\n\n", str);

//    FILE* fp = fopen("file.dat", "w");
//    fwrite(buff, 1, bytestream.bytes + bytestream.carry != 0, fp);
//    fclose(fp);
//    
//    char debuff[n + 1];
//
//    n = decode(tree, buff, NULL, &bytestream, debuff);
//    debuff[n] = 0;
//
//    fprintf(stderr, "%s\n", debuff);

    print_table(table);
    fprintf(stderr, "\n");
    print_table(table2);

    tree_clear(tree);
    free(symbols);
    return 0;
}

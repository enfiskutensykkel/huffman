#ifndef __HUFFMAN_ENTRY_H__
#define __HUFFMAN_ENTRY_H__

#include <stddef.h>
#include <stdint.h>


/*
 * Huffman tree entry.
 * Each entry represents a symbol (character) in the Huffman tree.
 * Contains frequency it occurs, depth in the Huffman tree (length of code) and
 * the Huffman code.
 */
struct HEntry
{
    size_t      freq;   // Frequency of symbol (or aggregated frequency of symbols)
    uint8_t     depth;  // Tree depth
    uint8_t     symbol; // The current symbol (if leaf node)
    uint16_t    code;   // Huffman code for node
};


#endif

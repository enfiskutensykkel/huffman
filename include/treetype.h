#ifndef __HUFFMAN_TREETYPE_H__
#define __HUFFMAN_TREETYPE_H__

#include "entry.h"
#include "tree.h"


/*
 * Huffman tree node.
 */
struct HTree
{
    /* Linked list node */
    struct HTree*   prev;
    struct HTree*   next;

    /* Huffman tree node */
    struct HTree*   left;   // Left child node
    struct HTree*   right;  // Right child node

    /* Huffman entry */
    struct HEntry   entry;
};

#endif

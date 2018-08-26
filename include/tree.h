#ifndef __HUFFMAN_TREE_H__
#define __HUFFMAN_TREE_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Forward declarations */
struct HEntry;
struct HTree;
struct BitPosition;


/*
 * Build Huffman tree from symbol frequency table.
 */
int tree_from_freq(struct HTree** root, struct HEntry** const, const size_t* freq_table);


/*
 * Build Huffman tree from binary representation.
 */
int tree_from_string(struct HTree** root, struct HEntry** const, const void* string, struct BitPosition* stream);


/*
 * Convert tree into binary representation.
 * Returns number of bits.
 */
size_t tree_string(const struct HTree* root, void* string, struct BitPosition* stream);


/*
 * Release the tree.
 */
void tree_clear(struct HTree* root);


/*
 * Get tree entry.
 */
const struct HEntry* tree_entry(const struct HTree* node);


/*
 * Get tree size.
 */
size_t tree_size(const struct HTree* root);

#endif

#ifndef __HUFFMAN_CODING_H__
#define __HUFFMAN_CODING_H__

#include <stddef.h>
#include <stdint.h>


struct HEntry;
struct HTree;
struct BitPosition;



int encode(struct HEntry** const table, 
           const void* src,
           size_t length,
           void* dst, 
           struct BitPosition* dstpos);


size_t decode(const struct HTree* root,
              const void* src, 
              struct BitPosition* srcpos,
              const struct BitPosition* srcend,
              void* dst);

#endif

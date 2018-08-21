#include "coding.h"
#include "entry.h"
#include "bitstream.h"
#include "treetype.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>


int encode(struct HEntry** const table, const void* src, size_t length, void* dst, bitpos_t* pos)
{
    const unsigned char* in = (const unsigned char*) src;
    const unsigned char* end = in + length;

    while (in != end)
    {
        const struct HEntry* ptr = table[*in++];
        if (ptr == NULL)
        {
            return EINVAL;
        }

        for (int i = 0; i < ptr->depth; ++i)
        {
            write_bit(dst, pos, !!(ptr->code & (1 << i)));
        }
    }

    return 0;
}


size_t decode(const struct HTree* root, const void* src, bitpos_t* pos, const bitpos_t* end, void* dst)
{
    bitpos_t defstream = STREAM_INIT;
    unsigned char* out = (unsigned char*) dst;

    if (pos == NULL)
    {
        pos = &defstream;
    }

    const struct HTree* ptr = root;
    while (pos->bytes < end->bytes || (pos->bytes == end->bytes && pos->carry < end->carry))
    {
        int bit = read_bit(src, pos);
        if (bit)
        {
            ptr = ptr->right;
        }
        else
        {
            ptr = ptr->left;
        }

        if (ptr->left == NULL)
        {
            *out++ = ptr->entry.symbol;
            ptr = root;
        }
    }

    return out - ((unsigned char*) dst);
}

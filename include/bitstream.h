#ifndef __HUFFMAN_BITSTREAM_H__
#define __HUFFMAN_BITSTREAM_H__

#include <stddef.h>
#include <limits.h>


/*
 * Keep track of position in a bitstream.
 */
struct BitPosition
{
    size_t      bytes;  // Current byte count
    unsigned    carry;  // How many bits over in next byte count
};


/*
 * Convenience type.
 */
typedef struct BitPosition bitpos_t;


#define STREAM_INIT {0, 0}
#define stream_init(pos, bytes, carry)     (*(pos) = {(bytes) + ((bits) / CHAR_BIT), (carry) % CHAR_BIT})


static inline int read_bit(const void* stream, bitpos_t* pos)
{
    unsigned char byte = *(((const unsigned char*) stream) + pos->bytes);
    int bit = !!(byte & (1 << (CHAR_BIT - 1 - pos->carry)));

    if (++pos->carry == CHAR_BIT)
    {
        pos->carry = 0;
        ++pos->bytes;
    }

    return bit;
}


static inline void write_bit(void* stream, bitpos_t* pos, int bit)
{
    unsigned char* bytes = (unsigned char*) stream;
    bytes[pos->bytes] &= (unsigned char) ~(1 << (CHAR_BIT - 1 - pos->carry));
    bytes[pos->bytes] |= (!!bit) << (CHAR_BIT - 1 - pos->carry);

    if (++pos->carry == CHAR_BIT)
    {
        pos->carry = 0;
        bytes[++pos->bytes] = 0;
    }
}

#endif

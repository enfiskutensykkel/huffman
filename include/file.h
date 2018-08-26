#ifndef __HUFFMAN_FILE_H__
#define __HUFFMAN_FILE_H__

#include <stddef.h>


struct FileHdr
{
    size_t  next_file;
    size_t  name_len;
    char    name[];
};


#endif

#include "treetype.h"
#include "tree.h"
#include "entry.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "bitstream.h"


static void init_node(struct HTree* node, uint8_t symbol, uint64_t freq)
{
    node->next = node;
    node->prev = node;
    node->left = NULL;
    node->right = NULL;
    node->entry.freq = freq;
    node->entry.depth = 0;
    node->entry.symbol = symbol;
    node->entry.code = 0;
}


static void insert(struct HTree* head, struct HTree* node)
{
    struct HTree* curr = head;

    // Find position to insert after
    while (curr->next != head && curr->next->entry.freq <= node->entry.freq)
    {
        curr = curr->next;
    }

    // Insert after position
    node->next = curr->next;
    node->prev = curr;
    curr->next->prev = node;
    curr->next = node;
}


static struct HTree* remove_first(struct HTree* head)
{
    if (head->next != head)
    {
        struct HTree* node = head->next;

        node->prev->next = node->next;
        node->next->prev = node->prev;
        node->prev = node;
        node->next = node;

        return node;
    }

    return NULL;
}


static struct HTree* create_node(uint8_t symbol, uint64_t freq)
{
    struct HTree* node = malloc(sizeof(struct HTree));
    if (node == NULL)
    {
        return NULL;
    }

    init_node(node, symbol, freq);
    return node;
}


static void clear_list(struct HTree* head)
{
    struct HTree* node = NULL;

    while ((node = remove_first(head)) != NULL)
    {
        tree_clear(node);
    }
}


void tree_clear(struct HTree* root)
{
    if (root != NULL)
    {
        tree_clear(root->left);
        tree_clear(root->right);
        free(root);
    }
}


static uint8_t traverse_and_code(struct HTree* root, uint8_t depth, uint32_t code)
{
    if (root->left == NULL)
    {
        root->entry.code = code;
        root->entry.depth = depth;
        return depth;
    }
    else
    {
        uint8_t x = traverse_and_code(root->left, depth + 1, code);
        uint8_t y = traverse_and_code(root->right, depth + 1, code | (1 << depth));
        //uint8_t x = traverse_and_code(root->left, depth + 1, code << 1);
        //uint8_t y = traverse_and_code(root->right, depth + 1, (code << 1) | 1);
        return x >= y ? x : y;
    }
}


static void traverse_and_count(const struct HTree* root, int* nodes, int* leaves)
{
    if (root->left == NULL)
    {
        *leaves += 1;
    }
    else
    {
        *nodes += 1;
        traverse_and_count(root->left, nodes, leaves);
        traverse_and_count(root->right, nodes, leaves);
    }
}


int tree_from_freq(struct HTree** root, struct HEntry** const table, const size_t* frequency)
{
    struct HTree unassigned;
    init_node(&unassigned, 0, 0);

    *root = NULL;

    // Create leaf nodes and set table
    for (int i = 0; i < 256; ++i)
    {
        table[i] = NULL;

        if (frequency[i] > 0)
        {
            struct HTree* leaf = create_node(i, frequency[i]);
            if (leaf == NULL)
            {
                clear_list(&unassigned);
                return errno;
            }
            
            table[i] = &leaf->entry;
            insert(&unassigned, leaf);
        }
    }

    // Create parent nodes and insert into tree
    while (unassigned.next->next != &unassigned)
    {
        struct HTree* parent = create_node(0, 0);
        if (parent == NULL)
        {
            clear_list(&unassigned);
            return errno;
        }

        struct HTree* left = remove_first(&unassigned);
        struct HTree* right = remove_first(&unassigned);

        parent->left = left;
        parent->right = right;
        parent->entry.freq = left->entry.freq + right->entry.freq;

        insert(&unassigned, parent);
    }

    struct HTree* super = remove_first(&unassigned);
    traverse_and_code(super, 0, 0);

    *root = super;
    return 0;
}


const struct HEntry* tree_entry(const struct HTree* root)
{
    if (root == NULL)
    {
        return NULL;
    }

    return (const struct HEntry*) (((const char*) root) - offsetof(struct HTree, entry));
}


static int build_symbol_string(const struct HTree* root, uint8_t* string, size_t bits)
{
    if (root->left == NULL)
    {
        string[bits / 8] |= 1 << (bits % 8);
        ++bits;

        for (int i = 0; i < 8; ++i, ++bits)
        {
            string[bits / 8] |= (!!(root->entry.symbol & (1 << i))) << (bits % 8);
        }
    }
    else
    {
        bits = build_symbol_string(root->left, string, bits + 1);
        bits = build_symbol_string(root->right, string, bits + 1);
    }

    return bits;
}


static struct HTree* build_node(const void* string, bitpos_t* pos)
{
    uint8_t symbol = 0;
    for (int i = 0; i < 8; ++i)
    {
        symbol <<= 1;
        symbol |= read_bit(string, pos);
    }

    struct HTree* node = create_node(symbol, 0);

}


static void build_tree(const struct HTree* root, const void* string, bitpos_t* pos)
{

}


int tree_from_string(struct HTree** tree, struct HEntry** const, const void* string)
{
    struct HTree* root = NULL;

    *tree = NULL;
    
    bitpos_t pos = STREAM_INIT;
    while (1)
    {
        int bit = read_bit(string, &pos);
        if (bit)
        {

        }
    }

    *tree = root;
    return 0;
}


int tree_string(void** string, size_t* size, const struct HTree* root)
{
    *string = NULL;
    *size = 0;

    int nonleaves = 0;
    int leaves = 0;
    traverse_and_count(root, &nonleaves, &leaves);
    uint32_t bytes = (nonleaves * 1 + leaves * 9) / 8 + !((nonleaves * 1 + leaves * 9) % 8);
    
    uint8_t* t = malloc(bytes);
    memset(t, 0, bytes);
    if (t == NULL)
    {
        return errno;
    }

    build_symbol_string(root, t, 0);

    *string = (void*) t;
    *size = bytes;
    return 0;
}


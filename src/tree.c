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
        return 1;
    }
    else
    {
        uint8_t x = traverse_and_code(root->left, depth + 1, code);
        uint8_t y = traverse_and_code(root->right, depth + 1, code | (1 << depth));
        //uint8_t x = traverse_and_code(root->left, depth + 1, code << 1);
        //uint8_t y = traverse_and_code(root->right, depth + 1, (code << 1) | 1);
        return x + y + 1;
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
    super->entry.depth = traverse_and_code(super, 0, 0);

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


static struct HTree* read_string_and_build_tree(struct HEntry** const table, const void* string, bitpos_t* pos)
{
    struct HTree* node = NULL;

    int leaf = read_bit(string, pos);
    if (leaf == 1)
    {
        uint8_t symbol = 0;
        for (int i = 0; i < 8; ++i)
        {
            symbol <<= 1;
            symbol |= read_bit(string, pos);
        }

        node = create_node(symbol, 0);
        if (node == NULL)
        {
            return NULL;
        }
        table[node->entry.symbol] = &node->entry;
    }
    else
    {
        node = create_node(0, 0);
        if (node == NULL)
        {
            return NULL;
        }

        node->left = read_string_and_build_tree(table, string, pos);
        if (node->left == NULL)
        {
            tree_clear(node);
            node = NULL;
        }
        
        node->right = read_string_and_build_tree(table, string, pos);
        if (node->right == NULL)
        {
            tree_clear(node);
            node = NULL;
        }
    }

    return node;
}


int tree_from_string(struct HTree** tree, struct HEntry** const table, const void* string, bitpos_t* stream)
{
    bitpos_t default_stream = STREAM_INIT;
    struct HTree* root;

    *tree = NULL;

    for (int i = 0; i < 256; ++i)
    {
        table[i] = NULL;
    }

    if (stream == NULL)
    {
        stream = &default_stream;
    }

    root = read_string_and_build_tree(table, string, stream);
    if (root == NULL)
    {
        return errno;
    }

    root->entry.depth = traverse_and_code(root, 0, 0);

    *tree = root;
    return 0;
}


static size_t traverse_and_write_string(const struct HTree* root, void* string, bitpos_t* pos)
{
    size_t bits = 1;

    if (root->left == NULL)
    {
        write_bit(string, pos, 1);
        for (int i = 0; i < 8; ++i)
        {
            write_bit(string, pos, root->entry.symbol & (1 << (8 - 1 - i)));
        }
        bits += 8;
    }
    else
    {
        write_bit(string, pos, 0);
        bits += traverse_and_write_string(root->left, string, pos);
        bits += traverse_and_write_string(root->right, string, pos);
    }

    return bits;
}


size_t tree_string(const struct HTree* root, void* string, bitpos_t* pos)
{
    return traverse_and_write_string(root, string, pos);
}


size_t tree_size(const struct HTree* root)
{
    return root->entry.depth;
}


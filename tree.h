#ifndef TREE_H
#define TREE_H

#include "pes.h"

// Tree entry structure
typedef struct {
    uint32_t mode;
    ObjectID id;
    char name[256];
} TreeEntry;

typedef struct {
    TreeEntry *entries;
    size_t count;
    size_t capacity;
} Tree;

// Function Prototypes
int tree_from_index(ObjectID *id_out);
void tree_free(Tree *tree);

#endif // TREE_H

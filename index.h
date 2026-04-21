#ifndef INDEX_H
#define INDEX_H

#include "pes.h"
#include <time.h>

typedef struct {
    uint32_t mode;
    ObjectID id;
    time_t mtime;
    uint32_t size;
    char path[512];
} IndexEntry;

typedef struct {
    IndexEntry *entries;
    size_t count;
    size_t capacity;
} Index;

// Function Prototypes
int index_load(Index *index);
int index_save(const Index *index);
int index_add(Index *index, const char *path);
IndexEntry* index_find(const Index *index, const char *path);
int index_status(const Index *index);

#endif // INDEX_H

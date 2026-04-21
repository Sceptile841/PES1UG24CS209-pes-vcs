#include "tree.h"
#include "index.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tree_from_index(ObjectID *id_out) {
    Index index;
    if (index_load(&index) != 0) return -1;

    // Calculate buffer size for tree serialization
    // Format: <mode> <name>\0<hash_bytes>
    size_t buf_size = 0;
    for (size_t i = 0; i < index.count; i++) {
        buf_size += sizeof(uint32_t) + strlen(index.entries[i].path) + 1 + HASH_SIZE;
    }

    unsigned char *buffer = malloc(buf_size);
    unsigned char *ptr = buffer;

    for (size_t i = 0; i < index.count; i++) {
        // Write mode
        memcpy(ptr, &index.entries[i].mode, sizeof(uint32_t));
        ptr += sizeof(uint32_t);

        // Write name
        strcpy((char*)ptr, index.entries[i].path);
        ptr += strlen(index.entries[i].path) + 1;

        // Write hash bytes
        memcpy(ptr, index.entries[i].id.hash, HASH_SIZE);
        ptr += HASH_SIZE;
    }

    int ret = object_write(OBJ_TREE, buffer, buf_size, id_out);
    
    free(buffer);
    // index_free handles cleaning up the index structure
    return ret;
}

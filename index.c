#include "index.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int index_load(Index *index) {
    index->count = 0;
    index->capacity = 100;
    index->entries = malloc(index->capacity * sizeof(IndexEntry));

    FILE *f = fopen(INDEX_FILE, "r");
    if (!f) return 0; // Empty index is fine

    while (index->count < index->capacity) {
        IndexEntry *e = &index->entries[index->count];
        char hex[HASH_HEX_SIZE + 1];
        if (fscanf(f, "%o %64s %ld %u %511s\n", 
            &e->mode, hex, (long*)&e->mtime, &e->size, e->path) != 5) break;
        
        for (int i = 0; i < HASH_SIZE; i++) {
            sscanf(hex + (i * 2), "%02hhx", &e->id.hash[i]);
        }
        index->count++;
    }
    fclose(f);
    return 0;
}

int index_save(const Index *index) {
    FILE *f = fopen(INDEX_FILE ".tmp", "w");
    if (!f) return -1;

    for (size_t i = 0; i < index->count; i++) {
        IndexEntry *e = &index->entries[i];
        char hex[HASH_HEX_SIZE + 1];
        for (int j = 0; j < HASH_SIZE; j++) sprintf(hex + (j * 2), "%02x", e->id.hash[j]);
        
        fprintf(f, "%o %s %ld %u %s\n", e->mode, hex, (long)e->mtime, e->size, e->path);
    }
    fclose(f);
    rename(INDEX_FILE ".tmp", INDEX_FILE);
    return 0;
}

IndexEntry* index_find(const Index *index, const char *path) {
    for (size_t i = 0; i < index->count; i++) {
        if (strcmp(index->entries[i].path, path) == 0) return &index->entries[i];
    }
    return NULL;
}

int index_add(Index *index, const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return -1;

    FILE *f = fopen(path, "rb");
    if (!f) return -1;
    void *data = malloc(st.st_size);
    fread(data, 1, st.st_size, f);
    fclose(f);

    ObjectID id;
    if (object_write(OBJ_BLOB, data, st.st_size, &id) != 0) {
        free(data); return -1;
    }

    IndexEntry *e = index_find(index, path);
    if (!e) e = &index->entries[index->count++];
    
    e->mode = (st.st_mode & S_IXUSR) ? 0100755 : 0100644;
    e->id = id;
    e->mtime = st.st_mtime;
    e->size = st.st_size;
    strncpy(e->path, path, sizeof(e->path)-1);

    free(data);
    return index_save(index);
}

int index_status(const Index *index) {
    printf("Staged changes:\n");
    if (index->count == 0) printf("  (nothing to show)\n");
    for (size_t i = 0; i < index->count; i++) {
        printf("  staged:     %s\n", index->entries[i].path);
    }
    return 0;
}

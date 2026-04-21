#ifndef PES_H
#define PES_H

#include <stddef.h>
#include <stdint.h>

// Constants
#define HASH_SIZE 32
#define HASH_HEX_SIZE 64
#define PES_DIR ".pes"
#define OBJECTS_DIR ".pes/objects"
#define REFS_DIR ".pes/refs/heads"
#define HEAD_FILE ".pes/HEAD"
#define INDEX_FILE ".pes/index"

// Object Types
typedef enum {
    OBJ_BLOB,
    OBJ_TREE,
    OBJ_COMMIT
} ObjectType;

// Object ID structure (SHA-256 hash)
typedef struct {
    uint8_t hash[HASH_SIZE];
} ObjectID;

// Function Prototypes for Object Store (object.c)
void hash_to_hex(const ObjectID *id, char *hex_out);
int hex_to_hash(const char *hex, ObjectID *id_out);
void object_path(const ObjectID *id, char *path_out);
int object_write(ObjectType type, const void *data, size_t len, ObjectID *id_out);
int object_read(const ObjectID *id, ObjectType *type_out, void **data_out, size_t *len_out);

// Helper for Author info
const char* pes_author(void);

#endif // PES_H

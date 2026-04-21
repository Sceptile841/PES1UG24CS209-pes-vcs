#ifndef COMMIT_H
#define COMMIT_H

#include "pes.h"
#include <time.h>

typedef struct {
    ObjectID tree_id;
    ObjectID parent_id;
    int has_parent;
    char author[256];
    time_t timestamp;
    char message[1024];
} Commit;

// Function Prototypes
int commit_create(const char *message, ObjectID *id_out);
int commit_walk(void (*callback)(const ObjectID *id, const Commit *commit, void *ctx), void *ctx);

// Ref helpers
int head_read(ObjectID *id_out);
int head_update(const ObjectID *new_commit);

#endif // COMMIT_H

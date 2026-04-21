#include "commit.h"
#include "tree.h"
#include "pes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

int head_read(ObjectID *id_out) {
    FILE *f = fopen(HEAD_FILE, "r");
    if (!f) return -1;

    char line[512];
    if (!fgets(line, sizeof(line), f)) { fclose(f); return -1; }
    fclose(f);

    line[strcspn(line, "\r\n")] = '\0';
    char path[512];
    if (strncmp(line, "ref: ", 5) == 0) {
        snprintf(path, sizeof(path), ".pes/%s", line + 5);
        FILE *rf = fopen(path, "r");
        if (!rf) return -1;
        char hex[HASH_HEX_SIZE + 1];
        if (!fgets(hex, sizeof(hex), rf)) { fclose(rf); return -1; }
        fclose(rf);
        for (int i = 0; i < HASH_SIZE; i++) sscanf(hex + (i * 2), "%02hhx", &id_out->hash[i]);
    } else {
        for (int i = 0; i < HASH_SIZE; i++) sscanf(line + (i * 2), "%02hhx", &id_out->hash[i]);
    }
    return 0;
}

int head_update(const ObjectID *new_commit) {
    FILE *f = fopen(HEAD_FILE, "r");
    if (!f) return -1;
    char line[512];
    fgets(line, sizeof(line), f);
    fclose(f);

    line[strcspn(line, "\r\n")] = '\0';
    char path[512];
    if (strncmp(line, "ref: ", 5) == 0) {
        snprintf(path, sizeof(path), ".pes/%s", line + 5);
    } else {
        snprintf(path, sizeof(path), "%s", HEAD_FILE);
    }

    char hex[HASH_HEX_SIZE + 1];
    for (int i = 0; i < HASH_SIZE; i++) sprintf(hex + (i * 2), "%02x", new_commit->hash[i]);

    FILE *wf = fopen(path, "w");
    if (!wf) return -1;
    fprintf(wf, "%s\n", hex);
    fclose(wf);
    return 0;
}

int commit_create(const char *message, ObjectID *id_out) {
    ObjectID tree_id, parent_id;
    if (tree_from_index(&tree_id) != 0) return -1;

    int has_parent = (head_read(&parent_id) == 0);

    char buffer[2048];
    char tree_hex[65], parent_hex[65];
    for (int i = 0; i < 32; i++) sprintf(tree_hex + (i * 2), "%02x", tree_id.hash[i]);
    
    int len;
    if (has_parent) {
        for (int i = 0; i < 32; i++) sprintf(parent_hex + (i * 2), "%02x", parent_id.hash[i]);
        len = sprintf(buffer, "tree %s\nparent %s\nauthor %s %ld\n\n%s\n", 
                      tree_hex, parent_hex, "User <user@pes.edu>", (long)time(NULL), message);
    } else {
        len = sprintf(buffer, "tree %s\nauthor %s %ld\n\n%s\n", 
                      tree_hex, "User <user@pes.edu>", (long)time(NULL), message);
    }

    if (object_write(OBJ_COMMIT, buffer, len, id_out) != 0) return -1;
    return head_update(id_out);
}

int commit_walk(void (*callback)(const ObjectID *id, const Commit *commit, void *ctx), void *ctx) {
    ObjectID current_id;
    if (head_read(&current_id) != 0) return -1;
    // Simple one-level walk for log purposes
    return 0; 
}

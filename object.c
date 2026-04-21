#include "pes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/evp.h>

// Helper to convert hash bytes to hex string
void hash_to_hex(const ObjectID *id, char *hex_out) {
    for (int i = 0; i < HASH_SIZE; i++) {
        sprintf(hex_out + (i * 2), "%02x", id->hash[i]);
    }
    hex_out[HASH_HEX_SIZE] = '\0';
}

// Helper to get the file path for an object
void object_path(const ObjectID *id, char *path_out) {
    char hex[HASH_HEX_SIZE + 1];
    hash_to_hex(id, hex);
    sprintf(path_out, "%s/%.2s/%s", OBJECTS_DIR, hex, hex + 2);
}

int object_write(ObjectType type, const void *data, size_t len, ObjectID *id_out) {
    char header[128];
    const char* type_str = (type == OBJ_BLOB) ? "blob" : (type == OBJ_TREE ? "tree" : "commit");
    int hdr_len = snprintf(header, sizeof(header), "%s %zu", type_str, len) + 1;

    // Compute SHA-256 Hash
    unsigned int hash_len;
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, header, hdr_len);
    EVP_DigestUpdate(ctx, data, len);
    EVP_DigestFinal_ex(ctx, id_out->hash, &hash_len);
    EVP_MD_CTX_free(ctx);

    char path[512], dir[512];
    object_path(id_out, path);
    
    // Create directory
    strncpy(dir, path, 512);
    char *last_slash = strrchr(dir, '/');
    if (last_slash) *last_slash = '\0';
    
    mkdir(OBJECTS_DIR, 0755);
    mkdir(dir, 0755);

    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    fwrite(header, 1, hdr_len, f);
    fwrite(data, 1, len, f);
    fclose(f);
    return 0;
}

int object_read(const ObjectID *id, ObjectType *type_out, void **data_out, size_t *len_out) {
    char path[512];
    object_path(id, path);
    
    FILE *f = fopen(path, "rb");
    if (!f) return -1;

    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    rewind(f);

    unsigned char *full_data = malloc(file_size);
    if (fread(full_data, 1, file_size, f) != file_size) {
        free(full_data);
        fclose(f);
        return -1;
    }
    fclose(f);

    char *null_byte = memchr(full_data, '\0', file_size);
    if (!null_byte) { free(full_data); return -1; }
    
    size_t hdr_len = (null_byte - (char*)full_data) + 1;
    *len_out = file_size - hdr_len;
    
    if (strncmp((char*)full_data, "blob", 4) == 0) *type_out = OBJ_BLOB;
    else if (strncmp((char*)full_data, "tree", 4) == 0) *type_out = OBJ_TREE;
    else *type_out = OBJ_COMMIT;

    *data_out = malloc(*len_out);
    memcpy(*data_out, null_byte + 1, *len_out);
    
    free(full_data);
    return 0;
}

// tt.c
#include "tt.h"
#include <stdlib.h>
#include <string.h>

static TTEntry* table;

void tt_init() {
    table = calloc(TT_SIZE, sizeof(TTEntry));
}

static inline size_t idx(uint64_t key) {
    return (key ^ (key >> 20)) & (TT_SIZE - 1);
}

TTEntry* tt_probe(uint64_t key) {
    TTEntry* e = &table[idx(key)];
    if (e->key == key) return e;
    return NULL;
}

void tt_store(uint64_t key, int depth, TTFlag flag, int value) {
    TTEntry* e = &table[idx(key)];
    // Remplace si plus profond ou case vide
    if (e->key != key || depth >= e->depth) {
        e->key   = key;
        e->depth = depth;
        e->flag  = flag;
        e->value = value;
    }
}

#ifndef TT_H
#define TT_H

#include <stdint.h>

#define TT_SIZE (1<<20)  // ajuste selon ta RAM (~1M entrées)

typedef enum { EXACT, LOWERBOUND, UPPERBOUND } TTFlag;

typedef struct {
    uint64_t key;
    int depth;
    TTFlag flag;
    int value;
} TTEntry;

void       tt_init();
TTEntry*   tt_probe(uint64_t key);
void       tt_store(uint64_t key, int depth, TTFlag flag, int value);

#endif

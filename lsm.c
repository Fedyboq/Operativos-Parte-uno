//
// Created by Federico on 14/09/2026.
//
#ifndef LSM_C
#define LSM_C

#include "hook.c"
#include <stdlib.h>

typedef struct {
    unsigned int active_hooks;
    hook* hooks;
} LSM;

void register_module(LSM* lsm, hook* h) {
    int sz = lsm->active_hooks + 1;
    hook* new_hooks = realloc(lsm->hooks, sizeof(hook) * sz);
    if (new_hooks) {
        new_hooks[sz - 1] = *h;
        lsm->active_hooks = sz;
        lsm->hooks = new_hooks;
    }
}



#endif
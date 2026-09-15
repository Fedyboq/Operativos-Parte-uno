//
// Created by Federico on 14/09/2026.
//
#ifndef HOOK_H
#define HOOK_H

#include "models.c"

typedef int (*open_hook_fn)(task_struct *task, inode *ino, int mask);

typedef struct {
    unsigned short priority;
    open_hook_fn fun;
} hook;

void init_hook(hook* h, unsigned short pr, open_hook_fn fun) {
    h->priority = pr;
    h->fun = fun;
}

#endif
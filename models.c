//
// Created by Federico on 14/09/2026.
//
#ifndef MODELS_H
#define MODELS_H

typedef struct {
    int mode;
    void *security;
} inode;

typedef struct {
    int uid;
    void *security;
} task_struct;

#endif
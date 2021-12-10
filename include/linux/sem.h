#ifndef _SEM_H
#define _SEM_H

#include <linux/sched.h>

#define MAX_NAME 20
#define SEM_TABLE_SIZE 20

typedef struct sem {
    char name[MAX_NAME];
    unsigned int value;
    struct task_struct* wait_queue;
} sem;

extern sem sem_table[SEM_TABLE_SIZE];

#endif

//
// Created by 52750 on 2021/12/10.
//
#include <linux/sem.h>
#include <linux/sched.h>
#include <unistd.h>
#include <asm/segment.h>
#include <linux/tty.h>
#include <linux/kernel.h>
#include <linux/fdreg.h>
#include <asm/system.h>
#include <asm/io.h>


sem sem_table[SEM_TABLE_SIZE];
int t_index = 0;

sem* sys_sem_open(const char* name, unsigned int value) {
    char kernel_name[20];
    int name_cnt = 0;
    while (get_fs_byte(name + name_cnt) != '\0') {
        name_cnt++;
    }
    if (name_cnt > MAX_NAME) {
        return NULL;
    }
    int i = 0;
    for (i = 0; i < name_cnt; i++) {
        kernel_name[i] = get_fs_byte(name + i);
    }
    int name_len = strlen(kernel_name);
    int exist = 0;
    for (i = 0; i < t_index; i++) {
        if (strlen(sem_table[i].name) == name_len && !strcmp(kernel_name, sem_table[i].name)) {
            exist = 1;
            break;
        }
    }

    if (exist) {
        return (sem*) (&sem_table[i]);
    }
    for (i = 0; i < name_len; i++)
    {
        sem_table[t_index].name[i] = kernel_name[i];
    }
    sem_table[t_index].value = value;
    return (sem*) (&sem_table[t_index++]);
}

int sys_sem_wait(sem* s) {
    cli();
    while ((s->value) <= 0) {
        sleep_on(&(s->wait_queue));
    }
    s->value--;
    sti();
    return 0;
}

int sys_sem_post(sem* s) {
    cli();
    s->value++;
    if ((s->value) <= 1) {
        wake_up(&(s->wait_queue));
    }
    sti();
    return 0;
}

int sys_sem_unlink(const char* name) {
    char kernel_name[20];
    int name_cnt = 0;
    while (get_fs_byte(name + name_cnt) != '\0') {
        name_cnt++;
    }
    if (name_cnt > MAX_NAME) {
        return -1;
    }
    int i = 0;
    for (i = 0; i < name_cnt; i++) {
        kernel_name[i] = get_fs_byte(name + i);
    }
    int name_len = strlen(kernel_name);
    int exist = 0;
    for (i = 0; i < t_index; i++) {
        if (strlen(sem_table[i].name) == name_len && !strcmp(kernel_name, sem_table[i].name)) {
            exist = 1;
            break;
        }
    }
    if (exist) {
        int tmp_index = i;
        for (tmp_index = i; tmp_index < t_index; tmp_index++) {
            sem_table[tmp_index] = sem_table[tmp_index + 1];
        }
        t_index--;
        return 0;
    }
    return -1;
}
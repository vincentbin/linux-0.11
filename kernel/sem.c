//
// Created by 52750 on 2021/12/10.
//

#include <linux/sem.h>
#include <string.h>             /* strcpy()  strcmp() */
#include <asm/segment.h>        /* get_fs_byte() */
#include <unistd.h>             /* NULL */
#include <asm/system.h>         /* cli()  sti() */
#include <linux/kernel.h>


sem sem_table[SEM_TABLE_SIZE];
int index = 0;

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
    for (i = 0; i < index; i++) {
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
        sem_table[index].name[i] = kernelname[i];
    }
    sem_table[index].value = value;
    return (sem*) (&sem_table[index++]);
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
        return NULL;
    }
    int i = 0;
    for (i = 0; i < name_cnt; i++) {
        kernel_name[i] = get_fs_byte(name + i);
    }
    int name_len = strlen(kernel_name);
    int exist = 0;
    for (i = 0; i < index; i++) {
        if (strlen(sem_table[i].name) == name_len && !strcmp(kernel_name, sem_table[i].name)) {
            exist = 1;
            break;
        }
    }
    if (exist) {
        int t_index = i;
        for (t_index = i; t_index < cnt; t_index++) {
            sem_table[t_index] = sem_table[t_index + 1];
        }
        index--;
        return 0;
    }
    return -1;
}
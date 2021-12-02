//
// Created by 52750 on 2021/12/2.
//
#include <errno.h>
#include <string.h>
#include <asm/segment.h> // get_fs_byte put_fs_byte

char __username[24];
int __length;

/**
 * 在内核空间取用户空间的数据 需要用get_fs_byte
 * @param name
 * @return
 */
int sys_iam(const char *name) {
    int name_len = 0;
    while (get_fs_byte(&name[name_len]) != '\0') {
        name_len+;
    }
    if (name_len > 23) {
        return -EINVAL;
    }
    int i = 0;
    for (; i < name_len; i++) {
        __username[i] = get_fs_byte(&name[i]);
    }
    __username[i] = '\0';
    __length = name_len;
    return __length;
}

/**
 * 复制内核数据到 name
 * @param name
 * @param size
 * @return
 */
int sys_whoami(char *name, int size) {
    if (size < __length) {
        return -EINVAL;
    }
    int i = 0;
    for (; i < __length; i++) {
        put_fs_byte(__username[i], &name[i]);
    }
    return __length;
}

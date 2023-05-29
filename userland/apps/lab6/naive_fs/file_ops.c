#include <string.h>
#include <stdio.h>

#include "file_ops.h"
#include "block_layer.h"

// define file system constants
#define MAX_FILE_NAME_LEN 32
#define MAX_FILE_NUM 128
#define MAX_FILE_SIZE (BLOCK_SIZE * 128)
#define BLOCK_NUM 128 * 128

int block_bitmap[BLOCK_NUM] = {0};

struct naive_fs_file {
    char name[MAX_FILE_NAME_LEN];
    int size;
    int block_num;
    int block_list[128];
};

struct naive_fs {
    struct naive_fs_file files[MAX_FILE_NUM];
    int file_num;
};

struct naive_fs naive_fs;


int block_get(int block_num) {
    return block_bitmap[block_num];
}


void block_set(int block_num) {
    block_bitmap[block_num] = 1;
}


void block_free(int block_num) {
    block_bitmap[block_num] = 0;
}


struct naive_fs_file *naive_fs_find_file(const char *name) {
    for (int i = 0; i < naive_fs.file_num; i++) {
        if (strcmp(naive_fs.files[i].name, name) == 0) {
            return &naive_fs.files[i];
        }
    }
    return NULL;
}

int naive_fs_alloc_block() {
    for (int i = 1; i < BLOCK_NUM; i++) {
        if (block_get(i) == 0) {
            block_set(i);
            return i;
        }
    }
    return -1;
}

int naive_fs_access(const char *name)
{
    /* LAB 6 TODO BEGIN */
    /* BLANK BEGIN */

    struct naive_fs_file *file = naive_fs_find_file(name);
    if (file == NULL) {
        return -1;
    }
    return 0;

    /* BLANK END */
    /* LAB 6 TODO END */
    return -2;
}

int naive_fs_creat(const char *name)
{
    /* LAB 6 TODO BEGIN */
    /* BLANK BEGIN */

    if (naive_fs_find_file(name) != NULL) {
        return -1;
    }
    if (naive_fs.file_num >= MAX_FILE_NUM) {
        return -1;
    }
    struct naive_fs_file *file = &naive_fs.files[naive_fs.file_num];
    strncpy(file->name, name, MAX_FILE_NAME_LEN);
    file->size = 0;
    file->block_num = 0;
    naive_fs.file_num++;
    return 0;

    /* BLANK END */
    /* LAB 6 TODO END */
    return -2;
}

int naive_fs_pread(const char *name, int offset, int size, char *buffer)
{
    /* LAB 6 TODO BEGIN */
    /* BLANK BEGIN */

    char *tmpbuf = malloc(size + 2 * BLOCK_SIZE);
    struct naive_fs_file *file = naive_fs_find_file(name);
    if (file == NULL) {
        return -1;
    }
    int block_begin_no = offset / BLOCK_SIZE;
    int block_end_no = (offset + size - 1) / BLOCK_SIZE;
    int block_offset = offset % BLOCK_SIZE;
    int total_size = 0;
    
    for (int i = block_begin_no; i <= block_end_no; ++i) {
        if (file->block_list[i] == 0) {
            return -1;
        }
        sd_bread(file->block_list[i], tmpbuf + total_size);
        total_size += BLOCK_SIZE;
    }
    for (int i = 0; i < size; ++i) {
        buffer[i] = tmpbuf[block_offset + i];
    }
    return size;

    /* BLANK END */
    /* LAB 6 TODO END */
    return -2;
}

int naive_fs_pwrite(const char *name, int offset, int size, const char *buffer)
{
    /* LAB 6 TODO BEGIN */
    /* BLANK BEGIN */

    char *tmpbuf = malloc(size + 2 * BLOCK_SIZE);
    struct naive_fs_file *file = naive_fs_find_file(name);
    if (file == NULL) {
        return -1;
    }
    if (offset + size > MAX_FILE_SIZE) {
        return -1;
    }


    int block_begin_no = offset / BLOCK_SIZE;
    int block_end_no = (offset + size - 1) / BLOCK_SIZE;
    int block_offset = offset % BLOCK_SIZE;
    int total_size = 0;

    for (int i = 0; i < size; ++i) {
        tmpbuf[block_offset + i] = buffer[i];
    }
    
    for (int i = block_begin_no; i <= block_end_no; ++i) {
        if (file->block_list[i] == 0) {
            file->block_list[i] = naive_fs_alloc_block();
        }
        sd_bwrite(file->block_list[i], tmpbuf + total_size);
        total_size += BLOCK_SIZE;
    }
    return size;

    /* BLANK END */
    /* LAB 6 TODO END */
    return -2;
}

int naive_fs_unlink(const char *name)
{
    /* LAB 6 TODO BEGIN */
    /* BLANK BEGIN */

    struct naive_fs_file *file = naive_fs_find_file(name);
    if (file == NULL) {
        return -1;
    }
    for (int i = 0; i < file->block_num; i++) {
        block_free(file->block_list[i]);
        file->block_list[i] = 0;
    }
    memset(file, 0, sizeof(struct naive_fs_file));
    naive_fs.file_num--;
    return 0;

    /* BLANK END */
    /* LAB 6 TODO END */
    return -2;
}
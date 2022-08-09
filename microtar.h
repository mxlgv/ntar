/*
 * Copyright (c) 2017 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `microtar.c` for details.
 */

#ifndef MICROTAR_H
#define MICROTAR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#define MTAR_VERSION   "0.1.1"
#define MTAR_FNAME_MAX 100

#if !defined(UINT_MAX) || !defined(UINT16_MAX)
#error "UINT_MAX and UINT16_MAX constants not declared!"
#endif

#if UINT_MAX == UINT16_MAX
#define MTAR_FMT_OCT_U32 "%lo"
#else
#define MTAR_FMT_OCT_U32 "%o"
#endif

enum {
    MTAR_ESUCCESS = 0,
    MTAR_EFAILURE = -1,
    MTAR_EOPENFAIL = -2,
    MTAR_EREADFAIL = -3,
    MTAR_EWRITEFAIL = -4,
    MTAR_ESEEKFAIL = -5,
    MTAR_EBADCHKSUM = -6,
    MTAR_ENULLRECORD = -7,
    MTAR_ENOTFOUND = -8
};

enum {
    MTAR_TREG = '0',
    MTAR_TLNK = '1',
    MTAR_TSYM = '2',
    MTAR_TCHR = '3',
    MTAR_TBLK = '4',
    MTAR_TDIR = '5',
    MTAR_TFIFO = '6'
};

typedef struct {
    uint32_t mode;
    uint32_t owner;
    size_t size;
    uint32_t mtime;
    uint32_t type;
    char name[MTAR_FNAME_MAX];
    char linkname[MTAR_FNAME_MAX];
} mtar_header_t;

typedef struct mtar_t mtar_t;

struct mtar_t {
    int (*read)(mtar_t *tar, void *data, size_t size);
    int (*write)(mtar_t *tar, const void *data, size_t size);
    int (*seek)(mtar_t *tar, size_t pos);
    int (*close)(mtar_t *tar);
    void *stream;
    size_t pos;
    size_t remaining_data;
    size_t last_header;
};

extern const char *MTAR_FMODE_W;
extern const char *MTAR_FMODE_R;
extern const char *MTAR_FMODE_W_PLUS;
extern const char *MTAR_FMODE_A_PLUS;

const char *mtar_strerror(int err);

int mtar_open(mtar_t *tar, const char *filename, const char *mode);
int mtar_close(mtar_t *tar);

int mtar_seek(mtar_t *tar, size_t pos);
int mtar_rewind(mtar_t *tar);
int mtar_next(mtar_t *tar);
int mtar_find(mtar_t *tar, const char *name, mtar_header_t *h);
int mtar_read_header(mtar_t *tar, mtar_header_t *h);
int mtar_read_data(mtar_t *tar, void *ptr, size_t size);

int mtar_write_header(mtar_t *tar, const mtar_header_t *h);
int mtar_write_file_header(mtar_t *tar, const char *name, size_t size);
int mtar_write_dir_header(mtar_t *tar, const char *name);
int mtar_write_data(mtar_t *tar, const void *data, size_t size);
int mtar_finalize(mtar_t *tar);

#ifdef __cplusplus
}
#endif

#endif

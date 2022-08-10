#ifndef _OS_H_
#define _OS_H_

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#if defined(_unix) || defined(__unix) || defined(__unix__) || defined(__CYGWIN__)
#define OS_UNIX
#elif defined(_WIN32) || defined(WIN32) || defined(WIN64) || defined(_WIN64) || defined(__MINGW32__) || defined(__MINGW64__)
#define OS_WINDOWS
#elif defined(_DOS) || defined(DOS)
#define OS_DOS
#elif defined(_KOLIBRI) || defined(KOLIBRI) || defined(KOS) || defined(_KOS)
#define OS_KOLIBRI
#else
#error "The platform is not supported. Implement functions for your platform in os.c"
#endif

#ifdef OS_KOLIBRI
#include <conio.h>
#define OS_CON_SET_TITLE(title) \
    con_init();                 \
    (*con_set_title)(title)
#define OS_CON_EXIT(status) (*con_exit)(status)
#else
#define OS_CON_SET_TITLE(title)
#define OS_CON_EXIT(status)
#endif

#define OS_PATH_MAX 4096

typedef struct {
    bool is_dir;
    size_t size;
    time_t mtime;
} os_stat_t;

#if defined(OS_WINDOWS) || defined(OS_DOS)
char *dirname(char *path);
#else
#include <libgen.h>
#endif

bool os_stat(const char *path, os_stat_t *info);
bool os_mkdir(const char *path);

#endif

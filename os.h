#ifndef _OS_H_
#define _OS_H_

#include <stdbool.h>
#include <stddef.h>

#if defined(_unix) || defined(__unix) || defined(__unix__) || defined(__CYGWIN__)
#define OS_UNIX
#elif defined(_WIN32) || defined(WIN32) || defined(WIN64) || defined(_WIN64) || defined(__MINGW32__) || defined(__MINGW64__)
#define OS_WINDOWS
#elif defined(_KOLIBRI) || defined(KOLIBRI)
#define OS_KOLIBRI
#else
#error "The platform is not supported. Implement functions for your platform in os.c"
#endif

#ifndef OS_WINDOWS
#include <libgen.h>
#else
extern char *dirname(char *path);
#endif

#ifdef OS_KOLIBRI
#include <conio.h>
#define OS_CON_SET_TITLE(title) con_init(); (*con_set_title)(title)
#define OS_CON_EXIT(status) (*con_exit)(status)
#else
#define OS_CON_SET_TITLE(title)
#define OS_CON_EXIT(status)
#endif

#define OS_PATH_MAX 4096

size_t os_get_fsize(const char *fname);
bool os_is_dir(const char *name);
bool os_exist(const char *name);
bool os_mkdir(const char *name);

#endif

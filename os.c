#include "os.h"

#if defined(OS_WINDOWS) || defined(OS_UNIX)

#include <sys/stat.h>

size_t os_get_fsize(const char *fname)
{
    struct stat finfo;
    if (stat(fname, &finfo) == -1) {
        return 0;
    }
    return finfo.st_size;
}

bool os_is_dir(const char *name)
{
    struct stat info;
    if (stat(name, &info) == -1) {
        return false;
    }
    if (info.st_mode & S_IFDIR) {
        return true;
    }
    return false;
}

bool os_mkdir(const char *name)
{
    struct stat info;
    if (!stat(name, &info)) {
        return true;
    }
#ifdef OS_WINDOWS
    if (mkdir(name) == -1) {
#else
    if (mkdir(name, 0777) == -1) {
#endif
        return false;
    }
    return true;
}

#endif

/* clang-format off */

#ifdef OS_WINDOWS
char *dirname(char *s) /* Adaptation from musl libc for windows  */
{
    size_t i;
    if (!s || !*s) return ".";
    i = strlen(s)-1;
    for (; s[i]=='\\'; i--) if (!i) return "\\";
    for (; s[i]!='\\'; i--) if (!i) return ".";
    for (; s[i]=='\\'; i--) if (!i) return "\\";
    s[i+1] = 0;
    return s;
}
#endif

/* clang-format on */

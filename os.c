#include "os.h"

#if defined _unix || __unix || __unix__ || __CYGWIN__
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

bool os_exist(const char *name)
{
    struct stat info;
    if (!stat(name, &info)) {
        return true;
    }
    return false;
}

bool os_mkdir(const char *name)
{
    if (mkdir(name, 777) == -1) {
        return false;
    }
    return true;
}

#else
#error "The platform is not supported. Implement functions for your platform in os.c"
#endif
#include "os.h"

#ifndef OS_KOLIBRI
#include <sys/stat.h>

bool os_stat(const char *path, os_stat_t *info)
{
    struct stat _info;
    info->is_dir = true;

    if (stat(path, &_info) == -1) {
        return false;
    }

    info->is_dir = _info.st_mode & S_IFDIR;
    info->size = _info.st_size;
    info->mtime = _info.st_mtime;
    return true;
}

bool os_mkdir(const char *path)
{
    os_stat_t info;
    if (os_stat(path, &info)) {
        return true;
    }
#if defined(OS_WINDOWS) || defined(OS_DOS)
    if (mkdir(path) == -1) {
#else
    if (mkdir(path, 0777) == -1) {
#endif
        return false;
    }
    return true;
}

#else
#include <sys/ksys.h>
#define KSYS_IS_DIR 0x10

/* !FIXME! Overriding the structure from ksys.h. */
typedef struct {
    uint32_t attr;
    uint32_t name_enc;
    ksys_time_t ctime;
    ksys_date_t cdate;
    ksys_time_t atime;
    ksys_date_t adate;
    ksys_time_t mtime;
    ksys_date_t mdate;
    uint64_t size;
    char name[0];
} ksys_bdfe_t;

bool os_stat(const char *path, os_stat_t *info)
{
    ksys_bdfe_t bdfe;
    if (_ksys_file_info(path, &bdfe)) {
        return false;
    }

    struct tm t;
    t.tm_hour = bdfe.mtime.hour;
    t.tm_min = bdfe.mtime.min;
    t.tm_sec = bdfe.mtime.sec;

    t.tm_year = bdfe.mdate.year;
    t.tm_mon = bdfe.mdate.month;
    t.tm_mday = bdfe.mdate.day;

    info->is_dir = bdfe.attr & KSYS_IS_DIR;
    info->size = bdfe.size;
    info->mtime = mktime(&t);

    return true;
}

bool os_mkdir(const char *name)
{
    return !_ksys_mkdir(name);
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

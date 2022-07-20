#ifndef _OS_H_
#define _OS_H_

#include <stdbool.h>
#include <stddef.h>

size_t os_get_fsize(const char *fname);
bool os_is_dir(const char *name);
bool os_exist(const char *name);
bool os_mkdir(const char *name);

#endif
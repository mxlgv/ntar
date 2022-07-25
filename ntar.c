/* Standard headers C99 */
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Platform-specific header */
#include "os.h"

/* Microtar library header */
#include "microtar.h"

#define BLOCK_LEN       (1024 * 4) // 4 KB
#define IFARG(str, dop) if (!strcmp(argv[1], (str)) && dop)

typedef struct {
    char **names;
    size_t fnum;
} ntar_flist_t;

typedef enum {
    NTAR_LIST,
    NTAR_EXTRACT,
} ntar_content_act_t;

const char* NO_MEM_MSG = "Not enough memory";
const char* UNABLE_TO_OPEN_TAR_MSG = "Unable to open tar '%s'! %s\n";
const char* UNABLE_TO_OPEN_MSG = "Unable to open file '%s!'\n";

const char* FMODE_RB = "rb";
const char* FMODE_WB = "wb";
const char* FMODE_AB = "ab";

bool mkdir_parent(const char *full_path)
{
    const char* SEP = "\\/";
    bool status = true;

    char *tmp_str = calloc(OS_PATH_MAX, sizeof(char));
    char *full_path_dup = strdup(full_path);

    if (!full_path_dup || !tmp_str) {
        return false;
    }
#if 0
    if (full_path[0] == '/' || full_path[0] == '\\') {
        tmp_str[0] = '\0';
    } else {
        strcpy(tmp_str, ".");
    }
#endif
    char *pch = strtok(full_path_dup, SEP);
    while (pch != NULL) {
        strcat(tmp_str, "/");
        strcat(tmp_str, pch);
        puts(tmp_str);
        if (!os_mkdir(tmp_str)) {
            status = false;
            break;
        }
        pch = strtok(NULL, SEP);
    }

    free(tmp_str);
    free(full_path_dup);
    return status;
}

void ntar_list(mtar_t *tar, mtar_header_t *header)
{
    if (header->type == MTAR_TDIR) {
        printf("Folder: %s\n", header->name);
    } else {
        printf("File:   %s (%d bytes)\n", header->name, header->size);
    }
}

void ntar_extract(mtar_t *tar, mtar_header_t *header)
{
    char *full_name = strdup(header->name);
    char *dir_name = header->name;
    if (header->type == MTAR_TREG) {
        dir_name = dirname(full_name);
    }

    if (!os_mkdir(dir_name)) {
        printf("Error creating folder '%s'!\n", header->name);
        goto exit;
    }

    if (header->type == MTAR_TREG) {
        FILE *fd = fopen(header->name, "wb");
        if (!fd) {
            printf(UNABLE_TO_OPEN_MSG, header->name);
            goto exit;
        }

        size_t new_size = 0;
        size_t readed = 0;

        void *data = malloc(BLOCK_LEN);
        if (!data) {
            puts(NO_MEM_MSG);
            goto exit;
        }

        while (readed < header->size) {
            new_size = header->size - readed;
            if (new_size >= BLOCK_LEN) {
                new_size = BLOCK_LEN;
            }

            int rd_st = mtar_read_data(tar, data, new_size);
            if (rd_st) {
                printf("Error reading file '%s' from tar! %s", header->name, mtar_strerror(rd_st));
                break;
            }
            if (fwrite(data, new_size, 1, fd) != 1) {
                printf("Error writing to file '%s'!\n", header->name);
                break;
            }
            readed += new_size;
        }
        free(data);
    }

    printf("Extracted %s\n", header->name);
exit:
    free(full_name);
}

int ntar_work_content(const char *tar_fname, ntar_flist_t flist, ntar_content_act_t act)
{
    mtar_t tar;
    mtar_header_t tar_header;
    int status = MTAR_ESUCCESS;

    status = mtar_open(&tar, tar_fname, FMODE_RB);
    if (status) {
        printf(UNABLE_TO_OPEN_TAR_MSG, tar_fname, mtar_strerror(status));
        return status;
    }

    if (flist.fnum > 0) {
        for (size_t i = 0; i < flist.fnum; i++) {
            status = mtar_find(&tar, flist.names[i], &tar_header);
            if (status != MTAR_ESUCCESS) {
                printf("Error extracting file '%s'! %s\n", flist.names[i], mtar_strerror(status));
                goto close;
            }
            ntar_extract(&tar, &tar_header);
        }
        goto close;
    }

    void (*action_fn)(mtar_t * tar, mtar_header_t * header);
    if (act == NTAR_EXTRACT) {
        action_fn = ntar_extract;
    } else {
        action_fn = ntar_list;
    }

    while (!(status = mtar_read_header(&tar, &tar_header))) {
        if (status != MTAR_ENULLRECORD && status) {
            printf("Error: %s!\n", mtar_strerror(status));
            break;
        }
        (*action_fn)(&tar, &tar_header);
        mtar_next(&tar);
    }

close:
    mtar_close(&tar);
    return status;
}

int ntar_add_files(const char *tar_fname, bool new_tar, ntar_flist_t flist)
{
    mtar_t tar;
    mtar_header_t tar_header;
    int tar_err_h = 0;
    int tar_err_d = 0;

    const char *mode = FMODE_WB;

    if (!new_tar) {
        mode = FMODE_AB;
    }

    int status = mtar_open(&tar, tar_fname, mode);
    if (status) {
        printf(UNABLE_TO_OPEN_TAR_MSG, tar_fname, mtar_strerror(status));
        return status;
    }

    for (size_t i = 0; i < flist.fnum; i++) {
        size_t size = 0;

        if (os_is_dir(flist.names[i])) {
            tar_err_h = mtar_write_dir_header(&tar, flist.names[i]);
        } else {
            FILE *fd = fopen(flist.names[i], FMODE_RB);
            if (!fd) {
                printf(UNABLE_TO_OPEN_MSG, flist.names[i]);
                continue;
            }

            size = os_get_fsize(flist.names[i]);
            tar_err_h = mtar_write_file_header(&tar, flist.names[i], size);

            while (!feof(fd)) {
                void *file_data = malloc(BLOCK_LEN);
                if (!file_data) {
                    puts(NO_MEM_MSG);
                    break;
                }
                size_t rsize = fread(file_data, 1, BLOCK_LEN, fd);
                tar_err_d = mtar_write_data(&tar, file_data, rsize);
                free(file_data);
            }
            fclose(fd);
        }

        if (tar_err_h || tar_err_d) {
            printf("Not added '%s'!\n", flist.names[i]);
        } else {
            printf("Added '%s' (%ld bytes)\n", flist.names[i], size);
        }
    }
    mtar_close(&tar);
    return status;
}

void show_help(void)
{
    printf(
        "     _\n"
        " _ _| |_ __ _ _ _   ntar (NANO tar) - v0.9 \n"
        "| ' \\  _/ _` | '_|  License: GPL-2.0-or-later\n"
        "|_||_\\__\\__,_|_|    Author - Maxim Logaev (2022 year)\n"
        "\n"
        "Usage: ntar [options] [tar-file] [files ...]\n"
        "Examples:\n"
        "   ntar -c test.tar foo bar  # Create archive.tar from files foo and bar."
        "\n"
        "   ntar -l test.tar          # List all files in test.tar.\n"
        "   ntar -x test.tar          # Extract all files from test.tar.\n"
        "   ntar -x test.tar foo bar  # Extract foo and bar from test.tar.\n"
        "\n"
        "Options:\n"
        "   -c                        Create tar file.\n"
        "   -a                        Add files to archive.\n"
        "   -l                        Show all files from archive.\n"
        "   -x                        Extract files.\n\n");
}

/* clang-format off */


int main(int argc, char **argv)
{
    int status = MTAR_ESUCCESS;
    if (argc < 3) {
        show_help();
        return status;
    }

    char *tar_fname = argv[2];

    ntar_flist_t flist_in;
    flist_in.names = &argv[3];
    flist_in.fnum = argc - 3;
    
    IFARG("-h", true) 
        show_help();
    else IFARG("-x", argc >= 3) 
        status = ntar_work_content(tar_fname, flist_in, NTAR_EXTRACT);
    else IFARG("-l", argc == 3)
        status = ntar_work_content(tar_fname, flist_in, NTAR_LIST);
    else IFARG("-c", argc > 3)
        status = ntar_add_files(tar_fname, true, flist_in);
    else IFARG("-a", argc > 3)
        status = ntar_add_files(tar_fname, false, flist_in);
    else
        puts("Use '-h' for help.");
    return EXIT_SUCCESS;
}

/* clang-format on */

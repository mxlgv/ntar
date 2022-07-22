/* Standard headers C99 */
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libgen.h>

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

bool mkdir_parent(const char *full_path)
{
    bool status = true;

    char *temp = malloc(strlen(full_path) + 3);
    char *new = strdup(full_path);

    if (!new || !temp) {
        return false;
    }

    if (full_path[0] == '/' || full_path[0] == '\\') {
        temp[0] = '\0';
    } else {
        strcpy(temp, ".");
    }

    char *pch = strtok(new, "\\/");
    while (pch != NULL) {
        strcat(temp, "/");
        strcat(temp, pch);
        if (!os_exist(full_path)) {
            if (os_mkdir(temp)) {
                status = false;
                break;
            }
            status = true;
            break;

            pch = strtok(NULL, "\\/");
        }
    }
    free(temp);
    free(new);
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

bool write_to_file(char *fname, void *data, size_t size)
{
    FILE *fd = fopen(fname, "wb");
    if (!fd) {
        return false;
    }
    int wc = fwrite(data, 1, size, fd);
    if (wc != size) {
        fclose(fd);
        return false;
    }
    fclose(fd);
    return true;
}

void ntar_extract(mtar_t *tar, mtar_header_t *header)
{
    if (header->type != MTAR_TDIR) {
        char *dir = dirname(header->name);
        if (!mkdir_parent(dir)) {
            printf("Error creating folder '%s'!\n", header->name);
            return;
        }

        void *data = malloc(header->size);
        if (!data) {
            puts("Memory allocation error!");
            return;
        }

        int rd_st = mtar_read_data(tar, data, header->size);
        if (!rd_st) {
            printf("Error reading file '%s' from tar! %s", header->name, mtar_strerror(rd_st));
            goto free;
        }

        if (!write_to_file(header->name, data, header->size)) {
            printf("Error writing to file '%s'!\n", header->name);
        }

    free:
        free(data);
    }
}

int ntar_work_content(const char *tar_fname, ntar_flist_t flist, ntar_content_act_t act)
{
    mtar_t tar;
    mtar_header_t tar_header;
    int status = MTAR_ESUCCESS;

    status = mtar_open(&tar, tar_fname, "rb");
    if (status) {
        printf("Unable to open tar '%s'! %s\n", tar_fname, mtar_strerror(status));
        return status;
    }

    if (flist.fnum > 0) {
        for (size_t i = 0; i < flist.fnum; i++) {
            int status = mtar_find(&tar, flist.names[i], &tar_header);
            if (status != MTAR_ESUCCESS) {
                printf("File extraction error '%s'! %s\n", flist.names[i], mtar_strerror(status));
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

    const char *mode = "wb";

    if (!new_tar) {
        mode = "ab";
    }

    int status = mtar_open(&tar, tar_fname, mode);
    if (status) {
        printf("Unable to open tar '%s'! %s\n", tar_fname, mtar_strerror(status));
        return status;
    }

    for (size_t i = 0; i < flist.fnum; i++) {
        size_t size = 0;

        if (os_is_dir(flist.names[i])) {
            tar_err_h = mtar_write_dir_header(&tar, flist.names[i]);
        } else {
            FILE *fd = fopen(flist.names[i], "rb");
            if (!fd) {
                printf("Error: Unable to open file '%s!'\n", flist.names[i]);
                continue;
            }

            size = os_get_fsize(flist.names[i]);
            tar_err_h = mtar_write_file_header(&tar, flist.names[i], size);

            while (!feof(fd)) {
                void *file_data = malloc(BLOCK_LEN);
                if (!file_data) {
                    puts("Memory allocation error!");
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
        " _ _| |_ __ _ _ _   ntar (NANO tar) - v0.2 \n"
        "| ' \\  _/ _` | '_|  License: GPL-2.0-or-later\n"
        "|_||_\\__\\__,_|_|    Author — Maxim Logaev (2022 year)\n"
        "\n"
        "Usage: ntar [options] [tar-file] [files ...]\n"
        "Examples:\n"
        "   ntar -c test.tar foo bar  # Create archive.tar from files foo and bar"
        "bar.\n"
        "   ntar -l test.tar          # List all files in test.tar.\n"
        "   ntar -x test.tar          # Extract all files from test.tar.\n"
        "   ntar -x test.tar foo bar  # Extract foo and bar from test.tar.\n"
        "\n"
        "Options:\n"
        "   -c                        Create tar file.\n"
        "   -a                        Add files to archive.\n"
        "   -l                        Show all files from archive.\n"
        "   -x                        Extract files (not implemented yet).\n\n");
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
        //status = ntar_work_content(tar_fname, flist_in, NTAR_EXTRACT);
        printf("Extract not implemented yet :(\n");
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
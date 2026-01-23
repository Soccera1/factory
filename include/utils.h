#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

// String utilities
char *str_dup(const char *s);
char *file_read_all(const char *filepath);
int file_exists(const char *filepath);
int run_command(const char *command);
int download_file(const char *url, const char *dest);
int extract_tarball(const char *filepath, const char *dest_dir);
char *join_path(const char *dir, const char *file);
void create_dir_if_not_exists(const char *dir_path);

// Blueprint utilities
#include "factory.h"
Blueprint *blueprint_parse(const char *filepath);
void blueprint_free(Blueprint *bp);

#endif

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "utils.h"

char *str_dup(const char *s) {
    if (!s) return NULL;
    return strdup(s);
}

char *file_read_all(const char *filepath) {
    FILE *f = fopen(filepath, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (buffer) {
        fread(buffer, 1, length, f);
        buffer[length] = '\0';
    }
    fclose(f);
    return buffer;
}

int file_exists(const char *filepath) {
    return access(filepath, F_OK) != -1;
}

int run_command(const char *command) {
    printf(">> Running: %s\n", command);
    return system(command);
}

int download_file(const char *url, const char *dest) {
    char cmd[2048];
    snprintf(cmd, sizeof(cmd), "curl -L -s -o '%s' '%s'", dest, url);
    printf("Downloading %s...\n", url);
    return run_command(cmd);
}

int extract_tarball(const char *filepath, const char *dest_dir) {
    char cmd[2048];
    create_dir_if_not_exists(dest_dir);
    snprintf(cmd, sizeof(cmd), "tar -xf '%s' -C '%s' --strip-components=1", filepath, dest_dir);
    printf("Extracting to %s...\n", dest_dir);
    return run_command(cmd);
}

char *join_path(const char *dir, const char *file) {
    size_t len = strlen(dir) + strlen(file) + 2;
    char *path = malloc(len);
    snprintf(path, len, "%s/%s", dir, file);
    return path;
}

void create_dir_if_not_exists(const char *dir_path) {
    struct stat st = {0};
    if (stat(dir_path, &st) == -1) {
        // mkdir -p is complex in pure C, let's use system or simple mkdir for now
        // For portability in this prototype, system("mkdir -p") is safest "less terrible" vs writing 100 lines of mkdir code
        char cmd[1024];
        snprintf(cmd, sizeof(cmd), "mkdir -p '%s'", dir_path);
        system(cmd);
    }
}

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "factory.h"
#include "utils.h"

static char *trim_quotes(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == '"') str++; // skip leading quote

    end = str + strlen(str) - 1;
    while(end > str && (isspace((unsigned char)*end) || *end == '"')) end--;
    end[1] = '\0';

    return str;
}

Blueprint *blueprint_parse(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (!file) return NULL;

    Blueprint *bp = calloc(1, sizeof(Blueprint));
    char line[1024];

    while (fgets(line, sizeof(line), file)) {
        char *p = strchr(line, '=');
        if (!p) continue;

        *p = '\0';
        char *key = line;
        char *val = p + 1;

        // Trim whitespace from key
        while(isspace((unsigned char)*key)) key++;
        char *key_end = key + strlen(key) - 1;
        while(key_end > key && isspace((unsigned char)*key_end)) *key_end-- = '\0';

        val = trim_quotes(val);

        if (strcmp(key, "name") == 0) bp->name = str_dup(val);
        else if (strcmp(key, "version") == 0) bp->version = str_dup(val);
        else if (strcmp(key, "url") == 0) bp->url = str_dup(val);
        else if (strcmp(key, "sha256") == 0) bp->sha256 = str_dup(val);
        else if (strcmp(key, "build_script") == 0) bp->build_script = str_dup(val);
        else if (strcmp(key, "dependencies") == 0) {
            char *dep_str = str_dup(val);
            char *token = strtok(dep_str, ",");
            while (token) {
                while(isspace((unsigned char)*token)) token++;
                bp->dependencies = realloc(bp->dependencies, sizeof(char*) * (bp->num_dependencies + 1));
                bp->dependencies[bp->num_dependencies++] = str_dup(token);
                token = strtok(NULL, ",");
            }
            free(dep_str);
        }
    }

    fclose(file);
    return bp;
}

void blueprint_free(Blueprint *bp) {
    if (!bp) return;
    free(bp->name);
    free(bp->version);
    free(bp->url);
    free(bp->sha256);
    free(bp->build_script);
    for (int i = 0; i < bp->num_dependencies; i++) {
        free(bp->dependencies[i]);
    }
    free(bp->dependencies);
    free(bp);
}

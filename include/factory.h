#ifndef FACTORY_H
#define FACTORY_H

typedef struct {
    char *name;
    char *version;
    char *url;
    char *sha256;
    char *build_script;
    char **dependencies;
    int num_dependencies;
} Blueprint;

int factory_install(const char *package_name);
int factory_remove(const char *package_name);
int factory_list(void);
int factory_update(void);

#endif

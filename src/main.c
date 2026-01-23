#include <stdio.h>
#include <string.h>
#include "factory.h"

void print_help(const char *prog) {
    printf("Usage: %s <command> [args]\n", prog);
    printf("Commands:\n");
    printf("  install <package>  Install a package\n");
    printf("  remove <package>   Remove a package\n");
    printf("  list               List installed packages\n");
    printf("  help               Show this help\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }

    const char *cmd = argv[1];

    if (strcmp(cmd, "install") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: factory install <package>\n");
            return 1;
        }
        return factory_install(argv[2]);
    } else if (strcmp(cmd, "remove") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: factory remove <package>\n");
            return 1;
        }
        return factory_remove(argv[2]);
    } else if (strcmp(cmd, "list") == 0) {
        return factory_list();
    } else if (strcmp(cmd, "update") == 0) {
        return factory_update();
    } else {
        print_help(argv[0]);
        return 1;
    }

    return 0;
}

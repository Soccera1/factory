#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "factory.h"
#include "utils.h"

#define FACTORY_ROOT ".factory"
#define CELLAR_DIR "cellar"
#define CACHE_DIR "cache"
#define REPO_DIR "repo"
#define DEFAULT_REPO_URL "https://github.com/example/factory-blueprints.git"

static void get_factory_path(char *buffer, size_t size, const char *subdir) {
    const char *home = getenv("HOME");
    if (!home) home = ".";
    if (subdir)
        snprintf(buffer, size, "%s/%s/%s", home, FACTORY_ROOT, subdir);
    else
        snprintf(buffer, size, "%s/%s", home, FACTORY_ROOT);
}

int factory_install(const char *package_name) {
    char repo_path[1024];
    get_factory_path(repo_path, sizeof(repo_path), REPO_DIR);

    char blueprint_path[1024];
    snprintf(blueprint_path, sizeof(blueprint_path), "%s/%s.blueprint", repo_path, package_name);

    if (!file_exists(blueprint_path)) {
        fprintf(stderr, "Error: Package '%s' not found (checked %s)\n", package_name, blueprint_path);
        return 1;
    }

    Blueprint *bp = blueprint_parse(blueprint_path);
    if (!bp) {
        fprintf(stderr, "Error: Failed to parse blueprint for %s\n", package_name);
        return 1;
    }

    printf("Installing %s %s...\n", bp->name, bp->version);

    // Resolve dependencies
    for (int i = 0; i < bp->num_dependencies; i++) {
        printf(">> Resolving dependency: %s for %s\n", bp->dependencies[i], bp->name);
        if (factory_install(bp->dependencies[i]) != 0) {
            fprintf(stderr, "Error: Failed to install dependency '%s'\n", bp->dependencies[i]);
            blueprint_free(bp);
            return 1;
        }
    }

    // Setup paths
    char cache_path[1024], cellar_path[1024], build_dir[1024];
    get_factory_path(cache_path, sizeof(cache_path), CACHE_DIR);
    get_factory_path(cellar_path, sizeof(cellar_path), CELLAR_DIR);
    
    // Create cache dir
    create_dir_if_not_exists(cache_path);

    // Download
    char *filename = strrchr(bp->url, '/');
    if (filename) filename++; else filename = "source.tar.gz";
    
    char local_tarball[1024];
    snprintf(local_tarball, sizeof(local_tarball), "%s/%s", cache_path, filename);

    if (!file_exists(local_tarball)) {
        if (download_file(bp->url, local_tarball) != 0) {
            fprintf(stderr, "Download failed.\n");
            blueprint_free(bp);
            return 1;
        }
    }

    // Prepare build dir (temp)
    snprintf(build_dir, sizeof(build_dir), "%s/build-%s", cache_path, bp->name);
    // Clean previous build
    char clean_cmd[1024];
    snprintf(clean_cmd, sizeof(clean_cmd), "rm -rf '%s'", build_dir);
    system(clean_cmd);

    if (extract_tarball(local_tarball, build_dir) != 0) {
        fprintf(stderr, "Extraction failed.\n");
        blueprint_free(bp);
        return 1;
    }

    // Build
    // Set PREFIX env var for the build script
    char prefix[1024];
    snprintf(prefix, sizeof(prefix), "%s/%s/%s", cellar_path, bp->name, bp->version);
    
    // Check if already installed
    if (file_exists(prefix)) {
        printf("%s %s is already installed.\n", bp->name, bp->version);
        blueprint_free(bp);
        return 0;
    }

    setenv("PREFIX", prefix, 1);
    
    // Change to build dir
    char cwd[1024];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd");
        return 1;
    }
    chdir(build_dir);

    printf("Running build script...\n");
    if (run_command(bp->build_script) != 0) {
        fprintf(stderr, "Build failed.\n");
        chdir(cwd);
        blueprint_free(bp);
        return 1;
    }

    chdir(cwd);
    printf("Successfully installed %s to %s\n", bp->name, prefix);

    blueprint_free(bp);
    return 0;
}

int factory_remove(const char *package_name) {
    char cellar_path[1024];
    get_factory_path(cellar_path, sizeof(cellar_path), CELLAR_DIR);
    
    char pkg_path[1024];
    snprintf(pkg_path, sizeof(pkg_path), "%s/%s", cellar_path, package_name);

    if (!file_exists(pkg_path)) {
        fprintf(stderr, "Package %s is not installed.\n", package_name);
        return 1;
    }

    printf("Uninstalling %s...\n", package_name);
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "rm -rf '%s'", pkg_path);
    
    return run_command(cmd);
}

int factory_list(void) {
    char cellar_path[1024];
    get_factory_path(cellar_path, sizeof(cellar_path), CELLAR_DIR);

    DIR *d = opendir(cellar_path);
    if (!d) {
        printf("No packages installed (Cellar not found).\n");
        return 0;
    }

    struct dirent *dir;
    printf("Installed packages:\n");
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_name[0] == '.') continue; // Skip hidden, ., and ..

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", cellar_path, dir->d_name);
        
        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            printf("- %s\n", dir->d_name);
        }
    }
        closedir(d);
        return 0;
    }
    
    int factory_update(void) {
        char repo_path[1024];
        get_factory_path(repo_path, sizeof(repo_path), REPO_DIR);
        
        const char *repo_url = getenv("FACTORY_REPO_URL");
        if (!repo_url) repo_url = DEFAULT_REPO_URL;
    
        if (file_exists(repo_path)) {
            printf("Updating repository at %s...\n", repo_path);
            char cmd[2048];
            snprintf(cmd, sizeof(cmd), "git -C '%s' pull", repo_path);
            return run_command(cmd);
        } else {
            printf("Cloning repository from %s...\n", repo_url);
            char cmd[2048];
            // Ensure parent dir exists
            char factory_root[1024];
            get_factory_path(factory_root, sizeof(factory_root), NULL);
            create_dir_if_not_exists(factory_root);
            
            snprintf(cmd, sizeof(cmd), "git clone '%s' '%s'", repo_url, repo_path);
            return run_command(cmd);
        }
    }
    
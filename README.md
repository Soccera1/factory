# Factory

Factory is a lightweight, source-based package manager written in C. It uses "blueprints" to define how packages should be downloaded and built.

## Features

-   **Source-based:** Compiles packages from source.
-   **Blueprints:** Simple configuration files (shell-compatible syntax) to define packages.
-   **Dependency Resolution:** Automatically installs dependencies defined in blueprints.
-   **Isolated Builds:** Installs packages into their own prefixes within `~/.factory/cellar`.

## Building and Installation

### Prerequisites

-   C Compiler (GCC or Clang)
-   Meson Build System
-   Ninja
-   `curl` and `tar` (runtime dependencies for downloading and extracting)
-   `git` (runtime dependency for updating blueprints)

### Build Steps

1.  **Setup the build directory:**
    ```bash
    meson setup build
    ```

2.  **Compile the project:**
    ```bash
    meson compile -C build
    ```

3.  **Install (Optional):**
    ```bash
    meson install -C build
    ```

## Usage

The main command is `factory`.

```bash
factory <command> [args]
```

### Commands

-   **`update`**
    Updates the local blueprint repository. By default, it clones/pulls from `https://github.com/example/factory-blueprints.git`.
    You can override the repository URL by setting the `FACTORY_REPO_URL` environment variable.

    ```bash
    factory update
    ```

-   **`install <package>`**
    Installs a package by name. It looks for `<package>.blueprint` in the repository.

    ```bash
    factory install gnu-hello
    ```

-   **`remove <package>`**
    Uninstalls a package.

    ```bash
    factory remove gnu-hello
    ```

-   **`list`**
    Lists all installed packages.

    ```bash
    factory list
    ```

## Configuration

### Directory Structure

Factory stores its data in `~/.factory`:

-   `~/.factory/repo`: Contains the git repository with blueprints.
-   `~/.factory/cache`: Stores downloaded source tarballs.
-   `~/.factory/cellar`: Installation directory for packages (each version in its own subdir).

### Environment Variables

-   `FACTORY_REPO_URL`: URL of the git repository containing blueprints (Default: `https://github.com/example/factory-blueprints.git`).

## Blueprint Format

Blueprints are simple text files defining package metadata and build instructions. They essentially look like shell variable assignments.

**Example: `examples/gnu-hello.blueprint`**

```bash
name="gnu-hello"
version="2.12"
url="http://ftp.gnu.org/gnu/hello/hello-2.12.tar.gz"
sha256="cf04af86dc085268c5f4480f74cc0cb66316aa0fd94feb6ed645d826e7a5d13b"
build_script="./configure --prefix=$PREFIX && make && make install"
```

**Fields:**

-   `name`: Name of the package.
-   `version`: Version string.
-   `url`: Download URL for the source tarball.
-   `sha256`: SHA-256 checksum of the download (Note: Verification is currently not implemented/enforced).
-   `dependencies`: Comma-separated list of package names that this package depends on (e.g., `dependencies="zlib,openssl"`).
-   `build_script`: The command(s) to build and install the software. Use `$PREFIX` to refer to the installation destination.

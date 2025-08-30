![logo](docs/images/logo.png)

Try not to get lost in the Great Halls of Diabaig

## INSTALLATION

Disclaimer: This is all a bit messy at the moment

Compile source code using:

```bash
$~ make
$~ make install
```

### Other compilation options

Alter installation path with `INSTALL_PATH=/path/to/folder`

If cross compiling, add `PLATFORM=<platform>` to each `make` command, where <platform> is chosen from `linux`,`macos`,`windows`. Alternatively add `TOOLCHAIN=/path/to/toolchain` to use a custom toolchain.
The executable can be linked statically on linux and macos with `make static`.
Package the executable to a .deb file on linux with `make all/static package`. On windows this will create a .zip folder with all the appropriate .dll files.


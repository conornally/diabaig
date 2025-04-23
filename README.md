![logo](docs/images/logo.png)

Try not to get lost in the Great Derelict Halls of Diabaig

## INSTALLATION

Disclaimer: I have no idea how to compile on windows, so the windows toolchain is purely for cross-compiling from linux.

Compile source code using:

```bash
$~ make
$~ make install
```

### Other compilation options

This is all still a bit messy and needs cleaning up.

If cross compiling, add `PLATFORM=<platform>` to each `make` command, where <platform> is chosen from `linux`,`macos`,`windows`. Alternatively add `TOOLCHAIN=/path/to/toolchain` to use a custom toolchain.
The executable can be linked statically on linux and macos with `make static`.
Package the executable to a .deb file on linux with `make all/static package`. On windows this will create a .zip folder with all the appropriate .dll files.


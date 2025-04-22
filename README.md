![logo](docs/images/logo.png)

Try not to get lost in the Great Winding Halls of Diabaig



## INSTALLATION

Disclaimer: I have no idea how to compile on windows, so the windows toolchain is purely for cross-compiling from linux.

Compilation should just be:
```bash
$~ make
$~ make install
```


### CROSS-COMPILATION

Set the target platform (linux, macos, windows). You can build a custom toolchain if required

```bash
$~ make PLATFORM=windows all
```
OR
```bash
$~ make TOOLCHAIN=toolchain/windows.mk all
```

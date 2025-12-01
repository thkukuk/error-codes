# Building and installing error-codes

## Building with Meson

Building with Meson is quite simple:

```shell
$ meson setup build
$ meson compile -C build
$ meson test -C build
$ sudo meson install -C build
```

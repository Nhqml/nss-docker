# libnss_docker
*A NSS service for Docker containers resolving*

:warning: *This is a **toy** project and has not been intensely tested! Use at your own risk*

## Dependencies

- `libcurl` (>= 7.80)
- `libcjson` (>= 1.7.15)

*Note that older versions of the dependencies may work, but they have not been tested*

## Install

Requires `meson` and `ninja`.

```bash
meson setup builddir && cd builddir && meson install
```

## Build

Requires `meson` and `ninja`.

```bash
meson setup --buildtype=debug builddir && cd builddir && meson compile
```

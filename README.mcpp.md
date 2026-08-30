# wayland-m

[Wayland](https://gitlab.freedesktop.org/wayland/wayland) 1.23.1 with mcpp build
support, consumed from [mcpp-index](https://github.com/mcpplibs/mcpp-index) as
three packages:

| package | output |
|---|---|
| `compat.wayland-scanner` | `wayland-scanner`, the protocol code generator |
| `compat.wayland` | `libwayland-client.so.0` |
| `compat.wayland-server` | `libwayland-server.so.0` |

```bash
mcpp build --workspace
```

## What was added

Nothing upstream was patched. The fork adds:

```
config.h              the probe results meson's configure_file() writes,
                      for linux/glibc (wayland-os.c says "../config.h")
mcpp/include/         wayland-version.h, substituted from src/wayland-version.h.in
mcpp/scanner/         manifest for the generator
mcpp/client/          manifest + build.mcpp for libwayland-client
mcpp/server/          manifest + build.mcpp for libwayland-server
mcpp.toml             the workspace root
```

`build.mcpp` declares the wayland-scanner invocations as build-graph edges, so
they re-run exactly when `protocol/wayland.xml` changes.

## Why the scanner is a package of its own

A host `wayland-scanner` is not interchangeable. 1.22 rejects 1.23's
`wayland.xml` outright — it does not know the `deprecated-since` attribute — so
reaching for whatever is on PATH either fails or, worse, emits code for a
different protocol revision than the headers describe. Asking for it with
`tools = ["wayland-scanner"]` makes the generator's version the dependency's
version, so that mismatch is not expressible.

## Upstream

Tracking wayland 1.23.1. Upstream sources, `protocol/`, `tests/` and the meson
build are untouched, so `meson setup build && ninja -C build` still works.

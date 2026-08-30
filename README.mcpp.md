# mcpplibs/wayland

[Wayland](https://gitlab.freedesktop.org/wayland/wayland) 1.26.0 with mcpp build
support. Consumed from [mcpp-index](https://github.com/mcpplibs/mcpp-index) as
four packages, all out of this one tarball:

| package | output |
|---|---|
| `freedesktop.wayland-scanner` | `wayland-scanner`, the protocol code generator |
| `freedesktop.wayland` | `libwayland-client.so.0` + `import wayland.client;` |
| `freedesktop.wayland-server` | `libwayland-server.so.0` + `import wayland.server;` |
| `freedesktop.wayland-util` | `import wayland.util;` — the macros, as entities |

```bash
mcpp build --workspace
```

## The module wrappers add no API

`import wayland.client;` replaces `#include <wayland-client.h>` and changes
nothing else. Every exported name is upstream's, spelled upstream's way, with
upstream's semantics — there are no wrapper types, no RAII, no renaming — so
code written against the C headers ports by swapping one line.

The export lists are **generated from the public headers** rather than kept by
hand, so a version bump cannot quietly drop a name.

## Macros are the one thing that could not cross

`export` names entities, and a macro is not one. Wayland's public surface has
fourteen, so `wayland.util` maps each to what it actually is:

| macro | in the module |
|---|---|
| `WL_MARSHAL_FLAG_DESTROY` | `inline constexpr` — the spelling survives intact |
| `wl_container_of(p, sample, member)` | `wl_container_of<&T::member>(p)` |
| `wl_list_for_each(p, head, member)` | `for (T *p : wl_list_each<&T::member>(head))` |
| `wl_list_for_each_safe` / `_reverse` / `_reverse_safe` | the matching `wl_list_each_*` ranges |
| `wl_array_for_each(p, array)` | `for (T *p : wl_array_each<T>(array))` |

The `_safe` variants keep upstream's guarantee — the successor is latched
before the body runs, so the current element may be removed or freed — and the
package's test exercises exactly that. It links nothing: the list is wired up by
hand so a package of templates keeps zero dependencies.

## Why this is a fork and not an index descriptor

Wayland is mostly **generated**. `protocol/wayland.xml` describes every
interface and `wayland-scanner` emits ~13,000 lines from it — most of both
libraries. The generator is a C program in this same tree, so it has to be
**compiled before it can run**.

An mcpp-index descriptor cannot express that: it has no build step, and an
`install()` hook cannot do it either, because mcpp compiles a package's sources
at *consumer-build* time — no package binary exists while another package is
installing. `build.mcpp` is the mechanism for exactly this, and it only exists
for a real mcpp project. Same reason [grpc-m](https://github.com/mcpplibs/grpc-m)
exists.

A host `wayland-scanner` is not a substitute. 1.22 rejects 1.26's
`protocol/wayland.xml` outright — it does not know the `deprecated-since`
attribute — so reaching for whatever is on PATH either fails or, worse, emits
code for a different protocol revision than the headers describe. Asking for it
with `tools = ["wayland-scanner"]` makes the generator's version the
dependency's version, so that mismatch is not expressible.

## Why four packages rather than one

`libwayland-client.so.0` and `libwayland-server.so.0` are distinct SONAMEs, and
Mesa's `libEGL_mesa` carries DT_NEEDED on **both** — so they must be two files
with disjoint contents. mcpp compiles a package's sources once and links every
library target against all of them; there is no per-target source list, and a
feature-gated second target still receives the feature's objects (measured).
mcpp's own diagnostic names the remedy: *"for config that must affect shared
code, split into a workspace member."*

Each library package carries its C library **and** its module, so there is one
package per library rather than a C one and a module one beside it.

## What was added

Nothing upstream was patched:

```
config.h              the probe results meson's configure_file() writes, for
                      linux/glibc (wayland-os.c says "../config.h", so it is
                      at the tree root)
mcpp/include/         wayland-version.h, substituted from src/wayland-version.h.in
mcpp/scanner/         the generator
mcpp/util/            the macro mappings + their test
mcpp/client/          libwayland-client + wayland.client, and build.mcpp
mcpp/server/          libwayland-server + wayland.server, and build.mcpp
mcpp.toml             the workspace root
```

`build.mcpp` declares the wayland-scanner invocations as build-graph edges, so
they re-run exactly when `protocol/wayland.xml` changes and a failure is
attributed to the edge rather than to "build.mcpp exited 1".

## Upstream

Tracking wayland 1.26.0. Upstream sources, `protocol/`, `tests/` and the meson
build are untouched — CI builds the tree with `meson setup && ninja` on every
run, so "no upstream file is patched" has a test rather than a promise.

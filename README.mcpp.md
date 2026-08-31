# mcpplibs/wayland

[Wayland](https://gitlab.freedesktop.org/wayland/wayland) 1.26.0 with mcpp build
support. Consumed from [mcpp-index](https://github.com/mcpplibs/mcpp-index) as
four packages, all out of this one tarball:

| package | output |
|---|---|
| `freedesktop.wayland-scanner` | `wayland-scanner`, the protocol code generator |
| `freedesktop.wayland` | `libwayland-client.so.0` + `import freedesktop.wayland.client;` |
| `freedesktop.wayland-server` | `libwayland-server.so.0` + `import freedesktop.wayland.server;` |
| `freedesktop.wayland-util` | `import freedesktop.wayland.util;` — the macros, as entities |

```bash
mcpp build --workspace
```

## The module wrappers add no API

`import freedesktop.wayland.client;` replaces `#include <wayland-client.h>` and changes
nothing else. Every exported name is upstream's, spelled upstream's way, with
upstream's semantics — there are no wrapper types, no RAII, no renaming — so
code written against the C headers ports by swapping one line.

The export lists are **generated from the public headers** rather than kept by
hand, so a version bump cannot quietly drop a name.

## What crosses the module boundary, and what does not

`export` names entities, and it cannot name one with **internal linkage**. That
rule decides the whole layout here:

- **Types, `wl_proxy_*` / `wl_display_*` / `wl_resource_*`, the interface
  objects, the enums** — external linkage, re-exported with `using ::name;`.
- **The protocol wrappers** (`wl_surface_attach`, `wl_data_device_send_drop`, …)
  — wayland-scanner emits them `static inline`. A copy of the generated header
  with `static inline` changed to `inline` is included *inside the module
  purview*, which gives them external linkage; everything in it sits inside
  `extern "C"`, so it keeps C language linkage and still matches
  `wayland-protocol.c`'s definitions. GCC accepts the naive `using ::name;`
  here and clang rejects it — clang is right, and CI runs both.
- **`wl_fixed_to_double` and friends, `wl_signal_*`** — `static inline` in
  `wayland-util.h` / `wayland-server-core.h`, which the module includes in its
  global module fragment. Those cannot be reached, so a consumer that needs them
  includes the header next to the import. Four and five names respectively.
- **Macros** — not entities at all; see below.

## Macros are the one thing that could not cross

`export` names entities, and a macro is not one. Wayland's public surface has
fourteen, so `freedesktop.wayland.util` maps each to what it actually is:

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

## Layout

```
upstream/             wayland 1.26.0, verbatim — never touched
mcpp/                 everything this fork adds
  config.h            the probe results meson's configure_file() writes, for
                      linux/glibc. Two spellings reach it: wayland-shm.c says
                      "config.h" and wayland-os.c says "../config.h", so the
                      manifests put both `mcpp/` and `mcpp/include/` on the
                      include path and one file answers both.
  include/            wayland-version.h, substituted from src/wayland-version.h.in
  scanner/            the generator
  util/               the macro mappings + their test
  generated/          wayland-scanner's output, checked in — see its README
  client/             libwayland-client + freedesktop.wayland.client
  server/             libwayland-server + freedesktop.wayland.server
mcpp.toml             the workspace root
```

Updating upstream is replacing `upstream/`. Nothing this fork adds lives inside
it, so a diff against a fresh release tarball is empty there by construction —
and CI checks exactly that on every run, by downloading wayland 1.26.0 and
comparing. Not by building it with meson: this repository builds one way,
through mcpp, and comparing is the stronger check anyway — a successful meson
build proves the tree still builds, not that nothing was edited.

The protocol code is **checked in** under `mcpp/generated/`, not produced during
the build. `wayland-client.h` includes the generated header, which makes it part
of the package's PUBLIC interface — every consumer needs it on their include
path — and `mcpp::include_dir()` from a `build.mcpp` is package-private by
design. Generating it at build time also raced the package's own compiles
(mcpp-community/mcpp#534). It is deterministic from a pinned `wayland.xml`, so
checking it in costs nothing, and CI regenerates and diffs it on every run.

`freedesktop.wayland-scanner` is still a package: a compositor generating
bindings for `wayland-protocols` XML needs exactly that tool, and asking for it
by name pins its version to this one.

## Upstream

Tracking wayland 1.26.0. `upstream/` is the release tarball byte for byte, and
CI diffs it against a freshly downloaded one on every run — so "no upstream file
is patched" has a test rather than a promise.

## ⭐ 八个曾经够不着的函数(本次修复)

`static inline` 是内部链接,C++ 禁止从模块导出。这个 fork 一直**知道**这个坑 ——
它为 wayland-scanner 生成的包装做了 `inline` 副本 —— 但漏了两个手写头文件里的:

| 头文件 | 函数 | 归属模块 |
|---|---|---|
| `wayland-server-core.h` | `wl_signal_init/_add/_get/_emit` | `freedesktop.wayland.server` |
| `wayland-util.h` | `wl_fixed_to/from_double/int` | `freedesktop.wayland.client` |

**这不是边角**。每一次 wlroots 监听注册都是 `wl_signal_add(&x->events.y, &l)` ——
**少了它就没法用模块路线写合成器**;而 `wl_fixed_t` 是协议里每个亚像素坐标的载体
(指针移动、触摸、数位板),**少了它处理不了输入事件**。

⚠️ **没人发现,因为这个包的测试从没调用过其中任何一个。** 缺口是从**外面**被问出来
的 —— 一个最小 wlroots 合成器写不下去。同一类问题在 `freedesktop.cairo`(少
`cairo_t`)和 `displayinfo`(少 295 个枚举量)上都出现过,发现方式也一样。

### 做法与两个坑

原来的 `inline` 副本手法在这里**不适用**:这两个头必须进 global module fragment
(其它声明要用它们的类型),所以 `static` 定义已经可见,在 purview 里同名定义就是
重定义。做法是**先把 static 原件改名让开**,再用真名定义并导出。

两个只有 clang 会说的问题:

- **`struct wl_signal *` 不能写**。elaborated-type-specifier 会在模块里**重新声明**
  该类型:`declaration of 'wl_signal' in module ... follows declaration in the
  global module`。GCC 静默接受。去掉 `struct` 关键字即可。
- **`wl_fixed_*` 不能放进 `freedesktop.wayland.util`**。那个模块是**和头文件配对**
  设计的(只出宏和模板,零个 `using ::`),它自己的测试就写着
  `#include <wayland-util.h>` + `import`。加同名实体会让每次调用
  `call to 'wl_fixed_from_int' is ambiguous`。也**没有**同时放进 server ——
  两个模块各自的实体,对同时 import 两者的 TU 一样是二义。

三个模块同时 import 已实测:gcc 16.1 与 llvm 22.1 都通过。


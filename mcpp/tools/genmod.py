#!/usr/bin/env python3
"""Regenerate the wayland module wrappers from the public headers."""
import re, sys, pathlib

F = pathlib.Path(sys.argv[1])          # fork root
GEN = F / "mcpp" / "generated"

def strip_comments(t):
    t = re.sub(r'/\*.*?\*/', ' ', t, flags=re.S)
    return re.sub(r'//[^\n]*', ' ', t)

STATIC_INLINE = re.compile(
    r'^static\s+inline\s+([^\n]+?)\s*\n(\w+)\(([^)]*)\)\s*\n\{', re.M | re.S)

def param_names(params):
    """The argument list to forward: last identifier of each parameter."""
    params = params.strip()
    if params in ("", "void"):
        return []
    out = []
    depth = 0
    cur = ""
    for ch in params:
        if ch == ',' and depth == 0:
            out.append(cur); cur = ""
            continue
        if ch in "([": depth += 1
        if ch in ")]": depth -= 1
        cur += ch
    out.append(cur)
    names = []
    for p in out:
        m = re.findall(r'(\w+)\s*(?:\[\s*\d*\s*\])?$', p.strip())
        if not m:
            raise SystemExit(f"cannot name parameter in {p!r}")
        names.append(m[-1])
    return names

ELABORATED = re.compile(r'\b(?:struct|union|enum)\s+(?=\w)')

def plain(t):
    """`struct wl_buffer *` -> `wl_buffer *`.

    An elaborated-type-specifier in a module-purview declaration DECLARES the
    type there, and clang then rejects it:

        error: declaration of 'wl_buffer' in module wayland.client follows
               declaration in the global module

    The types are already declared by the global module fragment's includes, so
    naming them plainly refers to those instead of introducing new ones.
    """
    return ELABORATED.sub("", t)

def scan(paths):
    types, funcs, vars_, macros = set(), set(), set(), set()
    for p in paths:
        raw = pathlib.Path(p).read_text(encoding="utf-8", errors="replace")
        for m in re.finditer(r'^\s*#\s*define\s+(\w+)', raw, flags=re.M):
            macros.add(m.group(1))
        t = strip_comments(raw)
        for m in re.finditer(r'\b(?:struct|union|enum)\s+(wl_\w+)', t): types.add(m.group(1))
        for m in re.finditer(r'\btypedef\b[^;{}]*?\b(wl_\w+)\s*;', t, flags=re.S): types.add(m.group(1))
        for m in re.finditer(r'\btypedef\b[^;]*?\(\s*\*\s*(wl_\w+)\s*\)', t, flags=re.S): types.add(m.group(1))
        for m in re.finditer(r'\bextern\b[^;{}]*?\b(wl_\w+)\s*;', t, flags=re.S): vars_.add(m.group(1))
        for m in re.finditer(r'\b(wl_\w+)\s*\(', t): funcs.add(m.group(1))
    funcs -= vars_ | types | macros
    types -= macros
    vars_ -= macros
    return types, funcs, vars_

HEAD = '''// {title}
//
// A module wrapper and nothing more: every name below is upstream's, spelled
// upstream's way, with upstream's signature. `import {mod};` replaces the
// #include and changes nothing else, so code written against the C headers
// ports by swapping one line.
//
// GENERATED from the public headers by mcpp/tools/genmod.py — not hand-kept,
// so a version bump cannot quietly drop a name.
//
// TWO KINDS OF EXPORT, AND THE REASON IS A LANGUAGE RULE.
//
// Most names are re-exported with `using ::name;`. The protocol convenience
// wrappers cannot be: wayland-scanner emits them `static inline`, which is
// INTERNAL LINKAGE, and C++ forbids exporting such an entity —
//
//     error: using declaration referring to 'wl_surface_attach' with
//            internal linkage cannot be exported
//
// GCC accepts it, Clang rejects it, and Clang is right. So each one gets a
// one-line `export inline` forwarder with the identical signature that calls
// the static original. Same name, same arguments, same semantics; the only
// thing that changed is a linkage the caller cannot observe.
//
// MACROS ARE NOT HERE, and cannot be: `export` names entities and a macro is
// not one. wayland's public macros live in the `wayland.util` module
// (freedesktop.wayland-util), which maps each to the entity it actually is.
module;

{includes}

export module {mod};

'''

for side, mod, incs, title in [
    ("client", "wayland.client",
     ["<wayland-client-core.h>"],
     "wayland.client — libwayland-client, as a C++23 module."),
    ("server", "wayland.server",
     ["<wayland-server-core.h>"],
     "wayland.server — libwayland-server, as a C++23 module."),
]:
    proto = GEN / f"wayland-{side}-protocol.h"
    hdrs = [F/"upstream/src/wayland-util.h", F/f"upstream/src/wayland-{side}-core.h"]
    hdrs = [h for h in hdrs if pathlib.Path(h).exists()]
    types, funcs, vars_ = scan(hdrs)

    # Anything the GMF headers define `static inline` has INTERNAL linkage and
    # cannot be exported — wayland-util.h's wl_fixed_* conversions and
    # wayland-server-core.h's wl_signal_* helpers are the two families. They
    # arrive with the header, so a consumer that needs them includes it next to
    # the import; the file's header comment says so.
    internal = set()
    for h in hdrs:
        src_h = strip_comments(pathlib.Path(h).read_text(encoding="utf-8"))
        internal |= {m.group(2) for m in STATIC_INLINE.finditer(src_h)}
    funcs -= internal

    body = HEAD.format(title=title, mod=mod,
                       includes="\n".join(f"#include {i}" for i in incs))
    body += "export {\n"
    for label, g in (("types", types), ("functions", funcs), ("interfaces / data", vars_)):
        body += f"\n// -- {label} --\n" + "".join(f"using ::{n};\n" for n in sorted(g))
    body += "}\n"

    body += f'''
// The protocol API: {mod.split(".")[1]}-side wrappers and the interface objects,
// generated by wayland-scanner and included HERE — inside the module purview —
// rather than in the global module fragment above.
//
// That placement is what makes them exportable. wayland-scanner emits them
// `static inline`, and C++ forbids exporting an entity with internal linkage:
//
//     error: using declaration referring to 'wl_surface_attach' with
//            internal linkage cannot be exported
//
// GCC accepts it, clang rejects it, and clang is right. The copy included below
// is the generated header with `static inline` changed to `inline` and nothing
// else; every declaration in it sits inside `extern "C"`, so it keeps C
// language linkage, is not attached to this module, and still matches the
// definitions in wayland-protocol.c.
export {{
#include "wayland-{side}-protocol-module.h"
}}
'''

    out = F / f"mcpp/{side}/src/wayland-{side}.cppm"
    out.write_text(body, encoding="utf-8")
    print(f"{side}: {len(types)+len(funcs)+len(vars_)} re-exported + the protocol header in purview")

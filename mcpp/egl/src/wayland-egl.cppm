// freedesktop.wayland.egl — libwayland-egl, as a C++23 module.
//
// A module wrapper and nothing more: every name below is upstream's, spelled
// upstream's way, with upstream's signature. `import freedesktop.wayland.egl;`
// replaces the #include and changes nothing else.
//
// HAND-WRITTEN, unlike the client and server wrappers, and the reason is scale
// rather than taste. `mcpp/tools/genmod.py` exists because those two re-export
// roughly two hundred names each and a version bump could quietly drop one.
// This header's entire public surface is FOUR functions and one opaque struct
// — small enough to read in one glance and to check by other means, which
// `tests/wayland-egl.cpp` does by taking the address of each.
//
// NO `export inline` FORWARDERS ARE NEEDED HERE. The client and server modules
// need them because wayland-scanner emits its convenience wrappers
// `static inline`, and C++ forbids exporting an entity with internal linkage.
// Nothing in `wayland-egl-core.h` is static: all four are ordinary external
// functions marked WL_EXPORT in the .c file, so a plain `using ::name;`
// re-export is both legal and exact.
//
// WL_EGL_PLATFORM is NOT here and cannot be — it is a macro, and `export`
// names entities. It matters: Mesa's `<EGL/eglplatform.h>` selects the Wayland
// platform typedefs when it is defined, so a consumer that needs it must
// `#define WL_EGL_PLATFORM 1` itself before including EGL's headers, exactly
// as it would with the C header. Same arrangement as the wayland macros, which
// live in `freedesktop.wayland.util` as entities.
module;

#include <wayland-egl-core.h>

export module freedesktop.wayland.egl;

export {

// -- types --
//
// `wl_egl_window` is opaque by design: its definition lives in
// wayland-egl-backend.h, which is the contract between this library and an EGL
// implementation, not part of the client-facing API.
using ::wl_egl_window;

// -- functions --
using ::wl_egl_window_create;
using ::wl_egl_window_destroy;
using ::wl_egl_window_resize;
using ::wl_egl_window_get_attached_size;

} // export

// freedesktop.wayland.cursor — libwayland-cursor, as a C++23 module.
//
// A module wrapper and nothing more: every name below is upstream's, spelled
// upstream's way, with upstream's signature. `import freedesktop.wayland.cursor;`
// replaces the #include and changes nothing else.
//
// HAND-WRITTEN, like the egl wrapper and for the same reason: the public
// surface is two structs and six functions, small enough to read in one glance
// and checked one by one in `tests/`. `mcpp/tools/genmod.py` exists because the
// client and server wrappers re-export roughly two hundred names each.
//
// Nothing here is `static inline`, so every name is a plain `using ::name;` —
// no `export inline` forwarders, unlike the client and server modules where
// wayland-scanner's convenience wrappers force them.
//
// THE TWO STRUCTS ARE EXPORTED AS TYPES, NOT AS OPAQUE HANDLES, and the
// difference matters to a caller: `wl_cursor` and `wl_cursor_image` have their
// fields in the public header — `image_count`, `images`, `hotspot_x` — and a
// client reads them directly to place the pointer. `wl_cursor_theme` is opaque
// and stays so.
module;

#include <wayland-cursor.h>

export module freedesktop.wayland.cursor;

export {

// -- types --
using ::wl_cursor_theme;   // opaque
using ::wl_cursor;         // fields are public: image_count, images, name
using ::wl_cursor_image;   // fields are public: width, height, hotspot_*, delay

// -- functions --
using ::wl_cursor_theme_load;
using ::wl_cursor_theme_destroy;
using ::wl_cursor_theme_get_cursor;
using ::wl_cursor_image_get_buffer;
using ::wl_cursor_frame;
using ::wl_cursor_frame_and_duration;

} // export

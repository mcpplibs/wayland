// freedesktop.wayland.client — libwayland-client, as a C++23 module.
//
// A module wrapper and nothing more: every name below is upstream's, spelled
// upstream's way, with upstream's signature. `import freedesktop.wayland.client;` replaces the
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
// not one. wayland's public macros live in the `freedesktop.wayland.util` module
// (freedesktop.wayland-util), which maps each to the entity it actually is.
module;

// ⭐ THE FOUR wl_fixed_* CONVERSIONS, RENAMED OUT OF THE WAY so this module can
// export them under their real names.
//
// `wayland-util.h` (reached through wayland-client-core.h) defines them
// `static inline`, and C++ forbids exporting internal linkage — so before this
// change a consumer on the MODULE ROUTE could not convert a fixed-point value
// at all. `wl_fixed_t` is how the protocol carries every sub-pixel coordinate:
// wl_pointer motion, touch points, tablet axes. A client that handles pointer
// input was simply stuck.
//
// Nothing here noticed because no test ever converted one.
//
// ⚠️ THEY LIVE HERE, NOT IN freedesktop.wayland.util. That module is
// deliberately header-PAIRED — it exports macros and templates and zero
// `using ::` names, and its own test writes `#include <wayland-util.h>`
// alongside the import. Adding same-name entities there makes every call
// ambiguous on clang:
//
//     error: call to 'wl_fixed_from_int' is ambiguous
//     note: candidate function (module) / candidate function (header)
//
// (GCC accepted it; the llvm leg caught it.) They are also NOT duplicated into
// the server module, because a TU importing both would get the same ambiguity
// between two module-attached entities.
#define wl_fixed_to_double    mcpp_wl_fixed_to_double_static
#define wl_fixed_from_double  mcpp_wl_fixed_from_double_static
#define wl_fixed_to_int       mcpp_wl_fixed_to_int_static
#define wl_fixed_from_int     mcpp_wl_fixed_from_int_static
#include <wayland-client-core.h>
#undef wl_fixed_to_double
#undef wl_fixed_from_double
#undef wl_fixed_to_int
#undef wl_fixed_from_int

export module freedesktop.wayland.client;

// The four, with upstream's bodies from wayland-util.h. No `struct` keyword in
// any signature: that would re-declare the type inside the module purview.
export {
inline double     wl_fixed_to_double(wl_fixed_t f)  { return f / 256.0; }
inline wl_fixed_t wl_fixed_from_double(double d)    { return (wl_fixed_t)(round(d * 256.0)); }
inline int        wl_fixed_to_int(wl_fixed_t f)     { return f / 256; }
inline wl_fixed_t wl_fixed_from_int(int i)          { return i * 256; }
} // export

export {

// -- types --
using ::wl_argument;
using ::wl_array;
using ::wl_dispatcher_func_t;
using ::wl_display;
using ::wl_event_queue;
using ::wl_fixed_t;
using ::wl_interface;
using ::wl_iterator_result;
using ::wl_list;
using ::wl_log_func_t;
using ::wl_message;
using ::wl_object;
using ::wl_proxy;

// -- functions --
using ::wl_array_add;
using ::wl_array_copy;
using ::wl_array_init;
using ::wl_array_release;
using ::wl_display_cancel_read;
using ::wl_display_connect;
using ::wl_display_connect_to_fd;
using ::wl_display_create_queue;
using ::wl_display_create_queue_with_name;
using ::wl_display_disconnect;
using ::wl_display_dispatch;
using ::wl_display_dispatch_pending;
using ::wl_display_dispatch_pending_single;
using ::wl_display_dispatch_queue;
using ::wl_display_dispatch_queue_pending;
using ::wl_display_dispatch_queue_pending_single;
using ::wl_display_dispatch_queue_timeout;
using ::wl_display_dispatch_timeout;
using ::wl_display_flush;
using ::wl_display_get_error;
using ::wl_display_get_fd;
using ::wl_display_get_protocol_error;
using ::wl_display_prepare_read;
using ::wl_display_prepare_read_queue;
using ::wl_display_read_events;
using ::wl_display_roundtrip;
using ::wl_display_roundtrip_queue;
using ::wl_display_set_max_buffer_size;
using ::wl_event_queue_destroy;
using ::wl_event_queue_get_name;
using ::wl_list_empty;
using ::wl_list_init;
using ::wl_list_insert;
using ::wl_list_insert_list;
using ::wl_list_length;
using ::wl_list_remove;
using ::wl_log_set_handler_client;
using ::wl_proxy_add_dispatcher;
using ::wl_proxy_add_listener;
using ::wl_proxy_create;
using ::wl_proxy_create_wrapper;
using ::wl_proxy_destroy;
using ::wl_proxy_get_class;
using ::wl_proxy_get_display;
using ::wl_proxy_get_id;
using ::wl_proxy_get_interface;
using ::wl_proxy_get_listener;
using ::wl_proxy_get_queue;
using ::wl_proxy_get_tag;
using ::wl_proxy_get_user_data;
using ::wl_proxy_get_version;
using ::wl_proxy_marshal;
using ::wl_proxy_marshal_array;
using ::wl_proxy_marshal_array_constructor;
using ::wl_proxy_marshal_array_constructor_versioned;
using ::wl_proxy_marshal_array_flags;
using ::wl_proxy_marshal_constructor;
using ::wl_proxy_marshal_constructor_versioned;
using ::wl_proxy_marshal_flags;
using ::wl_proxy_set_queue;
using ::wl_proxy_set_tag;
using ::wl_proxy_set_user_data;
using ::wl_proxy_wrapper_destroy;

// -- interfaces / data --
}

// The protocol API: wayland-side wrappers and the interface objects,
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
export {
#include "wayland-client-protocol-module.h"
}

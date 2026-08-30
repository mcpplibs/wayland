// wayland.server — libwayland-server, as a C++23 module.
//
// A module wrapper and nothing more: every name below is upstream's, spelled
// upstream's way, with upstream's signature. `import wayland.server;` replaces the
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

#include <wayland-server-core.h>

export module wayland.server;

export {

// -- types --
using ::wl_argument;
using ::wl_array;
using ::wl_client;
using ::wl_client_for_each_resource_iterator_func_t;
using ::wl_dispatcher_func_t;
using ::wl_display;
using ::wl_display_global_filter_func_t;
using ::wl_event_loop;
using ::wl_event_loop_fd_func_t;
using ::wl_event_loop_idle_func_t;
using ::wl_event_loop_signal_func_t;
using ::wl_event_loop_timer_func_t;
using ::wl_event_source;
using ::wl_fixed_t;
using ::wl_global;
using ::wl_global_bind_func_t;
using ::wl_global_withdrawn_func_t;
using ::wl_interface;
using ::wl_iterator_result;
using ::wl_list;
using ::wl_listener;
using ::wl_log_func_t;
using ::wl_message;
using ::wl_notify_func_t;
using ::wl_object;
using ::wl_protocol_logger;
using ::wl_protocol_logger_func_t;
using ::wl_protocol_logger_message;
using ::wl_protocol_logger_type;
using ::wl_resource;
using ::wl_resource_destroy_func_t;
using ::wl_shm_buffer;
using ::wl_shm_pool;
using ::wl_signal;
using ::wl_user_data_destroy_func_t;

// -- functions --
using ::wl_array_add;
using ::wl_array_copy;
using ::wl_array_init;
using ::wl_array_release;
using ::wl_client_add_destroy_late_listener;
using ::wl_client_add_destroy_listener;
using ::wl_client_add_resource_created_listener;
using ::wl_client_create;
using ::wl_client_destroy;
using ::wl_client_flush;
using ::wl_client_for_each_resource;
using ::wl_client_from_link;
using ::wl_client_get_credentials;
using ::wl_client_get_destroy_late_listener;
using ::wl_client_get_destroy_listener;
using ::wl_client_get_display;
using ::wl_client_get_fd;
using ::wl_client_get_link;
using ::wl_client_get_object;
using ::wl_client_get_user_data;
using ::wl_client_post_implementation_error;
using ::wl_client_post_no_memory;
using ::wl_client_set_max_buffer_size;
using ::wl_client_set_user_data;
using ::wl_display_add_client_created_listener;
using ::wl_display_add_destroy_listener;
using ::wl_display_add_protocol_logger;
using ::wl_display_add_shm_format;
using ::wl_display_add_socket;
using ::wl_display_add_socket_auto;
using ::wl_display_add_socket_fd;
using ::wl_display_create;
using ::wl_display_destroy;
using ::wl_display_destroy_clients;
using ::wl_display_flush_clients;
using ::wl_display_get_client_list;
using ::wl_display_get_destroy_listener;
using ::wl_display_get_event_loop;
using ::wl_display_get_serial;
using ::wl_display_init_shm;
using ::wl_display_next_serial;
using ::wl_display_remove_socket_fd;
using ::wl_display_run;
using ::wl_display_set_default_max_buffer_size;
using ::wl_display_set_global_filter;
using ::wl_display_terminate;
using ::wl_event_loop_add_destroy_listener;
using ::wl_event_loop_add_fd;
using ::wl_event_loop_add_idle;
using ::wl_event_loop_add_signal;
using ::wl_event_loop_add_timer;
using ::wl_event_loop_create;
using ::wl_event_loop_destroy;
using ::wl_event_loop_dispatch;
using ::wl_event_loop_dispatch_idle;
using ::wl_event_loop_get_destroy_listener;
using ::wl_event_loop_get_fd;
using ::wl_event_source_check;
using ::wl_event_source_fd_update;
using ::wl_event_source_remove;
using ::wl_event_source_timer_update;
using ::wl_fixes_handle_ack_global_remove;
using ::wl_global_create;
using ::wl_global_destroy;
using ::wl_global_get_display;
using ::wl_global_get_interface;
using ::wl_global_get_name;
using ::wl_global_get_user_data;
using ::wl_global_get_version;
using ::wl_global_remove;
using ::wl_global_set_user_data;
using ::wl_global_set_withdrawn_listener;
using ::wl_list_empty;
using ::wl_list_init;
using ::wl_list_insert;
using ::wl_list_insert_list;
using ::wl_list_length;
using ::wl_list_remove;
using ::wl_log_set_handler_server;
using ::wl_protocol_logger_destroy;
using ::wl_resource_add_destroy_listener;
using ::wl_resource_create;
using ::wl_resource_destroy;
using ::wl_resource_find_for_client;
using ::wl_resource_from_link;
using ::wl_resource_get_class;
using ::wl_resource_get_client;
using ::wl_resource_get_destroy_listener;
using ::wl_resource_get_id;
using ::wl_resource_get_interface;
using ::wl_resource_get_link;
using ::wl_resource_get_user_data;
using ::wl_resource_get_version;
using ::wl_resource_instance_of;
using ::wl_resource_post_error;
using ::wl_resource_post_error_vargs;
using ::wl_resource_post_event;
using ::wl_resource_post_event_array;
using ::wl_resource_post_no_memory;
using ::wl_resource_queue_event;
using ::wl_resource_queue_event_array;
using ::wl_resource_set_destructor;
using ::wl_resource_set_dispatcher;
using ::wl_resource_set_implementation;
using ::wl_resource_set_user_data;
using ::wl_shm_buffer_begin_access;
using ::wl_shm_buffer_create;
using ::wl_shm_buffer_end_access;
using ::wl_shm_buffer_get;
using ::wl_shm_buffer_get_data;
using ::wl_shm_buffer_get_format;
using ::wl_shm_buffer_get_height;
using ::wl_shm_buffer_get_stride;
using ::wl_shm_buffer_get_width;
using ::wl_shm_buffer_ref;
using ::wl_shm_buffer_ref_pool;
using ::wl_shm_buffer_unref;
using ::wl_shm_pool_unref;
using ::wl_signal_emit_mutable;

// -- interfaces / data --
}

// The protocol API: server-side wrappers and the interface objects,
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
#include "wayland-server-protocol-module.h"
}

/* config.h — the probe results meson's configure_file() would write, for
 * linux/glibc. Committed rather than generated: this fork is pinned to one
 * upstream version, and every entry below is a property of glibc, not of the
 * machine doing the build.
 *
 * It lives under mcpp/include/ instead of at the source root so that a meson
 * build of this same tree still uses its own generated config.h; the quote-form
 * `#include "config.h"` in wayland-shm.c falls through to this one only because
 * mcpp puts this directory on the include path.
 *
 * The test forms differ per macro and both spellings appear in the sources:
 *   #ifdef HAVE_ACCEPT4 / HAVE_MEMFD_CREATE   -> must be DEFINED
 *   #ifndef HAVE_STRNDUP                      -> must be DEFINED (glibc has it)
 *   #if HAVE_XUCRED_CR_PID / HAVE_BROKEN_...  -> must be defined and 0
 */
#pragma once

#define PACKAGE         "wayland"
#define PACKAGE_VERSION "1.26.0"

#define HAVE_SYS_PRCTL_H 1
/* sys/procctl.h and sys/ucred.h are BSD; absent on linux. */

#define HAVE_ACCEPT4         1
#define HAVE_MKOSTEMP        1
#define HAVE_POSIX_FALLOCATE 1
#define HAVE_PRCTL           1
#define HAVE_MEMFD_CREATE    1
#define HAVE_MREMAP          1
#define HAVE_STRNDUP         1
/* new in 1.26: wayland-server uses it for the client thread id. */
#define HAVE_GETTID          1

/* `struct xucred` is FreeBSD's; the socket peer-credential path uses
 * SO_PEERCRED on linux instead. */
#define HAVE_XUCRED_CR_PID 0

/* The FreeBSD MSG_CMSG_CLOEXEC workaround. */
#define HAVE_BROKEN_MSG_CMSG_CLOEXEC 0

# Generated protocol code

Produced by `wayland-scanner` from `upstream/protocol/wayland.xml`. **Do not
edit** — regenerate with the scanner this repository builds:

```bash
mcpp build -p mcpp/scanner
S=$(find mcpp/scanner/target -name wayland-scanner -type f | head -1)
X=upstream/protocol/wayland.xml

"$S" -s public-code    "$X" mcpp/generated/wayland-protocol.c
for side in client server; do
  "$S" -s $side-header    "$X" mcpp/generated/wayland-$side-protocol.h
  "$S" -s $side-header -c "$X" mcpp/generated/wayland-$side-protocol-core.h
done
```

CI does exactly that and diffs the result, so these files cannot drift from
`wayland.xml` or from the scanner's version.

## Why they are checked in rather than generated during the build

`wayland-client.h` does `#include "wayland-client-protocol.h"`, so these
headers are part of the package's **public interface** — every consumer that
includes `<wayland-client.h>` needs them on its include path. A `build.mcpp`
cannot provide that: `mcpp::include_dir()` colours only the package's own
translation units and is deliberately not propagated to consumers, which is
the documented rule (docs/07-build-mcpp.md: "An include directory consumers
must see is part of the public interface and belongs in the declarative
manifest").

Generating them at build time also raced the package's own compiles — an
action's outputs become ninja nodes but get no order-only edge into them
(mcpp-community/mcpp#534).

They are deterministic from a pinned `wayland.xml`, so checking them in costs
nothing and removes both problems. `freedesktop.wayland-scanner` remains a
package in its own right: a compositor generating bindings for
`wayland-protocols` XML needs exactly that tool, and asking for it by name
pins its version to this one.

# Pumpkin Plugin API for C

This package provides everything needed to write a Pumpkin server plugin compiled to WebAssembly using C or C++.

## Quick start

1. Download the latest release of this package.
```bash
# Assuming the latest version is 0.1.0-dev1
curl -OL https://github.com/Pumpkin-MC/pumpkin-api-c/releases/download/v0.1.0-dev1/pumpkin-api-0.1.0-dev1.tar.xz
tar -xJvf pumpkin-api-0.1.0-dev1.tar.xz

# Move and/or name it somewhere/something predictable
mv pumpkin-api-0.1.0-dev1 pumpkin-api
```

2. Create your plugin (`main.c`):

```c
#include "pumpkin_api.h"
#include <stdio.h>

static void on_player_join(
    pumpkin_plugin_server_borrow_server_t server,
    plugin_event_t *event
) {
    (void)server;

    if (event->tag == PUMPKIN_PLUGIN_EVENT_EVENT_PLAYER_JOIN_EVENT) {
        printf("A player joined the server!\n");
    }
}

pumpkin_metadata_t get_meta(void) {
    static const char* authors[] = {"you"};
    return (pumpkin_metadata_t) {
        .name = "my-c-plugin",
        .version = "0.1.0",
        .authors = authors,
        .authors_count = 1,
        .description = "A simple C plugin for Pumpkin",
        .dependencies_count = 0,
        .permissions_count = 0
    };
}

void on_load(plugin_own_context_t ctx) {
    pumpkin_plugin_context_borrow_context_t context =
        pumpkin_plugin_context_borrow_context(ctx);

    uint32_t handler_id = pumpkin_register_event_handler(
        context,
        on_player_join,
        PUMPKIN_PLUGIN_EVENT_EVENT_TYPE_PLAYER_JOIN_EVENT,
        PUMPKIN_PLUGIN_EVENT_EVENT_PRIORITY_NORMAL,
        false
    );

    if (handler_id == PUMPKIN_INVALID_HANDLER_ID) {
        printf("Failed to register player join handler\n");
        return;
    }

    printf("C plugin loaded!\n");
}

REGISTER_PUMPKIN_PLUGIN(((pumpkin_plugin_t){
    .get_metadata = get_meta,
    .on_load = on_load
}))
```

3. Build your plugin into a WebAssembly component:

To build for Pumpkin, you'll need the `wasi-sdk`.

```bash
# Compile to Wasm using wasi-sdk
/path/to/wasi-sdk/bin/wasm32-wasip2-clang -Oz \
    -Ipumpkin-api/include \
    pumpkin-api/lib/libpumpkin-api.a \
    main.c \
    -o my_plugin.wasm \
    -mexec-model=reactor
```

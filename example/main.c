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

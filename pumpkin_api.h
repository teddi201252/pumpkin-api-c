#ifndef PUMPKIN_API_H
#define PUMPKIN_API_H

#include "plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char* name;
    const char* version;

    const char** authors;
    size_t authors_count;

    const char* description;

    const char** dependencies;
    size_t dependencies_count;

    const char** permissions;
    size_t permissions_count;
} pumpkin_metadata_t;

typedef void (*pumpkin_on_load_t)(plugin_own_context_t ctx);
typedef void (*pumpkin_on_unload_t)(plugin_own_context_t ctx);
typedef pumpkin_metadata_t (*pumpkin_get_metadata_t)(void);

typedef void (*pumpkin_event_handler_t)(
    pumpkin_plugin_server_borrow_server_t server,
    plugin_event_t *event
);

typedef bool (*pumpkin_command_handler_t)(
    pumpkin_plugin_command_borrow_command_sender_t sender,
    pumpkin_plugin_server_borrow_server_t server,
    pumpkin_plugin_command_borrow_consumed_args_t args,
    int32_t *result,
    plugin_command_error_t *error
);

/** Returned when an event handler could not be registered. */
#define PUMPKIN_INVALID_HANDLER_ID UINT32_MAX

typedef struct {
    pumpkin_get_metadata_t get_metadata;
    pumpkin_on_load_t on_load;
    pumpkin_on_unload_t on_unload;
} pumpkin_plugin_t;

void pumpkin_register_plugin(pumpkin_plugin_t plugin);

uint32_t pumpkin_register_event_handler(
    pumpkin_plugin_context_borrow_context_t context,
    pumpkin_event_handler_t handler,
    pumpkin_plugin_context_event_type_t event_type,
    pumpkin_plugin_context_event_priority_t event_priority,
    bool blocking
);

uint32_t pumpkin_command_execute(
    pumpkin_plugin_command_borrow_command_t command,
    pumpkin_command_handler_t handler
);

uint32_t pumpkin_command_node_execute(
    pumpkin_plugin_command_borrow_command_node_t command_node,
    pumpkin_command_handler_t handler
);

#define REGISTER_PUMPKIN_PLUGIN(plugin) \
    void exports_plugin_init_plugin(void) { \
        pumpkin_register_plugin(plugin); \
    }

#ifdef __cplusplus
}
#endif

#endif

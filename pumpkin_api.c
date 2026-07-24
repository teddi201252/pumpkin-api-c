#include "pumpkin_api.h"
#include <stdlib.h>
#include <string.h>

static pumpkin_plugin_t g_plugin = {0};
static pumpkin_event_handler_t *g_event_handlers = NULL;
static size_t g_event_handler_count = 0;

void pumpkin_register_plugin(pumpkin_plugin_t plugin) {
    g_plugin = plugin;
}

uint32_t pumpkin_register_event_handler(
    pumpkin_plugin_context_borrow_context_t context,
    pumpkin_event_handler_t handler,
    pumpkin_plugin_context_event_type_t event_type,
    pumpkin_plugin_context_event_priority_t event_priority,
    bool blocking
) {
    if (handler == NULL ||
        g_event_handler_count >= UINT32_MAX ||
        g_event_handler_count >= SIZE_MAX / sizeof(*g_event_handlers)) {
        return PUMPKIN_INVALID_HANDLER_ID;
    }

    size_t new_count = g_event_handler_count + 1;
    pumpkin_event_handler_t *handlers = realloc(
        g_event_handlers,
        new_count * sizeof(*handlers)
    );
    if (handlers == NULL) {
        return PUMPKIN_INVALID_HANDLER_ID;
    }

    uint32_t handler_id = (uint32_t)g_event_handler_count;
    g_event_handlers = handlers;
    g_event_handlers[handler_id] = handler;
    g_event_handler_count = new_count;

    pumpkin_plugin_context_method_context_register_event(
        context,
        handler_id,
        event_type,
        event_priority,
        blocking
    );

    return handler_id;
}

// WIT exports

void exports_pumpkin_plugin_metadata_get_metadata(exports_pumpkin_plugin_metadata_plugin_metadata_t *ret) {
    if (g_plugin.get_metadata) {
        pumpkin_metadata_t meta = g_plugin.get_metadata();
        plugin_string_dup(&(ret->name), meta.name);
        plugin_string_dup(&(ret->version), meta.version);

        ret->authors.len = meta.authors_count;
        ret->authors.ptr = (plugin_string_t*)malloc(ret->authors.len * sizeof(plugin_string_t));
        for (size_t i = 0; i < ret->authors.len; ++i) {
            plugin_string_dup(&(ret->authors.ptr[i]), meta.authors[i]);
        }

        plugin_string_dup(&(ret->description), meta.description);

        ret->dependencies.len = meta.dependencies_count;
        ret->dependencies.ptr = (plugin_string_t*)malloc(ret->dependencies.len * sizeof(plugin_string_t));
        for (size_t i = 0; i < ret->dependencies.len; ++i) {
            plugin_string_dup(&(ret->dependencies.ptr[i]), meta.dependencies[i]);
        }

        ret->permissions.len = meta.permissions_count;
        ret->permissions.ptr = (plugin_string_t*)malloc(ret->permissions.len * sizeof(plugin_string_t));
        for (size_t i = 0; i < ret->permissions.len; ++i) {
            plugin_string_dup(&(ret->permissions.ptr[i]), meta.permissions[i]);
        }
    }
}

bool exports_plugin_on_load(plugin_own_context_t context_handle, plugin_string_t *err) {
    if (g_plugin.on_load) {
        g_plugin.on_load(context_handle);
    }
    return true;
}

bool exports_plugin_on_unload(plugin_own_context_t context_handle, plugin_string_t *err) {
    if (g_plugin.on_unload) {
        g_plugin.on_unload(context_handle);
    }

    free(g_event_handlers);
    g_event_handlers = NULL;
    g_event_handler_count = 0;

    return true;
}

void exports_plugin_handle_event(uint32_t event_id, plugin_own_server_instance_t server, plugin_event_t *event, plugin_event_t *ret) {
    *ret = *event;

    if (event_id < g_event_handler_count) {
        g_event_handlers[event_id](
            pumpkin_plugin_server_borrow_server(server),
            ret
        );
    }

    pumpkin_plugin_server_server_drop_own(server);
}

bool exports_plugin_handle_command(uint32_t command_id, plugin_own_command_sender_t sender, plugin_own_server_instance_t server, plugin_own_consumed_args_t args, int32_t *ret, plugin_command_error_t *err) {
    err->tag = PUMPKIN_PLUGIN_COMMAND_COMMAND_ERROR_COMMAND_FAILED;

    plugin_string_t str;
    plugin_string_set(&str, "Command not implemented");

    pumpkin_plugin_text_own_text_component_t text = pumpkin_plugin_text_static_text_component_text(&str);
    err->val.command_failed = text;

    return false;
}

void exports_plugin_handle_task(uint32_t handler_id, plugin_own_server_instance_t server) {
}

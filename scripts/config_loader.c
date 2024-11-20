#include <json-c/json.h>
#include "../common/config.h"

typedef struct {
    json_object *config;
    char *file_path;
} ConfigContext;

static ConfigContext client_ctx = {NULL, NULL};
static ConfigContext server_ctx = {NULL, NULL};

// Load configuration file
int load_config(const char *file_path, ConfigContext *ctx) {
    json_object *config = json_object_from_file(file_path);
    if (!config) {
        return -1;
    }
    
    if (ctx->config) {
        json_object_put(ctx->config);
    }
    
    ctx->config = config;
    ctx->file_path = strdup(file_path);
    return 0;
}

// Get config value helpers
const char* get_string_config(ConfigContext *ctx, const char *path) {
    json_object *value;
    if (json_pointer_get(ctx->config, path, &value) == 0) {
        return json_object_get_string(value);
    }
    return NULL;
}

int get_int_config(ConfigContext *ctx, const char *path, int default_value) {
    json_object *value;
    if (json_pointer_get(ctx->config, path, &value) == 0) {
        return json_object_get_int(value);
    }
    return default_value;
}

// Cleanup
void config_cleanup(ConfigContext *ctx) {
    if (ctx->config) {
        json_object_put(ctx->config);
        ctx->config = NULL;
    }
    if (ctx->file_path) {
        free(ctx->file_path);
        ctx->file_path = NULL;
    }
}
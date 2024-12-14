#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct terminal_context;

struct terminal_config
{
    size_t tab_size;
    uint32_t colour_palette[8];
    uint32_t bright_colour_palette[8];
    uint32_t default_fg_colour;
    uint32_t default_bg_colour;
    uint32_t cursor_colour;
    size_t width, height;
    void (*write_char)(struct terminal_context*, char c);
    void (*enable_cursor)(struct terminal_context*);
    void (*disable_cursor)(struct terminal_context*);
    void (*update_cursor)(struct terminal_context*);
    void (*scroll)(struct terminal_context*, size_t n_lines);
    void (*clear)(struct terminal_context*, size_t x, size_t y, size_t width, size_t height);
    void (*flush)(struct terminal_context*);
};

struct terminal_context
{
    const struct terminal_config* config;
    void* renderer_context;

    uint32_t fg_colour;
    uint32_t bg_colour;
    bool is_bright;
    size_t cursor_x, cursor_y;
};

static inline struct terminal_context terminal_create(struct terminal_config* config, void* renderer_context)
{
    struct terminal_context context;

    context.config = config;
    context.fg_colour = config->default_fg_colour;
    context.bg_colour = config->default_bg_colour;
    context.is_bright = false;
    context.cursor_x = 0;
    context.cursor_y = 0;
    context.renderer_context = renderer_context;

    config->clear(&context, 0, 0, config->width, config->height);

    return context;
}

void terminal_write(struct terminal_context* context, const char* msg, size_t msg_len);

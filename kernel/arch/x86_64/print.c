#include <limine/limine.h>
#include <string.h>

#include "kernel/arch/x86_64/com.h"
#include "lib/liback/terminal.h"
#include "kernel/arch/x86_64/char_display.h"

static bool terminal_initialised = false;
struct terminal_context terminal_context;
static struct terminal_config terminal_config;

void write_string_to_screen(const char* msg, size_t msg_len)
{
    if (terminal_initialised)
    {
        terminal_write(&terminal_context, msg, msg_len);
    }

    com_write(COM_PORT1, msg, msg_len);
}

static void write_char(struct terminal_context* context, char c)
{
    //kprintf(KERN_DEBUG "writing '%c', context: { .fg_colour = %#x, .bg_colour = %#x, is_bright: %s, cursor_xy: (%zu, %zu) }\n", c, context->fg_colour, context->bg_colour, context->is_bright ? "yes" : "no", context->cursor_x, context->cursor_y);
    char_display_draw_char(context->cursor_x, context->cursor_y, c, context->bg_colour, context->fg_colour);
}

static void flush(struct terminal_context* context)
{
    char_display_flush();
}

static void scroll(struct terminal_context* context, size_t n_lines)
{
    char_display_scroll(n_lines, context->bg_colour);

    //kprintf(KERN_DEBUG "scrolled %zu lines, context: { .fg_colour = %#x, .bg_colour = %#x, is_bright: %s, cursor_xy: (%zu, %zu) }, config { width: %zu, height: %zu }\n", n_lines, context->fg_colour, context->bg_colour, context->is_bright ? "yes" : "no", context->cursor_x, context->cursor_y, context->config->width, context->config->height);
}

static void enable_cursor(struct terminal_context* context)
{
    char_display_enable_cursor();
}

static void disable_cursor(struct terminal_context* context)
{
    char_display_disable_cursor();
}

static void update_cursor(struct terminal_context* context)
{
    char_display_update_cursor(context->cursor_x, context->cursor_y, context->config->cursor_colour);
}

bool init_terminal_functionality(void)
{
    if (!init_char_display()) return false;

    terminal_config = (struct terminal_config) {
        .tab_size = 4,
        .colour_palette = {
            0xffffffff,
            0xffcd3131,
            0xff0dbc79,
            0xffe5e510,
            0xff2472c8,
            0xffbc3fbc,
            0xff11a8cd,
            0xffe5e5e5
        },
        .bright_colour_palette = {
            0xff666666,
            0xfff14c4c,
            0xff23d18b,
            0xfff5f543,
            0xff3b8eea,
            0xffd670d6,
            0xff29b8db,
            0xffe5e5e5
        },
        .default_fg_colour = 0xffffffff,
        .default_bg_colour = 0xff000000,
        .cursor_colour = 0x77ffffff,
        .write_char = write_char,
        .scroll = scroll,
        .enable_cursor = enable_cursor,
        .disable_cursor = disable_cursor,
        .update_cursor = update_cursor,
        .flush = flush
    };

    char_display_get_width_and_height(&terminal_config.width, &terminal_config.height);

    terminal_context = terminal_create(&terminal_config, NULL);
    terminal_initialised = true;

    terminal_config.enable_cursor(&terminal_context);

    return true;
}

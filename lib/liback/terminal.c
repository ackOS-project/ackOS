#include <stdlib.h>

#include "lib/liback/terminal.h"

#define ASCII_ESCAPE '\e'
#define MAX_ESCAPE_PARAM 16
#define FROM_RGB(R, G, B) (0xff << 24 | ((uint32_t)R << 16) | ((uint32_t)G << 8) | (uint32_t)B)

size_t parse_int(const char* str, size_t max_len, size_t* i)
{
    size_t res = 0;

    for (; *i < max_len && str[*i] >= '0' && str[*i] <= '9'; (*i)++)
    {
        res *= 10;
        res += str[*i] - '0';
    }

    return res;
}

void terminal_show_cursor(struct terminal_context* context)
{
    context->config->enable_cursor(context);
}

void terminal_hide_cursor(struct terminal_context* context)
{
    context->config->disable_cursor(context);
}

void terminal_move_cursor_up(struct terminal_context* context, size_t n)
{
    if (context->cursor_y < n) return;

    context->cursor_y -= n;
}

void terminal_move_cursor_down(struct terminal_context* context, size_t n)
{
    if ((context->cursor_y + n) >= context->config->height) return;

    context->cursor_y += n;
}

void terminal_move_cursor_forward(struct terminal_context* context, size_t n)
{
    if ((context->cursor_x + n) >= context->config->width) return;

    context->cursor_x += n;
}

void terminal_move_cursor_back(struct terminal_context* context, size_t n)
{
    if (context->cursor_x < n) return;

    context->cursor_x -= n;
}

void terminal_move_cursor(struct terminal_context* context, size_t n, size_t m)
{
    size_t x = n == 0 ? n : n - 1;
    size_t y = m == 0 ? n : n - 1;

    if (x >= context->config->width || y >= context->config->height) return;

    context->cursor_x = x;
    context->cursor_y = y;
}

void terminal_clear(struct terminal_context* context, size_t n)
{
    if (n == 0) /* Erase everything between cursor and bottom */
    {
        context->config->clear(context, context->cursor_x, context->cursor_y, context->config->width - context->cursor_x, 1);

        size_t remaining_height = context->config->height - 1 - context->cursor_y;

        if (remaining_height)
        {
            context->config->clear(context, 0, context->cursor_y + 1, context->config->width, remaining_height);
        }
    }
    else if (n == 1) /* erase everything before cursor */
    {
        context->config->clear(context, 0, context->cursor_y, context->cursor_x, 1);

        size_t remaining_height = context->cursor_y;

        if (remaining_height)
        {
            context->config->clear(context, 0, 0, context->config->width, remaining_height);
        }
    }
    else if (n == 2) /* erase entire screen */
    {
        context->config->clear(context, 0, 0, context->config->width, context->config->height);
    }
}

static void terminal_parse_sgr(struct terminal_context* context, size_t escape_params[], size_t escape_param_n)
{
    for (size_t i = 0; i < escape_param_n;)
    {
        size_t param1 = escape_params[i++];

        if (param1 == 0)
        {
            context->fg_colour = context->config->default_fg_colour;
            context->bg_colour = context->config->default_bg_colour;
            context->is_bright = false;
        }
        else if (param1 == 1)
        {
            context->is_bright = true;
        }
        else if (param1 >= 30 && param1 <= 37)
        {
            uint8_t code = param1 - 30;

            context->fg_colour = context->is_bright ? context->config->bright_colour_palette[code] : context->config->colour_palette[code];
        }
        else if (param1 == 38)
        {
            size_t param2 = escape_params[i++];

            if (param2 == 2 && (escape_param_n - i) >= 3)
            {
                uint8_t r = escape_params[i++];
                uint8_t g = escape_params[i++];
                uint8_t b = escape_params[i++];

                context->fg_colour = FROM_RGB(r, g, b);
            }
        }
        else if (param1 == 39)
        {
            context->fg_colour = context->config->default_fg_colour;
        }
        else if (param1 >= 40 && param1 <= 47)
        {
            uint8_t code = param1 - 40;

            context->bg_colour = context->is_bright ? context->config->bright_colour_palette[code] : context->config->colour_palette[code];
        }
        else if (param1 == 48)
        {
            size_t param2 = escape_params[i++];

            if (param2 == 2 && (escape_param_n - i) >= 3)
            {
                uint8_t r = escape_params[i++];
                uint8_t g = escape_params[i++];
                uint8_t b = escape_params[i++];

                context->bg_colour = FROM_RGB(r, g, b);
            }
        }
        else if (param1 == 49)
        {
            context->bg_colour = context->config->default_bg_colour;
        }
    }
}

void terminal_write(struct terminal_context* context, const char* msg, size_t msg_len)
{
    for (size_t i = 0; i < msg_len;)
    {
        char c0 = msg[i++];

        if (c0 == ASCII_ESCAPE && i < msg_len && msg[i] >= 0x20 && msg[i] < 0x7F)
        {
            char c1 = msg[i++];

            if (c1 == '[') /* Control Sequence Introducer */
            {
                bool is_private = msg[i] == '?';

                if (is_private)
                {
                    i++;
                }

                size_t escape_params[MAX_ESCAPE_PARAM];
                size_t escape_param_n = 0;

                while (escape_param_n < MAX_ESCAPE_PARAM)
                {
                    escape_params[escape_param_n++] = parse_int(msg, msg_len, &i);

                    if (msg[i] == ';')
                    {
                        i++;
                    }
                    else
                    {
                        break;
                    }
                }

                char action = msg[i++];

                if (is_private)
                {
                    size_t code = escape_param_n > 0 ? escape_params[0] : 0;

                    if (code == 25 && action == 'h')
                    {
                        terminal_show_cursor(context);
                    }
                    if (code == 25 && action == 'l')
                    {
                        terminal_hide_cursor(context);
                    }
                }
                else
                {
                    if (action == 'A')
                    {
                        terminal_move_cursor_up(context, escape_param_n > 0 ? escape_params[0] : 1);
                    }
                    else if (action == 'B')
                    {
                        terminal_move_cursor_down(context, escape_param_n > 0 ? escape_params[0] : 1);
                    }
                    else if (action == 'C')
                    {
                        terminal_move_cursor_forward(context, escape_param_n > 0 ? escape_params[0] : 1);
                    }
                    else if (action == 'D')
                    {
                        terminal_move_cursor_back(context, escape_param_n > 0 ? escape_params[0] : 1);
                    }
                    else if (action == 'J')
                    {
                        terminal_clear(context, escape_param_n > 0 ? escape_params[0] : 0);
                    }
                    else if (action == 'H')
                    {
                        terminal_move_cursor(context, escape_param_n > 0 ? escape_params[0] : 1, escape_param_n > 1 ? escape_params[1] : 1);
                    }
                    else if (action == 'm')
                    {
                        terminal_parse_sgr(context, escape_params, escape_param_n);
                    }
                }
            }
        }
        else if (c0 == '\r')
        {
            context->cursor_x = 0;
        }
        else if (c0 == '\n')
        {
            // scroll if necessary
            context->cursor_x = 0;
            context->cursor_y++;
        }
        else if (c0 == '\t')
        {
            if ((context->config->width - context->cursor_x) < context->config->tab_size)
            {
                context->cursor_y++;
                context->cursor_x = 0;
            }

            context->cursor_x += context->config->tab_size;
        }
        else
        {
            context->config->write_char(context, c0);

            if (context->cursor_x == (context->config->width - 1))
            {
                context->cursor_x = 0;
                context->cursor_y++;
            }
            else
            {
                context->cursor_x++;
            }
        }

        if (context->cursor_y >= context->config->height)
        {
            size_t diff = context->config->height - context->cursor_y + 1;

            context->cursor_y = context->config->height - 1;

            context->config->scroll(context, diff);
        }
    }

    context->config->update_cursor(context);
    context->config->flush(context);
}

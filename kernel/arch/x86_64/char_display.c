#include "kernel/arch/x86_64/char_display.h"
#include "kernel/arch/x86_64/fb.h"
#include "kernel/arch/x86_64/simple_alloc.h"
#include "kernel/psf.h"

struct char_display_context
{
    struct framebuffer fb;
    struct psf_font_metrics font;
    size_t cursor_x, cursor_y;
    uint32_t cursor_colour;
    uint32_t* pixels_under_cursor;
    bool cursor_enabled;
};

static struct char_display_context char_display;

bool init_char_display(void)
{
    if (framebuffer_count() == 0 || !psf_is_text_rendering_available()) return false;

    char_display.fb = framebuffer_allocate(0);
    char_display.font = psf_get_font_metrics();
    char_display.cursor_x = 0;
    char_display.cursor_y = 0;
    char_display.cursor_colour = 0xffffffff;
    char_display.pixels_under_cursor = simple_allocate(char_display.font.bytes_per_glyph);
    char_display.cursor_enabled = false;

    return true;
}

void char_display_get_width_and_height(size_t* width, size_t* height)
{
    *width = char_display.fb.width / char_display.font.glyph_width;
    *height = char_display.fb.height / char_display.font.glyph_height;
}

static void char_display_remove_cursor(void);
static void char_display_add_cursor(void);

void char_display_draw_char(size_t x, size_t y, char c, uint32_t bg, uint32_t fg)
{
    struct psf_glyph_metrics metrics = psf_get_glyph_metrics(c);

    uint32_t pixels[metrics.width * metrics.height];
    psf_render_glyph(metrics, fg, bg, pixels, metrics.width, 0, 0);

    framebuffer_composite(&char_display.fb, pixels, metrics.width, metrics.height, char_display.font.glyph_width * x, char_display.font.glyph_height * y);

    if (char_display.cursor_enabled && x == char_display.cursor_x && y == char_display.cursor_y)
    {
        char_display_add_cursor();
    }
}

void char_display_scroll(size_t line_n, uint32_t bg)
{
    // Move the section:
    // (0, char_display.font.glyph_height * line_n)
    // width: char_display.fb.width
    // height: char_display.fb.height - char_display.font.glyph_height * line_n
    // To:
    // (0, 0)
    // Clear the section:
    // (0, char_display.fb.height - char_display.font.glyph_height * line_n)
    // width: char_display.fb.width
    // height: char_display.font.glyph_height * line_n

    char_display_remove_cursor();

    size_t main_sec_y = char_display.font.glyph_height * line_n;
    size_t main_sec_width = char_display.fb.width;
    size_t main_sec_height = char_display.fb.height - char_display.font.glyph_height * line_n;

    for (size_t y = 0; y < main_sec_height; y++)
    {
        for (size_t x = 0; x < main_sec_width; x++)
        {
            uint32_t pixel = char_display.fb.backbuffer[(main_sec_y + y) * char_display.fb.width + x];

            char_display.fb.backbuffer[y * char_display.fb.width + x] = pixel;
        }
    }

    framebuffer_fill(&char_display.fb, bg, char_display.fb.width, char_display.font.glyph_height * line_n, 0, char_display.fb.height - char_display.font.glyph_height * line_n);

    char_display_add_cursor();
}

void char_display_clear(uint32_t bg)
{
    framebuffer_fill(&char_display.fb, bg, char_display.fb.width, char_display.fb.height, 0, 0);

    char_display_add_cursor();
}

static void char_display_remove_cursor(void)
{
    if (!char_display.cursor_enabled) return;

    size_t start_x = char_display.font.glyph_width * char_display.cursor_x;
    size_t start_y = char_display.font.glyph_height * char_display.cursor_y;
    
    for (size_t y = 0; y < char_display.font.glyph_height; y++)
    {
        for (size_t x = 0; x < char_display.font.glyph_width; x++)
        {
            char_display.fb.backbuffer[(start_y + y) * char_display.fb.width + (start_x + x)] = char_display.pixels_under_cursor[y * char_display.font.glyph_width + x];
        }
    }
}

static void char_display_add_cursor(void)
{
    if (!char_display.cursor_enabled) return;

    size_t start_x = char_display.font.glyph_width * char_display.cursor_x;
    size_t start_y = char_display.font.glyph_height * char_display.cursor_y;
    
    for (size_t y = 0; y < char_display.font.glyph_height; y++)
    {
        for (size_t x = 0; x < char_display.font.glyph_width; x++)
        {
            char_display.pixels_under_cursor[y * char_display.font.glyph_width + x] = char_display.fb.backbuffer[(start_y + y) * char_display.fb.width + (start_x + x)];
        }
    }

    framebuffer_fill(&char_display.fb, char_display.cursor_colour, char_display.font.glyph_width, char_display.font.glyph_height, start_x, start_y);
}

#include "kernel/lib/log.h"

void char_display_enable_cursor()
{
    if (char_display.cursor_enabled) return;

    char_display.cursor_enabled = true;

    char_display_add_cursor();
}

void char_display_disable_cursor(void)
{
    if (!char_display.cursor_enabled) return;

    char_display_remove_cursor();

    char_display.cursor_enabled = false;
}

void char_display_update_cursor(size_t x, size_t y, uint32_t colour)
{
    char_display_remove_cursor();

    char_display.cursor_x = x;
    char_display.cursor_y = y;
    char_display.cursor_colour = colour;

    char_display_add_cursor();
}

void char_display_flush(void)
{
    framebuffer_flush(&char_display.fb);
}
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

bool init_char_display(void);

void char_display_get_width_and_height(size_t* width, size_t* height);
void char_display_draw_char(size_t x, size_t y, char c, uint32_t bg, uint32_t fg);
void char_display_scroll(size_t line_n, uint32_t bg);
void char_display_clear(uint32_t bg);
void char_display_enable_cursor(void);
void char_display_disable_cursor(void);
void char_display_update_cursor(size_t x, size_t y, uint32_t colour);
void char_display_flush(void);
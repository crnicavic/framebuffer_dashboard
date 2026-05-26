#include "framebuffer.h"
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    struct font font;
    font.glyph_w = 12;
    font.glyph_h = 16;
    font.data = console_font_12x16;
    struct framebuffer fb = framebuffer_init(320, 480, "/dev/fb1", &font);
    framebuffer_clear(&fb, PINK);
    framebuffer_render_char(&fb, 'a', 100, 100, WHITE);
    char str[] = "kurac";
    framebuffer_render_string(&fb, str, 200, 200, BLACK);

    framebuffer_draw(&fb);
    return 0;
}

#include "framebuffer.h"
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    struct framebuffer fb = init_framebuffer(320, 480);
    struct font font;
    font.glyph_w = 12;
    font.glyph_h = 16;
    font.data = console_font_12x16;
    clear_fb(&fb, PINK);
    render_char(&fb, 'a', 100, 100, &font);
    char str[] = "kurac";
    render_string(&fb, str, 200, 200, &font);

    FILE *fb_file = fopen("/dev/fb1", "wb");
    fwrite(fb.pixels, sizeof(short), fb.w * fb.h, fb_file);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "framebuffer.h"

#define FB_FILE_DEFAULT_PATH "/dev/fb1"

struct font default_font = {
    .glyph_w = 12,
    .glyph_h = 16,
    .data = console_font_12x16
};

struct framebuffer framebuffer_init(int w, int h, char* path, struct font* font)
{
    struct framebuffer fb;
    fb.w = w;
    fb.h = h;
    if (path == NULL) {
        fb.file = fopen(FB_FILE_DEFAULT_PATH, "wb");
    } else {
        fb.file = fopen(path, "wb");
    }
    if (fb.file == NULL) {
        printf("Opening framebuffer %s failed!\n", path);
    }

    if (font == NULL) {
        fb.font = &default_font;
    } else {
        fb.font = font;
    }
    fb.pixels = malloc(sizeof(short) * fb.w * fb.h);
    if (fb.pixels == NULL) {
        printf("framebuffer allocation failed!");
        exit(1);
    }
    return fb;
}

void framebuffer_draw(struct framebuffer *fb) {
    // movo to start of file
    fseek(fb->file, 0, SEEK_SET);
    fwrite(fb->pixels, sizeof(short), fb->w * fb->h, fb->file);
    fflush(fb->file);
}

int framebuffer_index(struct framebuffer *fb, int x, int y)
{
    int index = y * fb->w + x;
    if (index < 0 || index >= (fb->w * fb->h)) {
        printf("WARNING: index %d out of range %d\n", index, (fb->w * fb->h));
    }
    // check if index is out of bounds
    index = index < 0 ? 0 : index >= (fb->w * fb->h) ? (fb->w * fb->h - 1) : index;
    return index;
}

void framebuffer_set_pixel(struct framebuffer *fb, int x, int y, short color)
{
    int index = framebuffer_index(fb, x, y);
    fb->pixels[index] = color;
}

static int max(int a, int b)
{
    return a > b ? a : b;
}

static int min(int a, int b)
{
    return a < b ? a : b;
}

void framebuffer_render_rect(struct framebuffer *fb, struct rect *rect, short color)
{
    // only render the part of the rect that will be visible
    // by checking how much of the rect is within the framebuffer
    int start_x = max(0, rect->x);
    int start_y = max(0, rect->y);
    int end_x = min(rect->x + rect->w, fb->w);
    int end_y = min(rect->y + rect->h, fb->h);
    // if the entire rect is still outside, return
    if (start_x >= end_x || start_y >= end_y) {
        printf("rect out of fb\n");
        return;
    }

    for (int y = start_y; y < end_y; y++) {
        for (int x = start_x; x < end_x; x++) {
            framebuffer_set_pixel(fb, x, y, color);
        }
    }
}

void framebuffer_clear(struct framebuffer *fb, short color)
{
    for (int i = 0; i < (fb->h * fb->w); i++) {
        fb->pixels[i] = color;
    }
}

char* get_glyph(struct font* font, char c)
{
    // glyph size is calculated by rounding up the bit width to the number of bytes
    // and multiplying by the height - looking at the font.h will make it obvious
    int glyph_size = (font->glyph_w / 8 + (font->glyph_w % 8 != 0)) * font->glyph_h;
    int glyph_index = glyph_size * (int) c;
    return &font->data[glyph_index];
}

void framebuffer_render_char(struct framebuffer *fb, char c, int x0, int y0, short color)
{
    // the shape of a glyph is just 0 and 1, when 1 set the pixel,
    // if the value is 0, do not set the pixel, very simple
    char *glyph = get_glyph(fb->font, c);
    for (int y = y0; y < (y0 + fb->font->glyph_h); y++) {
        int glyph_row = *glyph << 4 | (*(glyph+1) >> 4);
        glyph +=2;
        int i = 0;
        for (int x = (x0 + fb->font->glyph_w -1); x >= x0; x--) {
            int bit = (glyph_row >> i++) & 1;
            if (bit) {
                framebuffer_set_pixel(fb, x, y, color);
            }
        }
    }
}

void framebuffer_render_string(struct framebuffer *fb, char *str, int x0, int y0, short color)
{
    int x = x0;
    int y = y0;
    while(*str) {
        framebuffer_render_char(fb, *str, x, y, color);
        x += fb->font->glyph_w;
        str++;
    }
}

void framebuffer_destroy(struct framebuffer *fb)
{
    fclose(fb->file);
    free(fb->pixels);
}

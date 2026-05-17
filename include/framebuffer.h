#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "font12x16.h"
#include <stdlib.h>
#include <stdio.h>

#define WHITE 0b1111111111111111
#define BLACK 0b0000000000000000
#define RED   0b1111100000000000
#define GREEN 0b0000011111100000
#define BLUE  0b0000000000011111
#define CYAN  0b0000011111111001
#define PINK  0b1111001001111100

struct font {
    int glyph_w;
    int glyph_h;
    char *data;
};

struct framebuffer {
    short *pixels;
    struct font* font;
    FILE *file;
    int w;
    int h;
};

struct rect {
    int x;
    int y;
    int w;
    int h;
};

struct framebuffer framebuffer_init(int w, int h, char* path, struct font* font);
void framebuffer_draw(struct framebuffer *fb);
int framebuffer_index(struct framebuffer *fb, int x, int y);
void framebuffer_set_pixel(struct framebuffer *fb, int x, int y, short color);
void render_rect(struct framebuffer *fb, struct rect *rect, short color);
void framebuffer_clear(struct framebuffer *fb, short color);
char* get_glyph(struct font* font, char c);
void render_char(struct framebuffer *fb, char c, int x0, int y0, short color);
void render_string(struct framebuffer *fb, char *str, int x0, int y0, short color);
void framebuffer_destroy(struct framebuffer *fb);
#endif

#include <stdio.h>
#include <stdlib.h>
#include <tslib.h>
#include "font12x16.h"

#define WHITE 0b1111111111111111
#define BLACK 0b0000000000000000
#define RED   0b1111100000000000
#define GREEN 0b0000011111100000
#define BLUE  0b0000000000011111
#define CYAN  0b1111101110111100
#define PINK  0b1111001001111100

struct framebuffer {
	short *pixels;
	int w;
	int h;
};

struct font {
	int glyph_w;
	int glyph_h;
	char *data;
};

struct framebuffer init_framebuffer(int w, int h)
{
	struct framebuffer fb;
	fb.w = w;
	fb.h = h;
	fb.pixels = malloc(sizeof(short) * fb.w * fb.h);
	return fb;
}

int framebuffer_index(struct framebuffer *fb, int x, int y)
{
	int index = y * fb->w + x;
	return index;
}

void framebuffer_set_pixel(struct framebuffer *fb, int x, int y, short color)
{
	int index = framebuffer_index(fb, x, y);
	fb->pixels[index] = color;
}

void clear_fb(struct framebuffer *fb, short color)
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

void render_char(struct framebuffer *fb, char c, int x0, int y0, struct font *font)
{

	char *glyph = get_glyph(font, c);
	short color = WHITE;
	for (int y = y0; y < (y0 + font->glyph_h); y++) {
		int glyph_row = *glyph << 4 | (*(glyph+1) >> 4);
		glyph +=2;
		int i = 0;
		for (int x = (x0 + font->glyph_w -1); x >= x0; x--) {
			int bit = (glyph_row >> i++) & 1;
			if (bit) {
				framebuffer_set_pixel(fb, x, y, color);
			}
		}
	}
}

void render_string(struct framebuffer *fb, char *str, int x0, int y0, struct font *font)
{
	int x = x0;
	int y = y0;
	while(*str) {
		render_char(fb, *str, x, y, font);
		x += font->glyph_w;
		str++;
	}
}

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

	ts_read();

	FILE *fb_file = fopen("/dev/fb1", "wb");
	fwrite(fb.pixels, sizeof(short), fb.w * fb.h, fb_file);
	return 0;
}

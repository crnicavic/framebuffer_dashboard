#include "dashboard.h"
#include <string.h>
#include <tslib.h>

struct dashboard init_dashboard(struct screen_params *p)
{
    struct framebuffer fb = framebuffer_init(p->pixels_w, p->pixels_h, p->framebuffer_path, NULL);
    struct dashboard dash = {
    .screen_params = p,
    .ts = ts_setup(p->touchscreen_path, 1),
    .fb = &fb,
    .buttons = (struct button **) malloc(sizeof(struct button*) * DASHBOARD_MAX_BUTTONS),
    .button_count = 0
    };
    if (dash.fb == NULL || dash.ts == NULL || dash.buttons == NULL) {
        printf("dashboard creation failed!\n");
        exit(1);
    }
    return dash;
}

void dashboard_add_button(struct dashboard *dash,
                          struct button *button)
{
    if (dash->button_count >= DASHBOARD_MAX_BUTTONS) {
        return;
    }
    dash->buttons[dash->button_count] = button;
    dash->button_count++;
}

void update_dashboard_state(struct dashboard *dash) {
    struct ts_sample samp;
    framebuffer_clear(dash->fb, BLACK);
    int ret = ts_read(dash->ts, &samp, 1);
    if (ret > 0) {
        struct rect touch_area = convert_touch_to_pixel(&samp, dash->screen_params);
        for (int b = 0; b < dash->button_count; b++) {
            struct button *button = dash->buttons[b];
            update_button_state(button, dash, &touch_area);
        }
    }
    framebuffer_draw(dash->fb);
}

static int clip(int x, int lo, int hi)
{
    return x < lo ? lo : x > hi ? hi : x;
}

struct rect convert_touch_to_pixel(struct ts_sample *samp,
                                   struct screen_params *p)
{
    struct rect touch_rect;
    // clip both inputs because the display is crap
    samp->x = clip(samp->x, p->touch_xmin, p->touch_xmax);
    samp->y = clip(samp->y, p->touch_ymin, p->touch_ymax);
    touch_rect.x = p->pixels_w * (samp->x - p->touch_xmin) / (p->touch_xmax - p->touch_xmin);
    touch_rect.x = p->touch_invert_x ? -touch_rect.x : touch_rect.x;
    touch_rect.x += (p->touch_invert_x ? p->pixels_w : 0) - p->touch_w / 2;

    touch_rect.y = p->pixels_h * (samp->y - p->touch_ymin) / (p->touch_ymax - p->touch_ymin);
    touch_rect.y = p->touch_invert_y ? -touch_rect.y : touch_rect.y;
    touch_rect.y += (p->touch_invert_y ? p->pixels_h : 0) - p->touch_h / 2;

    touch_rect.w = p->touch_w;
    touch_rect.h = p->touch_h;
    return touch_rect;
}

void render_button(struct framebuffer *fb,
                   struct button *button)
{
    short color = button->state == PRESSED ? button->press_color : button->color;
    render_rect(fb, &button->rect, color);
    int len = strlen(button->text);
    int x = button->rect.x + button->rect.x / 2 - len * fb->font->glyph_w / 2;
    int y = button->rect.y + button->rect.h / 2 - fb->font->glyph_h / 2;
    render_string(fb, button->text, x, y, button->text_color);
}

int rect_collision(struct rect *a, struct rect *b)
{
    return (a->x + a->w >= b->x &&
            a->x <= b->x + b->w &&
            a->y + a->h >= b->y &&
            a->y <= b->y + b->h);
}

void update_button_state(struct button *button,
                         struct dashboard *dash,
                         struct rect *touch_rect)
{
    button->state = rect_collision(touch_rect, &button->rect);
    render_button(dash->fb, button);
}

void destroy_dashboard(struct dashboard *dash)
{
    destroy_framebuffer(dash->fb);
    ts_close(dash->ts);
    free(dash->buttons);
}

#include "dashboard.h"
#include <string.h>
#include <tslib.h>

struct dashboard init_dashboard(struct screen_params *p)
{
    struct dashboard dash = {
    .screen_params = p,
    .ts = ts_setup(p->touchscreen_path, 1),
    .fb = framebuffer_init(p->pixels_w, p->pixels_h, p->framebuffer_path, NULL),
    .element_count = 0
    };
    if (dash.ts == NULL) {
        printf("dashboard touchscreen init failed!\n");
        exit(1);
    }
    return dash;
}

void dashboard_add_element(struct dashboard *dash,
                           struct dashboard_element *element)
{
    if (dash->element_count >= DASHBOARD_MAX_ELEMENTS) {
        return;
    }
    dash->elements[dash->element_count] = element;
    dash->element_count++;
}

void dashboard_touch_update(struct dashboard *dash) {
    struct ts_sample samp;
    int ret = ts_read(dash->ts, &samp, 1);
    if (ret > 0) {
        struct rect touch_area = convert_touch_to_pixel(&samp, dash->screen_params);
        for (int e = 0; e < dash->element_count; e++) {
            struct dashboard_element *element = dash->elements[e];
            int prev_state = element->touched;
            if (element->touchable) {
                element->touch(element, &touch_area);
            }
            if (element->touch_callback &&
                element->touched &&
                prev_state != element->touched) {
                // state changed!
                element->dirty = 1;
                element->touch_callback(element);
            }
        }
    } else {
        for (int e = 0; e < dash->element_count; e++) {
            struct dashboard_element *element = dash->elements[e];
            int prev_state = element->touched;
            element->touched = 0;
            if (element->touched != prev_state) {
                element->dirty = 1;
            }
        }
    }
}

void dashboard_render_elements(struct dashboard *dash)
{
    int any_dirty = 0;
    for (int e = 0; e < dash->element_count; e++) {
        struct dashboard_element *element = dash->elements[e];
        if (element->dirty) {
            element->framebuffer_render(&dash->fb, element);
            element->dirty = 0;
            any_dirty = 1;
        }
    }
    if (any_dirty) {
        framebuffer_draw(&dash->fb);
    }
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

int rect_collision(struct rect *a, struct rect *b)
{
    return (a->x + a->w >= b->x &&
            a->x <= b->x + b->w &&
            a->y + a->h >= b->y &&
            a->y <= b->y + b->h);
}

void dashboard_is_element_touched(struct dashboard_element *element,
                                  struct rect *touch_area)
{
    if (!element->touchable) return;
    element->touched = rect_collision(&element->box, touch_area);
}

void framebuffer_render_button(struct framebuffer *fb,
                               struct dashboard_element *element)
{
    struct button *button = (struct button *) element->element_data;
    short color = element->touched ? button->press_color : button->color;
    framebuffer_render_rect(fb, &element->box, color);
    int len = strlen(button->text);
    int x = element->box.x + element->box.x / 2 - len * fb->font->glyph_w / 2;
    int y = element->box.y + element->box.h / 2 - fb->font->glyph_h / 2;
    framebuffer_render_string(fb, button->text, x, y, button->text_color);
}

void framebuffer_render_label(struct framebuffer *fb,
                              struct dashboard_element *element)
{
    struct label *label = (struct label *) element->element_data;
    framebuffer_render_string(fb,
                              label->text,
                              element->box.x,
                              element->box.y,
                              label->color);
}

void framebuffer_render_indicator(struct framebuffer *fb,
                                  struct dashboard_element *element)
{
    struct indicator *indicator = element->element_data;
    short color = indicator->state == INDICATOR_ON ?
        indicator->on_color : indicator->off_color;
    framebuffer_render_rect(fb, &element->box, color);
}

void dashboard_destroy(struct dashboard *dash)
{
    framebuffer_destroy(&dash->fb);
    ts_close(dash->ts);
}

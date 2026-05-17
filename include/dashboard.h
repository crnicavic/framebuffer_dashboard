#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "framebuffer.h"
#include <tslib.h>
#define DASHBOARD_MAX_BUTTONS 16

enum button_state {
NOT_PRESSED = 0,
PRESSED = 1
};

struct button {
    struct rect rect;
    short color;
    short press_color;
    short text_color;
    char *text;
    int state;
    void (*callback)(void *);
    void* arg;   // currently support only one argument
};

struct screen_params {
    int touch_xmin;          // lowest x axis touchscreen value
    int touch_xmax;          // highest x axis touchscreen value
    int touch_ymin;          // lowest y axis touchscreen value
    int touch_ymax;          // highest y axis touchscreen value
    int touch_invert_x;      // should the x axis be inverted when converting to pixels
    int touch_invert_y;      // should the y axis be inverted when converting to pixels
    int touch_w;             // width of touch area
    int touch_h;             // height of touch area
    int pixels_w;            // width of the screen in pixels
    int pixels_h;            // height of the framebuffer/screen
    char *framebuffer_path;
    char *touchscreen_path;
};

struct dashboard {
    struct screen_params *screen_params;
    struct framebuffer *fb;
    struct tsdev *ts;
    struct button **buttons; //array of button pointers
    int button_count;
};

struct dashboard init_dashboard(struct screen_params *p);
void dashboard_add_button(struct dashboard *dash,
                          struct button *button);
void update_dashboard_state(struct dashboard *dash);
struct rect convert_touch_to_pixel(struct ts_sample *samp,
                                   struct screen_params *p);
void render_button(struct framebuffer *fb,
                   struct button *button);
int rect_collision(struct rect *a, struct rect *b);
void update_button_state(struct button *button,
                         struct dashboard *dash,
                         struct rect *touch_rect);
void destroy_dashboard(struct dashboard *dash);
#endif

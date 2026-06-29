#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "framebuffer.h"
#include <tslib.h>
#define DASHBOARD_MAX_ELEMENTS 16

struct button {
    short color;
    short press_color;
    short text_color;
    char *text;
};

struct label{
    char *text;
    short color;
};

enum indicator_state {
    INDICATOR_OFF = 0,
    INDICATOR_ON
};

struct indicator {
    short on_color;
    short off_color;
    enum indicator_state state;
};

struct touch_event {
    struct rect touch_area;
    int pressure;
};

struct dashboard_element {
    int visible;
    int touchable;
    struct rect box;
    void (*framebuffer_render)(struct framebuffer *, struct dashboard_element *);
    void (*touch)(struct dashboard_element *, struct touch_event *);
    void (*touch_callback)(struct dashboard_element *);
    void (*update_state)(struct dashboard_element *); //  used for buttons
    void *element_data; // the pointer to the element itself
    void *user_data;
    int dirty;
    int touched; // has the element been touched in the current poll (user doesnt need to set this)
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
    struct framebuffer fb;
    struct tsdev *ts;
    struct dashboard_element elements[DASHBOARD_MAX_ELEMENTS];
    int element_count;
};

struct dashboard init_dashboard(struct screen_params *p);
void dashboard_add_element(struct dashboard *dash,
                           struct dashboard_element *element);
void dashboard_touch_update(struct dashboard *dash);
void dashboard_render_elements(struct dashboard *dash);
struct rect convert_touch_to_pixel(struct ts_sample *samp,
                                   struct screen_params *p);
int rect_collision(struct rect *a, struct rect *b);
void dashboard_is_element_touched(struct dashboard_element *element,
                                  struct touch_event *t);
void framebuffer_render_button(struct framebuffer *fb,
                               struct dashboard_element *element);
void framebuffer_render_label(struct framebuffer *fb,
                              struct dashboard_element *element);
void framebuffer_render_indicator(struct framebuffer *fb,
                                  struct dashboard_element *element);
void dashboard_update_state(struct dashboard *dash);
void dashboard_destroy(struct dashboard *dash);
#endif

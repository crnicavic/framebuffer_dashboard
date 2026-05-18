#include "dashboard.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

void button_callback(struct dashboard_element *e)
{
    static int count = 0;
    struct button *b = (struct button *) e->element_data;
    //printf("(%d) I'm %s\n", count, b->text);
    count++;
}

int main() {
    struct screen_params params = {
        .touch_xmin = 200,
        .touch_xmax = 4000,
        .touch_ymin = 200,
        .touch_ymax = 4000,
        .touch_invert_x = 1,
        .touch_invert_y = 1,
        .touch_w = 10,
        .touch_h = 10,
        .pixels_w = 320,
        .pixels_h = 480,
        .framebuffer_path = "/dev/fb1",
        .touchscreen_path = "/dev/input/event0"
    };

    struct dashboard dash = init_dashboard(&params);

    struct button button = {
        .color = RED,
        .press_color = GREEN,
        .text_color = WHITE,
        .text = (char*) "dugme"
    };
    struct dashboard_element button_elem = {
        .visible = 1,
        .touchable = 1,
        .touched = 0,
        .dirty = 1,
        .box = {
            .x = 100,
            .y = 100,
            .w = 100,
            .h = 100
        },
        .framebuffer_render = &framebuffer_render_button,
        .touch = &dashboard_is_element_touched,
        .touch_callback = &button_callback,
        .update_state = NULL,
        .element_data = &button,
        .user_data = NULL
    };
    dashboard_add_element(&dash, &button_elem);

    struct label label = {
        .text = (char *) "cigla",
        .color = WHITE
    };
    struct dashboard_element label_elem = {
        .visible = 1,
        .touchable = 0,
        .box = {
            .x = 50,
            .y = 50
        },
        .framebuffer_render = &framebuffer_render_label,
        .touch = NULL,
        .touch_callback = NULL,
        .update_state = NULL,
        .element_data = &label,
        .user_data = NULL,
        .dirty = 1
    };
    dashboard_add_element(&dash, &label_elem);

    struct indicator indicator = {
        .on_color = GREEN,
        .off_color = RED,
        .state = INDICATOR_OFF
    };
    struct dashboard_element indicator_elem = {
        .visible = 1,
        .touchable = 0,
        .box = {
            .x = 270,
            .y = 20,
            .w = 30,
            .h = 30
        },
        .framebuffer_render = &framebuffer_render_indicator,
        .touch = NULL,
        .touch_callback = NULL,
        .update_state = NULL,
        .element_data = &indicator,
        .user_data = NULL,
        .dirty = 1
    };
    dashboard_add_element(&dash, &indicator_elem);

    while (1) {
        dashboard_touch_update(&dash);
        dashboard_render_elements(&dash);
        //printf("%lld\n", time_spent);
        usleep(10000);
    }

    dashboard_destroy(&dash);
    return 0;
}

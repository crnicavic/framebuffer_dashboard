#include "dashboard.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <tslib.h>

void update_indicator(struct dashboard_element *e) {
    struct indicator *indicator = (struct indicator *) e->element_data;
    enum indicator_state prev_state = indicator->state;
    int r = rand() % 10;
    indicator->state = (r >= 5);
    e->dirty = prev_state != indicator->state;
}

void touch_button(struct dashboard_element *e) {
    printf("BUTTON!\n");
}

int main() {
    srand(time(NULL));
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

    struct label label = {
    .text = (char *) "LOREM IPSUM",
    .color = PINK
    };

    struct dashboard_element label_elem = {
        .visible = 1,
        .touchable = 0,
        .box = {
            .x = 100,
            .y = 100
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
            .x = 10,
            .y = 10,
            .w = 30,
            .h = 30
        },
        .framebuffer_render = &framebuffer_render_indicator,
        .touch = NULL,
        .touch_callback = NULL,
        .update_state = &update_indicator,
        .element_data = &indicator,
        .user_data = NULL,
        .dirty = 1
    };
    dashboard_add_element(&dash, &indicator_elem);

    struct button button = {
        .color = GREEN,
        .press_color = DARK_GREEN,
        .text_color = WHITE,
        .text = (char *) "BUTTON"
    };
    struct dashboard_element button_elem = {
        .visible = 1,
        .touchable = 1,
        .box = {
            .x = 200,
            .y = 200,
            .w = 100,
            .h = 100
        },
        .framebuffer_render = &framebuffer_render_button,
        .touch = &dashboard_is_element_touched,
        .touch_callback = &touch_button,
        .update_state = NULL,
        .element_data = &button,
        .user_data = NULL,
        .dirty = 1
    };

    dashboard_add_element(&dash, &button_elem);

    int update_period = 100;
    int periods = 0;
    while (1) {
        dashboard_touch_update(&dash);
        if (periods == 0) {
            dashboard_update_state(&dash);
        }
        dashboard_render_elements(&dash);
        periods = (periods != update_period) * (periods+1);
        usleep(10000);
    }
    dashboard_destroy(&dash);
    return 0;
}

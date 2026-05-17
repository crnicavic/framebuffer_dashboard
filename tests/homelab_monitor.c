#include "dashboard.h"
#include <stdlib.h>
#include <stdio.h>

int main() {
    struct button button = {
        .rect = {
            .x = 100,
            .y = 100,
            .w = 100,
            .h = 100
        },
        .color = PINK,
        .press_color = GREEN,
        .text_color = WHITE,
        .text = (char*) "dugme"
    };

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
    };

    struct dashboard dash = init_dashboard(*params);
    while (1) {
        update_dashboard_state(dash);
        usleep(5000);
    }

    destroy_dashbooard(&dash);
    return 0;
}

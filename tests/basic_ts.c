#include <unistd.h>
#include <tslib.h>
#include <stdio.h>

int main() {
    struct tsdev *ts = ts_setup("/dev/input/event0", 0);
    if (!ts) return -1;

    struct ts_sample samp;

    while (1) {
        int ret = ts_read(ts, &samp, 1);
        if (ret > 0) {
            printf("Touch at %d,%d\n", samp.x, samp.y);
        }

        usleep(16000); // ~60 FPS
    }

    ts_close(ts);
    return 0;
}

#include <fcntl.h>
#include <unistd.h>
#include <tslib.h>
#include <stdio.h>

int main() {
    struct tsdev *ts = ts_setup(NULL, 0);
    if (!ts) return -1;

    // Make the file descriptor non-blocking
    int fd = ts_fd(ts);
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    struct ts_sample samp;

    while (1) {
        // --- Rendering code ---
        // e.g., redraw framebuffer here
        printf("Rendering frame...\n");

        // --- Touch handling ---
        int ret = ts_read(ts, &samp, 1);
        if (ret > 0) {
            printf("Touch at %d,%d\n", samp.x, samp.y);
        }

        usleep(16000); // ~60 FPS
    }

    ts_close(ts);
    return 0;
}

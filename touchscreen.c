#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

#include <tslib.h>

#define SLOTS 5
#define SAMPLES 1

int main(int argc, char **argv)
{
    struct tsdev *ts;
    char *tsdevice = NULL;
    struct ts_sample samp;
    int ret, i, j;

    ts = ts_setup(tsdevice, 0);
    if (!ts) {
            perror("ts_setup");
            return -1;
    }

    ts_read(ts, &samp, 1);
    printf("%d, %d", samp.x, samp.y);
    ts_close(ts);
}

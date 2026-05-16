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
    struct ts_sample_mt **samp_mt = NULL;
    int ret, i, j;

    ts = ts_setup(tsdevice, 0);
    if (!ts) {
            perror("ts_setup");
            return -1;
    }

    samp_mt = malloc(SAMPLES * sizeof(struct ts_sample_mt *));
    if (!samp_mt) {
            ts_close(ts);
            return -ENOMEM;
    }
    for (i = 0; i < SAMPLES; i++) {
            samp_mt[i] = calloc(SLOTS, sizeof(struct ts_sample_mt));
            if (!samp_mt[i]) {
                    for (i--; i >= 0; i--)
                            free(samp_mt[i]);
                    free(samp_mt);
                    ts_close(ts);
                    return -ENOMEM;
            }
    }

    while (1) {
            ret = ts_read_mt(ts, samp_mt, SLOTS, SAMPLES);
            if (ret < 0) {
                    perror("ts_read_mt");
                    ts_close(ts);
                    exit(1);
            }

            for (j = 0; j < ret; j++) {
            	for (i = 0; i < SLOTS; i++) {
		#ifdef TSLIB_MT_VALID
			if (!(samp_mt[j][i].valid & TSLIB_MT_VALID))
				continue;
		#else
			if (samp_mt[j][i].valid < 1)
				continue;
		#endif

			printf("%lld.%06lld: (slot %d) %6d %6d %6d\n",
			       (long long)samp_mt[j][i].tv.tv_sec,
			       (long long)samp_mt[j][i].tv.tv_usec,
			       samp_mt[j][i].slot,
			       samp_mt[j][i].x,
			       samp_mt[j][i].y,
			       samp_mt[j][i].pressure);
                    }
            }
    }

    ts_close(ts);
}

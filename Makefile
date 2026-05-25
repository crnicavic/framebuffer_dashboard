CC=gcc
AR=ar
ODIR=./obj
SDIR=./src
IDIR=./include

SRC=$(wildcard $(SDIR)/*.c)
OBJ=$(patsubst $(SDIR)/%.c,$(ODIR)/%.o,$(SRC))

FB_DEPS=$(filter-out $(ODIR)/dashboard.o, $(OBJ))

LIBS=-lts
INCLUDE=-I$(IDIR)

all: $(OBJ)

$(ODIR)/%.o: $(SDIR)/%.c $(IDIR)/%.h
	$(CC) -c -fPIC $< -o $@ $(INCLUDE)

libframebuffer.so: $(FB_DEPS)
	$(CC) -o $@ -shared $^

libdashboard.so: $(OBJ)
	$(CC) -o $@ -shared $^ $(LIBS)

libdashboard.a: $(OBJ)
	$(AR) rcs $@ $^

libframebuffer.a: $(FB_DEPS)
	$(AR) rcs $@ $^

basic_fb: libframebuffer.a tests/basic_fb.c
	$(CC) tests/basic_fb.c -o $@ libframebuffer.a $(INCLUDE)

basic_ts: tests/basic_ts.c
	$(CC) tests/basic_ts.c -o $@ $(LIBS)

basic_dash: tests/basic_dash.c libdashboard.a
	$(CC) tests/basic_dash.c -o $@ libdashboard.a $(LIBS) $(INCLUDE)

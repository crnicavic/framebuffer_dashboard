CC=gcc
ODIR=./obj
SDIR=./src
IDIR=./include

SRC=$(wildcard $(SDIR)/*.c)
OBJ=$(patsubst $(SDIR)/%.c,$(ODIR)/%.o,$(SRC))

LIBS=-lts
INCLUDE=-I$(IDIR)
all: $(OBJ)

$(OBJ): $(SRC)
	$(CC) -c $< $(LIBS) -o $@ $(INCLUDE)

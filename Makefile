NAME=meCh
CC=gcc
CFLAGS=-O3 -Wall -Werror -pedantic -g
LDFLAGS=
LIBS=

MODDIR = mod
_MODOBJ = test.o utopia.o tell.o
MODOBJ = $(patsubst %,$(MODDIR)/%,$(_MODOBJ))

DEPS = irc.h config.h module.h utils.h
OBJ = irc.o config.o module.o utils.o
MAIN = main.o


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $(CFLAGS) $<

$(MODDIR)/%.o: $(MODDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $(CFLAGS) $<

all: $(OBJ) $(MODOBJ) $(MAIN)
	$(CC) -o $(NAME) $(LDFLAGS) $(LIBS) $(OBJ) $(MODOBJ) $(MAIN)

clean:
	rm *.o
	rm $(MODDIR)/*.o
	rm $(NAME)
.PHONY: clean
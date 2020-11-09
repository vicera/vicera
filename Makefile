#
# VICERA by h34ting4ppliance
#
# Makefile
#
FILES = main.o cpu.o gpu.o logging.o spu.o sdl_gpu.o config.o debug.o
CFLAGS = -c
LFLAGS = -lSDL2 -lpthread

EXECNAME = vicera
CC = gcc
LD = gcc

.PHONY: clean

# The VICERA console itself
$(EXECNAME): $(FILES)
		$(LD) $(LFLAGS) $(FILES) -p -o $(EXECNAME)

%.o: %.c
	$(CC) $< $(CFLAGS) -o $@

clean:
	rm -f $(FILES) $(EXECNAME)

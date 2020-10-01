FILES = main.c cpu.c gpu.c logging.c spu.c sdl_gpu.c config.c
FIFO_EXT = fifo.c
ARGS = -Wall -Wextra -pedantic
LIBS = -lSDL2 -lpthread
EXECNAME = vicera
CC = gcc

# The VICERA console itself
vicera: $(FILES)
		$(CC) $(FILES) $(ARGS) $(LIBS) -o $(EXECNAME)

# The VICERA with the FIFO extension
fifo: $(FILES) $(FIFO_EXT)
		$(CC) $(FILES) $(FIFO_EXT) $(ARGS) $(LIBS) -DFIFOEXT -o $(EXECNAME)

# CPU Tests (outdated)
test: cpu.c logging.c tests.c
		$(CC) cpu.c logging.c tests.c -lcmocka -o vicera-testing

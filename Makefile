FILES = main.c cpu.c gpu.c logging.c spu.c sdl_gpu.c config.c
ARGS = -Wall -Wextra -pedantic
LIBS = -lSDL2 -lpthread
EXECNAME = vicera

vicera: $(FILES)
		$(CC) $(FILES) $(ARGS) $(LIBS) -o $(EXECNAME)


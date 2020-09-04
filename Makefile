tests: main.c cpu.c gpu.c logging.c
		$(CC) main.c cpu.c gpu.c logging.c -o vicera -Wall -Wextra -pedantic -lSDL2 -lpthread


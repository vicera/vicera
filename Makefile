tests: main.c cpu.c logging.c
		$(CC) main.c cpu.c logging.c -o vicera -Wall -Wextra -pedantic


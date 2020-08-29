tests: tests.c cpu.c logging.c
		$(CC) tests.c cpu.c logging.c -o vicera-testing -Wall -Wextra -pedantic -std=c99 -lcmocka


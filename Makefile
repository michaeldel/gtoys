CC = gcc
CFLAGS = -Wall -Wextra -pedantic -lX11

color: color.c
	$(CC) -o $@ $(CFLAGS) $^

clean:
	rm color

.PHONY: clean

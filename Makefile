CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic $(shell pkg-config --cflags cairo-xlib pangocairo)
LDFLAGS = $(shell pkg-config --libs cairo-xlib pangocairo)

color: color.c
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS) 

clean:
	rm color

.PHONY: clean

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic $(shell pkg-config --cflags cairo-xlib pangocairo)
LDFLAGS = -lm $(shell pkg-config --libs cairo-xlib pangocairo)

color: color.c
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS) 

test/test_util_color: test/test_util_color.c util/color.c
	$(CC) -o $@ $(CFLAGS) -iquote . $^ $(LDFLAGS)

test: test/test_util_color
	./test/test_util_color

clean:
	rm color test/test_util_color

.PHONY: clean test

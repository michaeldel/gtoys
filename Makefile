CC = gcc

color: color.c
	$(CC) -o $@ $^

clean:
	rm color

.PHONY: clean

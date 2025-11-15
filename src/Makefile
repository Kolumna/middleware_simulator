CC = gcc
CFLAGS = -Wall -g
TARGETS = request middleware

all: $(TARGETS)

request: request.c
	$(CC) $(CFLAGS) -o request request.c

middleware: middleware.c
	$(CC) $(CFLAGS) -o middleware middleware.c

clean:
	rm -f $(TARGETS)
	
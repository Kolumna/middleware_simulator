CC = gcc
CFLAGS = -Wall -g
TARGETS = request middleware

all: $(TARGETS)

request: src/request.c
	$(CC) $(CFLAGS) -o src/request src/request.c

middleware: src/middleware.c
	$(CC) $(CFLAGS) -o src/middleware src/middleware.c

clean:
	rm -f $(TARGETS)
	
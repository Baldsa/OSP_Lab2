CC = gcc
CFLAGS = -Wall -Wextra -Werror -O3

all: server client

server: lab2knmN32511_server.c
	$(CC) $(CFLAGS) -o lab2knmN32511_server lab2knmN32511_server.c

client: lab2knmN32511_client.c
	$(CC) $(CFLAGS) -o lab2knmN32511_client lab2knmN32511_client.c

clean:
	rm -f lab2knmN32511_client lab2knmN32511_server

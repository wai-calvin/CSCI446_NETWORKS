EXE = file_client file_server
CFLAGS = -Wall

all: $(EXE)

file_client: stream-talk-client.c
		gcc $(CFLAGS) stream-talk-client.c -o file_client

file_server: stream-talk-server.c
	gcc $(CFLAGS) stream-talk-server.c -o file_server

clean:
		rm -f $(EXE)

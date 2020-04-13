EXE = file_client file_server
CFLAGS = -Wall

all: $(EXE)

file_client: file_client.c
		gcc $(CFLAGS) file_client.c -o file_client

file_server: file_server.c
	gcc $(CFLAGS) file_server.c -o file_server

clean:
		rm -f $(EXE)

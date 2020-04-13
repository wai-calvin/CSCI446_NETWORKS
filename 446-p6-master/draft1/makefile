
all: snw_server snw_client

snw_server: snw_server.c libprogram3Functions.a program3Functions.h
	gcc -Wall snw_server.c -o snw_server -L . -lprogram3Functions

snw_client: snw_client.c libprogram3Functions.a program3Functions.h
	gcc -Wall snw_client.c -o snw_client -L . -lprogram3Functions

clean:
	rm -f snw_client
	rm -f snw_server

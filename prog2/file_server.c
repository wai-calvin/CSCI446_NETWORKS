/*	@author 		Jonathan Cheon & Calvin Wai
**	@file				file_server.c
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h> //for stat()
#include <fcntl.h>

//#define SERVER_PORT "5432"
#define MAX_LINE 256
#define MAX_PENDING 5

const char* SERVER_PORT;
/*
 * Create, bind and passive open a socket on a local interface for the provided service.
 * Argument matches the second argument to getaddrinfo(3).
 *
 * Returns a passively opened socket or -1 on error. Caller is responsible for calling
 * accept and closing the socket.
 */
int bind_and_listen( const char *service );

int main( int argc, char *argv[] ) {
	char buf[MAX_LINE];
	int s, new_s;
	int len;

	if ( argc == 2 ) {
		SERVER_PORT = argv[1];
	}
	/* Bind socket to local interface and passive open */
	if ( ( s = bind_and_listen( SERVER_PORT ) ) < 0 ) {
		exit( 1 );
	}

	/* Wait for connection, then receive and print text */
	while ( 1 ) {
		if ( ( new_s = accept( s, NULL, NULL ) ) < 0 ) {
			perror( "stream-talk-server: accept" );
			close( s );
			exit( 1 );
		}
		//receives file name from client
		while ( ( len = recv( new_s, buf, sizeof( buf ), 0 ) ) > 0 ) {
			if ( len < 0 ) {
				perror( "streak-talk-server: recv" );
				close( s );
				exit( 1 );
			}
		    //puts( buf ); //prints out fileName from client

        const char* fileName = buf;
        struct stat buffer;
        int fileExist = stat(fileName, &buffer);
				int fd;
				int rdonly;

        if(fileExist == -1)
        {
					perror( "stream-talk-server: stat");
          close (new_s);
          close(s);
        }
        if( (fd = open(fileName, O_RDONLY)) == -1)
        {
          perror( "stream-talk-server: open");

					const char* error = "error";
					strcpy(buf, error);

					send(new_s, buf, MAX_LINE, 0);
          close(new_s);
          close(s);
        }
				//read file then send to client.
        memset(buf, '\0', MAX_LINE);
				while ( (rdonly = read(fd, buf, MAX_LINE)) != 0 )
        {
					send(new_s, buf, rdonly, 0);
          memset(buf, '\0', MAX_LINE);
				}
        break;
		}
		close( new_s );
	}

	close( s );
	return 0;
}

int bind_and_listen( const char *service ) {
	struct addrinfo hints;
	struct addrinfo *rp, *result;
	int s;

	/* Build address data structure */
	memset( &hints, 0, sizeof( struct addrinfo ) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;

	/* Get local address info */
	if ( ( s = getaddrinfo( NULL, service, &hints, &result ) ) != 0 ) {
		fprintf( stderr, "stream-talk-server: getaddrinfo: %s\n", gai_strerror( s ) );
		return -1;
	}

	/* Iterate through the address list and try to perform passive open */
	for ( rp = result; rp != NULL; rp = rp->ai_next ) {
		if ( ( s = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol ) ) == -1 ) {
			continue;
		}

		if ( !bind( s, rp->ai_addr, rp->ai_addrlen ) ) {
			break;
		}

		close( s );
	}
	if ( rp == NULL ) {
		perror( "stream-talk-server: bind" );
		return -1;
	}
	if ( listen( s, MAX_PENDING ) == -1 ) {
		perror( "stream-talk-server: listen" );
		close( s );
		return -1;
	}
	freeaddrinfo( result );

	return s;
}

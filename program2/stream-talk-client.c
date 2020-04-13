/* This code is an updated version of the sample code from "Computer Networks: A Systems
 * Approach," 5th Edition by Larry L. Peterson and Bruce S. Davis. Some code comes from
 * man pages, mostly getaddrinfo(3). */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_LINE 256
//should this have 5432 in it?
const char* SERVER_PORT;
/*
 * Lookup a host IP address and connect to it using service. Arguments match the first two
 * arguments to getaddrinfo(3).
 *
 * Returns a connected socket descriptor or -1 on error. Caller is responsible for closing
 * the returned socket.
 */
int lookup_and_connect( const char *host, const char *service );

int main( int argc, char *argv[] ) {
  char *host;
	char buf[MAX_LINE];
	int s;
	int len;

  char* fileName;

	if ( argc == 4 ) {
        host = argv[1];
        SERVER_PORT = argv[2];
        fileName = argv[3];
	}
	else {
		fprintf( stderr, "usage: %s host\n", argv[0] );
		exit( 1 );
	}


	/* Lookup IP and connect to server */
	if ( ( s = lookup_and_connect( host, SERVER_PORT ) ) < 0 ) {
        printf("failed lookup and connect\n");
		exit( 1 );
	}

    printf("client: lookup and connect SUCCESS\n");

  //main
  printf("main\n");
  len = strlen(fileName);

  //filling buf with fileName
  for(int i = 0; i < len; i++)
  {
    buf[i] = fileName[i];
  }

  buf[len] = '\0';
  len += 1;

  printf("client: buf: %s\n", buf);

  if ( send( s, buf, len, 0 ) == -1 ) {
    perror( "stream-talk-client: send\n" );
  	close( s );
	  exit( 1 );
  }

  printf("client: send SUCCESS\n");

  //open received data from server
  int readBytes;
  int writeBytes;

  if( (readBytes = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
  {
    printf("Error opening file...\n");
  }

  printf("client: open SUCCESS\n");

  //ERROR HERE!! FACK
  while( (writeBytes = recv(s, buf, MAX_LINE, 0)) != -1)
  {
    printf("DO IT GET TO HERE THOUGH\n");
    printf( "%s\n", buf );
    write( readBytes, buf, writeBytes );
  }

  printf("client: write SUCCESS\n");
  close( s );
  return 0;
}

int lookup_and_connect( const char *host, const char *service ) {
	struct addrinfo hints;
	struct addrinfo *rp, *result;
	int s;

	/* Translate host name into peer's IP address */
	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
  hints.ai_protocol = 0;

	if ( ( s = getaddrinfo( host, service, &hints, &result ) ) != 0 ) {
		fprintf( stderr, "stream-talk-client: getaddrinfo: %s\n", gai_strerror( s ) );
		return -1;
	}

	/* Iterate through the address list and try to connect */
	for ( rp = result; rp != NULL; rp = rp->ai_next ) {
		if ( ( s = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol ) ) == -1 ) {
                continue;
        }

		if ( connect( s, rp->ai_addr, rp->ai_addrlen ) != -1 ) {
			break;
		}

		close( s );
	}
	if ( rp == NULL ) {
		perror( "stream-talk-client: connect" );
		return -1;
	}
	freeaddrinfo( result );

	return s;
}

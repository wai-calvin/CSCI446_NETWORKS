/*	@author 		Jonathan Cheon & Calvin Wai
**	@file				file_client.c
*/
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

  //grab data from cmdline
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
    perror("stream-talk-client: lookup_and_connect\n");
		exit( 1 );
	}

  len = strlen(fileName);

  //filling buf with fileName
  for(int i = 0; i < len; i++)
  {
    buf[i] = fileName[i];
  }

  //terminating char.
  buf[len] = '\0';
  len += 1;

  if ( send( s, buf, len, 0 ) == -1 ) {
    perror( "stream-talk-client: send\n" );
  	close( s );
    exit( 1 );
  }

  int readBytes;
  int writeBytes;

  memset(buf, '\0', MAX_LINE);
  writeBytes = recv(s, buf, MAX_LINE, 0);

  printf("%s", buf);

  char dest[MAX_LINE];
  char ErrorArr[MAX_LINE];
  memcpy(dest, buf, MAX_LINE);

  const char* error = "error";
  strcpy(ErrorArr, error);

  if (strcmp(dest, ErrorArr) == 0) {
    perror( "stream-talk-server: open");
    close ( s );
    exit ( 1 );
  }

  //open local file
  if( (readBytes = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
  {
    perror( "stream-talk-client: open\n");
  }

  write( readBytes, buf, writeBytes); //write again or else we'll miss the first packet

  //receive from server then write.
  while( (writeBytes = recv(s, buf, MAX_LINE, 0)) > 0) //!= -1
  {
    write( readBytes, buf, writeBytes);
    memset(buf, '\0', MAX_LINE);
  }

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

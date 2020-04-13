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
#include <errno.h>


#define SERVER_PORT "80"
//#define MAX_LINE 256
#define GETREQUEST "GET /~kkredo/file.html HTTP/1.0\r\n\r\n"
#define PTAG "<p>"

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
	char *buf;//need this dynamic
	int s;
  int byteSize;
	//int len;


	host = "www.ecst.csuchico.edu";
	if ( argc == 2 ) {
    byteSize = strtol(argv[1], NULL, 10);                //startol(): onverts string to int
    buf = (char*) malloc(byteSize * sizeof(char));        //allocating array
	}
	else {
		fprintf( stderr, "usage: %s host\n", argv[0] );
		exit( 1 );
	}

	/* Lookup IP and connect to server */
	if ( ( s = lookup_and_connect( host, SERVER_PORT ) ) < 0 ) {
		exit( 1 );
	}

	//use send() only once
	if(send(s, GETREQUEST, strlen(GETREQUEST), 0) == -1) //@return num of bytes sent. or -1 for error.
	{
		perror( "stream-talk-client: send" );       // is this supposed to be here?
		close( s );
		exit( 1 );                                  //
	}

	int numBytes = 0;
	int byteCounter = 0;
	int pcount = 0;

	while((numBytes = recv(s, buf, byteSize, 0)) != 0)
	{
		byteCounter += numBytes; //counts bytes

    // char* temp = strstr(buf, PTAG);
    //
    // if(temp != NULL)
    // {
    //   pcount++;
    //   temp++;
    //   while(temp != NULL)
    //   {
    //     if(temp){
    //       pcount++;
    //       temp++;
    //     }
    //   }
    // }

		if(strstr(buf, PTAG) != NULL) //need more logic here. 3 cases total
		{
			pcount++;
      buf++;
      while(strstr(buf, PTAG) != NULL)
      {
        if(strstr(buf, PTAG))
        {
          pcount++;
          buf++;
        }
      }
		}

	}

  //if(pcount > 0 && byteCounter > 0){
  printf("Number of <p> tags: %d\n", pcount);
  printf("Number of bytes: %d\n", byteCounter);
  //}

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

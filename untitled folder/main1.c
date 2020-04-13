/**
*   @file       main.c
*   @brief
*
*   @author     Jonathan Cheon and Calvin Wai
*   @class      CSCI 446
*   @semester   Spring2019
*   @updated    2/23/2019
*/


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
#include <ctype.h>
#include <fcntl.h>



#define SERVER_PORT "80"
//#define MAX_LINE 256
#define GETREQUEST "GET /~kkredo/reset_instructions.pdf HTTP/1.0\r\n\r\n"


int lookup_and_connect( const char *host, const char *service );



int main() {
  char *host;
	char buf[1000];
	int s;
  int byteSize;


	host = "www.ecst.csuchico.edu";
	/* Lookup IP and connect to server */
	if ( ( s = lookup_and_connect( host, SERVER_PORT ) ) < 0 ) {
		exit( 1 );
	}

	if(send(s, GETREQUEST, strlen(GETREQUEST), 0) == -1)
	{
		perror( "stream-talk-client: send" );
		close( s );
		exit( 1 );
	}


  int rdonly;
  int rdBytes;
  int wrBytes;
  int wronly;

  //char* file = "reset_instructions.pdf";
  // if( (rdonly = recv(s, buf, 1000, 0) ) == -1)
  // {
  //   printf("Error opening file...\n");
  // }
  if ((wronly = open("local_file", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR, 0666)) == -1) {
    printf("Error opening file...\n");
  }


  while((rdBytes = recv(s, buf, 1000, 0)) > 0)
  {
    wrBytes = write(wronly, buf, rdBytes);
  }


  close(s);
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

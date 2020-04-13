/*	@author 		Jonathan Cheon & Calvin Wai
 *	@file				file_client.c
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
#include <sys/select.h>

#include "program3Functions.h"

//#define MAX_PACKET_DATA_SIZE 256
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
  char buf[MAX_PACKET_DATA_SIZE];
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

  //send filename to server
  int delivercheck = deliverFilename(s, fileName);

  if ( (delivercheck  == -1)) {
    perror( "stream-talk-client: send\n" );
    printf("bad socket betch");
    close( s );
    exit( 1 );
    return 0;
  }

  if( (delivercheck == -2)) {
    perror("client: send\n");
    printf("File doesnt exist. cannot open file betch\n");
    close(s);
    exit(1);
    return 0;
  }

  if(delivercheck == 0)
  {
    int lastSeqNum = 0;
    int seqNum;
    int openfd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); //check Otrunc

    //get data from server.
    while(( len = recv(s, buf, MAX_PACKET_DATA_SIZE, 0)) > 0)
    {
      //grabs seq num from packet.
      memcpy(&seqNum, buf, sizeof(int));
      printf("%d", seqNum);

      if(seqNum == lastSeqNum)
      {

        printf("%s", buf);

        write(openfd, (buf + sizeof(seqNum)), (len - sizeof(seqNum)));

        lastSeqNum++;
      }

      //send seqnum to server.
      packetErrorSend(s, buf, sizeof(lastSeqNum), 0);

    }
    close(openfd);
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


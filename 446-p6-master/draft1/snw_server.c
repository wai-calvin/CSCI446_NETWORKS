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

#include "program3Functions.h"

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
  char buf[MAX_PACKET_DATA_SIZE];
  //char tempBuf[MAX_PACKET_DATA_SIZE-4];
  int s, new_s;
  //char len;
  int rvalue;
  int seqNumCheck;

  fd_set set;

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
    rvalue = receiveAndVerifyFilename(new_s, buf);

    if ( rvalue  == -1 ) {
      printf("server: Error receiving filename\n");
      close( s );
      exit( 1 );
    }

    if( rvalue  == -2)
    {
      printf("server: Error opening the file\n");
      close(s);
      exit(1);
    }

    if( rvalue == -3)
    {
      printf("Server: error sending response to client\n");
      close(s);
      exit(1);
    }

    if( rvalue == 0)
    {

      int fd;
      int rdonly;
      int retVal;

      struct timeval tv;
      FD_SET( new_s, &set );


      int seqNum = 0;
      int maxSeqNum = 255; //for rollback

      fd = open(buf, O_RDONLY);

      while ( (rdonly = read(fd, buf + sizeof(seqNum), MAX_PACKET_DATA_SIZE - sizeof(seqNum)) != 0 )) {

          memcpy(&buf, &seqNum, sizeof(seqNum));
          packetErrorSend(new_s, buf, MAX_PACKET_DATA_SIZE, 0);

          tv.tv_sec = 0;
          tv.tv_usec = 50000;

          while ((retVal = select(new_s+1, &set, NULL, NULL,  &tv)) == 0) {
            tv.tv_sec = 0;
            tv.tv_usec = 50000;
            packetErrorSend(new_s, buf, MAX_PACKET_DATA_SIZE, 0);
          }

          //get ARQ from client.
          recv(new_s, buf, sizeof(seqNum), 0);
          memcpy(&seqNumCheck, buf, sizeof(seqNum));
          printf("seqnum: %d\n", seqNum);

          if(seqNum == maxSeqNum)
          {
            seqNum = 0;
          }
          if (seqNumCheck == seqNum) {
            seqNum++;
          }
          else {
            printf("SUP BETCH\n");
            continue;
          }
        }
      close(fd);
      }
    close(new_s);
    break;
    }
  close(s);
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


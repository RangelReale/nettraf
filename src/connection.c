/*********************************************************************
 * NetTrafD 1.0
 *
 * Rangel Reale (rangel.reale@yahoo.com)
 *
 * This is FREE SOFTWARE. Use it at your own risk.
 *
 *******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include "connection.h"
#include "nettraf.h"
#include "errrpt.h"

#define MAX_RECV_SIZE 1024
#define NTD_PASSWORD "nettraf" //you should change this!

void thread_senddata(struct nt_thread_info *info)
{
  char buf[255];
  int buflen;
  nettraf_data data;
  if (nettraf_get_data(info->interface, &data)) {
    if (info->isnew) {
      if (info->prev_rx == -1) info->prev_rx = data.data[NTI_RX];
      if (info->prev_tx == -1) info->prev_tx = data.data[NTI_TX];
      buflen = sprintf(buf, "NDATA %Ld %Ld\n", data.data[NTI_RX] - info->prev_rx, data.data[NTI_TX] - info->prev_tx);
    } else {
      buflen = sprintf(buf, "DATA %Ld %Ld\n", data.data[NTI_RX], data.data[NTI_TX]); 
    }
    send(info->sockfd, buf, buflen, 0);
  
    if (strncmp(info->lastipaddr, data.ipaddr, strlen(data.ipaddr)) != 0) {
     buflen = sprintf(buf, "IPADDR %s\n", data.ipaddr);
     send(info->sockfd, buf, buflen, 0);
    }

    strcpy(info->lastipaddr, data.ipaddr);
    info->prev_rx = data.data[NTI_RX];
    info->prev_tx = data.data[NTI_TX];
  } else {
    strcpy(buf, "ERROR\n");
    send(info->sockfd, buf, strlen(buf), 0);
  }
}

void checkitfname(struct nt_thread_info *info)
{
  int i;

  for (i = 0; i < strlen(info->interface); i++) {
    if (iscntrl(info->interface[i])) {
      info->interface[i] = '\0';
      break;
    }
  }
}

int thread_process(struct nt_thread_info *info, char *data, int len)
{
  char buf[255];	    
  if (info->loggedin == 0)
  {
   if (strncmp(data, "PASS", 4) == 0) {
    if ((len >= 6) && (len < 255)) {
     strcpy(buf, &data[5]);
     if (strncmp(buf,NTD_PASSWORD,strlen(NTD_PASSWORD)) == 0)
     {
       strcpy(buf, "PASS OK\n");
       send(info->sockfd, buf, strlen(buf), 0);
       info->loggedin = 1;  
     } else {
       strcpy(buf, "PASS BAD\n");
       send(info->sockfd, buf, strlen(buf), 0);
     }
     return 1;
    }
   }
   strcpy(buf, "PASS NEED\n");
   send(info->sockfd, buf, strlen(buf), 0);
   return 0;
  }
  
  if (strncmp(data, "ITF", 3) == 0) {
    if (len >= 5) {
      strncpy(info->interface, &data[4], MAX_INTERFACE_NAME);
      info->isnew = 0;
      checkitfname(info);
      #ifdef DEBUG
      printf("New interface: %s\n", info->interface);
      #endif
    }
  }
  if (strncmp(data, "NITF", 4) == 0) {
    if (len >= 6) {
      strncpy(info->interface, &data[5], MAX_INTERFACE_NAME);
      info->isnew = 1;
      info->prev_tx = -1;
      info->prev_rx = -1;
      checkitfname(info);
      #ifdef DEBUG
      printf("New interface: %s\n", info->interface);
      #endif
    }
  }
  if (strncmp(data, "QUIT", 4) == 0) {
    return 0;
  }
  if (strncmp(data, "DIE", 3) == 0) {
    exit(0);
    return 0;
  }

  return 1;
}

/* This funcion MUST release info */
void thread_function(struct nt_thread_info *info)
{
  int recv_size;
  char recv_buf[MAX_RECV_SIZE];

  info->interface[0] = '\0';

  /* set the socket to non-blocking */
  fcntl(info->sockfd, F_SETFL, O_NONBLOCK);

  while (1) {
    recv_size = recv(info->sockfd, recv_buf, MAX_RECV_SIZE-1, 0);
    if (recv_size == 0)
      /* The sender went down. Close the link */
      break;
    if (recv_size == -1) {
      if (errno != EWOULDBLOCK) 
	break;
    }
    if (recv_size > 0) {
      recv_buf[recv_size] = '\0';
    
#ifdef DEBUG
      printf("Received from %s: %s\n", inet_ntoa(info->conn.sin_addr), recv_buf);
#endif

      if (! thread_process(info, recv_buf, recv_size)) {
	send(info->sockfd, "Closing...\n", 5, 0);
	break;
      }
    }

    /* Send the connection data */
    if (info->interface[0] != '\0') {
      thread_senddata(info);
    }

    sleep(1);
  }

  close(info->sockfd);
  free(info);
}

void ntd_newconnection(int sockfd, struct sockaddr_in conn)
{
  struct nt_thread_info *info;
  pthread_t thread;

  info = malloc(sizeof(struct nt_thread_info));
  memset(info, 0, sizeof(struct nt_thread_info));
  info->sockfd = sockfd;
  info->conn = conn;
  info->loggedin = 0;
  info->lastipaddr[0] = "\0" ;
  if (pthread_create(&thread, NULL, (void *)&thread_function, info) != 0) {
    error_sys_report("pthread_create");
    close(sockfd);
    free(info);
  }
}

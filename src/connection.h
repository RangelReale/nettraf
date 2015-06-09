/***************************************************************
 * NetTrafD 1.0
 *
 * Rangel Reale (rangel.reale@yahoo.com)
 *
 * This is FREE SOFTWARE. Use it at your own risk.
 *
 **************************************************************/

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#define MAX_INTERFACE_NAME 30

typedef struct nt_thread_info {
  int sockfd;
  struct sockaddr_in conn;
  char interface[MAX_INTERFACE_NAME];
  int isnew;
  long long prev_rx, prev_tx;
  int loggedin;
  char lastipaddr[255];
} TINFO;

void ntd_newconnection(int sockfd, struct sockaddr_in conn);

#endif

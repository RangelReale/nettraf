/*************************************************************
 * NetTrafD 1.0
 *
 * Rangel Reale (rangel.reale@yahoo.com)
 *
 * This is FREE SOFTWARE. Use it at your own risk.
 *
 *************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <syslog.h>
#ifdef USE_LIBWRAP
#include <tcpd.h>
int allow_severity=LOG_NOTICE;
int deny_severity=LOG_WARNING;
struct request_info request;
#endif
#include "nettrafd.h"
#include "nettraf.h"
#include "connection.h"
#include "errrpt.h"

#define AUTHOR "Rangel Reale (rangel.reale@yahoo.com)"

int sockfd;

void do_cleanup()
{
  close(sockfd);
  nettraf_finish();
  error_finish();
}

void *sighandler(int sig)
{
  fprintf(stderr, "SIGNAL SIGINT!\n");
  do_cleanup();
  exit(0);
}

void showversion()
{
  char temp[100];
  sprintf(temp, "nettrafd version %s - by %s\n", VERSION, AUTHOR);
  printf(temp);
}

/* close all FDs >= a specified value */
void close_all_fds(int fd)
{
  int fdlimit = sysconf(_SC_OPEN_MAX);

  while (fd < fdlimit)
    close(fd++);
}

int run_as_daemon() 
{
  switch (fork())
    {
    case 0: break;
    case -1: return -1;
    default: _exit(0); /* exit the original process */
    }

  if (setsid() < 0)    /* shouldn't fail */
    return -1;

  switch (fork()) 
    {
    case 0: break;
    case -1: return -1;
    default: _exit(0);
    }

  /* change to root dir to ensure no directory is blocked */
  chdir("/");

  /* close all fds */
  close_all_fds(0);
  dup(0); dup(0);

  return 0;
}

int main(int argc, char *argv[])
{
  int new_fd;
  struct sockaddr_in server_addr, conn_addr;
  int sin_size;
  int c;
  char isdaemon = 0;

  /* check parameters */
  while (( c = getopt(argc, argv, "Dv")) != -1)
    switch(c) {
    case 'D':
      isdaemon = 1;
      break;
    case 'v':
      showversion();
      exit(0);
      break;
    case '?':
      return 1;
    default:
      abort();
    }

  if (isdaemon)
    run_as_daemon();

  /* initialize the error logging */
  error_init("nettrafd", isdaemon);

  /* initialize the nettraf library - used to fetch information from the system */
  if (!nettraf_init())
    error_exit("Cannot initialize nettraf library");

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    error_sys_exit("socket");
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(NETTRAFD_PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
    error_sys_exit("bind");
  }

  if (listen(sockfd, 10) == -1) {
    error_sys_exit("listen");
  }

  signal(SIGINT, (void *)&sighandler);

  while (1) {
    sin_size = sizeof(struct sockaddr_in);
    if ((new_fd = accept(sockfd, (struct sockaddr *)&conn_addr, &sin_size)) == -1) {
      error_sys_report("accept");
      continue;
    }
#ifdef DEBUG
    printf("server: got connection from %s\n", inet_ntoa(conn_addr.sin_addr));
#endif

#ifdef USE_LIBWRAP
    request_init(&request, RQ_DAEMON, "nettraf", RQ_FILE, 0, 0);
    fromhost(&request);
    if (! hosts_access(&request)) {
      char text[256];

      sprintf(text, "Connection from %s:%d REFUSED by LIBWRAP", inet_ntoa(conn_addr.sin_addr), ntohs(conn_addr.sin_port));
      error_report(text);
      close(new_fd);
    } else
#endif
    ntd_newconnection(new_fd, conn_addr);
  }

  /* should never reach this... */
  do_cleanup();
  exit(0);
}




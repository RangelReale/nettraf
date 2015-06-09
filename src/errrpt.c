/***************************************************
 * NetTrafD 1.0
 *
 * Rangel Reale (rangel.reale@yahoo.com)
 *
 * This is FREE SOFTWARE. Use it at your own risk.
 *
 **************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

int err_isdaemon = 0;

void error_init(const char *ident, int isdaemon)
{
  if (isdaemon)
    openlog(ident, LOG_PID, LOG_DAEMON);
  err_isdaemon = isdaemon;
}

void error_finish()
{
  if (err_isdaemon)
    closelog();
}

void error_sys_report(const char *msg)
{
  if (err_isdaemon)
    syslog(LOG_INFO, "%s failed: %d (%m)", msg, errno);
  else
    fprintf(stderr, "%s failed: %d (%s)\n", msg, errno, strerror(errno));
}

void error_sys_exit(const char *msg)
{
  error_sys_report(msg);
  exit(1);
}

void error_report(const char *msg)
{
  if (err_isdaemon)
    syslog(LOG_INFO, msg);
  else
    fprintf(stderr, "%s\n", msg);
}

void error_exit(const char *msg)
{
  error_report(msg);
  exit(1);
}

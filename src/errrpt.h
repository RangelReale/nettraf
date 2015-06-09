/***************************************************
 * NetTrafD 1.0
 *
 * Rangel Reale (rangel.reale@yahoo.com)
 *
 * This is FREE SOFTWARE. Use it at your own risk.
 *
 **************************************************/

#ifndef _ERRRPT_H_
#define _ERRRPT_H_

void error_init(const char *ident, int isdaemon);
void error_finish();
void error_sys_report(const char *msg);
void error_sys_exit(const char *msg);
void error_report(const char *msg);
void error_exit(const char *msg);

#endif

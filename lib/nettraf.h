/**************************************************************
 * NetTraf library 1.0
 *
 * Rangel Reale (rangel.reale@yahoo.com)
 *
 * This is FREE SOFTWARE. Use it at your own risk.
 *
 *************************************************************/

#ifndef _NETTRAF_H_
#define _NETTRAF_H_

#define false 0
#define true 1

#define NTI_RX 0
#define NTI_TX 1
#define NTI_COLL 2
#define NTI_DROP_RX 4
#define NTI_DROP_TX 5
#define NTI_ERR_TX 6
#define NTI_ERR_RX 7

typedef struct {
  long long data[8];
  char ipaddr[255];
} nettraf_data;

int nettraf_init();
int nettraf_get_data(char *interface, nettraf_data *data);
int nettraf_finish();

#endif

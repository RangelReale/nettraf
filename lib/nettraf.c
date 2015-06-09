/*****************************************************************
 * NetTraf library 1.0
 *
 * Rangel Reale (rangel.reale@yahoo.com)
 *
 * This is FREE SOFTWARE. Use it at your own risk.
 *
 * Notes:
 *   The code in this file is mostly borrowed from the ifled-0.6
 *   program, by Mattias Wadman (napolium@sudac.org)
 *
 *****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include "nettraf.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#define SIOCGIFCONF 0x8912 /* get iface list */

int nettraf_isKernel20 = false;
int nettraf_initialized = false;

int nettraf_init()
{
  struct utsname uname_data;

  uname(&uname_data);
  nettraf_isKernel20 = strncmp(uname_data.release, "2.0", 3) == 0;

  nettraf_initialized = true;
  return true;
}

int nettraf_finish()
{
  nettraf_initialized = false;
  return true;
}

int nettraf_get_data(char *interface, nettraf_data *data)
{
  char b[255];
  long long dummy;
  FILE *procfd;
  int numreqs = 30, sd, n; 
  struct ifconf ifc; 
  struct ifreq *ifr; 
  struct in_addr *ia;
  
  if (!nettraf_initialized)
    return false;

  if((procfd = fopen("/proc/net/dev","r")) == NULL)
    /* Cannot open file */
    return false;


  sd=socket(AF_INET, SOCK_STREAM, 0);
  ifc.ifc_buf = NULL; 
  ifc.ifc_len = sizeof(struct ifreq) * numreqs; 
  ifc.ifc_buf = realloc(ifc.ifc_buf, ifc.ifc_len);
  if (ioctl(sd, SIOCGIFCONF, &ifc) < 0) { 
	perror("SIOCGIFCONF"); 
  } 
  
  ifr = ifc.ifc_req; 

   for (n = 0; n < ifc.ifc_len; n += sizeof(struct ifreq)) {
	 ia= (struct in_addr *) ((ifr->ifr_ifru.ifru_addr.sa_data)+2); 
//	 fprintf(stdout, ">%6s< %-15s check! wana: >%s< \n", ifr->ifr_ifrn.ifrn_name, inet_ntoa(*ia),interface);
	 if (strncmp(ifr->ifr_ifrn.ifrn_name,interface,strlen(interface)) == 0) 
	 { 
	  sprintf(&data->ipaddr[0], "%-15s", inet_ntoa(*ia));
//	  fprintf(stdout, ">%6s< %-15s FOUND! wana: >%s< \n", ifr->ifr_ifrn.ifrn_name, inet_ntoa(*ia),interface);
	 }
    ifr++; 
   }
  free(ifc.ifc_buf);
  close(sd); 
  while(fgets(b,sizeof(b),procfd) !=  NULL)
    {
      char *bp = b;
      
      while(*bp == ' ')
	*bp++;
      
      if(strncmp(bp,interface,strlen(interface)) == 0 && *(bp+strlen(interface)) == ':' )
	{
	  bp = bp+strlen(interface)+1;
	  if(nettraf_isKernel20)
	    sscanf(bp,"%Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
		   &data->data[NTI_RX],&data->data[NTI_ERR_RX],&data->data[NTI_DROP_RX],
		   &dummy,&dummy,&data->data[NTI_TX],&data->data[NTI_ERR_TX],
		   &data->data[NTI_DROP_TX],&dummy,&data->data[NTI_COLL]);
	  else
	    sscanf(bp,"%Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
		   &data->data[NTI_RX],&dummy,&data->data[NTI_ERR_RX],&data->data[NTI_DROP_RX],
		   &dummy,&dummy,&dummy,&dummy,&data->data[NTI_TX],&dummy,
		   &data->data[NTI_ERR_TX],&data->data[NTI_DROP_TX],&dummy,&data->data[NTI_COLL]);	
	  fclose(procfd);
	  return true;
	}
    }
  fclose(procfd);
  /* Unable to find Interface */
  return false; 
}

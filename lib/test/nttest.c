#include <stdio.h>
#include "nettraf.h"

int main(int argc, char *argv[])
{
  nettraf_data data;

  if (argc < 2) {
    fprintf(stderr, "1 parameter is needed\n");
    exit(1);
  }

  if (!nettraf_init()) {
    fprintf(stderr, "Error initializing nettraf library\n");
    exit(1);
  }

  if (!nettraf_get_data(argv[1], &data)) {
    fprintf(stderr, "Error getting data\n");
    exit(1);
  }

  printf("%ld %ld %ld %ld %ld %ld %ld\n", data.data[NTI_RX], data.data[NTI_TX], data.data[NTI_COLL], data.data[NTI_DROP_RX], data.data[NTI_DROP_TX], data.data[NTI_ERR_TX], data.data[NTI_ERR_TX]);

  nettraf_finish();

  exit(0);
}

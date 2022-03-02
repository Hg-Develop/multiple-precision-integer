#include <stdio.h>
#include "mpi.h"

int main(void) {
  Mpi n;
  Mpi TWO;
  int d;

  Mpi_0(&n);
  Mpi_2(&TWO);

  for (d = 0; d < MPI_LENGTH; d++) {
    Mpi_setData(&n, d, 9);
  }

  while (1) {
    Mpi_print(&n);
    printf(" is %sprime.\n", Mpi_isPrime(&n) ? "" : "not ");

    if (Mpi_sub(&n, &TWO, &n) != OK) {
      break;
    }
  }

  return 0;
}

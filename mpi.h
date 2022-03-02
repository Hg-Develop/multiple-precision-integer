#ifndef MPI
#define MPI

#include <stdio.h>
#include <stdlib.h>

#define DECIMAL             (10)

#define MPI_LENGTH          (11)

#define SIGN_PLUS           (1)
#define SIGN_MINUS          (0)

#define IS_TRUE             (1)
#define IS_FALSE            (0)

#define CMP_BIG             (1)
#define CMP_EQL             (0)
#define CMP_SML             (-1)

#define ERR_INVALID_DIGIT   (-2)
#define ERR_INVALID_ASSIGN  (-3)
#define ERR_NULL            (-4)
#define ERR_INVALID_NUMBER  (-5)
#define ERR_OVERFLOW        (-6)
#define OK                  (0)

typedef struct mpi {
  int data[MPI_LENGTH];
  int sign;
} Mpi;

/* ========== prototype ========== */
int Mpi_getData   (Mpi *, int);
int Mpi_getSign   (Mpi *);

int Mpi_setData   (Mpi *, int, int);
int Mpi_setSign   (Mpi *, int);

int Mpi_0         (Mpi *);
int Mpi_1         (Mpi *);
int Mpi_2         (Mpi *);
int Mpi_random    (Mpi *, int);
int Mpi_fromInt   (Mpi *, int);

int Mpi_copy      (Mpi *, Mpi *);
int Mpi_copySign  (Mpi *, Mpi *);
int Mpi_copyData  (Mpi *, int, Mpi *, int);
int Mpi_swap      (Mpi *, Mpi *);

int Mpi_add       (Mpi *, Mpi *, Mpi *);
int Mpi_sub       (Mpi *, Mpi *, Mpi *);
int Mpi_mul       (Mpi *, Mpi *, Mpi *);
int Mpi_div       (Mpi *, Mpi *, Mpi *, Mpi *);

int Mpi_abs       (Mpi *, Mpi *);
int Mpi_pow       (Mpi *, Mpi *, Mpi *);
int Mpi_sqrt      (Mpi *, Mpi *);
int Mpi_gcd       (Mpi *, Mpi *, Mpi *);
int Mpi_inc       (Mpi *, Mpi *);
int Mpi_dec       (Mpi *, Mpi *);
int Mpi_cmp       (Mpi *, Mpi *);
int Mpi_is0       (Mpi *);
int Mpi_isPrime   (Mpi *);

int Mpi_shiftL   (Mpi *, Mpi *);
int Mpi_shiftR   (Mpi *, Mpi *);

int Mpi_print     (Mpi *);
int Mpi_reverse   (Mpi *, Mpi *);

#endif
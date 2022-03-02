#include "mpi.h"

/* ========== getter ========== */
int Mpi_getData(Mpi *n, int d) {
  if (d < 0 || d > MPI_LENGTH - 1) {
    return ERR_INVALID_DIGIT;
  }

  if (n == NULL) {
    return ERR_NULL;
  }

  return n->data[d];
}

int Mpi_getSign(Mpi *n) {
  if (n == NULL) {
    return ERR_NULL;
  }

  return n->sign;
}

/* ========== setter ========== */
int Mpi_setData(Mpi *o, int d, int x) {
  if (d < 0 || d > MPI_LENGTH - 1) {
    return ERR_INVALID_DIGIT;
  }

  if (x < 0 || x > DECIMAL - 1) {
    return ERR_INVALID_ASSIGN;
  }

  if (o == NULL) {
    return OK;
  }

  o->data[d] = x;
  
  return OK;
}

int Mpi_setSign(Mpi *o, int s) {
  if (s != SIGN_MINUS && s != SIGN_PLUS) {
    return ERR_INVALID_ASSIGN;
  }

  if (o == NULL) {
    return OK;
  }

  if (Mpi_is0(o) && s == 0) {
    return ERR_INVALID_NUMBER;
  }

  o->sign = s;

  return OK;
}

/* ========== setup ========== */
int Mpi_0(Mpi *o) {
  if (o == NULL) {
    return OK;
  }

  int i;
  
  for (i = 0; i < MPI_LENGTH; i++) {
    Mpi_setData(o, i, 0);
  }

  Mpi_setSign(o, SIGN_PLUS);

  return OK;
}

int Mpi_1(Mpi *o) {
  if (o == NULL) {
    return OK;
  }

  Mpi_0(o);
  Mpi_setData(o, 0, 1);

  return OK;
}

int Mpi_2(Mpi *o) {
  if (o == NULL) {
    return OK;
  }

  Mpi_0(o);
  Mpi_setData(o, 0, 2);

  return OK;
}

int Mpi_random(Mpi *o, int l) {
  if (o == NULL) {
    return OK;
  }
  
  int i;

  for (i = 0; i < l; i++) {
    Mpi_setData(o, i, random() % DECIMAL);
  }

  if (Mpi_is0(o)) {
    Mpi_setSign(o, SIGN_PLUS);
  }
  else {
    Mpi_setSign(o, random() % 2);
  }

  return OK;
}

int Mpi_fromInt(Mpi *o, int x) {
  if (MPI_LENGTH < 10) {
    return ERR_OVERFLOW;
  }

  if (o == NULL) {
    return OK;
  }

  int i;

  if (x < 0) {
    Mpi_setSign(o, SIGN_MINUS);
    x *= -1;
  }
  else {
    Mpi_setSign(o, SIGN_PLUS);
  }

  for (i = 0; i < MPI_LENGTH; i++) {
    Mpi_setData(o, i, x % DECIMAL);
    x /= DECIMAL;
  }

  return OK;
}

/* ========== copy ========== */
int Mpi_copy(Mpi *n, Mpi *o) {
  if (n != o) {
    Mpi_0(o);
  }

  if (n == NULL) {
    return ERR_NULL;
  }

  if (o == NULL) {
    return OK;
  }

  if (n == o) {
    return OK;
  }

  *o = *n;

  return OK;
}

int Mpi_copyData(Mpi *n, int d_n, Mpi *o, int d_o) {
  if (d_n < 0 || d_n > MPI_LENGTH) {
    return ERR_INVALID_DIGIT;
  }

  if (d_o < 0 || d_o > MPI_LENGTH) {
    return ERR_INVALID_DIGIT;
  }

  if (n == NULL) {
    Mpi_0(o);

    return ERR_NULL;
  }

  if (o == NULL) {
    return OK;
  }

  return Mpi_setData(o, d_o, Mpi_getData(n, d_n));
}

int Mpi_copySign(Mpi *n, Mpi *o) {
  if (n == NULL) {
    Mpi_0(o);

    return ERR_NULL;
  }

  if (o == NULL) {
    return OK;
  }

  return Mpi_setSign(o, Mpi_getSign(n));
}

int Mpi_swap(Mpi *a, Mpi *b) {
  if (a == NULL || b == NULL) {
    return ERR_NULL;
  }

  if (a == b) {
    return OK;
  }
  
  Mpi t;
  Mpi_copy(b, &t);
  Mpi_copy(a, b);
  Mpi_copy(&t, a);

  return OK;
}

/* ========== four arithmetic ========== */
int Mpi_add(Mpi *a, Mpi *b, Mpi *o) {
  if (a != o && b != o) {
    Mpi_0(o);
  }
  
  if (a == NULL || b == NULL) {
    return ERR_NULL;
  }

  if (Mpi_getSign(a) && !Mpi_getSign(b)) {
    Mpi abs_b;

    Mpi_abs(b, &abs_b);

    return Mpi_sub(a, &abs_b, o);
  }
  else if (!Mpi_getSign(a) && Mpi_getSign(b)) {
    Mpi abs_a;

    Mpi_abs(a, &abs_a);

    return Mpi_sub(b, &abs_a, o);
  }
  else if (!Mpi_getSign(a) && !Mpi_getSign(b)) {
    int rslt;
    Mpi abs_a;
    Mpi abs_b;

    Mpi_abs(a, &abs_a);
    Mpi_abs(b, &abs_b);
    rslt = Mpi_add(&abs_a, &abs_b, o);
    Mpi_setSign(o, SIGN_MINUS);

    return rslt;
  }

  Mpi _o;
  int carry = 0;
  int w;
  int i;

  for (i = 0; i < MPI_LENGTH; i++) {
    w = Mpi_getData(a, i) + Mpi_getData(b, i) + carry;
    Mpi_setData(&_o, i, w % DECIMAL);
    carry = w / DECIMAL;
  }

  if (carry) {
    return ERR_OVERFLOW;
  }

  Mpi_setSign(&_o, SIGN_PLUS);
  Mpi_copy(&_o, o);

  return OK;
}

int Mpi_sub(Mpi *a, Mpi *b, Mpi *o) {
  if (a != o && b != o) {
    Mpi_0(o);
  }
  
  if (a == NULL || b == NULL) {
    return ERR_NULL;
  }

  if (Mpi_getSign(a) && !Mpi_getSign(b)) {
    Mpi abs_b;

    Mpi_abs(b, &abs_b);

    return Mpi_add(a, &abs_b, o);
  }
  else if (!Mpi_getSign(a) && Mpi_getSign(b)) {
    int rslt;
    Mpi abs_a;

    Mpi_abs(a, &abs_a);
    rslt = Mpi_add(b, &abs_a, o);
    Mpi_setSign(o, SIGN_MINUS);

    return rslt;
  }
  else if (!Mpi_getSign(a) && !Mpi_getSign(b)) {
    Mpi abs_a;
    Mpi abs_b;

    Mpi_abs(a, &abs_a);
    Mpi_abs(b, &abs_b);
    return Mpi_sub(&abs_b, &abs_a, o);
  }

  Mpi _o;
  int borrow = 0;
  int w;
  int i;
    
  if (Mpi_cmp(a, b) >= CMP_EQL) {
    for (i = 0; i < MPI_LENGTH; i++) {
      w = Mpi_getData(a, i) - Mpi_getData(b, i) - borrow;

      if (w < 0) {
        Mpi_setData(&_o, i, w + DECIMAL);
        borrow = 1;
      }
      else {
        Mpi_setData(&_o, i, w);
        borrow = 0;
      }
    }

    Mpi_setSign(&_o, SIGN_PLUS);
  }
  else {
    for (i = 0; i < MPI_LENGTH; i++) {
      w = Mpi_getData(b, i) - Mpi_getData(a, i) - borrow;

      if (w < 0) {
        Mpi_setData(&_o, i, w + DECIMAL);
        borrow = 1;
      }
      else {
        Mpi_setData(&_o, i, w);
        borrow = 0;
      }
    }

    Mpi_setSign(&_o, SIGN_MINUS);
  }

  if (borrow) {
    return ERR_OVERFLOW;
  }

  Mpi_copy(&_o, o);

  return OK;
}

int Mpi_mul(Mpi *a, Mpi *b, Mpi *o) {
  if (a != o && b != o) {
    Mpi_0(o);
  }
  
  if (a == NULL || b == NULL) {
    return ERR_NULL;
  }

  if (Mpi_is0(a) || Mpi_is0(b)) {
    Mpi_0(o);
    return OK;
  }
  
  Mpi ONE;
  Mpi_1(&ONE);

  if (!Mpi_cmp(a, &ONE)) {
    Mpi_copy(b, o);

    return OK;
  }
  else if (!Mpi_cmp(b, &ONE)) {
    Mpi_copy(a, o);

    return OK;
  }

  if (Mpi_getSign(a) && !Mpi_getSign(b)) {
    int rslt;
    Mpi abs_b;

    Mpi_abs(b, &abs_b);
    rslt = Mpi_mul(a, &abs_b, o);
    Mpi_setSign(o, SIGN_MINUS);

    return rslt;
  }
  else if (!Mpi_getSign(a) && Mpi_getSign(b)) {
    int rslt;
    Mpi abs_a;

    Mpi_abs(a, &abs_a);
    rslt = Mpi_mul(b, &abs_a, o);
    Mpi_setSign(o, SIGN_MINUS);

    return rslt;
  }

  Mpi _o;
  int i;
  int j;

  Mpi_0(&_o);

  for (i = 0; i < MPI_LENGTH; i++) {
    Mpi x;
    int rslt;
    int h = 0;
    
    Mpi_0(&x);

    for (j = 0; j < MPI_LENGTH; j++) {
      int w = Mpi_getData(a, j) * Mpi_getData(b, i) + h;
      Mpi_setData(&x, j + i, w % DECIMAL);
      h = w / DECIMAL;
    }

    if (h) {
      return ERR_OVERFLOW;
    }

    rslt = Mpi_add(&_o, &x, &_o);

    if (rslt) {
      return rslt;
    }
  }

  Mpi_setSign(&_o, SIGN_PLUS);
  Mpi_copy(&_o, o);

  return OK;
}

int Mpi_div(Mpi *a, Mpi *b, Mpi *o_q, Mpi *o_r) {
  if (a != o_q && b != o_q) {
    Mpi_0(o_q);
  }

  if (a != o_r && b != o_r) {
    Mpi_0(o_r);
  }
  
  if (a == NULL || b == NULL) {
    return ERR_NULL;
  }

  if (Mpi_is0(b)) {
    return ERR_INVALID_NUMBER;
  }

  if (Mpi_is0(a)) {
    Mpi_0(o_q);
    Mpi_0(o_r);
  }

  if (Mpi_getSign(a) && !Mpi_getSign(b)) {
    int rslt;
    Mpi abs_b;

    Mpi_abs(b, &abs_b);
    rslt = Mpi_div(a, &abs_b, o_q, o_r);
    Mpi_setSign(o_q, 0);

    return rslt;
  }
  else if (!Mpi_getSign(a) && Mpi_getSign(b)) {
    int rslt;
    Mpi abs_a;

    Mpi_abs(a, &abs_a);
    rslt = Mpi_div(&abs_a, b, o_q, o_r);
    Mpi_setSign(o_q, 0);
    Mpi_setSign(o_r, 0);

    return rslt;
  }
  else if (!Mpi_getSign(a) && !Mpi_getSign(b)) {
    int rslt;
    Mpi abs_a, abs_b;

    Mpi_abs(a, &abs_a);
    Mpi_abs(b, &abs_b);
    rslt = Mpi_div(&abs_a, &abs_b, o_q, o_r);
    Mpi_setSign(o_r, 0);

    return rslt;
  }
  
  Mpi _o_q;
  Mpi _o_r;
  Mpi s;
  Mpi c;

  Mpi_0(&_o_q);

  Mpi_copy(a, &_o_r);

  while (Mpi_cmp(&_o_r, b) >= CMP_EQL) {
    Mpi_copy(b, &s);
    Mpi_1(&c);

    while (1) {
      Mpi t;

      if (Mpi_shiftL(&s, &t)) {
        break;
      }

      if (Mpi_cmp(&_o_r, &t) <= CMP_EQL) {
        break;
      }

      Mpi_copy(&t, &s);
      Mpi_shiftL(&c, &c);
    }

    Mpi_sub(&_o_r, &s, &_o_r);
    Mpi_add(&_o_q, &c, &_o_q);
  }
  
  Mpi_copy(&_o_r, o_r);
  Mpi_copy(&_o_q, o_q);

  return OK;
}

/* ========== other arithmetic ========== */
int Mpi_abs(Mpi *n, Mpi *o) {
  if (n != o) {
    Mpi_0(o);
  }

  if (n == NULL) {
    return ERR_NULL;
  }

  Mpi_copy(n, o);
  Mpi_setSign(o, 1);

  return OK;
}

int Mpi_cmp(Mpi *a, Mpi *b) {
  if (a == NULL || b == NULL) {
    return ERR_NULL;
  }

  if (a == b) {
    return CMP_EQL;
  }

  if (Mpi_getSign(a) && !Mpi_getSign(b)) {
    return CMP_BIG;
  }
  else if (!Mpi_getSign(a) && Mpi_getSign(b)) {
    return CMP_SML;
  }
  else if (!Mpi_getSign(a) && !Mpi_getSign(b)) {
    Mpi abs_a, abs_b;
    
    Mpi_abs(a, &abs_a);
    Mpi_abs(b, &abs_b);

    return Mpi_cmp(&abs_b, &abs_a);
  }
  else {
    int i;

    for (i = MPI_LENGTH - 1; i > -1; i--) {
      if (Mpi_getData(a, i) > Mpi_getData(b, i)) {
        return CMP_BIG;
      }
      else if (Mpi_getData(a, i) < Mpi_getData(b, i)) {
        return CMP_SML;
      } 
    }

    return CMP_EQL;
  }
}

int Mpi_pow(Mpi *a, Mpi *b, Mpi *o) {
  if (a != o && b != o) {
    Mpi_0(o);
  }

  if (a == NULL) {
    return ERR_NULL;
  }
  else if (b == NULL) {
    return ERR_NULL;
  }

  if (!Mpi_getSign(b)) {
    return ERR_INVALID_NUMBER;
  }

  Mpi ONE;
  Mpi_1(&ONE);
  
  if (Mpi_is0(b)) {
    Mpi_copy(&ONE, o);

    return OK;
  }
  else if (!Mpi_cmp(b, &ONE)) {
    Mpi_copy(a, o);

    return OK;
  }

  Mpi _o;
  Mpi t_a;
  Mpi t_b;
  Mpi div_q, div_r;
  Mpi TWO;
  int rslt = 0;

  Mpi_2(&TWO);

  Mpi_div(b, &TWO, &div_q, &div_r);

  if (Mpi_is0(&div_r)) {
    rslt += Mpi_mul(a, a, &t_a);
    rslt += Mpi_div(b, &TWO, &t_b, &div_r);
    rslt += Mpi_pow(&t_a, &t_b, &_o);
  }
  else {
    Mpi p;
    rslt += Mpi_sub(b, &ONE, &t_b);
    rslt += Mpi_pow(a, &t_b, &p);
    rslt += Mpi_mul(a, &p, &_o);
  }

  Mpi_copy(&_o, o);

  return (rslt) ? -1 : 0;
}

int Mpi_sqrt(Mpi *n, Mpi *o) {
  if (n != o) {
    Mpi_0(o);
  }

  if (n == NULL) {
    return ERR_NULL;
  }

  if (!Mpi_getSign(n)) {
    return ERR_INVALID_NUMBER;
  }

  Mpi _o;
  Mpi p;
  Mpi pp;
  Mpi TWO;

  Mpi_2(&TWO);

  Mpi_div(n, &TWO, &_o, NULL);
  Mpi_copy(&_o, &p);

  while (1) {
    Mpi_copy(&p, &pp);
    Mpi_copy(&_o, &p);
    
    Mpi_div(n, &p, &_o, NULL);
    Mpi_add(&_o, &p, &_o);
    Mpi_div(&_o, &TWO, &_o, NULL);

    if (!Mpi_cmp(&_o, &p)) {
      break;
    }

    if (!Mpi_cmp(&_o, &pp)) {
      if (Mpi_cmp(&p, &_o) < CMP_EQL) {
        Mpi_copy(&p, &_o);
      }
      break;
    }
  }

  Mpi_copy(&_o, o);

  return OK;
}

int Mpi_is0(Mpi *n) {
  if (n == NULL) {
    return ERR_NULL;
  }

  if (!Mpi_getSign(n)) {
    return IS_FALSE;
  }

  int i;

  for (i = 0; i < MPI_LENGTH; i++) {
    if (Mpi_getData(n, i)) {
      return IS_FALSE;
    }
  }

  return IS_TRUE;
}

int Mpi_isPrime(Mpi *n) {
  if (n == NULL) {
    return ERR_NULL;
  }
  
  Mpi TWO;
  int cmp;

  Mpi_2(&TWO);

  if (!(cmp = Mpi_cmp(n, &TWO))) {
    return IS_TRUE;
  }
  else if (cmp < 0) {
    return IS_FALSE;
  }

  Mpi r;
  Mpi t;
  
  Mpi_sqrt(n, &r);

  Mpi_mul(&r, &r, &t);
  if (!Mpi_cmp(&t, n)) {
    return IS_FALSE;
  }

  Mpi d;
  Mpi div_r;

  Mpi_inc(&TWO, &d);

  while (1) {
    if (Mpi_cmp(&d, &r) > CMP_EQL) {
      return IS_TRUE;
    }

    Mpi_div(n, &d, NULL, &div_r);

    if (Mpi_is0(&div_r)) {
      return IS_FALSE;
    }

    Mpi_add(&d, &TWO, &d);
  }
}

int Mpi_gcd(Mpi *a, Mpi *b, Mpi *o) {
  if (a != o && b != o) {
    Mpi_0(o);
  }

  if (a == NULL) {
    return ERR_NULL;
  }
  else if (b == NULL) {
    return ERR_NULL;
  }
  
  Mpi big;
  Mpi sml;

  if (Mpi_cmp(a, b) > -1) {
    Mpi_copy(a, &big);
    Mpi_copy(b, &sml);
  }
  else {
    Mpi_copy(b, &big);
    Mpi_copy(a, &sml);
  }

  if (Mpi_is0(&sml)) {
    Mpi_copy(&big, o);
    
    return 1;
  }

  while (1) {
    Mpi div_q;
    Mpi div_r;

    Mpi_div(&big, &sml, &div_q, &div_r);

    if (Mpi_is0(&div_r)) {
      Mpi_copy(&sml, o);
      return 1;
    }

    Mpi_swap(&big, &sml);
    Mpi_copy(&sml, &div_r);
  }
}

int Mpi_inc(Mpi *n, Mpi *o) {
  if (n != o) {
    Mpi_0(o);
  }
  
  if (n == NULL) {
    return ERR_NULL;
  }

  Mpi ONE;
  Mpi_1(&ONE);
  
  return Mpi_add(n, &ONE, o);
}

int Mpi_dec(Mpi *n, Mpi *o) {
  if (n != o) {
    Mpi_0(o);
  }
  
  if (n == NULL) {
    return ERR_NULL;
  }

  Mpi ONE;
  Mpi_1(&ONE);
  
  return Mpi_sub(n, &ONE, o);
}

/* ========== shift ========== */
int Mpi_shiftL(Mpi *n, Mpi *o) {
  if (n != o) {
    Mpi_0(o);
  }
  
  if (n == NULL) {
    return ERR_NULL;
  }

  Mpi _o;
  int i;

  for (i = 1; i < MPI_LENGTH; i++) {
    Mpi_copyData(n, i - 1, &_o, i);
  }

  Mpi_setData(&_o, 0, 0);
  Mpi_copySign(n, &_o);
  Mpi_copy(&_o, o);

  return Mpi_getData(n, MPI_LENGTH - 1);
}

int Mpi_shiftR(Mpi *n, Mpi *o) {
  if (n != o) {
    Mpi_0(o);
  }
  
  if (n == NULL) {
    return ERR_NULL;
  }

  Mpi _o;
  int i;

  for (i = 0; i < MPI_LENGTH - 1; i++) {
    Mpi_copyData(n, i + 1, &_o, i);
  }

  Mpi_setData(&_o, MPI_LENGTH - 1, 0);
  Mpi_copySign(n, &_o);
  Mpi_copy(&_o, o);

  return Mpi_getData(n, 0);
}

/* ========== other ========== */
int Mpi_print(Mpi *n) {
  if (n == NULL) {
    return ERR_NULL;
  }

  int i;
  
  if (!(Mpi_getSign(n))) {
    putchar('-');
  }

  for (i = MPI_LENGTH - 1; i > -1; i--) {
    putchar('0' + Mpi_getData(n, i));
  }

  return OK;
}

int Mpi_reverse(Mpi *n, Mpi *o) {
  if (n != o) {
    Mpi_0(o);
  }

  if (n == NULL) {
    return ERR_NULL;
  }

  if (o == NULL) {
    return OK;
  }

  int i;
  int l;

  for (l = MPI_LENGTH - 1; !Mpi_getData(n, l) && l > -1; l--);
  for (i = 0; i <= l; i++) {
    Mpi_copyData(n, i, o, l - i);
  }

  Mpi_copySign(n, o);

  return OK;
}

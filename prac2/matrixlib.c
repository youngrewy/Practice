#include <stdio.h>
#include <stdlib.h>
#include "matrixlib.h"

int main() {
  double x = 3.5;
  struct matrix mas, mas1, mas2;
  mas1 = matrix_input();
  //mas2 = matrix_input();
  //mas = matrix_mul(mas1, mas2);
  x = matrix_det(mas1);
  printf("%lf\n", x);
  //matrix_output(mas);
  return(0);
}

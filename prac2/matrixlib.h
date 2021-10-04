#include <stdio.h>
#include <stdlib.h>

struct matrix
{
  double** array;
  int width,height;
};

struct matrix matrix_input()
{
  int width, height, var;
  printf("Enter width and height of matrix:\n");
  var = scanf("%d%d", &width, &height);
  if (var != 2)
  {
    printf("Input is incorrect\n");
    exit(0);
  }
  printf("Enter matrix:\n");
  struct matrix mas;
  mas.array = (double**)malloc(sizeof(double*)*height);
  for (int j = 0; j < height; j++)
  {
    mas.array[j] = (double*)malloc(sizeof(double)*width);
  }
  mas.width = width;
  mas.height = height;
  int j, i;
  for(j = 0; j < height; j++)
  {
    for(i = 0; i < width; i++)
    {
      var = scanf("%lf", &mas.array[j][i]);
      if (var != 1)
      {
        printf("Input is incorrect\n");
        exit(0);
      }
    }
  }
  return mas;
}

void matrix_output(struct matrix mas)
{
  for(int j = 0; j < mas.height; j++)
  {
    for(int i = 0; i < mas.width; i++)
    {
      printf("%s%lf", "    ", mas.array[j][i]);
    }
    printf("\n");
  }
}

struct matrix matrix_add(struct matrix mas1, struct matrix mas2)
{
  struct matrix mas;
  if ((mas1.width == mas2.width) && (mas1.height == mas2.height))
  {
    mas.width = mas1.width;
    mas.height = mas1.height;
    mas.array = (double**)malloc(sizeof(double*)*mas.height);
    for (int j = 0; j < mas.height; j++)
    {
      mas.array[j] = (double*)malloc(sizeof(double)*mas.width);
    }
    int j, i;
    for(j = 0; j < mas.height; j++)
    {
      for(i = 0; i < mas.width; i++)
      {
        mas.array[j][i] = mas1.array[j][i] + mas2.array[j][i];
      }
    }
    return mas;
  }
  else
  {
    printf("Dimensions of the matrices do not match\n");
    exit(0);
  }
}

struct matrix matrix_mul(struct matrix mas1, struct matrix mas2)
{
  struct matrix mas;
  if (mas1.width == mas2.height)
  {
    mas.width = mas2.width;
    mas.height = mas1.height;
    mas.array = (double**)malloc(sizeof(double*)*mas.height);
    for (int j = 0; j < mas.height; j++)
    {
      mas.array[j] = (double*)malloc(sizeof(double)*mas.width);
    }
    int j, i, k;
    for(j = 0; j < mas.height; j++)
    {
      for(i = 0; i < mas.width; i++)
      {
        mas.array[j][i] = mas1.array[j][0] * mas2.array[0][i];
        for (k = 1; k < mas1.width; k++)
        {
          mas.array[j][i] += mas1.array[j][k] * mas2.array[k][i];
        }
      }
    }
    return mas;
  }
  else
  {
    printf("Dimensions of the matrices do not match\n");
    exit(0);
  }
}

struct matrix matrix_mul_on_val(struct matrix mas1, double x)
{
  struct matrix mas;
    mas.width = mas1.width;
    mas.height = mas1.height;
    mas.array = (double**)malloc(sizeof(double*)*mas.height);
    for (int j = 0; j < mas.height; j++)
    {
      mas.array[j] = (double*)malloc(sizeof(double)*mas.width);
    }
    int j, i;
    for(j = 0; j < mas.height; j++)
    {
      for(i = 0; i < mas.width; i++)
      {
        mas.array[j][i] = mas1.array[j][i] * x;
      }
    }
    return mas;
}

void swap_strings(struct matrix mas,int i, int* sign)
{
  double* a;
  int j;
  for (j = i; j < mas.height; j++)
  {
    if (mas.array[j][i] != 0)
      break;
  }
  if (j != i)
  {
    a = mas.array[j];
    mas.array[j] = mas.array[i];
    mas.array[i] = a;
    *sign *= -1;
  }
}

double matrix_det(struct matrix mas)
{
  int* sign;
  *sign = 1;
  if (mas.width == mas.height)
  {
    double det, tmp;
    for (int i = 0; i < mas.width; i++)
    {
      swap_strings(mas, i ,sign);
      if (mas.array[i][i] == 0) break;
      for (int j = 1; j + i < mas.height; j++)
        {
          tmp = -mas.array[j+i][i] / mas.array[i][i];
          for (int k = 0; k < mas.width; k++)
            {
              mas.array[j+i][k] += mas.array[i][k] * tmp;
            }
        }
    }
    det = mas.array[0][0];
    for (int i = 1; i < mas.width; i++)
      {
        det *= mas.array[i][i];
      }
      det *= *sign;
    return det;
  }
  else
  {
    printf("Dimensions of the matrix do not match\n");
    exit(0);
  }
}

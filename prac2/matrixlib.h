#include <stdio.h>
#include <stdlib.h>

struct Matrix
{
  double** array;
  int width,height;
};

struct Matrix matrix_input()
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
  struct Matrix matrixArray;
  matrixArray.array = (double**)malloc(sizeof(double*)*height);
  for (int j = 0; j < height; j++)
  {
    matrixArray.array[j] = (double*)malloc(sizeof(double)*width);
  }
  matrixArray.width = width;
  matrixArray.height = height;
  int j, i;
  for(j = 0; j < height; j++)
  {
    for(i = 0; i < width; i++)
    {
      var = scanf("%lf", &matrixArray.array[j][i]);
      if (var != 1)
      {
        printf("Input is incorrect\n");
        exit(0);
      }
    }
  }
  return matrixArray;
}

void matrix_output(struct Matrix matrixArray)
{
  for(int j = 0; j < matrixArray.height; j++)
  {
    for(int i = 0; i < matrixArray.width; i++)
    {
      printf("%s%lf", "    ", matrixArray.array[j][i]);
    }
    printf("\n");
  }
}

struct Matrix matrix_add(struct Matrix matrixArray1, struct Matrix matrixArray2)
{
  struct Matrix matrixArray;
  if ((matrixArray1.width == matrixArray2.width) && (matrixArray1.height == matrixArray2.height))
  {
    matrixArray.width = matrixArray1.width;
    matrixArray.height = matrixArray1.height;
    matrixArray.array = (double**)malloc(sizeof(double*)*matrixArray.height);
    for (int j = 0; j < matrixArray.height; j++)
    {
      matrixArray.array[j] = (double*)malloc(sizeof(double)*matrixArray.width);
    }
    int j, i;
    for(j = 0; j < matrixArray.height; j++)
    {
      for(i = 0; i < matrixArray.width; i++)
      {
        matrixArray.array[j][i] = matrixArray1.array[j][i] + matrixArray2.array[j][i];
      }
    }
    return matrixArray;
  }
  else
  {
    printf("Dimensions of the matrices do not match\n");
    exit(0);
  }
}

struct Matrix matrix_multiply(struct Matrix matrixArray1, struct Matrix matrixArray2)
{
  struct Matrix matrixArray;
  if (matrixArray1.width == matrixArray2.height)
  {
    matrixArray.width = matrixArray2.width;
    matrixArray.height = matrixArray1.height;
    matrixArray.array = (double**)malloc(sizeof(double*)*matrixArray.height);
    for (int j = 0; j < matrixArray.height; j++)
    {
      matrixArray.array[j] = (double*)malloc(sizeof(double)*matrixArray.width);
    }
    int j, i, k;
    for(j = 0; j < matrixArray.height; j++)
    {
      for(i = 0; i < matrixArray.width; i++)
      {
        matrixArray.array[j][i] = matrixArray1.array[j][0] * matrixArray2.array[0][i];
        for (k = 1; k < matrixArray1.width; k++)
        {
          matrixArray.array[j][i] += matrixArray1.array[j][k] * matrixArray2.array[k][i];
        }
      }
    }
    return matrixArray;
  }
  else
  {
    printf("Dimensions of the matrices do not match\n");
    exit(0);
  }
}

struct Matrix matrix_multiply_by(struct Matrix matrixArray1, double multiplier)
{
  struct Matrix matrixArray;
  matrixArray.width = matrixArray1.width;
  matrixArray.height = matrixArray1.height;
  matrixArray.array = (double**)malloc(sizeof(double*)*matrixArray.height);
  for (int j = 0; j < matrixArray.height; j++)
  {
    matrixArray.array[j] = (double*)malloc(sizeof(double)*matrixArray.width);
  }
  int j, i;
  for(j = 0; j < matrixArray.height; j++)
  {
    for(i = 0; i < matrixArray.width; i++)
    {
      matrixArray.array[j][i] = matrixArray1.array[j][i] * multiplier;
    }
  }
  return matrixArray;
}

void swap_strings(struct Matrix matrixArray,int string_pointer, int* sign)
{
  double* temp_array;
  int j;
  for (j = string_pointer; j < matrixArray.height; j++)
  {
    if (matrixArray.array[j][string_pointer] != 0)
      break;
  }
  if ((j != string_pointer) && (j < matrixArray.height))
  {
    temp_array = matrixArray.array[j];
    matrixArray.array[j] = matrixArray.array[string_pointer];
    matrixArray.array[string_pointer] = temp_array;
    *sign *= -1;
  }
}

double matrix_determinant(struct Matrix matrixArray)
{
  if (matrixArray.width == matrixArray.height)
  {
    double** temp_array;
    temp_array = (double**)malloc(sizeof(double*)*matrixArray.height);
    for (int j = 0; j < matrixArray.height; j++)
    {
      temp_array[j] = (double*)malloc(sizeof(double)*matrixArray.width);
    }
    for(int j = 0; j < matrixArray.height; j++)
    {
      for(int i = 0; i < matrixArray.width; i++)
      {
        temp_array[j][i] = matrixArray.array[j][i];
      }
    }
    int sign = 1;
    double determinant, tmp;
    for (int i = 0; i < matrixArray.width; i++)
    {
      swap_strings(matrixArray, i ,&sign);
      if (matrixArray.array[i][i] == 0) break;
      for (int j = 1; j + i < matrixArray.height; j++)
        {
          tmp = -matrixArray.array[j+i][i] / matrixArray.array[i][i];
          for (int k = 0; k < matrixArray.width; k++)
            {
              matrixArray.array[j+i][k] += matrixArray.array[i][k] * tmp;
            }
        }
    }
    determinant = matrixArray.array[0][0];
    for (int i = 1; i < matrixArray.width; i++)
      {
        determinant *= matrixArray.array[i][i];
      }
    determinant *= sign;
    for(int j = 0; j < matrixArray.height; j++)
    {
      for(int i = 0; i < matrixArray.width; i++)
      {
        matrixArray.array[j][i] = temp_array[j][i];
      }
    }
    return determinant;
  }
  else
  {
    printf("Dimensions of the matrix do not match\n");
    exit(0);
  }
}

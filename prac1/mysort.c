#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct strings {
  int len;
  int buffer_size;
  char** strs;
};

void print_strings(struct strings* strs) {
  for (int i = 0; i < strs->len; ++i) {
    printf("%s\n", strs->strs[i]);
  }
}

void free_strings(struct strings* strs) {
  if (strs == 0)
    return;

  for (int i = 0; i < strs->buffer_size; ++i) {
    if (strs->strs[i] != 0)
      free(strs->strs[i]);
  }

  free(strs->strs);
  free(strs);
}

struct strings* read_strings() {
  struct strings* ret = (struct strings*)malloc(sizeof(struct strings));
  ret->buffer_size = 4;
  ret->strs = (char**)malloc(sizeof(char*) * ret->buffer_size);
  //зануляем указатели, чтоб не было рандомных байтов
  memset(ret->strs, 0, sizeof(char*) * ret->buffer_size);
  ret->len = 0;

  while (1) {
    //для текущей строки
    int buffer_size = 16;
    int cur_size = 0;

    char* str = (char*)malloc(sizeof(char) * buffer_size);

    while (1) {
      char character = getchar();

      if (character == EOF) {
        //end of input
        free(str);
        return ret;
      }
      else if (character == '\n') {
        //end of string
        str[cur_size] = 0;

        //закончилась память выделенная malloc для массива строк
      if (ret->len == ret->buffer_size) {
        ret->buffer_size *= 2;
        ret->strs = (char**)realloc(ret->strs, sizeof(char*) * ret->buffer_size);
        //зануляем новые указатели
        for (int i = ret->len; i < ret->buffer_size; ++i)
           ret->strs[i] = 0;
        }
        ret->strs[ret->len++] = str;

        break;
      }
      else {
        //закончилась память выделленная malloc для строки
        if (cur_size == buffer_size - 1) {
          buffer_size *= 2;
          str = (char*)realloc(str, sizeof(char) * buffer_size);
        }
        str[cur_size++] = character;
      }
    }
  }
  free_strings(ret);
  return 0;
}

void sort_strings(struct strings* strs) {
  for (int i = 0; i < strs->len-1; i++)
  {
    for (int j = strs->len-1; j > i; j--)
    {
      if (strcmp(strs->strs[j - 1], strs->strs[j]) > 0)
      {
        char* temp = strs->strs[j - 1];
        strs->strs[j - 1] = strs->strs[j];
        strs->strs[j] = temp;
      }
    }
  }
}

int main() {
  struct strings* strs = read_strings();
  sort_strings(strs);
  print_strings(strs);
  free_strings(strs);
}

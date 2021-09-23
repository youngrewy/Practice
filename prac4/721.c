#include <stdio.h>
#include <string.h>

int find_max_length(FILE* file_pointer)
{
  int current_length = 0, max_length = 0;
  char character;
  while((character = fgetc(file_pointer)) != EOF)
  {
    if (character == '\n')
    {
      if (current_length > max_length)
        max_length = current_length;
      current_length = 0;
    }
    else current_length++;
  }
  return max_length;
}

void find_most_common_string(char *most_common_string, int max_length, FILE *fp)
{
    int max_number = 0, current_number, current_offset, current_most_common_string;
    char current_string[max_length], actual_string[max_length];
    fseek(fp, 0, 0);
    while (fgets(current_string, max_length, fp) != NULL)
    {
        current_offset = ftell(fp);
        current_number = 0;
        while (fgets(actual_string, max_length, fp) != NULL)
        {
            if (strcmp(current_string, actual_string) == 0)
            {
                current_number++;
            }
        }
        if (current_number > max_number)
        {
            max_number = current_number;
            current_most_common_string = current_offset;
        }
        fseek(fp, current_offset, 0);
    }
    fseek(fp, current_most_common_string, 0);
    fgets(most_common_string, max_length, fp);
    return;
}

int main()
{
    char file_name[20];
    printf("%s\n", "Input name of file");
    scanf("%s", file_name);
    FILE *fp = fopen(file_name, "r+");
    if (fp != NULL)
    {
        int max_length;
        max_length = find_max_length(fp);
        char most_common_string[max_length];
        find_most_common_string(most_common_string, max_length, fp);
        printf("%s\n", most_common_string);
    }
    else
    {
        printf("%s\n", "Error: No such file exists!");
    }
    return 0;
}

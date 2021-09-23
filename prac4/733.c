#include <stdio.h>
#include <string.h>


void length_sort_file(FILE *fp, int max_length)
{
    long max_length_offset, current_offset, start_sorted_part_offset;
    int current_max_length = 0, current_length;
    char current_string[max_length], longest_string[max_length];
    fseek(fp, 0, 2);
    start_sorted_part_offset = ftell(fp);
    while (start_sorted_part_offset != 0)
    {
        fseek(fp, 0, 0);
        current_offset = 0;
        max_length_offset = 0;
        current_max_length = 0;
        while (current_offset != start_sorted_part_offset)
        {
            fgets(current_string, max_length, fp);
            current_length = strlen(current_string);
            if (current_max_length <= current_length)
            {
                current_max_length = current_length;
                max_length_offset = current_offset;
            }
            current_offset = ftell(fp);
        }
        fseek(fp, max_length_offset, 0);
        fgets(longest_string, max_length, fp);
        long put_offset = max_length_offset, get_offset = ftell(fp);
        while(get_offset != start_sorted_part_offset)
        {
            fgets(current_string, max_length, fp);
            get_offset = ftell(fp);
            fseek(fp, put_offset, 0);
            fputs(current_string, fp);
            put_offset = ftell(fp);
            fseek(fp, get_offset, 0);
        }
        start_sorted_part_offset = put_offset;
        fseek(fp, put_offset, 0);
        fputs(longest_string, fp);
    }
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
        printf("%s\n", "Input max length of file string");
        scanf("%d", &max_length);
        length_sort_file(fp, max_length);
        fclose(fp);
    }
    else
    {
        printf("%s\n", "Error: No such file exists!");
    }
    return 0;
}

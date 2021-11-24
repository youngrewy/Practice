#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <fcntl.h>

struct list
{
	char *element;
	struct list *next;
};

void addWord(struct list **List, char *word, int *count)
{
	if (!*List)
	{
		*List = (struct list*)malloc(sizeof(struct list));
		(*List)->element = (char*)malloc(sizeof(char)*(strlen(word) + 1));
		strcpy((*List)->element, word);
		if (strcmp(word, "&") != 0)
		{
			(*count)++;
		}
		(*List)->next = NULL;
	} else
	{
		addWord(&((*List)->next), word, count);
	}
}

void printList(struct list *List)
{
	if (List)
	{
		printf("	%s\n", List->element);
		printList(List->next);
	}
}

void deleteList(struct list *List)
{
	if (List)
	{
		deleteList(List->next);
		free(List->element);
		free(List);
	}
}

void addAndFree(struct list **List, char **word, int *count)
{
	if (strlen(*word) > 0)
	{
		addWord(List, *word, count);
		free(*word);
		*word = (char*)malloc(sizeof(char));
		*word[0] = '\0';
	}
}

int symbol(int c)
{
	if((c=='&')||(c=='>')||(c=='<')||(c=='|')||(c==','))
	{
		return 1;
	} else
	{
		return 0;
	}
}

void my_cd(struct list *List, int count)
{
	int d;
	if (count > 2)
	{
		printf("Wrong number of parameters of command cd! Please, try again\n");
	} else
	{
		if (List->next)
		{
			if (!strcmp((List->next)->element, "~"))
			{
				chdir("/home");
			} else
			{
				if ((d = chdir((List->next)->element)) == (-1))
				{
					printf("Error in command cd!");
				}
			}
		} else {
			chdir("/home");
		}
	}
}

char **makeArray(struct list *List, int size)
{
	int counter = 0;
	char** newArray;
	struct list *temp = List;
	newArray = (char**)malloc(sizeof(char*) * (size + 1));
	while (temp)
	{
		if (strcmp(temp->element, "&") != 0){
			newArray[counter] = temp->element;
			counter++;
		}
		temp = temp->next;
	}
	newArray[counter] = NULL;
	return newArray;
}

void spec_symbols(char** word, char character, int length, char spec_symbol)
{
	if (character == spec_symbol)
	{
		character = getchar();
		if (character == spec_symbol)
		{
			length = strlen(*word);
			*word = realloc(*word, sizeof(char) * (length + 2));
			(*word)[length] = character;
			(*word)[length+1] = '\0';
		} else
		{
			ungetc(character, stdin);
		}
	}
	return;
}

int main()
{
	char *word = malloc(sizeof(char)), buf[256];
	word[0] = '\0';
	int length, character, count, status = 0;
	struct list *List = NULL;
	char** arrayOfWords;
	printf("[%s] ", getcwd(buf, 256));
	while (1)
	{
		List = NULL;
		arrayOfWords = NULL;
		count = 0;
		character = getchar();
		while ((character != EOF) && (character!= '\n'))
		{
			while (isspace(character))
			{
				if(character == '\n')
				{
					addAndFree(&List, &word, &count);
				}
				character = getchar();
			}
			if (character == '"')
			{
				character = getchar();
				while ((character != EOF) && (character != '\n') && (character != '"'))
				{
					length = strlen(word);
					word = realloc(word, sizeof(char) * (length + 2));
					word[length] = character;
					word[length+1] = '\0';
					character = getchar();
				}
				if (character == '"')
				{
					character = getchar();
					if (isspace(character) || (character == EOF) || (character == '\n'))
					{
						addAndFree(&List, &word, &count);
					}
				} else
				{
					addAndFree(&List, &word, &count);
				}
			} else
			{
				while ((character != '"') && (character != EOF) && (character != '\n') && (!isspace(character)) && (character != '&') && (!symbol(character)))
				{
					length = strlen(word);
					word = realloc(word, sizeof(char) * (length + 2));
					word[length] = character;
					word[length+1] = '\0';
					character = getchar();
				}
				if (character != '"')
				{
					addAndFree(&List, &word, &count);
				}
				if (symbol(character))
				{
					length = strlen(word);
					word = realloc(word, sizeof(char) * (length + 2));
					word[length] = character;
					word[length+1] = '\0';
          spec_symbols(&word, character, length, '&');
					spec_symbols(&word, character, length, '>');
					spec_symbols(&word, character, length, '|');
					addAndFree(&List, &word, &count);
					character = getchar();
				}
			}
		}
		if (List)
		{
			if (!strcmp(List->element, "cd"))
			{
				my_cd(List, count);
			} else
			{
				arrayOfWords = makeArray(List, count);
				if (fork())
				{
					wait(&status);
					if (!status != 0)
					{
						perror(List->element);
					}
				} else
				{
					execvp(List->element, arrayOfWords);
					perror(List->element);
					exit(2);
				}
			}
			free(arrayOfWords);
		}
		deleteList(List);
		printf("[%s] ", getcwd(buf, 256));
	}
	return 0;
}

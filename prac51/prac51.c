#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

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
		(*count)++;
		(*List)->next = NULL;
	} else
	{
		addWord(&((*List)->next), word, count);
	}
	return;
}

void printList(struct list *List)
{
	if(List)
	{
		printf("	%s\n", List->element);
		printList(List->next);
	}
	return;
}

void deleteList(struct list *List)
{
	if(List)
	{
		deleteList(List->next);
		free(List->element);
		free(List);
	}
	return;
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
	return;
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
	char *word = malloc(sizeof(char));
	word[0] = '\0';
	int length, character, count;
	struct list *List=NULL;
	while ((character = getchar()) != EOF)
	{
		List = NULL;
		count = 0;
		while ((character != EOF) && (character != '\n'))
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
				}
				else
				{
					addAndFree(&List, &word, &count);
				}
			} else
			{
				while ((character != '"') && (character != EOF) && (character != '\n') && (!isspace(character)) && (!symbol(character)))
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
		printList(List);
		deleteList(List);
	}
	return 0;
}

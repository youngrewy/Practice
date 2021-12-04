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
#include <signal.h>

struct List
{
	char *element;
	struct List *next;
};

void addWord(struct List **list, char *word, int *count)
{
	if (!*list)
	{
		*list = (struct List*)malloc(sizeof(struct List));
		(*list)->element = (char*)malloc(sizeof(char)*(strlen(word) + 1));
		strcpy((*list)->element, word);
		if (strcmp(word, "&") != 0)
		{
			(*count)++;
		}
		(*list)->next = NULL;
	} else
	{
		addWord(&((*list)->next), word, count);
	}
}

void printList(struct List *list)
{
	if (list)
	{
		printf("	%s\n", list->element);
		printList(list->next);
	}
}

void deleteList(struct List *list)
{
	if (list)
	{
		deleteList(list->next);
		free(list->element);
		free(list);
	}
}

void addAndFree(struct List **list, char **word, int *count)
{
	if (strlen(*word) > 0)
	{
		addWord(list, *word, count);
		free(*word);
		*word = (char*)malloc(sizeof(char));
		*word[0] = '\0';
	}
}

int symbol(int character)
{
	if((character == '&')||(character == '>')||(character == '<')
		||(character == '|')||(character == ','))
	{
		return 1;
	} else
	{
		return 0;
	}
}

void mycd(struct List *list, int count)
{
	int d;
	if (count > 2)
	{
		printf("Wrong number of parameters of command cd! Please, try again\n");
	} else
	{
		if (list->next)
		{
			if (!strcmp((list->next)->element, "~"))
			{
				chdir("/home");
			} else
			{
				if ((d = chdir((list->next)->element)) == (-1))
				{
					printf("Error in command cd!\n");
				}
			}
		} else {
			chdir("/home");
		}
	}
}

char **makeArray(struct List *list, int size)
{
	int counter = 0;
	char** newArray;
	struct List *temp = list;
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

void specialSymbols(char** word, char character, int length, char specialSymbol)
{
	if (character == specialSymbol)
	{
		character = getchar();
		if (character == specialSymbol)
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

int isBackgroundMode(struct List *list)
{
	if (list)
	{
		if (!list->next)
		{
			if (!strcmp(list->element, "&"))
			{
				return 1;											//if it is a background mode
			} else
			{
				return 0;											//if it is NOT a background mode
			}
		} else if (!strcmp(list->element, "&"))
		{
			return 2;												//if & is NOT in right place(error)
		} else
		{
			return isBackgroundMode(list->next);
		}
	} else
	{
		return 3;													//if list is empty(error)
	}
}

int isConveyor(struct List *list)
{
	if (list)
	{
		if (!strcmp(list->element, "|"))
		{
			return 1;
		} else
		{
			return isConveyor(list->next);
		}
	} else
	{
		return 0;
	}
}

int howManyElements(struct List *list)
{
	int counter = 0;
	while (list)
	{
		if (!strcmp(list->element, "|"))
		{
			counter++;
		}
		list = list->next;
	}
	return counter + 1;
}

char **makeArrayOneProc(struct List **list, char **fileName, int *error)
{
	char **newArray;
	struct List *temp = *list;
	int size = 0, counter = 0, flag = 0;
	while ((temp) && (strcmp(temp->element, "|") != 0))
	{
		if ((strcmp(temp->element, ">>") && strcmp(temp->element, ">")) != 0)
		{
			size++;
		}
		temp = temp->next;
	}
	newArray = (char**)malloc(sizeof(char*) * (size + 1));
	while ((*list) && (strcmp((*list)->element, "|") != 0))
	{
		if (!strcmp((*list)->element, ">>"))
		{
			flag = 1;
			(*list) = (*list)->next;
			*fileName = (*list)->element;
			(*list) = (*list)->next;
		} else if (!strcmp((*list)->element, ">"))
		{
			flag = 2;
			(*list) = (*list)->next;
			*fileName = (*list)->element;
			(*list) = (*list)->next;
		}
		 else if (!strcmp((*list)->element, "&"))
		{
			flag = -1;
			(*list) = (*list)->next;
		} else
		{
			newArray[counter] = (*list)->element;
			counter++;
			(*list) = (*list)->next;
		}
	}
	if (*list)
	{
		(*list) = (*list)->next;
		if (flag == 1)
		{
			*error = 1;
		} else if (flag == (-1))
		{
			*error = 2;
		}
	}
	newArray[counter] = NULL;
	return newArray;
}

char ***makeArrayAllProc(struct List **list, int *size, char **fileName, int *error)
{
	char ***newArray = (char***)malloc(sizeof(char*) * howManyElements(*list));
	int counter = 0;
	while (*list)
	{
		newArray[counter] = makeArrayOneProc(list, fileName, error);
		counter++;
	}
	*size = counter;
	return newArray;
}

void executeConveyor(int size, char **conveyor[], int flag, char *fileName)
{
	int counter, fd[size - 1][2], fileDescriptor = 0;
	pipe(fd[0]);
	if (fork() == 0){
		close(fd[0][0]);
		dup2(fd[0][1], 1);
		close(fd[0][1]);
		printf("[%s]", *conveyor[0]);   //pointer to group of proccesses
		execvp(*conveyor[0], conveyor[0]);
		_exit(0);
	}
	wait(NULL);
	close(fd[0][1]);
	for (counter = 1; counter <= (size - 2); counter++)
	{
		pipe(fd[counter]);
		if (fork() == 0)
		{
			dup2(fd[counter][1], 1);
			close(fd[counter][1]);
			dup2(fd[counter - 1][0], 0);
			close(fd[counter - 1][0]);
			execvp(*conveyor[counter], conveyor[counter]);
			_exit(0);
		}
		wait(NULL);
		close(fd[counter][1]);
		close(fd[counter - 1][0]);
	}
	if (flag == 1)
	{
		fileDescriptor = open(fileName, O_WRONLY|O_APPEND|O_CREAT, 0666);
		if (fork() == 0)
		{
			close(fd[size - 2][1]);
			dup2(fd[size - 2][0], 0);
			close(fd[size - 2][0]);
			dup2(fileDescriptor, 1);
			close(fileDescriptor);
			execvp(*conveyor[size - 1], conveyor[size - 1]);
			_exit(0);
		}
		wait(NULL);
		close(fileDescriptor);
	} else if (flag == 2)
	{
		fileDescriptor = open(fileName, O_WRONLY|O_CREAT, 0666);
		if (fork() == 0)
		{
			close(fd[size - 2][1]);
			dup2(fd[size - 2][0], 0);
			close(fd[size - 2][0]);
			dup2(fileDescriptor, 1);
			close(fileDescriptor);
			execvp(*conveyor[size - 1], conveyor[size - 1]);
			_exit(0);
		}
		wait(NULL);
		close(fileDescriptor);
	} else
	{
		if (fork() == 0)
		{
			close(fd[size - 2][1]);
			dup2(fd[size - 2][0], 0);
			close(fd[size - 2][0]);
			execvp(*conveyor[size - 1], conveyor[size - 1]);
			_exit(0);
		}
	}
	wait(NULL);
	close(fd[size - 2][0]);
}

int isAddToEnd(struct List *list)
{
	while (list)
	{
		if (!strcmp(list->element, ">>"))
		{
			return 1;
		} else if (!strcmp(list->element, ">"))
		{
			return 2;
		}else
		{
			return isAddToEnd(list->next);
		}
	}
	return 0;
}

void addToEnd(int size, char **conveyor[], char *fileName, int isConv, int isEnd)
{
	int fileDescriptor;
	if (isConv)
	{
		executeConveyor(size, conveyor, isEnd, fileName);
	} else if (isEnd == 1)
	{
		fileDescriptor = open(fileName, O_WRONLY|O_CREAT|O_APPEND, 0666);
		if (fork() == 0)
		{
			dup2(fileDescriptor, 1);
			execvp(*conveyor[0], conveyor[0]);
			_exit(0);
		}
		wait(NULL);
		close(fileDescriptor);
	} else if (isEnd == 2)
	{
		fileDescriptor = open(fileName, O_WRONLY|O_CREAT, 0666);
		if (fork() == 0)
		{
			dup2(fileDescriptor, 1);
			execvp(*conveyor[0], conveyor[0]);
			_exit(0);
		}
		wait(NULL);
		close(fileDescriptor);
	}
}

int main()
{
	char *word = malloc(sizeof(char)), buf[256], *fileName;
	word[0] = '\0';
  int length, character, count, flag, status, size, isConv = 0, isEnd = 0, error = 0;
	pid_t pid;
	struct List *list = NULL, *temp;
	char** arrayOfWords, ***conveyor;
	printf("[%s] ", getcwd(buf, 256));
	signal(SIGCHLD, SIG_IGN);
	while (1)
	{
		list = NULL;
		arrayOfWords = NULL;
		count = 0;
		character = getchar();
		while ((character != EOF) && (character!= '\n'))
		{
			while (isspace(character))
			{
				if(character == '\n')
				{
					addAndFree(&list, &word, &count);
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
						addAndFree(&list, &word, &count);
					}
				} else
				{
					addAndFree(&list, &word, &count);
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
					addAndFree(&list, &word, &count);
				}
				if (symbol(character))
				{
					length = strlen(word);
					word = realloc(word, sizeof(char) * (length + 2));
					word[length] = character;
					word[length+1] = '\0';
          specialSymbols(&word, character, length, '&');
					specialSymbols(&word, character, length, '>');
					specialSymbols(&word, character, length, '|');
					addAndFree(&list, &word, &count);
					character = getchar();
				}
			}
		}
    if (list)
		{
			if (!strcmp(list->element, "cd"))
			{
				mycd(list, count);
			} else	if ((isConv = isConveyor(list)) && !isAddToEnd(list))
			{
				temp = list;
				conveyor = makeArrayAllProc(&temp, &size, &fileName, &error);
				if (error == 0)
				{
					executeConveyor(size, conveyor, isEnd, fileName);
					for (int counter = 0; counter < size; counter++)
					{
						free(conveyor[counter]);
					}
				} else if (error == 2)
				{
					printf("Wrong position of &!\n");
				}
				error = 0;
				free(conveyor);
			} else if ((isEnd = isAddToEnd(list)))
			{
				temp = list;
				conveyor = makeArrayAllProc(&temp, &size, &fileName, &error);
				if (error == 0)
				{
					addToEnd(size, conveyor, fileName, isConv, isEnd);
				} else	if (error)
				{
					printf("Wrong position of >> or >!\n");
				} else
				{
					printf("Wrong position of &!\n");
				}
				error = 0;
				for (int counter = 0; counter < size; counter++)
				{
					free(conveyor[counter]);
				}
				free(conveyor);
			} else
			{
				arrayOfWords = makeArray(list, count);
				if ((flag = isBackgroundMode(list)) == 0)
				{
					if ((pid  = fork()) == 0)
					{
						execvp(list->element, arrayOfWords);
						perror(list->element);
						exit(2);
					} else
					{
						wait(&status);
						if (!status != 0)
						{
							perror(list->element);
						}
					}
				}
				if(flag == 2)
				{
					printf("Wrong position of & !\n");
				}
			 	if(flag == 1)
				{
					if (fork() == 0)
					{
						execvp(list->element, arrayOfWords);
						perror(list->element);
						exit(2);
					}
				}
			}
			free(arrayOfWords);
		}
		deleteList(list);
		printf("[%s] ", getcwd(buf, 256));
	}
	return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

void cd(char * args[])
{
	//if user has specified a path, cd to there.
	//Otherwise, cd to HOME
	if(args[1])
	{
		if(chdir(args[1]) == -1)
			printf("Error: Could not change directory");
	}
	else
	{
		char home[100];
		getcwd(home, 100);
		if(chdir(home) == -1)
			printf("Error: Could not change directory");
	}
}

void searchPath(char * source, char * destination)
{
	char * path = getenv("PATH");
	char * check = strtok(path, ";");
	while(check)
	{
		strcpy(destination, check);
		strcat(destination, "/");
		strcat(destination, source);
		if(access(destination, F_OK) != -1)
			return;
		check = strtok(NULL, ";");
	}		
}

void runCommandsHelper(char* commands[], char * env[])
{
	char* head[20];
	//redirect the current head if 
	{
		char * head_base = commands[0];
		int len = strlen(head_base);
		for(int i = 0; i < len; i++)
			if(head_base[i] == '>')
			{
				if(i < len - 1 && head_base[i + 1] == '>')
				{
					head_base[i] = head_base[i + 1] = '\0';
					char * filename = strtok(head_base + i + 2, " ");
					close(1);
					open(filename, O_WRONLY|O_CREAT, 0644);
				}
				else
				{
					head_base[i] = '\0';
					char * filename = strtok(head_base + i + 1, " ");
					close(1);
					open(filename, O_APPEND);
				}
			}
			else if(head_base[i] == '<')
			{
				close(0);
				head_base[i] = '\0';
				char * filename = strtok(head_base + i + 1, " ");
				open(filename, O_RDONLY);
			}
	}
	head[0] = strtok(commands[0], " ");
	for(int i = 1; head[i] = strtok(NULL, " "); i++);
	
	char ** tail = commands + 1;
	//handlers for "cd" and "exit"
	if(strcmp(head[0], "cd") == 0)
	{
		cd(head);
	}
	else if(strcmp(head[0], "exit") == 0)
	{
		exit(1);
	}
	//base case. just run head
	else if(tail[0] == NULL)
	{
		printf("%s %s", head[0], head[1]);
		execve(head[0], head, env);
	}
	else
	{
		//make a pipe between head and tail
		int pd[2];
		pipe(pd);
		int pid = fork();
		if(pid < 0)
		{
			perror("Could not fork child. Exiting.");
			exit(1);
		}
		else if(pid)
		{
			close(pd[0]);
			close(1);
			dup(pd[1]);
			close(pd[1]);
			execve(head[0], head, env);
		}
		else
		{
			close(pd[1]);
			close(0);
			dup(pd[0]);
			close(pd[0]);
			runCommandsHelper(tail, env);
		}
	}
}

void runCommands(char *commands[], char * env[])
{
	int pid = fork();
	if(pid < 0)
	{
		perror("Could not fork child. Exiting\n");
		exit(1);
	}
	else if(pid)
	{
		int status;
		pid = wait(&status);
	}
	else
	{
		runCommandsHelper(commands, env);
	}
}

int main(int argc, char * argv[], char * env[])
{
	char input[1000];
	char *piped_commands[50] = {NULL};
	printf("Welcome to RyanOS!\n\n");
	while(1)
	{
		printf("RyanOS>");
		fgets(input, 1000, stdin);
		input[strlen(input) - 1] = '\0';
		//split into piped_commands terminated by NULL
		piped_commands[0] = strtok(input, "|");
		for(int i = 1; piped_commands[i] = strtok(NULL, "|"); i++);
		runCommands(piped_commands, env);
	}
}

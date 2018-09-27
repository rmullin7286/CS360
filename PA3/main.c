#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

void searchPath(char * source, char * destination)
{
	char * path = getenv("PATH");
	char * check = strtok(path, ":");
	while(check)
	{
		strcpy(destination, check);
		strcat(destination, "/");
		strcat(destination, source);
		if(access(destination, F_OK) != -1)
			return;
		check = strtok(NULL, ":");
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
					open(filename, O_APPEND);
				}
				else
				{
					head_base[i] = '\0';
					char * filename = strtok(head_base + i + 1, " ");
					close(1);
					open(filename, O_WRONLY|O_CREAT, 0644);
						
				}
			}
			else if(head_base[i] == '<')
			{
				head_base[i] = '\0';
				char * filename = strtok(head_base + i + 1, " ");
				close(0);
				open(filename, O_RDONLY);
			}
	}
	head[0] = strtok(commands[0], " ");
	for(int i = 1; head[i] = strtok(NULL, " "); i++);
	
	char ** tail = commands + 1;
	//base case. just run head
	if(tail[0] == NULL)
	{
		char fullpath[100];
		searchPath(head[0], fullpath); 
		execve(fullpath, head, env);
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
			char fullpath[100];
			searchPath(head[0], fullpath);
			execve(fullpath, head, env);
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
	//handlers here for cd and exit
	char copy[100];
	strcpy(copy, commands[0]);
	char * tok = strtok(copy, " ");
	if(strcmp(tok, "cd") == 0)
	{
		tok = strtok(NULL, " <>");
		if(!tok || strcmp(tok, "") == 0)
			tok = getenv("HOME");
		chdir(tok);
	}
	else if(strcmp(tok, "exit") == 0)
	{
		exit(1);
	}
	else
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
			printf("End run commands helper");
		}
	}
}

int main(int argc, char * argv[], char * env[])
{
	char input[1000];
	char *piped_commands[50] = {NULL};
	system("clear");
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

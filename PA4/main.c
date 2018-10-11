#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <unistd.h>

#include <sys/types.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Usage: myrcp source destination\n");
		return 1;
	}
	return myrcp(argv[1], argv[2]);
}

int myrcp(char *f1, char *f2)
{
	struct stat s1, s2;
	int f2_exists = lstat(f2, &s2);
	if(lstat(f1, &s1) < 0)
	{
		printf("file %s does not exist!\n");
		return 1;
	}
	if(S_ISREG(s1.st_mode) || S_ISLNK(s1.st_mode))
	{
		if(f2_exists < 0 || S_ISREG(s2.st_mode) || S_ISLNK(s2.st_mode))
			return cpf2f(f1, f2);
		else
			return cpf2d(f1, f2);
	}
	else if(S_ISDIR(s1.st_mode))
	{
		if(s1.st_ino == s2.st_ino)
		{
			printf("Cannot copy a directory to itself.\n");
			return 1;
		}

		if(f2_exists >= 0 && !S_ISDIR(s2.st_mode))
		{
			printf("Cannot copy a directory to a non directory file\n");
			return 1;
		}
		
		else if(f2_exists < 0)
		{
			mkdir(f2, s1.st_mode);
			return cpd2d(f1, f2);
		}
		else
		{
			char fullpath1[4096], fullpath2[4096];
			realpath(f1, fullpath1);
			realpath(f2, fullpath2);
			if(strncmp(fullpath1, fullpath2, strlen(fullpath1)) == 0)
			{
				printf("Cannot copy to a subdirectory of source.\n");
				return 1;
			}
			return cpd2d(f1, f2);
		}
	}
}

int cpf2f(char* f1, char* f2)
{
	struct stat s1, s2;
	lstat(f1, &s1);
	int f2_exists = lstat(f2, &s2);
	char buffer[4096];
	if(s1.st_ino == s2.st_ino)
	{
		printf("These files are the same!\n");
		return 1;
	}
	else if(S_ISLNK(s1.st_mode) && f2_exists >= 0)
	{
		printf("Cannot copy a link to another existing file!\n");
		return 1;
	}
	else if(S_ISLNK(s1.st_mode))
	{
		buffer[readlink(f1, buffer, 4096)] = '\0';
		symlink(buffer, f2);
		return 0;
	}
	int fd1 = open(f1, O_RDONLY);
	int fd2 = open(f2, O_WRONLY|O_CREAT|O_TRUNC, s1.st_mode);
	int len;
	while(len = read(fd1, buffer, 4096))
		write(fd2, buffer, len);
	return 0;
}

int cpf2d(char* f1, char* f2)
{
	char * base = basename(f1);
	char destination[4096];
	strcpy(destination, f2);
	strcat(destination, "/");
	strcat(destination, base);
	struct stat s2;
	int exists = stat(destination, &s2);
	if(exists < 0)
		cpf2f(f1, destination);
	else
		if(S_ISDIR(s2.st_mode))
			cpf2d(f1, destination);
		else
			cpf2f(f1, destination);
}

int cpd2d(char* f1, char* f2)
{
	struct dirent* entry;
	DIR* directory = opendir(f1);

	while(entry = readdir(directory))
	{
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		char fullpath1[4096];
		char fullpath2[4096];
		strcat(strcat(strcpy(fullpath1, f1), "/"), entry->d_name);
		strcat(strcat(strcpy(fullpath2, f2), "/"), entry->d_name);
		
		struct stat s;
		stat(fullpath1, &s);
		if(S_ISDIR(s.st_mode))
		{
			mkdir(fullpath2, s.st_mode);
			cpd2d(fullpath1, fullpath2);
		}
		else
		{
			cpf2f(fullpath1, fullpath2);
		}
	}

	return 0;	
}

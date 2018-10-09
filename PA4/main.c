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
	int f2_exists = stat(f2, &s2);
	if(stat(f1, &s1) < 0)
	{
		printf("file %s does not exist!\n");
		return 1;
	}
	if(S_ISREG(s1.st_mode))
	{
		if(f2_exists < 0 || S_ISREG(s2.st_mode))
			return cpf2f(f1, f2);
		else
			return 1;//cpf2d(f1, f2)
	}
	else if(S_ISDIR(s1.st_mode))
	{
		if(f2_exists >= 0 && !S_ISDIR(s2.st_mode))
		{
			printf("Cannot copy a directory to a non directory file\n");
			return 1;
		}
		else if(f2_exists < 0)
		{
			mkdir(f2, s1.st_mode);
		}
		return 1;//cpd2d(f1, f2)
	}
}

int cpf2f(char* f1, char* f2)
{
	struct stat s1, s2;
	stat(f1, &s1);
	int f2_exists = stat(f2, &s2);
	if(s1.st_ino == s2.st_ino)
	{
		printf("These files are the same!\n");
		return 1;
	}
	else if(S_ISLNK(s1) && f2_exists >= 0)
	{
		printf("Cannot copy a link to another existing file!\n");
		return 1;
	}
	int fd1 = open(f1, O_RDONLY);
	int fd2 = open(f2, O_WRONLY|O_CREAT|O_TRUNC, s1.st_mode);
	char buffer[4096]
	int len;
	while(len = read(fd1, buffer, 4096))
		write(fd2, buffer, len);
	return 1;
}
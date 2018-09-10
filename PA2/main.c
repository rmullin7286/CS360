#include "filetree.h"
#include <stdio.h>
#include <string.h>

char line[128];
char command[16], pathname[64];
char dname[64], bname[64];

int getCmd(char * cmd)
{
    if(strcmp(cmd, "mkdir") == 0)
        return 0;
    if(strcmp(cmd, "rmdir") == 0)
        return 1;
    if(strcmp(cmd, "ls") == 0)
        return 2;
    if(strcmp(cmd, "cd") == 0)
        return 3;
}

int main(void)
{
    FileTree ft = initFileTree();
    while(1)
    {
        fgets(line, 128, stdin);
        line[strlen(line) - 1] = 0;
        sscanf(line, "%s %s", command, pathname);
        switch(getCmd(command))
        {
            case 0: mkdir(&ft, pathname);
                break;
            case 1: rmdir(&ft, pathname);
                break;
            case 2: ls(&ft, pathname);
                break;
            case 3: cd(&ft, pathname);
                break;
            case 4: cd(&ft, pathname);
        }
    }
}
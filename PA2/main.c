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
    if(strcmp(cmd, "pwd") == 0)
        return 4;
    if(strcmp(cmd, "creat") == 0)
        return 5;
    if(strcmp(cmd, "rm") == 0)
        return 6;
    if(strcmp(cmd, "save") == 0)
        return 7;
    if(strcmp(cmd, "reload") == 0)
        return 8;
    if(strcmp(cmd, "menu") == 0)
        return 9;
    if(strcmp(cmd, "quit") == 0)
        return 10;
    return -1;
}

void menu(void)
{
    printf("mkdir pathname\nrmdir pathname\ncd [pathname]\nls [pathname]\n");
    printf("pwd\ncreat pathname\nrm pathname\nsave\nreload\nmenu\nquit\n");
}

void quit(FileTree * ft)
{
    save(ft);
    closeFileTree(ft);
}

int main(void)
{
    FileTree ft = initFileTree();
    while(1)
    {
        command[0] = pathname[0] = '\0';
        printf("RyanOS> ");
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
            case 4: pwd(&ft);
                break;
            case 5: creat(&ft, pathname);
                break;
            case 6: rm(&ft, pathname);
                break;
            case 7: save(&ft);
                break;
            case 8: reload(&ft);
                break;
            case 9: menu();
                break;
            case 10: quit(&ft);
                return 0;
            default: break;
        }
    }
}


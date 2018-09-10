#include "filetree.h"
#include <stdio.h>
#include <string.h>

char line[128];
char command[16], pathname[64];
char dname[64], bname[64];

int main(void)
{
    FileTree ft = initFileTree();
    while(1)
    {
        fgets(line, 128, stdin);
        line[strlen(line) - 1] = 0;
        sscanf(line, "%s %s", command, pathname);
        s
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

#define BLKSIZE 1024

typedef struct ext2_group_desc GD;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;

int inode_table_start;

INODE root;
INODE cwd;
int dev;

void get_block(int fd, int blk, char buf[ ])
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  read(fd, buf, BLKSIZE);
}

INODE get_inode(int fd, int num)
{
    INODE newInode;
    char buf[BLKSIZE];
    int blk = (num - 1) / 8 + inode_table_start;
    int offset = (num - 1) % 8;
    get_block(fd, blk, buf);
    memcpy(&newInode, (INODE*)buf + offset, sizeof(INODE));
    return newInode;
}

int search(INODE ip, const char * name)
{
    char dbuf[BLKSIZE], temp[256];
    DIR * dp;
    char * cp;
    for(int i = 0; i < 12; i++)
    {
        if(ip.i_block[i] == 0)
            break;
        get_block(dev, ip.i_block[i], dbuf);
        cp = dbuf;
        dp = (DIR*)dbuf;

        while(cp < dbuf + BLKSIZE)
        {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = '\0';
            if(strcmp(name, temp) == 0)
            {
                if(dp->file_type != EXT2_FT_DIR)
                {
                    printf("%s is a file, not a dir\n", temp);
                    return 0;
                }
                return dp->inode;
            }
            cp += dp->rec_len;
            dp = (DIR*)cp;
        }
    }

    printf("Could not find %s", name);
    return 0;
}

int cd(char *path)
{
    INODE cur;
    if(path[0] == '/')
    {
        cur = root;
        path++;
    }
    else
    {
        cur = cwd;
    }
    char * tok = strtok(path, "/");
    do
    {
        int num = search(cur, tok);
        if(num == 0)
            return 0;
        cur = get_inode(dev, num);
        
    }while(tok = strtok(NULL, "/"));
    
    cwd = cur;

    return 1;
}

void ls(char * path)
{
    INODE old_cwd = cwd;
    if(path)
        if(!cd(path))
            return;

    char dbuf[BLKSIZE];

    for(int i = 0; i < 12; i++)
    {
        if(cwd.i_block[i] == 0)
        {
            break;
        }

        get_block(dev, cwd.i_block[i], dbuf);

        char * cp = dbuf;
        DIR * dp = (DIR*)dbuf;

        while(cp - dbuf < BLKSIZE)
        {
            char name[256];
            strncpy(name, dp->name, dp->name_len);
            name[dp->name_len] = '\0';
            printf("%s\n", name);

            cp += dp->rec_len;
            dp = (DIR*)cp;
        }
    }

    cwd = old_cwd;
}

void pwd_helper(INODE cur)
{
    if(cur.i_block[0] == root.i_block[0])
    {
        return;
    }

    char dbuf[1024];
    get_block(dev, cur.i_block[0], dbuf);

    char * cp = dbuf;
    DIR * dp = (DIR*)dbuf;

    INODE parent;

    while(cp - dbuf < BLKSIZE)
    {
        if(dp->name[0] == '.' && dp->name[1] == '.')
        {
            parent = get_inode(dev, dp->inode);
            break;
        }
        cp += dp->rec_len;
        dp = (DIR*)cp;
    }

    for(int i = 0; i < 12; i++)
    {
        get_block(dev, parent.i_block[i],  dbuf);

        char * cp = dbuf;
        DIR * dp = (DIR*)dbuf;

        while(cp - dbuf < BLKSIZE)
        {
            INODE possible = get_inode(dev, dp->inode);
            if(possible.i_block[0] == cur.i_block[0])
            {
                pwd_helper(parent);
                char name[256];
                strncpy(name, dp->name, dp->name_len);
                name[dp->name_len] = '\0';
                printf("/%s", name);
                return;
            }

            cp += dp->rec_len;
            dp = (DIR*)cp;
        }
    }
}

void pwd(void)
{
    if(root.i_block[0] == cwd.i_block[0])
        printf("/");
    else
        pwd_helper(cwd);
    printf("\n");
}




int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        printf("usage: PA7 <diskname>\n");
        return 0;
    }
    if((dev = open(argv[1], O_RDONLY)) < 0)
    {
        printf("Could not open disk\n");
        return 1;
    }
    char buf[BLKSIZE];
    get_block(dev, 2, buf);
    inode_table_start = ((GD*)buf)->bg_inode_table;
    get_block(dev, inode_table_start, buf);
    cwd = root = get_inode(dev, 2);

    char input[256];
    printf("Welcome to RyanOS: EXT2 Edition!\n\n");
    while(1)
    {
        printf("RyanOS> ");
        fgets(input, 256, stdin);

        char * cmd = strtok(input, " \n"), * path = strtok(NULL, "\n ");
        if(strcmp(cmd, "cd") == 0)
        {
            cd(path);
        }
        else if(strcmp(cmd, "ls") == 0)
        {
            ls(path);
        }
        else if(strcmp(cmd, "pwd") == 0)
        {
            pwd();
        }
        else if(strcmp(cmd, "quit") == 0)
        {
            break;
        }
        else
        {
            printf("Command invalid\n");
        }
    }
}
#include <stdio.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <unistd.h>
#include <string.h>

#define BLKSIZE 1024

typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

int get_block(int fd, int blk, char buf[ ])
{
  lseek(fd,(long)blk*BLKSIZE, 0);
   read(fd, buf, BLKSIZE);
}

unsigned int search(INODE* ip, char * to_find, int fd)
{
    char buf[1024];
    unsigned int block0 = ip->i_block[0];
    get_block(fd, block0, buf);
    char* cp = buf;
    DIR* dp = (DIR*)buf;
    while(cp - buf < 1024)
    {
        char name[EXT2_NAME_LEN];
        strncpy(name, dp->name, dp->name_len);
        name[dp->name_len] = '\0';
        if(strcmp(name, to_find) == 0)
            return dp->inode;
        cp += dp->rec_len;
        dp = (DIR*)cp;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Usage: dir [disk] or\ndir [disk] [pathToSearch]\n");
        return 1;
    }

    char buf[BLKSIZE];
    int fd = open(argv[1], O_RDONLY);
    get_block(fd, 2, buf);
    GD* gp = (GD*)buf;
    get_block(fd, gp->bg_inode_table, buf);
    INODE* ip = (INODE*) buf + 1;
    
    if(argc == 2)
    {
        //print all entries under / directory
        unsigned int block0 = ip->i_block[0];
        get_block(fd, block0, buf);
        char* cp = buf;
        DIR* dp = (DIR*)buf;
        while(cp - buf < 1024)
        {
            char name[EXT2_NAME_LEN];
            strncpy(name, dp->name, dp->name_len);
            name[dp->name_len] = '\0';
            printf("%s\n", name);
            cp += dp->rec_len;
            dp = (DIR*)cp;         
        }
        return 0;
    }
    
    else if(argc == 3)
    {
        //as an example, search / for lost+found
        int ret = search(ip, argv[2], fd);
        if(ret == 0)
            printf("%s not found\n", argv[2]);
        else
            printf("inode number = %d\n", ret);
    }
    return 0;
}
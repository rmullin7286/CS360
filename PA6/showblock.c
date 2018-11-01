#include <stdio.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#define SUPERBLOCK_MAGIC_NUMBER 0xEF53
#define BLKSIZE 1024
#define SUPERBLK 1
#define GDBLK 2
#define FILE_NAME_MAX_SIZE 256

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc GD;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;

void get_block(int fd, long blk, char buf[])
{
    lseek(fd, blk*BLKSIZE, 0);
    read(fd, buf, BLKSIZE);
}

void print_indirect_blocks(char dbuf[])
{
    int line = 0;
    for(char* cur = dbuf; cur < dbuf + BLKSIZE; cur += 4)
    {
        uint32_t val = *((uint32_t*)cur);
        if(val == 0)
        {
            printf("\n");
            return;
        }
        printf("%d ", val);
        if(++line == 8)
        {
            printf("\n");
            line = 0;
        }
    }
    printf("\n");
}

void print_double_indirect_blocks(int fd, char dbuf[])
{
    for(char* cur = dbuf; cur < dbuf + BLKSIZE; cur += 4)
    {
        uint32_t val = *((uint32_t*)cur);
        if(val == 0)
        {
            printf("\n");
            return;
        }
        char dbuf2[BLKSIZE];
        get_block(fd, val, dbuf2);
        print_indirect_blocks(dbuf2);
    }
    printf("\n");
}

void print_blocks(int fd, const INODE *ip)
{
    for(int i = 0; i < 12; i++)
    {
        if(ip->i_block[i] == 0)
            return;
        printf("i_block[%d] = %d\n", i, ip->i_block[i]);
    }

    char dbuf[BLKSIZE];

    if(ip->i_block[12] != 0)
    {
        get_block(fd, ip->i_block[12], dbuf);
        printf("----------indirect blocks----------\n");
        print_indirect_blocks(dbuf);
    }

    if(ip->i_block[13] != 0)
    {
        get_block(fd, ip->i_block[13], dbuf);
        printf("----------double indirect blocks--------\n");
        print_double_indirect_blocks(fd, dbuf);
    }
}

int search(int dev, const INODE *ip, const char * name)
{
    printf("i_number    rec_len    name_len    name\n");
    char dbuf[BLKSIZE], temp[256];
    DIR* dp;
    char* cp;
    for(int i = 0; i < 12; i++)
    {
        if(ip->i_block[i] == 0)
            break;
        get_block(dev, ip->i_block[i], dbuf);
        cp = dbuf;
        dp = (DIR*)dbuf;


        while(cp < dbuf + BLKSIZE)
        {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = '\0';
            printf("%4d   %4d   %4d   %s\n",
                dp->inode, dp->rec_len, dp->name_len, temp);
            if(strcmp(name, temp) == 0)
                return dp->inode;
            cp += dp->rec_len;
            dp = (DIR*)cp;
        }
    }
    
    return 0;
}

int main(int argc, char* argv[])
{
    //check args
    if(argc != 3)
    {
        printf("Usage: showblock <device> <filepath>\n");
        return 0;
    }

    //get device
    int dev = open(argv[1], O_RDONLY);
    if(dev < 0)
    {
        printf("Failed to open device %s\n", argv[1]);
        return 1;
    }

    //read superblock
    printf("Checking EXT2 FS... ");
    char buf[BLKSIZE];
    get_block(dev, SUPERBLK, buf);
    if(((SUPER*)buf)->s_magic != SUPERBLOCK_MAGIC_NUMBER)
    {
        printf("ERROR: Incorrect filesystem type.\n");
        return 1;
    }
    printf("OK\n");

    //get inode table
    printf("Retrieving inode table...");
    get_block(dev, GDBLK, buf);
    int iblk = ((GD*)buf)->bg_inode_table;
    get_block(dev, iblk, buf);
    printf("DONE\n\n");

    //retrieve root dir
    INODE* ip = (INODE*)buf + 1;
    
    //search for inode
    char current_dir[FILE_NAME_MAX_SIZE] = "/";
    char * path = argv[2];
    if(path[0] == '/')
        path++;
    char * tok = strtok(path, "/");
    int num;
    do
    {
        printf("=========================\n");
        printf("Searching %s for %s... \n", current_dir, tok);
        num = search(dev, ip, tok);
        if(num == 0)
        {
            printf("NOT FOUND\n");
            return 0;
        }
        printf("FOUND\n\n");
        strcpy(current_dir, tok);
        
        int blk = (num - 1) / 8 + iblk;
        int offset = (num - 1) % 8;
        get_block(dev, blk, buf);
        ip = (INODE*)buf + offset;
    }while(tok = strtok(NULL, "/"));

    printf("==========================\n");
    printf("Found %s: ino = %d\n", current_dir, num);
    print_blocks(dev, ip);
    return 0;
}
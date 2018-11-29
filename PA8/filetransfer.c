#include "filetransfer.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>

#define MAX 256

int sendfile(int socket, char * pathname)
{
    char buffer[MAX];

    //first, load the size of the file into the first 4 bytes to be sent
    struct stat st;
    stat(pathname, &st);
    uint32_t size = st.st_size;
    memcpy(buffer, &size, 4);

    //read first 252 bytes or rest of file into the rest of the buffer
    FILE* f = fopen(pathname, "rb");
    if(!f)
        return -1;
    int i = st.st_size; 
    fread(buffer + 4, 1, MAX - 4, f);
    i -= (MAX - 4);

    //write first buffer to receiver
    write(socket, buffer, MAX);

    for(; i > 0; i -= MAX)
    {
        fread(buffer, 1, MAX, f);
        write(socket, buffer, MAX);
    }

    fclose(f);

    return size;
}

int receivefile(int socket, char * pathname)
{
    char buffer[MAX];
    FILE *f = fopen(basename(pathname), "wb");

    //read the first chunk and find the size
    read(socket, buffer, MAX);
    uint32_t size;
    memcpy(&size, buffer, 4);
    int i = size;

    fwrite(buffer + 4, 1, MAX - 4 > i ? i : MAX - 4, f);

    i -= (MAX - 4);

    for(; i > 0; i -= MAX)
    {
        read(socket, buffer, MAX);
        fwrite(buffer, 1, MAX > i ? i : MAX, f);
    }

    fclose(f);

    return size;
    
}
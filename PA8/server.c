// This is the echo SERVER server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>

#include "filetransfer.h"

#define  MAX 256

// Define variables:
struct sockaddr_in  server_addr, client_addr, name_addr;
struct hostent *hp;

int  mysock, client_sock;              // socket descriptors
int  serverPort;                     // server port number
int  r, length, n;                   // help variables

char pathname[246], command[10];

int send_file(int socket, char * pathname)
{

}

void ls_file(char * out, char * pathname)
{
  const char * t1 = "xwrxwrxwr-------";
  const char * t2 = "----------------";
  struct stat fstat;
  char ftime[64];
  if((r = lstat(pathname, &fstat)) < 0)
  {
    strcpy(out, "ls failed");
  }
  else
  {
    if(S_ISREG(fstat.st_mode))
      out[0] = '-';
    else if(S_ISDIR(fstat.st_mode))
      out[0] = 'd';
    else if(S_ISLNK(fstat.st_mode))
      out[0] = 'l';
    for(int i = 8; i >= 0; i--)
      if((fstat.st_mode) & 1 << i)
        out[i + 1] = t1[i];
      else
        out[9 - i] = t2[i];
  }

  char buffer[200];
  sprintf(buffer, " %4lu %4u %4u %4ld %s %s", fstat.st_nlink, fstat.st_gid, fstat.st_uid, fstat.st_size,
      ctime(&(fstat.st_atime)), basename(pathname));
  strcat(out, buffer);
  if(S_ISLNK(fstat.st_mode))
  {
    char buffer2[100];
    readlink(pathname, buffer2, 200);
    sprintf(buffer, " -> %s", buffer2);
    strcat(out, buffer);
  }
}

// Server initialization code:

int server_init(char *name)
{
   printf("==================== server init ======================\n");   
   // get DOT name and IP address of this host

   printf("1 : get and show server host info\n");
   hp = gethostbyname(name);
   if (hp == 0){
      printf("unknown host\n");
      exit(1);
   }
   printf("    hostname=%s  IP=%s\n",
               hp->h_name,  inet_ntoa(*((struct in_addr*)hp->h_addr_list[0])));
  
   //  create a TCP socket by socket() syscall
   printf("2 : create a socket\n");
   mysock = socket(AF_INET, SOCK_STREAM, 0);
   if (mysock < 0){
      printf("socket call failed\n");
      exit(2);
   }

   printf("3 : fill server_addr with host IP and PORT# info\n");
   // initialize the server_addr structure
   server_addr.sin_family = AF_INET;                  // for TCP/IP
   server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // THIS HOST IP address  
   server_addr.sin_port = 0;   // let kernel assign port

   printf("4 : bind socket to host info\n");
   // bind syscall: bind the socket to server_addr info
   r = bind(mysock,(struct sockaddr *)&server_addr, sizeof(server_addr));
   if (r < 0){
       printf("bind failed\n");
       exit(3);
   }

   printf("5 : find out Kernel assigned PORT# and show it\n");
   // find out socket port number (assigned by kernel)
   length = sizeof(name_addr);
   r = getsockname(mysock, (struct sockaddr *)&name_addr, &length);
   if (r < 0){
      printf("get socketname error\n");
      exit(4);
   }

   // show port number
   serverPort = ntohs(name_addr.sin_port);   // convert to host ushort
   printf("    Port=%d\n", serverPort);

   // listen at port with a max. queue of 5 (waiting clients) 
   printf("5 : server is listening ....\n");
   listen(mysock, 5);
   printf("===================== init done =======================\n");
}


int main(int argc, char *argv[])
{
   char *hostname;
   char line[MAX];

   if (argc < 2)
      hostname = "localhost";
   else
      hostname = argv[1];
 
   server_init(hostname); 

   // Try to accept a client request
   while(1){
     printf("server: accepting new connection ....\n"); 

     // Try to accept a client connection as descriptor newsock
     length = sizeof(client_addr);
     client_sock = accept(mysock, (struct sockaddr *)&client_addr, &length);
     if (client_sock < 0){
        printf("server: accept error\n");
        exit(1);
     }
     printf("server: accepted a client connection from\n");
     printf("-----------------------------------------------\n");
     printf("        IP=%s  port=%d\n", inet_ntoa(client_addr.sin_addr),
                                        ntohs(client_addr.sin_port));
     printf("-----------------------------------------------\n");

     // Processing loop: newsock <----> client
     while(1){
       n = read(client_sock, line, MAX);
       if (n==0){
           printf("server: client died, server loops\n");
           close(client_sock);
           break;
        }
      
      // show the line string
      printf("server: read  n=%d bytes; line=[%s]\n", n, line);

      sscanf(line, "%s %s", command, pathname);
      printf("%s", command);
      printf("%s", pathname);

      int skip = 0;

      if(strcmp(command, "mkdir") == 0)
        strcpy(line, (mkdir(pathname, 0755) == 0 ? "mkdir success" : "mkdir failure"));
      else if(strcmp(command, "rmdir") == 0)
        strcpy(line, (rmdir(pathname) == 0 ? "rmdir success" : "rmdir failure"));
      else if(strcmp(command, "rm") == 0)
        strcpy(line, (unlink(pathname) == 0 ? "rm success" : "rm failure"));
      else if(strcmp(command, "cd") == 0)
        strcpy(line, (chdir(pathname) == 0 ? "cd success" : "cd failure"));
      else if(strcmp(command, "pwd") == 0)
        getcwd(line, MAX);

      else if(strcmp(command, "ls") == 0)
      {
        //skip the final write out
        skip = 1;

        //TODO: implement rest of ls code here
      }    

      else if(strcmp(command, "get") == 0)
      {
        skip = 1;
        int i = sendfile(client_sock, pathname);
        if(i == -1)
        {
          printf("failed to send file\n");
        }
        else
        {
          printf("%d bytes sent\n", i);
        }
      }

      else if(strcmp(command, "put") == 0)
      {
        skip = 1;
        int i = receivefile(client_sock, pathname);
        if( i == -1)
          printf("failed to receive file\n");
        else
          printf("%d bytes read\n", i);
      }
        
      // send the echo line to client 
      if(!skip)
      {
        n = write(client_sock, line, MAX);

        printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
        printf("server: ready for next request\n");
      }
      else
        skip = 0;
    }
 }
}


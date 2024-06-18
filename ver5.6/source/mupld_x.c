/*
 * MTERM file UP-loader
 *
 *  hiro (miyasaka@caltech.edu)
 *  alpha version       v0.0.1   08/24/08
 *  beta                v0.1.0   01/26/09
 * 
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mterm.h"

int main(void)
{
   int  i,wend=1;
   int  fildes[2];
   int  res,pipe_fd,open_mode=O_WRONLY;
   char cmd0[BUFSIZ+1],cmd1[BUFSIZ+1],cmd2[BUFSIZ+1],buffer[BUFSIZ+1];
   pid_t pid;

   memset(cmd0,'\0',sizeof(cmd0));
   memset(cmd1,'\0',sizeof(cmd1));
   memset(cmd2,'\0',sizeof(cmd2));
   pipe_fd=open(FIFO_NAME,O_WRONLY| O_NONBLOCK);

   strcpy(cmd1,":u");
   write(pipe_fd,cmd1,strlen(cmd1)+1);
//   strcpy(cmd1,"READ");
//   write(pipe_fd,cmd1,strlen(cmd1)+1);
//   usleep(5000);

   while(fgets(cmd0,sizeof(cmd0),stdin) != NULL) {
           strncpy(cmd1,cmd0,strlen(cmd0)-2);
//           this works perfect for regular rs232 testing
//           printf("%s\0",cmd0);
           write(pipe_fd,cmd1,strlen(cmd1)+1);
           usleep(5000);
           strncpy(cmd1,cmd2,BUFSIZ+1);
   }

//   strcpy(cmd1,"\r");
//   write(pipe_fd,cmd1,strlen(cmd1)+1);
//   strcpy(cmd1,"OK");
//   write(pipe_fd,cmd1,strlen(cmd1)+1);
//   strcpy(cmd1,"\r");
//   write(pipe_fd,cmd1,strlen(cmd1)+1);
   strcpy(cmd1,":c");
   write(pipe_fd,cmd1,strlen(cmd1)+1);

   close(pipe_fd);

   return 0;
}


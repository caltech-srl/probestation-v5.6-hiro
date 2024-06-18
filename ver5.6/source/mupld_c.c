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

#include "mterm_sub.c"

FILE *fp1;

int main(int argc, char **argv)
{
   int  i,wend=1,iflg_open=0;
   int  fildes[2];
   int  res,pipe_fd,open_mode=O_WRONLY;
   char cmd0[BUFSIZ+1],cmd1[BUFSIZ+1],cmd2[BUFSIZ+1],cmd3[BUFSIZ+1];
   char apause[BUFSIZ+1],filein[BUFSIZ+1];
   pid_t pid;

   fp1=stdin;

   memset(cmd0,'\0',sizeof(cmd0));
   memset(cmd1,'\0',sizeof(cmd1));
   memset(cmd2,'\0',sizeof(cmd2));
   memset(cmd3,'\0',sizeof(cmd3));

   for (i=0;i<argc;i++) {
       if (*argv[i] == '-') {
          if        (strncmp(&(argv[i][1]),"p",1)==0){
             strcpy(apause,argv[++i]); // pause for X second
	     sleep(atoi(apause));
	  } else if (strncmp(&(argv[i][1]),"P",1)==0){
             strcpy(apause,argv[++i]); // pause for X mili-second
	     usleep(atoi(apause)*1000);
	  } else if (strncmp(&(argv[i][1]),"I",1)==0){
             strcpy(filein,argv[++i]); // open input file
	     fp1=fopen(filein,"r");
	     iflg_open=1;
          }
       }
   }

   //pipe_fd=open(FIFO_NAME,O_WRONLY| O_NONBLOCK);
   pipe_fd=open(FIFO_NAME,open_mode);


//   while(fgets(cmd0,sizeof(cmd0),stdin) != NULL) {
   while(fgets(cmd0,sizeof(cmd0),fp1) != NULL) {
           strncpy(cmd1,cmd0,strlen(cmd0)-1);
           sprintf(cmd2,"%s\n\0",cmd1);
           write(pipe_fd,cmd2,strlen(cmd2)+1);
           usleep(WAIT1);
           tcflush(pipe_fd,TCIOFLUSH);
           //tcflush(pipe_fd,TCIFLUSH);
           //usleep(WAIT1);
           //tcflush(pipe_fd,TCOFLUSH);
           //usleep(WAIT1);
           //fflush(fp1);
	   strncpy(cmd1,cmd3,BUFSIZ+1);
	   cmdwait(cmd2);
   }

   close(pipe_fd);
   if (iflg_open) fclose(fp1);

   return 0;
}


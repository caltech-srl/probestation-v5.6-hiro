/*
 * MTERM Commander
 *
 *  hiro (miyasaka@caltech.edu)
 *  alpha version       v0.0.1   08/20/08
 *  beta		v1.0.0   10/05/09
 *
 *  Send commands to MISC through FIFO.
 *  use raedline to save commands history.
 * 
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#include "mterm.h"

char *readline(const char *prompt);
void using_history(void);
void add_history(const char *string);
int read_history(const char *filename);
int write_history(const char *filename);

int main(int argc, char **argv)
{
   int  i,wend=1,pend=1,iflg_smode=0;
   int  fildes[2];
   //int  res,pipe_fd,open_mode=O_WRONLY|O_NONBLOCK;
   int  res,pipe_fd,open_mode=O_WRONLY;
   char *cmd0,cmd1[BUFSIZ+1],buffer[BUFSIZ+1];
   char cmdx[BUFSIZ+1],s1[10],s2[10];
   time_t t;
   struct tm *ptime;

   pid_t pid;

   s1[0]=27; s1[1]=91; s1[2]=65; s1[3]='\r';
   //s1[0]=27; s1[1]=91; s1[2]=65; s1[5]='\0';
   //s2[0]=27; s2[1]=91; s2[2]=64; s2[5]='\0';


   for (i=0;i<argc;i++) {
       if (*argv[i] == '-') {
          if        (strncmp(&(argv[i][1]),"s",1)==0){
             iflg_smode=1;  // single mode
          }
       }
   }

   memset(buffer,'\0',sizeof(buffer));
   if (access(FIFO_NAME, F_OK) == -1) {
      fprintf(stderr, "make FIFO  %s\n",FIFO_NAME);
      res = mkfifo(FIFO_NAME, 0777);
      if (res != 0) {
         fprintf(stderr, "Could not create fifo %s\n",FIFO_NAME);
         exit(EXIT_FAILURE);
      }
   }

   pipe_fd=open(FIFO_NAME,open_mode);
   //fprintf(stderr, "FIFO start %s\n",FIFO_NAME);

   using_history();
   read_history(".my_history");
   while(wend) {
        t=time(NULL);
        ptime=localtime(&t);
        if (iflg_smode==1) {
           sprintf(cmdx,"%02d:%02d:%02d MDA> ",
               ptime->tm_hour,ptime->tm_min,ptime->tm_sec);
	} else {
           sprintf(cmdx,"%02d:%02d:%02d > ",
               ptime->tm_hour,ptime->tm_min,ptime->tm_sec);
	}
    	cmd0 = readline(cmdx);
    	add_history(cmd0);
	if (strncmp(cmd0,":q",2)==0) {
           wend=0;
           pend=0;
        } else if (strncmp(cmd0,":Q",2)==0) {
           if (iflg_smode==1) sprintf(cmd0,":q");
	   wend=0;
           pend=0;
        }
	sprintf(cmd1,"%s\n\0",cmd0);
        write(pipe_fd,cmd1,strlen(cmd1)+1);
 	free(cmd0);
        if (iflg_smode==1) fprintf(stdout,"%s",&s1);
   }
   close(pipe_fd);
   write_history(".my_history");
   //fprintf(stderr, "FIFO end %s\n",FIFO_NAME);

   return 0;
}


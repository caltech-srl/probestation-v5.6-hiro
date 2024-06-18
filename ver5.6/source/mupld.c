/*
 * MISC file UP-loader
 *
 *  hiro (miyasaka@caltech.edu)
 *  alpha version       v0.0.1   08/24/08
 *  beta                v0.1.0   01/26/09
 *  v1.0		v1.0.0   10/05/09
 *
 *  Upload MISC md source codes.
 *  make sure mterm is not running.
 * 
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>

#include "mterm.h"

#include "mterm_sub.c"

int main(int argc, char **argv)
{
   int  i,j,wend=1,k,inflg=1;
   int  fildes[2];
   int  bufc=13;
   int  res,pipe_fd,open_mode=O_WRONLY;
   long ic=0l,iwait1=300000l;
   char misc_com[256];
   char cmd0[BUFSIZ+1],cmd1[BUFSIZ+1],cmd2[BUFSIZ+1],cmd3[BUFSIZ+1];
   char buf0[BUFSIZ+1],st1[BUFSIZ];
   struct termios old_misc_tio,new_misc_tio;
   struct termios old_stdtio,new_stdtio;
   ssize_t fsize;

   strcpy(misc_com,DEF_MISC_COM);

   memset(cmd0,'\0',sizeof(cmd0));
   memset(cmd1,'\0',sizeof(cmd1));
   memset(cmd2,'\0',sizeof(cmd2));
   memset(cmd3,'\0',sizeof(cmd3));

   for (i=0;i<argc;i++) {
       if (*argv[i] == '-') {
          if        (strncmp(&(argv[i][1]),"c",1)==0){
              strcpy(misc_com,argv[++i]); // MISC com port
	  } else if (strncmp(&(argv[i][1]),"n",1)==0){
              inflg=0;                    // do not send READ/OK command
	  } else if (strncmp(&(argv[i][1]),"w",1)==0){
              strcpy(st1,argv[++i]);      // wait MISC response before quit
              iwait1=atol(st1);           // dafault iwait1=300l
	  }
       }
   }

   // ------------ Serial device setting ------------------------------------
   //
   spMISC=open(misc_com, O_RDWR | O_NOCTTY | O_NONBLOCK);
   if (spMISC<0) {
	   fputs("open serial device failed\n",stderr);
   }
   tcgetattr(spMISC,&old_misc_tio);
   new_misc_tio.c_cflag=BAUDRATE2 | CRTSCTS | CS8 | CLOCAL | CREAD | CSTOPB;
   new_misc_tio.c_iflag=IGNPAR | ICRNL;
   new_misc_tio.c_oflag=0;
   new_misc_tio.c_lflag=ICANON;
   //new_misc_tio.c_lflag=0;
   new_misc_tio.c_cc[VMIN]=1;
   new_misc_tio.c_cc[VTIME]=0;
   tcflush(spMISC,TCIFLUSH);
   tcsetattr(spMISC,TCSANOW,&new_misc_tio);


   // ----------- Upload file ----------------------------------------------
   // before sending a file, needs to run "READ" command.
   if (inflg) {
      strcpy(cmd0,"READ");
      write(spMISC,cmd0,4);
      usleep(5000);
      write(spMISC,&bufc,1);
      usleep(5000);
   }

   /* working version
   while(read(0,&bufc,1) >0) {
      write(spMISC,&bufc,1);
   }
   */
   while((fsize=read(0,&bufc,sizeof(bufc))) >0) {
      write(spMISC,&bufc,fsize);
   }

   if (inflg) {
      bufc=26;
      write(spMISC,&bufc,1);
      usleep(5000);

   // after sending a file, "OK" commands will compile the program.
      strcpy(cmd1,"OK");
      write(spMISC,cmd1,2);
      usleep(5000);
   }
   bufc=13;
   write(spMISC,&bufc,1);

   // after compile, wait for a while to have any response from MISC.
   // ic (internal counter) < 200 is just empilical number. might to
   // adjust for diffent PC.
   while(ic<iwait1) {
      ic++;
      if (read(spMISC,&buf0,BUFSIZ)>0) {
         k=strlen(buf0)-2;
         strncpy(cmd1,buf0,k);
         //fprintf(stderr,"%s",buf0);
         //fflush(stderr);
         fprintf(stdout,"%s",buf0);
         fflush(stdout);
	 ic=0l;
      }
      memset(buf0,'\0',sizeof(buf0));
   }

   tcflush(spMISC,TCIOFLUSH);
   fflush(stdin);
   fflush(stdout);
   tcsetattr(spMISC,TCSANOW,&old_misc_tio);
   return 0;
}


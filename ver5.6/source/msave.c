/*****************************************************************
 MISC data saver

 hiro (miyasaka@caltech.edu)
 alpha version       v0.1.0   10/13/09

 *****************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <termios.h>

#include "mterm.h"

#include "mterm_sub.c"

//#define _POSIX_SOURCE 1
#define ENDTERM 32 // hit space to quite msave
#define FALSE 0
#define TRUE  1

FILE *fd1;
FILE   *fpMISC;
int     spMISC;

volatile int STOP=FALSE;
void child_handler(int s)
{
   STOP=TRUE;
}

int main(int argc, char **argv)
{
  int i, j, k, l=0,wend=1,dum1;
  int res,pipe_fd,islp=60;
  int upld_flg=0;
  int bufc;
  char save_fil[BUFSIZ];
  unsigned char buf0[256];
  time_t t;
  struct tm *ptime;
  ssize_t fsize;

  char misc_com[256], misc_fil[256],fifo_fil[256];
  struct termios old_misc_tio,new_misc_tio;
  struct sigaction saio;

  strcpy(misc_com,DEF_MISC_COM2);
  strcpy(save_fil,"test.bin");

  for (i=0;i<argc;i++) {
      if (*argv[i] == '-') {
	 if        (strncmp(&(argv[i][1]),"c",1)==0){ 
             strcpy(misc_com,argv[++i]); // MISC data com port
         } else if (strncmp(&(argv[i][1]),"s",1)==0){
             strcpy(save_fil,argv[++i]); // SAVE filename
         } else if (strncmp(&(argv[i][1]),"e",1)==0){
             islp=atoi(argv[++i]);      // terminate after x sec
      	 }
      }
  }

  // --------- Serial setting ------------------------------------
  // need to run TeraTERM first to set RS232 setting first.

  spMISC=open(misc_com, O_RDWR | O_NOCTTY );     /* open MISC device */
  //spMISC=open(misc_com, O_RDWR | O_NOCTTY | O_NONBLOCK);     /* open MISC device */
  if (spMISC<0) {
      fputs("open serial device failed\n",stderr);
  }


  tcgetattr(spMISC,&old_misc_tio);
  bzero(&new_misc_tio, sizeof(new_misc_tio));
  //new_misc_tio.c_iflag=BRKINT | IGNPAR;
  //
  new_misc_tio.c_cflag=BAUDRATE2 | CS8 | CLOCAL | CREAD | CSTOPB;
  new_misc_tio.c_iflag=IGNPAR;

  //new_misc_tio.c_cflag=BAUDRATE2 | CRTSCTS | CS8 | CLOCAL | CREAD | CSTOPB;
  //new_misc_tio.c_iflag=IGNPAR | ICRNL;
  new_misc_tio.c_oflag=0;
  new_misc_tio.c_lflag=ICANON;
  //new_misc_tio.c_lflag=0;
  new_misc_tio.c_cc[VMIN]=0;
  new_misc_tio.c_cc[VTIME]=0;
  tcflush(spMISC,TCIFLUSH);
  tcsetattr(spMISC,TCSANOW,&new_misc_tio);


  // --------- Main loop -----------------------------------------
  switch(fork())
  {
    case 0: // child
      if (islp>0) {
         sleep(islp);
      } else {
         while(1) {}
      }

      exit(0);
    case -1: //fail fork
      printf("fail fork !!!! \n");
      tcsetattr(spMISC,TCSANOW,&old_misc_tio);
      close(spMISC);
      exit(-1);
  }

  fd1=fopen(save_fil,"wb");
  // Main process
  // read MISC and save into stdout
  saio.sa_handler = child_handler;
  saio.sa_flags=0;
  sigaction(SIGCHLD,&saio, NULL);
  while(STOP==FALSE) {
  //while(1) {
     //read( spMISC, &bufc, 1);
     //write(1, &bufc, 1);

     fsize=read( spMISC,&buf0,sizeof(buf0));
     if (fsize>0) {
       fwrite(buf0,sizeof(unsigned char),fsize,fd1);
       //fwrite(buf0,sizeof(unsigned char),fsize,stdout);
     }
     //printf("%d %s\n",(bufc & 0xff),&bufc);
  }
  wait(NULL);

  tcflush(spMISC,TCIOFLUSH);
  fflush(fd1);
  t=time(NULL);
  ptime=localtime(&t);
  fprintf(stdout,"# msave end at %02d:%02d:%02d ",
         ptime->tm_hour,ptime->tm_min,ptime->tm_sec);

  tcsetattr(spMISC,TCSANOW,&old_misc_tio);
  close(spMISC);
  fclose(fd1);

  return 0;
}



/*****************************************************************
 MISC terminal

 hiro (miyasaka@caltech.edu)
 alpha version       v0.1.0   Import from dmm_gpib_4.c 8/17/08 
 beta                v1.0.0   now working 10/5/09

 MISC terminal server to send commands recieve response from MISC.
 run this mterm first as a server.
 to send commands to MISC, run mcmdr from diffent window.
 if user want to use script command files instead of type in from
 keyboard, use mupld_c.

 to send source code to MISC, use mupld instead of mterm.

 *****************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <time.h>

#include "mterm.h"

static int wend=1;

volatile int STOP=FALSE;
void signal_handler_IO(int status) {
    STOP=TRUE;
}

int main(int argc, char **argv)
{
  int i, j, k, l=0;
  int res,pipe_fd;
  int upld_flg=0;
  int itimflg;
  unsigned short bufc=0;
  int ibinflg=0,bincnt=0;
  char *bufx;
  pid_t  pid,pid_fifo;
  //int fsize;
  ssize_t fsize;

  char cmd0[BUFSIZ+1],cmd1[BUFSIZ+1], buf0[BUFSIZ+1],buf1[BUFSIZ+1];
  char cmd2[BUFSIZ+1],*st1;
  char misc_com[256], misc_fil[256],fifo_fil[256];
  char *str1;
  struct termios old_misc_tio,new_misc_tio;
  struct sigaction saio;
  time_t t;
  struct tm *ptime;

  strcpy(cmd2,"DONE");

  strcpy(misc_com,DEF_MISC_COM);
  strcpy(misc_fil,DEF_MISC_FIL);
  strcpy(fifo_fil,FIFO_NAME);

  memset(cmd0,'\0',sizeof(cmd0));
  memset(buf0,'\0',sizeof(buf0));

  for (i=0;i<argc;i++) {
      if (*argv[i] == '-') {
	 if        (strncmp(&(argv[i][1]),"c",1)==0){ 
             strcpy(misc_com,argv[++i]); // MISC com port
	 } else if (strncmp(&(argv[i][1]),"f",1)==0){ 
             strcpy(misc_fil,argv[++i]); // MISC intialize
	 } else if (strncmp(&(argv[i][1]),"p",1)==0){ 
             strcpy(fifo_fil,argv[++i]); // FIFO_NAME
      	 }
      }
  }

  // --------- Serial setting ------------------------------------
  // need to run TeraTERM first to set RS232 setting first.

  spMISC=open(misc_com, O_RDWR | O_NOCTTY | O_NONBLOCK);     /* open MISC device */
  //spMISC=open(misc_com, O_RDWR | O_NOCTTY );     /* open MISC device */
  if (spMISC<0) {
      fputs("open serial device failed\n",stderr);
  }


//  fcntl(spMISC, F_SETOWN, getpid());
  tcgetattr(spMISC,&old_misc_tio);
/*
  bzero(&new_misc_tio, sizeof(new_misc_tio));
  //new_misc_tio.c_iflag=BRKINT | IGNPAR;
 */
  // test 10/22/09
  new_misc_tio.c_cflag=BAUDRATE2 | CS8 | CLOCAL | CREAD | CSTOPB;
  new_misc_tio.c_iflag=IGNPAR;
  // end

  //new_misc_tio.c_cflag=BAUDRATE2 | CRTSCTS | CS8 | CLOCAL | CREAD | CSTOPB;
  //new_misc_tio.c_iflag=IGNPAR | ICRNL;
  new_misc_tio.c_oflag=0;
  new_misc_tio.c_lflag=ICANON;
  new_misc_tio.c_cc[VMIN]=0;
  //new_misc_tio.c_lflag=0;
  //new_misc_tio.c_cc[VMIN]=1;
  new_misc_tio.c_cc[VTIME]=0;
  tcflush(spMISC,TCIFLUSH);
  tcsetattr(spMISC,TCSANOW,&new_misc_tio);

  // --------- FIFO ----------------------------------------------
  if (access(FIFO_NAME, F_OK) != -1) {
     unlink(FIFO_NAME);
  }
  res = mkfifo(FIFO_NAME, 0777);
  if (res != 0) {
     fprintf(stderr, "Could not create fifo %s\n",FIFO_NAME);
     exit(EXIT_FAILURE);
  }

  //pipe_fd = open(FIFO_NAME,O_RDONLY | O_NONBLOCK);
  //pipe_fd = open(FIFO_NAME,O_RDONLY);
    pipe_fd = open(FIFO_NAME,O_RDWR);
  fprintf(stderr, "FIFO start %s\n",FIFO_NAME);

  // --------- Main loop -----------------------------------------

  switch(fork()) {
  // child process
  // read FIFO and send command to MISC
  case 0:
     while (wend) {
        j=read(pipe_fd,cmd0,BUFSIZ);
        if (j>0) {
           if (strncmp(cmd0,":q",2)==0) {
             wend=0;
	   } else if (strncmp(cmd0,":p",2)==0) {
             sleep(1);
	   } else if (strncmp(cmd0,":P",2)==0) {
             usleep(1000);
	   } else if (strncmp(cmd0,":Q",2)!=0) {
             misc_run(cmd0); // regular command
	   } //else fputs("else \n",stderr);
        } else if (j==0) {
           close(pipe_fd);
           pipe_fd = open(FIFO_NAME,O_RDONLY);
        }
     }
     tcsetattr(spMISC,TCSANOW,&old_misc_tio);
     fprintf(stderr, "FIFO end \n");
     close(pipe_fd);
     unlink(FIFO_NAME);
     exit(0);
  }

  // Main process
  // read MISC and print into stdout
  saio.sa_handler = signal_handler_IO;
  saio.sa_flags = 0;
  sigaction(SIGCHLD,&saio,NULL);

  while(STOP==FALSE) {
     fsize=read(spMISC,buf0,BUFSIZ);
     if (fsize > 0) {
        //k=strlen(buf0)-1;
	//k=BUFSIZ;
	k=fsize;
        for (i=0; i<k ; i++) {
           //bufc=((unsigned short)buf0[i] & 0xff);
           bufc=(buf0[i] & 0xff);
	   if (ibinflg==0 && bufc >=32 && bufc <127) {
              fprintf(stdout,"%s",&bufc);

   	      cmd1[0]=cmd1[1];
   	      cmd1[1]=cmd1[2];
   	      cmd1[2]=cmd1[3];
	      cmd1[3]=(char)bufc;
	      cmd1[4]='\0';
              st1=strstr(cmd1,cmd2);
              if (st1 !=0 ) {
                   sprintf(cmd1,"%s\n\0",":q");
                   write(pipe_fd,cmd1,strlen(cmd1)+1);
              }
              fflush(stdout);
	   } else if (ibinflg==0 && bufc == 13) {
              fprintf(stdout,"\n");
	      t=time(NULL);
	      ptime=localtime(&t);
	      fprintf(stdout,"%02d:%02d:%02d ",
                  ptime->tm_hour,ptime->tm_min,ptime->tm_sec);
              fflush(stdout);
	   } else if (ibinflg >  0) { // read bin data
              bincnt--;
              //fprintf(stderr, "bin %d %d\n",bincnt,(bufc & 0xff));
              if (bincnt==0) {
		 ibinflg=0;
                 //fprintf(stderr, "BIN end \n");
	      }
	   } else if (ibinflg == -1) { // size of data 
              bincnt=(bufc & 0xff)-1;
	      ibinflg=1;
              //fprintf(stderr, "byte %d %d\n",bincnt, bufc);
	   } else if (ibinflg == 0 && bufc == 26) { // binnary mode
              ibinflg=-1;
              //fprintf(stderr, "BIN %d \n",ibinflg);
	   }
	}
	memset(buf0,'\0',sizeof(buf0));
     }
  }

  //fprintf(stderr, "ending \n");
  wait(NULL);
  return;
}



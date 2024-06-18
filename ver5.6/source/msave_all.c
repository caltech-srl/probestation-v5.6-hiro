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

// --------- MISC control --------------------------------------
void cmdwait(char *cmd0)
{
    char cmd1[BUFSIZ+1],*st1,cmd2[1];

    strcpy(cmd1,"LOOK");
    st1=strstr(cmd0,cmd1);
    if (st1 != 0) {
       //fprintf(stderr,"LOOKing ,,,\n");
       sleep(WAIT2*4);
    }

    strcpy(cmd1,"CPMODE");
    st1=strstr(cmd0,cmd1);
    if (st1 != 0) {
       //fprintf(stderr,"MISC CPMODE\n");
       sleep(WAIT2*2);
    }

    strcpy(cmd1,"NMODE");
    st1=strstr(cmd0,cmd1);
    if (st1 != 0) {
       //fprintf(stderr,"MISC NMODE\n");
       sleep(WAIT2*2);
    }

    strcpy(cmd1,"TGO1");
    st1=strstr(cmd0,cmd1);
    if (st1 != 0) {
       sleep(WAIT2*4);
    }

    strcpy(cmd1,"CMD!");
    st1=strstr(cmd0,cmd1);
    if (st1 != 0) {
       sleep(WAIT2);
    }

    strcpy(cmd1,"ECMD!");
    st1=strstr(cmd0,cmd1);
    if (st1 != 0) {
       sleep(WAIT2);
    }

    strcpy(cmd1,":p");
    st1=strstr(cmd0,cmd1);
    if (st1 != 0) {
       sleep(1);
    }

    strcpy(cmd1,":P");
    st1=strstr(cmd0,cmd1);
    if (st1 != 0) {
       usleep(1000);
    }
}

void misc_run(char *cmd0)
{
    int i,k;
    char cmd1[BUFSIZ+1],*st1,cmd2[1];
    unsigned short bufc=0;

    k=strlen(cmd0)+1;
    sprintf(cmd1,"%s\r\0",cmd0);
    //fprintf(stderr,"MISC (%d) > %s\n",k,cmd0);
    for (i=0; i<k; i++) {
	bufc=(cmd0[i] & 0xff);
	if (bufc >= 32 && bufc<127) {
           cmd2[0]=cmd0[i];
           write(spMISC,cmd2,1);
	} else if (bufc == 13 || bufc == 0) {
           cmd2[0]='\r';
           write(spMISC,cmd2,1);
	} else {
           cmd2[0]=' ';
           write(spMISC,cmd2,1);
	}
    }


    //tcflush(spMISC,TCIOFLUSH);
    //usleep(5000);

    // some command should wait longer ----
    cmdwait(cmd0);
}

void misc_upld(char *cmd0)
{
    int k;
    char cmd1[BUFSIZ+1],*st1;

    k=strlen(cmd0)+1;
    sprintf(cmd1,"%s\r\0",cmd0);
    // while uploading files, usually do not echo.
    //fprintf(stderr,"MISC (%d) > %s\n",k,cmd0);
    write(spMISC,cmd1,k+1);
    //fflush(stdout);
    tcflush(spMISC,TCIOFLUSH);
    usleep(WAIT1);

}


void misc_set(char *misc_fil)
{
    int k;
    char cmd0[BUFSIZ+1],cmd1[BUFSIZ+1];

    if ((fpMISC = fopen (misc_fil, "r")) == NULL) {
        fprintf (stderr, " --- Input file does not exist !!! \n");
        return;
    }
    while(fgets(cmd1, 50, fpMISC) != NULL) {
        //fprintf(stderr,"MISC > %s",cmd0);
        sprintf(cmd0,"%s              \r",cmd1);
        misc_run(cmd0);
    }
    fclose(fpMISC);
}


/*
 * MTERM serial RX reader
 *
 *  hiro (miyasaka@caltech.edu)
 *  alpha version       v0.0.1   01/27/09
 * 
 */

int mrxdr(void)
{
   int  i,j,wend=1;
   char buf0[BUFSIZ+1];

   while (wend) {
      j=read(spMISC, &buf0, 1);    
//      j=read(spMISC, &buf0, BUFSIZ);    
      if (j>0) {
         fputs(buf0,stdout);
         fflush(stdout);
//         fprintf (stderr, "!");
      }
      if (strncmp(buf0,":q",2)==0) wend=0;
   }

   return 0;
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



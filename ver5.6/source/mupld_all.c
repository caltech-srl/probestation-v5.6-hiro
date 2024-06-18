/*
 * MISC file UP-loader
 *
 *  hiro (miyasaka@caltech.edu)
 *  alpha version       v0.0.1   08/24/08
 *  beta                v0.1.0   01/26/09
 *  v1.0                v1.0.0   10/05/09
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

FILE   *fpMISC;
int     spMISC;

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
   int  i,j,wend=1,k,inflg=1;
   int  fildes[2];
   int  bufc=13;
   int  res,pipe_fd,open_mode=O_WRONLY;
   long ic=0l,iwait1=300l;
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


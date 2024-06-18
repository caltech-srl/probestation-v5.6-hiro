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
        usleep(1000);
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


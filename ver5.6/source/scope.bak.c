/*
 * Report non-trigger pixel during the scope waveform capture
 * H.Miyasaka @ caltech
 *
 * alpha v0.0.1          10/26/09
 *
 * options
 * 	dmv: define DMVS range file
 * 	dac: define DAC STDEV range file
 * 	usr: set operator name
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

float test_vpl(int);
float test_ref(int);
int   iflg_nts=0;    // flag for no time stamp mode on MISC

int main(int argc, char *argv[])
{
    int i, j, k, r0, s1=0;
    char line1[BUFSIZ*8],cmd1[BUFSIZ+1],*st1,*st2,*st3;
    char str1[256],str2[256],str3[256],str4[256],str5[256],str6[256];

    time_t timeValue;
    struct tm *timeObject;

    time(&timeValue);
    timeObject = localtime(&timeValue);

    memset(str6,'\0',sizeof(str6));

    // ----------- read Scope data --------------------------------------
    while(fgets(line1,sizeof(line1),stdin)!=NULL) {
       strcpy(str2,str6);
       strcpy(str3,str6);
       strcpy(str4,str6);
       strcpy(str5,str6);
       sscanf(line1,"%s %s %s %s ",str2,str3,str4,str5);
//	     printf(" > %s %s \n",str3,str4);
//	     printf(" > %d \n",BUFSIZ);
       st1=strstr(str5,"XYSEL");
       if (str1 !=0) {
          fgets(line1,sizeof(line1),stdin);
          st1=strstr(line1,"-128");
          st2=strstr(line1,"127");
	  if (st1 !=0 || st2 != 0) {
	     if (s1==0) printf("Non-trigger pixel\n");
	     s1=1;
	     printf(" %s %s\n",str3,str4);
	  }
       }
    }

    // ----------- Print out DMVS test results --------------------------

}



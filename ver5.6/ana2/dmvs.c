/*
 * Analysis MISC DC test output from ASIC probe station.
 * H.Miyasaka @ caltech
 *
 * alpha v0.0.1          10/07/09
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
#include "dmvs.h"
#include "define_struct.h"

#include "dmvs_sub.c"

float test_vpl(int);
float test_ref(int);
int   iflg_nts=0;    // flag for no time stamp mode on MISC

int main(int argc, char *argv[])
{
    int i, j, k, r0, s1;
    int dmvs_pass=0,dmvs_fail=0,dmvs_noex;
    float v1=0.0,v2=0.0,r1,r2,dmvs_fail_rate,dac_sd[10];
    int  dmvs1_imax, dacs1_imax;
    char dmvs_fil[256],dacs_fil[256];
    char line1[256],cmd1[BUFSIZ+1];
    char str1[256],str2[256],str3[256],str4[256];
    char usr_name[80],run_time[80],run_temp[80],aid_name[80];
    struct  dmvs dmvs1[100];
    struct  dmvs *dmvs1_ptr = dmvs1;

    time_t timeValue;
    struct tm *timeObject;

    time(&timeValue);
    timeObject = localtime(&timeValue);

    sprintf(run_time,"%02d/%02d/%02d %02d:%02d:%02d" ,
           timeObject->tm_year + 1900 , timeObject->tm_mon + 1 ,
           timeObject->tm_mday , timeObject->tm_hour ,
           timeObject->tm_min , timeObject->tm_sec);
    strcpy(usr_name,"----------");
    strcpy(run_temp,"-----");
    strcpy(aid_name,"-----");
    memset(str4,'\0',sizeof(str3));

    strcpy(dmvs_fil,DEF_DMVS_FIL);  // set default DMVS volt range file
    strcpy(dacs_fil,DEF_DACS_FIL);  // set default DAC stdev range file

    // ----------- reads in start up options ----------------------------
    for (i=0;i<argc;i++) {
        if (*argv[i] == '-') {
           if        (strncmp(&(argv[i][1]),"dmv",3)==0){ 
               strcpy(dmvs_fil,argv[++i]); // change default DMVS set file
           } else if (strncmp(&(argv[i][1]),"dac",3)==0){
               strcpy(dacs_fil,argv[++i]); // change default DAC  set file
           } else if (strncmp(&(argv[i][1]),"usr",3)==0){
               strcpy(usr_name,argv[++i]); // set user(operator) name
           } else if (strncmp(&(argv[i][1]),"aid",3)==0){
               strcpy(aid_name,argv[++i]); // set ASIC ID name
           } else if (strncmp(&(argv[i][1]),"nts",3)==0){
               iflg_nts=1;                 // set no time stamp mode
           } else if (strncmp(&(argv[i][1]),"h",1)==0){
               printf("Usage  : dmvs < data_log.txt \n");
	       printf("Options:\n");
	       printf("   -dmv: change default DMVS set file \n");
	       printf("   -dac: change default DAC  set file \n");
	       printf("   -usr: set operator name \n");
	       printf("   -aid: set ASIC ID \n");
	       printf("   -nts: use no time stamp data\n");
	   }
        }
    }

    // ----------- Set test ranges of DMVS test -------------------------
    dmvs1_imax=set_dmvs_rng(dmvs_fil,dmvs1_ptr);
    dmvs1_imax=set_dacs_rng(dacs_fil,dmvs1_ptr,dmvs1_imax);
    
    // ----------- read DC test results ---------------------------------
    while(fgets(line1,sizeof(line1),stdin)!=NULL) {
       strcpy(str2,str4);
       strcpy(str3,str4);
       if (iflg_nts) 
          sscanf(line1,"%s %s ",str2,str3);
       else 
          sscanf(line1,"%s %s %s",str1,str2,str3);
       i=0;
       while(i<dmvs1_imax){
          if (strncmp(str2,(dmvs1_ptr+i)->pname,(dmvs1_ptr+i)->pname_len)==0) {
              if (strncmp(str3,"VERSUS",6)==0) {
                 if (strncmp(str2,"DISCTHRESH",10)==0) {
			 (dmvs1_ptr+i)->stdev=test_ref(10);
		 } else if (strncmp(str2,"VPL2",4)==0) {
			 (dmvs1_ptr+i)->stdev=test_vpl(16);
		 } else if (strncmp(str2,"VPL3",4)==0) {
			 (dmvs1_ptr+i)->stdev=test_vpl(16);
                 } else if (strncmp(str2,"4VREF",5)==0) {
			 (dmvs1_ptr+i)->stdev=test_ref(9);
		 } else if (strncmp(str2,"VNL1",4)==0) {
			 (dmvs1_ptr+i)->stdev=test_vpl(4);
		 } else if (strncmp(str2,"VNL2",4)==0) {
			 //printf("VNL2\n");
		 }
	      } else {
	         (dmvs1_ptr+i)->meas=atof(str3);
	         test_dmvs_rng(dmvs1_ptr, i);
	      }
	  } else if (strncmp(str2,"ADCTEMP",7)==0) {
             strcpy(run_temp,str3);
	  }
	  i++;
       }
    }

    // ----------- Print out DMVS test results --------------------------
    print_dmvs_header(aid_name,usr_name,run_time,run_temp);
    print_dmvs_result(dmvs1_ptr,dmvs1_imax);
    print_dacs_result(dmvs1_ptr,dmvs1_imax);

}



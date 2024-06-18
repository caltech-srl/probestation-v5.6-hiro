
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

char line2[]="------------------------------------------------------------------";
char line3[]="..................................................................";

int set_dmvs_rng(const char *dmvs_fil, void *d1_ptr)
{
   int i=0;
   float v1,v2,r0,r1,r2;
   char fname[256],line1[256],str1[256],str2[256],str3[256];
   struct dmvs *d1 = d1_ptr;

   fd1=fopen(dmvs_fil,"r");
   while(fgets(line1,sizeof(line1),fd1)!=NULL) {
       if (strncmp(line1,"#",1)!=0){
          memset(str1,'\0',sizeof(str1));
          memset(str2,'\0',sizeof(str2));
          memset(str3,'\0',sizeof(str3));
          sscanf(line1,"%s %s %s",str1,str2,str3);
          v1=atof(str2);
          v2=atof(str3);
          r1=0.0; r2=0.0;
          r1=(v1 < v2 ? v1 : v2);
          r2=(v1 < v2 ? v2 : v1);
          if (r1==0.0 && r2==0.0) r0=RNG_NOTSET; else r0=RNG_TEST;
          strcpy((d1+i)->pname,str1);
          (d1+i)->pname_len=strlen(str1);
          (d1+i)->act_flg=r0;
          (d1+i)->rng_min=r1;
          (d1+i)->rng_max=r2;
          i++;
       }
    }
    fclose(fd1);
    return i--;
}

int set_dacs_rng(const char *dacs_fil, void *d1_ptr, int imax)
{
   int    i=0, im=0;
   float  v1,v2,r0,r1,r2;
   char   fname[256],line1[256],str1[256],str2[256],str3[256];
   struct dmvs *d1 = d1_ptr;

   fd1=fopen(dacs_fil,"r");
   while(fgets(line1,sizeof(line1),fd1)!=NULL) {
       if (strncmp(line1,"#",1)!=0){
          memset(str1,'\0',sizeof(str1));
          memset(str2,'\0',sizeof(str2));
          memset(str3,'\0',sizeof(str3));
          sscanf(line1,"%s %s %s",str1,str2,str3);
          v1=atof(str2);
          v2=atof(str3);
          r0=0; r1=0.0; r2=0.0;
          r1=(v1 < v2 ? v1 : v2);
          r2=(v1 < v2 ? v2 : v1);
          if (r1==0.0 && r2==0.0) r0=RNG_NOTSET; else r0=RNG_TEST;
	  i=0; im=-1;
	  while(i<imax) {
             if(strncmp(str1,(d1+i)->pname,(d1+i)->pname_len)==0){
                im=i;
	     }
	     i++;
	  }
	  if (im==-1) im=(++imax)-1;
          strcpy((d1+im)->pname,str1);
          (d1+im)->pname_len=strlen(str1);
          (d1+im)->sd_act_flg=r0;
          (d1+im)->sd_rng_min=r1;
          (d1+im)->sd_rng_max=r2;
       }
    }
    fclose(fd1);
    return imax;
}

void test_dmvs_rng(void *d1_ptr, int i)
{
    float v1,r1,r2;
    struct dmvs *d1 = d1_ptr;

    if ((d1+i)->act_flg==RNG_TEST) {
       v1=(d1+i)->meas;
       r1=(d1+i)->rng_min;
       r2=(d1+i)->rng_max;
       if (v1 >= r1 && v1 <= r2) {
          (d1+i)->status=PASS;
       } else {
          (d1+i)->status=FAIL;
       }
    }
}

float fstdev(float *val, int imax)
{
    int i=0;
    float sum=0.0, sumsq=0.0, mean=0.0;

    for (i=0;i<imax;i++) sum = sum + val[i];

    mean=sum/(float)imax;
    for (i=0;i<imax;i++) sumsq = sumsq + (val[i]-mean)*(val[i]-mean);

    return( sqrt(sumsq/(float)(imax-1)));
}

float test_vpl(int imax)
{
    int i,j;
    float v1,v2[]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
    float v3[16],v4,stdev1;
    char  line1[256],str1[256],str2[256],str3[256],str4[256];
    extern iflg_nts;
    memset(str4,'\0',sizeof(str4)-1);

    for (i=0;i<imax;i++) {
       if (fgets(line1,sizeof(line1),stdin)!=NULL) {
	  strcpy(str2,str4);
 	  strcpy(str3,str4);
          if (iflg_nts)
             sscanf(line1,"%s %s",str2,str3);
          else
             sscanf(line1,"%s %s %s",str1,str2,str3);
	  j=atoi(str2);
	  v1=atof(str3);
	  v2[j]=v1;
       }
    }

    v4=(v2[15]-v2[0])/(float)(imax-1);
    for (i=0;i<imax-1;i++) {
       v3[i]=(v2[i+1]-v2[i])/v4;
    }
    stdev1=fstdev(v3,imax-1);
    return(stdev1);
}

float test_ref(int imax)
{
    int i,j;
    int   b1[16];
    float v1[]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
    float v2[16],v3[16],v4,v5[16],stdev1;
    float sum=0.0;
    char  line1[256],str1[256],str2[256],str3[256],str4[256];
    extern iflg_nts; //flag no time stamp
    memset(str4,'\0',sizeof(str4)-1);

    for (i=0;i<imax;i++) {
       if (fgets(line1,sizeof(line1),stdin)!=NULL) {
	  strcpy(str2,str4);
	  strcpy(str3,str4);
	  if (iflg_nts) 
              sscanf(line1,"%s %s",str2,str3);
	  else
              sscanf(line1,"%s %s %s",str1,str2,str3);
	  b1[i]=atoi(str2);
	  v1[i]=atof(str3);
       }
    }

    for (i=0;i<imax-1;i++) {
       v2[i]=v1[i+1]-v1[0];
       sum=sum+v2[i];
       //printf("%f \n",v2[i]);
    }
    for (i=0;i<imax-3;i++) {
       v3[i]=v2[imax-3]/(pow(2.0,(float)imax-3.0)/(pow(2.0,(float)i)));
       //printf("%f \n",v3[i]);
    }
   // v4=v2[imax-2];
    v4=sum-v2[imax-2];
       //printf("%f \n",v4);
    for (i=0;i<imax-3;i++) {
       v5[i]=(v2[i]-v3[i])/v4;
       //printf("%f \n",v5[i]);
    }
    stdev1=fstdev(v5,imax-3);
    return(stdev1);
}


void print_dmvs_header(const char *aid, const char *usr, const char *rtime, const char *rtemp)
{
    // header
    printf("\n");
    printf("%s \n",line2);
    printf("     ASIC probe station DC test report \n");
    printf("\n");
    printf(" ASIC ID      : %s \n",aid);
    printf(" Operator     : %s \n",usr);
    printf(" Date & time  : %s \n",rtime);
    printf(" Temperature  : %s C \n",rtemp);
    printf("\n");

    return;
}

void print_dmvs_result(void *d1_ptr, int imax)
{
    int i,dmvs_pass=0,dmvs_fail=0,dmvs_noex=0;
    float v1,r1,r2,dmvs_fail_rate;
    struct dmvs *d1 = d1_ptr;

    // DMVS result
    //
    printf("%s \n",line2);
    printf("%15s %10s %10s %10s           %5s\n",
           "Probe point","Meas [V]","Range min","- max [V]","Result");
    printf("%s\n",line3);
    for (i=0;i<imax;i++){
        v1=(d1+i)->meas;
        r1=(d1+i)->rng_min;
        r2=(d1+i)->rng_max;
        printf("%15s %10.5f %10.5f %10.5f ",(d1+i)->pname,v1,r1,r2);
        switch((d1+i)->status) {
        case PASS:
           dmvs_pass++;
           printf(" ........ pass \n");
           break;
        case FAIL:
           dmvs_fail++;
           printf(" -------- FAIL \n");
           break;
        default:
           dmvs_noex++;
           printf(" not exam      \n");
        }
    }
    printf("%s\n",line3);
    if (dmvs_fail==0) {
       printf(" DMVS test result : PASS \n");
    } else {
       printf(" DMVS test result : FAIL \n");
    }
    dmvs_fail_rate=(float)dmvs_fail/((float)(dmvs_pass+dmvs_fail))*100.0;
    printf(" %d (%7.2f %% ) out of %d fail. %d not exam. \n",
                    dmvs_fail, dmvs_fail_rate,dmvs_pass+dmvs_fail,dmvs_noex);

    printf("\n");
    //printf("%s \n",line2);

    return;
}


/**************************************************************************
   Print DAC versus test result
  
   *d1_ptr: dmvs structure
   imax:    number of items
 *************************************************************************/
void print_dacs_result(void *d1_ptr, int imax)
{
    int i,dacs_pass=0,dacs_fail=0,dacs_noex=0;
    float v1,r1,r2,dacs_fail_rate;
    struct dmvs *d1 = d1_ptr;

    // DACS result
    //
    printf("%s \n",line2);
    printf("%15s %10s %10s %10s           %5s\n",
           "Probe point","STDEV","Range min","- max   ","Result");
    printf("%s\n",line3);
    for (i=0;i<imax;i++){
	if (((d1+i)->sd_act_flg)!=0) {
           v1=(d1+i)->stdev;
           r1=(d1+i)->sd_rng_min;
           r2=(d1+i)->sd_rng_max;
           printf("%15s %10.5f %10.5f %10.5f ",(d1+i)->pname,v1,r1,r2);
	   if (((d1+i)->sd_act_flg)==RNG_TEST){
             if (v1 >= r1 && v1 <= r2) {
                (d1+i)->status=PASS;
                dacs_pass++;
                printf(" ........ pass \n");
             } else {
                (d1+i)->status=FAIL;
                dacs_fail++;
                printf(" -------- FAIL \n");
             }
	   } else if (((d1+i)->sd_act_flg)==RNG_NOTSET){
             dacs_noex++;
             printf(" not exam      \n");
	   }
	}
    }
    printf("%s\n",line3);
    if (dacs_fail==0) {
       printf(" DAC  test result : PASS \n");
    } else {
       printf(" DAC  test result : FAIL \n");
    }
    dacs_fail_rate=(float)dacs_fail/((float)(dacs_pass+dacs_fail))*100.0;
    printf(" %d (%7.2f %% ) out of %d fail. %d not exam. \n",
                    dacs_fail, dacs_fail_rate,dacs_pass+dacs_fail,dacs_noex);

    printf("\n");
    printf("%s \n",line2);

    return;
}


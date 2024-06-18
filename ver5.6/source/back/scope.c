/*
* Report non-trigger pixel during the scope waveform capture
*   H.Miyasaka @ caltech
* 
* alpha v0.0.1  10/26/09
*       v0.0.2  12/08/09  add feature to print single pixel waveform
*
* options
*     default: print out 0,0 pixels waveform
*     c: check any missed pixels
*     x: select colume
*     y: select row
*     s: short waveform format
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ITEM_SIZE	100
#define MAX_LINE_SIZE	6000
#define SAMPLE_POINT	1000

#define PROBE

char	*GetCSVItem(char *wp, char *buff, int size);

int	main(int argc, char *argv[])
{
    FILE *fp;
    char buff[MAX_LINE_SIZE], *wp, item[SAMPLE_POINT][MAX_ITEM_SIZE];
    char line1[BUFSIZ*8],cmd1[BUFSIZ+1],*st1;
    char str1[256],str2[256],str3[256],str4[256],str5[256],str6[256];
    char pix_pos[100];
    int	 i1, len, i=0, pixx=0, pixy=0, rpixx, rpixy;
    int  iflg_short=0,  iflg_check=0, iflg_dead, iflg_signal, iw;


    for (i=0;i<argc;i++) {
      if (*argv[i] == '-') {
         if        (strncmp(&(argv[i][1]),"x",1)==0){ 
             strcpy(pix_pos,argv[++i]); // pixel X
             pixx=atoi(pix_pos);
         } else if (strncmp(&(argv[i][1]),"y",1)==0){ 
             strcpy(pix_pos,argv[++i]); // pixel Y
             pixy=atoi(pix_pos);
         } else if (strncmp(&(argv[i][1]),"s",1)==0){ 
             iflg_short=1;               // short mode
         } else if (strncmp(&(argv[i][1]),"c",1)==0){ 
             iflg_check=1;               // check mode
         }
      }
    }

    memset(str6,'\0',sizeof(str6));
#ifdef PROBE
    if (iflg_check) printf("Non-signal pixel\n");
#endif

    while(fgets(line1,sizeof(line1),stdin)!=NULL) {
       strcpy(str2,str6);
       strcpy(str3,str6);
       strcpy(str4,str6);
       strcpy(str5,str6);
       sscanf(line1,"%s %s %s %s ",str2,str3,str4,str5);
       st1=strstr(str5,"XYSEL");
       if (st1 !=0) {
          rpixx=atoi(str3);
          rpixy=atoi(str4);
	  if (iflg_check==1) {
             fgets(buff,sizeof(buff),stdin);
	     wp=buff;
  	     for (i1=0;i1<SAMPLE_POINT;i1++) {
		if((wp = GetCSVItem(wp, item[i1], MAX_ITEM_SIZE)) == NULL){        
			break;
		}
	     }
	     iflg_dead=0;
	     iflg_signal=0;
	     for(i1 = 0; i1 < SAMPLE_POINT; i1++){
		iw=atoi(item[i1]);
		if (iw < -126 || iw >126) iflg_dead=1;
		if (iw >  10  || iw <-10) iflg_signal=1;
	     }
#ifdef PROBE
	     if (iflg_signal==0 || iflg_dead==1) printf(" %d %d \n",rpixx,rpixy);
#endif
#ifndef PROBE
	     if (iflg_signal==0) printf("no signal  %d %d \n",rpixx,rpixy);
	     if (iflg_dead  ==1) printf("no triger  %d %d \n",rpixx,rpixy);
#endif
	  } else {
	     //printf("> %d %d %d %d \n",rpixx,rpixy,pixx,pixy);
	     if (rpixx == pixx && rpixy == pixy) {
                fgets(buff,sizeof(buff),stdin);
	        wp=buff;
  	        for (i1=0;i1<SAMPLE_POINT;i1++) {
	   	   if((wp = GetCSVItem(wp, item[i1], MAX_ITEM_SIZE)) == NULL){        
			break;
		   }
	        }
	        for(i1 = 0; i1 < SAMPLE_POINT; i1++){
	   	   if (iflg_short == 0)
		       	printf("%s \n", item[i1]);
		   else 
			printf("%s ", item[i1]);
	        }
	        if (iflg_short == 1) printf("\n");
	     }
	  }
      }
  }
}


char *GetCSVItem(char *wp, char *buff, int size)
{
	int	i1;

	buff[0] = '\0';
	while(*wp == ' ' || *wp == '\t')
		wp++;
	if(*wp == '\0'){
		return(NULL);
	}
	for(i1 = 0; i1 < MAX_ITEM_SIZE; i1++, wp++){
		if(i1 >= size)
			return(NULL);
		buff[i1] = *wp;
		if(*wp == '\0'){
			buff[i1] = '\0';
			return(wp);
		}
		if(*wp == ','){
			wp++;
			buff[i1] = '\0';
			break;
		}
	}
	return(wp);
}

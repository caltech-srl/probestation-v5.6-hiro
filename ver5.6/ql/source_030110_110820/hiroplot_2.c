// 090907 impliment temp plot 
// 090807 able to compile on Linux and Cygwin
// 090607 beta as hirop
//
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "gptcall.h"
#include "hiroplot.h"
#define  PAI 3.1415927
#define  SPTIME 100000


int main (int argc, char *argv[])
{
  int i, n[5], plotmode;
  int open_flg[] = {-100,-100,-100,-100,-100,-100};
  float a, b;
  char fname[256],fname2[256];
  char cmd0[256], cmd1[256], cmd2[256];
  char opt0[256], opt1[256],dummy[2];

  plotmode = HISTO;

  plotmode=argset(argc,argv,fname, opt0);
//  printf("%s \n",opt0);

  strcpy (cmd0, EXE_PH_HIST);
  strcat (cmd0, opt0);

  sprintf(cmd2,"set tit \"%s\" \n",fname);

  while (plotmode >= 0) {
      switch (plotmode) {
	case HISTO:
	  open_flg[HISTO] = histmode(cmd0,cmd1,cmd2,open_flg[HISTO]);
	  break;
	case MAP:
	  open_flg[MAP]   = mapmode(cmd0,cmd1,cmd2,open_flg[MAP]);
	  break;
	case TEMP:
	  open_flg[TEMP]  = tempmode(cmd0,cmd1,cmd2,open_flg[TEMP]);
	  break;
	case WAVE:
	  open_flg[WAVE]  = wavemode(cmd0,cmd1,cmd2,open_flg[WAVE]);
	  break;
	case CNT:
	  open_flg[CNT]   = cntmode(cmd0,cmd1,cmd2,open_flg[CNT]);
	  break;
	}

      strcpy (cmd1, "rep \n");
      while (cmd1[0] != ':') {           // Gnuplot redirect mode
	  gptsend (open_flg[plotmode], cmd1);
	  printf ("hiroplot > ");
	  fgets (cmd1, 200, stdin);
	  strcat (cmd1, " \n");
	}
      switch (tolower(*(cmd1 + 1))) {   // Change plot mode
	case 'h': plotmode = HISTO; break;
	case 'm': plotmode = MAP  ; break;
	case 't': plotmode = TEMP ; break;
	case 'w': plotmode = WAVE ; break;
	case 'c': plotmode = CNT  ; break;
        case 'i':
            sscanf(cmd1,"%s %s",dummy,fname2);
            strcpy (cmd0, EXE_PH_HIST);
            strcat (cmd0, "-I ");
            strcat (cmd0, fname2);
	    sprintf(cmd2,"set tit \"%s\" \n",fname2);
	    for (i=0;i<6;i++) open_flg[i] = 
		    (open_flg[i] >=0 ? (open_flg[i]+1)*(-1): -100);
	    break;
	case 'p':
	  strcpy(fname2,"tmp");
	  sscanf(cmd1,"%s %s",dummy,fname2);
	  plotpng(fname2,plotmode,open_flg[plotmode]);
	  break;
	case 'e':
	  plotmode = -1;
	  break;
	}
    }

  gptcloseall;

}

// 090607 beta as hirop
// 090807 able to compile on Linux and Cygwin
// 090907 impliment temp plot (only for HEFT)
// ????08 impliment waveform and counts rate (only for HEFT) plot
// 080308 Commands are now autosave and able to recall 
//        requires readline and history library.
//        use makefile_2.0 & hiroplot_2.c for non-readline version
// 080408 add ":i" in order to change input file
// 080508 fix x-y ratio on map plot
//
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "gptcall.h"
#include "hiroplot.h"
#define  PAI 3.1415927
#define  SPTIME 100000

char *readline(const char *prompt);
void using_history(void);
void add_history(const char *string);
int read_history(const char *filename);
int write_history(const char *filename);



int main (int argc, char *argv[])
{
  int i, n[5], plotmode, skpflg;
  int open_flg[] = {-100,-100,-100,-100,-100,-100,-100};  
  float a, b;
  char fname[256],fname2[256];
  char cmd0[256], cmd1[256], cmd2[256];
  char opt0[256], opt1[256],dummy[2];
  char det_sel[5];
  char *cmdr;

  plotmode = HISTO;

  plotmode=argset(argc,argv,fname, opt0, det_sel);
//  printf("%s \n",opt0);
  using_history();
  read_history(".hp_history");

  strcpy (cmd0, EXE_PH_HIST);
  strcat (cmd0, opt0);

  sprintf(cmd2,"set tit \"%s\" \n",fname);
  strcpy(fname2, fname);

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
	case LEAK:
	  open_flg[LEAK]  = leakmode(cmd0,cmd1,cmd2,open_flg[LEAK]);
	  break;
	}

      strcpy (cmd1, "rep \n");
      while (cmd1[0] != ':') {           // Gnuplot redirect mode
	  gptsend (open_flg[plotmode], cmd1);
	  //printf ("hiroplot > ");
	  //fgets (cmd1, 200, stdin);
	  cmdr=readline("hiroplot > ");
	  add_history(cmdr);
	  strcpy(cmd1, cmdr);
	  strcat (cmd1, " \n");
	  free(cmdr);
	}
      skpflg=0;
      switch (tolower(*(cmd1 + 1))) {   // Change plot mode
	case 'h': plotmode = HISTO; break;
	case 'm': plotmode = MAP  ; break;
	case 't': plotmode = TEMP ; break;
	case 'w': plotmode = WAVE ; break;
	case 'c': plotmode = CNT  ; break;
	case 'l': plotmode = LEAK ; break;
	case 'd':
	  sscanf(cmd1,"%s %s",dummy,det_sel);
          skpflg=1;
	case 'i':
	  if (!(skpflg)) sscanf(cmd1,"%s %s",dummy,fname2);
          strcpy (cmd0, EXE_PH_HIST);
          strcat (cmd0, "-I ");
          strcat (cmd0, fname2);
          strcat (cmd0, " -d ");
          strcat (cmd0, det_sel);
	  fprintf (stderr,"%s \n",cmd0);
          sprintf(cmd2,"set tit \"%s\" \n",fname2);
          for (i=0;i<7;i++) open_flg[i] = (open_flg[i] >=0 ? (open_flg[i]+1)*(-1): -100);
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

  write_history(".hp_history");
  gptcloseall;

}

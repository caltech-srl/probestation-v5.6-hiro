/*
 * 091007   use this for make simpler main routine of hitoplot.c
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hiroplot.h"


// ------------------------------------------------------------
//      gnuplot call for HISTOGRAM mode 
// ------------------------------------------------------------
int
histmode (char *cmd0, char *cmd1, char *cmd2, int open_flg)
{
  char opt1[256];

  printf (" ---------- HISTO mode ------------------------- \n");
  printf (" Type  :m [-> Map mode]        :t [-> Temp mode] \n");
  printf ("       :w [-> Wave mode]       :c [-> Cnt mode]  \n");
  printf ("       :p filename [Print]     :I filename [open]\n");
  printf ("       :d Det position [0=all] :e [Exit]         \n");
  sprintf(cmd1,"%s > %s.hist.dat ",cmd0,TMPDIR);
  system (cmd1);
  //printf("cmd1=%s \n",cmd1);

  if (open_flg == -100)
    {
      open_flg = gptopen ("");
      gptsend (open_flg, "set mouse \n");
#ifdef OSX
      gptsend (open_flg, "set term x11 font \"arial,18\" \n");
#endif
#ifdef CYGWIN
      gptsend (open_flg, "set term x11 font \"arial,18\" \n");
#endif
      gptsend (open_flg, "l \"");
//      gptsend (open_flg, ".hist.gnu\" \n");
      gptsend (open_flg, HIST_GNU);
      gptsend (open_flg, cmd2);
      gptsend (open_flg, "set timestamp \"%%D  %%H:%%M\" \n");
    }
  else if (open_flg < 0) 
    {
      open_flg = -1 - open_flg;
      gptsend (open_flg, cmd2);
    }

  return (open_flg);
}

// ------------------------------------------------------------
//      gnuplot call for MAP mode 
// ------------------------------------------------------------
int
mapmode (char *cmd0, char *cmd1, char *cmd2, int open_flg)
{
  char opt1[256];

  printf (" ----------   MAP mode ------------------------- \n");
  printf (" Type  :h [-> Histogram mode]  :t [-> Temp mode] \n");
  printf ("       :w [-> Wave mode]       :c [-> Cnt mode]  \n");
  printf ("       :p filename [Print]     :I filename [open]\n");
  printf ("       :d Det position [0=all] :e [Exit]         \n");
  sprintf(cmd1,"%s -m > .map.dat ",cmd0);
  system (cmd1);

  if (open_flg == -100)
    {
      open_flg = gptopen ("");
      gptsend (open_flg, "set mouse \n");
#ifdef OSX
      gptsend (open_flg, "set term x11 font \"arial,18\" \n");
#endif
#ifdef CYGWIN
      gptsend (open_flg, "set term x11 font \"arial,16\" \n");
#endif
      gptsend (open_flg, "l \"");
//      gptsend (open_flg, ".map.gnu\" \n");
      gptsend (open_flg, MAP_GNU);
      gptsend (open_flg, cmd2);
      gptsend (open_flg, "set size 0.9, 1.0 \n");
      gptsend (open_flg, "set size ratio 1 \n");
      gptsend (open_flg, "set timestamp \"%%D %%H:%%M\" \n");
    }
  else if (open_flg < 0) 
    {
      open_flg = -1 - open_flg;
      gptsend (open_flg, cmd2);
    }
  return (open_flg);
}


// ------------------------------------------------------------
//      gnuplot call for TEMP mode 
// ------------------------------------------------------------
int
tempmode (char *cmd0, char *cmd1, char *cmd2, int open_flg)
{
  char opt1[256];

  printf (" ---------- TEMP  mode ------------------------- \n");
  printf (" Type  :h [-> Histogram mode]  :m [-> Map mode]  \n");
  printf ("       :w [-> Wave mode]       :c [-> Cnt mode]  \n");
  printf ("       :p filename [Print]     :I filename [open]\n");
  printf ("       :d Det position [0=all] :e [Exit]         \n");

  sprintf(cmd1,"%s -t > %s.temp.dat ",cmd0,TMPDIR);
  system (cmd1);

  if (open_flg == -100)
    {
      open_flg = gptopen ("");
      gptsend (open_flg, "set mouse \n");
#ifdef OSX
      gptsend (open_flg, "set term x11 font \"arial,18\" \n");
#endif
#ifdef CYGWIN
      gptsend (open_flg, "set term x11 font \"arial,18\" \n");
#endif
      gptsend (open_flg, "l \"");
      gptsend (open_flg, ".temp.gnu\" \n");
      gptsend (open_flg, cmd2);
      gptsend (open_flg, "set timestamp \"%%D %%H:%%M\" \n");
      gptsend (open_flg, "set pointsize 0.5 \n");
    }
  else if (open_flg < 0) 
    {
      open_flg = -1 - open_flg;
      gptsend (open_flg, cmd2);
    }
  return (open_flg);
}

// ------------------------------------------------------------
//      gnuplot call for WAVE mode 
// ------------------------------------------------------------
int
wavemode (char *cmd0, char *cmd1, char *cmd2, int open_flg)
{
  char opt1[256];

  printf (" ---------- WAVE  mode ------------------------- \n");
  printf (" Type  :h [-> Histogram mode]  :m [-> Map mode]  \n");
  printf ("       :t [-> Temp mode]       :c [-> Cnt mode]  \n");
  printf ("       :p filename [Print]     :I filename [open]\n");
  printf ("       :d Det position [0=all] :e [Exit]         \n");
  printf ("       :e [Exit]                                 \n");

  sprintf(cmd1,"%s -w > %s.wave.dat ",cmd0,TMPDIR);
  system (cmd1);

  if (open_flg == -100)
    {
      open_flg = gptopen ("");
      gptsend (open_flg, "set mouse \n");
#ifdef OSX
      gptsend (open_flg, "set term x11 font \"arial,18\" \n");
#endif
#ifdef CYGWIN
      gptsend (open_flg, "set term x11 font \"arial,18\" \n");
#endif
      gptsend (open_flg, "l \"");
      gptsend (open_flg, ".wave.gnu\" \n");
      gptsend (open_flg, cmd2);
      gptsend (open_flg, "set timestamp \"%%D %%H:%%M\" \n");
      gptsend (open_flg, "set pointsize 2 \n");
    }
  else if (open_flg < 0) 
    {
      open_flg = -1 - open_flg;
      gptsend (open_flg, cmd2);
    }
  return (open_flg);
}

// ------------------------------------------------------------
//      gnuplot call for CNT mode 
// ------------------------------------------------------------
int
cntmode (char *cmd0, char *cmd1, char *cmd2, int open_flg)
{
  char opt1[256];

  printf (" ---------- CNT   mode ------------------------- \n");
  printf (" Type  :h [-> Histogram mode]  :m [-> Map mode]  \n");
  printf ("       :w [-> Wave mode]       :t [-> Temp mode] \n");
  printf ("       :p filename [Print]     :I filename [open]\n");
  printf ("       :d Det position [0=all] :e [Exit]         \n");

  sprintf(cmd1,"%s -c > %s.cnt.dat ",cmd0,TMPDIR);
  system (cmd1);

  if (open_flg == -100)
    {
      open_flg = gptopen ("");
      gptsend (open_flg, "set mouse \n");
#ifdef OSX
      gptsend (open_flg, "set term x11 font \"arial,18\" \n");
#endif
#ifdef CYGWIN
      gptsend (open_flg, "set term x11 font \"arial,18\" \n");
#endif
      gptsend (open_flg, "l \"");
      gptsend (open_flg, ".cnt.gnu\" \n");
      gptsend (open_flg, cmd2);
      gptsend (open_flg, "set timestamp \"%%D %%H:%%M\" \n");
      gptsend (open_flg, "set pointsize 0.5 \n");
    }
  else if (open_flg < 0) 
    {
      open_flg = -1 - open_flg;
      gptsend (open_flg, cmd2);
    }
  return (open_flg);
}

// ------------------------------------------------------------
//      gnuplot call for LEAK mode 
// ------------------------------------------------------------
int
leakmode (char *cmd0, char *cmd1, char *cmd2, int open_flg)
{
  char opt1[256];

  printf (" ---------- LEAK  mode ------------------------- \n");
  printf (" Type  :h [-> Histogram mode]  :m [-> Map mode]  \n");
  printf ("       :w [-> Wave mode]       :t [-> Temp mode] \n");
  printf ("       :c [-> Cnt mode] \n");
  printf ("       :p filename [Print]     :I filename [open]\n");
  printf ("       :d Det position [0=all] :e [Exit]         \n");

  sprintf(cmd1,"%s -l > %s.leak.dat ",cmd0,TMPDIR);
  system (cmd1);

  if (open_flg == -100)
    {
      open_flg = gptopen ("");
      gptsend (open_flg, "set mouse \n");
#ifdef OSX
      gptsend (open_flg, "set term x11 font \"arial,18\" \n");
#endif
#ifdef CYGWIN
      gptsend (open_flg, "set term x11 font \"arial,18\" \n");
#endif
      gptsend (open_flg, "l \"");
      gptsend (open_flg, LEAK_GNU);
      gptsend (open_flg, cmd2);
      gptsend (open_flg, "set size 0.9, 1.0 \n");
      gptsend (open_flg, "set size ratio 1 \n");
      gptsend (open_flg, "set timestamp \"%%D %%H:%%M\" \n");
    }
  else if (open_flg < 0) 
    {
      open_flg = -1 - open_flg;
      gptsend (open_flg, cmd2);
    }
  return (open_flg);
}

// ------------------------------------------------------------
//      gnuplot call for print to file
// ------------------------------------------------------------
int
plotpng (char *fname0, int plotmode, int open_flg)
{
  int i,plot_flg=0;
  char fname1[256],fname2[256];
  char cmd1[256];

  switch(plotmode){
  case HISTO: sprintf(fname1,"\"%s_h.png\" \n",fname0); break;
  case MAP:   sprintf(fname1,"\"%s_m.png\" \n",fname0); break;
  case TEMP:  sprintf(fname1,"\"%s_t.png\" \n",fname0); break;
  default:
        printf("Print to file error.\n");
	return(-1);
  }

  printf("Output file to : %s \n",fname1);
  if (open_flg != -100)
    {
      gptsend (open_flg, "set term png font courb 16 size 640,480 \n");
      fgets(cmd1,255,stderr);
      fgets(cmd1,255,stderr);
      gptsend (open_flg, "set output ");
      gptsend (open_flg, fname1);
      gptsend (open_flg, "\n");
      fgets(cmd1,255,stderr);
      fgets(cmd1,255,stderr);
      gptsend (open_flg, "rep \n");
      fgets(cmd1,255,stderr);
      fgets(cmd1,255,stderr);
      sleep(3);
      gptsend (open_flg, "\n");
      gptsend (open_flg, "set output \n");
      gptsend (open_flg, "set term x11 \n");
      plot_flg=1;
    }

  return(plot_flg);

}



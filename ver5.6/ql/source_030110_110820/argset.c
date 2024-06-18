#include <stdio.h>
#include <string.h>
#include "hiroplot.h"


int argset (int argc, char *argv[], char *fname, char *opt0, char *det_sel)
{
  int i, plotmode=HISTO, help_flg = 0;

  strcpy (opt0, " ");
  for (i = 0; i < argc; ++i)
    {
      if (*argv[i] == '-')
	{
	  switch (*(argv[i] + 1))
	    {
	    case 'I':
	      strcpy (fname, argv[++i]);
              strcat(opt0," -I ");
              strcat(opt0,fname);
	      break;
	    case 't':
	      plotmode = TEMP;
	      break;
	    case 'm':
	      plotmode = MAP;
	      break;
	    case 'w':
	      plotmode = WAVE;
	      break;
	    case 'c':
	      plotmode = CNT;
	      break;
	    case 'l':
	      plotmode = LEAK;
	      break;
	    case 'a':
	      strcat (opt0, " -a ");
	      break;
	    case 'f':
	      strcat (opt0, " -f ");
	      strcat (opt0, argv[++i]);
	      break;
	    case 'M':
	      strcat (opt0, " -M ");
	      strcat (opt0, argv[++i]);
	      break;
	    case 'x':
	      strcat (opt0, " -x ");
	      strcat (opt0, argv[++i]);
	      strcat (opt0, " ");
	      strcat (opt0, argv[++i]);
	      break;
	    case 'y':
	      strcat (opt0, " -y ");
	      strcat (opt0, argv[++i]);
	      strcat (opt0, " ");
	      strcat (opt0, argv[++i]);
	      break;
	    case 'n':
	      strcat (opt0, " -n ");
	      break;
	    case 'd':
	      strcat (opt0, " -d ");
	      strcat (opt0, argv[++i]);
	      strcat (det_sel, argv[i]);
	      break;
	    case 'h':
	    default:
	      help_flg = 1;
	      plotmode = -1;
	    }
	}
    }
  if (help_flg == 1)
    {
      printf ("Usage: hiroplot [option] \n");
      printf ("  -I filename  ; set input file \n");
      printf ("  -x xmin xmax ; x range (def 0 47)\n");
      printf ("  -y ymin ymax ; y range (def 0 23)\n");
      printf ("  -f byte      ; byte size to raed from file\n");
      printf ("  -a           ; use all (def trig pix only)\n");
      printf ("  -t           ; temperature mode \n");
      printf ("  -m           ; map mode \n");
      printf ("  -w           ; wave mode \n");
      printf ("  -c           ; count mode \n");
      printf ("  -l           ; leak current mode \n");
      printf ("  -n           ; no hardware trigger info \n");
      printf ("  -d           ; use selected detector only (=0 all)\n");
      printf ("  -h           ; help \n");
    }

  return (plotmode);
}



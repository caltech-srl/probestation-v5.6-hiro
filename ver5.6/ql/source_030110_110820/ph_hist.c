/*
 * 090607
 *
 * ph_hist ---- 
 * purpose : read raw hybrid data and generate a quick look data
 * for format gnuplot.
 *
 * ver 1.0 add NuSTAR flight format 052309
 * ver 0.2 add wave form plot 032708
 * ver 0.1 read event data and output text histogram, event map.
 *         able to select region, and trigger mode.
 *
 * by Hiro
 *
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "define_struct.h"
#include "defs.h"
#include "ph_hist.h"

#include "read_event_2.c"

float therm(int);
float nu_therm(int);

void main (int argc, char *argv[])
{
  int i, j, k, n, ievflg, iph, ns1=0, ns2=0;
  long evflg[2] = { 0, 0 }, event_num = 0l, event_num1=0l, event_num2=0l;
  long icou = 0l, i_tp = 0l, i_up = 0l, evtcnt = 0l;
  long nu_cnt=0l, nu_shhicnt=0l, nu_shlocnt=0l;
  unsigned int  dat[MAXBYTE], x[MAXPIXEL], y[MAXPIXEL];
  unsigned int  sample[MAXSAMPLE];
  unsigned long sample_l[MAXSAMPLE];
  int pixodr[MAXPIXEL], pixgeom[MAXPIXEL];
  int run_c[PIXCOL * PIXROW];
  float ph_low, ph_high, ph;
  float fadcmax;
  float x1, x2, swav[10000];
  int phh[ADCMAX];
  struct event e1;
  struct event *e1_ptr = &e1;
  struct module m1;
  struct module *m1_ptr = &m1;
  struct nuleak l1;
  struct nuleak *l1_ptr = &l1;
  struct nuhouse h1;
  struct nuhouse *h1_ptr = &h1;
  struct nurate r1;
  struct nurate *r1_ptr = &r1;
  char fname[256];
  char string1[256];
  unsigned char ch;
  unsigned long l1_row[32];
//  int plotmode, xmin = 0, xmax = 47, ymin = 0, ymax = 23, pixmode = 0;
  int plotmode, xmin = 0, xmax = PIXCOL, ymin = 0, ymax = PIXROW, pixmode = 0;
  int flg_hit,xys[MAXPIXEL][MAXPIXEL];
  long imoc=0l,fmax=0l, fbyt=0l, fset=0l, fbyt2=0l;
  float fbytp;
  char cmd1[30];
  long imoc2=0l,imoc3=0l,hframe_old=0l,rframe_old=0l;
  off_t fsize1,fseek1;
  float atemp,btemp,stemp,aregs,eltim;
  float nu_temp,nu_stemp;
  int no_hard_trig=0,sel_det=0;
  long max_event = MAXEVENT;
  float xyl[4][MAXPIXEL][MAXPIXEL],xylc[4][MAXPIXEL][MAXPIXEL];
  int idet;

  srand((unsigned)time( NULL ));
  cmd1[0]=27; cmd1[1]=91; cmd1[2]=65; cmd1[3]='\r';

  strcpy (fname, string1);
  plotmode = HISTO;

  for (i = 0; i < argc; ++i)
    {
      if (*argv[i] == '-')
	switch (*(argv[i] + 1))
	  {
	  case 'I':
	    strcpy (fname, argv[++i]);
	    break;
	  case 't':
	    plotmode = TEMP;
	    break;
	  case 'm':
	    plotmode = MAP;
	    break;
	  case 'a':
	    pixmode = ALL;
	    break;
	  case 'w':
	    plotmode = WAVE;
	    break;
	  case 'c':
	    plotmode = CNT;
	    break;
	  case 'l':
	    plotmode = LEAK;
	    //plotmode = LEAK_RAW;
	    break;
	  case 'x':
	    xmin = atoi (argv[++i]);
	    xmax = atoi (argv[++i]);
	    break;
	  case 'y':
	    ymin = atoi (argv[++i]);
	    ymax = atoi (argv[++i]);
	    break;
	  case 'f':
	    fset = atol (argv[++i]);
	    break;
	  case 'M':
	    max_event = atol (argv[++i]);
	    break;
	  case 'n':
	    no_hard_trig = 1;
	    break;
	  case 'd':
	    sel_det = atoi (argv[++i]);
	    break;
	  case 'h':
	    printf ("Usage: ph_hist [option] \n");
	    printf ("  -I filename  ; set input file \n");
	    printf ("  -x xmin xmax ; x range (def 0 47)\n");
	    printf ("  -y ymin ymax ; y range (def 0 23)\n");
	    printf ("  -f byte      ; byte size to read from end of a file.\n");
	    printf ("  -M #         ; Number of events to readin.\n");
	    printf ("  -a           ; use all (def trigger pix only)\n");
	    printf ("  -t           ; temperature mode \n");
	    printf ("  -m           ; map mode \n");
	    printf ("  -w           ; wave mode \n");
	    printf ("  -c           ; count mode \n");
	    printf ("  -n           ; No hardware trig info \n");
	    printf ("  -d           ; use selected detector only (=0 all)\n");
	    printf ("  -h           ; help \n");
	    return;
	  default:
	    puts ("OPTION ERROR");
	  }
    }


  if ((fqdat = fopen (fname, "r")) == NULL)
    {
      fprintf (stderr, " --- Input file does not exist !!! \n");
      exit (-1);
    }


  //printf("%d \n",fset);
  //if (fset > 0l) {
     fseek(fqdat,0l,SEEK_END);
     fmax=ftell(fqdat);
  if (fset > 0l) {
     fbyt=fset*(fmax>fset)+fmax*(fmax<=fset);
     j=fseek(fqdat, -fbyt, SEEK_END);
     fprintf(stderr,"#ph_hist -- used only last %ul byte of data.\n",fbyt);
  } else {
     fbyt=fmax;
     j=fseek(fqdat, 0L, SEEK_SET);
  }

  for (i = 0; i < ADCMAX; i++)
    phh[i] = 0;

  for (i = 0; i < MAXPIXEL; i++) {
    for (j = 0; j < MAXPIXEL; j++) {
      xys[i][j] = 0;
      for (k = 0; k < 4; k++) {
	      xyl[k][i][j] = 0.0;
	      xylc[k][i][j] = 0.0;
      }
    }
  }


  i = 1;
  ievflg = 1;
  e1.frame = 0l;
  e1.subframe = 0l;
  e1.ttag = time ( NULL );

  //fprintf(stderr,"New !\n");
  
  // not going to use for probestaion //
  /*
  set_tty_raw();       // set up character-at-a-time
  while (ievflg >= 0)
    {
     ch = kb_getc();      // char typed by user?
     if (0x03 == ch)      // might be control-C
	{
	 set_tty_cooked();  // control-C, restore normal TTY mode
	 return 1;          // and get out
        }
     else if (ch != 0) 
        {
         //fprintf(stderr,"Hit hit!\n");
         ievflg=-1;
         continue;
        }

     //fprintf(stderr,"evt=%d\n",event_num);
     fbyt2=ftell(fqdat);
     fbytp=100.0-(((float)(fmax-fbyt2))/(float)fbyt)*100.0;
     fprintf(stderr,"[%5.1f %%] \n %s",fbytp,cmd1);
   */

  while (ievflg >= 0)
  {
     ievflg = read_event_raw (dat, evflg);
     if (ievflg <= 0) continue;
     //printf("%d \n",evflg[0]);
     switch (evflg[0])
	{
	case EVENT:
	  j = event_dat (dat, e1_ptr);
	  if (sel_det != 0 && e1_ptr->side != sel_det-1) break;
	  if (no_hard_trig) {
	    j = pixel_dat (dat, x, y, sample, pixodr, pixgeom);
	  } else {
	    j = pixel_dat_ht (dat, x, y, sample, pixodr, pixgeom);
	  }
	  if (j < 0) break;
	  // ph
	  flg_hit=0;
	  ph=0.0;
	  for (n = 0; n < e1_ptr->npix; n++)
	    {
	      if (x[n] >= xmin && x[n] <= xmax && y[n] >= ymin
		  && y[n] <= ymax)
		{
		  if ((pixgeom[n] == 3) + 
                     ((pixgeom[n]==0)+(pixgeom[n] == 1) + (pixgeom[n] == 2)) * pixmode)
			 // if pixmode=ALL then include these
		    {
		      ph_low = 0.0;
		      ph_high = 0.0;
		      for (k = 0; k < 6; k++)
			ph_low = ph_low + sample[n * CAPNUM + k];
		      for (k = 10; k < 16; k++)
			ph_high = ph_high + sample[n * CAPNUM + k];
		       ph = ph + (ph_high / 6.0 - ph_low / 6.0);
		       flg_hit=1;
		       if (plotmode == MAP) {
	  	          ++xys[x[n]][y[n]];
		       }
		  }
		  if (plotmode == WAVE) {
		     if (frand() < (pixgeom[n] == 3 ? 0.3: 0.001)) {
		        ph_low = 0.0;
		        for (k = 0; k < 6; k++)
		  	   ph_low = ph_low + sample[n * CAPNUM + k];
		        for (k =0; k<16; k++)
		   	    swav[ns1++]=sample[n * CAPNUM + k]-ph_low/6.0;
		        if (ns1 >= 8000) ns1=0;  // array pointer
			if (ns2 <  500 ) ++ns2;  // number event
		     }
		  }
		}
           }
	// count event
        if (flg_hit) {
	      iph = (int) ph + ADCMAX / 2;
	      if (iph >= 0 && iph < ADCMAX) {
		  ++phh[iph];
	      }
	      ++event_num1;
	}
	++event_num;
	++evtcnt;
	if (event_num >= max_event)
	  ievflg = -1;
	break;
	case NU_EVENT:
	  j = nu_event_dat (dat, e1_ptr);
	  if (sel_det != 0 && e1_ptr->side != sel_det-1) break;
          if (plotmode == CNT)  break;
          if (plotmode == TEMP) break;
          if (plotmode == LEAK) break;
          if (plotmode == LEAK_RAW) break;
	  j = nu_pixel_dat (dat, x, y, sample_l, pixodr, pixgeom);
	  if (j < 0) break;
	  // ph
	  flg_hit=0;
	  ph=0.0;
	  for (n = 0; n < e1_ptr->npix; n++)
	    {
	      if (x[n] >= xmin && x[n] <= xmax && y[n] >= ymin
		  && y[n] <= ymax)
		{
		  if ((pixgeom[n] == 3) + 
		     ((pixgeom[n] == 1) + (pixgeom[n] == 2)) * pixmode) 
			 // if pixmode=ALL then include these
		    {
		      ph_low  = sample_l[n * 2];
	 	      ph_high = sample_l[n * 2 + 1];
		      ph = ph+(ph_high / 6.0 - ph_low / 6.0);
	              flg_hit=1;
		      if (plotmode == MAP) {
	  	         ++xys[x[n]][y[n]];
		    }
		  }
		}
           }
	  // count event
          if (flg_hit) {
                  iph = (int) ph + ADCMAX / 2;
	          if (iph >= 0 && iph < ADCMAX) { 
	              ++phh[iph];
  	          }
		  ++event_num2;
	  }
	  ++event_num;
	  ++evtcnt;
	  if (event_num >= max_event) ievflg = -1;
	  break;
	case MODULE:
	  j = module_head (dat, m1_ptr);
	  eltim=((float)imoc)/6.0;
          if (plotmode == TEMP) {
             atemp=therm(m1_ptr->aczttemp);
             aregs=((float)m1_ptr->a_reg_val)/1000.0;
             printf ("%10.2f %10.3f %10.3f %d \n", eltim, atemp,aregs,m1_ptr->aczttemp);
          }
          if (plotmode == CNT) {
             printf ("%10.2f %d \n", eltim, evtcnt);
	     evtcnt=0l;
	  }
          ++imoc;
	  break;
	case NU_LEAK:
          if (plotmode == LEAK_RAW) {
	     j = nu_leak (dat, l1_ptr, l1_row);
	     for (i=0;i<32;i++){
	        printf ("%d %d %d %d %d \n", l1_ptr->frame, l1_ptr->det, i, l1_ptr->row, l1_row[i]);
	     }
	  }
	  if (plotmode == LEAK) {
	     j = nu_leak (dat, l1_ptr, l1_row);
	     for (i=0;i<32;i++){
  	        xyl[l1_ptr->det][i][l1_ptr->row]=xyl[l1_ptr->det][i][l1_ptr->row]+l1_row[i];
  	        xylc[l1_ptr->det][i][l1_ptr->row] += 1.0;
	     }
	  }
	  break;
	case NU_HOUSE:
	  j = nu_house(dat, h1_ptr);
          if (plotmode == TEMP) {
             nu_temp=nu_therm(h1_ptr->czttemp0);
	     if (sel_det == 1) nu_temp=nu_therm(h1_ptr->czttemp1);
	     if (sel_det == 2) nu_temp=nu_therm(h1_ptr->czttemp2);
	     if (sel_det == 3) nu_temp=nu_therm(h1_ptr->czttemp3);
             nu_stemp=nu_therm(h1_ptr->shldtemp);
	     eltim=((float)imoc2)/60.0;
	     if ((h1_ptr->frame)-hframe_old != 4) printf(" \n");
             printf ("%10.2f %f %f \n", eltim, nu_temp,nu_stemp);
          }
	  imoc2+=4l;
	  hframe_old=h1_ptr->frame;
	  break;
	case NU_RATE:
	  j = nu_rate(dat, r1_ptr);
          if (plotmode == CNT) {
             nu_cnt =r1_ptr->nczta_a;
	     if (sel_det == 1) nu_cnt =r1_ptr->ncztb_a;
	     if (sel_det == 2) nu_cnt =r1_ptr->ncztc_a;
	     if (sel_det == 3) nu_cnt =r1_ptr->ncztd_a;
	     nu_shhicnt =r1_ptr->nshhi_a;
	     nu_shlocnt =r1_ptr->nshlo_a;
	     eltim=((float)imoc3)/60.0;
	     if ((r1_ptr->frame)-rframe_old != 1) printf(" \n");
             printf ("%10.2f %d %d %d %d \n", eltim, nu_cnt, nu_shhicnt, nu_shlocnt);
	  }
	  ++imoc3;
	  rframe_old=r1_ptr->frame;
	  break;
	default:
	  break;
	}
    }

  /* 
   // not going to use for probestation
  set_tty_cooked();  // restore normal TTY mode
  fprintf (stderr,"[%5.1f %%] %d %d %d \n", fbytp, event_num, event_num1, event_num2);
  */

  fclose (fqdat);

  if (plotmode == HISTO)
    {
      for (i = 0; i < ADCMAX; i++)
	{
	  printf ("%d %d \n", i - ADCMAX / 2, phh[i]);
	}
    }
  if (plotmode == MAP)
    {
      for (i = 0; i < PIXCOL; i++) {
	printf ("%d %d %f \n", i,0 ,xys[i][0]+0.1);
        for (j = 1; j < PIXROW+1; j++) {
	  printf ("%d %d %f \n", i,j,xys[i][j-1]+0.1);
	  printf ("%d %d %f \n", i,j,xys[i][j]+0.1);
	}
        printf ("\n");
	printf ("%d %d %f \n", i+1,0 ,xys[i][0]+0.1);
        for (j = 1; j < PIXROW+1; j++) {
	  printf ("%d %d %f \n", i+1,j,xys[i][j-1]+0.1);
	  printf ("%d %d %f \n", i+1,j,xys[i][j]+0.1);
	}
        printf ("\n");
      }
    }
  if (plotmode == WAVE) {
      for (i = 0; i < ns2; i++) {
	  x2=frand()*0.8+0.1;
          for (k = 0; k < 16; k++) {
	      x1=(float)k+x2;
              printf ("%f %f \n", x1, swav[i*16+k]);
	  }
          printf ("\n");
      }
  }
  if (plotmode == LEAK)
    {
      idet=0;
      for (i = 0; i < PIXCOL; i++) {
	printf ("%d %d %f \n", i,0 ,xyl[idet][i][0]/(xylc[idet][i][0] ==0 ? 1.0 :xylc[idet][i][0]));
        for (j = 1; j < PIXROW+1; j++) {
	  printf ("%d %d %f \n", i,j,xyl[idet][i][j-1]/(xylc[idet][i][j-1]==0 ? 1.0 :xylc[idet][i][j-1]));
	  printf ("%d %d %f \n", i,j,xyl[idet][i][j]/(xylc[idet][i][j] ==0 ? 1.0 :xylc[idet][i][j]));
	}
        printf ("\n");
	printf ("%d %d %f \n", i+1,0 ,xyl[idet][i][0]/(xylc[idet][i][0] ==0 ? 1.0 :xylc[idet][i][0]));
        for (j = 1; j < PIXROW+1; j++) {
	  printf ("%d %d %f \n", i+1,j,xyl[idet][i][j-1]/(xylc[idet][i][j-1] ==0 ? 1.0 :xylc[idet][i][j-1]));
	  printf ("%d %d %f \n", i+1,j,xyl[idet][i][j]/(xylc[idet][i][j] ==0 ? 1.0 :xylc[idet][i][j]));
	}
        printf ("\n");
      }
    }

}

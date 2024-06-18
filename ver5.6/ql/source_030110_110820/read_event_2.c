/*
 * Crush2
 * 8/11/07	add Module header 
 *                                            by   H.Miyasaka   
 */

int read_event_raw( unsigned int *dat, int *evflg ) 
{
  // dat : return value of data.
  // evflg :[0] delimiter type. [1] 
  // readflg :  Flg for whether read successfully or not, and pointer of data.
  // offset_fqdat : Parameter to rewind the file pointer.
  // 			use when frame data find inside the event data.
  // c : for binary data 
  // s1-s6 : 3 (6) byte char to seek delimiter.
  int i=0,j,k,n,m,readflg,imax;
  int j1=0, vskp_flg;
  long offset_fqdat;
  unsigned int c;
  char s1[13],s2[13],s3[7],s4[7],s5[7],s6[7],s7[7],s8[7],s9[7],sa[7];
  //char s1[6],s2[6],s3[3],s4[3],s5[3],s6[3];
  dat[0]=0; dat[1]=0; dat[2]=0;

  sprintf(s1,"%x%x%x%x%x%x",0xf0,0xa5,0xa5,0xf0,0xa5,0xa5); //frame
  sprintf(s4,"%x%x%x",0xf0,0xaa,0xaa);  //event
  sprintf(s5,"%x%x%x",0xf0,0xa5,0xa5);  //frame short
  sprintf(s6,"%x%x%x",0xf0,0xf1,0xf2);  //module header
  sprintf(s7,"%x%x%x",0xf0,0xaa,0xab);  //NuSTAR event
  sprintf(s8,"%x%x%x",0xf0,0xaa,0xac);  //NuSTAR leakage current
  sprintf(s9,"%x%x%x",0xf0,0xaa,0xad);  //NuSTAR housekeeping
  sprintf(sa,"%x%x%x",0xf0,0xaa,0xae);  //NuSTAR rate

  //printf("%s \n",s4);

  readflg=-1; //If successfully read, this will be positive number.
  while (j1==0){
	if ((c=getc(fqdat)) == EOF) {
                #ifdef DEBUG
		  printf("%x -- EOF at search delimiter \n",c);
                #endif
		return(readflg);
	}
	dat[0]=dat[1];
	dat[1]=dat[2];
	dat[2]=c;
  	sprintf(s3,"%x%x%x",dat[0],dat[1],dat[2]);
        j1=(strcmp(s3,s4)==0)*EVENT+(strcmp(s3,s5)==0)*FRAME+(strcmp(s3,s6)==0)*MODULE
          +(strcmp(s3,s7)==0)*NU_EVENT+(strcmp(s3,s8)==0)*NU_LEAK+(strcmp(s3,s9)==0)*NU_HOUSE
          +(strcmp(s3,sa)==0)*NU_RATE;
	//#ifdef DEBUG
	//  fprintf(stderr,"%x %x %x %s %s %s \n",dat[0],dat[1],dat[2],s3,s4,s6);
        //#endif
	i++;
  }
  #ifdef DEBUG
    fprintf(stderr,"%d %d find!! %x %x %x \n",i,j1,dat[0],dat[1],dat[2]);
  #endif
  evflg[0]=j1;

  vskp_flg=0;
  switch(j1)
  {
        case NU_LEAK: // -------  Read NuSTAR leakage current -------
                for (i=0; i<NU_LEAK_BYTE; i++) {
                  if ((c=getc(fqdat)) == EOF) {
                          #ifdef DEBUG
                             printf("%x -- EOF at event \n",c);
                          #endif
                          return(-1);
                  }
                  //printf("%x ",c);
                  dat[i]=c;
                }
                readflg=NU_LEAK_BYTE;
		vskp_flg=1;
		break;
        case NU_EVENT: // -------  Read NuSTAR Event -------
                for (i=0; i<NU_EVENT_BYTE; i++) {
                  if ((c=getc(fqdat)) == EOF) {
                          #ifdef DEBUG
                             printf("%x -- EOF at event \n",c);
                          #endif
                          return(-1);
                  }
                  dat[i]=c;
                }
                readflg=NU_EVENT_BYTE;
                break;
	case EVENT: // ----------  Read Event header  ----------
   		for (i=0; i<EVENT_HEADER_BYTE; i++) {
		  if ((c=getc(fqdat)) == EOF) {
			  return(-1);
		  }
	  	  dat[i]=c;
   		}
   		// ----------  Read Event Record  ----------
   		imax=(int)((12.0*17.0)*dat[3] / 8.0 + 0.5)+ (dat[3] % 2); 
  		// byte to read for x,y and sample[16]. (2sample/3byte)
		// 1.5 byte will be dummy for odd npix event.
   		if (dat[3] < MAXPIXEL && dat[3] > 0) {
   			for (i=0; i<imax; i++) {
	  	  		if ((c=getc(fqdat)) == EOF) {
                        		#ifdef DEBUG
		  			  printf("%x -- EOF at event \n",c);
                        		#endif
		        		return(-1);
		  		}
		  		dat[i+EVENT_HEADER_BYTE]=c;
 	 		}
    		}else {
	  		return(0);
    		}
    		readflg=imax+EVENT_HEADER_BYTE;
   		break;
   	case MODULE: // ----------  Read Module header  ----------
  		for (i=0; i<MODULE_HEADER_BYTE; i++) {
	  		if ((c=getc(fqdat)) == EOF) {
                		#ifdef DEBUG
				  printf("%x -- EOF at Module reading \n",c);
                		#endif
				return(-1);
	  		}
	  		dat[i]=c;
       		  	#ifdef DEBUG
	    	  	  fprintf(stderr,"%x ",c);
                  	#endif
  		}
		readflg=MODULE_HEADER_BYTE;
   		#ifdef DEBUG
   		  fprintf(stderr,"\n");
   		#endif
		break;
        case NU_HOUSE: // ----------  Read NuSTAR Module header  ----------
                //printf(" NUSTAR housekeeping reading %d \n",evflg[0]);
                for (i=0; i<NU_HOUSE_BYTE; i++) {
                        if ((c=getc(fqdat)) == EOF) {
                                #ifdef DEBUG
                                  printf("%x -- EOF at NUSTAR housekeeping reading \n",c);
                                #endif
                                return(-1);
                        }
                        dat[i]=c;
                        #ifdef DEBUG
                          fprintf(stderr,"%x ",c);
                        #endif
                }
                readflg=NU_HOUSE_BYTE;
                #ifdef DEBUG
                  fprintf(stderr,"\n");
                #endif
                vskp_flg=1;
                break;
        case NU_RATE: // ----------  Read NuSTAR RATE BLOCK  ----------
                for (i=0; i<NU_RATE_BYTE; i++) {
                        if ((c=getc(fqdat)) == EOF) {
                                #ifdef DEBUG
                                  printf("%x -- EOF at NUSTAR rate reading \n",c);
                                #endif
                                return(-1);
                        }
                        dat[i]=c;
                        #ifdef DEBUG
                          fprintf(stderr,"%x ",c);
                        #endif
                }
                readflg=NU_RATE_BYTE;
                #ifdef DEBUG
                  fprintf(stderr,"\n");
                #endif
                vskp_flg=1;
                break;
   	default:
         	 return(-2);
  }

  if (vskp_flg == 0) {
  // ----------  Read exctra 5 byte & rewind ----------
  for (i=0;i<5;i++){
	if ((c=getc(fqdat)) == EOF) {
                #ifdef DEBUG
		printf("%x -- EOF at exctra byte reading \n",c);
                #endif
		return(readflg);
	}
	dat[readflg+i]=c;
  }
  offset_fqdat=-5l;
  j=fseek(fqdat,offset_fqdat,SEEK_CUR);

  // ----------  Check validity  ----------
  // Normal frame should follow by known delimiter

  sprintf(s3,"%x%x%x",dat[readflg],dat[readflg+1],dat[readflg+2]);
  j1=(strcmp(s3,s4)==0)*EVENT+(strcmp(s3,s5)==0)*FRAME+(strcmp(s3,s6)==0)*MODULE
    +(strcmp(s3,s7)==0)*NU_EVENT+(strcmp(s3,s8)==0)*NU_LEAK+(strcmp(s3,s9)==0)*NU_HOUSE
    +(strcmp(s3,sa)==0)*NU_RATE;

  if (j1 == 0 ) {
        #ifdef DEBUG
	fprintf(stderr,"%s %d not good event record \n ",s3,j1);
        #endif
	readflg=0;
  } 
  }

  return(readflg);
}


//
//  ------------- Event record extract ------------------//
//
int event_dat(unsigned int *dat, void *e1_ptr)
{
  int i;
  struct event *e1 = e1_ptr;

  e1->time   = dat[0]*65536l+dat[1]*256l+dat[2];
  e1->npix   = dat[3];
  e1->treset = dat[4]*256l+dat[5];
  e1->side   = (dat[6] & 0x80) >> 7;
  e1->tpulse = (dat[6] & 0x7c) >> 2;
  e1->upevent= (dat[6] & 0x03)*8 + ((dat[7] & 0xe0) >> 5);
  e1->shevent= (dat[7] & 0x1f);
  e1->tags   = (dat[6] & 0x7f)*256l+dat[7];
  e1->second = (dat[8] & 0xf0) >> 4;
  e1->scap   = (dat[8] & 0xf);
  e1->didscap= (dat[8] & 0xf);  // need add detector ID at bits4-7 later.
  e1->subframe= e1->subframe+1l;
  e1->frame  = e1->frame+1l;

  return(0);
}

//  ------------- NuSTAR Event record extract ------------------//
//
int nu_event_dat(unsigned int *dat, void *e1_ptr)
{
  int i;
  struct event *e1 = e1_ptr;

  e1->ftyp   =  N_FORMAT;          // NuSTAR format
  e1->time   = (dat[0] & 0x0f)*65536l+dat[1]*256l+dat[2];
  e1->npix   =  9;
  e1->nht    = (dat[3] & 0xe0) >> 5;        // number of hardware trigger
  e1->treset =  dat[4]*256l+dat[5];         // time since reset
  e1->side   = (dat[3] & 0x18) >> 3;        // detector number
  e1->tpulse = (dat[3] & 0x04) >> 2;        // test pulse flag
  e1->upevent= (dat[3] & 0x02) >> 1;        // u-p flag
  e1->shevent= (dat[3] & 0x01);             // shield hit flag
  e1->scap   = (dat[0] & 0xf0) >> 4;        // starting cap
  e1->tevt   =  dat[6]*65536l+dat[7]*256l+dat[8]; // time since last event
  e1->col    = (dat[9] & 0xf8) >> 3;        // center pixel (column)
  e1->row    = (dat[12]& 0xf8) >> 3;        // center pixel (row)
  e1->tr_num = (dat[ 9]& 0x01)*65536l+dat[10]*256l+dat[11]; // Time rise num
  e1->tr_den = (dat[13]& 0x3f)*256l+dat[17]; // Time rise den

  e1->subframe= e1->subframe+1l;
  e1->pixel += e1->npix;

  return(0);
}

//
//  ------------- Pixel record extract ------------------//
//
int pixel_dat(dat, x, y, sample, pixodr, pixgeom)
unsigned int *dat;
unsigned int *x;
unsigned int *y;
unsigned int *sample;
int *pixodr;
int *pixgeom;
{
  int i, j, npix, ips=0, idx1, idxflg, ipc, retflg=1;
  int gtab;
  //Geometry table
  int geomd[16][12]={{1,2,1,2,3,2,1,2,1,0,0,0},     // 9 pix
	            {1,2,2,1,2,3,3,2,1,2,2,1},     // 12 pix .. 3x4
      	            {1,2,1,2,3,2,2,3,2,1,2,1},     // 12 pix .. 4x3
      	            {1,2,2,3,0,0,0,0,0,0,0,0},     //  4 pix .. @[0,0]
	            {2,1,3,2,0,0,0,0,0,0,0,0},     //  4 pix .. @[0,max]
	            {2,3,1,2,0,0,0,0,0,0,0,0},     //  4 pix .. @[max,0]
	            {3,2,2,1,0,0,0,0,0,0,0,0},     //  4 pix .. @[max,max]
	            {1,2,1,2,3,2,0,0,0,0,0,0},     //  6 pix .. 2x3,@x=0
	            {2,3,2,1,2,1,0,0,0,0,0,0},     //  6 pix .. 2x3,@x=max
	            {1,2,2,3,1,2,0,0,0,0,0,0},     //  6 pix .. 3x2,@y=0
	            {2,1,3,2,2,1,0,0,0,0,0,0},     //  6 pix .. 3x2,@y=max
	            {1,2,2,1,2,3,3,2,0,0,0,0},     //  8 pix .. 2x4,@x=0
	            {2,3,3,2,1,2,2,1,0,0,0,0},     //  8 pix .. 2x4,@x=max
	            {1,2,2,3,2,3,1,2,0,0,0,0},     //  8 pix .. 4x2,@y=0
	            {2,1,3,2,3,2,2,1,0,0,0,0},     //  8 pix .. 4x2,@y=max
	            {0,0,0,0,0,0,0,0,0,0,0,0}};    //  default

  idx1=9;
  idxflg=0;
  ipc=0;

  npix   = dat[3];
  if (npix > MAXPIXEL-1) return(-1);

  for (i=0; i<npix; i++) {
      pixodr[i] = i;
      y[i] = ((dat[idx1] & 0xfc)>> 2)*(idxflg==0) + 
            (((dat[idx1] & 0xf)<<2) + ((dat[idx1+1] & 0xc0) >> 6 ))*(idxflg==1);
      x[i] = (((dat[idx1] & 0x3) << 4) + ((dat[idx1+1] & 0xf0) >> 4))*(idxflg==0) + 
                (dat[idx1+1] & 0x3f) *(idxflg==1);
      if (x[i] > PIXCOL-1 || y[i] > PIXROW-1) return(-1);
      idx1=idx1+1+idxflg;
      idxflg=(idxflg == 0);
      for (j=0; j<CAPNUM; j++){
          sample[ips] = (((dat[idx1] & 0xf) << 8) + dat[idx1+1])*(idxflg==1) +
                 ((dat[idx1] << 4) + ((dat[idx1+1] & 0xf0) >> 4))*(idxflg==0);
          idx1=idx1+1+idxflg;
          idxflg=(idxflg == 0);
          ips++;
      }

  }
   // fprintf(stderr,"\n");

  switch(npix)
  {
     case 9:   // Most popular event. center will be #4.
	     gtab=0;
	     break;
     case 12:  // Two adjcent pixel trigger event. There are 4x3 and 3x4 case.
	     if (x[0]==x[3]) gtab=1; else gtab=2;
	     break;
     case 4:  // This only happen when trigger a pixel at corner of the detector.
	     // There are 4 case which are [0,0],[0,max],[max,0],and [max,max]
	     if (x[3]==0) 
		     if (y[3]==0) gtab=3; else gtab=4;
	     else    if (y[3]==0) gtab=5; else gtab=6;
	     break;
     case 6: // This only happen when trigger a pixel at edge of the detector.
	     // There are 4 case which are x=0,x=max,y=0 and y=max
	     if (x[0]==x[2])
		     if (x[5]==0) gtab=7; else gtab=8;
	     else    if (y[5]==0) gtab=9; else gtab=10;
	     break;
     case 8: // This only happen when two adjacent pixel trigger at edge of the
	     // detector.
	     if (x[0]==x[3])
		     if (x[7]==0) gtab=11; else gtab=12;
	     else    if (y[7]==0) gtab=13; else gtab=14;
	     break;
     default:
             for (i=0; i<npix; i++) {
	          pixgeom[i]=0;
	     }
	     return(0);
  }
  for (i=0; i<npix; i++) {
	  pixgeom[i]=geomd[gtab][i];
  }

  return(retflg);

}



//
//  ------------- Pixel record extract ------------------//
//                          with hardware trigger info.
//  120707 add simple pulse-height estimater ph.
//  021508 add sample max value smax for bad data rejection
//  091108 add number of trigger pixels ntc[2]: 0=hard 1=soft
//
int pixel_dat_ht(dat, x, y, sample, pixodr, pixgeom)
unsigned int *dat;
unsigned int *x;
unsigned int *y;
unsigned int *sample;
int *pixodr;
int *pixgeom;
{ int i, j, npix, ips, ips1, idx1, idxflg, x1, y1;
  int pixall[PIXCOL_MAX][PIXROW_MAX]={0};
  float ph_l, ph_h;
  idx1=9;          // starting byte of event record.
  idxflg=0;        // flag of (column and row data) are in 12MSB or 12LSB.
  ips=0;           // ADC values array index.
  npix   = dat[3]; // number of pixels.

  if (npix > MAXPIXEL) return(-1);

  for (i=0; i<npix; i++) {
      // set pixodr, x and y
      pixodr[i] = i;
      y[i] = ((dat[idx1] & 0xfc)>> 2)*(idxflg==0) +
            (((dat[idx1] & 0xf)<<2) + ((dat[idx1+1] & 0xc0) >> 6 ))*(idxflg==1);
      x[i] = (((dat[idx1] & 0x3) << 4) + ((dat[idx1+1] & 0xf0) >> 4))*(idxflg==0) +
                (dat[idx1+1] & 0x3f) *(idxflg==1);
      if (x[i] > PIXCOL-1 || x[i] < 0 || y[i] > PIXROW-1 || y[i] < 0) return(-1);

      idx1=idx1+1+idxflg;
      idxflg=(idxflg == 0);
      // sample and hardware trigger info
      ips1=ips;
      for (j=0; j<CAPNUM; j++){
          sample[ips] = (((dat[idx1] & 0xf) << 8) + dat[idx1+1])*(idxflg==1) +
                 ((dat[idx1] << 4) + ((dat[idx1+1] & 0xf0) >> 4))*(idxflg==0);
          if (j==0) {
                  x1=x[i]+1; y1=y[i]+1;  // shift 1 pixel for adding (-1,-1) boundary 
                  if (sample[ips] & 1) {
                     pixall[x1+1][y1+1]=(pixall[x1+1][y1+1]==0)*1+((pixall[x1+1][y1+1]!=0)*pixall[x1+1][y1+1]);
                     pixall[x1+1][y1  ]=(pixall[x1+1][y1  ]<=1)*2+((pixall[x1+1][y1  ]> 1)*pixall[x1+1][y1  ]);
                     pixall[x1+1][y1-1]=(pixall[x1+1][y1-1]==0)*1+((pixall[x1+1][y1-1]!=0)*pixall[x1+1][y1-1]);
                     pixall[x1  ][y1+1]=(pixall[x1  ][y1+1]<=1)*2+((pixall[x1  ][y1+1]> 1)*pixall[x1  ][y1+1]);
                     pixall[x1  ][y1  ]=3;
                     pixall[x1  ][y1-1]=(pixall[x1  ][y1-1]<=1)*2+((pixall[x1  ][y1-1]> 1)*pixall[x1  ][y1-1]);
                     pixall[x1-1][y1+1]=(pixall[x1-1][y1+1]==0)*1+((pixall[x1-1][y1+1]!=0)*pixall[x1-1][y1+1]);
                     pixall[x1-1][y1  ]=(pixall[x1-1][y1  ]<=1)*2+((pixall[x1-1][y1  ]> 1)*pixall[x1-1][y1  ]);
                     pixall[x1-1][y1-1]=(pixall[x1-1][y1-1]==0)*1+((pixall[x1-1][y1-1]!=0)*pixall[x1-1][y1-1]);
                  }
          }
          idx1=idx1+1+idxflg;
          idxflg=(idxflg == 0);
          ips++;
      }
  }

  for (i=0; i<npix; i++) {
          pixgeom[i]=pixall[x[i]+1][y[i]+1];
  }

  return(1);

}

//
//  ------------- NuSTAR Pixel record extract ------------------//
//  052209 beta
//
//int nu_pixel_dat(dat, x, y, sample, pixodr, pixgeom, ph, smax, ntc)
int nu_pixel_dat(dat, x, y, sample, pixodr, pixgeom)
unsigned int *dat;
unsigned int *x;
unsigned int *y;
unsigned long *sample;
int *pixodr;
int *pixgeom;
//int *ph;
//int *smax;
//int *ntc;
{  int i, j, npix, ips, ips1, idx1, idxflg, x1, y1;
  int pixall[PIXCOL_MAX][PIXROW_MAX]={0};
  int col0, row0;
  float ph_l, ph_h, ph_1;
  int colx[]={-1,0,1,-1,0,1,-1,0,1};
  int rowx[]={-1,-1,-1,0,0,0,1,1,1};
  idx1=15;          // starting byte of event record.
  ips=0;            // ADC values array index.
  npix   = 9;       // number of pixels.
  //smax[0]=0;

  col0    = (dat[9] & 0xf8) >> 3;        // center pixel (column)
  row0    = (dat[12]& 0xf8) >> 3;        // center pixel (row)

  for (i=0; i<npix; i++) {
      // set pixodr, x and y
      pixodr[i] = i;
      y[i] = row0+rowx[i];
      x[i] = col0+colx[i];
      //if (x[i] > PIXCOL-1 || x[i] < 0 || y[i] > PIXROW-1 || y[i] < 0) return(-1);
      if (x[i] < PIXCOL && x[i] >= 0 && y[i] < PIXROW && y[i] >= 0) {

      // sample and hardware trigger info
      //ips1=ips;
      sample[ips  ] = (((long)(dat[idx1  ] & 0x7f) << 8) + (long)dat[idx1+1]);
      sample[ips+1] = (((long)(dat[idx1+2] & 0x7f) << 8) + (long)dat[idx1+3]);
      //if (sample[ips] > smax[0]) smax[0]=sample[ips];
      x1=x[i]+1; y1=y[i]+1;  // shift 1 pixel for adding (-1,-1) boundary 
      if ((dat[idx1] & 0x80) >>7) {
         //ntc[0]++; // increment number of Hardware trigger
         pixall[x1+1][y1+1]=(pixall[x1+1][y1+1]==0)*1+((pixall[x1+1][y1+1]!=0)*pixall[x1+1][y1+1]);
         pixall[x1+1][y1  ]=(pixall[x1+1][y1  ]<=1)*2+((pixall[x1+1][y1  ]> 1)*pixall[x1+1][y1  ]);
         pixall[x1+1][y1-1]=(pixall[x1+1][y1-1]==0)*1+((pixall[x1+1][y1-1]!=0)*pixall[x1+1][y1-1]);
         pixall[x1  ][y1+1]=(pixall[x1  ][y1+1]<=1)*2+((pixall[x1  ][y1+1]> 1)*pixall[x1  ][y1+1]);
         pixall[x1  ][y1  ]=3;
         pixall[x1  ][y1-1]=(pixall[x1  ][y1-1]<=1)*2+((pixall[x1  ][y1-1]> 1)*pixall[x1  ][y1-1]);
         pixall[x1-1][y1+1]=(pixall[x1-1][y1+1]==0)*1+((pixall[x1-1][y1+1]!=0)*pixall[x1-1][y1+1]);
         pixall[x1-1][y1  ]=(pixall[x1-1][y1  ]<=1)*2+((pixall[x1-1][y1  ]> 1)*pixall[x1-1][y1  ]);
         pixall[x1-1][y1-1]=(pixall[x1-1][y1-1]==0)*1+((pixall[x1-1][y1-1]!=0)*pixall[x1-1][y1-1]);
      }
      }
      idx1=idx1+4;
      ips=ips+2;
      //ph_l=(float)sample[ips1  ]/6.0;
      //ph_h=(float)sample[ips1+1]/6.0;
      //ph[i] = (int)(ph_h - ph_l);
      //printf("ph_h,ph_l= %f %f %f\n",ph[i],ph_h,ph_l);
  }
  for (i=0; i<npix; i++) {
          pixgeom[i]=pixall[x[i]+1][y[i]+1];
  }
  return(1);
}


//
//  ------------- MODULE Header extract ------------------//
//
int module_head(unsigned int *dat, void *m1_ptr)
{
  int i;
  struct module *m1 = m1_ptr;

  m1->frame_count   = dat[ 0]*65536l+dat[ 1]*256l+dat[ 2];
  m1->livetime      = dat[ 3]*65536l+dat[ 4]*256l+dat[ 5];
  m1->d_event_c     = dat[ 6];
  m1->s_event_c     = dat[ 7]*256l+dat[ 8];
  m1->s_reject_f    = (dat[ 9] & 0x2) >> 1;
  m1->t_pulse_f     = (dat[ 9] & 0x1);
  m1->acc_event_c   = dat[10];
  m1->rej_event_c   = dat[11];
  m1->pho_event_c   = dat[12];
  m1->sd_coin_c     = dat[13];
  m1->up_event_c    = dat[14];
  m1->tp_event_c    = dat[15];
  m1->aside_event_c = dat[16];
  m1->bside_event_c = dat[17];
  m1->both_event_c  = dat[18];
  m1->nor_event_c   = dat[19];
  m1->a_reg_val     = dat[24]*65536l+dat[25]*256l+dat[26];
  m1->aczttemp      = dat[27]*65536l+dat[28]*256l+dat[29];
  m1->bczttemp      = dat[30]*65536l+dat[31]*256l+dat[32];
  m1->misctemp      = dat[33]*65536l+dat[34]*256l+dat[35];
  m1->powtemp       = dat[36]*65536l+dat[37]*256l+dat[38];
  m1->pmttemp       = dat[39]*65536l+dat[40]*256l+dat[41];
  m1->m9V           = dat[42]*65536l+dat[43]*256l+dat[44];
  m1->mn9V          = dat[45]*65536l+dat[46]*256l+dat[47];
  m1->m5Vd          = dat[48]*65536l+dat[49]*256l+dat[50];
  m1->m51Va         = dat[51]*65536l+dat[52]*256l+dat[53];
  m1->mn5Va         = dat[54]*65536l+dat[55]*256l+dat[56];

  return(0);
}

//
//  ------------- NuSTAR housekeeping extract ------------//
//
int nu_house(unsigned int *dat, void *h1_ptr)
{
  int i;
  struct nuhouse *h1 = h1_ptr;

  h1->frame         = dat[ 0]*65536l+dat[ 1]*256l+dat[ 2];
  h1->p5dmon        = dat[ 3]*65536l+dat[ 4]*256l+dat[ 5];
  h1->p5amon        = dat[ 6]*65536l+dat[ 7]*256l+dat[ 8];
  h1->czttemp0      = dat[ 9]*65536l+dat[10]*256l+dat[11];
  h1->czttemp1      = dat[12]*65536l+dat[13]*256l+dat[14];
  h1->czttemp2      = dat[15]*65536l+dat[16]*256l+dat[17];
  h1->czttemp3      = dat[18]*65536l+dat[19]*256l+dat[20];
  h1->electemp      = dat[21]*65536l+dat[22]*256l+dat[23];
  h1->radtemp       = dat[24]*65536l+dat[25]*256l+dat[26];
  h1->pmttemp       = dat[27]*65536l+dat[28]*256l+dat[29];
  h1->shldtemp      = dat[30]*65536l+dat[31]*256l+dat[32];


  /*
   * base on NuSTAR_InstrumentTelemFormat_O 
   */
  h1->heater_det    = dat[35];
  h1->heater_shld   = dat[34];
  h1->auto_ht_det   = (dat[33] & 0x2) >> 1;
  h1->auto_ht_shld  = (dat[33] & 0x1);
  h1->cpmode        = (dat[33] & 0x4) >> 2;
  h1->lcbalance     = (dat[33] & 0x8) >> 3;

  /*
   * base on NuSTAR_InstrumentTelemFormat_K 
   *
  h1->swcounter0    = dat[33]*65536l+dat[34]*256l+dat[35];
  h1->swcounter1    = dat[36]*65536l+dat[37]*256l+dat[38];
  h1->swcounter2    = dat[39]*65536l+dat[40]*256l+dat[41];
  h1->swcounter3    = dat[42]*65536l+dat[43]*256l+dat[44];
  h1->swcounter4    = dat[45]*65536l+dat[46]*256l+dat[47];
  h1->swcounter5    = dat[48]*65536l+dat[49]*256l+dat[50];
  h1->swcounter6    = dat[51]*65536l+dat[52]*256l+dat[53];
  h1->swcounter7    = dat[54]*65536l+dat[55]*256l+dat[56];
  h1->swcounter8    = dat[57]*65536l+dat[58]*256l+dat[59];
  h1->swcounter9    = dat[60]*65536l+dat[61]*256l+dat[62];
  */

  return(0);
}

//
//  ------------- NuSTAR rate data extract --------------//
//
int nu_rate(unsigned int *dat, void *r1_ptr)
{
  int i;
  struct nurate *r1 = r1_ptr;

  r1->frame         = dat[ 0]*65536l+dat[ 1]*256l+dat[ 2];
  /* obsolute 
  r1->czt0          = dat[ 3]*65536l+dat[ 4]*256l+dat[ 5];
  r1->czt1          = dat[ 6]*65536l+dat[ 7]*256l+dat[ 8];
  r1->czt2          = dat[ 9]*65536l+dat[10]*256l+dat[11];
  r1->czt3          = dat[12]*65536l+dat[13]*256l+dat[14];
  r1->shieldlo      = dat[15]*65536l+dat[16]*256l+dat[17];
  r1->shieldhi      = dat[18]*65536l+dat[19]*256l+dat[20];
  */
  r1->shldlo        = dat[ 3]*65536l+dat[ 4]*256l+dat[ 5];
  r1->shldhi        = dat[ 6]*65536l+dat[ 7]*256l+dat[ 8];
  r1->livetime      = dat[ 9]*65536l+dat[10]*256l+dat[11];
  r1->evtime        = dat[12]*65536l+dat[13]*256l+dat[14];
  r1->nev           = dat[15]*65536l+dat[16]*256l+dat[17];
  r1->naccept       = dat[18]*65536l+dat[19]*256l+dat[20];
  r1->nreject       = dat[21]*65536l+dat[22]*256l+dat[23];
  r1->nczta_a       = dat[24]*65536l+dat[25]*256l+dat[26];
  r1->ncztb_a       = dat[27]*65536l+dat[28]*256l+dat[29];
  r1->ncztc_a       = dat[30]*65536l+dat[31]*256l+dat[32];
  r1->ncztd_a       = dat[33]*65536l+dat[34]*256l+dat[35];
  r1->nshlo_a       = dat[36]*65536l+dat[37]*256l+dat[38];
  r1->nshhi_a       = dat[39]*65536l+dat[40]*256l+dat[41];
  r1->nuptrig_a     = dat[42]*65536l+dat[43]*256l+dat[44];
  r1->ntptrig_a     = dat[45]*65536l+dat[46]*256l+dat[47];
  r1->nczta_r       = dat[48]*65536l+dat[49]*256l+dat[50];
  r1->ncztb_r       = dat[51]*65536l+dat[52]*256l+dat[53];
  r1->ncztc_r       = dat[54]*65536l+dat[55]*256l+dat[56];
  r1->ncztd_r       = dat[57]*65536l+dat[58]*256l+dat[59];
  r1->nshlo_r       = dat[60]*65536l+dat[61]*256l+dat[62];
  r1->nshhi_r       = dat[63]*65536l+dat[64]*256l+dat[65];
  r1->nuptrig_r     = dat[66]*65536l+dat[67]*256l+dat[68];
  r1->ntptrig_r     = dat[69]*65536l+dat[70]*256l+dat[71];

  return(0);
}


//
//  ------------- Nu leakage current extract -------------//
//
int nu_leak(unsigned int *dat, void *l1_ptr, unsigned long  *l1_row)
{
  int i;
  struct nuleak *l1 = l1_ptr;

  l1->frame         = (dat[ 0] << 8) + (dat[ 1]);
  l1->cpsel         = (dat[ 2] & 0x80) >> 7;
  l1->det           = (dat[ 2] & 0x60) >> 5;
  l1->row           = (dat[ 2] & 0x1f) ;

  for (i=0;i<16;i++) {
      l1_row[(i*2)  ] = (dat[(i*3)+3] << 4) + ((dat[(i*3)+4] & 0xf0) >> 4);
      l1_row[(i*2)+1] =((dat[(i*3)+4] & 0x0f) << 8) + dat[(i*3)+5];
  }
  
  return(0);
}

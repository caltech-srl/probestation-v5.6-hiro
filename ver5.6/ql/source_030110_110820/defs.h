
#define CAPNUM 16		//
#define MAXPIXEL 50		// Max pixel for each event
#define MAXSAMPLE 2000		//
#define MAXBYTE 2000		// 
#define MAXEVENT 13000000l	// Max event to process
//#define MAXEVENT 130l	// Max event to process
//
#define PIXCOL_MAX 100          // Max column number for future reserve.
#define PIXROW_MAX 100          // Max row number for future reserve.
#define PHRNG   6               // range to estimate ph.


#define EVENT     1
#define FRAME     2
#define MODULE    3
#define NU_EVENT  4
#define NU_LEAK   5
#define NU_HOUSE  6
#define NU_RATE   7

#ifdef HEFT
  #define PIXCOL 48
  #define PIXROW 24
#endif
#ifdef DB
  #define PIXCOL 32
  #define PIXROW 32
#endif
#define ADCMAX 8000

#define EVENT_HEADER_BYTE 9
#define EVENT_HEADER_BYTE_long 9l
#define MODULE_HEADER_BYTE 57
#define NU_EVENT_BYTE 51
#define NU_LEAK_BYTE 51
#define NU_HOUSE_BYTE 36
#define NU_RATE_BYTE 72

// Format type
#define D_FORMAT 1   // DB,HEFT2 format
#define N_FORMAT 2   // NuSTAR format

FILE *fqdat,*fqout1;
FILE *fqout_up,*fqout_tp;
FILE *fqout_mo;



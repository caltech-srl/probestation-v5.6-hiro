
struct event
{
	unsigned long time;
	int npix;
	unsigned long treset;
	int side;
	int tpulse;
	int upevent;
	int shevent;
	int second;
	int scap;
	unsigned long frame;
	unsigned long subframe;
	unsigned long ttag;
	unsigned long tags;
	int didscap;
        unsigned long pixel;            // Index of pixel struct. Points last pixel of event.
	// added on 05-2009 for nustar flight format
	int ftyp;
	int nht;
        unsigned long tevt;
	int col;
	int row;
	unsigned long tr_num;
	unsigned long tr_den;
};

struct module
{
  	unsigned long frame_count;
  	unsigned long livetime;
  	unsigned int  d_event_c;
  	unsigned long s_event_c;
  	unsigned int  s_reject_f;
  	unsigned int  t_pulse_f;
  	unsigned int  acc_event_c;
  	unsigned int  rej_event_c;
  	unsigned int  pho_event_c;
  	unsigned int  sd_coin_c;
  	unsigned int  up_event_c;
  	unsigned int  tp_event_c;
  	unsigned int  aside_event_c;
  	unsigned int  bside_event_c;
  	unsigned int  both_event_c;
  	unsigned int  nor_event_c;
  	unsigned long a_reg_val;
  	unsigned long b_reg_val;
  	unsigned long aczttemp;
  	unsigned long bczttemp;
  	unsigned long misctemp;
  	unsigned long powtemp;
  	unsigned long pmttemp;
  	unsigned long m9V;
  	unsigned long mn9V;
  	unsigned long m5Vd;
  	unsigned long m51Va;
  	unsigned long mn5Va;
};


struct nuleak
{
  	unsigned long frame;
	int cpsel;
  	int det;
  	int row;
};


struct nuhouse
{
        unsigned long frame;
        unsigned long p5dmon;
        unsigned long p5amon;
        unsigned long czttemp0;
        unsigned long czttemp1;
        unsigned long czttemp2;
        unsigned long czttemp3;
        unsigned long electemp;
        unsigned long radtemp;
        unsigned long pmttemp;
        unsigned long shldtemp;

        unsigned int  heater_det;
        unsigned int  heater_shld;
        unsigned int  auto_ht_det;
        unsigned int  auto_ht_shld;
        unsigned int  cpmode;
        unsigned int  lcbalance;

        /*
        unsigned int  swcounter0;
        unsigned int  swcounter1;
        unsigned int  swcounter2;
        unsigned int  swcounter3;
        unsigned int  swcounter4;
        unsigned int  swcounter5;
        unsigned int  swcounter6;
        unsigned int  swcounter7;
        unsigned int  swcounter8;
        unsigned int  swcounter9;
        */
};

struct nurate
{
        unsigned long frame;
        /* obsolute
        unsigned long czt0;
        unsigned long czt1;
        unsigned long czt2;
        unsigned long czt3;
        */
        unsigned long shldlo;
        unsigned long shldhi;
        unsigned long livetime;
        unsigned long evtime;
        unsigned long nev;
        unsigned long naccept;
        unsigned long nreject;
        unsigned long nczta_a;
        unsigned long ncztb_a;
        unsigned long ncztc_a;
        unsigned long ncztd_a;
        unsigned long nshlo_a;
        unsigned long nshhi_a;
        unsigned long nuptrig_a;
        unsigned long ntptrig_a;
        unsigned long nczta_r;
        unsigned long ncztb_r;
        unsigned long ncztc_r;
        unsigned long ncztd_r;
        unsigned long nshlo_r;
        unsigned long nshhi_r;
        unsigned long nuptrig_r;
        unsigned long ntptrig_r;
};


/* 
 * define data structures
 *
 * 10/07/09
 *
 */

struct dmvs
{
	char   pname[256];      // probe point name
	int    pname_len;       // probe point name length
	int    act_flg;         // action flag. 0=range
	float  rng_min;         // range min.
	float  rng_max;         // range max.
	float  meas;            // actual measured value
	int    status;          // test result. fit into range=1 out of range=0
	int    sd_act_flg;      // STDEV action flag. 0=range
	float  sd_rng_min;      // STDEV range min.
	float  sd_rng_max;      // STDEV range max.
	float  stdev;           // STDEV of measuerments
	int    sd_status;       // DAC test result. fit into range=1 out of range=0
};


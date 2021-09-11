#pragma once
#include "OutputWriter.h"

class OutputWriterSat : public OutputWriter
{
private:	
	typedef struct
	{
		double tow;
		int version;
		int numSv;

		int gnss_id = -1;	/* Satellite system identification number (0=GPS,1=SBAS,2=Galileo,3=BeiDou,4=IMES,5=GZSS,6=GLONASS)*/
		int	sv_id = -1;		/* Space vehicle identification number */
		int cno = 0;
		double elevation;
		double azimuth;
		double residual;
		
		// various flags
		int quality;
		int used;
		int health;
		int diff;
		int smoothed;
		int orbit_source;
		int eph_available;
		int alm_available;
		int ano_available;
		int aop_available;
		int sbas_corr_used;
		int rtcm_corr_used;
		int slas_corr_used;
		int pr_corr_used;
		int cr_corr_used;
		int do_corr_used;
	} SAT;

	int epoch_index;

	void store_and_write_SAT(char * inbuffer);
	void write_message(OutputWriterSat::SAT my_data);
	UbloxSpecs::UBX_HEAD get_reference_header();
	void open_file(std::string file_name);

public:
	OutputWriterSat();
	~OutputWriterSat();
	void parse_and_write(char * inbuffer);
	
};


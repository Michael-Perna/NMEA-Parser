#pragma once
#include "OutputWriter.h"

class OutputWriterSignal : public OutputWriter
{
private:
	typedef struct
	{
		double tow;
		int version;
		int numSig;

		int gnss_id = -1; /* SIGNALellite system identification number (0=GPS,1=SBAS,2=Galileo,3=BeiDou,4=IMES,5=GZSS,6=GLONASS)*/
		int	sv_id = -1; /* Space vehicle identification number */
		int sig_id = -1;
		int freq_id = -1; /* only for Glonass */
	
		double residual;

		// various flags
		int quality;
		int correction_source;
		int iono_model;
		int health;

		int pseudorange_smoothed;
		int pseudorange_used;

		int carrier_used;
		int doppler_used;
		
		int pr_corr_used; // also present in NAV-SAT
		int cr_corr_used; // also present in NAV-SAT
		int do_corr_used; // also present in NAV-SAT

	} SIGNAL;

	int epoch_index;

	void store_and_write_SIGNAL(char * inbuffer);
	void write_message(OutputWriterSignal::SIGNAL my_data);
	UbloxSpecs::UBX_HEAD get_reference_header();
	void open_file(std::string file_name);

public:
	OutputWriterSignal();
	~OutputWriterSignal();
	void parse_and_write(char * inbuffer);

};


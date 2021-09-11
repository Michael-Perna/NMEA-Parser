#pragma once
#include "OutputWriter.h"

class OutputWriterStatus : public OutputWriter
{
private:
	typedef struct
	{
		double tow; // s
		int gps_fix;
		int gps_fix_ok;
		int diffetential_sol;
		int weeknumber_set;
		int tow_set;
		int differential_corr_applied;
		int carr_sol_valid;
		int map_matching_status;
		int power_safe_mode_status;
		int spoofing_detection_state;
		int carr_sol_status;
		double time_to_first_fix; // s
		double time_since_startup; // s
	} STATUS;

	OutputWriterStatus::STATUS store_STATUS(char * inbuffer);
	void write_message(OutputWriterStatus::STATUS my_data);
	UbloxSpecs::UBX_HEAD get_reference_header();
	void open_file(std::string file_name);

public:
	OutputWriterStatus();
	~OutputWriterStatus();
	void parse_and_write(char * inbuffer);
};


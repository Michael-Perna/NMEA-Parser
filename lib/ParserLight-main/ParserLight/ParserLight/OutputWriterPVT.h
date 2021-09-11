#pragma once
#include "OutputWriter.h"

class OutputWriterPVT : public OutputWriter
{
private:	
	typedef struct
	{
		double tow;
		int year;
		int month;
		int day;
		int hour;
		int min;
		double sec;
		int validDate;
		int validTime;
		int UTC_resolved;
		int validMag;
		double time_accuracy;
		int fix_type;
		int gnssFixOK;
		int diffSoln;
		int psmState;
		int head_vehicle_valid;
		int carr_sol_status;
		int UTC_validity_available;
		int date_confirmed;
		int time_confirmed;
		int num_SV;
		double lon;
		double lat;
		double height;
		double hMSL;
		double accuracy_horizontal;
		double accuracy_vertical;
		double velocity_n;
		double velocity_e;
		double velocity_d;
		double ground_speed;
		double heading_of_motion;
		double accuracy_speed;
		double accuracy_heading;
		double PDOP;
		int invalid_llh;
		double heding_of_vehicle;
		double magnetic_declination;
		double accuracy_declination;
	} PVT;

	OutputWriterPVT::PVT store_HPPVT(char * inbuffer);
	void write_message(OutputWriterPVT::PVT my_data);
	UbloxSpecs::UBX_HEAD get_reference_header();
	void open_file(std::string file_name);

public:
	OutputWriterPVT();
	~OutputWriterPVT();
	void parse_and_write(char * inbuffer);
	

};


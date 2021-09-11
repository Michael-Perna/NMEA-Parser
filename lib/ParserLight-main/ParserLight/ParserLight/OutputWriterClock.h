#pragma once
#include "OutputWriter.h"

class OutputWriterClock : public OutputWriter
{
private:	
	typedef struct
	{
		double tow; // ms
		double clock_bias; // ns
		double clock_drift; // ns s-1
		double time_accuracy; // ns
		double frequency_accuracy; // ps s-1
	} CLOCK;

	OutputWriterClock::CLOCK store_CLOCK(char * inbuffer);
	void write_message(OutputWriterClock::CLOCK my_data);
	UbloxSpecs::UBX_HEAD get_reference_header();
	void open_file(std::string file_name);

public:
	OutputWriterClock();
	~OutputWriterClock();
	void parse_and_write(char * inbuffer);
};


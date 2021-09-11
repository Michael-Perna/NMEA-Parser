#pragma once
#include "OutputWriter.h"

class OutputWriterPos : public OutputWriter
{
private:	
	typedef struct
	{
		double tow;
		double lon;
		double lat;
		double height;
		double hmsl;
		double hz_acc;
		double v_acc;
	} POSLLH;

	OutputWriterPos::POSLLH store_HPPOSLLH(char * inbuffer);
	void write_message(OutputWriterPos::POSLLH my_data);
	UbloxSpecs::UBX_HEAD get_reference_header();
	void open_file(std::string file_name);

public:
	OutputWriterPos();
	~OutputWriterPos();
	void parse_and_write(char * inbuffer);
	

};


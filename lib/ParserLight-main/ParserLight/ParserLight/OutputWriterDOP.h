#pragma once
#include "OutputWriter.h"

class OutputWriterDOP : public OutputWriter
{
private:	
	typedef struct
	{
		double tow;
		double GDOP;
		double PDOP;
		double TDOP;
		double VDOP;
		double HDOP;
		double NDOP;
		double EDOP;
	} DOP;

	OutputWriterDOP::DOP store_DOP(char * inbuffer);
	void write_message(OutputWriterDOP::DOP my_data);
	UbloxSpecs::UBX_HEAD get_reference_header();
	void open_file(std::string file_name);

public:
	OutputWriterDOP();
	~OutputWriterDOP();
	void parse_and_write(char * inbuffer);
};


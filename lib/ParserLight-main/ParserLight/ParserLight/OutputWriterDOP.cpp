#include "stdafx.h"
#include "OutputWriterDOP.h"
#include "UbloxSpecs.h"
#include <iostream>
#include <cstring>
#include <cmath>


OutputWriterDOP::OutputWriterDOP()
{

}


OutputWriterDOP::~OutputWriterDOP()
{

}

void OutputWriterDOP::parse_and_write(char *inbuffer)
{
	OutputWriterDOP::DOP my_data;
	my_data = store_DOP(inbuffer);

	// skip if rate option was chosen
	if (OutputWriter::rate > 0)
	{
		int divisor = (int)round(my_data.tow / rate);
		double reminder = my_data.tow - divisor * rate;
		if (fabs(reminder) > OutputWriter::threshold)
		{
			return;
		}
	}

	write_message(my_data);
}

OutputWriterDOP::DOP OutputWriterDOP::store_DOP(char *inbuffer)
{
	UbloxSpecs::UBX_HEAD ub_head = get_reference_header();
	UbloxSpecs::UBX_NAV_DOP my_dop;
	DOP temp;

	unsigned char head[4] = { 0xb5, 0x62, ub_head.cl, ub_head.id }; // header

	char inhead[4] = { inbuffer[0], inbuffer[1], inbuffer[2], inbuffer[3] };

	if (memcmp(&head, &inhead, 4) == 0)
	{
		ub_head.cl = inbuffer[2];
		ub_head.id = inbuffer[3];
		memcpy(&ub_head.len, &inbuffer[4], 2);	//length

		// READ
		memcpy(&my_dop.iTOW, &inbuffer[6], 4);
		memcpy(&my_dop.gDOP, &inbuffer[10], 2);
		memcpy(&my_dop.pDOP, &inbuffer[12], 2);
		memcpy(&my_dop.tDOP, &inbuffer[14], 2);
		memcpy(&my_dop.vDOP, &inbuffer[16], 2);
		memcpy(&my_dop.hDOP, &inbuffer[18], 2);
		memcpy(&my_dop.nDOP, &inbuffer[20], 2);
		memcpy(&my_dop.eDOP, &inbuffer[22], 2);

		temp.tow = 1e-3*my_dop.iTOW;
		temp.GDOP = 1.0e-2*my_dop.gDOP;
		temp.PDOP = 1.0e-2*my_dop.pDOP;
		temp.TDOP = 1.0e-2*my_dop.tDOP;
		temp.VDOP = 1.0e-2*my_dop.vDOP;
		temp.HDOP = 1.0e-2*my_dop.hDOP;
		temp.NDOP = 1.0e-2*my_dop.nDOP;
		temp.EDOP = 1.0e-2*my_dop.eDOP;
	}

	return temp;
}

void OutputWriterDOP::write_message(OutputWriterDOP::DOP my_data) {
	char buffer[2000]; // buffer aimed to write the file
	// epoch
	sprintf(buffer, "%10.3f; ", my_data.tow);
	file << buffer;

	// DOPs
	sprintf(buffer, "%6.2f; ", my_data.GDOP);
	file << buffer;
	sprintf(buffer, "%6.2f; ", my_data.PDOP);
	file << buffer;
	sprintf(buffer, "%6.2f; ", my_data.TDOP);
	file << buffer;
	sprintf(buffer, "%6.2f; ", my_data.VDOP);
	file << buffer;
	sprintf(buffer, "%6.2f; ", my_data.HDOP);
	file << buffer;
	sprintf(buffer, "%6.2f; ", my_data.NDOP);
	file << buffer;
	sprintf(buffer, "%6.2f; ", my_data.EDOP);
	file << buffer;

	sprintf(buffer, "\n");
	file << buffer;
}

UbloxSpecs::UBX_HEAD OutputWriterDOP::get_reference_header()
{
	UbloxSpecs::UBX_HEAD my_head;
	my_head.cl = 0x01;
	my_head.id = 0x04;
	return my_head;
}

void OutputWriterDOP::open_file(std::string file_name)
{
	file_name = file_name + ".dop";
	file.open(file_name);
	return;
}
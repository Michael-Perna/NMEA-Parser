#include "stdafx.h"
#include "OutputWriterPos.h"
#include "UbloxSpecs.h"
#include <iostream>
#include <cstring>
#include <cmath>


OutputWriterPos::OutputWriterPos()
{

}


OutputWriterPos::~OutputWriterPos()
{

}

void OutputWriterPos::parse_and_write(char *inbuffer)
{
	OutputWriterPos::POSLLH my_data;
	my_data = store_HPPOSLLH(inbuffer);

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

OutputWriterPos::POSLLH OutputWriterPos::store_HPPOSLLH(char *inbuffer)
{
	UbloxSpecs::UBX_HEAD ub_head;
	ub_head.cl = 0x01; // HPPOSLLH class
	ub_head.id = 0x14; // HPPOSLLH id

	UbloxSpecs::UBX_NAV_HPPOSLLH my_hpposllh;
	POSLLH temp;

	unsigned char head[4] = { 0xb5, 0x62, ub_head.cl, ub_head.id }; // header

	char inhead[4] = { inbuffer[0], inbuffer[1], inbuffer[2], inbuffer[3] };

	if (memcmp(&head, &inhead, 4) == 0)
	{
		ub_head.cl = inbuffer[2];
		ub_head.id = inbuffer[3];
		memcpy(&ub_head.len, &inbuffer[4], 2);	//length

		// READ
		memcpy(&my_hpposllh.version, &inbuffer[6], 1);
		memcpy(&my_hpposllh.reserved1, &inbuffer[7], 2);
		memcpy(&my_hpposllh.validity, &inbuffer[9], 1);
		memcpy(&my_hpposllh.iTOW, &inbuffer[10], 4);
		memcpy(&my_hpposllh.lon, &inbuffer[14], 4);
		memcpy(&my_hpposllh.lat, &inbuffer[18], 4);
		memcpy(&my_hpposllh.height, &inbuffer[22], 4);
		memcpy(&my_hpposllh.hMSL, &inbuffer[26], 4);
		memcpy(&my_hpposllh.lonHp, &inbuffer[30], 1);
		memcpy(&my_hpposllh.latHp, &inbuffer[31], 1);
		memcpy(&my_hpposllh.heightHp, &inbuffer[32], 1);
		memcpy(&my_hpposllh.hMSLHp, &inbuffer[33], 1);
		memcpy(&my_hpposllh.hAcc, &inbuffer[34], 4);
		memcpy(&my_hpposllh.hVacc, &inbuffer[38], 4);

		temp.tow = my_hpposllh.iTOW / 1000.0;
		temp.lon = 1.0e-7*my_hpposllh.lon + 1.0e-9*my_hpposllh.lonHp;
		temp.lat = 1.0e-7*my_hpposllh.lat + 1.0e-9*my_hpposllh.latHp;
		temp.height = 1.0e-3*(my_hpposllh.height + 0.1*my_hpposllh.heightHp);
		temp.hmsl = 1.0e-3*(my_hpposllh.hMSL + 0.1*my_hpposllh.hMSLHp);
		temp.hz_acc = 1.0e-4*my_hpposllh.hAcc;
		temp.v_acc = 1.0e-4*my_hpposllh.hVacc;
	}

	return temp;
}

void OutputWriterPos::write_message(OutputWriterPos::POSLLH my_data) {
	char buffer[2000]; // buffer aimed to write the file
	// epoch
	sprintf(buffer, "%10.3f; ", my_data.tow);
	file << buffer;

	// lat long height
	sprintf(buffer, "%14.9f; ", my_data.lon);
	file << buffer;
	sprintf(buffer, "%14.9f; ", my_data.lat);
	file << buffer;
	sprintf(buffer, "%14.4f; ", my_data.height);
	file << buffer;
	sprintf(buffer, "%14.4f; ", my_data.hmsl);
	file << buffer;

	// accuracies
	sprintf(buffer, "%8.4f; ", my_data.hz_acc);
	file << buffer;
	sprintf(buffer, "%8.4f\n", my_data.v_acc);
	file << buffer;
}

UbloxSpecs::UBX_HEAD OutputWriterPos::get_reference_header()
{
	UbloxSpecs::UBX_HEAD my_head;
	my_head.cl = 0x01;
	my_head.id = 0x14;
	return my_head;
}

void OutputWriterPos::open_file(std::string file_name)
{
	file_name = file_name + ".pos";
	file.open(file_name);
	return;
}
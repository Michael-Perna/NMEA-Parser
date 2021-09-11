#include "stdafx.h"
#include "OutputWriterClock.h"
#include "UbloxSpecs.h"
#include <iostream>
#include <cstring>
#include <cmath>


OutputWriterClock::OutputWriterClock()
{

}


OutputWriterClock::~OutputWriterClock()
{

}

void OutputWriterClock::parse_and_write(char *inbuffer)
{
	OutputWriterClock::CLOCK my_data;
	my_data = store_CLOCK(inbuffer);

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

OutputWriterClock::CLOCK OutputWriterClock::store_CLOCK(char *inbuffer)
{
	UbloxSpecs::UBX_HEAD ub_head = get_reference_header();

	UbloxSpecs::UBX_NAV_CLOCK my_clock;
	CLOCK temp;

	unsigned char head[4] = { 0xb5, 0x62, ub_head.cl, ub_head.id }; // header

	char inhead[4] = { inbuffer[0], inbuffer[1], inbuffer[2], inbuffer[3] };

	if (memcmp(&head, &inhead, 4) == 0)
	{
		ub_head.cl = inbuffer[2];
		ub_head.id = inbuffer[3];
		memcpy(&ub_head.len, &inbuffer[4], 2);	//length

		// READ
		memcpy(&my_clock.iTOW, &inbuffer[6], 4);
		memcpy(&my_clock.clkB, &inbuffer[10], 4);
		memcpy(&my_clock.cldD, &inbuffer[14], 4);
		memcpy(&my_clock.tAcc, &inbuffer[18], 4);
		memcpy(&my_clock.fAcc, &inbuffer[22], 4);


		temp.tow = 1e-3*my_clock.iTOW;
		temp.clock_bias = 1e-9*my_clock.clkB;
		temp.clock_drift = 1e-9*my_clock.cldD;
		temp.time_accuracy = 1e-9*my_clock.tAcc;
		temp.frequency_accuracy = 1e-12*my_clock.fAcc;
	}

	return temp;
}

void OutputWriterClock::write_message(OutputWriterClock::CLOCK my_data) {
	char buffer[2000]; // buffer aimed to write the file
	// epoch
	sprintf(buffer, "%10.3f; ", my_data.tow);
	file << buffer;

	// drift and bias
	sprintf(buffer, "%11.9f; ", my_data.clock_bias);
	file << buffer;
	sprintf(buffer, "%11.9f; ", my_data.clock_drift);
	file << buffer;

	// accuracies
	sprintf(buffer, "%11.9f; ", my_data.time_accuracy);
	file << buffer;
	sprintf(buffer, "%14.12f; ", my_data.frequency_accuracy);
	file << buffer;

	sprintf(buffer, "\n");
	file << buffer;
}

UbloxSpecs::UBX_HEAD OutputWriterClock::get_reference_header()
{
	UbloxSpecs::UBX_HEAD my_head;
	my_head.cl = 0x01;
	my_head.id = 0x22;
	return my_head;
}

void OutputWriterClock::open_file(std::string file_name)
{
	file_name = file_name + ".clock";
	file.open(file_name);
	return;
}
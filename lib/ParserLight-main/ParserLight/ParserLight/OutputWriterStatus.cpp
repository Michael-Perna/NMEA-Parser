#include "stdafx.h"
#include "OutputWriterStatus.h"
#include "UbloxSpecs.h"
#include <iostream>
#include <cstring>
#include <cmath>


OutputWriterStatus::OutputWriterStatus()
{

}


OutputWriterStatus::~OutputWriterStatus()
{

}

void OutputWriterStatus::parse_and_write(char *inbuffer)
{
	OutputWriterStatus::STATUS my_data;
	my_data = store_STATUS(inbuffer);

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

OutputWriterStatus::STATUS OutputWriterStatus::store_STATUS(char *inbuffer)
{
	UbloxSpecs::UBX_HEAD ub_head = get_reference_header();
	UbloxSpecs::UBX_NAV_STATUS my_STATUS;
	STATUS temp;

	unsigned char head[4] = { 0xb5, 0x62, ub_head.cl, ub_head.id }; // header

	char inhead[4] = { inbuffer[0], inbuffer[1], inbuffer[2], inbuffer[3] };

	if (memcmp(&head, &inhead, 4) == 0)
	{
		ub_head.cl = inbuffer[2];
		ub_head.id = inbuffer[3];
		memcpy(&ub_head.len, &inbuffer[4], 2);	//length

		// READ
		memcpy(&my_STATUS.iTOW, &inbuffer[6], 4);
		memcpy(&my_STATUS.gpsFix, &inbuffer[10], 1);
		memcpy(&my_STATUS.flags, &inbuffer[11], 1);
		memcpy(&my_STATUS.fixStat, &inbuffer[12], 1);
		memcpy(&my_STATUS.flags2, &inbuffer[13], 1);
		memcpy(&my_STATUS.ttff, &inbuffer[14], 4);
		memcpy(&my_STATUS.msss, &inbuffer[18], 4);
		

		temp.tow = 1e-3*my_STATUS.iTOW;
		temp.gps_fix = my_STATUS.gpsFix;
		temp.gps_fix_ok = my_STATUS.flags.gpsFixOk;
		temp.diffetential_sol = my_STATUS.flags.diffSoln;
		temp.weeknumber_set = my_STATUS.flags.wknSet;
		temp.tow_set = my_STATUS.flags.towSet;
		temp.differential_corr_applied = my_STATUS.fixStat.diffCorr;
		temp.carr_sol_valid = my_STATUS.fixStat.carrSolnValid;
		temp.map_matching_status = my_STATUS.fixStat.mapMatching;
		temp.power_safe_mode_status = my_STATUS.flags2.psmState;
		temp.spoofing_detection_state = my_STATUS.flags2.spoofDetState;
		temp.carr_sol_status = my_STATUS.flags2.carrSoln;
		temp.time_to_first_fix = 1e-3*my_STATUS.ttff;
		temp.time_since_startup = 1e-3*my_STATUS.msss;
	}

	return temp;
}

void OutputWriterStatus::write_message(OutputWriterStatus::STATUS my_data) {
	char buffer[2000]; // buffer aimed to write the file
	// epoch
	sprintf(buffer, "%10.3f; ", my_data.tow);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.gps_fix);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.gps_fix_ok);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.diffetential_sol);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.weeknumber_set);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.tow_set);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.differential_corr_applied);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.carr_sol_valid);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.map_matching_status);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.power_safe_mode_status);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.spoofing_detection_state);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.carr_sol_status);
	file << buffer;
	sprintf(buffer, "%5.3f; ", my_data.time_to_first_fix);
	file << buffer;
	sprintf(buffer, "%7.3f; ", my_data.time_since_startup);
	file << buffer;

	sprintf(buffer, "\n");
	file << buffer;
}

UbloxSpecs::UBX_HEAD OutputWriterStatus::get_reference_header()
{
	UbloxSpecs::UBX_HEAD my_head;
	my_head.cl = 0x01;
	my_head.id = 0x03;
	return my_head;
}

void OutputWriterStatus::open_file(std::string file_name)
{
	file_name = file_name + ".status";
	file.open(file_name);
	return;
}
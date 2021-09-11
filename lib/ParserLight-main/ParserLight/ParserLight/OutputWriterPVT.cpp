#include "stdafx.h"
#include "OutputWriterPVT.h"
#include "UbloxSpecs.h"
#include <iostream>
#include <cstring>
#include <cmath>


OutputWriterPVT::OutputWriterPVT()
{

}


OutputWriterPVT::~OutputWriterPVT()
{

}

void OutputWriterPVT::parse_and_write(char *inbuffer)
{
	OutputWriterPVT::PVT my_data;
	my_data = store_HPPVT(inbuffer);

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

OutputWriterPVT::PVT OutputWriterPVT::store_HPPVT(char *inbuffer)
{
	UbloxSpecs::UBX_HEAD ub_head = get_reference_header();
	UbloxSpecs::UBX_NAV_PVT my_PVT;
	PVT temp;

	unsigned char head[4] = { 0xb5, 0x62, ub_head.cl, ub_head.id }; // header

	char inhead[4] = { inbuffer[0], inbuffer[1], inbuffer[2], inbuffer[3] };

	if (memcmp(&head, &inhead, 4) == 0)
	{
		ub_head.cl = inbuffer[2];
		ub_head.id = inbuffer[3];
		memcpy(&ub_head.len, &inbuffer[4], 2);	//length

		// READ
		memcpy(&my_PVT.iTOW, &inbuffer[6], 4);
		memcpy(&my_PVT.year, &inbuffer[10], 2);
		memcpy(&my_PVT.month, &inbuffer[12], 1);
		memcpy(&my_PVT.day, &inbuffer[13], 1);
		memcpy(&my_PVT.hour, &inbuffer[14], 1);
		memcpy(&my_PVT.min, &inbuffer[15], 1);
		memcpy(&my_PVT.sec, &inbuffer[16], 1);
		memcpy(&my_PVT.valid, &inbuffer[17], 1);
		memcpy(&my_PVT.tAcc, &inbuffer[18], 4);
		memcpy(&my_PVT.nano, &inbuffer[22], 4);
		memcpy(&my_PVT.fixType, &inbuffer[26], 1);
		memcpy(&my_PVT.fix_status_flag, &inbuffer[27], 1);
		memcpy(&my_PVT.add_flags, &inbuffer[28], 1);
		memcpy(&my_PVT.numSV, &inbuffer[29], 1);
		memcpy(&my_PVT.lon, &inbuffer[30], 4);
		memcpy(&my_PVT.lat, &inbuffer[34], 4);
		memcpy(&my_PVT.height, &inbuffer[38], 4);
		memcpy(&my_PVT.hMSL, &inbuffer[42], 4);
		memcpy(&my_PVT.hAcc, &inbuffer[46], 4);
		memcpy(&my_PVT.vAcc, &inbuffer[50], 4);
		memcpy(&my_PVT.velN, &inbuffer[54], 4);
		memcpy(&my_PVT.velE, &inbuffer[58], 4);
		memcpy(&my_PVT.velD, &inbuffer[62], 4);
		memcpy(&my_PVT.gSpeed, &inbuffer[66], 4);
		memcpy(&my_PVT.headMot, &inbuffer[70], 4);
		memcpy(&my_PVT.sAcc, &inbuffer[74], 4);
		memcpy(&my_PVT.headAcc, &inbuffer[78], 4);
		memcpy(&my_PVT.pDOP, &inbuffer[82], 2);
		memcpy(&my_PVT.invalidLlh, &inbuffer[84], 1);
		memcpy(&my_PVT.headVeh, &inbuffer[90], 4);
		memcpy(&my_PVT.magDec, &inbuffer[94], 2);
		memcpy(&my_PVT.magAcc, &inbuffer[96], 2);


		// mapping
		temp.tow = 1e-3*my_PVT.iTOW;
		temp.year = my_PVT.year;
		temp.month = my_PVT.month;
		temp.day = my_PVT.day;
		temp.hour = my_PVT.hour;
		temp.min = my_PVT.min;
		temp.sec = my_PVT.sec + 1.0e-9*my_PVT.nano;
		temp.validDate = my_PVT.valid.validDate;
		temp.validTime = my_PVT.valid.validTime;
		temp.UTC_resolved = my_PVT.valid.fullyResolved;
		temp.validMag = my_PVT.valid.validMag;
		temp.time_accuracy = 1e-9*my_PVT.tAcc;
		temp.fix_type = my_PVT.fixType;
		temp.gnssFixOK = my_PVT.fix_status_flag.gnssFixOK;
		temp.diffSoln = my_PVT.fix_status_flag.diffSoln;
		temp.psmState = my_PVT.fix_status_flag.psmState;;
		temp.head_vehicle_valid = my_PVT.fix_status_flag.headVehValid;
		temp.carr_sol_status = my_PVT.fix_status_flag.carrSoln;
		temp.UTC_validity_available = my_PVT.add_flags.confirmedAvai;
		temp.date_confirmed = my_PVT.add_flags.confirmedDate;
		temp.time_confirmed = my_PVT.add_flags.confirmedTime;
		temp.num_SV = my_PVT.numSV;
		temp.lon = 1.0e-7*my_PVT.lon;
		temp.lat = 1.0e-7*my_PVT.lat;
		temp.height = 1.0e-3*my_PVT.height;
		temp.hMSL = 1.0e-3*my_PVT.hMSL;
		temp.accuracy_horizontal = 1.0e-3*my_PVT.hAcc;
		temp.accuracy_vertical = 1.0e-3*my_PVT.vAcc;
		temp.velocity_n = 1.0e-3*my_PVT.velN;
		temp.velocity_e = 1.0e-3*my_PVT.velE;
		temp.velocity_d = 1.0e-3*my_PVT.velD;
		temp.ground_speed = 1.0e-3*my_PVT.gSpeed;
		temp.heading_of_motion = 1.0e-5*my_PVT.headMot;
		temp.accuracy_speed = 1.0e-3*my_PVT.sAcc;
		temp.accuracy_heading = 1.0e-5*my_PVT.headAcc;
		temp.PDOP = 0.01*my_PVT.pDOP;
		temp.invalid_llh = my_PVT.invalidLlh.valid;
		temp.heding_of_vehicle = 1.0e-5*my_PVT.headVeh;
		temp.magnetic_declination = 1.0e-2*my_PVT.magDec;
		temp.magnetic_declination = 1.0e-2*my_PVT.magAcc;
	}

	return temp;
}

void OutputWriterPVT::write_message(OutputWriterPVT::PVT my_data) {
	char buffer[2000]; // buffer aimed to write the file
	// epoch
	sprintf(buffer, "%10.3f; ", my_data.tow);
	file << buffer;

	// UTC Time
	if (my_data.UTC_resolved == 1)
	{
		sprintf(buffer, "%u; %u; %u; %u; %u; %11.9f; ", my_data.year, my_data.month, my_data.day, my_data.hour, my_data.min, my_data.sec);
		file << buffer;
	}
	else
	{
		sprintf(buffer, "%u; %u; %u; %u; %u; %11.9f; ", 0, 0, 0, 0, 0, 0.0);
		file << buffer;
	}
	// UTC time accuracy
	sprintf(buffer, "%11.9f; ", my_data.time_accuracy);
	file << buffer;
	
	// fix status
	sprintf(buffer, "%u; ", my_data.gnssFixOK);
	file << buffer;

	// velocities
	sprintf(buffer, "%10.3f; ", my_data.velocity_n);
	file << buffer;
	sprintf(buffer, "%10.3f; ", my_data.velocity_e);
	file << buffer;
	sprintf(buffer, "%10.3f; ", my_data.velocity_d);
	file << buffer;
	// speed accuracy
	sprintf(buffer, "%5.3f; ", my_data.accuracy_speed);
	file << buffer;

	// lat long height
	sprintf(buffer, "%10.3f; ", my_data.lon);
	file << buffer;
	sprintf(buffer, "%10.3f; ", my_data.lat);
	file << buffer;
	sprintf(buffer, "%10.3f; ", my_data.height);
	file << buffer;
	sprintf(buffer, "%10.3f; ", my_data.hMSL);
	file << buffer;

	// accuracies
	sprintf(buffer, "%8.4f; ", my_data.accuracy_horizontal);
	file << buffer;
	sprintf(buffer, "%8.4f; ", my_data.accuracy_vertical);
	file << buffer;

	// DOP
	sprintf(buffer, "%6.2f;", my_data.PDOP);
	file << buffer;

	// new line
	sprintf(buffer, "\n");
	file << buffer;
}

UbloxSpecs::UBX_HEAD OutputWriterPVT::get_reference_header()
{
	UbloxSpecs::UBX_HEAD my_head;
	my_head.cl = 0x01;
	my_head.id = 0x07;
	return my_head;
}

void OutputWriterPVT::open_file(std::string file_name)
{
	file_name = file_name + ".pvt";
	file.open(file_name);
	return;
}
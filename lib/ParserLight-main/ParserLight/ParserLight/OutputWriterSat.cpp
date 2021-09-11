#include "stdafx.h"
#include "OutputWriterSat.h"
#include "UbloxSpecs.h"
#include <iostream>
#include <cstring>
#include <cmath>


OutputWriterSat::OutputWriterSat()
{
	OutputWriterSat::epoch_index = 0;
}


OutputWriterSat::~OutputWriterSat()
{

}

void OutputWriterSat::parse_and_write(char *inbuffer)
{
	store_and_write_SAT(inbuffer);
}

void OutputWriterSat::store_and_write_SAT(char *inbuffer)
{
	UbloxSpecs::UBX_HEAD ub_head = get_reference_header();
	UbloxSpecs::UBX_NAV_SAT my_SAT;
	UbloxSpecs::UBX_NAV_SAT_SV my_SV;
	SAT temp;

	unsigned char head[4] = { 0xb5, 0x62, ub_head.cl, ub_head.id }; // header

	char inhead[4] = { inbuffer[0], inbuffer[1], inbuffer[2], inbuffer[3] };

	if (memcmp(&head, &inhead, 4) == 0)
	{
		OutputWriterSat::epoch_index = OutputWriterSat::epoch_index + 1; // increment a unique identifier for the epochs

		ub_head.cl = inbuffer[2];
		ub_head.id = inbuffer[3];
		memcpy(&ub_head.len, &inbuffer[4], 2);	//length

		// READ
		memcpy(&my_SAT.iTOW, &inbuffer[6], 4);
		memcpy(&my_SAT.version, &inbuffer[10], 1);
		memcpy(&my_SAT.numSvs, &inbuffer[11], 1);

		// start of repeated block
		for (int i = 0; i < my_SAT.numSvs; i++)
		{
			memcpy(&my_SV.gnssId, &inbuffer[i * 12 + 14], 1);
			memcpy(&my_SV.svId, &inbuffer[i * 12 + 15], 1);
			memcpy(&my_SV.cno, &inbuffer[i * 12 + 16], 1);
			memcpy(&my_SV.elev, &inbuffer[i * 12 + 17], 1);
			memcpy(&my_SV.azim, &inbuffer[i * 12 + 18], 2);
			memcpy(&my_SV.prRes, &inbuffer[i * 12 + 20], 2);
			memcpy(&my_SV.qualityFlag, &inbuffer[i * 12 + 22], 4);

			// map
			temp.tow = 1e-3*my_SAT.iTOW;
			temp.version = my_SAT.version;
			temp.numSv = my_SAT.numSvs;
			temp.gnss_id = my_SV.gnssId;
			temp.sv_id = my_SV.svId;
			temp.cno = my_SV.cno;
			temp.elevation = my_SV.elev;
			temp.azimuth = my_SV.azim;
			temp.residual = 0.1*my_SV.prRes;
			temp.quality = my_SV.qualityFlag.qualityInd;
			temp.used = my_SV.qualityFlag.svUsed;
			temp.health = my_SV.qualityFlag.health;
			temp.diff = my_SV.qualityFlag.diffCorr;
			temp.smoothed = my_SV.qualityFlag.smoothed;
			temp.orbit_source = my_SV.qualityFlag.orbitSource;
			temp.eph_available = my_SV.qualityFlag.ephAvail;
			temp.alm_available = my_SV.qualityFlag.almAvail;
			temp.ano_available = my_SV.qualityFlag.anoAvail;
			temp.aop_available = my_SV.qualityFlag.aopAvail;
			temp.sbas_corr_used = my_SV.qualityFlag.sbasCorrUsed;
			temp.rtcm_corr_used = my_SV.qualityFlag.rtcmCorrUsed;
			temp.slas_corr_used = my_SV.qualityFlag.slasCorrUsed;
			temp.pr_corr_used = my_SV.qualityFlag.prCorrUsed;
			temp.cr_corr_used = my_SV.qualityFlag.crCorrUsed;
			temp.do_corr_used = my_SV.qualityFlag.doCorrUsed;

			// write message
			if (OutputWriter::rate > 0)
			{
				int divisor = (int)round(temp.tow / rate);
				double reminder = temp.tow - divisor * rate;
				if (fabs(reminder) > OutputWriter::threshold)
				{
					return;
				}
			}
			write_message(temp);

		}
	}
}

void OutputWriterSat::write_message(OutputWriterSat::SAT my_data) {
	char buffer[2000]; // buffer aimed to write the file
	// epoch
	sprintf(buffer, "%10.3f; ", my_data.tow);
	file << buffer;
	// epoch ID
	sprintf(buffer, "%3u; ", OutputWriterSat::epoch_index);
	file << buffer;
	
	// SV infos
	sprintf(buffer, "%2u; ", my_data.numSv);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.gnss_id);
	file << buffer;
	sprintf(buffer, "%2u; ", my_data.sv_id);
	file << buffer;
	sprintf(buffer, "%2u; ", my_data.cno);
	file << buffer;
	sprintf(buffer, "%6.2f; ", my_data.elevation);
	file << buffer;
	sprintf(buffer, "%7.2f; ", my_data.azimuth);
	file << buffer;
	sprintf(buffer, "%6.1f; ", my_data.residual);
	file << buffer;
	
	// flags
	sprintf(buffer, "%u; ", my_data.quality);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.used);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.health);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.diff);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.smoothed);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.orbit_source);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.eph_available);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.alm_available);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.ano_available);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.aop_available);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.sbas_corr_used);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.rtcm_corr_used);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.slas_corr_used);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.pr_corr_used);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.cr_corr_used);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.do_corr_used);
	file << buffer;

	sprintf(buffer, "\n");
	file << buffer;
}


UbloxSpecs::UBX_HEAD OutputWriterSat::get_reference_header()
{
	UbloxSpecs::UBX_HEAD my_head;
	my_head.cl = 0x01;
	my_head.id = 0x35;
	return my_head;
}

void OutputWriterSat::open_file(std::string file_name)
{
	file_name = file_name + ".sat";
	file.open(file_name);
	return;
}
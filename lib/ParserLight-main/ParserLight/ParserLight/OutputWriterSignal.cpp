#include "stdafx.h"
#include "OutputWriterSignal.h"
#include "UbloxSpecs.h"
#include <iostream>
#include <cstring>
#include <cmath>


OutputWriterSignal::OutputWriterSignal()
{
	OutputWriterSignal::epoch_index = 0;
}


OutputWriterSignal::~OutputWriterSignal()
{

}

void OutputWriterSignal::parse_and_write(char *inbuffer)
{
	store_and_write_SIGNAL(inbuffer);
}

void OutputWriterSignal::store_and_write_SIGNAL(char *inbuffer)
{
	UbloxSpecs::UBX_HEAD ub_head = get_reference_header();
	UbloxSpecs::UBX_NAV_SIG my_SIGNAL;
	UbloxSpecs::UBX_NAV_SIG_SIG my_SIGNAL_SIG;
	SIGNAL temp;

	unsigned char head[4] = { 0xb5, 0x62, ub_head.cl, ub_head.id }; // header

	char inhead[4] = { inbuffer[0], inbuffer[1], inbuffer[2], inbuffer[3] };

	if (memcmp(&head, &inhead, 4) == 0)
	{
		OutputWriterSignal::epoch_index = OutputWriterSignal::epoch_index + 1; // increment a unique identifier for the epochs

		ub_head.cl = inbuffer[2];
		ub_head.id = inbuffer[3];
		memcpy(&ub_head.len, &inbuffer[4], 2);	//length

		// READ
		memcpy(&my_SIGNAL.iTOW, &inbuffer[6], 4);
		memcpy(&my_SIGNAL.version, &inbuffer[10], 1);
		memcpy(&my_SIGNAL.numSig, &inbuffer[11], 1);

		// start of repeated block
		for (int i = 0; i < my_SIGNAL.numSig; i++)
		{
			memcpy(&my_SIGNAL_SIG.gnssId,		&inbuffer[i * 16 + 14], 1);
			memcpy(&my_SIGNAL_SIG.svId,			&inbuffer[i * 16 + 15], 1);
			memcpy(&my_SIGNAL_SIG.sigId,		&inbuffer[i * 16 + 16], 1);
			memcpy(&my_SIGNAL_SIG.freqId,		&inbuffer[i * 16 + 17], 1);
			memcpy(&my_SIGNAL_SIG.prRes,		&inbuffer[i * 16 + 18], 2);
			memcpy(&my_SIGNAL_SIG.cno,			&inbuffer[i * 16 + 20], 1);
			memcpy(&my_SIGNAL_SIG.qualityInd,	&inbuffer[i * 16 + 21], 1);
			memcpy(&my_SIGNAL_SIG.corrSource,	&inbuffer[i * 16 + 22], 1);
			memcpy(&my_SIGNAL_SIG.ionoModel,	&inbuffer[i * 16 + 23], 1);
			memcpy(&my_SIGNAL_SIG.sigFlags,		&inbuffer[i * 16 + 24], 2);

			// map
			temp.tow = 1e-3*my_SIGNAL.iTOW;
			temp.version = my_SIGNAL.version;
			temp.numSig = my_SIGNAL.numSig;
			temp.gnss_id = my_SIGNAL_SIG.gnssId;
			temp.sv_id = my_SIGNAL_SIG.svId;
			temp.sig_id = my_SIGNAL_SIG.sigId;
			temp.freq_id = my_SIGNAL_SIG.freqId - 7;
			temp.residual = 0.1*my_SIGNAL_SIG.prRes;
			temp.quality = my_SIGNAL_SIG.qualityInd;
			temp.correction_source = my_SIGNAL_SIG.corrSource;
			temp.iono_model = my_SIGNAL_SIG.ionoModel;
			temp.health = my_SIGNAL_SIG.sigFlags.health;
			temp.pseudorange_smoothed = my_SIGNAL_SIG.sigFlags.prSmoothed;
			temp.pseudorange_used = my_SIGNAL_SIG.sigFlags.prUsed;
			temp.carrier_used = my_SIGNAL_SIG.sigFlags.crUsed;
			temp.doppler_used = my_SIGNAL_SIG.sigFlags.doUsed;
			temp.pr_corr_used = my_SIGNAL_SIG.sigFlags.prCorrUsed;
			temp.cr_corr_used = my_SIGNAL_SIG.sigFlags.crCorrUsed;
			temp.do_corr_used = my_SIGNAL_SIG.sigFlags.doCorrUsed;

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

void OutputWriterSignal::write_message(OutputWriterSignal::SIGNAL my_data) {
	char buffer[2000]; // buffer aimed to write the file
	// epoch
	sprintf(buffer, "%10.3f; ", my_data.tow);
	file << buffer;
	// epoch ID
	sprintf(buffer, "%u; ", OutputWriterSignal::epoch_index);
	file << buffer;

	// everything else
	sprintf(buffer, "%u; ", my_data.numSig);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.gnss_id);
	file << buffer;
	sprintf(buffer, "%2u; ", my_data.sv_id);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.sig_id);
	file << buffer;
	sprintf(buffer, "%2i; ", my_data.freq_id);
	file << buffer;
	sprintf(buffer, "%6.1f; ", my_data.residual);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.quality);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.correction_source);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.iono_model);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.health);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.pseudorange_smoothed);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.pseudorange_used);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.carrier_used);
	file << buffer;
	sprintf(buffer, "%u; ", my_data.doppler_used);
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


UbloxSpecs::UBX_HEAD OutputWriterSignal::get_reference_header()
{
	UbloxSpecs::UBX_HEAD my_head;
	my_head.cl = 0x01;
	my_head.id = 0x43;
	return my_head;
}

void OutputWriterSignal::open_file(std::string file_name)
{
	file_name = file_name + ".signal";
	file.open(file_name);
	return;
}
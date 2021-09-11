#include "stdafx.h"
#include "myConstants.h"
#include "UbxReader.h"

#include "OutputWriter.h"
#include "OutputWriterPos.h"
#include "OutputWriterRinex.h"
#include "OutputWriterClock.h"
#include "OutputWriterDOP.h"
#include "OutputWriterPVT.h"
#include "OutputWriterSat.h"
#include "OutputWriterSignal.h"
#include "OutputWriterStatus.h"

#include "UbloxSpecs.h"

#include <string>
#include <cstring>
#include <vector>
#include <bitset>

#include "stdafx.h"

bool check_option(char *value, std::string option);

int main(int argc, char *argv[])
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// test that the variable size implementation is correct in this build
	UbloxSpecs::test_specs();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// general options
	std::string file_name;
	std::string output_file_name = "";
	bool opt_wait = false;
	double rate = 0;
	double threshold = 0.02;

	// options for RINEX
	bool rinex_out = false;
	int mjd_valid = 0;
	int rinex_version = 3;
	OutputWriterRinex::RINEX_HEADER_META meta_data;

	// options for position output
	bool pos_out = false;

	// various messages
	bool clock_out = false;
	bool dop_out = false;
	bool pvt_out = false;
	bool sat_out = false;
	bool signal_out = false;
	bool status_out = false;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// reading the options
	file_name = argv[1];

	for (int i = 2; i < argc; i = i + 2)
	{
		// general options
		if (strcmp(argv[i], "rate") == 0) { rate = atof(argv[i + 1]); continue; };
		if (strcmp(argv[i], "threshold") == 0) { threshold = atof(argv[i + 1]); continue; };
		if (strcmp(argv[i], "wait") == 0) { opt_wait = check_option(argv[i + 1], "wait"); continue; };
		if (strcmp(argv[i], "o") == 0) { output_file_name = argv[i + 1]; continue; };

		// rinex options
		if (strcmp(argv[i], "rinex") == 0) { rinex_out = check_option(argv[i + 1], "rinex"); continue; };
		if (strcmp(argv[i], "mjd") == 0) { mjd_valid = atoi(argv[i + 1]); continue; };
		if (strcmp(argv[i], "rinex_version") == 0) { rinex_version = atoi(argv[i + 1]); continue; };
		// rinex header info
		if (strcmp(argv[i], "run_by") == 0) { meta_data.run_by = argv[i + 1]; continue; };
		if (strcmp(argv[i], "observer") == 0) { meta_data.observer = argv[i + 1]; continue; };
		if (strcmp(argv[i], "agency") == 0) { meta_data.agency = argv[i + 1]; continue; };
		if (strcmp(argv[i], "marker_name") == 0) { meta_data.marker_name = argv[i + 1]; continue; };
		if (strcmp(argv[i], "marker_number") == 0) { meta_data.marker_number = atoi(argv[i + 1]); continue; };
		if (strcmp(argv[i], "receiver_number") == 0) { meta_data.receiver_number = atoi(argv[i + 1]); continue; };
		if (strcmp(argv[i], "receiver_type") == 0) { meta_data.receiver_type = argv[i + 1]; continue; };
		if (strcmp(argv[i], "receiver_version") == 0) { meta_data.receiver_version = argv[i + 1]; continue; };
		if (strcmp(argv[i], "antenna_number") == 0) { meta_data.antenna_number = atoi(argv[i + 1]); continue; };
		if (strcmp(argv[i], "antenna_type") == 0) { meta_data.antenna_type = argv[i + 1]; continue; };
		if (strcmp(argv[i], "approx_pos_x") == 0) { meta_data.approx_pos_x = atof(argv[i + 1]); continue; };
		if (strcmp(argv[i], "approx_pos_y") == 0) { meta_data.approx_pos_y = atof(argv[i + 1]); continue; };
		if (strcmp(argv[i], "approx_pos_z") == 0) { meta_data.approx_pos_z = atof(argv[i + 1]); continue; };
		if (strcmp(argv[i], "delta_h") == 0) { meta_data.delta_h = atof(argv[i + 1]); continue; };
		if (strcmp(argv[i], "delta_e") == 0) { meta_data.delta_e = atof(argv[i + 1]); continue; };
		if (strcmp(argv[i], "delta_n") == 0) { meta_data.delta_n = atof(argv[i + 1]); continue; };
		if (strcmp(argv[i], "comment") == 0) { meta_data.comment = argv[i + 1]; continue; };

		// position options
		if (strcmp(argv[i], "pos") == 0) { pos_out = check_option(argv[i + 1], "pos"); continue; };

		// Clock output option
		if (strcmp(argv[i], "clock") == 0) { clock_out = check_option(argv[i + 1], "clock"); continue; };

		// DOP output option
		if (strcmp(argv[i], "dop") == 0) { dop_out = check_option(argv[i + 1], "dop"); continue; };

		// PVT output option
		if (strcmp(argv[i], "pvt") == 0) { pvt_out = check_option(argv[i + 1], "pvt"); continue; };

		// Sat output option
		if (strcmp(argv[i], "sat") == 0) { sat_out = check_option(argv[i + 1], "sat"); continue; };

		// Signal output option
		if (strcmp(argv[i], "signal") == 0) { signal_out = check_option(argv[i + 1], "signal"); continue; };

		// Status ouput option
		if (strcmp(argv[i], "status") == 0) { status_out = check_option(argv[i + 1], "status"); continue; };

		// unknown options
		std::cout << argv[i] << " option unknown. Option ignored.\n";
	}

	// generate output file name
	if (output_file_name.size() == 0)
	{
		output_file_name = file_name;
	}
	// remove the extension, if present
	size_t ind;
	ind = output_file_name.find_last_of(".");
	output_file_name = output_file_name.substr(0, ind);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// check for option consistency
	if (rinex_out)
	{
		if (mjd_valid == 0)
		{
			std::cout << "mjd_validity is requested\nAbort.";

			if (opt_wait)
			{
				std::cout << "\n\nFinished. Press any button\n";
				getchar();
			}

			return 0;
		}
	}
	
	OutputWriter::rate = rate; // set the rate for all classes [seconds]
	OutputWriter::threshold = threshold; // set the threshold applied to the rate [seconds]
	UbxReader myReader;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// register readers
	if (rinex_out)
	{
		OutputWriterRinex* output_writer_rinex = new OutputWriterRinex(mjd_valid, meta_data);
		myReader.register_reader(output_writer_rinex);
	}
	if (pos_out)
	{
		OutputWriterPos* output_writer_pos = new OutputWriterPos();
		myReader.register_reader(output_writer_pos);
	}
	if (clock_out)
	{
		OutputWriterClock* output_writer_clock = new OutputWriterClock();
		myReader.register_reader(output_writer_clock);
	}
	if (dop_out)
	{
		OutputWriterDOP* output_writer_dop = new OutputWriterDOP();
		myReader.register_reader(output_writer_dop);
	}
	if (pvt_out)
	{
		OutputWriterPVT* output_writer_pvt = new OutputWriterPVT();
		myReader.register_reader(output_writer_pvt);
	}
	if (sat_out)
	{
		OutputWriterSat* output_writer_sat = new OutputWriterSat();
		myReader.register_reader(output_writer_sat);
	}
	if (signal_out)
	{
		OutputWriterSignal* output_writer_signal = new OutputWriterSignal();
		myReader.register_reader(output_writer_signal);
	}
	if (pos_out)
	{
		OutputWriterStatus* output_writer_status = new OutputWriterStatus();
		myReader.register_reader(output_writer_status);
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// processing
	myReader.open_file(file_name);
	myReader.read_any_message(output_file_name);
	myReader.close_file();

	if (opt_wait)
	{
		std::cout << "\n\nFinished. Press any button\n";
		getchar();
	}
	return 0;

}

//---------------------------------------------------------------------------------
bool check_option(char *value, std::string option)
{
	bool my_bool;
	switch (atoi(value))
	{
	case 0:
		my_bool = false; break;
	case 1:
		my_bool = true; break;
	default:
		std::cout << "Invalid argument for option " << option << ". Option set to false.\n";
		my_bool = false;
	}
	return my_bool;
}
//---------------------------------------------------------------------------------
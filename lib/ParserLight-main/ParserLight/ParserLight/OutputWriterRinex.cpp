#include "stdafx.h"
#include "OutputWriterRinex.h"
#include "UbloxSpecs.h"
#include "Utils.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

OutputWriterRinex::OutputWriterRinex(int mjd_valid)
{
	OutputWriterRinex::mjd_valid = mjd_valid;
	OutputWriterRinex::first_message = true;
	OutputWriterRinex::mjd_first_epoch = 0;
	OutputWriterRinex::sec_first_epoch = 0;
	OutputWriterRinex::leap_sec = 0;

	OutputWriterRinex::rinex_version = 3;
}

OutputWriterRinex::OutputWriterRinex(int mjd_valid, OutputWriterRinex::RINEX_HEADER_META meta_data)
{
	OutputWriterRinex::mjd_valid = mjd_valid;
	OutputWriterRinex::first_message = true;
	OutputWriterRinex::mjd_first_epoch = 0;
	OutputWriterRinex::sec_first_epoch = 0;
	OutputWriterRinex::leap_sec = 0;

	OutputWriterRinex::rinex_version = 3;

	set_meta_data(meta_data);
}

OutputWriterRinex::~OutputWriterRinex()
{
}

void OutputWriterRinex::open_file(std::string file_name)
{
	short int month;
	short int year;
	double hour;
	short int day;
	short int doy;
	char doy03[10];
	char year02[10];
	std::ostringstream string_stream;

	/* Create a name following the rinex naming convention */
	Utils::YMDHfromMJD(OutputWriterRinex::mjd_valid, &year, &month, &day, &hour);
	Utils::YearDoyFromMJD(OutputWriterRinex::mjd_valid, &year, &doy);
	// year with 2 digits
	sprintf(year02, "%02.0f", year - 100 * trunc(year / 100));
	// add leading zero to DOY
	sprintf(doy03, "%03u", doy);
	string_stream << doy03 << "0." << year02 << "O";
	// transform to upper case
	std::transform(file_name.begin(), file_name.end(), file_name.begin(), ::toupper);
	// write name: xxxDOY0.YYO
	file_name = file_name + string_stream.str();
	file.open(file_name);
}

void OutputWriterRinex::set_meta_data(OutputWriterRinex::RINEX_HEADER_META meta_data)
{
	rinex_header_info = meta_data;
}

void OutputWriterRinex::set_rinex_version(int version)
{
	OutputWriterRinex::rinex_version = version;
}


void OutputWriterRinex::set_first_epoch(int mjd, double sec, int leap_sec)
{
	OutputWriterRinex::mjd_first_epoch = mjd;
	OutputWriterRinex::sec_first_epoch = sec;
	OutputWriterRinex::leap_sec = leap_sec;
}

/* Write a Rinex Observation file, format 2.11 */
void OutputWriterRinex::write_header() 
{
	short int month;
	short int year;
	short int minutes;
	double hour;
	double sec;
	short int day;
	short int doy;
	char buffer[2000]; // buffer aimed to write the file
	double rinex_version = 0.00;
	std::ostringstream string_stream;

	if (OutputWriterRinex::rinex_version == 2)
	{
		rinex_version = 2.11;
	}
	if (OutputWriterRinex::rinex_version == 3)
	{
		rinex_version = 3.04;
	}

	/* Create a name following the rinex naming convention */
	Utils::YMDHfromMJD(OutputWriterRinex::mjd_first_epoch, &year, &month, &day, &hour);
	Utils::YearDoyFromMJD(OutputWriterRinex::mjd_first_epoch, &year, &doy);

	// get local time
	time_t now = time(0);
	tm tm = *localtime(&now);

	// get the time of the first epoch
	sec = OutputWriterRinex::sec_first_epoch; // read the mjd of the first epoch
	hour = (int)(sec / 3600);
	minutes = (int)((sec - hour * 3600) / 60);
	sec = sec - hour * 3600 - minutes * 60;

	/* write header lines  */
	sprintf(buffer, "     %4.2f           OBSERVATION DATA    M                   RINEX VERSION / TYPE\n", rinex_version);
	file << buffer;
	
	sprintf(buffer, "%-20s%20s", "UBX PARSER", rinex_header_info.run_by.c_str());
	file << buffer << std::put_time(&tm, "%d-%m-%y %H:%M") << "      PGM / RUN BY / DATE\n";
	sprintf(buffer, "%60sMARKER NAME\n", rinex_header_info.marker_name.c_str());
	file << buffer;
	sprintf(buffer, "%-60iMARKER NUMBER\n", rinex_header_info.marker_number);
	file << buffer;
	sprintf(buffer, "%-20s%-40sOBSERVER / AGENCY\n", rinex_header_info.observer.c_str(), rinex_header_info.agency.c_str());
	file << buffer;
	sprintf(buffer, "%20i%-20s%20sREC # / TYPE / VERS\n", rinex_header_info.receiver_number, rinex_header_info.receiver_type.c_str(), rinex_header_info.receiver_version.c_str());
	file << buffer;
	sprintf(buffer, "%20i%20s%20sANT # / TYPE\n", rinex_header_info.antenna_number, rinex_header_info.antenna_type.c_str(), "");
	file << buffer;
	sprintf(buffer, "%14.4f%14.4f%14.4f%18sAPPROX POSITION XYZ\n", rinex_header_info.approx_pos_x, rinex_header_info.approx_pos_y, rinex_header_info.approx_pos_z, "");
	file << buffer;
	sprintf(buffer, "%14.4f%14.4f%14.4f%18sANTENNA: DELTA H/E/N\n", rinex_header_info.delta_h, rinex_header_info.delta_e, rinex_header_info.delta_n, "");
	file << buffer;
	if (OutputWriterRinex::rinex_version == 3)
	{
		for (int i = 0; i < myConstants::MAX_ID; i++)
		{
			std::string temp = get_signals_string_per_GNSS(i, 1, 1, 0, 1);
			if (temp.size() == 0)
			{
				continue;
			}
			sprintf(buffer, "%-60sSYS / # / OBS TYPES\n", temp.c_str());
			file << buffer;
		}
	}
	sprintf(buffer, "     1     1                                                WAVELENGTH FACT L1/2\n");
	file << buffer;
	
	if (OutputWriterRinex::rinex_version == 2)
	{
		sprintf(buffer, "     4    C1    L1    D1    S1                              # / TYPES OF OBSERV\n");
		file << buffer;
	}
	
	sprintf(buffer, "  %4u    %2u    %2u    %2u    %2u   %10.7f     GPS         TIME OF FIRST OBS\n", (int)year, (int)month, (int)day, (int)hour, (int)minutes, sec);
	file << buffer;
	//sprintf(buffer, "  %4u    %2u    %2u    %2u    %2u   %10.7f     GPS         TIME OF LAST OBS\n", (int)year_end, (int)month_end, (int)day_end, (int)hour_end, (int)minutes_end, sec_end);
	//file << buffer;
	if (OutputWriterRinex::leap_sec > 0)
	{
		sprintf(buffer, "    %2u                                                      LEAP SECONDS\n", OutputWriterRinex::leap_sec);
		file << buffer;
	}
	//sprintf(buffer, "    %2u                                                      # OF SATELLITES\n", num_sats);
	//file << buffer;
	sprintf(buffer, "%-60sCOMMENT\n", rinex_header_info.comment.c_str());
	file << buffer;
	sprintf(buffer, "                                                            END OF HEADER\n");
	file << buffer;
}

OutputWriterRinex::GNSS_EPOCH OutputWriterRinex::store_RAWX(char *inbuffer)
{
	int my_mjd;
	double my_sec;
	UbloxSpecs::UBX_HEAD ub_head;
	ub_head.cl = 0x02; // RAWX class
	ub_head.id = 0x15; // RAWX id

	UbloxSpecs::UBX_RXM_RAWX my_rawx;
	GNSS_EPOCH temp;

	unsigned char head[4] = { 0xb5, 0x62, ub_head.cl, ub_head.id }; // header

	char inhead[4] = { inbuffer[0], inbuffer[1], inbuffer[2], inbuffer[3] };

	if (memcmp(&head, &inhead, 4) == 0)
	{
		ub_head.cl = inbuffer[2];
		ub_head.id = inbuffer[3];
		memcpy(&ub_head.len, &inbuffer[4], 2);	//length

		// READ
		memcpy(&my_rawx.rcv_tow, &inbuffer[6], 8); /* [s] */
		memcpy(&my_rawx.week, &inbuffer[14], 2);
		memcpy(&my_rawx.leap_s, &inbuffer[16], 1);
		memcpy(&my_rawx.num_meas, &inbuffer[17], 1);
		memcpy(&my_rawx.rec_stat, &inbuffer[18], 1);

		// MAP to general bin
		Utils::get_mjd(1000.0*my_rawx.rcv_tow, my_rawx.week, &my_mjd, &my_sec); // time conversion
		temp.mjd = my_mjd;
		temp.sec = my_sec;
		temp.num_meas = my_rawx.num_meas;
		temp.leap_sec = my_rawx.leap_s;
		temp.leap_sec_valid = my_rawx.rec_stat.leap_sec;
		temp.clock_reset = my_rawx.rec_stat.clock_res;

		for (int i = 0; i < my_rawx.num_meas; i++)
		{
			GNSS_MEAS temp_meas;
			UbloxSpecs::UBX_RXM_RAWX_SV my_sv;

			// READ
			memcpy(&my_sv.pr_mes, &inbuffer[i * 32 + 22], 8);
			memcpy(&my_sv.cp_mes, &inbuffer[i * 32 + 30], 8);
			memcpy(&my_sv.do_mes, &inbuffer[i * 32 + 38], 4);
			memcpy(&my_sv.gnss_id, &inbuffer[i * 32 + 42], 1);
			memcpy(&my_sv.sv_id, &inbuffer[i * 32 + 43], 1);
			memcpy(&my_sv.sig_id, &inbuffer[i * 32 + 44], 1);
			memcpy(&my_sv.freq_id, &inbuffer[i * 32 + 45], 1);
			memcpy(&my_sv.locktime, &inbuffer[i * 32 + 46], 2);
			memcpy(&my_sv.cno, &inbuffer[i * 32 + 48], 1);
			memcpy(&my_sv.pr_stdev, &inbuffer[i * 32 + 49], 1);
			memcpy(&my_sv.cp_stdev, &inbuffer[i * 32 + 50], 1);
			memcpy(&my_sv.do_stdev, &inbuffer[i * 32 + 51], 1);
			memcpy(&my_sv.trk_stat, &inbuffer[i * 32 + 52], 1);

			// MAP to general bin
			temp_meas.C1 = my_sv.pr_mes;
			temp_meas.L1 = my_sv.cp_mes;
			temp_meas.D1 = my_sv.do_mes;
			temp_meas.sv_id = my_sv.sv_id;
			temp_meas.sig_id = my_sv.sig_id;
			temp_meas.gnss_id = my_sv.gnss_id;
			temp_meas.freq_id = my_sv.freq_id;
			temp_meas.locktime = my_sv.locktime;
			temp_meas.mes_qi = -1;
			temp_meas.cno = my_sv.cno;
			temp_meas.lol = -1;
			temp_meas.C1_stdev = 0.01*pow(2, my_sv.pr_stdev.std_flag);
			temp_meas.L1_stdev = 0.004*my_sv.cp_stdev.std_flag;
			temp_meas.D1_stdev = 0.002*pow(2, my_sv.do_stdev.std_flag);
			temp_meas.C1_valid = my_sv.trk_stat.pr_valid;
			temp_meas.L1_valid = my_sv.trk_stat.cp_valid;
			temp_meas.half_cycle_valid = my_sv.trk_stat.half_cyc;
			temp_meas.hald_cycle_substracted = my_sv.trk_stat.sub_half_cyc;

			temp.meas.push_back(temp_meas);
		}
	}
	return temp;
}


void OutputWriterRinex::write_message(OutputWriterRinex::GNSS_EPOCH my_gnss_epoch)
{
	if (OutputWriterRinex::rinex_version == 2)
	{
		write_message_2(my_gnss_epoch);
	}
	if (OutputWriterRinex::rinex_version == 3)
	{
		write_message_3(my_gnss_epoch);
	}
}


void OutputWriterRinex::write_message_2(OutputWriterRinex::GNSS_EPOCH my_gnss_epoch) {
	double mjd;
	double sec;
	short int minutes;
	short int month;
	short int year;
	double hour;
	short int day;
	short int year2;
	char buffer[2000]; // buffer aimed to write the file
	int rinex_snr;
	std::string rinex_lli;


	if (my_gnss_epoch.num_meas > 0)
	{
		// get the first date
		mjd = my_gnss_epoch.mjd; // read the mjd of the first epoch
		Utils::YMDHfromMJD(mjd, &year, &month, &day, &hour);

		// get the time of the epoch
		sec = my_gnss_epoch.sec; // read the mjd of the first epoch

		// DEBUG UHRZEUGS
		// sec = round(sec);

		hour = (int)(sec / 3600);
		minutes = (int)((sec - hour * 3600) / 60);
		sec = sec - hour * 3600 - minutes * 60;

		year2 = (short)(year - 100 * trunc(year / 100));

		sprintf(buffer, " %02u %2u %2u %2u %2u %10.7f %2u%3u", (int)year2, (int)month, (int)day, (int)hour, (int)minutes, sec, 0, my_gnss_epoch.num_meas);
		file << buffer;

		std::string gnss_id = " ";
		for (int j = 0; j < my_gnss_epoch.num_meas; j++)
		{
			// skip everything which is not L1 in Rinex 2.11
			if (my_gnss_epoch.meas[j].sig_id > 0)
			{
				continue;
			}

			gnss_id = get_constellation_id(my_gnss_epoch.meas[j].gnss_id);

			if ((ceil(j / 12.0) == j / 12.0) && (j != 0)) // if more than 12 sats, use continuation line
			{
				sprintf(buffer, "\n                                ");
				file << buffer;
			}
			sprintf(buffer, "%1s%02u", gnss_id.c_str(), my_gnss_epoch.meas[j].sv_id);
			file << buffer;
		}
		sprintf(buffer, "\n");
		file << buffer;
		/* writing the actual observations */
		for (int j = 0; j < my_gnss_epoch.num_meas; j++)
		{
			if (my_gnss_epoch.meas[j].sig_id > 0)
			{
				continue;
			}

			/* C/A */
			rinex_snr = std::min(std::max((int)ceil(my_gnss_epoch.meas[j].cno / 6.0), 1), 9);
			sprintf(buffer, "%14.3f  ", my_gnss_epoch.meas[j].C1);
			file << buffer;
			/* L1 */
			rinex_lli = " ";
			if (my_gnss_epoch.meas[j].lol == -1)
			{
				// rinex loss of lock indicator is put to 2 in case of half cycle ambiguities
				if (my_gnss_epoch.meas[j].half_cycle_valid == 0)
				{
					rinex_lli = "2";
				}

			}
			else
			{
				if (my_gnss_epoch.meas[j].lol > 0)
				{
					std::stringstream convert;
					convert << my_gnss_epoch.meas[j].lol;
					rinex_lli = convert.str();
				}
			}
			sprintf(buffer, "%14.3f%1s%1u", my_gnss_epoch.meas[j].L1, rinex_lli.c_str(), rinex_snr);
			file << buffer;
			/* D1 */
			sprintf(buffer, "%14.3f  ", my_gnss_epoch.meas[j].D1);
			file << buffer;
			/* SNR */
			sprintf(buffer, "%14.3f\n", (double)my_gnss_epoch.meas[j].cno);
			file << buffer;
		}
	}
}

void OutputWriterRinex::write_message_3(OutputWriterRinex::GNSS_EPOCH my_gnss_epoch) {
	double mjd;
	double sec;
	short int minutes;
	short int month;
	short int year;
	double hour;
	short int day;
	char buffer[2000]; // buffer aimed to write the file
	int rinex_snr;
	std::string rinex_lli;

	if (my_gnss_epoch.num_meas <= 0)
	{
		return;
	}

	// time stuff
	mjd = my_gnss_epoch.mjd;
	Utils::YMDHfromMJD(mjd, &year, &month, &day, &hour);

	// get the time of the epoch
	sec = my_gnss_epoch.sec; // read the mjd of the first epoch

	hour = (int)(sec / 3600);
	minutes = (int)((sec - hour * 3600) / 60);
	sec = sec - hour * 3600 - minutes * 60;

	// gather measurements par sv
	std::vector<std::vector<std::vector<GNSS_MEAS>>> meas_per_sv_per_GNSS;
	std::vector<std::vector<int>> nb_rows_per_sv_per_GNSS;
	// initialize
	for (int i = 0; i < myConstants::MAX_ID; i++)
	{
		std::vector<std::vector<GNSS_MEAS>> temp_vec_vec_GNSS;
		//
		std::vector<int> temp_vec_int;
		for (int j = 0; j < myConstants::MAX_PRN; j++)
		{
			std::vector<GNSS_MEAS> temp_vec_GNSS;
			temp_vec_vec_GNSS.push_back(temp_vec_GNSS);
			//
			temp_vec_int.push_back(0);
		}
		meas_per_sv_per_GNSS.push_back(temp_vec_vec_GNSS);
		//
		nb_rows_per_sv_per_GNSS.push_back(temp_vec_int);
	}

	// populate
	for (int j = 0; j < my_gnss_epoch.num_meas; j++)
	{
		int gnss_id = my_gnss_epoch.meas.at(j).gnss_id;
		int sv_id = my_gnss_epoch.meas.at(j).sv_id;
		int sig_id = my_gnss_epoch.meas.at(j).sig_id;

		meas_per_sv_per_GNSS.at(gnss_id).at(sv_id).push_back(my_gnss_epoch.meas.at(j));
		RINEX3_CODES my_code = get_rinex3_codes(gnss_id, sig_id);

		nb_rows_per_sv_per_GNSS.at(gnss_id).at(sv_id) = std::max(nb_rows_per_sv_per_GNSS.at(gnss_id).at(sv_id), my_code.row_index + 1);
	}

	// get some statistics
	int nb_unique_sv = 0;
	for (int i = 0; i < myConstants::MAX_ID; i++)
	{
		for (int j = 0; j < myConstants::MAX_PRN; j++)
		{
			if (nb_rows_per_sv_per_GNSS.at(i).at(j) > 0)
			{
				nb_unique_sv = nb_unique_sv + 1;
			}
		}
	}

	// start writing the file
	sprintf(buffer, "> %04u %2u %2u %2u %2u %11.7f  %1u%3u\n", (int)year, (int)month, (int)day, (int)hour, (int)minutes, sec, 0, nb_unique_sv);
	file << buffer;

	// write the epochs
	// iterate over gnss_ids and sv_ids
	for (int i = 0; i < myConstants::MAX_ID; i++)
	{
		for (int j = 0; j < myConstants::MAX_PRN; j++)
		{
			// continue if there is nothing to write
			if (nb_rows_per_sv_per_GNSS.at(i).at(j) == 0)
			{
				continue;
			}

			// create an empty vector to sort the measurements according to row index
			std::vector<GNSS_MEAS> current_meas;
			for (int k = 0; k < nb_rows_per_sv_per_GNSS.at(i).at(j); k++)
			{
				GNSS_MEAS temp;
				current_meas.push_back(temp);
			}
			
			// fill the current measures in a vector that represents the actual rows
			for (int m = 0; m < meas_per_sv_per_GNSS.at(i).at(j).size(); m++)
			{
				int gnss_id = meas_per_sv_per_GNSS.at(i).at(j).at(m).gnss_id;
				int sv_id = meas_per_sv_per_GNSS.at(i).at(j).at(m).sv_id;
				int sig_id = meas_per_sv_per_GNSS.at(i).at(j).at(m).sig_id;

				RINEX3_CODES my_code = get_rinex3_codes(gnss_id, sig_id);
				current_meas.at(my_code.row_index) = meas_per_sv_per_GNSS.at(i).at(j).at(m);
			}

			// write the system code
			std::string gnss_id = get_constellation_id(meas_per_sv_per_GNSS.at(i).at(j).at(0).gnss_id);
			sprintf(buffer, "%1s%02u", gnss_id.c_str(), meas_per_sv_per_GNSS.at(i).at(j).at(0).sv_id);
			file << buffer;

			// fill
			for (int m = 0; m < current_meas.size(); m++)
			{
				if (current_meas.at(m).C1 == 0)
				{
					// empty record
					sprintf(buffer, "%48s", "");
					file << buffer;
					continue;
				}

				/* Code */
				rinex_snr = std::min(std::max((int)ceil(current_meas.at(m).cno / 6.0), 1), 9);
				sprintf(buffer, "%14.3f  ", current_meas.at(m).C1);
				file << buffer;
				/* Phase and indicators */
				rinex_lli = " ";
				if (current_meas.at(m).lol == -1)
				{
					// rinex loss of lock indicator is put to 2 in case of half cycle ambiguities
					if (current_meas.at(m).half_cycle_valid == 0) { rinex_lli = "2"; };
				}
				else
				{
					if (current_meas.at(m).lol > 0)
					{
						std::stringstream convert;
						convert << current_meas.at(m).lol;
						rinex_lli = convert.str();
					}
				}
				sprintf(buffer, "%14.3f%1s%1u", current_meas.at(m).L1, rinex_lli.c_str(), rinex_snr);
				file << buffer;
				/* Doppler */
				// If doppler is enabled -> you have to adapt the header !
				// sprintf(buffer, "%14.3f  ", current_meas.at(m).D1);
				// file << buffer;
				/* SNR */
				sprintf(buffer, "%14.3f  ", (double)current_meas.at(m).cno);
				file << buffer;
			}
			sprintf(buffer, "\n");
			file << buffer;
		}
	}	
}

std::string OutputWriterRinex::get_signals_string_per_GNSS(int gnss_id, bool ca, bool l, bool d, bool cn0)
{
	int nb_signals = 0;
	char buffer[2000];
	std::string string_codes;
	std::stringstream ss;
	
	for (int i = 0; i < myConstants::MAX_SIG_ID; i++)
	{
		RINEX3_CODES my_code = get_rinex3_codes(gnss_id, i);
		if (my_code.row_index == -1)
		{
			// answer for an non existing constellation
			if (i == 0)
			{
				return "";
			}
			break;
		}
		if (ca) { string_codes = string_codes + " " + my_code.code_code; nb_signals = nb_signals + 1;};
		if (l) { string_codes = string_codes + " " + my_code.code_phase; nb_signals = nb_signals + 1;};
		if (d) { string_codes = string_codes + " " + my_code.code_doppler; nb_signals = nb_signals + 1;};
		if (cn0) { string_codes = string_codes + " " + my_code.code_signal; nb_signals = nb_signals + 1;};
	}

	sprintf(buffer, "%s  %3i %s", get_constellation_id(gnss_id).c_str(), nb_signals, string_codes.c_str());
	ss << buffer;

	std::string output_string = ss.str();
	return output_string;
}



OutputWriterRinex::RINEX3_CODES OutputWriterRinex::get_rinex3_codes(int gnss_id, int sig_id)
{
	OutputWriterRinex::RINEX3_CODES my_code;

	int row_index = -1;
	std::string code_code = "";
	std::string code_phase = "";
	std::string code_doppler = "";
	std::string code_signal = "";

	if (gnss_id == 0 && sig_id == 0) { row_index = 0; code_code = "C1C"; code_phase = "L1C"; code_doppler = "D1C"; code_signal = "S1C";};
	if (gnss_id == 0 && sig_id == 3) { row_index = 1; code_code = "C2L"; code_phase = "L2L"; code_doppler = "D2L"; code_signal = "S2L";};
	if (gnss_id == 0 && sig_id == 4) { row_index = 2; code_code = "C2X"; code_phase = "L2X"; code_doppler = "D2X"; code_signal = "S2X";};
	if (gnss_id == 1 && sig_id == 0) { row_index = 0; code_code = "C1C"; code_phase = "L1C"; code_doppler = "D1C"; code_signal = "S1C";};
	if (gnss_id == 2 && sig_id == 0) { row_index = 0; code_code = "C1C"; code_phase = "L1C"; code_doppler = "D1C"; code_signal = "S1C";};
	if (gnss_id == 2 && sig_id == 1) { row_index = 1; code_code = "C1X"; code_phase = "L1X"; code_doppler = "D1X"; code_signal = "S1X";};
	if (gnss_id == 2 && sig_id == 5) { row_index = 2; code_code = "C7I"; code_phase = "L7I"; code_doppler = "D7I"; code_signal = "S7I";};
	if (gnss_id == 2 && sig_id == 6) { row_index = 3; code_code = "C7Q"; code_phase = "L7Q"; code_doppler = "D7Q"; code_signal = "S7Q";};
	if (gnss_id == 3 && sig_id == 0) { row_index = 0; code_code = "C2I"; code_phase = "L2I"; code_doppler = "D2I"; code_signal = "S2I";};
	if (gnss_id == 3 && sig_id == 1) { row_index = 0; code_code = "C2I"; code_phase = "L2I"; code_doppler = "D2I"; code_signal = "S2I";};
	if (gnss_id == 3 && sig_id == 2) { row_index = 1; code_code = "C7I"; code_phase = "L7I"; code_doppler = "D7I"; code_signal = "S7I";};
	if (gnss_id == 3 && sig_id == 3) { row_index = 1; code_code = "C7I"; code_phase = "L7I"; code_doppler = "D7I"; code_signal = "S7I";};
	if (gnss_id == 5 && sig_id == 0) { row_index = 0; code_code = "C1C"; code_phase = "L1C"; code_doppler = "D1C"; code_signal = "S1C";};
	if (gnss_id == 5 && sig_id == 1) { row_index = 1; code_code = "C1S"; code_phase = "L1S"; code_doppler = "D1S"; code_signal = "S1S";};
	if (gnss_id == 5 && sig_id == 4) { row_index = 2; code_code = "C2S"; code_phase = "L2S"; code_doppler = "D2S"; code_signal = "S2S";};
	if (gnss_id == 5 && sig_id == 5) { row_index = 3; code_code = "C2L"; code_phase = "L2L"; code_doppler = "D2L"; code_signal = "S2L";};
	if (gnss_id == 6 && sig_id == 0) { row_index = 0; code_code = "C1C"; code_phase = "L1C"; code_doppler = "D1C"; code_signal = "S1C";};
	if (gnss_id == 6 && sig_id == 2) { row_index = 1; code_code = "C2C"; code_phase = "L2C"; code_doppler = "D2C"; code_signal = "S2C";};

	/*
	0 0 0 C1C L1C D1C S1C
	0 3 1 C2L L2L D2L S2L
	0 4 2 C2X L2X D2X S2X
	2 0 0 C1C L1C D1C S1C
	2 1 1 C1X L1X D1X S1X
	2 5 2 C7I L7I D7I S7I
	2 6 3 C7Q L7Q D7Q S7Q
	3 0 0 C2I L2I D2I S2I
	3 1 0 C2I L2I D2I S2I
	3 2 1 C7I L7I D7I S7I
	3 3 1 C7I L7I D7I S7I
	5 0 0 C1C L1C D1C S1C
	5 1 1 C1S L1S D1S S1S
	5 4 2 C2S L2S D2S S2S
	5 5 3 C2L L2L D2L S2L
	6 0 0 C1C L1C D1C S1C
	6 2 1 C2C L2C D2C S2C
	*/

	my_code.row_index = row_index;
	my_code.code_code = code_code;
	my_code.code_phase = code_phase;
	my_code.code_doppler = code_doppler;
	my_code.code_signal = code_signal;

	return my_code;
}

std::string OutputWriterRinex::get_constellation_id(int gnss_id)
{
	std::string my_string;

	switch (gnss_id)
	{
	case 0:
		my_string = "G"; break; // GPS
	case 1:
		my_string = "S"; break; // SBAS
	case 2:
		my_string = "E"; break; // Galileo
	case 3:
		my_string = "X"; break; // BeiDou
	case 4:
		my_string = "X"; break; // IMES
	case 5:
		my_string = "X"; break; // QZSS
	case 6:
		my_string = "R"; break; // GLONASS
	default:
		my_string = "G";
	}

	return my_string;
}


void OutputWriterRinex::parse_and_write(char * inbuffer)
{
	OutputWriterRinex::GNSS_EPOCH my_data;
	my_data = store_RAWX(inbuffer);
	
	// reject all messages with an invalid mjd
	if (abs(mjd_valid - my_data.mjd) > 10)
	{
		return;
	}

	// skip message if rate option was chosen
	if (OutputWriter::rate > 0)
	{
		int divisor = (int)round(my_data.sec / rate);
		double reminder = my_data.sec - divisor * rate;
		if (fabs(reminder) > OutputWriter::threshold)
		{
			return;
		}
	}

	if (OutputWriterRinex::first_message)
	{
		// set the first epoch
		if (my_data.leap_sec_valid)
		{
			set_first_epoch(my_data.mjd, my_data.sec, my_data.leap_sec);
		}
		else
		{
			set_first_epoch(my_data.mjd, my_data.sec, 0);
		}
		set_first_epoch(my_data.mjd, my_data.sec, my_data.leap_sec);

		// write the header
		write_header();
		OutputWriterRinex::first_message = false;
	}

	// write the message
	write_message(my_data);
}

UbloxSpecs::UBX_HEAD OutputWriterRinex::get_reference_header()
{
	UbloxSpecs::UBX_HEAD my_head;
	my_head.cl = 0x02;
	my_head.id = 0x15;
	return my_head;
}

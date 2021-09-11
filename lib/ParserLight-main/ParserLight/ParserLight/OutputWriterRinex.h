#pragma once
#include "OutputWriter.h"
#include <vector>
#include "myConstants.h"

class OutputWriterRinex : public OutputWriter
{
private:
	/* Common bin for GNSS measurements*/
	struct GNSS_MEAS
	{
		double		C1 = 0;			/* Code measurement L1 [m] */
		double		L1 = 0;			/* Phase measurement L1 [cycles] */
		double		D1 = 0;			/* Doppler measurement L1 [Hz] */
		int			sv_id = -1;		/* Space vehicle identification number */
		int			gnss_id = -1;	/* Satellite system identification number (0=GPS,1=SBAS,2=Galileo,3=BeiDou,4=IMES,5=GZSS,6=GLONASS)*/
		int			sig_id = -1;	/* Signal identifier, see https://www.u-blox.com/en/docs/UBX-18010854 page 17*/
		int			freq_id = -1;	/* Frequence ID for GLONASS (slot + 7) */
		long int	locktime;		/* Carrier phase locktime counter (max. 64500 ms) */
		int			mes_qi;			/* Quality indicator (>=4 : PR+DO OK, >=5 : PR+DO+CP OK, <6 : likely loss of carrier lock)*/
		int			cno;			/* Carrier to noise [dB-Hz] */
		int			lol;			/* Loss of lock indicator (RINEX convention) */
		double		C1_stdev;		/* Standard deviation of C1 [m] */
		double		L1_stdev;		/* Standard deviation of L1 [cycles] */
		double		D1_stdev;		/* Standard deviation of D1 [Hz] */
		int			C1_valid;
		int			L1_valid;
		int			half_cycle_valid;
		int			hald_cycle_substracted;

		bool operator < (const GNSS_MEAS &a) const
		{
			if (gnss_id != a.gnss_id)
			{
				return gnss_id < a.gnss_id;
			}
			if (sv_id != a.sv_id)
			{
				return sv_id < a.sv_id;
			}
			return sig_id < a.sig_id;
		}
	};

	// sort operator to sort only according to PRN
	struct SORT_SVID 
	{
		bool operator() (GNSS_MEAS i, GNSS_MEAS j) { return (myConstants::MAX_PRN * i.gnss_id + i.sv_id < myConstants::MAX_PRN * j.gnss_id + j.sv_id); }
	} my_sort_SVID;

	struct RINEX3_CODES
	{
		int row_index = 0;
		std::string code_code;
		std::string code_phase;
		std::string code_doppler;
		std::string code_signal;
	};

	/* Common bin for gnss epoch*/
	struct GNSS_EPOCH
	{
		long int				mjd;				/* modified julian date of day */
		double					sec;				/* seconds within day, with fractional part */
		int						num_meas;			/* number of measurements within epoch */
		int						leap_sec;			/* leap seconds */
		int						leap_sec_valid;		/* leap second flag */
		int						clock_reset;		/* clock reset flag */
		std::vector<GNSS_MEAS>	meas;				/* actual measurements */
		bool operator < (const GNSS_EPOCH &a) const
		{
			return 86400 * mjd + sec < 86400 * a.mjd + a.sec;
		}
	};

	int first_message;

	int mjd_first_epoch;
	double sec_first_epoch;
	int leap_sec;

	int mjd_valid;

	void write_header();
	OutputWriterRinex::GNSS_EPOCH store_RAWX(char * inbuffer);
	void write_message(OutputWriterRinex::GNSS_EPOCH my_gnss_epoch);
	void write_message_2(OutputWriterRinex::GNSS_EPOCH my_gnss_epoch); // Rinex 2
	void write_message_3(OutputWriterRinex::GNSS_EPOCH my_gnss_epoch); // Rinex 3

	int rinex_version;

	std::string get_signals_string_per_GNSS(int gnss_id, bool ca, bool l, bool d, bool cn0);
	RINEX3_CODES get_rinex3_codes(int gnss_id, int sig_id);
	std::string get_constellation_id(int gnss_id);


public:
	struct RINEX_HEADER_META
	{
		std::string run_by;
		std::string observer;
		std::string agency;
		std::string marker_name;
		int marker_number;
		int receiver_number;
		std::string receiver_type;
		std::string receiver_version;
		int antenna_number;
		std::string antenna_type;
		double approx_pos_x;
		double approx_pos_y;
		double approx_pos_z;
		double delta_h;
		double delta_e;
		double delta_n;
		std::string comment;

		RINEX_HEADER_META()
		{
			run_by = "";
			observer = "";
			agency = "";
			marker_name = "";
			marker_number = 999999;
			receiver_number = 999999;
			receiver_type = "";
			receiver_version = "";
			antenna_number = 999999;
			antenna_type = "";
			approx_pos_x = 0;
			approx_pos_y = 0;
			approx_pos_z = 0;
			delta_h = 0;
			delta_e = 0;
			delta_n = 0;
			comment = "";
		}
	};

	RINEX_HEADER_META rinex_header_info;

	OutputWriterRinex(int mjd_valid);
	OutputWriterRinex(int mjd_valid, OutputWriterRinex::RINEX_HEADER_META meta_data);
	~OutputWriterRinex();

	void open_file(std::string file_name);
	void set_meta_data(OutputWriterRinex::RINEX_HEADER_META meta_data);
	void set_rinex_version(int version);
	void set_first_epoch(int mjd, double sec, int leap_sec);
	void parse_and_write(char * inbuffer);

	UbloxSpecs::UBX_HEAD get_reference_header();
};


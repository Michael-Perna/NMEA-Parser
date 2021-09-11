#pragma once
#include "OutputWriter.h"
#include "myConstants.h"
#include "UbloxSpecs.h"
#include <iostream>		// for console output
#include <fstream>		// reading and writing files
#include <iomanip>		// ability to set the length of outputs when writing files
#include <cmath>		// math tools
#include <algorithm>	// e.g. max()
#include <string>
#include <vector>
#include <cinttypes>

class UbxReader
{
public:
	UbxReader();
	UbxReader(double rate);
	~UbxReader();

	std::streampos file_size;
	std::ifstream file;

	std::vector<OutputWriter*> registered_writers; /* a vector of all registered messages */

	bool open_file(std::string file_name);
	void close_file();

	void read_any_message(std::string output_file_name);
	void extract_next_message(char * message, UbloxSpecs::UBX_HEAD * head_out, bool * found);
	void register_reader(OutputWriter * my_writer);
};

///* Common bin for additional information*/
///* Container for NAV_SOL (0x01 0x06) messages */
//struct NAV_EPOCH
//{
//	long int	mjd;			/* modified julian date [days] */
//	double		sec;			/* seconds within day [sec] */
//	int			gpsfix;			/* GPS fix type, range 0..3 (see below) */
//	int			fix_ok;			
//	int			dgps_used;
//	int			valid_week;
//	int			valid_tow;
//	double		ecef_x;			/* ECEF x coordinate [m] */
//	double		ecef_y;			/* ECEF y coordinate [m] */
//	double		ecef_z;			/* ECEF z coordinate [m] */
//	double		p_acc;			/* 3d position accuracy estimate [m] */
//	double		ecef_vx;		/* ECEF x velocity [m/s] */
//	double		ecef_vy;		/* ECEF y velocity [m/s] */
//	double		ecef_vz;		/* ECEF z velocity [m/s] */
//	double		s_acc;			/* speed accuracy estimate [m/s] */
//	double		pdop;			/* position DOP [1e-2] */
//	int			numsv;			/* number of SVs used in nav solution */
//	bool operator < (const NAV_EPOCH &a) const
//	{
//		return 86400*mjd+sec<86400*a.mjd+a.sec;
//	}
//} ;
//std::vector<NAV_EPOCH> my_nav_epoch;

///* Bin for SV_INFO SV */
//typedef struct
//{
//	int channel;		/* channel number */
//	int sv_id;			/* SV id */
//	int svUsed;			/* SV is used for navigation */
//	int diffCorr1;		/* Differential correction data is available for this SV */
//	int orbitAvail;		/* Orbit information is available for this SV (Ephemeris or Almanac) */
//	int orbitEph;		/* Orbit information is Ephemeris */
//	int unhealthy;		/* SV is unhealthy / shall not be used */
//	int orbitAlm;		/* Orbit information is Almanac Plus */
//	int orbitAop;		/* Orbit information is AssistNow Autonomous */
//	int smoothed;		/* Carrier smoothed pseudorange used */
//	int noSignal;		/* no signal */
//	int searchSignal;	/* searching signal */
//	int signalAquired;	/* signal aquired */
//	int signalUnusable;	/* signal detected but unusable */
//	int codeLocked;		/* code locked and time synchronized */
//	int carrierLocked;	/* code and carrier locked and time synchronized */
//	int cno;			/* CN0 [dB-Hz] */
//	int elevation;		/* [deg] */
//	int azimuth;		/* [deg] */
//	double res;			/* pseudo range residuals [m] */
//} SVINFO_BIN;

///* Bin for SV_INFO*/
//typedef struct
//{
//	double		sow; /* seconds within week */
//	int			num_ch;
//	int			generation;
//	std::vector<SVINFO_BIN> sv;
//} SVINFO_EPOCH;
//std::vector<SVINFO_EPOCH> my_svinfo_epoch;
//struct gps_nav_meta
//{
//	int len;
//	int len2;
//	int pos;
//	int pos2;
//	bool sgn;
//	int scale;
//	int save_index;
//	std::string name;
//};
//std::vector< std::vector<gps_nav_meta> > my_specs;

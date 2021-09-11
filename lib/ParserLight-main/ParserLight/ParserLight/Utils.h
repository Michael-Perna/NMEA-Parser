#pragma once
#include <math.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>

class Utils
{
public:
    Utils();

	static double JDfromYMDH(short int year, short int month, short int day, double hour)
	{
		long int jd12h;
		double tjd;

		jd12h = (long)day - 32075L + 1461L * ((long)year + 4800L
			+ ((long)month - 14L) / 12L) / 4L
			+ 367L * ((long)month - 2L - ((long)month - 14L) / 12L * 12L)
			/ 12L - 3L * (((long)year + 4900L + ((long)month - 14L) / 12L)
			/ 100L) / 4L;
		tjd = (double)jd12h - 0.5 + hour / 24.0;

		return (tjd);
	}

	static double MJDfromYMDH(short int year, short int month, short int day, double hour)
	{
		double mjd;
		mjd = JDfromYMDH(year, month, day, hour);
		mjd = mjd - 2400000.5;
		return mjd;
	}

	static void YMDHfromJD(double tjd, short int *year, short int *month, short int *day, double *hour)
	{
		long int jd, k, m, n;
		double djd;

		djd = tjd + 0.5;
		jd = (long int)djd;

		*hour = fmod(djd, 1.0) * 24.0;

		k = jd + 68569L;
		n = 4L * k / 146097L;

		k = k - (146097L * n + 3L) / 4L;
		m = 4000L * (k + 1L) / 1461001L;
		k = k - 1461L * m / 4L + 31L;

		*month = (short int)(80L * k / 2447L);
		*day = (short int)(k - 2447L * (long int)*month / 80L);
		k = (long int)*month / 11L;

		*month = (short int)((long int)*month + 2L - 12L * k);
		*year = (short int)(100L * (n - 49L) + m + k);

		return;
	}

	static void YMDHfromMJD(double mjd, short int *year, short int *month, short int *day, double *hour)
	{
		double jd;
		jd = mjd + 2400000.5;
		YMDHfromJD(jd, year, month, day, hour);
	}

	static void YearDoyFromMJD(double mjd, short int *year, short int *doy)
	{
		short int month;
		short int day;
		double hour;
		double mjd0; // mjd at beginning of the year

		YMDHfromMJD(mjd, year, &month, &day, &hour); // get the date of the mjd
		mjd0 = MJDfromYMDH(*year, 1, 1, 0.0);
		*doy = (short)(mjd - mjd0 + 1);
	}

	static void WDfromMJD(double mjd, short int *week, short int *day)
	{
		double red_mjd;
		red_mjd = mjd - 44244;
		*week = (int)(red_mjd / 7);
		*day = (short)(red_mjd - *week * 7);
	}

	static double MJDfromWD(short int week, double day)
	{
		return week * 7 + 44244 + day;
	}

	static void write_log(std::string my_string)
	{
		std::ofstream log_file("log_file.txt", std::ios_base::out | std::ios_base::app);
		log_file << my_string << std::endl;
	}

	static std::ostringstream fexpn(double my_double, int my_width)
	{

		double k;
		int mexp;
		double mfl;

		if (my_width < 8)
		{
			my_width = 8;
		}

		std::ostringstream ss;
		k = abs(my_double);

		if (k < 1e-99)
		{
			ss << " 0." << std::setw(my_width - 7) << std::setfill('0') << 0 << "D+00";
			return ss;
		}
		else
		{
			mexp = (int) floor(log10(k)) + 1;
			mfl = k / pow(10, mexp);
		}
		
		if (my_double >= 0)
		{
			ss << " " << std::left << std::setw(my_width - 5) << std::setfill('0') << std::setprecision(my_width - 7) << mfl;
		}
		else
		{
			ss << "-" << std::left << std::setw(my_width - 5) << std::setfill('0') << std::setprecision(my_width - 7) << mfl;
		}

		if (mexp >= 0)
		{
			ss << "D+" << std::setw(2) << std::setfill('0') << abs(mexp);
		}
		else
		{
			ss << "D-" << std::setw(2) << std::setfill('0') << abs(mexp);
		}

		return ss;
	}

	/* STATIC Function to get MJD and seconds within day from miliseconds in week (tow) and week */
	static void get_mjd(double tow_ms, int week, int *mjd, double *sec)
	{
		int dow; // day of week
		double tow_sec; // time of week in seconds
		dow = (int)trunc(tow_ms / (1000.0*86400.0));
		*mjd = 44244 + 7 * week + dow;
		tow_sec = (double)tow_ms / 1000.0;
		*sec = tow_sec - 86400 * dow;
		return;
	}

	/* 
    void deg2dms(double deg, int *outdeg, int *outmin, double *outsec);
    //QString deg2dmsString(double dbldeg, int secdecimal);
	//void dms2deg(QString sign, int deg, int min, double sec, double *outdeg);
	//void hms2hour(QString sign, int hour, int min, double sec, double *outhour);
    void hour2hms(double hour, int *outhour, int *outmin, double *outsec);
	//QString hour2hmsString(double dbldeg, int secdecimal);
	//QString hms2hmsString(int hour, int min, double sec,int secdecimal);
	//QString hms2hmsString(int hour, int min, int sec);
	//QString dmy2dmyString(int day, int month, int year);
    void gpst2date(int gpsweek, double secofweek, int *outyear,int *outmonth,int *outday,int *outhour,int *outmin,double *outsec);
    
	<std::string> tokenize (std::string str);
    std::vector<std::string> tokenize (std::string str, char separator);
	*/

};

#pragma once
namespace myConstants
{
	const static long int SPEED_OF_LIGHT = 299792458; // [m s-1]
	const static long double WAVELENGTH1 = SPEED_OF_LIGHT / (154 * 10.23e6); // [m]
	const static long double MG = 398.6004415e12; // [m3 s-2] *DIFFERS FROM GPS-ICD!!!*
	const static long double PI = 3.1415926535898; // pi according to GPS-ICD

	const static double SIMULTANEITY_TOL = 0.030; // Tolerance for simultaneity of measurements

	const static int MAX_PRN = 50; /* maxmimal number of GNSS sats within one system */
	const static int MAX_ID = 10; /* maximal number of GNSS systems, e.g. GPS = 0, GLO = 1, ... */
	const static int MAX_SIG_ID = 10; /* maximal number of signals within one GNSS */

	static const int NB_SLOTS = 40; /* maximal number of channels available */
	static const int MAX_MESSAGE_LENGTH = 4000; /* maximum messange length */
	static const int MAX_SAMPLING_RATE = 10; /* Maximum sampling rate in Hz */
	static const int MAX_MJD_DIFF = 5; /* Maxmimal difference in days between an epoch and the median epoch in a file */
	static const int MAX_VALID_PRN = 100000000; /* Maximal validity range for Code measurements */
};


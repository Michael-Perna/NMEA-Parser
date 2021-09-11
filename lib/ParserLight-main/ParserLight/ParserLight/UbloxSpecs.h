#pragma once
#include <cinttypes>
#include <vector>

class UbloxSpecs
{
public:
	UbloxSpecs();
	~UbloxSpecs();

	static void test_specs();

	/* The uBlox data types */
	typedef uint8_t		ub_u1;
	typedef uint16_t	ub_u2;
	typedef uint32_t	ub_u4;
	typedef int8_t		ub_i1;
	typedef int16_t		ub_i2;
	typedef int32_t		ub_i4;
	typedef float		ub_r4;
	typedef double		ub_r8;

	// for debug purpose
	typedef struct
	{
		unsigned int w1 : 1;
		unsigned int w2 : 1;
		unsigned int w3 : 1;
		unsigned int w4 : 1;
		unsigned int w5 : 1;
		unsigned int w6 : 1;
		unsigned int w7 : 1;
		unsigned int w8 : 1;
	} MY_BYTE;

	/* validity flag */
	typedef struct
	{
		unsigned int valid : 1;	/* Leap second determined */
		unsigned int unused : 7; /* pad up to 8 bits*/
	} VAL_FLAG;

	/* PVT validity flag */
	typedef struct
	{
		unsigned int validDate : 1; /* Date is valid */
		unsigned int validTime : 1; /* Time of day is valid */
		unsigned int fullyResolved : 1; /* UTC time of day fully resolved */
		unsigned int validMag : 1; /* Valid magnetic declination */
		unsigned int unused : 4;
	} PVT_FLAG;

	/* PVT fix status flags */
	typedef struct
	{
		unsigned int gnssFixOK : 1;
		unsigned int diffSoln : 1;
		unsigned int psmState : 3;
		unsigned int headVehValid : 1;
		unsigned int carrSoln : 2;
	} PVT_FIX_FLAG;

	/* PVT additional flags */
	typedef struct
	{
		unsigned int confirmedAvai : 1;
		unsigned int confirmedDate : 1;
		unsigned int confirmedTime : 3;
		unsigned int unused : 3;
	} PVT_ADD_FLAG;

	/* trkStat flag */
	typedef struct
	{
		unsigned int pr_valid : 1;		/* Pseudorange valid */
		unsigned int cp_valid : 1;		/* Carrier phase valid */
		unsigned int half_cyc : 1;		/* Half cycle valid */
		unsigned int sub_half_cyc : 1;	/* Substracted half cycle from phase */
		unsigned int unused : 4;		/* pad up to 8 bits*/
	} TRK_FLAG;

	typedef struct
	{
		unsigned int qualityInd : 3;
		unsigned int svUsed : 1;
		unsigned int health : 2;
		unsigned int diffCorr : 1;
		unsigned int smoothed : 1;
		unsigned int orbitSource : 3;
		unsigned int ephAvail : 1;
		unsigned int almAvail : 1;
		unsigned int anoAvail : 1;
		unsigned int aopAvail : 1;
		unsigned int unused0 : 1;
		unsigned int sbasCorrUsed : 1;
		unsigned int rtcmCorrUsed : 1;
		unsigned int slasCorrUsed : 1;
		unsigned int unused1 : 1;
		unsigned int prCorrUsed : 1;
		unsigned int crCorrUsed : 1;
		unsigned int doCorrUsed : 1;
		unsigned int unused2 : 9;
	} UBX_NAV_SAT_FLAG;

	typedef struct
	{
		unsigned int health : 2;
		unsigned int prSmoothed : 1;
		unsigned int prUsed : 1;
		unsigned int crUsed : 1;
		unsigned int doUsed : 1;
		unsigned int prCorrUsed : 1;
		unsigned int crCorrUsed : 1;
		unsigned int doCorrUsed : 1;
		unsigned int unused : 7;
	} UBX_NAV_SIG_FLAG;

	/* recStat Flag */
	typedef struct
	{
		unsigned int leap_sec : 1;	/* Leap second determined */
		unsigned int clock_res : 1;	/* Clock reset applied */
		unsigned int unused : 6;	/* pad up to 8 bits*/
	} REC_FLAG;

	/* std_flag */
	typedef struct
	{
		unsigned int std_flag : 4;
		unsigned int unused : 4; // pad up to 8 bits
	} STD_FLAG;

	/* NAV-STATUS-FLAG1 */
	typedef struct
	{
		unsigned int gpsFixOk : 1;
		unsigned int diffSoln : 1;
		unsigned int wknSet : 1;
		unsigned int towSet : 1;
		unsigned int reserved : 4;
	} NAV_STATUS_FLAG1;

	/* NAV-STATUS-FLAG2 */
	typedef struct
	{
		unsigned int diffCorr : 1;
		unsigned int carrSolnValid : 1;
		unsigned int reserved : 4;
		unsigned int mapMatching : 2;
	} NAV_STATUS_FLAG2;

	/* NAV-STATUS-FLAG3 */
	typedef struct
	{
		unsigned int psmState : 2;
		unsigned int reserved0 : 1;
		unsigned int spoofDetState : 2;
		unsigned int reserved1 : 1;
		unsigned int carrSoln : 2;
	} NAV_STATUS_FLAG3;

	/* Container for a measurement of RXM_RAWX (0x02 0x15) messages */
	typedef struct
	{
		ub_r8 pr_mes;       /* Pseudorange measurement [m]*/
		ub_r8 cp_mes;       /* Carrier phase measurement [L1 cycles]*/
		ub_r4 do_mes;       /* Doppler measurement [Hz] */
		ub_u1 gnss_id;		/* GNSS identifier*/
		ub_u1 sv_id;        /* Space Vehicle Number */
		ub_u1 sig_id;		/* Signal ID */
		ub_u1 freq_id;		/* GLONASS frequence id: slot + 7 (0 to 13) */
		ub_u2 locktime;		/* Carrier phase locktime counter (max. 64500 ms) */
		ub_u1 cno;          /* Signal strength C/No. (dbHz) */
		STD_FLAG pr_stdev;	/* Estimated pseudorange measurement st. dev.*/
		STD_FLAG cp_stdev;	/* Estimated phase measurement st. dev.*/
		STD_FLAG do_stdev;	/* Estimated doppler measurement st. dev.*/
		TRK_FLAG trk_stat;	/* Tracking status bitfield */
		ub_u1 reserved3;	/* reserved3 */
	} UBX_RXM_RAWX_SV;

	/* Container for a RXM_RAWX (0x02 0x15) messages */
	typedef struct
	{
		ub_r8 rcv_tow;					/* Full resolution time of week in receiver local time [s] */
		ub_u2 week;						/* Measurement GPS week number (Receiver Time)*/
		ub_i1 leap_s;					/* GPS leap seconds (GPS-UTC)*/
		ub_u1 num_meas;					/* Number of measurements to follow */
		REC_FLAG rec_stat;				/* receiver tracking status */
		ub_u1 reserved1[3];				/* reserved1 */
		std::vector<UBX_RXM_RAWX_SV> meas;	/* Measurements */
	} UBX_RXM_RAWX;

	/* Container for a NAV-HPPOSLLH (0x01 0x14) messages */
	typedef struct
	{
		ub_u1 version;
		ub_u1 reserved1[2];
		VAL_FLAG validity;
		ub_u4 iTOW;
		ub_i4 lon;
		ub_i4 lat;
		ub_i4 height;
		ub_i4 hMSL;
		ub_i1 lonHp;
		ub_i1 latHp;
		ub_i1 heightHp;
		ub_i1 hMSLHp;
		ub_u4 hAcc;
		ub_u4 hVacc;
	} UBX_NAV_HPPOSLLH;

	/* Container for a NAV-CLOCK (0x01 0x22) messages */
	typedef struct
	{
		ub_u4 iTOW;		// time of week in ms
		ub_i4 clkB;		// clock bias in ns
		ub_i4 cldD;		// clock drift in ns/s
		ub_u4 tAcc;		// time accuracy estimate in ns
		ub_u4 fAcc;		// frequency accuracy estimate in ps/s
	} UBX_NAV_CLOCK;

	/* Container for a NAV-DOP (0x01 0x04) messages */
	typedef struct
	{
		ub_u4 iTOW;	// time of week in ms
		ub_u2 gDOP; // 0.01
		ub_u2 pDOP; // 0.01
		ub_u2 tDOP; // 0.01
		ub_u2 vDOP; // 0.01
		ub_u2 hDOP; // 0.01
		ub_u2 nDOP; // 0.01
		ub_u2 eDOP; // 0.01
	} UBX_NAV_DOP;

	/* Container for a NAV-PVT (0x01 0x07) messages */
	typedef struct
	{
		ub_u4 iTOW;	// time of week in ms
		ub_u2 year;
		ub_u1 month;
		ub_u1 day;
		ub_u1 hour;
		ub_u1 min;
		ub_u1 sec;
		PVT_FLAG valid;
		ub_u4 tAcc; // time accuracy estimate (UTC)
		ub_i4 nano; // ns
		ub_u1 fixType; // 0 = no fix; 1 = dead reckoning only; 2 = 2D - fix; 3 = 3D - fix; 4 = GNSS + dead reckoning combined; 5 = time only fix
		PVT_FIX_FLAG fix_status_flag;
		PVT_ADD_FLAG add_flags;
		ub_u1 numSV;
		ub_i4 lon; // 1e-7 deg
		ub_i4 lat; // 1e-7 deg
		ub_i4 height; // mm
		ub_i4 hMSL; // mm
		ub_u4 hAcc; // mm
		ub_u4 vAcc; // mm
		ub_i4 velN; // mm/s
		ub_i4 velE; // mm/s
		ub_i4 velD; // mm/s
		ub_i4 gSpeed; // mm/s
		ub_i4 headMot; // 1e-5 deg
		ub_u4 sAcc; // mm
		ub_u4 headAcc; // 1e-5 mm
		ub_u2 pDOP; // 0.01
		VAL_FLAG invalidLlh;
		ub_u1 reserved0[5];
		ub_i4 headVeh; // 1e-5 deg
		ub_i2 magDec; // 1e-5 deg
		ub_u2 magAcc; // 1e-5 deg
	} UBX_NAV_PVT;

	/* Container for a NAV-SAT-SV */
	typedef struct
	{
		ub_u1 gnssId;
		ub_u1 svId;
		ub_u1 cno; // dB-Hz
		ub_i1 elev; // deg
		ub_i2 azim; // deg
		ub_i2 prRes; // 0.1 m
		UBX_NAV_SAT_FLAG qualityFlag;
	} UBX_NAV_SAT_SV;

	/* Container for a NAV-SAT (0x01 0x35) messages */
	typedef struct
	{
		ub_u4 iTOW;	// time of week in ms
		ub_u1 version;
		ub_u1 numSvs;
		ub_u1 reserved0[2];
		std::vector<UBX_NAV_SAT_SV> sats;
	} UBX_NAV_SAT;

	/* Container for a NAV-SIG-Signals */
	typedef struct
	{
		ub_u1 gnssId;
		ub_u1 svId;
		ub_u1 sigId;
		ub_u1 freqId;
		ub_i2 prRes; // 0.1 m
		ub_u1 cno; // dB-Hz
		ub_u1 qualityInd;
		ub_u1 corrSource;
		ub_u1 ionoModel;
		UBX_NAV_SIG_FLAG sigFlags;
		ub_u1 reserved1[4];
	} UBX_NAV_SIG_SIG;

	/* Container for a NAV-SIG (0x01 0x43) messages */
	typedef struct
	{
		ub_u4 iTOW;	// time of week in ms
		ub_u1 version;
		ub_u1 numSig;
		ub_u1 reserved0[2];
		std::vector<UBX_NAV_SIG_SIG> sigs;
	} UBX_NAV_SIG;

	/* Container for a NAV-STATUS (0x01 0x03) messages */
	typedef struct
	{
		ub_u4 iTOW;	// time of week in ms
		ub_u1 gpsFix;
		NAV_STATUS_FLAG1 flags;
		NAV_STATUS_FLAG2 fixStat;
		NAV_STATUS_FLAG3 flags2;
		ub_u4 ttff; // ms
		ub_u4 msss; // ms
	} UBX_NAV_STATUS;

	/* Header of a UBX message */
	typedef struct
	{
		ub_u1 sig[2];	/* The UBX signature (see above) */
		ub_u1 cl;		/* The class number */
		ub_u1 id;       /* The message number (id) */
		ub_u2 len;      /* The payload length */
	} UBX_HEAD;

	/* The checksum of a UBX message */
	typedef struct
	{
		ub_u1 a;          /* CK_A */
		ub_u1 b;          /* CK_B */
	} UBX_CHECKSUM;

	/* STATIC Function to calculate the checksum of a UBX message */
	static UbloxSpecs::UBX_CHECKSUM ubx_gen_checksum(char *payload, UbloxSpecs::UBX_HEAD ub_head)
	{
		int i;
		UbloxSpecs::UBX_CHECKSUM temp;
		temp.a = ub_head.cl + ub_head.id + (ub_head.len & 0xff) + (ub_head.len >> 8);
		temp.b = 4 * ub_head.cl + 3 * ub_head.id + 2 * (ub_head.len & 0xff) + (ub_head.len >> 8);
		for (i = 0; i < ub_head.len; i++) {
			temp.a += payload[i];
			temp.b += temp.a;
		}
		return(temp);
	}

	/* STATIC Function to checksum a UBX message */
	static bool ub_checksum(char *payload, UbloxSpecs::UBX_HEAD ub_head, UbloxSpecs::UBX_CHECKSUM ub_chksum)
	{
		UbloxSpecs::UBX_CHECKSUM temp;

		if (ub_head.len == 0) { return false; }

		temp = ubx_gen_checksum(payload, ub_head);
		return(temp.a == ub_chksum.a && temp.b == ub_chksum.b);
	}
};


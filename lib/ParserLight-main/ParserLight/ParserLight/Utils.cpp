#include "stdafx.h"
#include "Utils.h"
using namespace std;

Utils::Utils()
{

}

/*
void Utils::deg2dms(double deg, int *outdeg, int *outmin, double *outsec)
{
        //Calcul en degrés sexagésimaux
        if(deg>=0)
        {
                double deg_temp=(double)((int)deg);
                double deg_min_temp=(deg-deg_temp)*60;
                double min_temp=(double)((int)deg_min_temp);
                double sec_temp=(deg_min_temp-min_temp)*60;

                *outdeg=(int)deg_temp;
                *outmin=(int)min_temp;
                *outsec=sec_temp;

        }
        if(deg<0)
        {
                double deg_temp=(double)((int)deg);
                double deg_min_temp=-(deg-deg_temp)*60;
                double min_temp=(double)((int)deg_min_temp);
                double sec_temp=(deg_min_temp-min_temp)*60;

                *outdeg=(int)deg_temp;
                *outmin=(int)min_temp;
                *outsec=sec_temp;
        }

}

void Utils::dms2deg(QString sign, int deg, int min, double sec, double *outdeg)
{
        double dbldeg=fabs(deg);
        double dblmin=fabs(min);
        double dblsec=fabs(sec);

        if(sign=="+")
        {
                *outdeg=dbldeg+(dblmin/60.0)+(dblsec/3600.0);
        }
        else
        {
                *outdeg=-dbldeg-(dblmin/60.0)-(dblsec/3600.0);
        }
}


void Utils::hms2hour(QString sign, int hour, int min, double sec, double *outhour)
{
        double dblhour=hour;
        double dblmin=min;
        double dblsec=sec;

        if(sign=="+")
        {
                *outhour=dblhour+(dblmin/60.0)+(dblsec/3600.0);
        }
        else
        {
                *outhour=-dblhour-(dblmin/60.0)-(dblsec/3600.0);
        }
}

void Utils::hour2hms(double hour, int *outhour, int *outmin, double *outsec)
{
        double hour_temp=(double)((int)hour);
        double hour_min_temp=(hour-hour_temp)*60.0;
        double min_temp=(double)((int)hour_min_temp);
        double sec_temp=(hour_min_temp-min_temp)*60.0;

        *outhour=(int)hour_temp;
        *outmin=(int)min_temp;
        *outsec=sec_temp;

}


QString Utils::deg2dmsString(double dbldeg, int secdecimal)
{
        int deg; int min; double sec;
        QString txtdms;
        QString txtdeg;
        QString txtmin;
        QString txtsec;

        deg2dms(dbldeg,&deg,&min,&sec);

        if(dbldeg < 0 && deg == 0)
            txtdeg = "- " + QString::number(deg) + " \xB0 ";
        else
                txtdeg = QString::number(deg) + " \xB0 ";


        if(min<10)
        {
                txtmin = "0" + QString::number(min) + " \xB4 ";
        }
        else
        {
                txtmin = QString::number(min) + " \xB4 ";
        }

        if(sec<=9.9999999)
        {
                txtsec = "0" + QString::number(sec,'f',secdecimal) + " \xB4\xB4";
        }
        else
        {
                txtsec = QString::number(sec,'f',secdecimal) + " \xB4\xB4";
        }


        txtdms = txtdeg + txtmin + txtsec;

        return txtdms;
}

QString Utils::hour2hmsString(double dblhour,int secdecimal)
{
        int hour; int min; double sec;
        QString txthms;
        QString txthour;
        QString txtmin;
        QString txtsec;

        hour2hms(dblhour,&hour,&min,&sec);

        txthour = QString::number(hour) + " h ";

        if(min<10)
        {
                txtmin = "0" + QString::number(min) + " min ";
        }
        else
        {
                txtmin = QString::number(min) + " min ";
        }

        if(sec<10)
        {
                txtsec = "0" + QString::number(sec,'f',secdecimal) + " sec";
        }
        else
        {
                txtsec = QString::number(sec,'f',secdecimal) + " sec";
        }


        txthms = txthour + txtmin + txtsec;

        return txthms;
}

QString Utils::hms2hmsString(int hour, int min, double sec,int secdecimal)
{
        QString txthms;
        txthms = QString::number(hour) + " h ";

        if(min<10)
        {
                txthms += "0" + QString::number(min) + " min ";
        }
        else
        {
                txthms += QString::number(min) + " min ";
        }

        if(sec<10)
        {
                txthms += "0" + QString::number(sec,'f',secdecimal) + " sec";
        }
        else
        {
                txthms += QString::number(sec,'f',secdecimal) + " sec";
        }

        return txthms;
}

QString Utils::hms2hmsString(int hour, int min, int sec)
{
        QString txthms;
        txthms = QString::number(hour) + " h ";

        if(min<10)
        {
                txthms += "0" + QString::number(min) + " min ";
        }
        else
        {
                txthms += QString::number(min) + " min ";
        }

        if(sec<10)
        {
                txthms += "0" + QString::number(sec) + " sec";
        }
        else
        {
                txthms += QString::number(sec) + " sec";
        }

        return txthms;
}

QString Utils::dmy2dmyString(int day, int month, int year)
{
        QString txtdmy;
        txtdmy = QString::number(day) + ".";

        if(month<10)
        {
                txtdmy  += "0" + QString::number(month) + ".";
        }
        else
        {
                txtdmy  += QString::number(month) + ".";
        }

        txtdmy  += QString::number(year);

        return txtdmy;
}


void Utils::gpst2date(int gpsweek, double secofweek, int *outyear,int *outmonth,int *outday,int *outhour,int *outmin,double *outsec)
{

    double days2origin = gpsweek*7 + secofweek/(3600.0*24.0);
    double JD_GPSOrigin =JDfromYMDH(1980,01,06,0);
    double JD = JD_GPSOrigin + days2origin;

    short int year;short int month;short int day;double hour;
    YMDHfromJD(JD,&year,&month,&day,&hour);

    *outyear = year;
    *outmonth = month;
    *outday = day;

    hour2hms(hour, outhour, outmin, outsec);
}

std::vector<std::string> Utils::tokenize (std::string str)
{
    std::istringstream s(str);
    std::vector <std::string> v;
    std::string tok;

    while(std::getline(s,tok,';'))
    {
        v.push_back(tok);
    }

    return v;

}

std::vector<std::string> Utils::tokenize (std::string str, char separator)
{
    std::istringstream s(str);
    std::vector <std::string> v;
    std::string tok;

    while(std::getline(s,tok,separator))
    {
        v.push_back(tok);
    }

    return v;

}

*/
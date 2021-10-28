# NMEA-Parser
This parser read from mixed text type encoding (ubx end utf-8) and it extract nmea message. The output is a database structure divided into single days and in form of a text file.
It his higly robust to unwanted and unexpected charachters which is ideally for long NMEA series off messages. The code works throuhg an GUI which allow to select single txt file or entire folder. The files must have the .txt extension

## make_database.py
This script concatanate NMEA files inside a folder into a database according to the date in their names. It's created to work order the NMEA files generate by the MovingLab project.

## **parse-nmea**
deprecated

## **parse-nmea_plus.py**
This script add a new functionnality which can extract all the GSV messages and output as a database which order all the parameters of the satellite signals used in the GNSS solution by epochs which should facilitate an analysis for GNSS mulipath. 

## **getrinex.py**
This script his an attempt to automitize the process of extracting the wanted rinex thanks to the ParserLigth of @bpshop. 

## **env.txt** 
It contain the requirement to the script to work

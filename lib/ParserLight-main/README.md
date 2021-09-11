# ParserLight
Parser for ublox UBX files

# Call
ParserLight input_file [argument value]

For instance:
ParserLight.exe ubxdatatest20112020.ubx rate 60 pos 1 wait 1 
ParserLight.exe ant2_az0.ubx rinex 1 mjd 57919 wait 1 

## Options

### General options
#### rate
Output rate in seconds. Default = 0 exports all messages.

#### threshold
Threshold for the above mentionned option. Default is 0.02 seconds.

#### wait
Tell the console to stay open at the end of the processing (1) or close automatically (0). Default = 0.

#### o
Optional output file name. Default = input file name

### Rinex optios
Rinex observation files output.

#### rinex
Enable (1) or disable (0) Rinex Observations 2.11 format files (Rinex 3.0 is not implemented yet, only Obs files are generated).
If this option is enabled, mjd is mandatory.

#### mjd
It is mandatory to provide the Modified Julian Date of the desired epochs if a rinex output is chosen.

#### rinex_version
Set this variable to 2 or 3, to switch either to Rinex Version 2.11 or to Rinex Version 3.04 output. Default is set to 3. Please consider, that with Rinex 2.11, only L1 C/A will be output. To benefit from mutli-frequency signals, use Rinex 3.04.

### Rinex header options
#### run_by
#### observer
#### agency
#### marker_name
#### marker_number
#### receiver_number
#### receiver_type
#### receiver_version
#### antenna_number
#### antenna_type
#### approx_pos_x
#### approx_pos_y
#### approx_pos_z
#### delta_h
#### delta_e
#### delta_n
#### comment

### Other messages
All these messages are disabled by default

#### pos (NAV-HPPOSLLH)
Enable the csv output of NAV-HPPOSLLH with the option "clock 1"

#### clock (NAV-CLOCK)
Enable the csv output of NAV-CLOCK with the option "clock 1"

#### dop (NAV-DOP)
Enable the csv output of NAV-DOP with the option "dop 1"

#### pvt (NAV-DOP)
Enable the csv output of NAV-PVT with the option "pvt 1"

#### sat (NAV-SAT)
Enable the csv output of NAV-SAT with the option "sat 1"

#### signal (NAV-SIG)
Enable the csv output of NAV-SIG with the option "signal 1"

#### status (NAV-STATUS)
Enable the csv output of NAV-STATUS with the option "status 1"

# Compilation under Linux
Software not tested under Linux!

stdafx_for_linux.h instead of stdafx.h (rename it accordingly)

g++ -c stdafx.h -o stdafx.h.gch

g++ -std=c++11 ParserLight.cpp OutputWriter.cpp Utils.cpp UbloxSpecs.cpp UbxReader.cpp OutputWriterRinex.cpp OutputWriterPos.cpp -I . -o ParserLight


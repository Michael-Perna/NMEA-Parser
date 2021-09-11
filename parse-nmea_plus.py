# -*- coding: utf-8 -*-
"""
Created on Mon Feb  1 08:12:26 2021

@author: Michael
"""


# -*- coding: utf-8 -*-
"""
Created on Thu Jan  7 07:48:15 2021

    PLUS - VERSION
    The  PLUS version create new data frame which include the VGS messages 
    neccessary for the multipath analysis


    This parser is heavly dependent on the ZDA message without it does not work

@author: Michael
"""

# from csv import reader
# import csv
# import codecs
import iso8601
import pytz
from datetime import datetime
# import logging
import pandas as pd
import lib.gui_parsenmea_plus as gui
import os
import re
from tqdm import tqdm

class ReadNmea:
    
    def __init__(self, sourcefile, get_nmea, get_gsv):
        self.sourcefile = sourcefile
        self.get_nmea = get_nmea
        self.get_gsv = get_gsv
        self.timestamp = None
        self.posMode = 'Missing'
        self.time = None
        self.count_invalid = 0
        self.count_valid = 0
        self.skipped_msg = 0
        self.exist_fileA = False
        self.exist_fileB = False
        self.filenameA= None
        self.filenameB =None
        self.receiver_name = None
        self.index = 0
        self.start = False
        # The time is syncronizde on the zda time. 
        self.wait_zda = False
        self.db_path = './DataBase'
        self.endmessage =f'''
        
 End of process
 Number of parsed message: {self.count_valid + self.count_valid}
 Number of invalid NMEA message: {self.count_invalid}
 Number of skipped messages: {self.skipped_msg}
                    '''
                    
        # This code is ade to work as batch process
        # then this code is specific for the followg filenames
        if 'sapcorda' in self.sourcefile:
            self.receiver_name = 'sapcorda.snmea'
            self.receiver_name_b = 'sapcorda.signals'
        elif 'swipos' and 'ublox' in self.sourcefile:
            self.receiver_name = 'swipos_ublox.snmea'
            self.receiver_name_b = 'swipos_ublox.signals'
        elif 'swipos' and ('UART' or 'uart') in self.sourcefile:
            self.receiver_name = 'swipos_ublox.snmea'
            self.receiver_name_b = 'swipos_ublox.signals'
        elif 'swipos' in self.sourcefile:
            self.receiver_name = 'swipos_ublox.snmea'
            self.receiver_name_b = 'swipos_ublox.signals'
        elif 'netR9' or 'NetR9' or 'netr9' in self.sourcefile:
            self.receiver_name = 'swipos_NetR9.snmea'
            self.receiver_name_b = 'swipos_NetR9.signals'
        else:
            self.recevier_name = 'unkown.csv'
            
        basename = os.path.basename(self.sourcefile)

        try:
            date = basename[0:10]
            self.date = datetime.strptime(date, '%Y-%m-%d')
            self.dateGSV = datetime.strptime(date, '%Y-%m-%d')
        except:
            # print(e)
            pass
        try:
            date = basename[0:10]
            self.date = datetime.strptime(date, '%d-%m-%Y')
            self.dateGSV = datetime.strptime(date, '%d-%m-%Y')
        
        except Exception as e:
            print(e)
            print(date, ':()')
            pass


    def parser(self):
        
        # Create Data Frame 
        columns = ['timestamp', 'lon', 'stdLong', 'lat', 'stdLat', 'alt',
                   'stdAlt', 'sep', 'rangeRMS', 'posMode', 'numSV', 'difAge',
                   'numGP', 'numGL', 'numGA', 'numGB', 'opMode', 'navMode',
                   'PDOP', 'HDOP', 'VDOP', 'stdMajor', 'stdMinor', 'orient']
        self.df = pd.DataFrame(data=None, index=None,
                               columns=columns, dtype=None, copy=False)
        
        # Create Data Frame for satellites signals
        columns = ['timestamp', 'signalId', 'svId', 'elv', 'az', 'cno']
        self.dfB = pd.DataFrame(data = None, index = None, columns = columns,
                               copy = False)
        
        pbar = tqdm(total=100)
        # Open file in read mode as bytes'
        with open(self.sourcefile, 'rb') as read_obj:
            
            byte_line = read_obj.readline()
            
            while byte_line:
                # keep only valid nmea (as string)
                str_line = self.search_nmea(byte_line)
                
                # Skip on None type object
                if str_line==None:
                    # Advanced of one line
                    byte_line = read_obj.readline()
                    continue 
                
                # remove chesum from nmea message 
                str_line[-1], cksum = str_line[-1].split('*')
                
                # singular case
                if str_line[0][-3:].lower()=='bs' \
                    or str_line[0][-3:].lower()=='ns' \
                        or str_line[0][-3:].lower()=='ls' \
                            or str_line[0][-3:].lower()=='nb':
                    byte_line = read_obj.readline()
                    continue
                
                # select message name and function
                if str_line[0][-3:].lower() == 'zda':
                    getattr(ReadNmea, str_line[0][-3:].lower())(self, str_line)
                   
                if str_line[0][-3:].lower() == 'gsv' and self.get_gsv:
                    getattr(ReadNmea, str_line[0][-3:].lower())(self, str_line)
                elif str_line[0][-3:].lower() != 'gsv' and self.get_nmea:
                    getattr(ReadNmea, str_line[0][-3:].lower())(self, str_line)


                # extract index
                index = self.df.index

                # extract index
                indexB = self.dfB.index
                '''check if the line as been fully filled

                This help to reduce the neccessary flash memory used to
                process this code by just keep one output line at the 
                time. 
                
                '''

                # Write in "<receiver>.nmea" file
                if len(index)>2:
                    # open "<receiver>.nmea" file
                    if not self.exist_fileA:
                        self.filenameA = self.createfile()
                        self.exist_fileA = True
                    # Write the line in "<receiver>.nmea" file
                    self.writefile(self.df, index[-2])
                    # Drop the saved row
                    self.df = self.df.drop(index[-2])


                # Write in "<receiver>.signal" file
                if len(indexB)>2:
                    # open "<receiver>.signal" file
                    if not self.exist_fileB:
                        self.filenameB = self.createGSV_file()
                        self.exist_fileB = True

                    # Write the line in "<receiver>.signal" file
                    self.writeGSV_file(self.filenameB, self.dfB, indexB[-2])
                    # Drop the saved row
                    self.dfB = self.dfB.drop(indexB[-2])
                    
                # Advanced of one line
                byte_line = read_obj.readline()
                pbar.update(1)
        pbar.close()
        print(self.endmessage)
        return self.df
    
    def search_nmea(self, byte_line):
        # /!\ I suppose there is maximum one correct nmea msg pro line
        
        # Initialize of parameters
        nmea=None # output
        # look for '$' sign in the byte string 
        for x in re.split(b'\$', byte_line):
            # catch only nmea message 
            try:
                # skip empty 'x' generated by 're.split'
                if x==b'':
                    continue
                else:
                    str_line = x.decode('utf-8')
                    
            except:
                continue
            
            # some ubx messages still get decoded
            # This dictionnary try to remove them
            if str_line.find('\x00')!= -1:
                continue
            
            elif str_line.find('\x01')!=-1:
                continue
            elif str_line.find('\x02')!=-1:
                continue
            elif str_line.find('\x03')!=-1:
                continue
            elif str_line.find('\x04')!=-1:
                continue
            elif str_line.find('\x05')!=-1:
                continue
            elif str_line.find('\x06')!=-1:
                continue
            elif str_line.find('\x07')!=-1:
                continue
            elif str_line.find('\x08')!=-1:
                continue
            elif str_line.find('\x09')!=-1:
                continue
            elif str_line.find('\x09')!=-1:
                continue
            elif str_line.find('\x11')!=-1:
                continue
            elif str_line.find('\x1a')!=-1:
                continue
            
            line = re.split(',', str_line)
            # Check nma validityy
            if self.checksum(','.join(line)):
                # Save nmea message in a list
                # print(line)
                nmea = line
                return nmea
         
        return nmea

    
    def checksum(self, nmea_msg):
        
        if nmea_msg == '':
            return False

        # message without '*' are considered invalid
        try:
            # split the message and the cheksum
            cksumdata, cksum = nmea_msg.split('*')
            # remove first charachter '$'
            # empty, cksumdata = cksumdata.split('$')
            
        except:
            # print(e, ': ', nmea_msg, 'La vita è bella')
            self.count_invalid += 1
            return False 
        
        # Some times the EOL charaters is missing
        try:
            # remove EOL charachter '\r\n'
            cksum, eol = cksum.split('\r\n')
        except:
            # print(e, ': ', nmea_msg)
            pass
            
            
        # cksum with more than two digit are considered invalid
        if len(cksum) > 3:
            self.count_invalid +=1
            return False
        
        # cksum wich are not base 16 are considered invalid
        try:
            int(cksum, 16)
        except:
            # print(e, ': ', nmea_msg)
            self.count_invalid += 1
            return False 
        
        
        # Initializing first XOR value
        csum = 0 
        for c in cksumdata:
            # XOR'ing value of csum against the next char in line
            # and storing the new XOR value in csum
           csum ^= ord(c)
        
        # if csum is equal do cksum the message is valid
        if hex(csum) == hex(int(cksum, 16)):
            
            self.count_valid += 1
            return True
    
        else:
            self.count_invalid += 1
            return False
    
    def insertValues(self, time, dict):
        # If the first zda message it is arrived the algorithm start to fill 
        # the database
        
        if self.wait_zda or self.start: 
            # print(time)
            # if there is a new valid time create a new line
            if not (self.df.index == time).any() and \
                (self.df.index == time).size <= 0 :
                    # create new line
                    self.df.append(pd.Series(name=time,dtype='float64'))
    
            for i, n in dict.items():  
                # append row to the dataframe
                self.df.loc[time, i] = n

            # Say to other insertValues function it can start
            self.start = True

    def insertGSV_Values(self, idx, dict):
        # If the first zda message it is arrived the algorithm start to fill
        # the database

        
        if self.wait_zda or self.start:

            # Loop over each signal in GSV message
            # if there is a new valid time create a new line
            # if not (self.df.index == sig_index).any() and
            # (self.df.index == sig_index).size <= 0 :
            #         # create new line
            self.dfB.append(pd.Series(name = idx, dtype = 'float64'))

            for i, n in dict.items():
                # append row to the dataframe
                self.dfB.loc[idx, i] = n

            # Say to other insertValues function it can start
            self.start = True

    def createfile(self):

        # Extract the month day 
        year = self.date.year
        # month = self.date.month
        month2 = self.date.strftime('%B')
        day = self.date.day
        
        # Compose folder name
        folder_name = self.db_path + '/' \
            + str(year) + '/' \
            + month2 + '/' \
            + str(day).zfill(2)
        
        # if the directory does not exist it create it 
        if not os.path.isdir(folder_name):
            os.makedirs(folder_name)
            print("created folder : ", folder_name)
        
        # Compose unique name of file
        file_name = folder_name + '/' + self.receiver_name
        file_name = self.uniquefilename(file_name)
        if self.get_nmea:
            self.filenameA = file_name
        if self.get_gsv:
            self.filenameB = file_name

        print("Create new file : ", file_name)

        return file_name

    def createGSV_file(self):
        # Extract the month day
        year = self.dateGSV.year
        # month = self.date.month
        month2 = self.dateGSV.strftime('%B')
        day = self.dateGSV.day

        # Compose folder name
        folder_name = self.db_path + '/' \
            + str(year) + '/' \
            + month2 + '/' \
            + str(day).zfill(2) + '/' \
            + 'signals'
    
        # if the directory does not exist it create it
        if not os.path.isdir(folder_name):
            os.makedirs(folder_name)
            print("created folder : ", folder_name)

        # Compose unique name of file
        file_name = folder_name + '/' + self.receiver_name_b
        file_name = self.uniquefilename(file_name)
        self.filenameB = file_name
        print("Open new file : ", file_name)

        return file_name

    def uniquefilename(self, path_name):
        if os.path.isfile(path_name):
            info = os.path.splitext(path_name)
            path_name = info[0] + '-copy' + info[1]
            path_name = self.uniquefilename(path_name)
        return path_name


    def writefile(self, df, index):
        #### NOT EFFICIENT ######
        # Check that month day correspond to the actuall timeseries
        date_new = datetime.strptime(self.timestamp, '%Y-%m-%dT%H:%M:%SZ')

        # Open new file with updated date
        if not date_new.day == self.date.day:
            #Update the date
            self.date = date_new
            # Open new file with updated date
            self.filenameA = self.createfile()
        #### until here NOT EFFICIENT ######


        # Open The File
        if type(self.filenameA)==str:
            self.filenameA = open(self.filenameA, 'a')
        else:
            self.filenameA = open(self.filenameA.name, 'a')

        # print(self.df.loc[[index]])
        df.loc[[index]].to_csv(path_or_buf=self.filenameA,
                                sep=',',
                                na_rep='',
                                header=False,
                                index=False, 
                                index_label=True,
                                mode='a',
                                line_terminator = '\n')

        # Close the file
        self.filenameA.close()

    def writeGSV_file(self, df, index):
        #### NOT EFFICIENT ######
        # Check that month day correspond to the actuall timeseries
        date_new = datetime.strptime(self.timestamp, '%Y-%m-%dT%H:%M:%SZ')

            
        # Open new file with updated date
        if not date_new.day == self.dateGSV.day:
            #Update the date
            self.dateGSV = date_new
            # Open new file with updated date
            self.filenameB = self.createGSV_file()
        #### until here NOT EFFICIENT ######

        # Open The File
        if type(self.filenameB)==str:
            self.filenameB = open(self.filenameB, 'a')
        else:
            self.filenameB = open(self.filenameB.name, 'a')

        # print(self.df.loc[[index]])
        df.loc[[index]].to_csv(path_or_buf=self.filemameB,
                                sep=',',
                                na_rep='',
                                header=False,
                                index=False, 
                                index_label=True,
                                mode='a',
                                line_terminator = '\n')

        # Close the file
        self.file_name.close()
        
    def utcrcf3339(self,_date):
        _date_obj = iso8601.parse_date(_date)
        _date_utc = _date_obj.astimezone(pytz.utc)
        _date_utc_zformat = _date_utc.strftime('%Y-%m-%dT%H:%M:%SZ')
        return _date_utc_zformat

    def zda(self, row):
        # Message: Time and date
        
        # ZDA message exemple:
        #
        #   __________________________________
        #  |__0__|_1__|_2_|__3__|_4__|___5____|
        #  | Name|time|day|month|year|timezone|
        # ['$GNZDA', '120008.02', '27', '12', '2020', '00', '00*77']
    

        # Create a UTC RCF3339 timestamp
        _date=row[4] + row[3] + row[2] + 'T' + row[1]
        # print(_date)
        # print(row[1]!='' and row[2]!='' and row[3]!='' and row[4]!='')
        # Exit if empty date
        if row[1]!='' and row[2]!='' and row[3]!='' and row[4]!='':
            self.timestamp = self.utcrcf3339(_date)
            self.time = row[1]
            
            # As the first zda mesage is arrived change the flag name
            self.wait_zda = True
            # Insert values in df at the right timestamp 
            dict = {'timestamp': self.timestamp}
            self.insertValues(self.time, dict)
        

    def gns(self, row):
        # Message: GNSS fix data
        
        # GNGNS message example:
        #
        #  ____________________________________________________________________________________________________________________________________________
        # |____0___|_____1______|_______2________|_3__|________4________|_5__|____6____|__7__|__8___|____9_____|___10____|__11___|____12_____||
        # |__Name__|____time____|___longitude_____|_NS_|____latitude____|_EW_|_posMode_|numSV|_HDOP_|_altitude_|___sep___|diffAge|diffStation||
        # ['$GNGNS', '120013.00', '4655.66628490', 'N', '00727.09259001', 'E', 'RRRRNN', '19', '0.7', '572.413', '48.020',  '' , '*59']
        # ['$GPGNS', '120013.00', '', '', '', '', '', '6', '', '', '', '1.0', '1964*71']
        # ['$GLGNS', '120013.00', '', '', '', '', '', '6', '', '', '', '1.0', '1964*6D']
        # ['$GAGNS', '120013.00', '', '', '', '', '', '6', '', '', '', '1.0', '1964*60']
        # ['$GBGNS', '120013.00', '', '', '', '', '', '1', '', '', '', '1.0', '1964*64']
        # time = row[1]
        if row[0] == 'GNGNS':
            self.posMode = row[6]
            lat = row[2]
            lon = row[4]
            numSV = row[7]
            alt = row[8]
            sep = row[9]
            
            # HDOP = row[7]
            difAge = row[11]
            
            # Fill the table
            # Insert lon in df
            dict = {'lat': lat,
                    'lon':lon,
                    'alt':alt,
                    'sep': sep,
                    'numSV':numSV,
                    'posMode':self.posMode,
                    'difAge': difAge
                    }
                    
                    
            self.insertValues(self.time, dict)
            
        else:
    ####### /!\ difAge diffStation navStatus are given for each 
    #######     constellation. Here just one is saved
    #######     To BE CHANGED  v
            # Specify number of satellite pro constellation
            if row[0] == 'GPGNS':
                numGP = row[7]
                difAgeGP = row[11]
                # diffStationGP = row[12]

                # Fill the table
                dict = {'numGP': numGP, 
                        'difAge': difAgeGP,
                        # 'posModeGP':self.posMode[0]
                        }
                self.insertValues(self.time, dict)

                        
            if row[0] == 'GLGNS':
                numGL = row[7]
                # difAgeGL = row[11]
                # diffStationGL = row[12]
                
                # Fill the table
                dict = {'numGL': numGL,
                        # 'difAgeGL': difAgeGL,
                        # 'posModeGL':self.posMode[1]
                        }
                self.insertValues(self.time, dict)

            if row[0] == 'GAGNS':
                numGA = row[7]
                # difAgeGA = row[11]
                # diffStationGA = row[12]
                
                # Fill the table
                dict = {'numGA': numGA,
                        # 'difAgeGA': difAgeGA,
                        # 'posModeGA':self.posMode[2]
                        }
                self.insertValues(self.time, dict)
            
            if row[0] == 'GBGNS':
                # difAgeGB = row[11]
                numGB = row[7]
                # diffStationGB = row[12]
                
                # Fill the table
                dict = {#'difAgeGB': difAgeGB, 
                        'numGB': numGB,
                        # 'posModeGB':self.posMode[3]
                        }
                self.insertValues(self.time, dict)
                
               
    def gsa(self, row):
        # GNSS DOP and active satellite
        
        # GNGSA message example:
        #  ___________________________________________________
        # |_0__|__1___|___2___|_3__|_4__|_5__|_6__|_____7_____|
        # |name|opMode|navMode|svid|PDOP|HDOP|VDOP|systemId*cs|
        # ['$GNGSA', 'A', '3', '28', '5', '7', '13', '15', '30', '', '', '', '', '', '', '1.3', '0.7', '1.1*24']
        # ['$GNGSA', 'A', '3', '77', '68', '67', '66', '76', '78', '', '', '', '', '', '', '1.3', '0.7', '1.1*28']
        # ['$GNGSA', 'A', '3', '25', '2', '5', '24', '8', '3', '', '', '', '', '', '', '1.3', '0.7', '1.1*24']
        # ['$GNGSA', 'A', '3', '114', '', '', '', '', '', '', '', '', '', '', '', '1.3', '0.7', '1.1*1D']
        opMode = row[1]
        navMode = row[2]
        # svid = row[3]
        PDOP = row[4]
        HDOP = row[5]
        VDOP = row[6]
        # systemId = row[7]
        
         # Fill the table
        dict = {'timestamp':self.timestamp,
                'opMode': opMode, 
                'navMode': navMode,
                'HDOP':HDOP,
                'VDOP':VDOP,
                'PDOP':PDOP
                }

        if self.time==None:
            self.skipped_msg = self.skipped_msg + 1
            print(f'GSA message skipped:{row}')
            return
        
        self.insertValues(self.time, dict)
        
    def gst(self, row):
        # Message: GNSS pseudorange statistic
        
        # GNGST message example:
        #   __________________________________________________________________________________
        #  |___0___|____1_____|____2____|___3____|___4____|___5___|___6____|___7____|____8____|
        #  |_Name__|___time___|rangeRMS_|stdMajor|stdMinor|orient_|_stdLat_|stdLong_|_stdAlt__|
        #['$GNGST', '120008.00', '0.509', '0.003', '0.002', '24.0', '0.003', '0.002', '0.006*7E']

        rangeRMS = row[2]
        stdMajor = row[3]
        stdMinor = row[4]
        orient = row[5]
        stdLat = row[6]
        stdLong = row[7]
        stdAlt = row[8]
        # Fill the table
        dict = {'rangeRMS':rangeRMS,
                'stdMajor': stdMajor, 
                'stdMinor': stdMinor,
                'orient':orient,
                'stdLat':stdLat,
                'stdLong':stdLong,
                'stdAlt':stdAlt
                }
        self.insertValues(self.time, dict)
        
    def dtm(self, row):
        # Message: Datum reference
        
        # GNDTN message example:
        #
        #  ________________________________________________________________
        # |___0___|__1___|___2____|__3___|_4__|__5___|_6__|__7___|____8____|
        # |_Name__|datum_|subDatum|_lat__|_NS_|_lon__|_EW_|_alt__|refeDatum|
        #['$GNDTM', 'W84', ''     , '0.0', 'N', '0.0', 'W', '0.0', 'W84*63']
        
        #### NOT IMPLEMENTED YET####
        pass
    def rmc(self, row):
        pass
    
    def vtg(self, row):
        pass
    
    def gga(self, row):
        # Message: Global positioning system fix data
    
        # GNGGA message example:
        
        #  ________________________________________________________________________________________________________________________________________________
        # |____0___|_____1______|_______2________|_3__|__ ______4________|_5__|___6___|__7__|__8___|____9_____|__10___|___11____|__12___|__13___|____14_____|
        # |__Name__|____time____|___latitude_____|_NS_|____longitude____|_EW_|quality|numSV|_HDOP_|_altitude_|altUnit|___sep___|SepUnit|diffAge|diffStation|
        # ['$GNGGA', '120009.00', '4655.66628503', 'N', '00727.09258987', 'E', '4'   , '20', '0.7', '572.414',  'M'  , '48.020', 'M'   , '1.0' , '1964*58']
        #### NOT IMPLEMENTED YET####
        self.time = row[1]
        if self.time == '':
            pass
        # else:
            # # Create a UTC RCF3339 timestamp
            # _date=row[4] + row[3] + row[2] + 'T' + row[1]
            # self.timestamp = self.utcrcf3339(_date)
            # self.time = row[1]
            
            # # Insert values in df
            # dict = {'timestamp': self.timestamp}
            # self.insertValues(self.time, dict)
        
        

    def gbs(self, row):
        # Message: GNSS satellite fault detection
        
        # GNGBS message example:
        #
        #  ________________________________________________________________________________________________
        # |____0___|_____1______|___2____|___3_____|___4____|__5__|___6____|___7_____|_____8_____|____?____|
        # |__Name__|____time____|_errLat_|_errLon__|_errAlt_|svid_|__prob__|__bias___|systemID   |signal_ID|
        # ['$GNGBS', '120012.00', '0.207', '-0.203', '0.113', '68', '0.000', '-1.457', '6.302*78']

        #### NOT IMPLEMENTED YET####
        pass
    
    def txt(self, row):
         #### NOT IMPLEMENTED YET####
        pass

    def gsv(self, row):
        # Message: GNSS satellite in view

        # GNGSA message example:
        #   _________________________________________________________________
        #  |___0___|__1___|__2___|__3__|4+4n_|_5+4n|6+4n|7+4n| ... |____8____|
        #  |_Name__|numMsg|msgNum|numSV|svid1|elev1|az1 |cno1| ... |signalId1|
        # ['$GPGSV', '10', '1', '32', '15', '23', '296', '45', '18', '12', '315', '40', '7', '25', '060', '45', '28', '47', '135', '48*7D']
        # ['$GPGSV', '10', '2', '32', '5', '58', '220', '49', '13', '60', '301', '50', '30', '57', '060', '50', '14', '51', '125', '47*7B']
        # ['$GPGSV', '10', '3', '32', '8', '8', '046', '37*7C']
        # ['$GLGSV', '10', '4', '32', '67', '81', '109', '52', '78', '37', '210', '51', '84', '9', '326', '33', '68', '41', '321', '48*6F']
        # ['$GLGSV', '10', '5', '32', '77', '83', '074', '41', '76', '28', '037', '48', '66', '26', '134', '47*6F']
        # ['$GAGSV', '10', '6', '32', '8', '57', '303', '50', '25', '60', '081', '50', '24', '15', '040', '43', '13', '6', '268', '27*50']
        # ['$GAGSV', '10', '7', '32', '3', '65', '116', '51', '26', '8', '318', '41', '2', '46', '179', '48', '5', '13', '119', '42*55']
        # ['$GAGSV', '10', '8', '32', '7', '7', '300', '37*67']
        # ['$GBGSV', '10', '9', '32', '105', '15', '122', '35', '110', '33', '058', '40', '114', '39', '307', '44', '122', '5', '142', '41*60']
        # ['$GBGSV', '10', '10', '32', '107', '16', '040', '37', '121', '54', '157', '49', '126', '62', '213', '50*62']

        # Skip messages with no signals
        if (len(row)-4)//4 == 0:
            return

        for n in range((len(row)-4)//4):
            svId = row[4+4*n]
            elv = row[5+4*n]
            az = row[6+4*n]
            cno = row[7+4*n]
            signalId = row[4 + ((len(row)-4)//4)]
            self.index += 1

            # Fill the table
            dict = {'svId': svId,
                    'elv': elv,
                    'az': az,
                    'cno': cno,
                    'timestamp': self.timestamp,
                    'signalId': signalId
                    }

            self.insertGSV_Values(self.index, dict)


    def gll(self, row):
        #### NOT IMPLEMENTED YET####
        pass

class Batch:
    def __init__(self, foldername):
        self.foldername1 = foldername
        # self.count = 1
        # self.numFiles = self.countFiles(foldername)
        
        print('Start process\n\n')
        
    def scanDir(self):

        for entry in os.scandir(self.foldername1):
            if os.path.isdir(entry.path):
                self.foldername1 = entry
                self.scanDir()
            
            if os.path.isfile(entry.path):
                root, extension = os.path.splitext(entry.path)
                if extension == '.txt': # and root.find('netR9') != -1
                    print(f'File that is parsing {os.path.basename(entry)}: ')
                    receiver = ReadNmea(entry.path, get_nmea, get_gsv)
                    receiver.parser()
                # if extension == '.ubx':
                #     print(f'File that is parsing {os.path.basename(entry)}: ')
                #     receiver = ReadNmea(entry.path, get_nmea, get_gsv)
                #     receiver.parser()


# Call the interface class
app = gui.Interface()
app.title('Parse NMEA messages')
app.mainloop()
filepath, get_nmea, get_gsv = app.output()
# filepath = 'C:/SwisstopoMobility/Analyse/DataBase/2021/April/05/2021-04-05_netr9.txt'
# filepath = 'C:/SwisstopoMobility/Analyse/DataBase/2021/April/05/2021-04-05_swipos.txt'
# filepath = 'C:/SwisstopoMobility/Analyse/DataBase/2021/April/13/2021-04-13_swipos.txt'
# filepath = 'C:/SwisstopoMobility/Analyse/DataBase/2021/April/05'
get_gsv = False
get_nmea = True

# Executre once for the selcted file
if os.path.isfile(filepath):
    receiver = ReadNmea(filepath, get_nmea, get_gsv)
    print(filepath)
    df = receiver.parser()

# Iteration over all files in folder
if os.path.isdir(filepath):
    batch = Batch(filepath)
    batch.scanDir()
    print('End of batch process')
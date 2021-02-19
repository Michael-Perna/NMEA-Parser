# -*- coding: utf-8 -*-
"""
Created on Mon Dec 14 17:07:52 2020

@author: Michael
"""
import pandas as pd


            
class NmeaDf:
    def __init__(self, filename):
        self.dateFormat = '%H%M%S.%f'
        self.filename = filename
        
        
        self.header = ['timestamp',
                        'time', 
                        'lon', 
                        'stdLong', 
                        'lat', 
                        'stdLat',
                        'alt',
                        'stdAlt',
                        'sep', 
                        'rangeRMS',
                        'posMode',
                        'numSV',
                        'numGP',
                        'posModeGP',
                        'diffAgeGP',
                        'numGL',
                        'posModeGL',
                        'difAgeGL',
                        'numGA',
                        'posModeGA',
                        'difAgeGA',
                        'numGB',
                        'posModeGB',
                        'difAgeGB',
                        'opMode',
                        'navMode',
                        'PDOP',
                        'HDOP', 
                        'VDOP',
                        'stdMajor',
                        'stdMinor', 
                        'orient']

        self.types = {'timestamp':str,
                        'time':str, 
                        'lon':float, 
                        'stdLong':float, 
                        'lat':float, 
                        'stdLat':float,
                        'alt':float,
                        'stdAlt':float,
                        'sep':float, 
                        'rangeRMS':float,
                        'posMode':str,
                        'numSV':int,
                        'numGP':int,
                        'posModeGP':str,
                        'diffAgeGP':float,
                        'numGL':int,
                        'posModeGL':str,
                        'difAgeGL':float,
                        'numGA':int,
                        'posModeGA':str,
                        'difAgeGA':float,
                        'numGB':str,
                        'posModeGB':str,
                        'difAgeGB':float,
                        'opMode':str,
                        'navMode':int,
                        'PDOP':float,
                        'HDOP':float, 
                        'VDOP':float,
                        'stdMajor':float,
                        'stdMinor':float,
                        'orient':float}

        self.qualityFlag = {0: 'Missing',
                            1:'No Fix',
                            2:'autonomous GNSS fix',
                            3:'Differential GNSS fix',
                            4:'RTK fixed',
                            5:'RTK float',
                            6:'Estimated or dead reckoning fix'}
        


    def getDataFrame(self):
        
        # Open the file
        file = open(self.filename)
        
        # Import csv file as DataFrame with pandas as String
        df = pd.read_csv(file, sep=',', header=None, dtype=str, error_bad_lines=False, warn_bad_lines=True)
    
        # Add name to each column
        df.columns = self.header
        
        # Apply the type format according to the dico 
        for col, col_type in self.types.items():
            df[col] = df[col].astype(col_type, errors='ignore')

        # Close the File 
        file.close()
        
        # # Set index 
        # df.set_index(['timestamp', 'lon', 'lat'],inplace=True, drop=True)
        
        # Assing name to respective flag
        df['navMode'] = df['navMode'].map(self.qualityFlag)
        
        return df

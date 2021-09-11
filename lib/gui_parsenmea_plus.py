
# -*- coding: utf-8 -*-
"""
Created on Mon Jan 18 09:54:11 2021

@author: Michael
"""

import tkinter as tk
import sys
from tkinter.filedialog import askopenfilename
from tkinter.filedialog import askdirectory
from tkinter import  Radiobutton


class Interface(tk.Tk):
    def __init__(self):
        tk.Tk.__init__(self)
        self.create_widgets()
        self.filename = None
        self.do_parser = None
        
    def create_widgets(self):
        self.title('Parse NMEA messages')
        self.geometry("500x500") 
        self.configure(background='white')
        
        # Frame 1
        self.frame1 = tk.LabelFrame(self, text='Select .nmea file or a folder to parse',
                                    bg='white',padx=40, pady=60)
        
        # Frame 1: Buttons
        self.button = tk.Button(self.frame1, text='Browse File',width=25,
                                command=lambda:self.open_file())
        self.button2 = tk.Button(self.frame1, text='Browse Folder',width=25,
                                command=lambda:self.open_folder())
        
        # Subframe: frame2
        self.frame2 = tk.LabelFrame(self, text='Do you want to parse NMEA values? ',
                                    bg='white', padx=30, pady=10)
        
        # Set frame 2 defeault radiobuttons values 
        self.var =tk.IntVar()
        self.var.set(1)
        
        # Set frame2 radiobuttons
        self.radio1 = Radiobutton(self.frame2, text="Yes", 
                                  variable = self.var, value=1,
                                  command=self.selection,bg='white')
        self.radio2 = Radiobutton(self.frame2, text="No", 
                                  variable=self.var, value=0,
                                  bg='white', command=self.selection)
        
        # Subframe: frame3
        self.frame3 = tk.LabelFrame(self, text='Do you want to extract satellite signals values? ',
                                    bg='white', padx=30, pady=10)
        
        # Set frame 3 defeault radiobuttons values 
        self.var2 =tk.IntVar()
        self.var2.set(0)
        
        # Set frame3 radiobuttons
        self.radio3 = Radiobutton(self.frame3, text="Yes", 
                                  variable = self.var2, value=1,
                                  command=self.selection2,bg='white')
        self.radio4 = Radiobutton(self.frame3, text="No", 
                                  variable=self.var2, value=0,
                                  bg='white', command=self.selection2)


        # Pack Frames 
        self.frame1.pack(pady=10)
        self.frame2.pack(pady=10)
        self.frame3.pack(pady=10)
        
        # Pack Buttons
        self.button.pack()
        self.button2.pack()
        self.radio1.pack()
        self.radio2.pack()
        self.radio3.pack()
        self.radio4.pack()

        
    def open_file(self):
        self.filename = askopenfilename(initialdir="./DataBase",
                               filetypes =(('NMEA File', '*.txt'),
                                           ("Text File", "*.txt"),
                                           ("All Files","*.*")),
                               title = "Choose a file."
                               )
        self.destroy()

    def open_folder(self):
        self.filename = askdirectory(initialdir="./DataBase",
                               title = "Choose a folder."
                               )
        self.destroy()

    def selection(self):
        if self.var.get() == 1:
            self.get_nmea = True
            # return True
        elif self.var.get() == 0:
            self.get_nmea = False
            # return False
        else:
            sys.exit("Missing input")
    
    def selection2(self):

        if self.var2.get() == 1:
            self.get_gsv= True
            # return True
        elif self.var2.get() == 0:
            self.get_gsv = False
            # return False
        else:
            sys.exit("Missing input")
    
    def output(self):
        if self.var.get()==1:
            self.get_nmea = False
        if self.var2.get()==0:
            self.get_gsv = False
        return self.filename, self.get_nmea, self.get_gsv
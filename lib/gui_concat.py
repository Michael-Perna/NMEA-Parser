# -*- coding: utf-8 -*-
"""
Created on Mon Jan 18 09:54:11 2021

@author: Michael
"""

import tkinter as tk
from tkinter.filedialog import askdirectory
from tkinter import Entry
from tkinter import  Radiobutton
import sys

class Interface(tk.Tk):
    def __init__(self):
        tk.Tk.__init__(self)
        self.create_widgets()
        self.filename = None
        self.do_parser = None
        
    def create_widgets(self):
        # Set main frame parameters 
        self.title('Parse UBX messages')
        self.geometry("500x400") 
        self.configure(background='white')
        
        # Subframe: frame1
        self.frame1 = tk.LabelFrame(self, text='Select a folder ',
                                    bg='white',padx=30, pady=10)
        # Set frame1 buttons
        self.button2 = tk.Button(self.frame1, text='Browse Folder',width=25,
                                command=lambda:self.open_folder())
        # Subframe: frame2
        self.frame2 = tk.LabelFrame(self, text='Receiver name',
                                    bg='white',padx=30, pady=20)
        
        # Frame2: set entry box
        self.format_name = tk.Label(self.frame2, text='Enter output name',
                                    width=25, bg='white')
        self.e= Entry(self.frame2)
        self.e.insert(1,'sapcorda')
        self.format_name.grid(row=0, column=0)
        self.e.grid(row=0, column=1)
        self.button3 = tk.Button(self, text='OK',width=10,
                                command=lambda:self.entry())
        
        # Subframe: frame3
        self.frame3 = tk.LabelFrame(self, text='Choose extension type',
                                    bg='white', padx=30, pady=10)
        # Set frame 2 defeault radiobuttons values 
        self.var =tk.IntVar()
        self.var.set(0)
        
        # Set frame3 radiobuttons
        self.radio1 = Radiobutton(self.frame3, text="*.ubx", variable = self.var, value=1,
                  command=self.selection,bg='white')
        self.radio2 = Radiobutton(self.frame3, text="*.txt", variable=self.var, value=0,
                  bg='white', command=self.selection)
        
        # Pack frames
        self.frame1.pack(pady=10)
        self.frame3.pack(pady=10)
        self.frame2.pack(pady=10)
        
        # Pack buttons
        self.button2.pack()
        self.radio1.pack()
        self.radio2.pack()
        self.button3.pack()

        
    def open_folder(self):
        self.folder = askdirectory(initialdir="C:/SwisstopoMobility/analysis/data",
                               title = "Choose a folder."
                               )

    
    def entry(self):
        self.output_name = self.e.get()
        self.destroy()
        
    def selection(self):
        print(self.var.get())
        
        if self.var.get() == 1:
            self.ext = '.ubx'
            # return True
        elif self.var.get() == 0:
            self.ext = '.txt'
            # return False
        else:
            sys.exit("Missing input")

    
    def output(self):
        if self.var.get()==0:
            self.ext = '.txt'
        return self.folder, self.output_name, self.ext
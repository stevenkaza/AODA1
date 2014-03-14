#!/usr/bin/python3
#Steven Kazavchinski
#CIS 2750 
#A3 
#0761977    
import os
import subprocess
from tkinter import *
from tkinter import tix
from tkinter import ttk    
from tkinter.scrolledtext import *
from tkinter import filedialog
#import Vcf 
from tkinter import messagebox 
from tkinter.filedialog import askopenfilename
class App:
    def __init__(self,master):
      
        #menu bar intilization 
        self.menuInit()
        #frames intilization
        self.framesInit()
        #Labels init
        self.labelsInit()
        #Table/HList Init      
        self.tablesInit(0)

        self.scrolledLog = ScrolledText(self.logFrame,width = 50,height = 10)

        self.buttonsInit()


        #scrolledLog.insert(END, "This is line %d\n")

        #clearButton = Button(self.scrolledLog, text = "Clear")
       # clearButton.pack(side=LEFT)


        logLabel = Label(self.logFrame,text = "Log Display Panel")
        logLabel.pack(side=TOP)
        self.scrolledLog.pack()

    def fileOpen(self):
        ftypes = [('vCard files', '*.vcf'), ('All files', '*')]
        fname = askopenfilename(filetypes=(ftypes))                                                                     
        data= self.displayFileInfo(fname)
               # self.scrolledLog.insert(END, fname)
                                
               #        fl = dlg.show()
    def aboutProgram(self):
        messagebox.showinfo("About this program","This is a visual utlity for using Vcards. It is compatibile with vCard versoin 3.0 only, and created by Steven Kazavchinski")

    def displayFileInfo(self,fname):
        #cmd = ['./vcftool','-info','<','wow.vcf']
#        print(subprocess.call('dir', shell=True))
        #cmd = ['./vcftool -info < wow.vcf'] 
    #subprocess.check_output(["ls","-l"])
        #process = subprocess.Popen(cmd, shell=True,
        #stdout=subprocess.PIPE, 
        #stderr=subprocess.PIPE)
        with open("output.vcf") as f:
            content = f.read()
        self.scrolledLog.insert(END,content)    
        status =  Vcf.readFile(fname)
        print (status)
        
        print (status)
        card = []
        numCards=Vcf.getCard(card)
        print("Numcards = "+str(numCards))
        self.updateFVP(numCards)
        print("Do we get here or no way?")
    # wait for the process to terminate
        os.system('./vcftool -info <' +fname+' >output.vcf')
       # out, err = process.communicate()
       # errcode = process.returncode
        #output = subprocess.Popen( cmd, stdout=subprocess.PIPE ).communicate()[0]
     #print (output)
     #   print (out)

       # os.system("./vcftool -info < %s" % fname)
    def exitProgram(self):

        root.destroy()
  #  def cvp(self):
        #
    def updateFVP(self,numCards):
        for i in range(1,numCards):
            self.fileViewScrolledList.hlist.add("R"+str(i))
            self.fileViewScrolledList.hlist.item_create("R"+str(i),0,text = "card "+str(i))

    #def framesInit(self):
    def buttonsInit(self):
        #FVP Buttons
        upButton = Button(self.fvpFrame, text = "  Up  ")
        downButton = Button(self.fvpFrame, text = "Down ")
        addButton = Button(self.fvpFrame, text = "Add")
        deleteButton = Button(self.fvpFrame, text = "Delete ")
        revertButton = Button(self.fvpFrame, text = "Revert ")
        commitButton = Button(self.fvpFrame, text = "Commit")
        #Log button/clear
        clearButton = Button(self.logFrame, text = "Clear",command = self.clearLog)
        clearButton.pack(side=LEFT)
          
        downButton.pack(side= BOTTOM, padx = 10,pady=5)
        upButton.pack(side=BOTTOM, padx = 10,pady=5)
        addButton.pack(side= BOTTOM, padx = 10,pady=5)
        deleteButton.pack(side=BOTTOM, padx = 10,pady=5)
        revertButton.pack(side= BOTTOM, padx = 10,pady=5)
        commitButton.pack(side=BOTTOM, padx = 10,pady=5)
        self.adjustPos()
        
    def adjustPos(self):
        theLabel = Label(self.cvpFrame,text = "  ")
        theLabel.pack(side = LEFT, padx=20)
    
    def clearLog(self):
        self.scrolledLog.delete(0.0,END)
        
        


    def menuInit(self):
        menuBar = Menu(root)

        menuBar = Menu(root)

        fileMenu = Menu(menuBar)
        fileMenu.add_command(label = "Open",command = self.fileOpen)
        fileMenu.add_command(label = "Append")
        fileMenu.add_command(label = "Save")
        fileMenu.add_command(label = "Save As")
        fileMenu.add_command(label = "Exit",command = self.exitProgram)

        #Making it appear
        menuBar.add_cascade(label='File', menu = fileMenu)

        orgMenu = Menu(menuBar)
        orgMenu.add_command(label = "Sort")
        orgMenu.add_command(label = "Canonicalize")
        orgMenu.add_command(label = "Select")
        orgMenu.add_command(label = "Undo")

        menuBar.add_cascade(label='Organize', menu = orgMenu)

        helpMenu = Menu(menuBar)

        helpMenu.add_command(label = "Card flags and colours")

        helpMenu.add_command(label = "About xvcf",command = self.aboutProgram)
        menuBar.add_cascade(label='Help', menu = helpMenu)
        root.config(menu=menuBar)
    def framesInit(self):
        frame =  Frame(root,width = "1024")

        frame.grid()
        self.fvpFrame = Frame(frame)
     #   self.fvpFrame.pack(side = LEFT)
        self.cvpFrame = Frame(frame)
       


        self.fvpFrame.grid(row=0,column = 0)
    #    self.cvpFrame.pack(side=TOP)
       # self.fvpFrame.pack(side =TOP)
        # LOGS ---------------------#

        self.logFrame = Frame(frame)
        #self.logFrame.pack(side = BOTTOM)


        self.fvpFrame.grid(row=0,column = 0)
        self.cvpFrame.grid(row=1,column = 0)

        self.logFrame.grid(row = 3,column = 0)

    def labelsInit(self):
        fileLabel = Label(self.fvpFrame,text = "File View Panel")
        fileLabel.pack(side=TOP)
        
        cardLabel = Label(self.cvpFrame,text = "Card View Panel")
        cardLabel.pack(side=TOP)

    def tablesInit(self,numRows):
        #Creating the scrolled list 
        self.fileViewScrolledList = tix.ScrolledHList(self.fvpFrame, width = 70, options='hlist.columns 7 hlist.header 1 ')
        self.fileViewScrolledList.config(width = 450)
        self.fileViewScrolledList.pack(side =RIGHT )
        #able to access hlist subwidget this way 
        fileView = self.fileViewScrolledList.hlist
      
        #Creating the headers
        numCards = 10
        numRows = numCards 
        fileView.header_create(0, text = "Card #")
        fileView.header_create(1, text = "Name" )
        fileView.header_create(2, text = "Region")
        fileView.header_create(3, text = "Country")
        fileView.header_create(4, text = '#ADR')
        fileView.header_create(5, text = "#TEL")
        fileView.header_create(6, text = 'Flags')

        # Loop for filling in Card #'s , creating rows based on # of cards 
     #   for i in range(1,numRows):
      #       fileView.add("R"+str(i))
       #      fileView.item_create("R"+str(i),0,text = "card "+str(i))

        #scroll = Scrollbar(self.fvpFrame, command=fileView.yview)

        #fileView.(yscrollcommand=scroll.set)
        
        #scroll.pack(side=RIGHT, fill=Y)
        cardViewScrolledList = tix.ScrolledHList(self.cvpFrame, width = 70, options='hlist.columns 4 hlist.header 1')
        cardViewScrolledList.config(width = 450)
        cardViewScrolledList.pack(side = RIGHT)
        cardView = cardViewScrolledList.hlist

        cardView.header_create(0, text = "Name")
        cardView.header_create(1, text = "Type" )
        cardView.header_create(2, text = "Value")
        cardView.header_create(3, text = "Property")
        for i in range(1,numRows):
            cardView.add("T"+str(i))
            cardView.item_create("T"+str(i),0,text = "card "+str(i))
root = tix.Tk()
app=App(root)
#mlb = MultiListbox(tk, (('Subject', 40), ('Sender', 20), ('Date', 10)))

#fvp = Label(root, text = "File View Panel")
#cvp = Label(root, text = "Card View Panel")
#fvp.pack()
#cvp.pack()
root.mainloop()


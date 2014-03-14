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
import Vcf 
from tkinter import messagebox 
from tkinter.filedialog import askopenfilename
class App:
    def __init__(self,master):
        self.fname = None 
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

        #clearButton = Button(self.crcrolledLog, text = "Clear")
       # clearButton.pack(side=LEFT)


        logLabel = Label(self.logFrame,text = "Log Display Panel")
        logLabel.pack(side=TOP)
        self.scrolledLog.pack()
        #while (self.photoState==0):
         #   print ("true")
            

    def fileOpen(self):
        ftypes = [('vCard files', '*.vcf'), ('All files', '*')]
        returned_values = {}
        self.fname = askopenfilename(filetypes=(ftypes))                                                                     
        data= self.displayFileInfo()
               # self.scrolledLog.insert(END, fname)
                                
               #         fl = dlg.show()
    def aboutProgram(self):
        messagebox.showinfo("About this program","This is a visual utlity for using Vcards. It is compatibile with vCard versoin 3.0 only, and created by Steven Kazavchinski")
    def selectCards(self):
        
        self.photoState = IntVar()
        self.urlState = IntVar()
        self.locationState = IntVar()
        self.cancelSel = 0
        
        self.window = Toplevel()
        test = 5
        #Check buttons
        location = Checkbutton(self.window, text = "Location", variable = self.locationState, \
                 onvalue = 1, offvalue = 0, height=5, \
                 width = 20)

        photo = Checkbutton(self.window,text = "  Photo", variable = self.photoState)
        
        url = Checkbutton(self.window, text = "      URL",variable = self.urlState)
        ok = Button(self.window,text = "OK",command = self.launchSelect)
        cancel = Button(self.window,text="Cancel", command = self.cancelSel)


        location.grid(row=0,column=0)
        photo.grid(row=1,column=0)
        url.grid(row=2,column=0)
  
        ok.grid(row=3,column=0)
        cancel.grid(row=3,column=1)
        
        print(self.photoState)
    def launchSelect(self):
        location = ""
        photo = ""
        url = ""
        if self.photoState.get() == 1:
            locaton = "g"
        if self.photoState.get() == 1:
           photo = "p"
        if self.urlState == 1:
            url = "u"
        print (self.fname)            
        os.system('./vcftool -select '+photo+' <'+self.fname)
            
        self.window.destroy()
        #changes the global variable to cancelled state, so main program knows to not select cards
    def cancelSelect(self):
        self.cancelSelect = 1
        self.window.destroy
        
    def checkStates(self,var):
        print(var.get())
        print ("anything")
     
    def drawChecks(self):
        integer = 2
        print("wtf")
    def displayFileInfo(self):
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
        status =  Vcf.readFile(self.fname)
        print (status)
        
        print (status)
        cards = []
        i = 0
        numCards=Vcf.getNumCards() 
        while (i < numCards):
             card=[]
             Vcf.getCard(card)	
             cards.append(card)
       	     i = i + 1
        foundFN = 0
        adrCount = 0 
        telCount = 0 
        i = 0 
        for Card in cards:
            firstFNVal = None
            adrCount = 0
            telCount = 0
            foundFN = 0 
            for Tuple in Card:
              #Looking for firstFN value
              if (Tuple[0]==4 and foundFN==0):
                  firstFNVal = Tuple[1]
                  foundFN = 1
              #  ADR Counting
              if (Tuple[0]==8):
                  adrCount = adrCount +1
              # tel counting
              if (Tuple[0]==10):
                  telCount = telCount + 1
            i = i + 1
            self.updateFVP(firstFNVal,adrCount,telCount,i)

       
	 #cards(0)=card
        print ("Here comes the card")
        print("Numcards = "+str(numCards))
        #self.updateCVP(numCards,cards)
        print("Do we get here or no way?")
    # wait for the process to terminate
        os.system('./vcftool -info <' +self.fname+' >output.vcf')

    
    def exitProgram(self):

        root.destroy()
  #  def cvp(self):
        #
    def updateFVP(self,firstFNVal,adrCount,telCount,rowNum):
        self.fileViewScrolledList.hlist.add((rowNum-1))
        #card sequence, displaying the results for each row
        strRowNum = str(rowNum)
        sstrRowNum = str.format(strRowNum) 
        strAdrCount = str(adrCount)
        strTelCount = str(telCount)
        print(strRowNum)
        print(rowNum)
        self.fileViewScrolledList.hlist.item_create((rowNum-1),0,text = strRowNum)
        self.fileViewScrolledList.hlist.item_create((rowNum-1),1,text = firstFNVal)
        self.fileViewScrolledList.hlist.item_create((rowNum-1),4,text = strAdrCount)
        self.fileViewScrolledList.hlist.item_create((rowNum-1),5,text = strTelCount)
    def updateCVP(self,numCards,card):
        for i in range(0,1000):
            self.cardViewScrolledList.hlist.add("R"+str(i))
            #self.cardViewScrolledList.hlist.item_create("R"+str(i),0,text = "card "+str(i))


        i = 0
       # for card in cards: 
        #i = 0 
        for Tuple in card:
            self.cardViewScrolledList.hlist.item_create("R"+str(i),0,text= str(Tuple[0]))   
            self.cardViewScrolledList.hlist.item_create("R"+str(i),1,text =Tuple[1])
            self.cardViewScrolledList.hlist.item_create("R"+str(i),2, text =Tuple[2])
            i = i + 1
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
        orgMenu.add_command(label = "Select",command = self.selectCards)
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
        self.cardViewScrolledList = tix.ScrolledHList(self.cvpFrame, width = 70, options='hlist.columns 4 hlist.header 1')
        self.cardViewScrolledList.config(width = 450)
        self.cardViewScrolledList.pack(side = RIGHT)
        cardView = self.cardViewScrolledList.hlist

        cardView.header_create(0, text = "Name")
        cardView.header_create(1, text = "Type" )
        cardView.header_create(2, text = "Value")
        cardView.header_create(3, text = "Property")
       # for i in range(1,numRows):
            #cardView.add("T"+str(i))
            #cardView.item_create("T"+str(i),0,text = "card "+str(i))
root = tix.Tk()
app=App(root)
#mlb = MultiListbox(tk, (('Subject', 40), ('Sender', 20), ('Date', 10)))

#fvp = Label(root, text = "File View Panel")
#cvp = Label(root, text = "Card View Panel")
#fvp.pack()
#cvp.pack()
print ("test")

root.mainloop()


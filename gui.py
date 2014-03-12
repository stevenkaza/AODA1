#!/usr/bin/python3
import os
import subprocess
from tkinter import *
from tkinter import tix
from tkinter import ttk    
from tkinter.scrolledtext import *
from tkinter import filedialog
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
        self.tablesInit()


        self.scrolledLog = ScrolledText(self.logFrame,width = 60,height = 10)
        #scrolledLog.insert(END, "This is line %d\n")

        self.scrolledLog.pack()



    def fileOpen(self):
        ftypes = [('vCard files', '*.vcf'), ('All files', '*')]
        fname = askopenfilename(filetypes=(ftypes))                                                                     
        data= self.displayFileInfo(fname)
               # self.scrolledLog.insert(END, fname)
                                
               #        fl = dlg.show()

    def displayFileInfo(self,fname):
        #cmd = ['./vcftool','-info','<','wow.vcf']
#        print(subprocess.call('dir', shell=True))
       	#cmd = ['./vcftool -info < wow.vcf'] 
        os.system('./vcftool -info <' +fname+' >output.vcf')
	#subprocess.check_output(["ls","-l"])
        #process = subprocess.Popen(cmd, shell=True,
        #stdout=subprocess.PIPE, 
        #stderr=subprocess.PIPE)
        with open("output.vcf") as f:
            content = f.read()
        self.scrolledLog.insert(END,content)
       # wait for the process to terminate
       # out, err = process.communicate()
       # errcode = process.returncode
        #output = subprocess.Popen( cmd, stdout=subprocess.PIPE ).communicate()[0]
     #print (output)
     #   print (out)

       # os.system("./vcftool -info < %s" % fname)
    def exitProgram(self):

        root.destroy()
    def cvp(self):
        print ("Card View Panel")

    def fvp(self):
        print ("File view Panel")


    #def framesInit(self):


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

        helpMenu.add_command(label = "About xvcf")
        menuBar.add_cascade(label='Help', menu = helpMenu)
        root.config(menu=menuBar)
    def framesInit(self):
        frame =  Frame(root)

        frame.pack()
        self.fvpFrame = Frame(frame)
        self.fvpFrame.pack(side = TOP)
        self.cvpFrame = Frame(frame)

        self.logFrame = Frame(frame)
        self.logFrame.pack(side = BOTTOM)
        
        self.cvpFrame.pack(side=TOP)
        self.fvpFrame.pack(side =TOP)
    def labelsInit(self):
        fileLabel = Label(self.fvpFrame,text = "File View Panel")
        fileLabel.pack(side=TOP)
        cardLabel = Label(self.cvpFrame,text = "Card View Panel")
        cardLabel.pack(side=TOP)
    def tablesInit(self):
        fileView = tix.HList(self.fvpFrame,width = 60, columns = 7,header=True)	
        fileView.pack(side = BOTTOM)

      
      #  fileView.header(0)
        fileView.header_create(0, text = "Card #")
        fileView.header_create(1, text = "Name" )
        fileView.header_create(2, text = "Region")
        fileView.header_create(3, text = "Country")
        fileView.header_create(4, text = '#ADR')
        fileView.header_create(5, text = "#TEL")
        fileView.header_create(6, text = 'Flags')



        #fileViewself.Xadd(0,text = "wow")
        #fileView.add(1,text = "wow1")


        #scroll = Scrollbar(self.fvpFrame, command=fileView.yview)

        #fileView.configure(yscrollcommand=scroll.set)
		
        #scroll.pack(side=RIGHT, fill=Y)
        cardView = tix.HList(self.cvpFrame, width = 60,columns = 4,header = True)
        cardView.pack(side = BOTTOM)

        cardView.header_create(0, text = "Name")
        cardView.header_create(1, text = "Type" )
        cardView.header_create(2, text = "Value")
        cardView.header_create(3, text = "Property")
root = tix.Tk()
app=App(root)
#mlb = MultiListbox(tk, (('Subject', 40), ('Sender', 20), ('Date', 10)))

#fvp = Label(root, text = "File View Panel")
#cvp = Label(root, text = "Card View Panel")
#fvp.pack()
#cvp.pack()
root.mainloop()


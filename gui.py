#!/usr/bin/python3
import os
import subprocess
from tkinter import *
from tkinter import tix

from tkinter.scrolledtext import *
from tkinter import filedialog
from tkinter.filedialog import askopenfilename
class App:
    def __init__(self,master):
        frame =  Frame(root)

        frame.pack()
        #menu bar
        self.menuInit()
        #self.master.config(menu=menubar)
        #menuBar.pack()

        #file menu

        #Organize menu
        #self.framesInit()


        #Frames
        self.fvpFrame = Frame(frame)
        self.cvpFrame = Frame(frame)

        self.logFrame = Frame(frame)
        self.logFrame.pack(side = BOTTOM)

        cvpTixThing = tix.TList(self.cvpFrame,height = 10, width = 60)
        cvpTixThing.pack()

        fvpTixThing = tix.TList(self.fvpFrame,height = 10, width = 60)
        fvpTixThing.pack()

        self.scrolledLog = ScrolledText(self.logFrame,width = 60,height = 10)
        #scrolledLog.insert(END, "This is line %d\n")

        self.scrolledLog.pack()

        self.cvpFrame.pack(side=TOP)
        self.fvpFrame.pack(side =TOP)


    def fileOpen(self):
                ftypes = [('Python files', '*.py'), ('All files', '*')]
                fname = askopenfilename(filetypes=(ftypes))
                                           
                                        
                data= self.displayFileInfo(fname)
                self.scrolledLog.insert(END, data)
                                
                return data
               #        fl = dlg.show()

    def displayFileInfo(self,fname):
        cmd = ['dir']
#        print(subprocess.call('dir', shell=True))
        process = subprocess.Popen(cmd, shell=True,
        stdout=subprocess.PIPE, 
        stderr=subprocess.PIPE)

       # wait for the process to terminate
        out, err = process.communicate()
        errcode = process.returncode
        #output = subprocess.Popen( cmd, stdout=subprocess.PIPE ).communicate()[0]
     #print (output)
        print (out)
        return out 


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


root = tix.Tk()
app=App(root)


#fvp = Label(root, text = "File View Panel")
#cvp = Label(root, text = "Card View Panel")
#fvp.pack()
#cvp.pack()
root.mainloop()


#!/usr/bin/python3

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
        menuBar = Menu(root)
        #self.master.config(menu=menubar)
        #menuBar.pack()

        #file menu
        fileMenu = Menu(menuBar)
        fileMenu.add_command(label = "Open",command = self.fileOpen)
        fileMenu.add_command(label = "Append")
        fileMenu.add_command(label = "Save")
        fileMenu.add_command(label = "Save As")
        fileMenu.add_command(label = "Exit",command = self.exitProgram)

        #Making it appear
        menuBar.add_cascade(label='File', menu = fileMenu)

        #Organize menu
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



                #Frames
        self.fvpFrame = Frame(frame)
        self.cvpFrame = Frame(frame)

        self.logFrame = Frame(frame)
        self.logFrame.pack(side = BOTTOM)

        cvpTixThing = tix.TList(self.cvpFrame,height = 10, width = 60)
        cvpTixThing.pack()

        fvpTixThing = tix.TList(self.fvpFrame,height = 10, width = 60)
        fvpTixThing.pack()

        scrolledLog = ScrolledText(self.logFrame,width = 60,height = 10)
        scrolledLog.pack()
        self.cvpFrame.pack(side=TOP)
        self.fvpFrame.pack(side =TOP)

        root.config(menu=menuBar)

    def fileOpen(self):
                ftypes = [('Python files', '*.py'), ('All files', '*')]
                fname = askopenfilename(filetypes=(("Template files", "*.tplate"),
                                           ("HTML files", "*.html;*.htm"),
                                           ("All files", "*.*") ))
               #        fl = dlg.show()

    def exitProgram(self):

        root.destroy()
    def cvp(self):
        print ("Card View Panel")

    def fvp(self):
        print ("File view Panel")
root = tix.Tk()
app=App(root)


#fvp = Label(root, text = "File View Panel")
#cvp = Label(root, text = "Card View Panel")
#fvp.pack()
#cvp.pack()
root.mainloop()

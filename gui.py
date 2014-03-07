#!/usr/bin/python3



from tkinter import *
from tkinter import tix 
from tkinter.scrolledtext import * 

class App:
	def __init__(self,master): 
		frame =  Frame(root)
		frame.pack() 
		#menu bar
		menuBar = Menu(root)
		#self.master.config(menu=menubar)
		#menuBar.pack()
		
		fileMenu = Menu(menuBar)
		fileMenu.add_command(label = "exit")
		menuBar.add_cascade(label='File', menu = fileMenu)
		self.fvpFrame = Frame(frame) 
		self.cvpFrame = Frame(frame)

		self.logFrame = Frame(frame)
		self.logFrame.pack(side = BOTTOM)

		cvpTixThing = tix.TList(self.cvpFrame,height = 10, width = 40)
		cvpTixThing.pack()

		fvpTixThing = tix.TList(self.fvpFrame,height = 10, width = 40) 
		fvpTixThing.pack()

		scrolledLog = ScrolledText(self.logFrame,width = 40,height = 10)
		scrolledLog.pack() 
		self.cvpFrame.pack(side=TOP)
		self.fvpFrame.pack(side =TOP) 

		root.config(menu=menuBar)

	
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

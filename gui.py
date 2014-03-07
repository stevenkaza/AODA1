#!/usr/bin/python3



from tkinter import *
from tkinter import tix 
class App:
	def __init__(self,master): 
		frame =  Frame(master)
		frame.pack() 

		self.fvp = Button(
			frame, text="File View Panel", fg="red", command = self.fvp
			)
		self.fvp.pack(side=TOP)

		self.cvp = Button(
			frame,
			text = "Card View Panel",
			command = self.cvp)
		self.cvp.pack(side=BOTTOM) 
	
	def cvp(self):
		print ("Card View Panel")

	def fvp(self):
		print ("File view Panel") 
root = Tk()
app=App(root)


#fvp = Label(root, text = "File View Panel")
#cvp = Label(root, text = "Card View Panel") 
#fvp.pack()
#cvp.pack() 
root.mainloop()

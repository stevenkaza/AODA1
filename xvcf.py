#!/usr/bin/python3
#Steven Kazavchinski
#CIS 2750 
#A4 
#0761977    
import os 
import sys
import subprocess
import mysql.connector 

import string
from GMapData import *
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
#        self.createDbTables()
        self.sqlConnection()
        self.fname = None 
        self.cvpHasData = 0
        self.fvpHasData = 0
        #menu bar intilization 
        self.menuInit()
        #frames intilization
        self.framesInit()
        #Labels init
        self.labelsInit()
        #Table/HList Init      
        self.tablesInit(0)

        self.scrolledLog = ScrolledText(self.logFrame,width = 50,height = 10)
        self.title=("xvcf Vcutil");
        self.buttonsInit()

        logLabel = Label(self.logFrame,text = "Log Display Panel")
        logLabel.pack(side=TOP)
        self.scrolledLog.pack()

    #Code from Professor Gardners website        
    def sqlConnection(self):
        self.username = sys.argv[1]
        self.password = sys.argv[2]
        print (self.password)
        if len(sys.argv) == 3:
            self.hostname = "dursley.socs.uoguelph.ca"
        else:
            self.hostname = sys.argv[3]
        print (self.username,self.password,self.hostname)
        self.cnx = mysql.connector.connect(user=self.username,
                              password=self.password ,
                              host=self.hostname, 
                              database=self.username)
        self.cursor = self.cnx.cursor()
        print ("Creating Tables")
        query = "DROP TABLE PROPERTY;"

        self.cursor.execute(query)
        self.cnx.commit()
        query = "DROP TABLE NAME;"
        self.cursor.execute(query)
        self.cnx.commit()
        query = "CREATE TABLE IF NOT EXISTS NAME (name_id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR( 60 ) NOT NULL);"
        self.cursor.execute(query)        
        #query = "CREATE TABLE IF NOT EXISTS PROPERTY (name_id INT NOT NULL ,pname CHAR( 8 ) NOT NULL, pinst SMALLINT NOT NULL, partype TINYTEXT, parval TINYTEXT, value TEXT, FOREIGN KEY(name_id) REFRENCES NAME(name_id) ON DELETE CASCADE);"
        query = "CREATE TABLE IF NOT EXISTS PROPERTY( PRIMARY KEY(name_id,pname,pinst), FOREIGN KEY(name_id) REFERENCES NAME(name_id) ON DELETE CASCADE, name_id INT NOT NULL REFERENCES     NAME ON DELETE CASCADE, pname CHAR(8) NOT NULL, pinst SMALLINT NOT NULL, partype TINYTEXT, parval TINYTEXT, value TEXT);"
        self.cursor.execute(query)
#        query = "NSERT INTO NAME(name_id,name) VALUES(4,'broooo');"
 #       cursor.execute(query)
        self.cnx.commit()
  #      query = "SELECT * FROM NAME;"
   #     cursor.execute(query)
    #    for line in cursor:
     #       print  (line)
       # cursor.close()
       # cnx.close()
    def launchGoogle(self):
	
	#1. start CGI/HTTP server
        startWebServer(41977)

	#2. create GMapData object and populate with data
        gmd = GMapData( "maptest", "Univ. of Guelph", [43.530318,-80.223241], 14 )	# use default values
        photo = "http://www.uoguelph.ca/~gardnerw/head.gif" # photo url
        address = ";;50 Stone Road East\, Reynolds 105;\\nGuelph;Ontario;N1G2W1" # address
        gmd.addPoint( [43.530318,-80.223241], photo, address )	# s.b. center of map
        gmd.addOverlay( 0, 1, 3 )	# single point, blue icon

	#3. generate HTML to serve
        gmd.serve( "public_html/index.html" );

	#4. launch browser
        launchBrowser( "http://localhost:41977/" )


        input( "Press enter:" )		# pause
	
        gmd.addPoint( [43.530318,-80.223241] )	# 3-point red line
        gmd.addPoint( [43.535538,-80.223461] )
        gmd.addPoint( [43.520758,-80.220681] )
        gmd.addOverlay( 1, 3, 0 )
	
        gmd.serve( "public_html/index.html" );
        launchBrowser( "http://localhost:41977/" )


        input( "Press enter:" )
	
        gmd.addPoint( [43.530318,-80.223241] )	# 3-point blue line
        gmd.addPoint( [43.515538,-80.220461] )
        gmd.addPoint( [43.520758,-80.225681] )
        gmd.addOverlay( 4, 3, 2 )
	
        gmd.serve( "public_html/index.html" );
        launchBrowser( "http://localhost:41977/" )


        print( "Close the Tk window to proceed with server shutdown..." )

	#5. kill servers
        killServers()
	
	
    def fileOpen(self):
        self.ftypes = [('vCard files', '*.vcf'), ('All files', '*')]
        returned_values = {}
        self.fname = askopenfilename(filetypes=(self.ftypes))                                                                     
        root.title(self.fname)
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
        
    def launchSelect(self):
        location = ""
        photo = ""
        url = ""
        if self.locationState.get() == 1:
            locaton = "g"
        if self.photoState.get() == 1:
           photo = "p"
        if self.urlState.get() == 1:
            url = "u"
        # removing the already present output.vcf
        os.system('rm output.vcf')
        print (self.fname)
        os.system('./vcftool -select '+photo+url+location+' <'+self.fname+"> o.vcf")
       # Vcf.freeFile();
            
        self.window.destroy()
        os.system("chmod +x o.vcf");
        self.clearPanels()
        self.clearCardLists()
        self.displaySelectedCards()
       
        #changes the global variable to cancelled state, so main program knows to not select cards
    def cancelSelect(self):
        self.cancelSelect = 1
        self.window.destroy
    
        
    def storeAll(self):
        #query = "INSERT INTO NAME (name) VALUES('"+self.foundName+"');"
        if len(self.cards)==0:
            print ("error")
            return 2
        for Card in self.cards:
            nameFound = 0 
            print ("new card")
            for Tuple in Card:
                pname = self.getPropName(Tuple)

                
                # If the tuple property is a name value,
                # we need to check if that name currently exists in the 
                #database, and if not, then we need to insert
                #that name and the cards properties into the proper tables
                if nameFound == 1: 
                   #insert all fields into prop table as long as partype and parval are not null
                   if Tuple[2] != None and Tuple[3] != None:
                       query = "INSERT INTO PROPERTY(name_id,pname,pinst,partype,parval,value) VALUES(LAST_INSERT_ID(),'"+pname+"',1,'"+Tuple[2]+"','"+Tuple[3]+"','"+Tuple[1]+"');"
                   # if partype is null but parval is ok 
                   elif Tuple[2] == None and Tuple[3] != None:
                       query = "INSERT INTO PROPERTY(name_id,pname,pinst,parval,value) VALUES(LAST_INSERT_ID(),'"+pname+"',1,'"+Tuple[3]+"','"+Tuple[1]+"');"
                   # if partype is not null but parval is null
                   elif Tuple[2] != None and Tuple[3] == None:
                       query = "INSERT INTO PROPERTY(name_id,pname,pinst,partype,value) VALUES(LAST_INSERT_ID(),'"+pname+"',1,'"+Tuple[2]+"','"+Tuple[1]+"');"
                   # if parval and partype are null
                   elif Tuple[2] == None and Tuple[3] == None:
                       query = "INSERT INTO PROPERTY(name_id,pname,pinst,value) VALUES(LAST_INSERT_ID(),'"+pname+"',1,'"+Tuple[1]+"');"
                   self.cursor.execute(query)
                   self.cnx.commit()
                # if its the first name in the card (checks name value for 3 which represents name a
                    #name flag to be 0)
                if Tuple[0] == 3 and nameFound == 0:

                     self.foundName = Tuple[1]
                     query = "SELECT name FROM NAME WHERE NAME = '"+self.foundName+"';"
                     print (query)
                     self.cursor.execute(query)
                     #seeing if the results from the query are empty 
                     checkFlag = False
                     hasName = 0 
                     for line in self.cursor:
                        hasName = 1
                        print ("line = ")
                        print (line)
                     if hasName == 1: #if the name exists already, its time to bring up a popup
                         print ("has name")   
                         modulPopup = Toplevel()
                         self.frame.wait_window(modulPopup)
                         modulLabel = "Name already in table"
                         print (modulLabel)
                     #if its not in the table of names
                     elif hasName ==0:
                        print ("name did not exist")
                        #if cards first name hasnt been stored yet
                        if nameFound == 0:
                            query = "INSERT INTO NAME(name) VALUES('"+self.foundName+"');"
                            self.cursor.execute(query)
                            self.cnx.commit()
                            nameFound = 1 
         
                            # but no matter what, we still insert the prop
                        

        
        #print (self.cards)
        print("Storing all")    
        # check if name already is in Name table, if not insert it 
        # go thru the cards one by one 
        #query = "INSERT "
    def getPropName(self,Tuple):
        if Tuple[0] == 0:
            pname = "BEGIN"
        elif Tuple[0] == 1:
            pname = "END"
        elif Tuple[0] == 2:
            pname = "VERSION"
        elif Tuple[0] == 3:
            pname = "N" 
        elif Tuple[0] == 4:
            pname = "FN"
        elif Tuple[0] == 5:
            pname = "NICKNAME"
        elif Tuple[0] == 6:
            pname = "PHOTO" 
        elif Tuple[0] == 7:
            pname = "BDAY"
        elif Tuple[0] == 8:
            pname = "ADR"
        elif Tuple[0] == 9:
            pname = "LABEL" 
        elif Tuple[0] == 10:
            pname = "TEL"
        elif Tuple[0] == 11:
            pname = "EMAIL"
        elif Tuple[0] == 12:
            pname = "GEO" 
        elif Tuple[0] == 13:
            pname = "TITLE"
        elif Tuple[0] == 14:
            pname = "ORG"
        elif Tuple[0] == 15:
            pname = "NOTE" 
        elif Tuple[0] == 16:
            pname = "UID"
        elif Tuple[0] == 17:
            pname = "URL"
        elif Tuple[0] == 18:
            pname = "OTHER" 
        return pname 
    def clearPanels(self):
        self.cardViewScrolledList.hlist.delete_all()
        self.fileViewScrolledList.hlist.delete_all()
    def displaySelectedCards(self):
        print (self.fname) 

        self.fname = "/home/undergrad/0/skazavch/testA2/AODA1/AODA1/o.vcf"
        self.displayFileInfo()
        #Vcf.readFile("/home/undergrad/0/skazavch/testA2/AODA1/AODA1/o.vcf")
        print ( "Do we get here? ")
        #card  = []
       # self.cards = [] 
       # numCards = Vcf.getNumCards(); 
        #i = 0 
        #while i < numCards:
         #   Vcf.getCard(card)
          #  self.cards.append(card); 
           # i = i + 1
    def clearCardLists(self):
        del self.cards[:]
    def drawChecks(self):
        integer = 2
    def badFile(self,status):
        messagebox.showinfo("Error","Error code " + str(status)+ "detected. Please choose a proper vcf file")
    def displayFileInfo(self):
        #Deleting the panel 
        self.fileViewScrolledList.hlist.delete_all()
        self.cardViewScrolledList.hlist.delete_all()
        #opening the output file and updating the log
        os.system('./vcftool -info <' +self.fname+' >output.vcf')
        with open("output.vcf") as f:
            content = f.read()
        self.scrolledLog.insert(END,content)    
        #self.scrolledLog.state = 'DISABLED'
        status =  Vcf.readFile(self.fname)

        while status !=0:
            self.badFile(status)
            status = Vcf.readFile(self.fname) 
            self.fname = askopenfilename(filetypes=(self.ftypes))
            status = Vcf.readFile(self.fname)
        #List to hold cards 
        self.cards = []
        i = 0
        numCards=Vcf.getNumCards() 
        print(numCards)
        while (i < numCards):
             #List to hold each card 
             card=[]
             Vcf.getCard(card)	
             self.cards.append(card)
       	     i = i + 1
        foundFN = 0
        adrCount = 0 
        telCount = 0 
        i = 0 
        for Card in self.cards:
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
         #updating the CVP for the first intial card selection
        self.updateCVP(self.cards[0])
        print (self.cards[0])
       
	 #cards(0)=card
        #self.updateCVP(numCards,cards)
    # wait for the process to terminate
        #os.system('./vcftool -info <' +self.fname+' >output.vcf')

    
    def exitProgram(self):
        root.destroy()
    # Returns the row # that was selected by mouse in file view panel
    def key(self,event):
        rowNum = event.widget.nearest(event.y)
        rowNUM=int(rowNum)
        self.updateCVP(self.cards[rowNUM])
	
    def updateFVP(self,firstFNVal,adrCount,telCount,rowNum):
        if self.fvpHasData==1:
             self.fileViewScrolledList.hlist.delete_all()
             self.fvpHasData=0
        self.fileViewScrolledList.hlist.add((rowNum-1))
        #card sequence, displaying the results for each row
        strRowNum = str(rowNum)
        sstrRowNum = str.format(strRowNum) 
        strAdrCount = str(adrCount)
        strTelCount = str(telCount)
        self.fileViewScrolledList.hlist.item_create((rowNum-1),0,text = strRowNum)
        self.fileViewScrolledList.hlist.item_create((rowNum-1),1,text = firstFNVal)
        self.fileViewScrolledList.hlist.item_create((rowNum-1),4,text = strAdrCount)
        self.fileViewScrolledList.hlist.item_create((rowNum-1),5,text = strTelCount)
        

    def updateCVP(self,card):
        
    
            #self.cardViewScrolledList.hlist.item_create("R"+str(i),0,text = "card "+str(i))
        if (self.cvpHasData==1):
            i = 0
            self.cardViewScrolledList.hlist.delete_all()
            self.cvpHasData=0
        i = 0 
        for prop in card:
            
            self.cardViewScrolledList.hlist.add(i)
            i=i+1
       # for card in cards: 
        i = 0 
        for Tuple in card:
            self.cardViewScrolledList.hlist.item_create(i,0,text= str(Tuple[0]))   
            self.cardViewScrolledList.hlist.item_create(i,3,text =Tuple[1])
            self.cardViewScrolledList.hlist.item_create(i,1, text =Tuple[2])
            self.cardViewScrolledList.hlist.item_create(i,2,text = Tuple[3])
            i = i + 1
        self.cvpHasData = 1 
        self.fvpHasData = 1
    #def framesInit(self):
    def buttonsInit(self):
        #CVP Buttons
        upButton = Button(self.cvpFrame, text = "  Up  ")
        downButton = Button(self.cvpFrame, text = "Down ")
        addPropButton = Button(self.cvpFrame, text = "Add Property")
        deleteButton = Button(self.cvpFrame, text = "Delete ")
        revertButton = Button(self.cvpFrame, text = "Revert ")
        commitButton = Button(self.cvpFrame, text = "Commit")

        #FILE VIEW BUTTONS
        mapButton = Button(self.fvpFrame, text = " Map Selected",command = self.launchGoogle)
        resetButton = Button(self.fvpFrame, text = "Reset Map")
        browseButton = Button(self.fvpFrame, text = "Browse selected")
        deleteSelButton = Button(self.fvpFrame, text = "Delete Selected ")
        addButton = Button(self.fvpFrame, text = "Add card ")

        #Log button/clear
        clearButton = Button(self.logFrame, text = "Clear",command = self.clearLog)
        clearButton.pack(side=BOTTOM,pady = 8)
          
        downButton.pack(side= BOTTOM, padx = 10,pady=5)
        upButton.pack(side=BOTTOM, padx = 10,pady=5)
        addPropButton.pack(side= BOTTOM, padx = 10,pady=5)
        deleteButton.pack(side=BOTTOM, padx = 10,pady=5)
        revertButton.pack(side= BOTTOM, padx = 10,pady=5)
        commitButton.pack(side=BOTTOM, padx = 10,pady=5)

        mapButton.pack(side= BOTTOM, padx = 10,pady=5)
        resetButton.pack(side=BOTTOM, padx = 10,pady=5)
        browseButton.pack(side=BOTTOM, padx = 10,pady=5)
        deleteSelButton.pack(side=BOTTOM, padx = 10,pady=5)
        addButton.pack(side= BOTTOM, padx = 10,pady=5)

        
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
        dbMenu = Menu(menuBar)
        dbMenu.add_command(label = "Store All",command = self.storeAll)
        dbMenu.add_command(label = "Store Selected")
        dbMenu.add_command(label = "Open From Database")
        dbMenu.add_command(label = "Append From Database")
        dbMenu.add_command(label = "Query", command = self.launchQueryWindow)
        menuBar.add_cascade(label = 'Database', menu = dbMenu)
        helpMenu = Menu(menuBar)

        helpMenu.add_command(label = "Card flags and colours")

        helpMenu.add_command(label = "About xvcf",command = self.aboutProgram)
        menuBar.add_cascade(label='Help', menu = helpMenu)
        root.config(menu=menuBar)
    def launchQueryWindow(self):
        self.queryWindow = Toplevel() 
        labelQuery = Label(self.queryWindow, text = "Query time")
        queryTop = Frame(self.queryWindow)
        queryTop.pack(side = TOP)
        queryBot = Frame(self.queryWindow)
        queryBot.pack(side = BOTTOM)

        self.selecttextBox = Entry(queryTop)
        self.selecttextBox.pack(side = TOP)
        self.selecttextBox.insert(0, "SELECT ")

       
        self.queryResults = ScrolledText(queryBot, width = 20, height = 10)
        self.queryResults.pack(side=TOP)
        clearQuery = Button(queryBot, text = "Clear",command = self.clearQueryPanel)
        clearQuery.pack(side=BOTTOM)
        submitQuery = Button(queryTop, text = "Submit" , command = self.sendQuery)
        submitQuery.pack(side=BOTTOM)
        queryHelp = Button(queryTop, text = "Help" , command =  self.launchHelp)
        queryHelp.pack(side = RIGHT)
    def launchHelp(self):
        print( "query help")
    def clearQueryPanel(self):
        self.queryResults.delete(0.0,END)

    def sendQuery(self):
        print ("submitting query")
         
        query = self.selecttextBox.get()
        #Auto fixing query to make sure it has a semiclon
        #at the end incase user forgot to enter it
        if query.find(";") == -1:
            query = query + ";"
        upperQuery = query.upper()
        commitFlag = 0
        if upperQuery.find("INSERT") != -1:
            commitFlag = 1
        else:
            commitFlag = 0
        self.cursor.execute(query)
        if commitFlag == 1:
            self.cnx.commit()
        for value in self.cursor:
            #value = value + "\n"
            self.queryResults.insert(END,value)
            self.queryResults.insert(END,"\n")
            print (value)

    def framesInit(self):
        self.frame =  Frame(root,width = "1024")

        self.frame.grid()
        self.fvpFrame = Frame(self.frame)
     #   self.fvpFrame.pack(side = LEFT)
        self.cvpFrame = Frame(self.frame)
       


        self.fvpFrame.grid(row=0,column = 0)
    #    self.cvpFrame.pack(side=TOP)
       # self.fvpFrame.pack(side =TOP)
        # LOGS ---------------------#

        self.logFrame = Frame(self.frame)
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
        self.fileViewScrolledList = tix.ScrolledHList(self.fvpFrame, width = 70,
         options='hlist.columns 7 hlist.header 1 hlist.selectBackground white ')
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
        self.fileViewScrolledList.hlist.bind("<Button-1>",self.key)

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
root.title("xvcf");
app=App(root)
#mlb = MultiListbox(tk, (('Subject', 40), ('Sender', 20), ('Date', 10)))

#fvp = Label(root, text = "File View Panel")
#cvp = Label(root, text = "Card View Panel")
#fvp.pack()
#cvp.pack()
root.mainloop()


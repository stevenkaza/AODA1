/* vcutil.c
/*
vCard  utlity library/ Parser 

Author:Steven Kazavchinski 
Created: January 24th 2014 
Contact: skazavch@uoguelph.ca
*/ 


/* Added space input for getUnfolded*/ 

/* 
 Check hook for being NULL */ 

/* Add vcp org to assignPropName */ 

/* Store entire buff as a value if propname = other */ 
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vcutil.h"
#include <ctype.h>


/* Assigns property name into a VcProp struct
   Also determines which property name to assign based on string 
  
  
RETURNS: The enumrated value of what prop name was found */ 


int assignPropName(VcProp * const propp,char * propName);

/*
Returns a 1 if a semi colon was found before a colon */ 
int semiFirst(char * tempString);

/* writePropName 
  Writes a property name to stdout. 
  RETURNS: string length in int of whatever property name was written to the file
  -1 if vcp_other */ 

int writePropName(VcPname name);


int Contains(char * string, char pattern); 
int checkPosition(char * string, int position); 
void removeSpaces(char * string);
int removeNewLine(char * string);
VcStatus readVcFile (FILE *const vcf, VcFile *const filep)
{
    /* Assigning default values for filep */ 
    filep->ncards = 0;
    filep->cardp = NULL;
    VcStatus newStatus;


    int i=0;
    /* Checking for invalid file */
    if (vcf==NULL)
    {
        newStatus.code =  IOERR;
    }
    /* By default, assigning code to be OK */ 
    newStatus.code = OK;
   
    /* Until end of file */
   while (feof(vcf)==0)
    { 
        /* Incrementing # of cards */ 
        filep->ncards=filep->ncards+1;
	/* Allocating for a pointer to go inside the array of card pointers */ 
        filep->cardp=realloc(filep->cardp,(sizeof(Vcard*)*filep->ncards));
        if (newStatus.code==OK)
          newStatus = readVcard(vcf,&filep->cardp[i]);

       if ( filep->cardp[i]==NULL)
        {
	          filep->ncards=0;
	          //free(filep);
           break;
        }
        i++;

        if (newStatus.code!=OK)
	         break;


   }/*end of while loop */ 
    printf("i=%d\n",i );
    return newStatus;
}

/* Precondition: 
   vcf is open for reading. cardp points to 
   a variable where the address of a newly allocated Vcard will be returned.

   Postcondition:  
    If status=OK, and * cardp is non-NULL,
    the Vcard structure has been filled in. OK status and NULL *
    cardp means normal EOF, and VcStatus line numbers are the total lines read from the file.
    If error status, * cardp contents are undefined (but no additional storage remains allocated). */

VcStatus readVcard( FILE * const vcf, Vcard **const cardp)
{
    VcStatus newStatus; 
    char  *buff=NULL;
  //  char testString[2121212];

    int fnFlag = 0;
    int nFlag = 0;  
    int endFlag=0;
    (*cardp)=NULL;
    VcError error; 
    int versionFlag =0; /*If version flag stays 0, then we know no version was found */ 

    int i = 0;
    VcProp * tempProp = NULL;
    int beginFlag=0; 
   // for (i=0;i<12112;i++)
     //  testString[i]='t';
    /* How do access the individual vcards in here */ 

    if (vcf==NULL)
    {
        newStatus.code = IOERR; 
    }
     /*Checks for begin, and version  */ 
    do
    {
         /* In the case the file is null, exit this loop */ 
         while (feof(vcf)!=0)
         {
            goto end;
         }
         newStatus=getUnfolded(vcf,&buff);
         printf("Buff = %s\n\n",buff);


	
	     // printf("buff!! = %s\n",buff);
        /* If we see another begin before an end, 
                                        return an ERROR */
         /* If buff is null right away, we had an empty file */ 
         if (i==0 && buff ==NULL)
	       {
	           //  if (*(cardp)==NULL)
	          //{    printf("ERROR\n\n\n");
             goto end;  
             // }
	        }
          	assert(buff);
         if (buff[0] == ':' || buff[0] == ';')
         {
            newStatus.code = SYNTAX;
            assert(buff);
	          free(buff);
            return newStatus;
         }
        // printf("buff = %s\n",buff);
        if (beginFlag==1)  /* Ensuring no Two Begins in a row */ 
        {
            if (strcmp("BEGIN:VCARD",buff)==0) 
            {
	              printf("here?\n");
                newStatus.code =BEGEND; 
                goto end;
            }
        }

        /*Ensuring line begins with BEGIN */ 
        /* If not, set an error */ 
        if (beginFlag==0) 

        {
            if (strcmp("BEGIN:VCARD",buff)==0)
            {
                beginFlag=1;
            }
            else
            {
                printf("here?\n");
                newStatus.code = BEGEND;
                goto end; 
            }
        }
    
        /* Checking for FN / N */ 

        if (buff[0]=='F')
        {
          if (buff[1]=='N')
          {
            fnFlag=1; 
          }
        }
         
        if (buff[0]=='N')
        {
          if (buff[1]==';' || buff[1] == ':')
              nFlag=1; 
        }
        
       /* if we reach the end of the vcard, go to the  flag checks */ 
       if ((strcmp("END:VCARD",buff) ==0) && (beginFlag==1))
       {
          endFlag=1;
      	  assert(buff);
      	  free(buff);
      	  assert(buff);
          goto check;
       }
       /* If the buff contains version, check that its the proper version number */ 

       if (strstr(buff,"VERSION:")!=NULL)
       {
	    //    printf("i = %d, buff = %s\n",i,buff);
          versionFlag=1;
      	  if (i!=0)
      	     	versionFlag=0;
          if ((buff[8] != '3') ||  (buff[9] != '.') || (buff[10]!='0'))
          {
            newStatus.code = BADVER; 
	          free(buff);
            goto end; 
          }
       }
       /*This check allows begin, end, and version properties to be ignored
       and not assigned to the data structure */ 
       if ((newStatus.code == OK) && (strcmp("END:VCARD",buff)!=0) &&
          (strcmp("BEGIN:VCARD",buff)!=0) && (strcmp("VERSION:3.0",buff)!=0))
       {
            /*Send the buff for parsing */
            buff[strlen(buff)]='\0';
            /* Allocating for a VcProp struct  for property */ 
            tempProp=malloc(sizeof(VcProp));
	          assert(tempProp);
            if (strstr(buff,":")!=NULL) /* Only pass it to parseVcProp if it has a colon in it.
                                          * Avoiding empty lines */ 
           error=parseVcProp(buff,tempProp);
	         free(buff);
            if (error!=OK)
            {
              newStatus.code = error;
              goto end;
            }
            /* If we are on the first prop, we simply alloc for 1*/ 
            if (i==0)
            {
              (*cardp)=malloc(sizeof(Vcard)+sizeof(VcProp));
	            (*cardp)->nprops=1;	
            } 

            else
	          {
               (*cardp)=realloc((*cardp),sizeof(Vcard)+(sizeof(VcProp)*(i+1)));
            }
            (*cardp)->prop[i]=*tempProp;
            i=i+1;
          //  free(buff);
        
	    if (newStatus.code == BEGEND)
            {
              goto end; 
            }
            
             (*cardp)->nprops=i;
	         free(tempProp);
       }
       else
    {    
      free(buff); /* buff is not needed. free it */ 
    }
      // if (buff==NULL)
       //:q	: break;
				
    } while (buff!=NULL);
     
    /* If we went through the entire vcard and couldnt find a FN, error */ 
    /* FLAG CHECKING FOR ERRORS */ 
    check:
    if (fnFlag ==0 || nFlag==0)
    {
       newStatus.code=NOPNFN;
    }

    if (endFlag==0 || beginFlag==0)
    {


       newStatus.code=BEGEND;
    }
    
    if (versionFlag==0 ) /* No version was found */ 
    {
       /*Since this error only found at the end, subtract the line to get
       back to the beggining where ver would be */ 
       newStatus.lineto=newStatus.lineto-i;
       newStatus.linefrom=newStatus.lineto;
       newStatus.code =NOPVER;
    }
      
   // buff=NULL;
    end:
    buff = NULL;
   if (endFlag==0&&(*cardp)!=NULL)
      newStatus.code = 4; 
    return newStatus;

}



VcStatus getUnfolded ( FILE * const vcf, char **const buff )
{
   /*CRLF Flags */ 
    int rFlag=0; 
    int crlfFlag=0; 
    int i =0; /* String indexing */ 
    static int endOfFile=0; 
    static char ch; /* static char to hold the last char read in for getUnfolded next call */ 
    static int staticFlag=0; /* this would be 1 its second call*/ 
    char * tempString=NULL;
    int lineDoneFlag = 0; /*Flag to indicate the buff is ready to be returned and is unfolded */ 
    static int lineCounter=0; 
    VcStatus newStatus;
    newStatus.code = OK;  
    static int foldedFlag = 0; /* this will keep value as well. Hmm*/ 
    newStatus.lineto = lineCounter; 
    if (foldedFlag==0)
        newStatus.linefrom = lineCounter;
    
    /* Satisfiying the "special" case where vcf is NULL */ 
    if (vcf==NULL)
    {
  //    printf("PPLZPLZ\n");
      lineCounter=0; 
      endOfFile=0; 
      foldedFlag=0; 
      newStatus.lineto=0;
      newStatus.linefrom=0; 
      staticFlag = 0; 
      if (tempString!=NULL)
        free(tempString);
      //*buff =NULL;
      return newStatus;  
    }
    /* Looping through char by char until a crlf is found, then a flag is set and 
      those chars are not read into the buff */ 
    do 
    {
      
      if (staticFlag!=1)/* Skips over a char if its static to avoid missing one */ 
         ch = fgetc(vcf); /* Reading a char in */ 
        // printf("c = %c\n",ch);
        if (ch==EOF&&endOfFile==1)
        {
//           *buff = NULL;
  //         return newStatus; 
        }
        endOfFile=1; /*We know that we dont have an empty file */ 
        switch (ch)
        {

            case '\r':
            {
               // printf("R FLAG???\n");
                rFlag = 1; 
                break;
            }
            case '\n': /* if its a new line, set crlf flag on */ 
            {
               // printf("N FLAG???\n");

            //  if (rFlag==1)
             // {
                crlfFlag=1; 

                if (foldedFlag==0)
                  newStatus.linefrom=newStatus.linefrom+1; /*Updating the line counters */
                newStatus.lineto = newStatus.lineto+1; 
             // }
              break;
            }

            case ' ': /* Its a space */ 
            {

              if (crlfFlag==1) /* If its a folded line, reset the crlf flag and increment lineto */ 
              {
                    crlfFlag=0;    
                    foldedFlag =1; 
              }

              else 
              {
                  /* If its the first character, allocate space for one*/ 
                  /* I counts the number of chars */ 
                  if (i==0)
                    tempString=(char *)malloc(sizeof(char));
                  else
                    tempString=realloc(tempString,sizeof(char)*(i+1));
                  tempString[i++] = ch; /* Duplicate code. Place outside while loop */ 
                  staticFlag = 0; 
              }
              break;
            }
            /* ADD PROPER PROCEDURE HERE AS ABOVE */ 
            case '\t':

               if (crlfFlag==1)
               {
                    newStatus.lineto = newStatus.lineto +1; 
                    crlfFlag=0;
                    foldedFlag=1; 
               }
              else 
              {
                  /* If its the first character, allocate space for one*/ 
                  /* I counts the number of chars */ 
                  if (i==0)
                    tempString=(char *)malloc(sizeof(char));
                  else
                    tempString=realloc(tempString,sizeof(char)*(i+1));
                  tempString[i++] = ch; /* Duplicate code. Place outside while loop */ 
                  staticFlag = 0; 
               }
               break;
                /* Flags */ 
            default:
            {
        /*If a new char is found while the crlf flag is still on, we are on a new flag so its time to exit */ 
              if (crlfFlag==1)
                {

                    staticFlag=1; /* Says DONT READ CHAR */ 
                    lineDoneFlag=1;
                    printf("DID IT TURN ON?\n");
                    /* We have not even seen a colon or a semi colon yet , blankline*/ 
		              	assert(tempString);
                    if (tempString !=NULL)
		               {
                    if (strspn(tempString," \r\n") && strlen(tempString) >0)
                    {
                      printf("GOING HERE EVERY TIME?\n");
                      free(tempString);
                      i=0;
                      lineDoneFlag=0;
                      crlfFlag=0;
                    }
                  }

                }
                if (crlfFlag==0) 
                {
                   /* Allocating for first char, otherwise reallocating
      f               *    for subsequent chars */ 
                   if (i==0)
                       tempString=(char *)malloc(sizeof(char));
                   else
                       tempString=realloc(tempString,sizeof(char)*(i+1));
                    /* Store it in a temp buff as long as 
                      its not a terminating char or EOF */ 
                   if (ch!=EOF && ch!= '\0') 
                         tempString[i++] = ch; 
                    staticFlag=0; /* Ready to grab the next line*/ 
                }

              break;
            }

        if (lineDoneFlag==1)
           break;
        }/*Switch Ends */ 

     if (lineDoneFlag==1)
            break;

    }while (ch!=EOF);
    /* Setting the null terminator for the string */ 
    if (ch==EOF)
    {
        tempString=realloc(tempString,sizeof(char)*(i+1));

        tempString[i]='\0';
        endOfFile=1; 
        if (strlen(tempString)<2)
        {
            free(tempString);
           *buff=NULL;
           return newStatus;
        }
       // staticFlag=1;
    }   
    else
    {
        tempString=realloc(tempString,sizeof(char)*(i+1));
        tempString[i]='\0';
    }
    /* Allocating space and assigning buff */ 
   // printf("TEMP STRING = %s\n",tempString);
 //   printf("linefrom = %d, lineto = %d \n",newStatus.linefrom,newStatus.lineto);
    *buff = (char*)calloc(strlen(tempString)+1,sizeof(char));
    strncpy(*buff,tempString,strlen(tempString)+1); /* Maybe remove this + 1 */ 
    free(tempString);

    //f (strlen(tempString)>0 && tempString!=NULL)
       // free(tempString);

    newStatus.code = OK;
    
    lineCounter = newStatus.lineto; /*Updating the static line counter for getUnfoldes next call */ 
      // if (ch==EOF && endOfFile==1&& strlen(*buff)==0)
  // *buff=NULL;

    return newStatus;


}


VcError parseVcProp(const char * buff,VcProp * const propp)
{

    char * propName; 
    char * copyString2;
    VcError error=OK; 
    
    propp->value=NULL;
    propp->partype=NULL;
    propp->parval=NULL;
    propp->hook=NULL;
    int i = 0;  /* String indexing */ 
    char * tempString; 

    int stateFlag = 0;  /* Parameter state flag */
    
    /* If these states are ever 1, it means time to read ch in */   
    int typeState = 0; 
    int valueState = 0;
    /* String indexes for value and type */   
    int vIndex = 0; 
    int tIndex = 0; 
    int m;
    char * value; 
    char *  valueValueString=NULL; /*to hold the value on the right side of the colon */ 
    int vvIndex = 0; 
    /* if 1, immediatly stop reading for types and values */ 
    int optionalFlagDone = 0; 
    /* 1 if theres a type/values by comma */ 
    int regularValueState=0; 
    
    char *  valueString=NULL;
    char * partypeString=NULL;
    char * parvalueString=NULL;
    valueString = (char*)calloc(200,sizeof(char));
    partypeString = (char*)calloc(200,sizeof(char));
    valueValueString=(char*)calloc(200,sizeof(char));
    parvalueString=(char*)calloc(200,sizeof(char));
    /* If this is set to 1, weve hit a colon and its time to parse values */ 
    int valueValue = 0; 
    tempString = (char*)calloc(strlen(buff)+1,sizeof(char));
    strcpy(tempString,buff);
    if (strstr(tempString,":")==NULL)
    {
       free(tempString);
       error=SYNTAX;
       return error;
    }
    /* Grabbing the value */ 
    if (semiFirst(tempString)==0 && periodFirst(tempString)!=2)
    {
       propName=strtok(tempString,":");
       assignPropName(propp,propName);
     //  printf("testing string %s \n",buff);

       if (propp->name==VCP_OTHER)
       {
            propp->value = (char *)malloc((strlen(buff)+1)*sizeof(char));
           // printf("BUFF=%s\n",buff );
            strcpy(propp->value,buff);
       }


       else 
       {
         value = strtok(NULL,"\n");
         propp->value = (char *)malloc((strlen(value)+1)*sizeof(char));
         strcpy(propp->value,value);
	// free(value);
       }
  
     }
     /* If a period comes before the colon, we have an optional group */ 
   else if (semiFirst(tempString)==0 && periodFirst(tempString)==2)
    {
      propp->name = VCP_NICKNAME;
      propName=strtok(tempString,":");
      value = strtok(NULL,"\n");
      propp->value = (char *)malloc((strlen(value)+1)*sizeof(char));
      strcpy(propp->value,value);

    }

    else /* Optional parameter parsing */ 
    {
    /* If there are optional parameters, but no = sign, return syntax error*/ 
    if (strstr(buff,"=")==NULL)
    {
      error=SYNTAX;
      return error; 
    }
    for (i=0;i<strlen(buff);i++)
    {
        if (valueValue==1)
        {
            valueValueString[vvIndex++]=buff[i];
        }
        if (buff[i]==':')
        {
            /* Reseting all other flags
                to ensure rest of string 
                only stored in proper variable */ 
           // if (typeState==1 && valueValue==0)
                if (regularValueState==0)
                partypeString[tIndex]='\0';
              else
                partypeString[tIndex]='\0';


       //     if (valueState==1&&valueValue==0)
		         {
		 parvalueString[vIndex]='\0';
 }  
          valueValue=1; 
            stateFlag =0; 
            valueState=0;
            typeState=0; 
        }

        /*This ignores semi colons that might be value values */ 
        if (buff[i]==';'  && valueValue!=1)
        {
      /* Reset all States if a semi colon is spotted */ 
         // vIndex=0;
        //  tIndex=0;
          valueState=0;
          typeState=0; 
            if (stateFlag!=1)
                stateFlag = 1; 

        }

        /* Storing the optional parameters as types or values, state based */ 
        if (valueState==1 || typeState ==1)
        {
            if (buff[i]==',')
            {
                regularValueState = 1; 
            }
      /* Store it in as a char to a type/value as long as its not a colon or semi colon */ 
            if (buff[i]!=';' && buff[i]!=':')
            {
                if (valueState==1)
		{
//		     printf("vc = %c\n",buff[i]);
                     parvalueString[vIndex++]=buff[i];
		}
                if (typeState==1)
		{
		   // printf("tindex = %d\n",tIndex);
//		    printf("tc = %c\n",buff[i]);
                    partypeString[tIndex++]=buff[i];
		}
           }
          else 
            {
		/* Upon Colon, Optional mode is done if it existed, and we move to valuevalue mode */ 

                if (buff[i]==':')
                {
              		  optionalFlagDone = 1;
              		  valueValue = 1; 
              		  valueState = 0; 
              		  typeState = 0; 
                
                }
            }
        }
        if (stateFlag ==1 && buff[i]=='=')
        {
	    /* IS IT A TYPE? */  
            if (buff[i-2] == 'P' || buff[i-2] == 'p')
            {
	      
                /* Turn the read in typeFlag on */ 
                typeState = 1; 
		/* if type already has something stored inside of it, we need to insert a comma */ 
          		if (tIndex>0)
          		  partypeString[tIndex++]=',';
            }
            /* IS IT A VALUE? */ 
            else if (buff[i-2] == 'u' || buff[i-2] == 'U')
            /* Check if its fursure value */ 
            {
	        /* read in value flag on  */ 
                valueState = 1; 
		            if (vIndex>0)
		              parvalueString[vIndex++]=',';
            }
            /* If an equal sign was found but no charset or encoding, parover */ 
            else if (strstr(buff,"CHARSET")==NULL&&strstr(buff,"ENCODING")==NULL)
            {
              error = PAROVER; 
              return error; 
            }

        }
	
    }
    
      if (tIndex>0)
      {
    	  propp->partype=(char *)malloc((strlen(partypeString)+1)*sizeof(char));
    	  strcpy(propp->partype,partypeString);  
	  free(partypeString);
	  partypeString=NULL;
      }
      if (vIndex>0)
      {
        propp->parval = (char *)malloc((strlen(parvalueString)+1)*sizeof(char));
        strncpy(propp->parval,parvalueString,strlen(parvalueString)+1);
        free(parvalueString);
	parvalueString=NULL;     
 }
      if (vvIndex>0) /* The only way a value value would get assil pgned here
      would be if it had optional parameters */ 
      /* assigning property name for a buff that has optional parameters */ 
      {
         propName=strtok(tempString,";");
         assignPropName(propp,propName);
	 propp->value = (char *)malloc((strlen(valueValueString)+1)*sizeof(char));
         strcpy(propp->value,valueValueString);
	 free(valueValueString);
	 valueValueString=NULL;
      }
    }
      /*if (vvIndex>0)
      {
        propp->value = (char *)malloc((strlen(valueValueString)+1)*sizeof(char));
        strncpy(propp->value,valueValueString,strlen(valueValueString)+1);	
      }*/
      //if (vvIndex==0)
	     //  error=SYNTAX;
      free(tempString);
      if (parvalueString!=NULL)
	free(parvalueString);
     if (partypeString!=NULL)
	free(partypeString);
    if (valueValueString!=NULL)
	free(valueValueString);
	if (valueString!=NULL)
	free(valueString);     
 return error; 
      
}


VcStatus writeVcFile(FILE  *const  vcf, VcFile const *filep)
{
  int name; 
  int result = 0; 
  int i = 0;
  int k = 0;
  int j = 0; 
  int foldedFlag = 0; 
  int charCounter = 0; /* A counter for each line */ 
  char * propNames[20];
  for (i=0;i<20;i++)
  {
    //propName[i] = malloc(sizeof(char)*10);
  }

  

  for (i=0;i<filep->ncards;i++)/*Indexing through each card */
  {

    fprintf(stdout,"BEGIN:VCARD\r\n");
    fprintf(stdout,"VERSION:3.0\r\n");
    for (k=0;k<filep->cardp[i]->nprops;k++) /* Indexing through the properities of each card */ 
    {
        charCounter = 0; /* reset the charCounter for each line */ 
        result=writePropName(filep->cardp[i]->prop[k].name);
        if (result!=-1)
        {
          charCounter=charCounter+result; 
        }
        if (filep->cardp[i]->prop[k].partype!=NULL)
        {
              charCounter=charCounter+strlen(filep->cardp[i]->prop[k].partype);
              fprintf(stdout,";TYPE=%s",filep->cardp[i]->prop[k].partype);
        charCounter=charCounter + strlen(";TYPE=");
        }
        if (filep->cardp[i]->prop[k].parval!=NULL)
        {
             charCounter=charCounter+strlen(filep->cardp[i]->prop[k].parval);
             fprintf(stdout,";VALUE=%s",filep->cardp[i]->prop[k].parval);
       charCounter = charCounter + strlen(";VALUE=");
        }
        if (filep->cardp[i]->prop[k].value!=NULL)
        {
          charCounter=charCounter+strlen(filep->cardp[i]->prop[k].value);
          if (charCounter < 75)
           fprintf(stdout,":%s\r\n",filep->cardp[i]->prop[k].value);
    else
    {
    charCounter=charCounter-strlen(filep->cardp[i]->prop[k].value);
    for (j=0;j<strlen(filep->cardp[i]->prop[k].value);j++)
    {
  //    if (foldedFlag==0)
  //      charCounter = charCounter + j; 
  //    else
        charCounter = charCounter + 1; 
      if (j==0)
      {
        charCounter++;
        fprintf(stdout,":");
      }
      if (charCounter ==75)
      {
         fprintf(stdout,"%c\r\n",filep->cardp[i]->prop[k].value[j]);
         fprintf(stdout," ");
         charCounter=0; 
         foldedFlag=1; 
               continue; 
      } 
      fprintf(stdout,"%c",filep->cardp[i]->prop[k].value[j]);     
  
    }
    foldedFlag=0; 
    fprintf(stdout,"\r\n");
    




    }
        }
      
    }
    fprintf(stdout,"END:VCARD\r\n");


}
}

int writePropName(VcPname name)

{

      if (name == VCP_N)
      {
        fprintf(stdout,"N");
        return 1; 
      }
      if (name == VCP_FN)
      {
        fprintf(stdout,"FN");
        return 2; 
      }
      if (name == VCP_NICKNAME)
      {
        fprintf(stdout,"NICKNAME");
        return strlen("NICKNAME");
      }
      if (name == VCP_PHOTO)
      {
        fprintf(stdout,"PHOTO");
        return strlen("PHOTO");
      }
      if (name == VCP_BDAY)
      {
        fprintf(stdout,"BDAY");
        return strlen("BDAY");
      }
      if (name == VCP_ADR)
      {
        fprintf(stdout,"ADR");
        return strlen("ADR");
      }
      if (name == VCP_LABEL)
      {
        fprintf(stdout,"LABEL");
        return strlen("LABEL");

      }
      if (name == VCP_TEL)
      {
        fprintf(stdout,"TEL");
        return strlen("TEL");
      }     

      if (name == VCP_EMAIL)
      {
        fprintf(stdout,"EMAIL");
        return strlen("EMAIL");
      }
      if (name == VCP_GEO)
      {
        fprintf(stdout,"GEO");
        return strlen("GEO");
      }
      if (name == VCP_TITLE)
      {
        fprintf(stdout,"TITLE");
        return strlen("TITLE");
      }     
      if (name == VCP_ORG)
      {
        fprintf(stdout,"ORG");
        return strlen("ORG");
      }
      if (name == VCP_NOTE)
      {
        fprintf(stdout,"NOTE");
        return strlen("NOTE");
      }
      if (name == VCP_UID)
      {
        fprintf(stdout,"UID");
        return strlen("UID");
      }     
      if (name == VCP_URL)
      {
        fprintf(stdout,"URL");
        return strlen("URL");
      }
      if (name == VCP_OTHER)
      { 
        return -1; 
      }
    
}

void freeVcFile ( VcFile * const filep)
{
        int i, j;
        Vcard * tmp;
        VcProp * tmp2;
 
        for(i = 0; i < filep->ncards; i++)
        {
                tmp = filep->cardp[i];
 
                for(j = 0; j < tmp->nprops; j++)
                {
                        tmp2 = &(tmp->prop[j]);
			if (tmp2->partype!=NULL)
  	                      free(tmp2->partype);
  			if (tmp2->parval!=NULL)
                       	      free(tmp2->parval);
			if (tmp2->value!=NULL)
                       	    free(tmp2->value);
			if (tmp2->hook!=NULL)
                       		 free(tmp2->hook);
                }
                free(tmp);
        }
        free(filep->cardp);
}

int assignPropName(VcProp * const propp,char * propName)
{
    if (strcmp("BEGIN",propName)==0)
    {
        propp->name=VCP_BEGIN;
        return 0; 
    }
    if (strcmp("END",propName)==0)
    {
        propp->name=VCP_END;
        return 1; 
    }
    if (strcmp("VERSION",propName)==0)
    {
        propp->name=VCP_VERSION;
        return 2; 
    }
    if (strcmp("N",propName)==0)
    {
        propp->name=VCP_N;
        return 3; 
    }
    if (strcmp("FN",propName)==0)
    {
        propp->name=VCP_FN;
        return 4; 
    }
    if (strcmp("NICKNAME",propName)==0)
    {
        propp->name=VCP_NICKNAME;
        return 5; 
    }
    if (strcmp("PHOTO",propName)==0)
    {
        propp->name=VCP_PHOTO;
        return 6; 
    }
    if (strcmp("BDAY",propName)==0)
    {
        propp->name=VCP_BDAY;
        return 7; 
    }
    if (strcmp("ADR",propName)==0)
    {
        propp->name=VCP_ADR;
        return 8; 
    }
    if (strcmp("TEL",propName)==0)
    {
        propp->name=VCP_TEL;
        return 9; 
    }

    if (strcmp("EMAIL",propName)==0)
    {
        propp->name=VCP_EMAIL;
        return 10; 
    }
    if (strcmp("GEO",propName)==0)
    {
        propp->name=VCP_GEO;
        return 11; 
    }
    if (strcmp("TITLE",propName)==0)
    {
        propp->name=VCP_TITLE;

        return 12; 
    }
    if (strcmp("ORG",propName)==0 || (strcmp("Org",propName) == 0))
    {
        propp->name=VCP_ORG;
        return 13; 
    }
    if (strcmp("NOTE",propName)==0)
    {
        propp->name=VCP_NOTE;
        return 14; 
    }
    if (strcmp("UID",propName)==0)
    {
        propp->name=VCP_UID;
        return 15; 
    }
    if (strcmp("URL",propName)==0)
    {
        propp->name=VCP_URL;
        return 16; 
    }
    if (strcmp("OTHER",propName)==0)
    {
        propp->name=VCP_OTHER;
        return 17; 
    }
    if (strcmp("LABEL",propName)==0)
    {
	propp->name=VCP_LABEL;
	return 18; 
    }
    
    propp->name=VCP_OTHER;
    return 0;



}
int semiFirst(char * tempString)
{
    int i = 0; 
    int semiFlag=0;
    int colonFlag=0;
    for (i=0;i<strlen(tempString);i++) /* Test string: Kirk:Stinky; */ 
    {
        /* Looking for colon */ 
      if (tempString[i] == ':')
        {
          if (semiFlag ==1) /* we found semi first */ 
                colonFlag=0;
            else 
            {
                colonFlag = 1; 
                return 0;
            }
            break;
        }
      if (tempString[i] == ';')
        {
          if (colonFlag ==1) /* We found colon first */ 
            {
                semiFlag = 0; 
            }
            else
            {
                return 1; 
                semiFlag = 1; 
            }
            break;
        }
    }
 
 return 0;
}


int periodFirst(char * tempString)
{
    int i = 0; 
    int periodFlag=0; 
    int semiFlag=0;
    int colonFlag=0;
    for (i=0;i<strlen(tempString);i++) /* Test string: Kirk:Stinky; */ 
    {
        /* Looking for colon */ 
      if (tempString[i] == ':')
        {
          if (semiFlag ==1) /* we found semi first */ 
                colonFlag=0;
            else 
            {
                colonFlag = 1; 
                return 0;
            }
            break;
        }
      if (tempString[i] == ';')
        {
          if (colonFlag ==1) /* We found colon first */ 
            {
                semiFlag = 0; 
            }
            else
            {
                return 1; 
                semiFlag = 1; 
            }
            break;
        }

        if (tempString[i]=='.' && colonFlag==0 && semiFlag==0)
          return 2; 
    }
 
 return 0;
}



/* int Contains(char * string, char pattern);
 * Returns: position where 0; pattern was found 
 * 0 if nothing found                   */ 
int Contains(char * string, char  pattern)
{
    int i =0;
    if (string=='\0')
        return 0;
    for (i=0;i<strlen(string);i++)
    {
      if (string[i]==pattern)
        {
            return i; 
        } 
    }
    return 0; 
}
/* Returns a 1 if a pattern is found at 
  a certain location */
int checkPosition(char * string, int position)
{
    if ((string[position+1] == '\n'))// && (string[position+2]==' '))
    {
        return 1; 
    }
    return 0; 
}


void removeSpaces(char * string)  /*Function to remove spaces, found from  http://roseindia.net/c-tutorials/c-string-remove-spaces.shtml */ 
{
        

  char *p1 = string;
  char *p2 = string;
  p1 = string;
  
  while(*p1 != 0) 
  {         
     if(isspace(*p1)) /*if a space is found, rearange the pointers so the spaces are ignored */ 
  {
     ++p1;
  }
  
     else
     *p2++ = *p1++; 
  }
    
  *p2 = 0; 


}



int removeNewLine(char * string)
{
    int i = 0; 
    for (i=0;i<=strlen(string);i++)
    {
      if (string[i]=='\n')
        {
            string[i]='\0';
            return 1; 
        }

    }
    return 0; 
}



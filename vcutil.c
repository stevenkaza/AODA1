
/* vcutil.c

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
	          printf("Were Done  %d\n",newStatus.code);
	          filep->ncards=0;    
	          //free(filep);
           break;
        }
        i++;

        if (newStatus.code!=OK)
	         break;


   }/*end of while loop */ 
    
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
    int fnFlag = 0;
    int nFlag = 0;  
    int endFlag;]
    (*cardp)=NULL;
    VcError error; 
    int versionFlag =0; /*If version flag stays 0, then we know no version was found */ 

    int i = 0; 
    VcProp * tempProp = NULL;
    int beginFlag=0; 

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

	
	      // printf("buff!! = %s\n",buff);
        /* If we see another begin before an end, 
                                        return an ERROR */
	       if (buff==NULL) /* If bull is null, GET OUT */ 
	       {
	           //  if (*(cardp)==NULL)
	          //{    printf("ERROR\n\n\n");
             goto end;  
             // }
	       }
         printf("buff = %s\n",buff);
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
	  //    	printf("HER, buff =%s\n",buff);

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
          goto check;
       }
       /* If the buff contains version, check that its the proper version number */ 

       if (strstr(buff,"VERSION:")!=NULL)
       {
          versionFlag=1;
          if ((buff[8] != '3') ||  (buff[9] != '.') || (buff[10]!='0'))
          {
            newStatus.code = BADVER; 
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
            tempProp=malloc(sizeof(VcProp));
            error=parseVcProp(buff,tempProp);
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
	             // printf("buff = %s\n",buff);
               (*cardp)=realloc((*cardp),sizeof(Vcard)+(sizeof(VcProp)*(i+1)));
            }
            (*cardp)->prop[i]=*tempProp;
            i=i+1;
          //  free(buff);
        
	    if (newStatus.code == BEGEND)
            {
              goto end; 
            }
            
            if (buff=='\0')
            {
              break;
            }
            (*cardp)->nprops=i;

       }
       if (buff==NULL)
        break;
    } while (strcmp("END:VCARD",buff)!=0);
     
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
    if (buff!=NULL)
      free(buff);
    buff = NULL;
    return newStatus;

}



VcStatus getUnfolded ( FILE * const vcf, char **const buff )
{
    /*CRLF Flags */ 
    int rFlag=0; 
    int crlfFlag=0; 
    int i =0;
    static int endOfFile=0; 
    static char ch; /* static char to hold the last char read in for getUnfolded next call */ 
    static int staticFlag; /* this would be 1 its second call*/ 
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
      newStatus.lineto=0;
      newStatus.linefrom=0; 
      if (tempString!=NULL)
        free(tempString);
      return newStatus;  
    }
    /* Looping through char by char until a crlf is found, then a flag is set and 
      those chars are not read into the buff */ 
    do 
    {
      
      if (staticFlag!=1)
         ch = fgetc(vcf); /* Skips over a char */ 
        if (ch==EOF&&endOfFile==1)
	      {
	   //  printf("YES\n");
	       *buff = NULL;
	       return newStatus; 
	      }
	      endOfFile=1; /*We know that we dont have an empty file */ 
        switch (ch)
        {
	  
            case '\r':
            {
                rFlag = 1; 
                break;
            }
            case '\n':
            {
              if (rFlag==1)
              {
                   crlfFlag=1; 

		                if (foldedFlag==0)
                        newStatus.linefrom=newStatus.linefrom+1; /*Updating the line counters */

                   newStatus.lineto = newStatus.lineto+1; 
              }
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
                }
               else if (crlfFlag==0) 
                {
		               if (i==0)
		                   tempString=(char *)malloc(sizeof(char));
	                 else
			                 tempString=realloc(tempString,sizeof(char)*(i+1));
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
	      tempString[i]='\0';
        endOfFile=1; 
        if (strlen(tempString)<1)
        {
           *buff=NULL;
           return newStatus;
        }
        staticFlag=1;
    }
	
    
    else
        tempString[i]='\0';
    /* Allocating space and assigning buff */ 
   // printf("TEMP STRING = %s\n",tempString);
 //   printf("linefrom = %d, lineto = %d \n",newStatus.linefrom,newStatus.lineto);
    *buff = (char*)calloc(strlen(tempString)+2,sizeof(char));
    strncpy(*buff,tempString,strlen(tempString)+1); /* Maybe remove this + 1 */ 

    //f (strlen(tempString)>0 && tempString!=NULL)
       // free(tempString);

    newStatus.code = OK;
    
    lineCounter = newStatus.lineto; /*Updating the static line counter for getUnfoldes next call */ 
      // if (ch==EOF && endOfFile==1&& strlen(*buff)==0)
	// *buff=NULL;
	  
    return newStatus;

}


VcError parseVcProp ( const char * buff, VcProp * const propp)
{

    char * propName; 
    char * copyString2;
    VcError error=OK; 
    
    propp->value=NULL;
    propp->partype=NULL;
    propp->parval=NULL;
    propp->hook=NULL;
    char * tempString;
    char * typeString;
    char * value;
  
    
    /*Copying buff for use since its a const */ 
    tempString = (char*)calloc(strlen(buff)+1,sizeof(char));
    copyString2 = (char*)calloc(strlen(buff)+1,sizeof(char));

    strcpy(tempString,buff);
    //strcpy(copyString,buff);
    strcpy(copyString2,buff);
    /* If the line does not contain a  colon,
    we have an error */
    if (strstr(tempString,":")==NULL)
    {
	     error=SYNTAX;
	     return error;
    }

    /* If we find a colon before a semi colon, then we know there are no types */ 

    if (semiFirst(tempString)==0)
    {
       propName=strtok(tempString,":");
       value = strtok(NULL,"\n");
       propp->value = (char *)malloc((strlen(value)+1)*sizeof(char));
       strcpy(propp->value,value);
       assignPropName(propp,propName);
  
     }

    
    /* We know that there are no types, since parameter types
         only exist if a semi colon occurs before a colon */ 
    
   else if (semiFirst(tempString)==1) /* Semi colon occurs before colon, indicating optional parameters */ 
    {      
      propName=strtok(tempString,";"); /* Prop name found for optlionalse parameter case */ 
      typeString=strtok(NULL,"=");
     
      /* Type one found */ 
      typeString=strtok(NULL,":");
      if (typeString!=NULL)
      {
       if (strlen(typeString)>0)
         propp->partype=(char *)malloc((strlen(typeString)+1)*sizeof(char));
         strcpy(propp->partype,typeString);

      }
      value=strtok(NULL,"\n");
      if (value!=NULL)
      {
        propp->value = (char *)malloc((strlen(buff)+1)*sizeof(char));
        strncpy(propp->value,value,strlen(value)+1);
      }

      assignPropName(propp,propName);


    }
    return error;


    
}



void freeVcFile ( VcFile * const filep)
{
}
/* Assigning a property name. Returning the enumerated type value
of what was assigned */ 

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

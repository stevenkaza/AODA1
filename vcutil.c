#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vcutil.h"
#include "helper.h"

/*
 vcStatusReadVcFILE( FILE * const, vcf, VcFile * const file) ;

/* Precondition: vcf has been opened for reading,
 and is assumed to be positioned at beginning-of-life.
 filep  points to the VcFile structure to be filled in (its current contents is ignored) */

/* Post condition
   If status=OK, the file was read to normal EOF and is available in memory for access via the filep pointer. VcStatus line numbers give the total lines read from the file.  The caller is respnsibile for eventually
freeing the contents of filep  with the help of freeVcFile. If status is not OK, VcStatus contains both the error code and the line numbers in the file where the error was detectedm, and the VcFile contents (*cardp) has already been fired.             */

/*Create a vcard in readVcFile  */
/*Read vcard will know if the struct has been filled in */
VcStatus readVcFile (FILE *const vcf, VcFile *const filep)
{

        filep->ncards = 0;
        filep->cardp = NULL;
        VcStatus newStatus;


        if (vcf==NULL)
        {
                printf("file pointer NULL \n");
                newStatus.code =  IOERR;
        }

        newStatus = readVcard(vcf,filep->cardp);
        if (newStatus.code == OK)
            printf("File was parsed \n");
        else
            printf("Houston, we have a problem \n");

        return newStatus;


        /*Now we have to figure out what we do with each vcard and the flow of the program */

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
    char * buff;
    int state = 0; 
    
    if (vcf==NULL)
        newStatus.code = IOERR; 
     /*Checks for begin, and version  */ 
    while (feof(vcf)==0)
    {
        newStatus=getUnfolded(vcf,&buff);
        if (newStatus.code == OK)
        {
            /*Send the buff for parsing */
            buff[strlen(buff)]='\0';
            printf(" buff = %s \n",buff); 
            free(buff);
        }
        else if (newStatus.code == BEGEND)
            printf("Beggining not found \n");
        if (buff=='\0')
        {
            state =1; 
            printf("BROKEN\n");
            break;
        }
    }
    *buff=NULL;
    return newStatus;

}

VcStatus getUnfolded ( FILE * const vcf, char **const buff )
{
    /* For each line in the vcard, unfold */
    /*Its only returning the line that begins with the whitespace.... Why? */ 
    /* Should only assign to the buff once it has been unfolded */ 
    int rFlag=0; 
    int crlfFlag=0; 
    static int storedFlag; 
    static char ch;
    static char nextLine[75];
    static int staticFlag; 
    int foldedFlag=0;
    int foldedState;  /* 0 if line is unfolded ok, 1 if needs to be folded */ 
    char tempString[200];
    int i=0;
    int lineDoneFlag = 0; 
    VcStatus newStatus; 
    do 
    {
        if (staticFlag!=1)
         ch = fgetc(vcf);
        switch (ch)
        {
            case '\r':
            {
                rFlag = 1; 
                break;
            }
            /*Things */ 
            case '\n':
            {
                if (rFlag==1)
                    crlfFlag=1; 
                break;
            }
            /*Stuff */ 
            case ' ':
            {
                if (crlfFlag==1)
                    crlfFlag=0;
                break;
            }
            case '\t':
                 if (crlfFlag==1)
                    crlfFlag=0;
                break;
                /* Flags */ 
            default:
            {
                //if (foldedFlag==1) /*We gotta keep reading it untill we find a \r\n with no whitespace following */ 
                if (crlfFlag==1)
                {
                    staticFlag=1;
                    lineDoneFlag=1;
                    //tempString[i++]=ch;
                }
               else if (crlfFlag==0)
               {
                    tempString[i++] = ch; 
                    staticFlag=0; 
                }

                break;
            }
        }/*Switch Ends */ 

        if (lineDoneFlag==1)
            break;

    }while (ch!=EOF);
    if (ch==EOF)
        tempString[i]='\0';
    else
        tempString[i]='\0';
    *buff = (char*)calloc(strlen(tempString)+1,sizeof(char));
    strncpy(*buff,tempString,strlen(tempString)+1);
    newStatus.code = OK;
    return newStatus;

}



VcError parseVcProp ( const char * buff, VcProp * const propp)
{
    /* String Matching */ 
}

void freeVcFile ( VcFile * const filep)
{
}

/* int Contains(char * string, char pattern);
 * Returns: position where pattern was found 
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
    int i = 0; 
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

int isFolded(char * string)
{
    if ((string[0]==' ') || (string[0] == '\t'))
        return 1; 
    return 0; 
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
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
    
    if (vcf==NULL)
        newStatus.code = IOERR; 
     /*Checks for begin, and version  */ 
    while (buff!=NULL)
    {
        newStatus=getUnfolded(vcf,&buff);
        if (newStatus.code == OK)
        {
            /*Send the buff for parsing */
            printf("Parsing Buffer \n"); 
            return newStatus;
        }
        else if (newStatus.code == BEGEND)
            printf("Beggining not found \n");
            return newStatus;
    }

}

VcStatus getUnfolded ( FILE * const vcf, char **const buff )
{
    /* For each line in the vcard, unfold */
    /*Its only returning the line that begins with the whitespace.... Why? */ \.
    /* Should only assign to the buff once it has been unfolded */ 
    int position; 
    char lineAhead[30];
    VcStatus newStatus; 
    FILE * copy; 
    *buff = (char*)calloc(75,sizeof(char));
    while(fgets(*buff,75,vcf)!=NULL)
    {
      position=0;
      /*Checking to see if the line has a CR */ 
      position = Contains(*buff,'\r');
      /*CRLF CHECKING Part1: CR Check */
      while (position>0)
      {
        /*CRLF CHECKING Part 2: LF check */ 
        if (checkPosition(*buff,position)==1)
        {
            fgets(lineAhead,30,vcf);
            if (lineAhead[0]==' ')
            {
                removeSpaces(lineAhead);
                strncat(*buff,lineAhead,strlen(lineAhead));
                printf("%s\n",*buff);
                return newStatus;
            }
        }
        /*Scans untill a folded line is reached */ 
        /* If it does, return the folded line */ 
        /* Otherwise scan the entire file 
        / * Buffer must contain the files next unfolded line line with trailing EOL removed, Caller is responsible
        / * To free Buff. VcStatus is   OK and indicates the lines that were unfolded. and line no = 50 and = 100 read. Buff ignored in this case

        
      }
        /* Grabbing the next line and appending it to buff 
        fgets(lineAhead,100,vcf);
        strncat(*buff,lineAhead,strlen(lineAhead));
    */ 
    }
    *buff = NULL;
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
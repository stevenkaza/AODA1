#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vcutil.h"

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
        VcStatus newStatus = NULL;

        newStatus = malloc(sizeof(VcStatus));

        if (vcf==NULL)
        {
                printf("file pointer NULL \n");
                newStatus->code =  IOERR;
        }

        newStatus->code = readVcard(vcf,filep->card);

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
    char buff[1000];
    if (vcf==NULL)
        VcStatus->code = IOERR; 
     /*Checks for begin, and version  */ 

    getUnfolded(vcf,&buff);

}

VcStatus getUnfolded ( FILE * const vcf, char **const buff )
{
    /* For each line in the vcard, unfold */ 
    int position; 
    char lineAhead[100];
    while(fgets(buff,75,fp)!=NULL)
    {
      position = Contains(buff,'\r');
      /*CRLF CHECKING Part1: CR Check */

      while (position==0)
      {
        /*CRLF CHECKING Part 2: LF check */ 
        if (checkPosition(buff,position)==1)
        {
            /*This means the current line has the end in it */ 
            return buff;
        }
        position = Contains(buff,'\r');
        
        
      }
        /* Grabbing the next line and appending it to buff */ 
        fgets(lineAhead,100,fp);
        strncat(buff,lineAhead,strlen(lineAhead));

    }

    

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
    if (string[position+1] == '\n')
        return 1; 
    return 0; 
}

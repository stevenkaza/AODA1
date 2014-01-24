#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vcutil.h"
#include <ctype.h>
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
    Vcard * test;
    Vcard * test1;
  
    int i=0;
    if (vcf==NULL)
    {
        printf("file pointer NULL \n");
        newStatus.code =  IOERR;
    }
   while (feof(vcf)==0)
    { 
 
        /* Incrementing # of cards */ 
        filep->ncards=filep->ncards+1;
        newStatus.code = OK;
        filep->cardp=realloc(filep->cardp,(sizeof(Vcard*)*filep->ncards));

        if (newStatus.code==OK)
          newStatus = readVcard(vcf,&filep->cardp[i]);


        i++;

        if (newStatus.code!=OK)
         { 
           break;   
          }

   }/*end of while loop */ 

    

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


int checkStructs(VcFile      cardp)
{

}
VcStatus readVcard( FILE * const vcf, Vcard **const cardp)
{
    VcStatus newStatus; 
    char buff1[222];
    char  *buff;
    int nameFlag=0;
    int fnFlag = 0;
    int nFlag = 0;  
    int endFlag;
    VcError error; 

    int i = 0; 
    VcProp * tempProp = NULL;
    VcProp * proppp=NULL; 
    int beginFlag=0; 

    /* How do access the individual vcards in here */ 

    if (vcf==NULL)
    {

        newStatus.code = IOERR; 
        printf("cddfode = %d\n",newStatus.code );

    }
     /*Checks for begin, and version  */ 
    do
    {
       // printf("%s\n",buff );
        /*do this check outside? */ 
         while (feof(vcf)!=0)
         {
            goto check;
         }



        newStatus=getUnfolded(vcf,&buff);
        if (beginFlag==1)
        {
            if (strcmp("BEGIN:VCARD",buff)==0) /* If we see another begin before an end, 
                                        ERROR */
            {
                printf("wasdfasdfasdfow\n");
                newStatus.code =4; 
              //  newStatus.code= BEGEND;
                goto end;
            }
        }
        if (beginFlag==0)
        {
            if (strcmp("BEGIN:VCARD",buff)==0)
            {
                printf("BEGIN FOUND\n");
                beginFlag=1;
            }
            else
            {
              printf("BEGIN NOT FOUND\n");
                newStatus.code = BEGEND;

                return newStatus;
                goto end;
              //  goto end;
            }
        }
    /*    if ((beginFlag==1) && (strcmp("END:VCARD",buff)==0 ))
        {
            newStatus.code = OK;
            goto end;
        }*/
        /* Checking for FN / N */ 

        if (buff[0]=='F')
        {
          if (buff[1]=='N')
          {
          //  printf("%s\n", buff);
            fnFlag=1; 
          }
        }
        if (buff[0]=='N')
        {
          if (buff[1]==';' || buff[1] == ':')
             nFlag=1; 
        }
        
       /* if we reach the end of the vcard, go to the checks */ 
       if ((strcmp("END:VCARD",buff) ==0) && (beginFlag==1))
       {
        endFlag=1;
        goto check;
       }
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
              return newStatus;

            }
            /* If we are on the first prop, we simply alloc for 1*/ 
            if (i==0)
            {
              (*cardp)=malloc(sizeof(Vcard)+sizeof(VcProp));
	            (*cardp)->nprops=1;	
            } 
            else
              (*cardp)=realloc((*cardp),sizeof(Vcard)+(sizeof(VcProp)*(i+1)));

            (*cardp)->prop[i]=*tempProp;

            i=i+1;
            free(buff);
      //  }
         if (newStatus.code == BEGEND)
         {
            printf("Beggining not found \n");
            goto end; 
         }
      if (buff=='\0')
        {
            printf("BROKEN\n");
            break;
        }
       (*cardp)->nprops=i;

   }
 }while (strcmp("END:VCARD",buff)!=0);
      /* If we went through the entire vcard and couldnt find a FN, error */ 
    printf("fnFlag=%d\n",fnFlag );

    /* FLAG CHECKING FOR ERRORS */ 
    check:
    if (fnFlag ==0 || nFlag==0)
    {
      printf("fn not found\n");
      newStatus.code=7;
    }

    if (endFlag==0 || beginFlag==0)
    {
      printf("end not found\n");
      newStatus.code=4;
    }
      
    //*buff=NULL;
    end:
    printf("right before, code = %d\n",newStatus.code );
    return newStatus;

}

VcStatus getUnfolded ( FILE * const vcf, char **const buff )
{
    /* For each line in the vcard, unfold */
    /*Its only returning the line that begins with the whitespace.... Why? */ 
    /* Should only assign to the buff once it has been unfolded */ 
    int rFlag=0; 
    int crlfFlag=0; 
    static char ch;
    static int staticFlag; 
    char tempString[200];
    int i=0;
    int lineDoneFlag = 0; 
    static int lineCounter=0;
    VcStatus newStatus;
    newStatus.code = OK;  
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
              {
                    crlfFlag=1; 
                    lineCounter++;
              }
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
    newStatus.linefrom = lineCounter;
    end:
    return newStatus;

}


VcError parseVcProp ( const char * buff, VcProp * const propp)
{
      /* String Matching */ 
    // storage for 0-2 parameters (NULL if not present)
   // char partype[50];      // TYPE=string
//    char *parval;       // VALUE=string
    char * propName; 
    char * copyString;
    char * copyString2;
    int extraCheck;
    VcError error; 
    int returnVal; 

    char *value;        // property value string
    propp->value=NULL;
    propp->partype=NULL;
    char * tempString;
    char * typeString;
    /* If the line does not contain a semi colon,
    we have an error */
    if (Contains(buff,':')==0)
    {
      error=SYNTAX;
      return error;
    }
    tempString = (char*)calloc(strlen(buff)+1,sizeof(char));
    copyString2 = (char*)calloc(strlen(buff)+1,sizeof(char));

    strcpy(tempString,buff);
    //strcpy(copyString,buff);
    strcpy(copyString2,buff);

 /*   hasProps(tempString);
    trbName(tempString);
    setName(
    /* Iterate through tempString */ 

    /* If we find a colon before a semi colon, then we know there are no types */ 

    if (semiFirst(tempString)==0)
    {
      //  printf("come on\n");
       propName=strtok(tempString,":");
       value = strtok(NULL,"\n");
       propp->value = (char *)malloc((strlen(buff)+1)*sizeof(char));
       strcpy(propp->value,value);
    //   printf("%s\n",buff );
       assignPropName(propp,propName);
       

     }
      // printf("%s\n",buff);

    

    /* We know that there are no types, since parameter types
                           only exist if a semi colon occurs before a colon */ 
        /* Grab everything after the colon and store it in par value string */ 
	//printf("%s\n",buff);
    
   else if (semiFirst(tempString)==1) /* Semi colon occurs before colon, indicating optional parameters */ 
    {
   //   printf("String = %s\n",buff);

      propName=strtok(tempString,";"); /* Prop name found for optlionalse parameter case */ 
      typeString=strtok(NULL,"=");
      //printf("%s\n",typeString);
     
      /* Type one found */ 
      typeString=strtok(NULL,":");
   if (Contains(typeString,'='))  /* If there is more than one type or value */ 
      {
    //          printf("type = %s\n",typeString);
      //  char * type1;
        /* Check to see that = signs match ";" and then
        attempt to tokenize based on that */ 
     //   printf("type = %s\n",typeString);


        

      }
      if (strlen(typeString)>0)
        propp->partype=(char *)malloc((strlen(typeString)+1)*sizeof(char));
      strcpy(propp->partype,typeString);
      value=strtok(NULL,"\n");
      propp->value = (char *)malloc((strlen(buff)+1)*sizeof(char));
      strncpy(propp->value,value,strlen(value)+1);
      assignPropName(propp,propName);


    }


    
}



void freeVcFile ( VcFile * const filep)
{
}


int assignPropName(VcProp * const propp,char * propName, char  * const string)
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
    if (strcmp("ORG",propName)==0)
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

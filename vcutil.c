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
    Vcard * test;

    filep->cardp=malloc(sizeof(Vcard)+sizeof(VcProp));

    int i=1;
    if (vcf==NULL)
    {
        printf("file pointer NULL \n");
        newStatus.code =  IOERR;
    }
    while (feof(vcf)==0)
    {

            test = malloc(sizeof(Vcard)+sizeof(VcProp));

            filep->cardp[0] =test;
        filep->cardp[0]->nprops=14;
        printf("test= %d\n",filep->cardp[0]->nprops);

 
        /*for(i = 0; i < 100; i++)
         {
            filep->cardp[i] = NULL;
         }*/

       //  filep->cardp[0]=test; 

       //  filep->cardp[0]->nprops = 2; 


        newStatus = readVcard(vcf,filep->cardp);
        printf("NEW CARD %d\n",i);

        i++;

        if (newStatus.code!=OK)
            break;
        

    }

    if (newStatus.code == OK)
        printf("File was parsed \n");
    else
        printf("Houston, we have a problem \n");
    printf("%d\n",newStatus.code );

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
    char  buff[100];
   // int state = 0; 
    VcProp * prop=NULL; 
   // int endFlag=0; 
    int beginFlag=0; 

    cardp[0]->nprops=112;
    cardp[0]->prop[14].name=125;
    cardp[0]->prop[14].value=(char *)malloc((strlen(buff)+1)*sizeof(char));
      cardp[0]->prop[13].value=(char *)malloc((strlen(buff)+1)*sizeof(char));

    strcpy(cardp[0]->prop[14].value,"fuckme");
        strcpy(cardp[0]->prop[13].value,"fuckdfdfme");

    printf("INNERtest= %s\n",cardp[0]->prop[14].value);
        printf("INNERtest= %s\n",cardp[0]->prop[13].value);


   // cardp->ncards=2;

    /* How do access the individual vcards in here */ 

    if (vcf==NULL)
        newStatus.code = IOERR; 
     /*Checks for begin, and version  */ 
    do
    {
       // printf("%s\n",buff );
        while (feof(vcf)!=0)
         {
            newStatus.code = IOERR; 
            return newStatus;
        }

      if (beginFlag==1)
        {
            if (strcmp("BEGIN:VCARD",buff)==0) /* If we see another begin before an end, 
                                        ERROR */
            {
                newStatus.code= BEGEND;
                goto end;
            }
        }
    // printf("bflag=%d\n",beginFlag);
        newStatus=getUnfolded(vcf,&buff);
      //  printf("%s\n", buff);
        if (beginFlag==0)
        {

            if (strcmp("BEGIN:VCARD",buff)==0)
            {
                beginFlag=1;

            }
            else
            {
                newStatus.code = BEGEND;
                goto end;
            }
        }
        if ((beginFlag==1) && (strcmp("END:VCARD",buff)==0 ))
        {
            newStatus.code = OK;
            goto end;
        }
       

       // printf("%d\n",newStatus.linefrom );
      if (newStatus.code == OK)
        {
            /*Send the buff for parsing */
        
            buff[strlen(buff)]='\0';
            prop=malloc(sizeof(VcProp));
            parseVcProp(buff,prop);

        
         //   printf(" buff = %s \n",buff); 
            free(buff);
        }
        else if (newStatus.code == BEGEND)
            printf("Beggining not found \n");
      if (buff=='\0')
        {
          //  state =1; 
            printf("BROKEN\n");
                        break;

        }
    }while (strcmp("END:VCARD",buff)!=0);
    
    //*buff=NULL;
    end:
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

  //  int colonFlag = 0; 
    //int semiFlag = 0;       
    char *value;        // property value string
   // void *hook;
   // int typeIndex = 0; 
  //  char * string; 
   // char * firstHalf; 
   // int typeFlag=0; 
    propp->value=NULL;
    propp->partype=NULL;
    char * tempString;
    char * typeString;

    tempString = (char*)calloc(strlen(buff)+1,sizeof(char));
    copyString = (char*)calloc(strlen(buff)+1,sizeof(char));
    copyString2 = (char*)calloc(strlen(buff)+1,sizeof(char));

    strcpy(tempString,buff);
    strcpy(copyString,buff);
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

 //   printf("\n");
    printf("String = %s\n",buff);
    printf("prop name =%d\n",propp->name);
    printf("value=%s\n",propp->value);
    printf("par type=%s\n",propp->partype);
    free(propp->partype);
    
   // printf("\n");
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

#include <stdlib.h>
#include "vcutil.h"
#include "vcftool.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
int main(int argc, char * argv[])
{

	VcStatus newStatus; 
	VcFile * filep=NULL;
	FILE * fp; 
	filep = malloc(sizeof(VcFile));
	newStatus = readVcFile(stdin,filep);
	printf("First Name: %s\n",filep->cardp[0]->prop[0].value);
//	newStatus=writeVcFile(stdout,filep);

	if (argv[1]!=NULL)
	{
		if (strcmp(argv[1],"-info")==0)
		{
			vcfInfo(stdout,filep);
		}
		if (strcmp(argv[1],"-sort")==0)
		{
			vcfSort(filep);
		}
    }
}

int vcfInfo( FILE *const outfile, const VcFile *filep )
{
	int i = 0; /* Index for going through cards */ 
	int k = 0; /* Index for going through props */ 

	int photoCounter = 0; 
	int geoCounter = 0; 
	int urlCounter = 0; 
	
	/* if 1, photo has been found in card */ 
	int photoFound = 0; 
	int urlFound = 0; 
	int geoFound = 0; 
	

	if (outfile==NULL)
	{
		printf("outfile is null \n");
		return 1; 
	}

	if (filep==NULL)
	{
		printf("filep is null \n");
		return 1; 
	}

	/* Adjust this to accurately display sorted cards later */ 
	if (isSorted(filep)==0)
		fprintf(outfile,"%d cards (not sorted)\n",filep->ncards);
	else if (isSorted(filep)==1)
		fprintf(outfile,"%d cards (sorted)\n",filep->ncards);
 	for(i = 0; i < filep->ncards; i++)
        {
    	/* Reseting the flags for each card */ 
    	geoFound = 0; 
    	photoFound = 0; 
    	urlFound = 0; 
        for(k = 0; k < filep->cardp[i]->nprops; k++)
        {
        	if (filep->cardp[i]->prop[k].name==VCP_PHOTO && photoFound == 0)
        	{
        		photoCounter++;
        		photoFound = 1; 
        	}
             if (filep->cardp[i]->prop[k].name==VCP_GEO && geoFound ==0)
            {
        		geoCounter++;
        		geoFound = 1; 
        	}
         if (filep->cardp[i]->prop[k].name==VCP_URL && urlFound ==0)
        	{
			printf("url = %s\n",filep->cardp[i]->prop[k].value);
        		urlCounter++;
        		urlFound=1; 
        	}
        	
        }

     }

     fprintf(outfile,"%d with photos\n",photoCounter);
     fprintf(outfile, "%d with URLs\n",urlCounter);
     fprintf(outfile,"%d with geographic coordinates\n",geoCounter);



}

/* Returns a 1 if the cards are  sorted*/ 
int isSorted(VcFile * const filep)
{
	int result;
	int i = 0; 
	int k = 0;
	char * firstName1; 
	char * firstName2; 
	/* Strings to store the value copies of name */ 
	char * nameValue1=NULL; 
	char * nameValue2=NULL; 
	char * nameValue1Copy=NULL; 
	char * nameValue2Copy=NULL;
	/* Tokens for getting the last name between the ; */ 
	char * lastName1; 
	char * lastName2;  
	Vcard * card1; 
	Vcard * card2;
	/* Holds value of K , index where name was found in array of props */ 
	int nCard1 = 0; 
	int nCard2 = 0; 
	for (i=0; i<filep->ncards;i++)
	{
	    nameValue1=NULL;
		/* Comparing two cards at a time. First we find the name prop of card1, 
			and then of card 2 (the one ahead) */
		for(k=0;k<filep->cardp[i]->nprops;k++)
		{
			if (filep->cardp[i]->prop[k].name == VCP_N)
			{
				/* we have found the prop with the name element for this card */ 
				nCard1= k; 
				break; 
			}

		} 
	
		/* Grabbing prop for card2*/ 
		if (i<(filep->ncards-1))
		{
			for (k=0;k<filep->cardp[i+1]->nprops;k++)
			{
				if (filep->cardp[i+1]->prop[k].name==VCP_N)
				{
					nCard2=k; /* Found card2s name property */  
					break; 

				}

			}
	        
		/* copying the first value so its able to be strtoked without destryoing filep */
//		printf("==1 %s\n",filep->cardp[i]->prop[nCard1].value); 
//		printf("==2 %s\n",filep->cardp[i+1]->prop[nCard2].value); 

		nameValue1 = (char*)calloc(strlen(filep->cardp[i]->prop[nCard1].value)+1,sizeof(char));
		strcpy(nameValue1,filep->cardp[i]->prop[nCard1].value); 
//		printf("length = %d %d\n",strlen(filep->cardp[i+1]->prop[nCard2].value),strlen(filep->cardp[i]->prop[nCard1].value));		
		nameValue2 = (char*)calloc(strlen(filep->cardp[i+1]->prop[nCard2].value)+1,sizeof(char));
	//	nameValue2 = malloc(sizeof(strlen(filep->cardp[i+1]->prop[nCard2].value))); 
		strcpy(nameValue2,filep->cardp[i+1]->prop[nCard2].value); 
		lastName1 = strtok(nameValue1,";"); 
		lastName2 = strtok(nameValue2,";"); 
		result = strcmp(nameValue1,nameValue2); 
		printf("result = %d\n",result); 
		/* If result == -1, the first string was greater than the second card string,
		 * indicating that it is sorted. Time to check the next card and increment i*/ 
		if (result==-1)
		{
				if (nameValue1!=NULL)
					free(nameValue1); 
				if (nameValue2!=NULL)
					free(nameValue2);
				continue;
	
		}
		/* if result == 1, the cards are not sorted */ 
		else if (result==1)
		{
				if (nameValue1!=NULL)
					free(nameValue1); 
				if (nameValue2!=NULL)
					free(nameValue2);
			return 0; /* Return 0 indicating they are not sorted*/ 

		}
		/* The last names happened to be the same between the cards. Time to check first name */ 
		else if (result==0)
		{
			/* Copying the strings before strtoking as strtok causes headaches */ 
			nameValue1Copy=malloc(sizeof(strlen(nameValue1)));
			nameValue2Copy=malloc(sizeof(strlen(nameValue2)));
			strcpy(nameValue1Copy,nameValue1); 
			strcpy(nameValue2Copy,nameValue2);
			firstName1 = strtok(nameValue1Copy,"\n"); 
			firstName2 = strtok(nameValue2Copy,"\n"); 
			result = strcmp(firstName1,firstName2); 
			if (result==-1)
			{
				if (nameValue1!=NULL)
					free(nameValue1); 
				if (nameValue2!=NULL)
					free(nameValue2);
				continue; 
			}
			else if (result==1)
			{
					if (nameValue1!=NULL)
						free(nameValue1); 
					if (nameValue2!=NULL)
						free(nameValue2);
				return 0; /* Indicates not sorted */ 

			}

			else if (result==0) 
			{
				  printf("firstname 1 = %s, firstname 2 = %s\n",firstName1,firstName2 );
			     // printf("STRING = %s %s\n",firstName1,firstName2);
			     // printf("nameValue1 = %s nv2 = %s \n",nameValue1,nameValue2); 
			      if (nameValue1!=NULL)
			      {
				free(nameValue1); 
				nameValue1=NULL;
			      }
			      if (nameValue2!=NULL)
			      {
					free(nameValue2);
			        nameValue2=NULL;
			      }
			      continue; /*They are equal, continue to the next pair of cards to compare */
			} 
		}

		/* Time to compare them */ 
		//strcmp goes here. if strcmp(card1,card2) == -1, then we know sorted. 
	     }

	    
	}
	

	if (nameValue1!=NULL)
		free(nameValue1); 
	if (nameValue2!=NULL)
		free(nameValue2);
	return 1; 
}

int cmpare(const void  * card1,const void * card2)
{
	printf("welcome to compare \n"); 
	int result; 
	int i = 0; 
	int k = 0;
	char * firstName1; 
	char * firstName2; 
	/* Strings to store the value copies of name */ 
	char * nameValue1=NULL; 
	char * nameValue2=NULL; 
	char * nameValue1Copy=NULL; 
	char * nameValue2Copy=NULL;
	/* Tokens for getting the last name between the ; */ 
	char * lastName1; 
	char * lastName2;  
	/* Holds value of K , index where name was found in array of props */ 
	int nCard1 = 0; 
	int nCard2 = 0; 
	
	    nameValue1=NULL;
		/* Comparing two cards at a time. First we find the name prop of card1, 
		and then of card 2 (the one ahead) */
	for(k=0;k<(*(Vcard **)card1)->nprops;k++)
	{
		if ((*(Vcard **)card1)->prop[k].name == VCP_N)
		{
			/* we have found the prop with the name element for this card */ 
			nCard1= k; 
			break; 
		}

	} 

	/* Grabbing prop for card2*/ 
	
		for (k=0;k<(*(Vcard **)card2)->nprops;k++)
		{
			if ((*(Vcard **)card2)->prop[k].name==VCP_N)
			{
				nCard2=k; /* Found card2s name property */  
				break; 
			}
		}
        
	/* copying the first value so its able to be strtoked without destryoing filep */

	nameValue1 = (char*)calloc(strlen((*(Vcard **)card1)->prop[nCard1].value)+1,sizeof(char));
	strcpy(nameValue1,(*(Vcard **)card1)->prop[nCard1].value); 
//		printf("length = %d %d\n",strlen(card2->prop[nCard2].value),strlen(card1->prop[nCard1].value));		
	nameValue2 = (char*)calloc(strlen((*(Vcard **)card2)->prop[nCard2].value)+1,sizeof(char));
//	nameValue2 = malloc(sizeof(strlen(card2->prop[nCard2].value))); 
	strcpy(nameValue2,(*(Vcard **)card2)->prop[nCard2].value); 
	lastName1 = strtok(nameValue1,";"); 
	lastName2 = strtok(nameValue2,";"); 
	printf("nv1 %s nv2 %s\n",nameValue1,nameValue2); 
	result = strcasecmp(nameValue1,nameValue2); 
	printf("result = %d\n",result); 
	if (result!=0)
	{
		if (nameValue1!=NULL)
			free(nameValue1); 
		if (nameValue2!=NULL)
			free(nameValue2);
		return (strcasecmp(nameValue1,nameValue2)); 
	}
	/* If result == -1, the first string was greater than the second card string,
	 * indicating that it is sorted. Time to check the next card and increment i*/ 
	if (result==-1)
	{
			if (nameValue1!=NULL)
				free(nameValue1); 
			if (nameValue2!=NULL)
				free(nameValue2);
			return -1; 

	}
	/* if result == 1, the cards are not sorted */ 
	else if (result==1)
	{
			if (nameValue1!=NULL)
				free(nameValue1); 
			if (nameValue2!=NULL)
				free(nameValue2);
		return 1; /* Return 0 indicating they are not sorted*/ 

	}
	/* The last names happened to be the same between the cards. Time to check first name */ 
	else if (result==0)
	{
		return 0; 
		/* Copying the strings before strtoking as strtok causes headaches */ 
		nameValue1Copy=malloc(sizeof(strlen(nameValue1)));
		nameValue2Copy=malloc(sizeof(strlen(nameValue2)));
		strcpy(nameValue1Copy,nameValue1); 
		strcpy(nameValue2Copy,nameValue2);
		firstName1 = strtok(nameValue1Copy,"\n"); 
		firstName2 = strtok(nameValue2Copy,"\n"); 
		result = strcmp(firstName1,firstName2); 
		if (result==-1)
		{
			if (nameValue1!=NULL)
				free(nameValue1); 
			if (nameValue2!=NULL)
				free(nameValue2);
			return -1; 
		}
		else if (result==1)
		{
				if (nameValue1!=NULL)
					free(nameValue1); 
				if (nameValue2!=NULL)
					free(nameValue2);
			return 1; /* Indicates not sorted */ 

		}

		else if (result==0) 
		{
		     // printf("STRING = %s %s\n",firstName1,firstName2);
		     // printf("nameValue1 = %s nv2 = %s \n",nameValue1,nameValue2); 
		      if (nameValue1!=NULL)
		      {
			free(nameValue1); 
			nameValue1=NULL;
		      }
		      if (nameValue2!=NULL)
		      {
			free(nameValue2);
		        nameValue2=NULL;
		      }
		return 0; 	
	} 
	}

		/* Time to compare them */ 
		//strcmp goes here. if strcmp(card1,card2) == -1, then we know sorted. 
	     

	    
	
	

	if (nameValue1!=NULL)
		free(nameValue1); 
	if (nameValue2!=NULL)
		free(nameValue2);
	return 1; 

}
int vcfSort(VcFile * const filep)
{
	int k = 0; 
	int i = 0; 
		qsort(filep->cardp,filep->ncards,sizeof(Vcard *),cmpare);

		writeVcFile(stdout,filep); 
	
}



int vcfSelect( VcFile *const filep, const char *which);


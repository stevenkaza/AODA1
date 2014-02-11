#include <stdlib.h>
#include "vcutil.h"
#include "vcftool.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
int main(int argc, char * argv[])
{
	int ww; 
	VcStatus newStatus; 
	VcFile * filep=NULL;
	FILE * fp; 
	if (stdin==NULL)
	{
		fprintf(stderr, "%s\n", "No file specified");
		return 1; 
	}

	filep = malloc(sizeof(VcFile));
	newStatus = readVcFile(stdin,filep);
//	newStatus=writeVcFile(stdout,filep);

	if (argv[1]!=NULL)
	{
		if (strcasecmp(argv[1],"-select")==0)
		{
			if (strlen(argv[2])>3 || strlen(argv[2])<1)
			{
				fprintf(stderr,"Invalid arguement size for -select\n");
				return 1; 
			}
			

			if (strspn(argv[2],"pug")==0)
			{
				fprintf(stderr,"Invalid arguement for -select. Must contain at least a p, u, g\n");
				return 1; 
			}
			vcfSelect(filep,argv[2]);
		}
		if (strcmp(argv[1],"-info")==0)
		{
			vcfInfo(stdout,filep);
		}
		if (strcasecmp(argv[1],"-sort")==0)
		{
			vcfSort(filep);
		    writeVcFile(stdout,filep); 
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
		fprintf(stderr,"outfile is null \n");
		return 1; 
	}

	if (filep==NULL)
	{
		fprintf(stderr,"filep is null \n");
		return 1; 
	}

	/* Adjust this to accurately display sorted cards later */ 
	if (isSorted(filep)==1)
		fprintf(outfile,"%d cards (not sorted)\n",filep->ncards);
	else if (isSorted(filep)==0)
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
        		urlCounter++;
        		urlFound=1; 
        	}
        	
        }

     }

     fprintf(outfile,"%d with photos\n",photoCounter);
     fprintf(outfile, "%d with URLs\n",urlCounter);
     fprintf(outfile,"%d with geographic coordinates\n",geoCounter);



}

/* Returns a 1 if the cards are not sorted
	indexes through all cards*/ 
int isSorted(const VcFile *  filep)
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
		/* If result == -1, the first string was greater than the second card string,
		 * indicating that it is sorted. Time to check the next card and increment i*/ 
	    result = strcasecmp(lastName1,lastName2); 
	    if (result<0)
	    {
			if (nameValue1!=NULL)
				free(nameValue1); 
			if (nameValue2!=NULL)
				free(nameValue2);
			continue;
	    }

		if (result>0)
		{
				if (nameValue1!=NULL)
					free(nameValue1); 
				if (nameValue2!=NULL)
					free(nameValue2);
				return 1; 
	
		}
		/* if result == 1, the cards are not sorted */ 
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
				return ; /* Indicates not sorted */ 

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
			      continue; /*They are equal, continue to the next pair of cards to compare */
			} 
		}

		/* Time to compare them */ 
		//strcmp goes here. if strcmp(card1,card2) == -1, then we know sorted. 
	     }

	    
	}
	
	if (nameValue1!=NULL)
		free(nameValue1); 
	//if (nameValue2!=NULL)
	//	free(nameValue2);
	return 0; 
}

int cmpare( void  * card1, void * card2)
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

	result = strcasecmp(lastName1,lastName2); 
	if (result!=0)
	{
		//if (nameValue1!=NULL)
		//	free(nameValue1); 
	//	if (nameValue2!=NULL)
	//		free(nameValue2);
		return (strcasecmp(lastName1,lastName2)); 
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
	//return 1; 
//	return 1; 

}
int vcfSort(VcFile * const filep)
{
	int k = 0; 
	int i = 0; 
		qsort(filep->cardp,filep->ncards,sizeof(Vcard *),cmpare);

	
}

/* returns card # where proprety was found, -1 if not found */ 
/* returns an array containing the card #'s that contain the prpoerty */
int findCard(VcFile * const filep,char property,int array[])
{
	int i = 0; 
	int k = 0; 
	int geoCounter=0;
	int photoCounter=0; 
	int urlCounter=0;  
   if (property=='g')
	{
	for(i = 0; i < filep->ncards; i++)
	{
		for(k = 0; k < filep->cardp[i]->nprops; k++)
        {        
             if (filep->cardp[i]->prop[k].name==VCP_GEO )
            {
            	array[geoCounter++]=i;
        	}        
        }
      }
  }

    else if (property=='u')
    {
	   
	for(i = 0; i < filep->ncards; i++)
	{
	    for(k = 0; k < filep->cardp[i]->nprops; k++)
	        {        
	             if (filep->cardp[i]->prop[k].name==VCP_URL )
	            {
	            	array[urlCounter++]=i;
	        	}
	        	
	        }
     }
   }

    else if (property=='p')
    {
    	for(i = 0; i < filep->ncards; i++)
	{
    	for(k = 0; k < filep->cardp[i]->nprops; k++)
	        {        
	          if (filep->cardp[i]->prop[k].name==VCP_PHOTO)
        	 {
        		array[photoCounter++]=i; 
        	 }	        	
	        }
		}
    }




	}

int vcfSelect( VcFile *const filep, const char *which)
{
	fprintf(stdout,"welcome to select\n");
	int k = 0; 
	/* Property flag */ 
	int i = 0; 
	int j = 0; 
	Vcard * tmp = NULL;
	int * matchedArray=NULL; 
	/* if one, means to look for them */ 
	int findGeo=0; 
	int findPhoto = 0; 
	int findURL = 0; 

	/* if one, means they were found */ 
	int photoFound=0; 
	int urlFound = 0; 
	int geoFound = 0; 
	int matchedCounter = 0; 
	int * geoArray=NULL; 
	int * photoArray=NULL; 
	int * urlArray=NULL; 
	/* Determining which properties to look for */ 
	/* creating an array if needed to hold location
	of cards where speicifc letter was found */ 
	for (k=0;k<strlen(which);k++)
	{
		printf("fp = %d\n",filep->ncards);
		if (which[k]=='g')
		{
			findGeo = 1;  	
		}
		if (which[k]=='u')
		{
			findURL = 1; 

		}
		if (which[k]=='p')
		{
			findPhoto = 1; 

		}

	}


    for (i=0; i<filep->ncards;i++)
    {
    	photoFound=0;
    	geoFound=0;
    	urlFound=0; 
    	for(k = 0; k < filep->cardp[i]->nprops; k++)
    	{


      		  	if (filep->cardp[i]->prop[k].name==VCP_PHOTO)
      		  		photoFound=1;
      		  	
 				if (filep->cardp[i]->prop[k].name==VCP_URL)
      		  		urlFound=1;
      		  	
  				if (filep->cardp[i]->prop[k].name==VCP_GEO)
      		  		geoFound=1;	     		  	    		  	
    	}/* end of card */ 

    	if (findGeo==1 && findPhoto==1 && findURL==1) /* CASE ONE SEARCH FOR ALL 3*/ 
    	{
    		if (urlFound==1 && photoFound==1 && geoFound==1)/* ALL 3 WERE FOUND */ 
    		{
    			/* do nothing */ 
    			//matchedArray[matchedCounter++]=i; /*Storing the position card to be kept as i */

    		}
    		else /* Must free the useless card */ 
    		{
    			freeVcard(filep,i);
    		}
    	}

    	else if (findGeo==1 && findPhoto==0 && findURL==1) /* CASE TWO SEARCH FOR JUST 2 geo/url*/ 
    	{
			if (urlFound==1 && geoFound==1)/* 2 WERE FOUND */ 
    		{
    			/* do nothing */ 

    		}
    		else /* Must free the useless card */ 
    		{
    			freeVcard(filep,i);
    		}

    	}

    	else if (findGeo==1 && findPhoto==1 && findURL==0) /* CASE THREE SEARCH FOR JUST 2 geo/photo*/ 
    	{
    		if (geoFound==1 && photoFound==1)
    		{
    			/* Do nothing */ 
    		}
    		else
    		{
    			freeVcard(filep,i);
    		}
    	}


    	else if (findGeo==0 && findPhoto==1 && findURL==1) /* CASE 4 SEARCH FOR JUST 2 url/photo*/ 
    	{
    		if (photoFound==1 && urlFound==1)
    		{
    			/* Do nothing */ 
    		}
    		else
    		{
    			freeVcard(filep,i);
    		}
    	}

    	else if (findGeo==1 && findPhoto==0 && findURL==0) /* CASE FIVE JUST GEO */ 
    	{
    		if (geoFound==1)
    		{
    			/* Do nothing */ 
    		}
    		else
    		{
    			/* no geo was found. Free that card */ 
    			freeVcard(filep,i);
    		}



    	}

       else if (findGeo==0 && findPhoto==1 && findURL==0) /* CASE SIX JUST photo */ 
       {
			if (photoFound==1)
			{

			}
			else
			{
				freeVcard(filep,i);
			}
	   }

       else if (findGeo==0 && findPhoto==0 && findURL==1) /* CASE SIX JUST URL */ 
	   {
	   	if (urlFound==1)
	   	{

	   	}
	   	else
	   	{
	   		freeVcard(filep,i);
	   	}

	   }
    }
    if (geoFound==0 && urlFound==0 && photoFound==0)
    {
    	fprintf(stderr,"No cards selected");
		 	return 1; 
	}

	for (i=0;i<filep->ncards;i++)
	{
		if (filep->cardp[i]!=NULL)
		{
			printf("i=%d\n",i );
		}
	}


} 



/*

	for(k = 0; k < filep->cardp[i]->nprops; k++)
        {

        	if (filep->cardp[i]->prop[k].name==VCP_PHOTO &&  == 0)
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
        		urlCounter++;
        		urlFound=1; 
        	}
        	
        }

*/


int freeVcard(const VcFile * filep, int i)
{
	int j = 0; 
    Vcard * tmp;
    VcProp * tmp2;
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
        filep->cardp[i]==NULL;
}
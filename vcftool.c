#include <stdlib.h>
#include "vcutil.h"
#include "vcftool.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char * argv[])
{

	VcStatus newStatus; 
	VcFile * filep=NULL;
	filep = malloc(sizeof(VcFile));
	newStatus = readVcFile(stdin,filep);
	writeVcFile(stdout,filep);

	if (newStatus.code!=OK)
	{
		printf("%d error code detected. read lines %d to %d exiting \n", newStatus.code,newStatus.linefrom,newStatus.lineto);
	}
	if (argv[1]!=NULL)
	{
		if (strcmp(argv[1],"-info")==0)
		{
			vcfInfo(stdout,filep);
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
	fprintf(outfile,"%d cards (not sorted)\n",filep->ncards);

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
             else if (filep->cardp[i]->prop[k].name==VCP_GEO && geoFound ==0)
            {
        		geoCounter++;
        		geoFound = 1; 
        	}
        	else if (filep->cardp[i]->prop[k].name==VCP_URL && urlFound ==0)
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

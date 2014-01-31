#include <stdio.h>
#include <stdlib.h>
#include "vcutil.h"
int main()
{
//
	FILE * vcf = fopen("samples-4.vcf","r"); 
	VcStatus newStatus; 
	int wow; 
	if (vcf==NULL)
	    return 1; 
	
	
     VcFile * filep = NULL;
	 filep = malloc(sizeof(VcFile));	


	/* for(i = 0; i < 100; i++)
	 {
	 	filep->cardp[i] = NULL;
	 }
	 */
	 newStatus = readVcFile(vcf,filep);
        if (newStatus.code == OK)
        {	
	}


      //freeVcFile(filep);
	 if (newStatus.code == OK)
	 	printf("Success \n");
	 else
	 	printf("No way. Problem code = %d \n",newStatus.code);

	return 0; 

}

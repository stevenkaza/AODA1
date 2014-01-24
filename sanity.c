#include <stdio.h>
#include <stdlib.h>
#include "vcutil.h"
int main()
{

	FILE * vcf = fopen("samples-10.vcf","r"); 
	VcStatus newStatus; 
	int wow; 
	if (vcf==NULL)
	    return 1; 
	
	
     VcFile * filep = NULL;
	 filep = malloc(sizeof(VcFile));	

	 int i=0;
	int k=0;

	/* for(i = 0; i < 100; i++)
	 {
	 	filep->cardp[i] = NULL;
	 }
	 */
	 newStatus = readVcFile(vcf,filep);
	 		 //printf("please = %s\n",filep->cardp[0]->prop[14].value);
 
               if (newStatus.code == OK)
     {
		for (i=0;i <(filep->ncards);i++)
		{
                       
			for(k=0;  k<(filep->cardp[i]->nprops);k++)
				printf("val=%s\n" , filep->cardp[i]->prop[k].value);


		}

}
	 if (newStatus.code == OK)
	 	printf("Success \n");
	 else
	 	printf("No way. Problem code = %d \n",newStatus.code);

	return 0; 

}

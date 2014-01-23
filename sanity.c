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

	 int i;

	/* for(i = 0; i < 100; i++)
	 {
	 	filep->cardp[i] = NULL;
	 }
	 */
	 newStatus = readVcFile(vcf,filep);
	 		 //printf("please = %s\n",filep->cardp[0]->prop[14].value);

		 printf("please = %s\n",filep->cardp[5]->prop[2].value);


	 if (newStatus.code == OK)
	 	printf("Success \n");
	 else
	 	printf("No way. Problem \n");

	return 0; 

}

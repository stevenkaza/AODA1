#include <stdio.h>
#include <stdlib.h>
#include "vcutil.h"
int main()
{

	FILE * vcf = fopen("samples-10.vcf","r"); 
	VcStatus newStatus; 
	if (vcf==NULL)
	    return 1; 
	
	
         VcFile * filep = NULL;
	 filep = malloc(sizeof(VcFile)); 
	 
	 newStatus = readVcFile(vcf,filep);
	 if (newStatus.code == OK)
	 	printf("Success \n");
	 else
	 	printf("No way. Problem \n");

	return 0; 

}

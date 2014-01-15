#include <stdio.h>
#include <stdlib.h>
#include "vcutil.h"
int main()
{

	FILE * vcf = fopen("vCard.vcf","r"); 
	if (vcf==NULL)
	    return 1; 
	
	
         VcFile * filep = NULL;
	 filep = malloc(sizeof(VcFile)); 
	 
	 readVcFile(vcf,filep);



}
